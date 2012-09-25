/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

#ifndef _SEL4FUNCTIONS_H_
#define _SEL4FUNCTIONS_H_

#include <sel4/types.h>
#include <sel4/globals.h>

static inline seL4_IPCBuffer* seL4_GetIPCBuffer(void) {
    return *(seL4_IPCBuffer**)seL4_TLS_IPCBuffer;
}

static inline void seL4_SetMR(int i, seL4_Word v) {
    seL4_GetIPCBuffer()->msg[i] = v;
}

static inline seL4_Word seL4_GetMR(int i) {
    return seL4_GetIPCBuffer()->msg[i];
}

#endif /* _SEL4FUNCTIONS_H_ */
