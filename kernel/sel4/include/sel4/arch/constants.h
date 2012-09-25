/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

#ifndef _SEL4CONSTANTS_H_
#define _SEL4CONSTANTS_H_

#define TLS_GDT_ENTRY    6
#define TLS_GDT_SELECTOR ((TLS_GDT_ENTRY << 3) | 3)

typedef enum {
    seL4_IA32_4K = seL4_NonArchObjectTypeCount,
    seL4_IA32_4M,
    seL4_IA32_PageTableObject,
    seL4_IA32_PageDirectoryObject,
    seL4_IA32_IOPageTableObject,
    seL4_ObjectTypeCount
} seL4_ArchObjectType;

enum {
    seL4_IA32_ASIDControlCap = seL4_NonArchInitCapCount,
    seL4_IA32_InitASIDPool,
    seL4_IA32_InitIOPort,
    seL4_IA32_InitIOSpace,
    seL4_IA32_InitPageTable,
    seL4_InitCapCount
};

#endif /* _SEL4CONSTANTS_H_ */
