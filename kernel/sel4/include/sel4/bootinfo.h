/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

#ifndef __SEL4_BOOT_INFO_H_
#define __SEL4_BOOT_INFO_H_

#include <sel4/types.h>

/** 
 * structure to pass region information 
 */
typedef struct {
  seL4_CPtr start;
  seL4_CPtr end;
} seL4_bootinfo_t; 


/**
 * Get the start and size of a cap region of the given type
 */
seL4_bootinfo_t seL4_retrieve_infor(seL4_Word type, seL4_BootInfo *info);

/* Handling bootinformation passed by the kernel
 */
void seL4_InitBootInfo(seL4_BootInfo *ptr);

extern seL4_BootInfo *bootinfo;

seL4_BootInfo * seL4_GetBootInfo(void);

#endif
