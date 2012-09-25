////////////////////////////////////////////////////////////////////////////////
///@file    memory.h
////////////////////////////////////////////////////////////////////////////////
///@short   Memory allocation and mapping functions
///
///@author  J.Nider
///@date    01-Jun-2007 11:37:06 AM
////////////////////////////////////////////////////////////////////////////////

#ifndef _LAYOUT__H
#define _LAYOUT__H

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


#define CSOS_BASE_ADDRESS     0x00000000
#define CSOS_LIMIT            0xFFFFFFFF
#define DSOS_BASE_ADDRESS     0x00000000
#define DSOS_LIMIT            0xFFFFFFFF
#define CSAPP_BASE_ADDRESS    0x00400000     // user code can execute from here - includes kernel stubs
#define CSAPP_LIMIT           0xFFFFFFFF
#define DSAPP_BASE_ADDRESS    0x01000000     // user writable memory starts from 16MB
#define DSAPP_LIMIT           0xFFFFFFFF


// segment indices into the GDT
enum GDTEntries
{
   NULL_SEGMENT,
   CSINT_SEGMENT,
   DSINT_SEGMENT,
   CSOS_SEGMENT,
   DSOS_SEGMENT,
   CSAPP_SEGMENT,
   DSAPP_SEGMENT,
   OSTSS_SEGMENT,
   APPTSS_SEGMENT
};

#endif // _LAYOUT__H
