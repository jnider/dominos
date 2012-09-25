/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

#ifndef __ARCH_SEL4TYPES_H__
#define __ARCH_SEL4TYPES_H__

#include <stdint.h>

#define seL4_WordBits 32

#define seL4_PageBits 12
#define seL4_SlotBits 4
#define seL4_TCBBits 10
#define seL4_EndpointBits 4
#define seL4_PageTableBits 12
#define seL4_PageDirBits 12
#define seL4_IOPageTableBits 12

typedef uint32_t seL4_Word;
typedef seL4_Word seL4_CPtr;

typedef seL4_CPtr seL4_IA32_ASIDControl;
typedef seL4_CPtr seL4_IA32_ASIDPool;
typedef seL4_CPtr seL4_IA32_IOSpace;
typedef seL4_CPtr seL4_IA32_IOPort;
typedef seL4_CPtr seL4_IA32_Page;
typedef seL4_CPtr seL4_IA32_PageDirectory;
typedef seL4_CPtr seL4_IA32_PageTable;
typedef seL4_CPtr seL4_IA32_IOPageTable;

/*
 * User context as used by seL4_TCB_ReadRegisters / seL4_TCB_WriteRegisters.
 */

typedef union {
    struct {
        /* frameRegisters */
        seL4_Word eip, esp, eflags, eax, ebx, ecx, edx, esi, edi, ebp;
        /* gpRegisters */
        seL4_Word tls_base, fs, gs;
    } regs;
    seL4_Word raw[13];
} seL4_UserContext;

typedef enum {
    seL4_IA32_CacheDisabled = 0x01,
    seL4_IA32_WriteThrough  = 0x02,
    seL4_IA32_Default_VMAttributes = 0,
    SEL4_FORCE_LONG_ENUM(seL4_IA32_Default_VMAttributes),
} seL4_IA32_VMAttributes;

#endif /* __ARCH_SEL4TYPES_H__ */
