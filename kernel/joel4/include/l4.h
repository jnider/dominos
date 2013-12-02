#ifndef _L4__H
#define _L4__H

/* Based on the X.2 v6 API version */


/* If you update this enum, make sure you update the corresponding table in syscall.c */
typedef enum L4_syscall
{
   SYSCALL_INVALID,
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

/* KIP structures */
typedef struct __attribute__((packed)) L4_KIPProcess
{
   Word sp;    // stack pointer
   Word ip;    // instruction poitner
   Word low;   // ?
   Word high;  // ?
} L4_KIPProcess;

typedef struct __attribute__((packed)) L4_KIP
{
   char magic[4];             // 0x00: magic number: must be L4µK
   Word apiVersion;
   Word apiFlags;
   Word kernDescPtr;
   Word kdebugInit;           // 0x10: kernel debugger
   Word kdebugEntry;
   Word kdebugLow;
   Word kdebugHi;
   L4_KIPProcess sigma0;      // 0x20: Sigma 0 process
   L4_KIPProcess sigma1;      // 0x30: Sigma 1 process
   L4_KIPProcess root;        // 0x40: Root process
   Word padding0;             // 0x50:
   Word memoryInfo;
   Word kdebugConfig0;
   Word kdebugConfig1;
   Word padding1[0x40];       // 0x60: padding
   Word padding2[2];          // 0xA0:
   Word utcbInfo;
   Word kipAreaInfo;
   Word padding3[2];          // 0xB0:
   Word bootInfo;
   Word procDescPtr;
} L4_KIP;

typedef struct BootInfo
{
   Word magic;          // 0x00: magic number must be 14B0021D
   Word version;        // version of the header (currently v1)
   Word size;           // complete size of boot info struct including header and all records
   Word first;          // pointer to the first boot record
   Word count;          // number of boot records
} BootInfo;

typedef struct GenericBootRecord
{
   Word type;           // the actual type of boot record
   Word version;        // header version
   Word next;           // pointer to the next boot record
} GenericBootRecord;

typedef struct SimpleModule
{
   GenericBootRecord header;
   Word start;
   Word size;
   Word cmdlineOffset;
} SimpleModule;

typedef struct SimpleExecutable
{
   GenericBootRecord header;
   Word codePStart;     // physical address of code segment
   Word codeVStart;     // virtual address of code segment
   Word codeSize;       // size in bytes of code segment
   Word dataPStart;     // physical address of data segment
   Word dataVStart;     // virtual address of data segment
   Word dataSize;       // size in bytes of data segment
   Word bssPStart;
   Word bssVStart;
   Word bssSize;
   Word entry;          // entry point
   Word flags;          // flags
   Word label;
   Word cmdlineOffset;
} SimpleExecutable;

#define BOOT_INFO_MAGIC                   0x14B0021D
#define BOOT_RECORD_TYPE_SIMPLE_MODULE    0x1

#ifdef ARCH_IA32
/* A note about register selection for passing parameters using the SYSENTER method:
 *
 * The X.2 document specifies register choices for passing parameters, but the spec
 * seems unreasonable. The SYSENTER instruction does not save the stack or instruction
 * pointer of the calling function. This is a problem since we need to be able to return
 * to where the system call was called from. In the SYSEXIT instruction (companion to
 * SYSENTER) the registers ECX and EDX are specified for this purpose; holding the stack
 * pointer and instruction pointer respectively. In any case, this information must be
 * passed to the system call for later use.
 *
 * EAX: system call index
 * ECX: parameter 1
 * EDX: parameter 2
 * ESI: parameter 3
 * EDI: parameter 4
 * EBX: parameter 5
 * EBP: parameter 6
 * ESP: parameter 7
 *
 * This does not leave any more general purpose registers that can be accessed from
 * inside the system call. So since we must use 2 registers for passing the CS:IP,
 * it makes sense to use ECX & EDX for this purpose. So it seems the SYSENTER-SYSEXIT
 * mechanism (which is the fastest way to make a system call in x86 32-bit mode) is
 * not compatible with the X.2 spec.
 *
 * Therefore, I suggest renaming the registers is the following way:
 *
 * EAX: system call index
 * ECX: caller's stack pointer
 * EDX: caller's instruction pointer
 * ESI: parameter 1
 * EDI: parameter 2
 * EBX: parameter 3
 * EBP: parameter 4
 *
 * ESP cannot be used to pass a parameter since the stack is required on the receiver
 * side, and would therefore corrupt the value in ESP.
 */
#endif // IA32

/* constants */
#define NILTHREAD    (L4_ThreadId)0
#define ANYTHREAD    (L4_ThreadId)0xFFFFFFFF

/**
 * returns the base address of the KIP (kernel interface page) as mapped in the current address space
 */
void* L4_KernelInterface(Word* ApiVersion, Word* ApiFlags, Word* KernelId);

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
static inline Word L4_ThreadControl(L4_ThreadId dest, L4_ThreadId space, L4_ThreadId scheduler, L4_ThreadId pager, void* UtcbLocation)
{
   Word ret;
   asm volatile
   (
      "pushl %%ebp         \n"      /* save the base pointer for when we come back */  
      "movl %6, %%eax      \n"      /* store the reason code (system call index) in EAX */
      "movl %1, %%esi      \n"      /* store the dest in ESI */
      "movl %2, %%edi      \n"      /* store the address space specifier in EDI */
      "movl %3, %%ebx      \n"      /* store the scheduler specifier in EDI */
      "movl %%esp, %%ecx   \n"      /* save the stack pointer in ECX */                
      "movl %4, %%esp      \n"      /* store the pager in EBP */
      "movl %5, %%ebp      \n"      /* store the UTCB location in ESP */
      "leal 1f, %%edx      \n"      /* save the instruction pointer in EDX */          
      "sysenter            \n"      /* make the call */
      "1:                  \n"
      "popl %%ebp          \n"      /* restore the base pointer, and we're done */     
      : /* output operands */ 
        "=A" (ret)
      : /* input operands */
        "m" (dest),                       /* %1: dest -> ESI */
        "m" (space),                      /* %2: space -> EDI */
        "m" (scheduler),                  /* %3: scheduler -> EBX */
        "m" (pager),                      /* %4: pager -> ESP */
        "m" (UtcbLocation),               /* %5: UtcbLocation -> EBP */
        "I" (SYSCALL_KERNEL_INTERFACE)    /* %6: reason code -> EAX */
      : /* clobber list */
         "%edx"
   );
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
void L4_DebugPutChar(int c);

#ifdef ARCH_IA32
#endif

#ifdef ARCH_ARM
#endif

#endif // _L4__H

