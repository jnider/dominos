////////////////////////////////////////////////////////////////////////////////
/// @file   ktask.cpp
/// @short  task related functions and data
/// @author J.Nider
/// @date   30/10/2009
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "kstdio.h"
//#include "ktimer.h"                    // k_startTimer
//#include "klist.h"                     // KPtrList
#include "memory.h"                    // _BASE_ADDRESS
#include "task.h"                     // task_t, task_list
#include "eflags.h"

//extern tss_t userTSS;
extern unsigned short user_tss;

static task_list allTaskList;   ///< list of all tasks
static task_list readyTaskQueue;///< queue of ready tasks
static task_t* pCurrentTask; ///< currently executing task
static task_t defaultTaskParams;       ///< new tasks copy their default values from here

static unsigned int nextTaskID=1;      ///< the last assigned task ID

//extern "C" void k_cooperate(regs_t* pRegs);

static unsigned int createTaskID(void)
{
   int found=0;
   unsigned int i;
   unsigned int id;
   do
   {
      if (found)
         nextTaskID++;

      // look for this ID in the all task list
      found = (int)k_taskListFindByID(&allTaskList, nextTaskID);
   } while (found);

   return nextTaskID++;
}

void k_initTask(unsigned short codeSeg, unsigned short dataSeg, unsigned short stackSegInt, unsigned int intStack)
{
   /* initialize the lists */
   k_taskListInit(&allTaskList);
   k_taskListInit(&readyTaskQueue);

   k_printf("CS: 0x%x DS: 0x%x SS0: 0x%x ESP: 0x%x\n", codeSeg, dataSeg, stackSegInt, intStack);
   k_memset(&defaultTaskParams, 0, sizeof(task_t));
   defaultTaskParams.segment.cs = codeSeg;
   defaultTaskParams.segment.ds = dataSeg;
   defaultTaskParams.segment.es = dataSeg;
   defaultTaskParams.segment.ss = dataSeg;
   defaultTaskParams.segment.ebp = 0;
   defaultTaskParams.segment.esp = 0;
   defaultTaskParams.segment.eflags =  EFLAGS_IF |                  // make sure interrupts are enabled!
                                    EFLAGS_IOPL(PRIVILEGE_LEVEL_USER);  // and no access to IO ports    
   defaultTaskParams.segment.ss0 = stackSegInt;
   defaultTaskParams.segment.esp0 = intStack;
   //defaultTaskParams.segment.io_map = ; // 13.5.2

   //ISR_RegisterISRHandler(48, k_cooperate);	// allow for cooperative multitasking (giving up timeslices)
}

task_t* k_createTask(void)
{
   // allocate the structure for the context
   task_t* pTask = (task_t*)kmalloc(sizeof(task_t));
   if (!pTask)
   {
      k_printf("Out of kernel memory\n");
      return 0;
   }

   k_memcpy(pTask, &defaultTaskParams, sizeof(task_t));
   pTask->taskID = createTaskID();
   k_printf("Task ID: 0x%x\n", pTask->taskID);

   // add it to the all task list
   k_printf("task: add to list\n");
   k_taskListAdd(&allTaskList, pTask);

   return pTask;
}

task_t* k_createThread(task_t* pTask, unsigned int* code, unsigned int codeSize, unsigned int* data, unsigned int dataSize, unsigned int entryPoint)
{
   int i;

   k_printf("Code: 0x%x (0x%x)\n", code, codeSize);
   k_printf("Data: 0x%x (0x%x)\n", data, dataSize);
   k_printf("Entry point: 0x%x\n", entryPoint);

   // choose entry point
   pTask->segment.eip = entryPoint;
   pTask->segment.esp = APP_STACK + APP_STACK_SIZE;

   // set up page directory
   pTask->segment.pdbr = (unsigned int)k_allocPageDirectory();
   k_printf("PDBR: 0x%x\n", pTask->segment.pdbr);

   // map the kernel
   for (i=0; i < KERNEL_MEMORY_LIMIT; i+=PAGE_SIZE_4M)
      k_map4MPage((unsigned int*)pTask->segment.pdbr, i, i, MEMORY_PAGE_TABLE_USER_MODE);

   // map the table - for now, take it from the kernel
   k_printf("Task: map tables\n");
   unsigned int* pTable = k_allocKernelPage();
   k_mapTable((unsigned int*)pTask->segment.pdbr, (unsigned int)APP_CODE, pTable);
   k_mapTable((unsigned int*)pTask->segment.pdbr, (unsigned int)APP_DATA, k_allocKernelPage());

   // map the code
   k_printf("Task: map code\n");
   for (i=0; i < codeSize; i+= PAGE_SIZE)
      k_map4KPage((unsigned int*)pTask->segment.pdbr, (unsigned int)code+i, (unsigned int)APP_CODE+i,
         MEMORY_PAGE_USER_MODE);

   // map the data
   k_printf("Task: map data\n");
   for (i=0; i < dataSize; i+= PAGE_SIZE)
      k_map4KPage((unsigned int*)pTask->segment.pdbr, (unsigned int)data+i, (unsigned int)APP_DATA+i,
         MEMORY_PAGE_WRITE | MEMORY_PAGE_USER_MODE);

   // map the stack
   k_printf("Task: map stack\n");
   for (i=0; i < APP_STACK_SIZE; i+= PAGE_SIZE)
   {
      k_map4KPage((unsigned int*)pTask->segment.pdbr, (unsigned int)k_allocKernelPage(), (unsigned int)APP_STACK+i,
         MEMORY_PAGE_WRITE | MEMORY_PAGE_USER_MODE);
   }

}

task_t* k_getCurrentTask()
{
   return pCurrentTask;
}

void k_setCurrentTask(task_t* pTask)
{
   pCurrentTask = pTask;
}

void k_switchTask(regs_t* pRegs, task_t* pNextTask)
{
   k_printf("switching to task 0x%x\n", pNextTask->taskID);

   // if there is a current task (we are switching because we finished a timeslice)
   if (pCurrentTask)
   {
      // save the context of whatever is currently running
      //k_printf("saving context\n");
      //k_printf("old page directory to 0x%x\n", pCurrentTask->task.pdbr);
      pCurrentTask->segment.eax = pRegs->eax;
      pCurrentTask->segment.ebx = pRegs->ebx;
      pCurrentTask->segment.ecx = pRegs->ecx;
      pCurrentTask->segment.edx = pRegs->edx;

      pCurrentTask->segment.ebp = pRegs->ebp;
      pCurrentTask->segment.esi = pRegs->esi;
      pCurrentTask->segment.edi = pRegs->edi;

      pCurrentTask->segment.cs = pRegs->cs;
      pCurrentTask->segment.ds = pRegs->ds;
      pCurrentTask->segment.es = pRegs->es;
      pCurrentTask->segment.fs = pRegs->fs;
      pCurrentTask->segment.gs = pRegs->gs;

      pCurrentTask->segment.eflags = pRegs->eflags;
      pCurrentTask->segment.eip = pRegs->eip;
      pCurrentTask->segment.esp = pRegs->esp;
      pCurrentTask->segment.ss = pRegs->ss;
      //k_printRegs(pRegs);
      //while(1);
   }

   //k_printf("switching to task: 0x%x\n", pNextTask->taskID);
   pCurrentTask = pNextTask;

   asm volatile("mov %0,%%cr3": :"r" (pNextTask->segment.pdbr));

   pRegs->eax = pCurrentTask->segment.eax;
   pRegs->ebx = pCurrentTask->segment.ebx;
   pRegs->ecx = pCurrentTask->segment.ecx;
   pRegs->edx = pCurrentTask->segment.edx;

   pRegs->ebp = pCurrentTask->segment.ebp;
   pRegs->esi = pCurrentTask->segment.esi;
   pRegs->edi = pCurrentTask->segment.edi;

   pRegs->cs = pCurrentTask->segment.cs;
   pRegs->ds = pCurrentTask->segment.ds;
   pRegs->es = pCurrentTask->segment.es;
   pRegs->fs = pCurrentTask->segment.fs;
   pRegs->gs = pCurrentTask->segment.gs;

   pRegs->eip = pCurrentTask->segment.eip;
   pRegs->eflags = pCurrentTask->segment.eflags;
   pRegs->esp = pCurrentTask->segment.esp;
   pRegs->ss = pCurrentTask->segment.ss;

   //k_printf("eflags: 0x%x\n",  pRegs->eflags);

   //k_printRegs(pRegs);
   // switch page directories
   //k_printf("changing page directory to 0x%x\n", pCurrentTask->task.pdbr);
   //SetPageDirectory((PageDirectory*)pCurrentTask->task.pdbr);
   //   asm volatile("mov %0,%%cr3": :"r" (pNextTask->task.pdbr));
   //k_printf("done\n");
   //while(1);
}

void k_scheduler(regs_t* pRegs)
{
   task_t* pNextTask = 0;
   //k_printf("scheduler (count: %i)\n", readyTaskQueue.Count());

   if (k_taskListCount(&readyTaskQueue) == 0)  // the idle task is always ready to run
   {
      k_printf("no ready tasks!\n");
      while(1);
   }

   // pick the next task to run - don't remove it, just point to it
   //task_t* pNextTask = readyTaskQueue[0];
   // for now, move it to the back of the list (poor man's scheduling)
   pNextTask = k_taskListHead(&readyTaskQueue);
   //readyTaskQueue.PushBack(pNextTask);

   // switch to it
   if (pNextTask->taskID != pCurrentTask->taskID)
   {
      k_switchTask(pRegs, pNextTask);
   }
}

void k_setTaskAsReady(task_t* pTask)
{
   pTask->state = TASK_STATE_READY;

   // if no insertion point was found, add it to the end
   k_taskListAdd(&readyTaskQueue, pTask);
}

void k_setTaskAsPending(task_t* pTask, taskState state)
{
   unsigned int i;

   //k_printf("Setting task as pending\n");
   // change the state of the task
   pTask->state = state;

   // remove it from the ready queue
   k_taskListDeleteByID(&readyTaskQueue, pTask->taskID);
}

task_t* k_getTaskByID(unsigned int taskID)
{
   return k_taskListFindByID(&allTaskList, taskID);
}

task_t* k_getTask(const char* name)
{
   return k_taskListFindByName(&allTaskList, name);
}

unsigned int k_getCurrentTaskID()
{
   //k_printf("ptr: 0x%x\n", pCurrentTask);
   //k_printf("id: 0x%x\n", pCurrentTask->taskID);
   return pCurrentTask->taskID;
}

/*
void k_sleep(unsigned int taskID, unsigned int ms)
{
   task_t* pTask = k_getTask(taskID);
   if (!pTask)
   {
      while(1);
   }

   k_setTaskAsPending(pTask, TASK_STATE_SLEEPING);

	// set up the timer
   k_startTimer(ms, 0, 0, taskID);

 	__ASM("int $48\n");	// give up the timeslice
}

extern "C" void k_cooperate(regs_t* pRegs)
{
	//k_printf("task 0x%x is now blocked\n", pCurrentTask->taskID);

	k_scheduler(pRegs);
}
*/

