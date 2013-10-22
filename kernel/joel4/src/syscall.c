#include "l4.h"
#include "task.h"
#include "memory.h"

typedef Word(syscall_handler_t)(Word p1, Word p2, Word p3, Word p4, Word p5);

/* returns the address of the kernel interface page (KIP) */
static inline Word KernelInterface(Word p1, Word p2, Word p3, Word p4, Word p5)
{
   k_printf("KernelInterface\n");
   return KERNEL_INTERFACE_PAGE;
}

/* create, modify or delete a thread */
static inline Word ThreadControl(Word p1, Word p2, Word p3, Word p4, Word p5)
{
   k_printf("ThreadControl %i\n", p1);
   return 0;
}

static inline Word DebugPrintChar(Word p1, Word p2, Word p3, Word p4, Word p5)
{
   k_putchar((char)p1);
   serial_putc((char)p1);
   return 0;
}

static inline Word syscall_create_task_wrapper(Word p1, Word p2, Word p3, Word p4, Word p5)
{
   task_t* pTask = k_createTask();
   return pTask->taskID;
}

/* Each entry in this table is a function pointer to a syscall handler. The order is very important, and
   must match the L4_sycall enum, defined in l4.h. This table is used by the syscall mechanism, which has
   a small stub in assembly language. The stub pushes the parameters on the stack, and then jumps to the
   requested function as listed in the table (specified in the EAX register). */
syscall_handler_t* syscall_handler_table[] =
{
   KernelInterface,
   ThreadControl,
   DebugPrintChar
};

