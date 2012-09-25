/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

#ifndef __SEL4_IA32_PAGEFAULT_IPC_
#define __SEL4_IA32_PAGEFAULT_IPC_

/**
 * This is what pagefault ipc looks like
 * Not a standalone include! include messages.h
 */
#define SEL4_PFIPC_LABEL              2
#define SEL4_PFIPC_LENGTH             4

#define SEL4_PFIPC_FAULT_IP           0
#define SEL4_PFIPC_FAULT_ADDR         1
#define SEL4_PFIPC_PREFETCH_FAULT     2
#define SEL4_PFIPC_FSR                3

typedef union {
    struct {
        seL4_Word fault_ip;
        seL4_Word fault_addr;
        seL4_Word prefetch_fault;
        seL4_Word fsr;
    } regs;
    seL4_Word raw[4];
} seL4_PageFaultIpcRegisters;

/**
 * Get values from page fault ipc
 */
static inline seL4_Word seL4_PF_FIP(void)
{
  return seL4_GetMR(SEL4_PFIPC_FAULT_IP);
}


static inline seL4_Word seL4_PF_Addr(void)
{
  return seL4_GetMR(SEL4_PFIPC_FAULT_ADDR);
}

static inline seL4_Word seL4_isPageFault_MSG(void)
{
  return (seL4_Get_MSGLabel() == SEL4_PFIPC_LABEL);
}

static inline seL4_Word seL4_isPageFault_Tag(seL4_MessageInfo tag)
{
  return (seL4_Label_From(tag) == SEL4_PFIPC_LABEL);
}

static inline bool seL4_Fault_isWriteFault(seL4_Word FaultStatusRegister)
{
    return (FaultStatusRegister & 0x2);
}

static inline bool seL4_Fault_isReadFault(seL4_Word FaultStatusRegister)
{
    return !(FaultStatusRegister & 0x2);
}

static inline bool seL4_Fault_isProtectionFault(seL4_Word FaultStatusRegister)
{
    return (FaultStatusRegister & 0x1);
}

static inline bool seL4_Fault_isNonPresentFault(seL4_Word FaultStatusRegister)
{
    return !(FaultStatusRegister & 0x1);
}


#endif
