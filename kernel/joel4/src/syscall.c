#include "l4.h"
#include "task.h"
#include "memory.h"
#include "serial.h"
#include "kvideo.h"

typedef Word(*syscall_handler)(Word* stackPtr);

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
   L4_ThreadId dest = *(stackPtr);
   L4_ThreadId space = *(stackPtr+1);
   L4_ThreadId scheduler = *(stackPtr+2);
   L4_ThreadId pager = *(stackPtr+3);
   void* utcbLocation = (void*)*(stackPtr+4);
   k_printf("ThreadControl dest:0x%x space: 0x%x scheduler: 0x%x pager: 0x%x utcb: 0x%x\n", dest, space, scheduler, pager, utcbLocation);

   /*
   k_createThread(rootTask, &_root_task_code_start,
                  (unsigned int)&_root_task_code_size,
                  (void*)&_root_task_data_start,
                  (unsigned int)&_root_task_data_size,
                  (unsigned int)root_task_main);
   */
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

