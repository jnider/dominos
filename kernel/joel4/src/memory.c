////////////////////////////////////////////////////////////////////////////////
/// @file   memory.c
/// @short  physical memory manager
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
#define CR4_PGE (1<<7)     // 1=Page Size Extensions enabled

// page fault codes
#define PAGE_FAULT_PRESENCE   0x0001
#define PAGE_FAULT_WRITE      0x0002      // write access violation
#define PAGE_FAULT_USER       0x0004      // code running in user(1) or supervisor(0) mode
#define PAGE_FAULT_RSVD       0x0008      // a reserved bit is 1 with PSE or PAE enabled
#define PAGE_FAULT_IFETCH     0x0010      // caused by instruction fetch

#define BUILD_LOGICAL(_dirIndex, _tableIndex) ((_dirIndex << 22) | (_tableIndex << 12))
#define GET_TABLE_INDEX(_addr)      (_addr >> 22)
#define GET_PAGE_INDEX(_addr)       ((_addr & 0x3FF000) >> 12)
#define GET_DISK_LOCATION(_addr)    (_addr & 0xFFFFF000)

#define GET_DISK_4M_LOCATION(_addr) (_addr & 0xFFC00000)

#define PAGE_DIRECTORY_MAX_ENTRIES  1024
#define PAGE_TABLE_MAX_ENTRIES      1024

void k_unmap4KPage(unsigned int* pPageDir, unsigned int logical);
static unsigned char kernelFreePageMap[((USER_MEMORY_START - KERNEL_MEMORY_LIMIT)/PAGE_SIZE/8)];
static void* freePageStore;
static void* nextFreePage;
static void* kernelPageDir;

static void* nextFreeUserPage;
static void* lastFreeUserPage;

void _fh_page_fault(regs_t* pRegs);

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
   //k_printf("k_setPageDirectory 0x%x\n", pageDir);
   asm __volatile__ ("movl %0, %%cr3" :: "r" (pageDir), "m" (__force_order));
}

__inline void* k_getPageDirectory(void)
{
   void* rv;
   asm __volatile__ ("movl %%cr3, %0": "=r" (rv), "=m" (__force_order));
   return rv;
}

__inline int k_isTablePresent(unsigned int* pPageDir, unsigned int address)
{
   return (pPageDir[GET_TABLE_INDEX(address)]) & MEMORY_PAGE_TABLE_PRESENT;
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

int k_isPageFree(void* address, char map[])
{
   // which page is this address in?
   unsigned int page = (unsigned int)address >> 12; // 4096 bytes in a page

   // which byte is used for this page?
   unsigned int index = page >> 3; // 8 pages represented in a byte

   // which bit in the byte?
   unsigned int mask = 1 << (page & 0x07);

   //k_printf("page: 0x%x index: 0x%x mask: 0x%x\n", page, index, mask);
   return !(mask & map[index]);
}

void k_markPhysPageUsed(char map[], void* address)
{
   //k_printf("Marking 0x%x as used\n", address);

   // which page is this address in?
   unsigned int page = (unsigned int)address >> 12; // 4096 bytes in a page

   // which byte is used for this page?
   unsigned int index = page >> 3; // 8 pages represented in a byte

   // which bit in the byte?
   unsigned int mask = 1 << (page & 0x07);

   map[index] |= mask;
   //k_printf("page: 0x%x index: 0x%x mask: 0x%x\n", page, index, mask);
   //k_hexout((const char*)pBitmap, 0x40);
}

void k_markPhysPageFree(void* address)
{
   //k_printf("Marking 0x%x as used\n", address);

   // which page is this address in?
   unsigned int page = (unsigned int)address >> 12; // 4096 bytes in a page

   // which byte is used for this page?
   unsigned int index = page >> 3; // 8 pages represented in a byte

   // which bit in the byte?
   unsigned int mask = 1 << (page & 0x07);

   kernelFreePageMap[index] &= ~mask;
   //k_printf("page: 0x%x index: 0x%x mask: 0x%x\n", page, index, mask);
   //k_hexout((const char*)pBitmap, 0x40);
}

/* allocates a 4K physical page from reserved physical memory
 * this does nothing with virtual memory since all the kernel's memory
 * should already be mapped as global huge pages into every memory space
 * this is used for page-sized dynamic allocations the kernel does for itself which are
 * temporary and can be released & reused again
 */
void* k_allocKernelPage(void)
{
   void* freePage = nextFreePage;
   void* startPage = nextFreePage; /* used to detect when we wrap-around and hit our starting point */
   do
   {
      nextFreePage += PAGE_SIZE;
      if ((unsigned int)nextFreePage >= KERNEL_MEMORY_LIMIT) /* wrap around */
         nextFreePage = (void*)(KERNEL_MEMORY_LIMIT-(RESERVED_PAGE_COUNT<<12));
      if (nextFreePage == startPage)
      {
         k_printf("Out of physical kernel memory!!\n");
         while(1);   // this should notify the user?
      }
   }while (!k_isPageFree(nextFreePage, kernelFreePageMap));

   k_markPhysPageUsed(kernelFreePageMap, freePage);
   k_printf("KERN ALLOC: 0x%x\n", freePage);
   return freePage;
}

void* k_allocUserPage(void)
{
   void* freeUserPage = nextFreeUserPage;
   nextFreeUserPage += PAGE_SIZE;
   if (freeUserPage >= lastFreeUserPage)
   {
      k_printf("Out of physical user memory!!\n");
      while(1);   // this should run the page swapper
   }

   //k_printf("USER ALLOC: 0x%x\n", freeUserPage);
   return freeUserPage;
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

void k_mapTable(unsigned int* pPageDir, unsigned int address)
{
   int i;
   unsigned int* pTable;

   // make sure the table doesn't exist
   unsigned int dirIndex = GET_TABLE_INDEX(address);

	if (k_isTablePresent(pPageDir, address)) 
   {
      k_printf("Table %i already exists\n", dirIndex);
      return;
   }

   pTable = k_allocKernelPage();

   for (i=0; i < PAGE_TABLE_MAX_ENTRIES; i++)
      pTable[i] = MEMORY_PAGE_USER_MODE | MEMORY_PAGE_GLOBAL | MEMORY_PAGE_WRITE;

   pPageDir[dirIndex] = GET_DISK_LOCATION((unsigned int)pTable) | MEMORY_PAGE_TABLE_PRESENT |
      MEMORY_PAGE_TABLE_WRITE | MEMORY_PAGE_TABLE_GLOBAL | MEMORY_PAGE_TABLE_USER_MODE;
}

__inline void k_map4MPage(unsigned int* pPageDir, unsigned int physical, unsigned int logical, unsigned int flags)
{
   //k_printf("map 4M dir:0x%x phys:0x%x log:0x%x flags:0x%x\n", pPageDir, physical, logical, flags);

   pPageDir[GET_TABLE_INDEX(logical)] = GET_DISK_4M_LOCATION(physical) | MEMORY_PAGE_TABLE_PRESENT | MEMORY_PAGE_TABLE_SIZE | flags;
}

__inline int k_map4KPage(unsigned int* pPageDir, unsigned int physical, unsigned int logical, unsigned int flags)
{
   unsigned int* pTable;
   int alloc = 0;

   //k_printf("map 4K page dir:0x%x phys:0x%x log:0x%x flags:0x%x\n", pPageDir, physical, logical, flags);

   /* make sure the page table exists, and is present */
   unsigned int tableIndex = GET_TABLE_INDEX(logical);
   if (!k_isTablePresent(pPageDir, logical))
   {
      if (GET_DISK_LOCATION(pPageDir[tableIndex]) != 0)
      {
         /* this means the table is created, but not in physical memory */
         k_printf("Table %i not present\n", tableIndex);
         HALT();
      }
      else
      {
         k_mapTable(pPageDir, logical);
      }
   }

   pTable = (void*)GET_DISK_LOCATION(pPageDir[tableIndex]);

   // bits 12-21 in the linear address is the entry into the page table
   unsigned int index = GET_PAGE_INDEX(logical);

   pTable[index] |= GET_DISK_LOCATION(physical) | MEMORY_PAGE_PRESENT | flags;

   return 0;
}

void k_unmap4KPage(unsigned int* pPageDir, unsigned int logical)
{
   unsigned int* pTable;

   //k_printf("Unmapping: logical addr 0x%x\n", logical);

   if (pPageDir != kernelPageDir)
   {
      k_printf("can't do that yet\n");
      return;
   }

   // bits 22-31 in the linear address is the index of the page table
   unsigned int tableIndex = GET_TABLE_INDEX(logical);

   // make sure it's loaded into memory
   if (!k_isTablePresent(pPageDir, logical))
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

int k_initMemory(void* kernelStart, void* kernelEnd, void* userStart, void* userEnd)
{
   unsigned int i, index;

   /* enable PSE (4MB pages in 32-bit paging mode) and PGE (enable global pages) */
   //k_printf("Enabling PSE & PGE\n");
   writeCR4((readCR4() | CR4_PSE | CR4_PGE));

   // register the page fault handler
   ISR_RegisterISRHandler(14, _fh_page_fault);

   /* initialize the free kernel page store */
   freePageStore = kernelStart;
   nextFreePage = freePageStore;
   k_memset(kernelFreePageMap, 0, sizeof(kernelFreePageMap));

   /* initialize the free user page store */
   nextFreeUserPage = userStart;
   lastFreeUserPage = userEnd;
   //k_printf("user mem start: 0x%x user mem end: 0x%x\n", userStart, userEnd);

   /* allocate the kernel page directory */
   kernelPageDir = k_allocPageDirectory();
   //k_printf("Kernel page directory at: 0x%x\n", kernelPageDir);

   /* map the kernel memory 1:1 */
   for (i=0; i < KERNEL_MEMORY_LIMIT; i+= PAGE_SIZE_4M)
      k_map4MPage(kernelPageDir, i, i, MEMORY_PAGE_TABLE_WRITE | MEMORY_PAGE_TABLE_GLOBAL);

   /* allocate kernel temporary page table */
   k_mapTable(kernelPageDir, KERNEL_MEMORY_LIMIT);

   // set the page directory to CR3
   k_setPageDirectory(kernelPageDir);

   // turn on paging
   writeCR0(readCR0() | CR0_PG);
   //k_printf("paging on\n");

   /* now turn on PGE - must be done after paging is on */
}

void _fh_page_fault(regs_t* pRegs)
{
   unsigned int* pDir;

   // the address that generated the page fault is stored in CR2
   unsigned int addr = readCR2();
   // a page fault can happen for 5 reasons:
   // 1) page not present
   // 2) user code trying to access supervisor page
   // 3) trying to write to a read-only page
   // 4) trying to execute non-executable page
   // 5) a reserved bit is not 0
   // luckily, we can see the reason for the fault in the error code

   // print the reason
   if (pRegs->err_code & PAGE_FAULT_WRITE)
      k_printf("write access ");
   else
      k_printf("read access ");

   if (pRegs->err_code & PAGE_FAULT_PRESENCE)
      k_printf("0x%x access violation\n", addr);
   else
      k_printf("0x%x not present\n", addr);

   if (pRegs->err_code & PAGE_FAULT_USER)
      k_printf("user mode\n");
   else
      k_printf("supervisor mode\n");

   if (pRegs->err_code & PAGE_FAULT_RSVD)
      k_printf("a reserved bit is 1\n");

   if (pRegs->err_code & PAGE_FAULT_IFETCH)
      k_printf("caused during instruction fetch\n");

   // now see if we can handle it
	//k_printf("task: 0x%x\n", k_getCurrentTask()->taskID);
   k_printRegs(pRegs);
   asm __volatile__ ("movl %%cr3, %0": "=a" (pDir));
   k_printf("page dir: 0x%x\n", pDir);
   k_dumpPageDirectory(pDir);

   unsigned int tableIndex = GET_TABLE_INDEX(addr);
   if (k_isTablePresent(pDir, addr))
   {
      k_printf("table 0x%x is present\n", tableIndex);
      unsigned int location = GET_DISK_LOCATION(pDir[tableIndex]);
      k_printf("phys page 0x%x\n", location);
      unsigned int* pTable = (unsigned int*)location;
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
   HALT();
}

__inline void* k_getKernelPageDirectory(void)
{
   return kernelPageDir;
}

int k_isMapped(unsigned int logical)
{
   unsigned int* pDir = k_getPageDirectory();

   // if the table is not mapped, the page cannot possibly be mapped
   if (!k_isTablePresent(pDir, logical))
      return 0;

   unsigned int* pTable = (void*)GET_DISK_LOCATION(pDir[GET_TABLE_INDEX(logical)]);
   if (!(pTable[GET_PAGE_INDEX(logical)] & MEMORY_PAGE_PRESENT))
      return 0;
   
   return 1;
}

void k_dumpPageDirectory(unsigned int* pDir)
{
   unsigned int i, j;
   char flags[8];
   k_printf("----------- Page Directory 0x%x contents -----------\n", pDir);
   for (i=0; i < PAGE_DIRECTORY_MAX_ENTRIES; i++)
   {
      if (pDir[i] & MEMORY_PAGE_TABLE_PRESENT)
      {
         k_memset(flags, 0, 8);
         if (pDir[i] & MEMORY_PAGE_TABLE_PRESENT)
            flags[0] = 'P';
         if (pDir[i] & MEMORY_PAGE_TABLE_WRITE)
            flags[1] = 'W';
         if (pDir[i] & MEMORY_PAGE_TABLE_USER_MODE)
            flags[2] = 'U';
         if (pDir[i] & MEMORY_PAGE_TABLE_RESERVED)
            flags[3] = '*';
         if (pDir[i] & MEMORY_PAGE_TABLE_SIZE)
            flags[4] = 'M';
         else
            flags[4] = 'K';
         if (pDir[i] & MEMORY_PAGE_TABLE_GLOBAL)
            flags[5] = 'G';

         if (pDir[i] & MEMORY_PAGE_TABLE_SIZE)
            k_printf("%i: 0x%08x [%s]\n", i, GET_DISK_LOCATION(pDir[i]), flags);
         else
         {
            unsigned int* pTable = (void*)GET_DISK_LOCATION(pDir[i]);
            if (pDir == k_getPageDirectory())
            {
               for (j=0; j < PAGE_TABLE_MAX_ENTRIES; j++)
               {
                  k_memset(flags, '-', 8);
                  if (pTable[j] & MEMORY_PAGE_TABLE_PRESENT)
                     flags[0] = 'P';
                  if (pTable[j] & MEMORY_PAGE_TABLE_WRITE)
                     flags[1] = 'W';
                  if (pTable[j] & MEMORY_PAGE_TABLE_USER_MODE)
                     flags[2] = 'U';
                  if (pTable[j] & MEMORY_PAGE_TABLE_RESERVED)
                     flags[3] = '*';
                  if (pTable[j] & MEMORY_PAGE_TABLE_SIZE)
                     flags[4] = 'M';
                  else
                     flags[4] = 'K';
                  if (pDir[i] & MEMORY_PAGE_TABLE_GLOBAL)
                     flags[5] = 'G';
                  flags[6] = 0;
                  if (pTable[j] & MEMORY_PAGE_PRESENT)
                     k_printf("0x%08x: 0x%08x [%s]\n", BUILD_LOGICAL(i,j), GET_DISK_LOCATION(pTable[j]), flags);
               }
            }
         }
      }
   }
}

