#ifndef _MEMORY__H
#define _MEMORY__H

#define PAGE_SIZE             0x1000         ///< 4KB page
#define PAGE_SIZE_MASK        (PAGE_SIZE-1)  ///< number of bits in page size

typedef unsigned int PageTable;
typedef unsigned int PageDirectory[PAGE_SIZE/sizeof(unsigned int)];

typedef enum PagingMethod
{
   PAGING_4K_NORMAL,
   PAGING_2M_PAE,
   PAGING_4M_PSE
} PagingMethod;

int k_initMemory(PageDirectory* pageDir, PagingMethod method);

#endif /* _MEMORY__H */

