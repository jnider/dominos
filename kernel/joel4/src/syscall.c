#include "l4.h"
#include "task.h"

typedef uint32(syscall_handler_t)(uint32 param1);

static inline uint32 DebugPrintChar(uint32 param1)
{
   k_putchar((char)param1);
   serial_putc((char)param1);
   return 0;
}

static inline uint32 syscall_create_task_wrapper(uint32 p1)
{
   task_t* pTask = k_createTask();
   return pTask->taskID;
}

syscall_handler_t* syscall_handler_table[] =
{
   DebugPrintChar,
   syscall_create_task_wrapper
};

