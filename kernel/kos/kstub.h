////////////////////////////////////////////////////////////////////////////////
///@file    kstub.h
///@short   macros for writing kernel stubs
///@author  J.Nider
///@date    30/10/2009
////////////////////////////////////////////////////////////////////////////////
/// all the definitions needed to write a user space stub
////////////////////////////////////////////////////////////////////////////////

#ifndef _KSTUB__H
#define _KSTUB__H

#include "kstdio.h"  // __ASM
//#include "memory.h"  // *_BASE_ADDRESS

extern unsigned int dsos_segment;
extern unsigned int dsapp_segment;

//unsigned int dsos_segment_fixed = dsos_segment + 0x1000000;
//const unsigned int dsapp_segment_fixed = dsapp_segment + 0x1000000;


// the stack actually stays in the same place, we just look at it from a different
// point of view, so the DS and SS stay the same.  Otherwise, we get horrible problems
// later on, such as using 'div' instructions which expect DS==SS for some reason.
// maybe this is just a compiler anomaly?  In any case, this is a big hack because
// GCC doesn't support multiple segments (yet).
#define __USE_DSOS()          __ASM("movl $0x23, %eax\n     \
                                    mov %ax, %ds\n          \
                                    mov %ax, %ss\n          \
                                    add $0x1000000,%esp\n   \
                                    add $0x1000000,%ebp")
/*
#define __USE_DSOS()          __ASM("mov (%0), %%eax\n      \
                                    shl $3, %%eax\n         \
                                    or $3, %%eax\n          \
                                    mov %%ax, %%ds\n        \
                                    mov %%ax, %%ss\n        \
                                    add $0x1000000,%%esp\n  \
                                    add $0x1000000,%%ebp" :: "am"(dsos_segment))
*/

#define __USE_DSAPP()         __ASM("movl $6, %eax\n        \
                                    shl $3, %eax\n          \
                                    or $3, %eax\n           \
                                    mov %ax, %ds\n          \
                                    mov %ax, %ss\n          \
                                    sub $0x1000000, %esp\n  \
                                    sub $0x1000000, %ebp")
/*
#define __USE_DSAPP()         __ASM("movl %0, %%eax\n       \
                                    mov (%%eax), %%eax\n    \
                                    shl $3, %%eax\n         \
                                    or $3, %%eax\n          \
                                    mov %%ax, %%ds\n        \
                                    mov %%ax, %%ss\n        \
                                    sub $0x1000000,%%esp\n  \
                                    sub $0x1000000,%%ebp" :: "am"(dsapp_segment))
*/

#endif // _KSTUB__H
