#include "l4.h"
#include "task.h"
#include "memory.h"

static InvalidHandler(Word index)
{
   k_printf("Invalid syscall index %i\n", index);
   HALT();
}

/* returns the address of the kernel interface page (KIP) */
static Word KernelInterface(Word* apiVersion, Word* apiFlags, Word* kernelID)
{
   k_printf("KernelInterface\n");
   *apiVersion = 0x1111111;
   *apiFlags = 0x2222222;
   *kernelID = 0x3333333;
   return KERNEL_INTERFACE_PAGE;
}

/* create, modify or delete a thread */
static Word ThreadControl(Word p1, Word p2, Word p3, Word p4)
{
   k_printf("ThreadControl %i\n", p1);
   return 0;
}

static Word DebugPrintChar(Word p1, Word p2, Word p3, Word p4)
{
   //k_printf("DebugPrintChar %i\n", p1);
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

