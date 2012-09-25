/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

#ifndef __SEL4_IA32_EXCEPTION_IPC_
#define __SEL4_IA32_EXCEPTION_IPC_

/**
 * NOT A STANDALONE INCLUDE
 */

/* Unknown syscall exception message. */
#define SEL4_EXCEPT_IPC_LABEL                 3

/* User exception (such as divide by zero) message. */
#define SEL4_USER_EXCEPTION_LABEL             4
#define SEL4_USER_EXCEPTION_LENGTH            5

typedef enum {
	EXCEPT_IPC_USER_MR_FAULT_IP,
	EXCEPT_IPC_USER_MR_ESP,
	EXCEPT_IPC_USER_MR_EFLAGS,
	EXCEPT_IPC_USER_MR_NUMBER,
	EXCEPT_IPC_USER_MR_CODE,
	SEL4_USER_EXCEPT_IPC_LENGTH
} seL4_UserExceptIPCRegister;

typedef enum {
	EXCEPT_IPC_SYS_MR_EAX,
	EXCEPT_IPC_SYS_MR_EBX,
	EXCEPT_IPC_SYS_MR_ECX,
	EXCEPT_IPC_SYS_MR_EDX,
	EXCEPT_IPC_SYS_MR_ESI,
	EXCEPT_IPC_SYS_MR_EDI,
	EXCEPT_IPC_SYS_MR_EBP,
	EXCEPT_IPC_SYS_MR_EIP,
	EXCEPT_IPC_SYS_MR_ESP,
	EXCEPT_IPC_SYS_MR_EFLAGS,
	EXCEPT_IPC_SYS_MR_SYSCALL,
	SEL4_EXCEPT_IPC_LENGTH,
} seL4_ExceptIPCRegister;

typedef union {
    struct {
        seL4_Word fault_ip;
        seL4_Word esp;
        seL4_Word eflags;
        seL4_Word exception_number;
        seL4_Word exception_code;
    } regs;
    seL4_Word raw[5];
} seL4_UserExceptionIpcRegisters;

typedef union {
    struct {
        seL4_Word eax;
        seL4_Word ebx;
        seL4_Word ecx;
        seL4_Word edx;
        seL4_Word esi;
        seL4_Word edi;
        seL4_Word ebp;
        seL4_Word eip;
        seL4_Word esp;
        seL4_Word eflags;
        seL4_Word syscall;
    } regs;
    seL4_Word raw[11];
} seL4_ExceptionIpcRegisters;

static inline seL4_Word seL4_isExceptIPC_Tag(seL4_MessageInfo tag)
{
  return (seL4_Label_From(tag) == SEL4_EXCEPT_IPC_LABEL);
}

static inline seL4_Word seL4_ExceptIPC_Get(seL4_Word mr)
{
  return seL4_GetMR(mr);
}

static inline void seL4_ExceptIPC_Set(seL4_Word index, seL4_Word val)
{
  seL4_SetMR(index, val);
}

static inline seL4_Word seL4_IsArchSyscallFrom(seL4_MessageInfo tag)
{
  return (seL4_Length_From(tag) == SEL4_EXCEPT_IPC_LENGTH);
}

static inline seL4_Word seL4_IsArchExceptionFrom(seL4_MessageInfo tag)
{
  return (seL4_Length_From(tag) == SEL4_USER_EXCEPTION_LENGTH);
}

#endif // __SEL4_IA32_EXCEPTION_IPC_
