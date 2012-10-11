////////////////////////////////////////////////////////////////////////////////
///@file    kmalloc.cpp
///@short   Memory allocation functions for the kernel
///@author  J.Nider
///@date    11-Jun-2007 03:48:06 PM
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "kstdio.h"
#include "memory.h"

#define MIN_ALLOC 4 // must allocate at least 4 bytes

typedef struct mem_node
{
   int size;
   struct mem_node* next;
} mem_node_t;

typedef struct mem_list
{
   mem_node_t* pFirst;
   mem_node_t* pLast;
} mem_list_t;

/* Keeping pointers to the first and the last element of the free list. */
static mem_list_t memList = { 0, 0 };

void k_initHeap(int address, int size)
{
   /* create the first node from the free memory */
   memList.pFirst = (mem_node_t*)address;
   memList.pFirst->size = size - sizeof(mem_node_t);
   memList.pFirst->next = 0;

   k_printf("initHeap address: 0x%x size %i\n", address, size);

   /* map it for use by kmalloc */
   memList.pLast = memList.pFirst;

   k_printf("There are %i bytes free for kernel objects on the heap\n", memList.pFirst->size);
}

//////////
///@date    11-Jun-2007 03:48:38 PM
///@brief   Allocation for kernel-space memory from the heap
///@param   size The size in bytes of the requested block
//////////
void* kmalloc(int size)
{
   // don't allocate an empty block
   if (size == 0)
      return 0;

   // must allocate a minimum size block
   if (size < MIN_ALLOC)
      size = MIN_ALLOC;

   mem_node_t* pCurrent = memList.pFirst;
   mem_node_t* pPrevious = 0;
   mem_node_t* pFirstfit = 0;
   mem_node_t* pFirstfit_pPrevious = 0;

   /* Trying exact fit */
   while (pCurrent)
   {
      if (pCurrent->size == size)
      {
         /* Found exact fit, allocate node */
         if (pPrevious)
         {
            /* Node in the middle of the list */
            pPrevious->next = pCurrent->next;
         }
         else
         {
            /* Node is pFirst in list */
            memList.pFirst = pCurrent->next;
         }
         if (memList.pLast == pCurrent)
         {
            /* Node is pLast in list */
            memList.pLast = pPrevious;
         }
         return (char *)pCurrent + sizeof( mem_node_t );
      }
      else if (pCurrent->size > size && (pFirstfit == 0 || pCurrent->size < pFirstfit->size))
      {
         /* Remember pPrevious node in case we do not get an exact fit.
         Note that this is the node *pointing to* the pFirst fit,
         as we need that for allocating (i.e., changing next pointer).
         */
         pFirstfit_pPrevious = pPrevious;
         pFirstfit = pCurrent;
      }
      /* Skip to next node */
      pPrevious = pCurrent;
      pCurrent = pCurrent->next;
   }

   /* No exact fit; go for pFirst fit */
   if (pFirstfit != 0)
   {
      if ((pFirstfit->size - size) > MIN_ALLOC)
      {
         /* Oversized - split into two nodes */
         mem_node_t* newnode = (mem_node_t*)((char*)pFirstfit + sizeof(mem_node_t) + size);
         newnode->size = pFirstfit->size - size - sizeof(mem_node_t);
         newnode->next = pFirstfit->next;
         pFirstfit->next = newnode;
         pFirstfit->size = pFirstfit->size - newnode->size - sizeof(mem_node_t);
      }
      if (pFirstfit_pPrevious != 0)
      { 
         /* Node in the middle of the list */
         pFirstfit_pPrevious->next = pFirstfit->next;
      }
      else
      {
         /* Node is pFirst in list */
         memList.pFirst = pFirstfit->next;
      }
      if (memList.pLast == pFirstfit)
      {
         /* Node is pLast in list */
         memList.pLast = pFirstfit_pPrevious;
      }
      return (char*)pFirstfit + sizeof(mem_node_t);
   }

   /* No fit - out of memory */
   return 0;
}

void kfree(void* ptr)
{
   k_printf("kfree\n");
   mem_node_t* pNode = (mem_node_t*)((char*)ptr - sizeof(mem_node_t));
   pNode->next = 0;

   // return it to the free list
   if (memList.pLast)
      memList.pLast->next = pNode;
   else
      memList.pFirst = pNode;

   memList.pLast = pNode;
}

unsigned int k_freeMem(void)
{
   unsigned int space = 0;
   mem_node_t* pNode = memList.pFirst;
   while (pNode)
   {
      space += pNode->size;
      pNode = pNode->next;
   }
   return space;
}

