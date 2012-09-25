/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

#ifndef __SEL4SYSCALLS_H__
#define __SEL4SYSCALLS_H__

#include <stdint.h>

static inline void
seL4_Send(seL4_CPtr dest, seL4_MessageInfo msgInfo)
{
    asm volatile (
        "pushl %%ebp       \n"
        "movl %%ecx, %%ebp \n"
        "movl %%esp, %%ecx \n"
        "leal 1f, %%edx    \n"
        "1:                \n"
        "sysenter          \n"
        "popl %%ebp        \n"
        :
        : "a" (seL4_SysSend),
          "b" (dest),
          "S" (msgInfo.raw),
          "D" (seL4_GetIPCBuffer()->msg[0]),
          "c" (seL4_GetIPCBuffer()->msg[1])
        : "%edx"
    );
}

static inline void
seL4_NBSend(seL4_CPtr dest, seL4_MessageInfo msgInfo)
{
    asm volatile (
        "pushl %%ebp       \n"
        "movl %%ecx, %%ebp \n"
        "movl %%esp, %%ecx \n"
        "leal 1f, %%edx    \n"
        "1:                \n"
        "sysenter          \n"
        "popl %%ebp        \n"
        :
        : "a" (seL4_SysNBSend),
          "b" (dest),
          "S" (msgInfo.raw),
          "D" (seL4_GetIPCBuffer()->msg[0]),
          "c" (seL4_GetIPCBuffer()->msg[1])
        : "%edx"
    );
}

static inline void
seL4_Reply(seL4_MessageInfo msgInfo)
{
    asm volatile (
        "pushl %%ebp       \n"
        "movl %%ecx, %%ebp \n"
        "movl %%esp, %%ecx \n"
        "leal 1f, %%edx    \n"
        "1:                \n"
        "sysenter          \n"
        "popl %%ebp        \n"
        :
        : "a" (seL4_SysReply),
          "S" (msgInfo.raw),
          "D" (seL4_GetIPCBuffer()->msg[0]),
          "c" (seL4_GetIPCBuffer()->msg[1])
        : "%ebx", "%edx"
    );
}

static inline void
seL4_Notify(seL4_CPtr dest, seL4_Word msg)
{
    asm volatile (
        "pushl %%ebp       \n"
        "movl %%esp, %%ecx \n"
        "leal 1f, %%edx    \n"
        "1:                \n"
        "sysenter          \n"
        "popl %%ebp        \n"
        :
        : "a" (seL4_SysSend),
          "b" (dest),
          "S" ((seL4_MessageInfo){{ .length = 1 }}.raw),
          "D" (msg)
        : "%ecx", "%edx"
    );
}

static inline seL4_MessageInfo
seL4_Wait(seL4_CPtr src, seL4_Word* sender)
{
    seL4_Word        badge;
    seL4_MessageInfo info;
    
    asm volatile (
        "pushl %%ebp       \n"
        "movl %%esp, %%ecx \n"
        "leal 1f, %%edx    \n"
        "1:                \n"
        "sysenter          \n"
        "movl %%ebp, %%ecx \n"
        "popl %%ebp        \n"
        :
          "=b" (badge),
          "=S" (info.raw),
          "=D" (seL4_GetIPCBuffer()->msg[0]),
          "=c" (seL4_GetIPCBuffer()->msg[1])
        : "a" (seL4_SysWait),
          "b" (src)
        : "%edx", "memory"
    );
    if (sender) {
        *sender = badge;
    }
    return info;
}

static inline seL4_MessageInfo
seL4_Call(seL4_CPtr dest, seL4_MessageInfo msgInfo)
{
    seL4_MessageInfo info;
    
    asm volatile (
        "pushl %%ebp       \n"
        "movl %%ecx, %%ebp \n"
        "movl %%esp, %%ecx \n"
        "leal 1f, %%edx    \n"
        "1:                \n"
        "sysenter          \n"
        "movl %%ebp, %%ecx \n"
        "popl %%ebp        \n"
        :
          "=S" (info.raw),
          "=D" (seL4_GetIPCBuffer()->msg[0]),
          "=c" (seL4_GetIPCBuffer()->msg[1]),
          "=b" (dest) /* dummy, tells GCC that ebx is clobbered */
        : "a" (seL4_SysCall),
          "b" (dest),
          "S" (msgInfo.raw),
          "D" (seL4_GetIPCBuffer()->msg[0]),
          "c" (seL4_GetIPCBuffer()->msg[1])
        : "%edx", "memory"
    );
    return info;
}

static inline seL4_MessageInfo
seL4_ReplyWait(seL4_CPtr dest, seL4_MessageInfo msgInfo, seL4_Word *sender)
{
    seL4_Word        badge;
    seL4_MessageInfo info;
    
    asm volatile (
        "pushl %%ebp       \n"
        "movl %%ecx, %%ebp \n"
        "movl %%esp, %%ecx \n"
        "leal 1f, %%edx    \n"
        "1:                \n"
        "sysenter          \n"
        "movl %%ebp, %%ecx \n"
        "popl %%ebp        \n"
        :
          "=b" (badge),
          "=S" (info.raw),
          "=D" (seL4_GetIPCBuffer()->msg[0]),
          "=c" (seL4_GetIPCBuffer()->msg[1])
        : "a" (seL4_SysReplyWait),
          "b" (dest),
          "S" (msgInfo.raw),
          "D" (seL4_GetIPCBuffer()->msg[0]),
          "c" (seL4_GetIPCBuffer()->msg[1])
        : "%edx", "memory"
    );
    if (sender) {
        *sender = badge;
    }
    return info;
}

static inline void
seL4_Yield(void) {
    asm volatile (
        "pushl %%ebp       \n"
        "movl %%esp, %%ecx \n"
        "leal 1f, %%edx    \n"
        "1:                \n"
        "sysenter          \n"
        "popl %%ebp        \n"
        :
        : "a" (seL4_SysYield)
        : "%ebx", "%ecx", "%edx", "%esi", "%edi", "memory"
    );
}

static inline void
seL4_DebugPutChar(char c) {
#if defined(SEL4_DEBUG_KERNEL)
    asm volatile (
        "pushl %%ebp       \n"
        "movl %%esp, %%ecx \n"
        "leal 1f, %%edx    \n"
        "1:                \n"
        "sysenter          \n"
        "popl %%ebp        \n"
        :
        : "a" (seL4_SysDebugPutChar),
          "b" (c)
        : "%ecx", "%edx", "%esi", "%edi", "memory"
    );
#endif
}

static inline void
seL4_DebugHalt(void) {
    asm volatile (
        "pushl %%ebp       \n"
        "movl %%esp, %%ecx \n"
        "leal 1f, %%edx    \n"
        "1:                \n"
        "sysenter          \n"
        "popl %%ebp        \n"
        :
        : "a" (seL4_SysDebugHalt)
        : "%ebx", "%ecx", "%edx", "%esi", "%edi", "memory"
    );
}

static inline void
seL4_DebugSnapshot(void) {
#if defined(SEL4_DEBUG_KERNEL)
    asm volatile (
        "pushl %%ebp       \n"
        "movl %%esp, %%ecx \n"
        "leal 1f, %%edx    \n"
        "1:                \n"
        "sysenter          \n"
        "popl %%ebp        \n"
        :
        : "a" (seL4_SysDebugSnapshot)
        : "%ebx", "%ecx", "%edx", "%esi", "%edi", "memory"
    );
#endif
}

#endif /* __SEL4SYSCALLS_H__ */
