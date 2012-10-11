#ifndef _MEMORY__H
#define _MEMORY__H

#define PAGE_SIZE             0x1000         ///< 4KB page
#define PAGE_SIZE_MASK        (PAGE_SIZE-1)  ///< number of bits in page size
#define PAGE_SIZE_4M          0x400000

#define FREE_PAGE_COUNT       0x100       /* used for page directories and temporary page tables */
#define KERNEL_MEMORY_LIMIT   0x800000    /* how much memory is associated with the kernel */

typedef unsigned int PageTable;
typedef unsigned int PageDirectory[PAGE_SIZE/sizeof(unsigned int)];

typedef enum PagingMethod
{
   PAGING_4K_NORMAL,
   PAGING_2M_PAE,
   PAGING_4M_PSE
} PagingMethod;

int k_initMemory(unsigned int* pageDir, unsigned int limit, PagingMethod method, unsigned int pageStore);

unsigned int k_allocKernelPage(void);

#endif /* _MEMORY__H */

