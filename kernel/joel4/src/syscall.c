#include "l4.h"
#include "task.h"
#include "memory.h"

/*******************************************************************
 * The userspace stub functions
 *******************************************************************/
void* L4_KernelInterface(Word* ApiVersion, Word* ApiFlags, Word* KernelId) 
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
   return (void*)ret;
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
static InvalidHandler(Word index)
{
   k_printf("Invalid syscall index %i\n", index);
   HALT();
}

/* returns the address of the kernel interface page (KIP) */
static Word KernelInterface(Word* stackPtr)
{
   Word* apiVersion = *(stackPtr);
   Word* apiFlags = *(stackPtr+1);
   Word* kernelID = *(stackPtr+2);
   k_printf("user stack: 0x%x\n", stackPtr);
   k_printf("&apiVersion: 0x%x\n", apiVersion);
   k_printf("*apiVersion: 0x%x\n", *apiVersion);
   *apiVersion = 0x1111111;
   k_printf("*apiVersion: 0x%x\n", *apiVersion);
   //*apiFlags = 0x2222222;
   //*kernelID = 0x3333333;

   return KERNEL_INTERFACE_PAGE;
}

/* create, modify or delete a thread */
static Word ThreadControl(Word p1, Word p2, Word p3, Word p4)
{
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
void* syscall_handler_table[] =
{
   InvalidHandler,
   KernelInterface,
   ThreadControl,
   DebugPrintChar
};

