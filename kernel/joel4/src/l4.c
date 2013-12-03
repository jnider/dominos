/** L4 userspace functions */

#include "l4.h"

int L4_ValidateKIPMagic(const L4_KIP* pKip)
{
   if ((pKip->magic[0] != KIP_MAGIC_0) ||
       (pKip->magic[1] != KIP_MAGIC_1) ||
       (pKip->magic[3] != KIP_MAGIC_3))
      return 1;

   return 0;
}

L4_KIP* L4_KernelInterface(Word* ApiVersion, Word* ApiFlags, Word* KernelId) 
{
   register Word ret asm("%eax");
   asm volatile
   (
      "movl %1, %%eax      \n"
      "movl %%esp, %%ecx   \n"      /* save the stack pointer in ECX */                
      "leal 1f, %%edx      \n"      /* save the instruction pointer in EDX */          
      "leal %2, %%ebx      \n"      /* save address of first parameter in EBX */
      "sysenter            \n"      /* make the call */
      "1:                  \n"
      : /* output operands */ 
        "=A" (ret)                       /* %0: ret <- EAX */
      : /* input operands */
        "I" (SYSCALL_KERNEL_INTERFACE),  /* %1: reason code -> EAX */
         "m" (ApiVersion)
      : /* clobber list */
         "%ebx",
         "%ecx",
         "%edx"
   );
   return (L4_KIP*)ret;
}

Word L4_ThreadControl(L4_ThreadId dest, L4_ThreadId space, L4_ThreadId scheduler, L4_ThreadId pager, void* UtcbLocation)
{
   register Word ret asm("%eax");
   asm volatile
   (
      "movl %1, %%eax      \n"
      "movl %%esp, %%ecx   \n"      /* save the stack pointer in ECX */                
      "leal 1f, %%edx      \n"      /* save the instruction pointer in EDX */          
      "leal %2, %%ebx      \n"      /* save address of first parameter in EBX */
      "sysenter            \n"      /* make the call */
      "1:                  \n"
      : /* output operands */ 
        "=A" (ret)                       /* %0: ret <- EAX */
      : /* input operands */
        "I" (SYSCALL_THREAD_CONTROL),  /* %1: reason code -> EAX */
         "m" (dest)
      : /* clobber list */
         "%ebx",
         "%ecx",
         "%edx"
   );
   return (Word)ret;
}

void L4_DebugPutChar(int c)
{
   asm volatile
   (
      "movl %0, %%eax      \n"
      "movl %%esp, %%ecx   \n"      /* save the stack pointer in ECX */                
      "leal 1f, %%edx      \n"      /* save the instruction pointer in EDX */          
      "leal %1, %%ebx      \n"      /* save address of first parameter in EBX */
      "sysenter            \n"      /* make the call */
      "1:                  \n"
      : /* output operands */ 
      : /* input operands */
        "I" (SYSCALL_DEBUG_PUT_CHAR),     /* reason code -> EAX */
        "m" (c)
      : /* clobber list */
         "%eax",
         "%ebx",
         "%ecx",
         "%edx"
   );
}


