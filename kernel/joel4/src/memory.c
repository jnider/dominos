////////////////////////////////////////////////////////////////////////////////
/// @file   memory.c
/// @short  kernel memory manager
/// @author J.Nider
/// @date   30/10/2009
////////////////////////////////////////////////////////////////////////////////

#include "memory.h"
#include "isr.h"     /* regs_t */

// Page table flags
#define MEMORY_PAGE_TABLE_PRESENT      (1<<0)   // 1=table present
#define MEMORY_PAGE_TABLE_WRITE        (1<<1)   // 1=write access
#define MEMORY_PAGE_TABLE_USER_MODE    (1<<2)   // 0=supervisor mode, 1=user mode
#define MEMORY_PAGE_TABLE_WRITETHROUGH (1<<3)   // write-through
#define MEMORY_PAGE_TABLE_CACHED       (1<<4)   // cached
#define MEMORY_PAGE_TABLE_ACCESSED     (1<<5)   // accessed
#define MEMORY_PAGE_TABLE_RESERVED     (1<<6)   // reserved bit - must be 0
#define MEMORY_PAGE_TABLE_SIZE         (1<<7)   // 0=4Kb page
#define MEMORY_PAGE_TABLE_GLOBAL       (1<<8)   // global table
// bits 0x0200, 0x0400 and 0x0800 are available for system use

// Page directory entry flags
#define MEMORY_DIR_ENTRY_PRESENT       (1<<0)   // 1=entry present
#define MEMORY_DIR_ENTRY_WRITE         (1<<1)   // 1=write access
#define MEMORY_DIR_ENTRY_USER_MODE     (1<<2)   // 0=supervisor mode, 1=user mode
#define MEMORY_DIR_ENTRY_WRITETHROUGH  (1<<3)   // write-through
#define MEMORY_DIR_ENTRY_CACHED        (1<<4)   // cached
#define MEMORY_DIR_ENTRY_ACCESSED      (1<<5)   // accessed
#define MEMORY_DIR_ENTRY_DIRTY         (1<<6)   // reserved bit - must be 0
#define MEMORY_DIR_ENTRY_SIZE          (1<<7)   // 0=4K 1=4M
#define MEMORY_DIR_ENTRY_GLOBAL        (1<<8)   // global table
// bits 0x0200, 0x0400 and 0x0800 are available for system use
#define MEMORY_DIR_ENTRY36_BASE_ADDR     (0x1F<<13)
#define MEMORY_DIR_ENTRY_BASE_ADDR(_x)   (_x & 0xFFC00000)

// Page flags
#define MEMORY_PAGE_PRESENT            0x0001   // 1=page present
#define MEMORY_PAGE_WRITE              0x0002   // 1=write access
#define MEMORY_PAGE_USER_MODE          0x0004   // 0=supervisor mode, 1=user mode
#define MEMORY_PAGE_WRITETHROUGH       0x0008   // write-through
#define MEMORY_PAGE_CACHED             0x0010   // cached
#define MEMORY_PAGE_ACCESSED           0x0020   // accessed
#define MEMORY_PAGE_DIRTY              0x0040   // reserved bit - must be 0
#define MEMORY_PAGE_ATTR_INDEX         0x0080   // 0=4Kb page
#define MEMORY_PAGE_GLOBAL             0x0100   // global page
// bits 0x0200, 0x0400 and 0x0800 are available for system use

// flags in CR0 control register
#define CR0_PG 0x80000000  // 1=paging enabled

// CR4 register
#define CR4_PSE (1<<4)     // 1=Page Size Extensions enabled
#define CR4_PAE (1<<5)     // 1=Physical Address Extensions enabled

// page fault codes
#define _PF_CODE_P      0x0001
#define _PF_CODE_WR     0x0002      // write access violation
#define _PF_CODE_US     0x0004
#define _PF_CODE_RSVD   0x0008
#define _PF_CODE_ID     0x0010

#define BUILD_LOGICAL(_dirIndex, _tableIndex) ((_dirIndex << 22) | (_tableIndex << 12))
#define GET_PAGE_TABLE_INDEX(_addr) (_addr >> 22)
#define GET_PAGE_INDEX(_addr)       ((_addr & 0x3FF000) >> 12)
#define GET_DISK_LOCATION(_addr)    (_addr & 0xFFFFF000)

#define PAGE_DIRECTORY_MAX_ENTRIES  1024
#define PAGE_TABLE_MAX_ENTRIES      1024

#define TOTAL_KERNEL_MEMORY         0x400000 /* 4MB should be enough for anybody */

static unsigned char bitmap[(TOTAL_KERNEL_MEMORY/PAGE_SIZE)/sizeof(char)];  // replace this with the define MEMORY_MAP_PHYS
static PageTable kernelPageTable0 __attribute__((aligned(4096)));
static unsigned int nextFreePage = (unsigned int)&kernelPageTable0;
////////////////////////////////////////////////////////////////////////////////
/// @date   03/09/2008
/// @short  Map the logical address to the physical address - only in real mode
/// @param  pPageDirectory
/// @param  physical
/// @param  logical
///
////////////////////////////////////////////////////////////////////////////////
static void k_realMapAddr(PageDirectory* pPageDir, unsigned int physical, unsigned int logical);

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

__inline void k_setPageDirectory(PageDirectory* pPageDir)
{
   //asm __volatile__ ("movl %0, %%cr3" :: "dN" (pPageDir));
   asm __volatile__ ("movl %0, %%cr3" :: "r" (pPageDir), "m" (__force_order));
   //asm volatile("mov %0,%%cr3": :"r" (val), "m" (__force_order));
}

__inline PageDirectory* k_getPageDirectory(void)
{
   unsigned int rv;
   asm __volatile__ ("movl %%cr3, %0": "=r" (rv), "=m" (__force_order));
   return (PageDirectory*)rv;
}

__inline int k_isTablePresent(PageDirectory* pLogicalDir, unsigned int tableIndex)
{
   return (*pLogicalDir[tableIndex]) & MEMORY_PAGE_TABLE_PRESENT;
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

int IsPhysPageFree(unsigned int address)
{
   // which page is this address in?
   unsigned int page = address >> 12; // 4096 bytes in a page

   // which byte is used for this page?
   unsigned int index = page >> 3; // 8 pages represented in a byte

   // which bit in the byte?
   unsigned int mask = 1 << (page & 0x07);

   //k_printf("page: 0x%x index: 0x%x mask: 0x%x\n", page, index, mask);
   return !(mask & bitmap[index]);
}

void k_markPhysPageUsed(unsigned int address)
{
   //k_printf("Marking 0x%x as used\n", address);

   // which page is this address in?
   unsigned int page = address >> 12; // 4096 bytes in a page

   // which byte is used for this page?
   unsigned int index = page >> 3; // 8 pages represented in a byte

   // which bit in the byte?
   unsigned int mask = 1 << (page & 0x07);

   bitmap[index] |= mask;
   //k_printf("page: 0x%x index: 0x%x mask: 0x%x\n", page, index, mask);
   //k_hexout((const char*)pBitmap, 0x40);
}


unsigned int k_findFreePhysicalPage(void)
{
   unsigned int freePage = nextFreePage;
   unsigned int startPage = nextFreePage;
   do
   {
      nextFreePage += PAGE_SIZE;
      if (nextFreePage > TOTAL_KERNEL_MEMORY)
         nextFreePage = 0;
      if (nextFreePage == startPage)
      {
         k_printf("Out of physical memory!!\n");
         while(1);   // this should run the page swapper
      }
   }while (!IsPhysPageFree(nextFreePage));
   //k_printf("using free page: 0x%x\n", freePage);
   return freePage;
}

// this function doesn't work after paging is turned on - the initialization
//  of the page causes a write fault because the logical value is not mapped
//  yet
PageTable* k_realCreatePageTable(PageDirectory* pDir, unsigned int index, int global)
{
   int i;
   PageTable* pPageTable;

   k_printf("k_realCreatePageTable\n");
   if (*pDir[index] & MEMORY_PAGE_TABLE_PRESENT)
   {
      k_printf("table already exists\n");
      return 0;
   }

   // find a free page for the new page table
   pPageTable = (PageTable*)k_findFreePhysicalPage();
   k_markPhysPageUsed((unsigned int)pPageTable);

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
   *pDir[index] |= (unsigned int)pPageTable | MEMORY_PAGE_TABLE_PRESENT;

   k_printf("Created kernel page table 0x%x at: 0x%x\n", index, pPageTable);

   return pPageTable;
}

static void k_realMapAddr(PageDirectory* pPageDirectory, unsigned int physical, unsigned int logical)
{
   //k_printf("Mapping phys: 0x%x to log: 0x%x\n", physical, logical);

   // make sure the table is present
   unsigned int tableIndex = GET_PAGE_TABLE_INDEX(logical);

	if (!(*pPageDirectory[tableIndex] & MEMORY_PAGE_TABLE_PRESENT))
   {
      //k_printf("Table %i not present\n", tableIndex);
      if (GET_DISK_LOCATION(*pPageDirectory[tableIndex]) == 0)
      {
         k_printf("table not created\n");
         HALT();
      }
      else
      {
         // otherwise load it from disk
         k_printf("Trying to load page from disk directory=0x%x\n", pPageDirectory[tableIndex]);
         HALT();
      }
   }

   // bits 22-31 in the linear address is the index of the page table
   PageTable* pTable = (PageTable*)GET_DISK_LOCATION(*pPageDirectory[tableIndex]);
   //k_printf("table: 0x%x\n", pTable);

   // bits 12-21 in the linear address is the entry into the page table
   unsigned int index = GET_PAGE_INDEX(logical);
   //k_printf("page table index: 0x%x\n", index);

   pTable[index] |= GET_DISK_LOCATION(physical) | MEMORY_PAGE_PRESENT;
   //k_printf("mapped ok\n");
}

/*
static void KernelUnmapAddr(unsigned int logical)
{
   PageTable* pTable;
   PageDirectory* pPageDirectory = k_getPageDirectory();

   //k_printf("Unmapping: logical addr 0x%x\n", logical);

   // bits 22-31 in the linear address is the index of the page table
   unsigned int tableIndex = GET_PAGE_TABLE_INDEX(logical);

   // make sure it's loaded into memory
   if (!k_isTablePresent(&osTask, tableIndex))
   {
      if (pPageDirectory[tableIndex] == 0)
      {
         // if the value of the entry is 0, then the page was never created
         k_printf("Trying to unmap an address that was never mapped! 0x%x\n", logical);
         return;
      }
      else
      {
         // otherwise load it from disk
      }
   }

   // get the right table
   pTable = (PageTable*)GET_DISK_LOCATION(pPageDirectory[tableIndex]);

   // bits 12-21 in the linear address is the entry into the page table
   unsigned int index = GET_PAGE_INDEX(logical);
   //k_printf("page table index: 0x%x\n", index);

   pTable[index] &= ~MEMORY_PAGE_PRESENT;
}
*/

int k_initMemory(PageDirectory* pageDir, PagingMethod method)
{
   unsigned int i;

   if (method == PAGING_2M_PAE)
   {
      k_printf("2MB PAE not implemented\n");
      return 1;
   }
   else if (method == PAGING_4M_PSE)
   {
      k_printf("Enabling PSE\n");
      /* enable PSE */
      writeCR4((readCR4() | CR4_PSE));
   }

   // initialize the page directory with empty tables
   k_printf("initializing\n");
   for (i=0; i < PAGE_DIRECTORY_MAX_ENTRIES; i++)
      *pageDir[i] = MEMORY_PAGE_TABLE_GLOBAL | MEMORY_PAGE_TABLE_WRITE;

   /* map the first 4 MB into kernel space */
   if (method == PAGING_4M_PSE)
   {
      *pageDir[0] |= MEMORY_DIR_ENTRY_PRESENT | MEMORY_DIR_ENTRY_SIZE;
   }
   else
   {
      PageTable* pTable = k_realCreatePageTable(pageDir, 0, 1);
	   k_printf("page dir: 0x%x\n", pageDir);
	   k_printf("page table 0: 0x%x\n", pTable);
      for (i=0; i < TOTAL_KERNEL_MEMORY; i += PAGE_SIZE)
         k_realMapAddr(pageDir, i, i);
   }

  
   // set the page directory to CR3
   k_setPageDirectory(pageDir);

   // turn on paging
   k_printf("turning paging on 0x%x\n", pageDir);
   writeCR0(readCR0() | CR0_PG);
   k_printf("paging on\n");

   /* now turn on PGE - must be done after paging is on */

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
   if (pRegs->err_code & _PF_CODE_P)
   {
      k_printf("page level protection violation at: 0x%x\n", addr);
   }
   else if (pRegs->err_code & _PF_CODE_WR)
   {
      k_printf("write access violation at: 0x%x\n", addr);
   }
   else
   {
      k_printf("read access violation at: 0x%x\n", addr);
   }

	//k_printf("task: 0x%x\n", k_getCurrentTask()->taskID);
   k_printf("eip: 0x%x\n", pRegs->eip);

   /*
   unsigned int tableIndex = GET_PAGE_TABLE_INDEX(addr);
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

