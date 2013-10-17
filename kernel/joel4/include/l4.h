#ifndef _L4__H
#define _L4__H

/* Based on the X.2 v6 API version */


/* If you update this enum, make sure you update the corresponding table in syscall.c */
typedef enum L4_syscall
{
   SYSCALL_KERNEL_INTERFACE,
   SYSCALL_THREAD_CONTROL,
   SYSCALL_DEBUG_PUT_CHAR
} L4_syscall;

/* basic types */
typedef unsigned int Word;
typedef unsigned long long Word64;
typedef Word L4_ThreadId;
typedef Word64 L4_Clock;
typedef Word L4_MsgTag;
typedef Word L4_GrantItem[2];
typedef Word L4_Fpage; // flex page (flexible size memory page)

#ifdef ARCH_IA32
#define SYSCALL(_reason, _ret, _param1)   asm volatile (                               \
        "pushl %%ebp       \n"      /* save the base pointer for when we come back */  \
        "movl %%esp, %%ecx \n"      /* save the stack pointer in ECX */                \
        "leal 1f, %%edx    \n"      /* save the instruction pointer in EDX */          \
        "sysenter          \n"      /* make the call */                                \
        "1:                \n"                                                         \
        "popl %%ebp        \n"      /* restore the base pointer, and we're done */     \
        : "=A" (_ret)                                                                   \
        : "a" (_reason),            /* EAX = syscall reason */                         \
          "b" (_param1)             /* EBX = param 1 */                                \
          : "%edx");

#endif

/* constants */
#define NILTHREAD    (L4_ThreadId)0
#define ANYTHREAD    (L4_ThreadId)0xFFFFFFFF

/**
 * returns the base address of the KIP (kernel interface page) as mapped in the current address space
 */
static inline void* L4_KernelInterface(Word* ApiVersion, Word* ApiFlags, Word* KernelId) 
{
   Word ret;
   SYSCALL(SYSCALL_KERNEL_INTERFACE, ret, 0);
   return (void*)ret;
}

/* Registers (memory, buffer) */
void L4_StoreMR(int i, Word* w); ///< write a memory register
void L4_LoadMR(int i, Word w); ///< read a memory register
void L4_StoreBR(int i, Word* w); ///< write a buffer register
void L4_LoadBR(int i, Word w); ///< read a buffer register

/**
 * return the thread id of the current thread
 */
static inline L4_ThreadId L4_Myself(void)
{
   // read from the TCR
   return 0;
}

Word L4_ProcessorNo(void); ///< return the processor index of the current thread (see KIP)
L4_ThreadId L4_ExchangeRegisters(L4_ThreadId dest, Word control, Word sp, Word ip, Word flags, Word UserDefinedHandle, L4_ThreadId pager,
   Word* old_control, Word* old_sp, Word* old_ip, Word* old_flags, Word* old_UserDefinedHandle, L4_ThreadId* old_pager);
/**
 * Create, modify or delete a thread
 */
static inline Word L4_ThreadControl(L4_ThreadId dest, L4_ThreadId SpaceSpecifier, L4_ThreadId Scheduler, L4_ThreadId Pager, void* UtcbLocation)
{
   Word ret;
   SYSCALL(SYSCALL_THREAD_CONTROL, dest, ret);
   return ret;
}

void L4_ThreadSwitch(L4_ThreadId dest); ///< switch to the specified thread

/* Clock */
L4_Clock L4_SystemClock(void); ///< get the system clock tick

/* Memory */
void L4_Unmap (Word control);
//Word SpaceControl (ThreadId SpaceSpecifier,Word control, Fpage KernelInterfacePageArea, UtcbArea, ThreadId Redirector, Word& old Control);
//MapItem MapItem (Fpage f, Word SndBase);
//Bool IsMapItem (MapItem m); ///< Delivers true if map item is valid otherwise delivers false
//L4_GrantItem L4_Grant(L4_Fpage f, Word SndBase);

/* IPC */
L4_MsgTag L4_SendIPC(L4_ThreadId to, L4_ThreadId FromSpecifier, Word Timeouts, L4_ThreadId* from);
L4_MsgTag L4_SendLIPC(L4_ThreadId to, L4_ThreadId FromSpecifier, Word Timeouts, L4_ThreadId* from);

/* Miscellaneous functions */
//Word ProcessorControl (Word ProcessorNo, InternalFrequency, ExternalFrequency, voltage);

/* Debug */
inline void L4_DebugHalt(void);
static inline void L4_DebugPutChar(char c)
{
   Word ret;
   SYSCALL(SYSCALL_DEBUG_PUT_CHAR, ret, c);
}

#ifdef ARCH_IA32
#endif

#ifdef ARCH_ARM
#endif

#endif // _L4__H

