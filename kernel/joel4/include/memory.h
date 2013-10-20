#ifndef _MEMORY__H
#define _MEMORY__H

// Page table flags
#define MEMORY_PAGE_TABLE_PRESENT      (1<<0)   // 1=table present
#define MEMORY_PAGE_TABLE_WRITE        (1<<1)   // 1=write access
#define MEMORY_PAGE_TABLE_USER_MODE    (1<<2)   // 0=supervisor mode, 1=user mode
#define MEMORY_PAGE_TABLE_WRITETHROUGH (1<<3)   // write-through
#define MEMORY_PAGE_TABLE_CACHED       (1<<4)   // cached
#define MEMORY_PAGE_TABLE_ACCESSED     (1<<5)   // accessed
#define MEMORY_PAGE_TABLE_RESERVED     (1<<6)   // reserved bit - must be 0
#define MEMORY_PAGE_TABLE_SIZE         (1<<7)   // 0=4Kb page 1=4M
#define MEMORY_PAGE_TABLE_GLOBAL       (1<<8)   // global table

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

#define PAGE_SIZE             0x1000         ///< 4KB page
#define PAGE_SIZE_MASK        (PAGE_SIZE-1)  ///< number of bits in page size
#define PAGE_SIZE_4M          0x400000

#define FREE_PAGE_COUNT       0x100       /* used for page directories and temporary page tables */
#define APP_STACK_SIZE        0x1000

/* The following definitions describe the logical memory layout for each user-space process */
#define KERNEL_MEMORY_LIMIT   0x800000    /* how much memory is associated with the kernel */
#define KERNEL_INTERFACE_PAGE 0x900000    /* logical address of KIP for each memory space */
#define APP_CODE              0x1000000   /* logical address for code section for each app */
#define APP_STACK             0x1100000   /* logical address for stack */
#define APP_DATA              0x1200000   /* logical address for data section for each app */

int k_initMemory(void* pageStore);
void* k_allocKernelPage(void);
void* k_allocPageDirectory(void);
__inline void* k_getKernelPageDirectory(void);
__inline void k_copyPageDirectory(void* dest, void* src);
__inline void k_map4MPage(unsigned int* pPageDir, unsigned int physical, unsigned int logical, unsigned int flags);
__inline int k_map4KPage(unsigned int* pPageDir, unsigned int physical, unsigned int logical, unsigned int flags);
void* k_realCreatePageTable(unsigned int* pDir, unsigned int index, int global);
void k_mapTable(unsigned int* pPageDir, unsigned int address, unsigned int* pTable);

/// DEBUG FUNCTIONS
void k_dumpPageDirectory(unsigned int* pDir);

#endif /* _MEMORY__H */

