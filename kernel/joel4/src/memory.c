////////////////////////////////////////////////////////////////////////////////
/// @file   memory.c
/// @short  kernel memory manager
/// @author J.Nider
/// @date   30/10/2009
////////////////////////////////////////////////////////////////////////////////

#include "memory.h"
#include "isr.h"     /* regs_t */

// flags in CR0 control register
#define CR0_PG 0x80000000  // 1=paging enabled

// CR4 register
#define CR4_PSE (1<<4)     // 1=Page Size Extensions enabled
#define CR4_PAE (1<<5)     // 1=Physical Address Extensions enabled

// page fault codes
#define PAGE_FAULT_PRESENCE      0x0001
#define PAGE_FAULT_WRITE     0x0002      // write access violation
#define _PF_CODE_US     0x0004
#define _PF_CODE_RSVD   0x0008
#define _PF_CODE_ID     0x0010

#define BUILD_LOGICAL(_dirIndex, _tableIndex) ((_dirIndex << 22) | (_tableIndex << 12))
#define GET_DIR_INDEX(_addr)        (_addr >> 22)
#define GET_PAGE_INDEX(_addr)       ((_addr & 0x3FF000) >> 12)
#define GET_DISK_LOCATION(_addr)    (_addr & 0xFFFFF000)

#define GET_DIR_INDEX(_addr)        (_addr >> 22)
#define GET_DISK_4M_LOCATION(_addr) (_addr & 0xFFC00000)

#define PAGE_DIRECTORY_MAX_ENTRIES  1024
#define PAGE_TABLE_MAX_ENTRIES      1024

extern int _globalFreeSpace;
static void k_unmap4KPage(unsigned int* pPageDir, unsigned int logical);
static unsigned char kernelFreePageMap[(KERNEL_MEMORY_LIMIT/PAGE_SIZE)/sizeof(char)];
static void* freePageStore;
static void* nextFreePage;
static void* kernelPageDir;
static void* globalFreeSpace = &_globalFreeSpace;
void _fh_page_fault(regs_t* pRegs);

////////////////////////////////////////////////////////////////////////////////
/// @date   03/09/2008
/// @short  Map the logical address to the physical address - only in real mode
/// @param  pPageDirectory
/// @param  physical
/// @param  logical
///
////////////////////////////////////////////////////////////////////////////////

/*
Linus says:
* "Volatile isn't enough to prevent the compiler from reordering the
* read/write functions for the control registers and messing everything up.
* A memory clobber would solve the problem, but would prevent reordering of
* all loads stores around it, which can hurt performance. Solution is to
* use a variable and mimic reads and writes to it to enforce serialization"
* I belive this is called register shadowing.
*/
static unsigned long __force_order;

__inline void k_setPageDirectory(void* pageDir)
{
   //asm __volatile__ ("movl %0, %%cr3" :: "dN" (pPageDir));
   asm __volatile__ ("movl %0, %%cr3" :: "r" (pageDir), "m" (__force_order));
   //asm volatile("mov %0,%%cr3": :"r" (val), "m" (__force_order));
}

__inline void* k_getPageDirectory(void)
{
   void* rv;
   asm __volatile__ ("movl %%cr3, %0": "=r" (rv), "=m" (__force_order));
   return rv;
}

__inline int k_isTablePresent(unsigned int* pLogicalDir, unsigned int tableIndex)
{
   return (pLogicalDir[tableIndex]) & MEMORY_PAGE_TABLE_PRESENT;
}

__inline unsigned int readCR0(void)
{
   unsigned int rv = 0;
   asm __volatile__ ("movl %%cr0, %0": "=a" (rv));
   return rv;
}

__inline void writeCR0(unsigned int val)
{
   asm __volatile__ ("movl %0, %%cr0" :: "dN" (val));
}

__inline unsigned int readCR2(void)
{
   unsigned int rv = 0;
   asm __volatile__ ("movl %%cr2, %0": "=a" (rv));
   return rv;
}

__inline void writeCR4(unsigned int val)
{
   asm __volatile__ ("movl %0, %%cr4" :: "dN" (val));
}

__inline unsigned int readCR4(void)
{
   unsigned int rv = 0;
   asm __volatile__ ("movl %%cr4, %0": "=a" (rv));
   return rv;
}

int k_isPageFree(void* address)
{
   // which page is this address in?
   unsigned int page = (unsigned int)address >> 12; // 4096 bytes in a page

   // which byte is used for this page?
   unsigned int index = page >> 3; // 8 pages represented in a byte

   // which bit in the byte?
   unsigned int mask = 1 << (page & 0x07);

   //k_printf("page: 0x%x index: 0x%x mask: 0x%x\n", page, index, mask);
   return !(mask & kernelFreePageMap[index]);
}

void k_markPhysPageUsed(void* address)
{
   //k_printf("Marking 0x%x as used\n", address);

   // which page is this address in?
   unsigned int page = (unsigned int)address >> 12; // 4096 bytes in a page

   // which byte is used for this page?
   unsigned int index = page >> 3; // 8 pages represented in a byte

   // which bit in the byte?
   unsigned int mask = 1 << (page & 0x07);

   kernelFreePageMap[index] |= mask;
   //k_printf("page: 0x%x index: 0x%x mask: 0x%x\n", page, index, mask);
   //k_hexout((const char*)pBitmap, 0x40);
}


void* k_allocKernelPage(void)
{
   void* freePage = nextFreePage;
   void* startPage = nextFreePage;
   do
   {
      nextFreePage += PAGE_SIZE;
      if ((unsigned int)nextFreePage > KERNEL_MEMORY_LIMIT)
         nextFreePage = 0;
      if (nextFreePage == startPage)
      {
         k_printf("Out of physical memory!!\n");
         while(1);   // this should run the page swapper
      }
   }while (!k_isPageFree(nextFreePage));
   //k_printf("using free page: 0x%x\n", freePage);
   return freePage;
}

void* k_allocPageDirectory(void)
{
   int i;
   unsigned int* pNewDir = k_allocKernelPage();
   if (!pNewDir)
      return 0;

   k_printf("Created page directory 0x%x\n", pNewDir);

   // initialize the page directory with empty tables
   //for (i=0; i < PAGE_DIRECTORY_MAX_ENTRIES; i++)
   //   pNewDir[i] = MEMORY_PAGE_TABLE_WRITE;
   k_memset(pNewDir, 0, PAGE_SIZE);

   return pNewDir;
}

// this function doesn't work after paging is turned on - the initialization
//  of the page causes a write fault because the logical value is not mapped
//  yet
void* k_realCreatePageTable(unsigned int* pDir, unsigned int index, int global)
{
   int i;
   unsigned int* pPageTable;

   k_printf("k_realCreatePageTable\n");
   if (pDir[index] & MEMORY_PAGE_TABLE_PRESENT)
   {
      k_printf("table already exists\n");
      return 0;
   }

   // find a free page for the new page table
   pPageTable = k_allocKernelPage();
   k_markPhysPageUsed(pPageTable);

   // initialize it
   k_printf("createPageTable init\n");
   if (global)
   {
      for (i=0; i < PAGE_TABLE_MAX_ENTRIES; i++)
         pPageTable[i] = MEMORY_PAGE_USER_MODE | MEMORY_PAGE_GLOBAL | MEMORY_PAGE_WRITE;
   }
   else
   {
      for (i=0; i < PAGE_TABLE_MAX_ENTRIES; i++)
         pPageTable[i] = MEMORY_PAGE_USER_MODE | MEMORY_PAGE_WRITE;
   }

   // set the page in the page directory
   pDir[index] |= (unsigned int)pPageTable | MEMORY_PAGE_TABLE_PRESENT;

   k_printf("Created kernel page table 0x%x at: 0x%x\n", index, pPageTable);

   return pPageTable;
}

__inline void k_map4MPage(unsigned int* pPageDir, unsigned int physical, unsigned int logical, unsigned int flags)
{
   k_printf("map 4M dir:0x%x phys:0x%x log:0x%x flags:0x%x\n", pPageDir, physical, logical, flags);

   pPageDir[GET_DIR_INDEX(logical)] = GET_DISK_4M_LOCATION(physical) | MEMORY_DIR_ENTRY_PRESENT | MEMORY_DIR_ENTRY_SIZE | flags;
}

__inline int k_map4KPage(unsigned int* pPageDir, unsigned int physical, unsigned int logical, unsigned int flags)
{
   unsigned int* pTable;
   unsigned int* pLogTable;
   int alloc = 0;

   k_printf("map 4K dir:0x%x phys:0x%x log:0x%x flags:0x%x\n", pPageDir, physical, logical, flags);

   // make sure the table is present
   unsigned int tableIndex = GET_DIR_INDEX(logical);

   /* make sure the page table exists, and is present */
	if (!(pPageDir[tableIndex] & MEMORY_PAGE_TABLE_PRESENT))
   {
      if (GET_DISK_LOCATION(pPageDir[tableIndex]) != 0)
      {
         /* this means the table is created, but not in physical memory */
         k_printf("Table %i not present\n", tableIndex);
         HALT();
      }
      else
      {
         k_printf("table 0x%x not created\n", tableIndex);
         return 1;
      }
   }

   // get the physical address of the page table
   pTable = (void*)GET_DISK_LOCATION(pPageDir[tableIndex]);
   k_printf("table index: 0x%x phys: 0x%x\n", tableIndex, pTable);

   /* map target table locally */
   if (pPageDir == kernelPageDir)
   {
      pLogTable = pTable;
   }
   else
   {
      pLogTable = globalFreeSpace;
      k_map4KPage(kernelPageDir, (unsigned int)pTable, (unsigned int)pLogTable, 0);
   }

   // bits 12-21 in the linear address is the entry into the page table
   unsigned int index = GET_PAGE_INDEX(logical);

   pLogTable[index] |= GET_DISK_LOCATION(physical) | MEMORY_PAGE_PRESENT | flags;

   /* unmap target table */
   if (pPageDir != kernelPageDir)
   {
      k_unmap4KPage(kernelPageDir, (unsigned int)pLogTable);
   }
}

void k_mapTable(unsigned int* pPageDir, unsigned int address, unsigned int* pTable)
{
   // make sure the table doesn't exist
   unsigned int dirIndex = GET_DIR_INDEX(address);
   k_printf("Mapping table 0x%x for address 0x%x\n", pTable, address);
   k_printf("Using dir index 0x%x\n", dirIndex);

	if ((pPageDir[dirIndex] & MEMORY_PAGE_TABLE_PRESENT) ||
      (GET_DISK_LOCATION(pPageDir[dirIndex]) != 0))
   {
      k_printf("Table %i already exists\n", dirIndex);
      return;
   }

   pPageDir[dirIndex] = GET_DISK_LOCATION((unsigned int)pTable) | MEMORY_PAGE_TABLE_PRESENT |
      MEMORY_PAGE_TABLE_WRITE | MEMORY_PAGE_TABLE_USER_MODE;
}

static void k_unmap4KPage(unsigned int* pPageDir, unsigned int logical)
{
   unsigned int* pTable;

   k_printf("Unmapping: logical addr 0x%x\n", logical);

   if (pPageDir != kernelPageDir)
   {
      k_printf("can't do that yet\n");
      return;
   }

   // bits 22-31 in the linear address is the index of the page table
   unsigned int tableIndex = GET_DIR_INDEX(logical);

   // make sure it's loaded into memory
	if (!(pPageDir[tableIndex] & MEMORY_PAGE_TABLE_PRESENT))
   {
      if (pPageDir[tableIndex] == 0)
      {
         // if the value of the entry is 0, then the page was never created
         k_printf("Trying to unmap an address that was never mapped! 0x%x\n", logical);
         return;
      }
   }

   // get the right table
   pTable = (unsigned int*)GET_DISK_LOCATION(pPageDir[tableIndex]);

   // bits 12-21 in the linear address is the entry into the page table
   unsigned int index = GET_PAGE_INDEX(logical);
   //k_printf("page table index: 0x%x\n", index);

   pTable[index] = 0;
}

int k_initMemory(void* pageStore)
{
   unsigned int i, index;

   /* enable PSE */
   k_printf("Enabling PSE\n");
   writeCR4((readCR4() | CR4_PSE));

   // register the page fault handler
   ISR_RegisterISRHandler(14, _fh_page_fault);

   /* initialize the free kernel page store */
   freePageStore = pageStore;
   nextFreePage = freePageStore;
   k_memset(kernelFreePageMap, 0, sizeof(kernelFreePageMap));

   /* allocate the kernel page directory */
   kernelPageDir = k_allocPageDirectory();
   k_printf("Kernel page directory at: 0x%x\n", kernelPageDir);

   /* map the kernel memory 1:1 */
   for (i=0; i < KERNEL_MEMORY_LIMIT; i+= PAGE_SIZE_4M)
      k_map4MPage(kernelPageDir, i, i, MEMORY_DIR_ENTRY_WRITE | MEMORY_DIR_ENTRY_GLOBAL);

   /* allocate kernel temporary page table */
   //kernelTempTable = k_allocKernelPage();
   //k_memset(kernelTempTable, 0, PAGE_SIZE);
   unsigned int tableIndex = GET_DIR_INDEX(KERNEL_MEMORY_LIMIT);
   k_realCreatePageTable(kernelPageDir, tableIndex, 0);

   // set the page directory to CR3
   k_setPageDirectory(kernelPageDir);

   // turn on paging
   writeCR0(readCR0() | CR0_PG);
   k_printf("paging on\n");

   /* now turn on PGE - must be done after paging is on */

   //for (i=0x200000; i < 0x800000; i++)
   //   *((int*)i) = i;
      
}

void _fh_page_fault(regs_t* pRegs)
{
   // the address that generated the page fault is stored in CR2
   unsigned int addr = readCR2();
   // a page fault can happen for 5 reasons:
   // 1) page not present
   // 2) user code trying to access supervisor page
   // 3) trying to write to a read-only page
   // 4) trying to execute non-executable page
   // 5) a reserved bit is not 0
   // luckily, we can see the reason for the fault in the error code:
   if (pRegs->err_code & PAGE_FAULT_PRESENCE)
   {
      k_printf("page 0x%x not present\n", addr);
   }
   else if (pRegs->err_code & PAGE_FAULT_WRITE)
   {
      k_printf("write access violation at: 0x%x\n", addr);
   }
   else
   {
      k_printf("read access violation at: 0x%x\n", addr);
   }

	//k_printf("task: 0x%x\n", k_getCurrentTask()->taskID);
   k_printRegs(pRegs);

   /*
   unsigned int tableIndex = GET_DIR_INDEX(addr);
   if (k_isTablePresent(pDir, tableIndex))
   {
      k_printf("table 0x%x is present\n", tableIndex);
      unsigned int location = GET_DISK_LOCATION(pDir[tableIndex]);
      k_printf("phys page 0x%x\n", location);
      PageTable* pTable = (PageTable*)location;
      unsigned int index = GET_PAGE_INDEX(addr);
      k_printf("index 0x%x\n", index);
      unsigned int entry = pTable[index];
      if (entry & MEMORY_PAGE_PRESENT)
         k_printf("entry 0x%x\n", GET_DISK_LOCATION(entry));
      else
         k_printf("entry not present!\n");
   }
   else
   {
      k_printf("table 0x%x is not present\n", tableIndex);
   }
   */
   HALT();
}

__inline void* k_getKernelPageDirectory(void)
{
   return kernelPageDir;
}
