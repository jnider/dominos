#include "l4.h"
#include "task.h"
#include "memory.h"
#include "serial.h"
#include "kvideo.h"

typedef Word(*syscall_handler)(void* params);

typedef struct KernelInterface_params
{
   Word* apiVersion;
   Word* apiFlags;
   Word* kernelID;
} KernelInterface_params;

typedef struct ThreadControl_params
{
   L4_ThreadId dest;
   L4_ThreadId space;
   L4_ThreadId scheduler;
   L4_ThreadId pager;
   void* utcbLocation;
} ThreadControl_params;

/*******************************************************************
 * The kernelspace handlers
 *******************************************************************/
static Word InvalidHandler(void* index)
{
   k_printf("Invalid syscall index %i\n", index);
   HALT();
}

/* returns the address of the kernel interface page (KIP) */
static Word KernelInterface(void* params)
{
	KernelInterface_params* p = (KernelInterface_params*)params;
   *p->apiVersion = ((L4_API_VERSION_X2 << 24) | (6 << 16)); // X.2 v6
   *p->apiFlags = 0; // little-endian, 32-bit
   *p->kernelID = (L4_KERNEL_ID_JOEL4 << 24);

   return KERNEL_INTERFACE_PAGE;
}

/* create, modify or delete a thread */
static Word ThreadControl(void* params)
{
	ThreadControl_params* p = (ThreadControl_params*)params;
   k_printf("ThreadControl dest:0x%x space: 0x%x scheduler: 0x%x pager: 0x%x utcb: 0x%x\n", p->dest, p->space, p->scheduler, p->pager, p->utcbLocation);

	// check to see if the address space already exists
   unsigned long mspace=0;
	task_t* mainTask = k_getTaskByID(p->space);
	if (mainTask)
   {
		k_printf("Address space %i already exists\n", p->space);
      mspace = k_getMemorySpace(mainTask);
   }
	else
   {
		k_printf("Address space %i doesn't exist\n", p->space);
      mspace = k_createMemorySpace();
   }
   task_t* newTask = k_createThread(mspace, 0, 0, 0);
   if (newTask)
      return 0;
   return 1;
}

static inline Word DebugPrintChar(void* params)
{
   Word p1 = *(Word*)(params);
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
   ThreadControl,					/* SYSCALL_THREAD_CONTROL */
   DebugPrintChar
};

