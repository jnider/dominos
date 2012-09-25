////////////////////////////////////////////////////////////////////////////////
///@file    memory.h
////////////////////////////////////////////////////////////////////////////////
///@short   Memory allocation and mapping functions
///
///@author  J.Nider
///@date    01-Jun-2007 11:37:06 AM
////////////////////////////////////////////////////////////////////////////////

#ifndef _MEMORY__H
#define _MEMORY__H

#include "ktask.h"
//#include "ksegments.h"
#include "layout.h"

#define PAGE_SIZE             0x1000         ///< 4KB page
#define PAGE_SIZE_MASK        (PAGE_SIZE-1)  ///< number of bits in page size

#define APP_MAX_SIZE          0x01000000     ///< allow up to 16MB of user code
#define APP_STACK_ADDRESS     (DSAPP_BASE_ADDRESS + APP_STACK_SIZE)
#define APP_STACK_SIZE        0x00008000     ///< 32KB stack
#define APP_INT_STACK_ADDRESS (APP_STACK_ADDRESS + INT_STACK_SIZE)
#define APP_LOAD_ADDRESS      (DSAPP_BASE_ADDRESS + 0x100000)

#define KERNEL_STACK_ADDRESS  ((unsigned int)&_kernelStack)
#define KERNEL_STACK_SIZE     0x00001000     ///< 4KB stack
#define INT_STACK_ADDRESS     ((unsigned int)&_intStack)
#define INT_STACK_SIZE        0x00001000     ///< 4KB stack
#define KERNEL_PAGE_DIRECTORY (PageDirectory*)(&_kernelPageDirectory)
#define KERNEL_STUB_PHYS      (unsigned int)&_stubCode      ///< physical address of stubs
#define MEMORY_MAP_PHYS       (unsigned char*)&_memoryMap   ///< physical address of memory map
#define KERNEL_TABLE_COUNT    1              ///< first few tables are for kernel stuff
#define KERNEL_STUB_ADDRESS   CSAPP_BASE_ADDRESS
#define KERNEL_CODE_PHYS      (unsigned int)&_kernelCodeStart

#define APP_PAGE_DIRECTORY    (PageDirectory*)(DSAPP_BASE_ADDRESS-0x4)
#define APP_MEMBREAK          (APP_LOAD_ADDRESS + APP_MAX_SIZE + APP_MAX_SIZE)

#define START_KERNEL (unsigned char*)&_startKernel
#define END_KERNEL (unsigned char*)&_endKernel

#ifdef __cplusplus

typedef unsigned int PageTable;
typedef unsigned int PageDirectory;

////////////////////////////////////////////////////////////////////////////////
/// @date   03/09/2008
/// @short  Initializes the memory manager
/// @param  size
///
////////////////////////////////////////////////////////////////////////////////
void k_initMemory(unsigned int size);

////////////////////////////////////////////////////////////////////////////////
/// @date   03/09/2008
/// @short  Creates a page directory
///
////////////////////////////////////////////////////////////////////////////////
PageDirectory* CreatePageDirectory();

////////////////////////////////////////////////////////////////////////////////
/// @date   03/09/2008
/// @short  Creates a page table in real mode (before paging)
////////////////////////////////////////////////////////////////////////////////
PageTable* k_realCreatePageTable(PageDirectory* pDir, unsigned int index, bool global);

////////////////////////////////////////////////////////////////////////////////
/// @date   03/09/2008
/// @short  Creates a page table
/// @param  pTask the task in which to map the new table
/// @param  index location of the table in the directory
///
////////////////////////////////////////////////////////////////////////////////
PageTable* k_createPageTable(task_t* pTask, PageDirectory* pLogicalDir, unsigned int index, bool unmap = true);

////////////////////////////////////////////////////////////////////////////////
/// @date   03/09/2008
/// @short  Loads a page directory into the processor
/// @param  dir
///
////////////////////////////////////////////////////////////////////////////////
extern "C" void SetPageDirectory(PageDirectory* dir);

////////////////////////////////////////////////////////////////////////////////
/// @date   03/09/2008
/// @short  Allocate a new page of memory
/// @param  pPageDir
/// @param  address
///
////////////////////////////////////////////////////////////////////////////////
void* AllocatePage(unsigned int address);

////////////////////////////////////////////////////////////////////////////////
/// @date   03/09/2008
/// @short  Mark a physical page of memory as used
/// @param  address
///
////////////////////////////////////////////////////////////////////////////////
void k_markPhysPageUsed(unsigned int address);

////////////////////////////////////////////////////////////////////////////////
/// @date   03/09/2008
/// @short  Remove mapping for this logical page
/// @param  pPageDirectory
/// @param  logical
///
////////////////////////////////////////////////////////////////////////////////
void UnmapAddr(PageDirectory* pPageDirectory, unsigned int logical);

////////////////////////////////////////////////////////////////////////////////
/// @date   03/09/2008
/// @short  Find an unused physical page of memory
/// @param  void
///
////////////////////////////////////////////////////////////////////////////////
unsigned int k_findFreePhysicalPage(void);

////////////////////////////////////////////////////////////////////////////////
/// @date   31/10/2009
/// @short  Create page table for new task
/// @param  void
////////////////////////////////////////////////////////////////////////////////
/// After the page table is created, the interrupt stack is mapped, as well as
/// the kernel stubs
////////////////////////////////////////////////////////////////////////////////
bool PrepareProcessMemory(task_t* pTask);

void* sbrk(unsigned int increment);
void* brk(const void* end);

void writeCR0(unsigned int val);
unsigned int readCR0(void);
unsigned int readCR2(void);

PageDirectory* k_getPageDirectory(void);
bool k_isTablePresent(PageDirectory* pLogicalDir, unsigned int tableIndex);
bool k_getLogicalAddr(PageDirectory* pLogicalDir, unsigned int physical, unsigned int &logical);   ///< searches the page tables for this physical address
bool k_getPhysicalAddr(PageDirectory* pLogicalDir, unsigned int logical, unsigned int &physical);   ///< looks up the physical page if mapped

////////// this should be in an interface header file - not here ////////
void k_mapAddr(task_t* pTask, unsigned int physical, unsigned int logical);

////////////////////////////////////////////////////////////////////////////////
/// @date   08/09/2009
/// @short  Create a task from an executable image in memory
/// @param  name name of the module being loaded
/// @param  loadAddress the starting physical address
/// @param  endAddress the ending physical address
////////////////////////////////////////////////////////////////////////////////
/// The image is sitting in contiguous physical memory between the two given
/// addresses.  It is assumed that the image is in ELF format.  The ELF sections
/// are parsed, and mapped to logical memory for a new task.  The entry point
/// of the task becomes the e_entry field of the ELF header.
////////////////////////////////////////////////////////////////////////////////
task_t* LoadApp(char* name, unsigned int loadAddress, unsigned int endAddress);

#endif // __cplusplus
#endif // _MEMORY__H

