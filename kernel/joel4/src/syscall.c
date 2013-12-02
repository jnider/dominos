#include "l4.h"
#include "task.h"
#include "memory.h"

typedef Word(*syscall_handler)(Word* stackPtr);

/*******************************************************************
 * The userspace stub functions
 *******************************************************************/
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


/*******************************************************************
 * The kernelspace handlers
 *******************************************************************/
static Word InvalidHandler(Word* index)
{
   k_printf("Invalid syscall index %i\n", index);
   HALT();
}

/* returns the address of the kernel interface page (KIP) */
static Word KernelInterface(Word* stackPtr)
{
   Word* apiVersion = (Word*)*(stackPtr);
   Word* apiFlags = (Word*)*(stackPtr+1);
   Word* kernelID = (Word*)*(stackPtr+2);

   *apiVersion = ((L4_API_VERSION_X2 << 24) | (6 << 16)); // X.2 v6
   *apiFlags = 0; // little-endian, 32-bit
   *kernelID = (L4_KERNEL_ID_JOEL4 << 24);

   return KERNEL_INTERFACE_PAGE;
}

/* create, modify or delete a thread */
static Word ThreadControl(Word* stackPtr)
{
   Word p1 = *(stackPtr);
   k_printf("ThreadControl %i\n", p1);
   return 0;
}

static inline Word DebugPrintChar(Word* stackPtr)
{
   Word p1 = *(stackPtr);
   k_putchar((char)p1);
   serial_putc((char)p1);
   return 0;
}

static Word syscall_create_task_wrapper(Word p1, Word p2, Word p3, Word p4)
{
   task_t* pTask = k_createTask();
   return pTask->taskID;
}

/* Each entry in this table is a function pointer to a syscall handler. The order is very important, and
   must match the L4_syscall enum, defined in l4.h. This table is used by the syscall mechanism, which has
   a small stub in assembly language. The stub pushes the parameters on the stack, and then jumps to the
   requested function as listed in the table (specified in the EAX register). */
syscall_handler syscall_handler_table[] =
{
   InvalidHandler,
   KernelInterface,
   ThreadControl,
   DebugPrintChar
};

