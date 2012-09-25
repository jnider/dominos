////////////////////////////////////////////////////////////////////////////////
/// @file   ktask.cpp
/// @short  task related functions and data
/// @author J.Nider
/// @date   30/10/2009
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "kstdio.h"
#include "ktimer.h"                    // k_startTimer
#include "klist.h"                     // KPtrList
#include "memory.h"                    // _BASE_ADDRESS
#include "ktask.h"                     // task_t
#include "kstub.h"

//extern tss_t userTSS;
extern unsigned short user_tss;

static KPtrList<task_t> allTaskList;   ///< list of all tasks
static KPtrList<task_t> readyTaskQueue;///< queue of ready tasks
static task_t* pCurrentTask = &osTask; ///< currently executing task
static task_t defaultTaskParams;       ///< new tasks copy their default values from here

static unsigned int nextTaskID=1;      ///< the last assigned task ID

extern "C" void k_cooperate(regs_t* pRegs);

extern "C" unsigned int __getTaskID(void)
{
   __ASM("add $4, %ebp\n");
   k_getCurrentTaskID();
   __ASM("sub $4, %ebp\n");
   __KERNEL_RETURN();

   return 0;   // the real value is kept in eax - this is to suppress the warning
}

extern "C" void __sleep(unsigned int taskID, unsigned int ms)
{
   __ASM("add $4, %ebp\n");
   k_sleep(taskID, ms);
   __ASM("sub $4, %ebp\n");
   __KERNEL_RETURN();
}

static unsigned int createTaskID()
{
   bool found=false;
   do
   {
      if (found)
      {
         found = false;
         nextTaskID++;
      }

      // look for this ID in the all task list
      for (unsigned int i=0; i < allTaskList.Count(); i++)
      {
         if (allTaskList[i]->taskID == nextTaskID)
         {
            found = true;
            continue;
         }
      }

   } while (found);

   unsigned int id = nextTaskID++;
   return id;
}

void k_initTask(unsigned short codeSeg, unsigned short dataSeg, unsigned short stackSegInt)
{
   //k_printf("CS: 0x%x DS: 0x%x SS0: 0x%x\n", codeSeg, dataSeg, stackSegInt);
   k_memset(&defaultTaskParams, 0, sizeof(task_t));
   defaultTaskParams.task.cs = codeSeg;
   defaultTaskParams.task.ds = dataSeg;
   defaultTaskParams.task.es = dataSeg;
   defaultTaskParams.task.ss = dataSeg;
   defaultTaskParams.task.ebp = APP_STACK_ADDRESS - DSAPP_BASE_ADDRESS;
   defaultTaskParams.task.esp = APP_STACK_ADDRESS - DSAPP_BASE_ADDRESS;
   defaultTaskParams.task.eflags =  EFLAGS_IF |                         // make sure interrupts are enabled!
                                    EFLAGS_IOPL(PRIVILEGE_LEVEL_USER);  // and no access to IO ports    
   defaultTaskParams.task.ss0 = stackSegInt;
   defaultTaskParams.task.esp0 = APP_INT_STACK_ADDRESS;// - DSAPP_BASE_ADDRESS;
   //defaultTaskParams.task.io_map = ; // 13.5.2

   ISR_RegisterISRHandler(48, k_cooperate);	// allow for cooperative multitasking (giving up timeslices)
}

task_t* k_createTask(unsigned int entryPoint)
{
   // allocate the structure for the context
   task_t* pTask = (task_t*)kmalloc(sizeof(task_t));

   k_memcpy(pTask, &defaultTaskParams, sizeof(task_t));
   pTask->taskID = createTaskID();
   //k_printf("Task ID: 0x%x\n", pTask->taskID);

   // choose entry point
   pTask->task.eip = entryPoint;
   //k_printf("task 0x%x eip: 0x%x\n", pTask->taskID, pTask->task.eip);

   // set up page directory and tables
   if (!PrepareProcessMemory(pTask))
   {
      k_printf("can't create process memory\n");
      kfree(pTask);
      return NULL;
   }

   // add it to the all task list
   allTaskList.PushBack(pTask);

   //k_printf("PDBR: 0x%x\n", pTask->task.pdbr);
   return pTask;
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
   //k_printf("switching to task 0x%x\n", pNextTask->taskID);

   // if there is a current task (we are switching because we finished a timeslice)
   if (pCurrentTask)
   {
      // save the context of whatever is currently running
      //k_printf("saving context\n");
      //k_printf("old page directory to 0x%x\n", pCurrentTask->task.pdbr);
      pCurrentTask->task.eax = pRegs->eax;
      pCurrentTask->task.ebx = pRegs->ebx;
      pCurrentTask->task.ecx = pRegs->ecx;
      pCurrentTask->task.edx = pRegs->edx;

      pCurrentTask->task.ebp = pRegs->ebp;
      pCurrentTask->task.esi = pRegs->esi;
      pCurrentTask->task.edi = pRegs->edi;

      pCurrentTask->task.cs = pRegs->cs;
      pCurrentTask->task.ds = pRegs->ds;
      pCurrentTask->task.es = pRegs->es;
      pCurrentTask->task.fs = pRegs->fs;
      pCurrentTask->task.gs = pRegs->gs;

      pCurrentTask->task.eflags = pRegs->eflags;
      pCurrentTask->task.eip = pRegs->eip;
      pCurrentTask->task.esp = pRegs->esp;
      pCurrentTask->task.ss = pRegs->ss;
      //k_printRegs(pRegs);
      //while(1);
   }

   //k_printf("switching to task: 0x%x\n", pNextTask->taskID);
   pCurrentTask = pNextTask;

   asm volatile("mov %0,%%cr3": :"r" (pNextTask->task.pdbr));

   pRegs->eax = pCurrentTask->task.eax;
   pRegs->ebx = pCurrentTask->task.ebx;
   pRegs->ecx = pCurrentTask->task.ecx;
   pRegs->edx = pCurrentTask->task.edx;

   pRegs->ebp = pCurrentTask->task.ebp;
   pRegs->esi = pCurrentTask->task.esi;
   pRegs->edi = pCurrentTask->task.edi;

   pRegs->cs = pCurrentTask->task.cs;
   pRegs->ds = pCurrentTask->task.ds;
   pRegs->es = pCurrentTask->task.es;
   pRegs->fs = pCurrentTask->task.fs;
   pRegs->gs = pCurrentTask->task.gs;

   pRegs->eip = pCurrentTask->task.eip;
   pRegs->eflags = pCurrentTask->task.eflags;
   pRegs->esp = pCurrentTask->task.esp;
   pRegs->ss = pCurrentTask->task.ss;

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
   //k_printf("scheduler (count: %i)\n", readyTaskQueue.Count());

   if (readyTaskQueue.Count() == 0)  // the idle task is always ready to run
   {
      k_printf("no ready tasks!\n");
      while(1);
   }

   // pick the next task to run - don't remove it, just point to it
   //task_t* pNextTask = readyTaskQueue[0];
   // for now, move it to the back of the list (poor man's scheduling)
   task_t* pNextTask = readyTaskQueue.PopFront();
   readyTaskQueue.PushBack(pNextTask);

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
   readyTaskQueue.PushBack(pTask);
}

void k_setTaskAsPending(task_t* pTask, taskState state)
{
   //k_printf("Setting task as pending\n");
   // change the state of the task
   pTask->state = state;

   // remove it from the ready queue
   for (unsigned int i=0; i < readyTaskQueue.Count(); i++)
   {
      if (readyTaskQueue[i]->taskID == pTask->taskID)
      {
         if (readyTaskQueue.Delete(i))
         {
            return;
         }   
      }
   }
}

task_t* k_getTask(unsigned int taskID)
{
   // look for this ID in the all task list
   for (unsigned int i=0; i < allTaskList.Count(); i++)
   {
      if (allTaskList[i]->taskID == taskID)
      {
         return allTaskList[i];
      }
   }

   return NULL;
}

task_t* k_getTask(const char* name)
{
   // look for this name in the all task list
   for (unsigned int i=0; i < allTaskList.Count(); i++)
   {
      if (!k_strcmp(allTaskList[i]->name, name))
      {
         return allTaskList[i];
      }
   }

   return NULL;
}

unsigned int k_getCurrentTaskID()
{
   //k_printf("ptr: 0x%x\n", pCurrentTask);
   //k_printf("id: 0x%x\n", pCurrentTask->taskID);
   return pCurrentTask->taskID;
}

void k_getTaskMessage(KOSMessage* pMsg)
{
   //k_printf("k_getTaskMessage\n");

   // find the task
   task_t* pTask = k_getCurrentTask();
   if (!pTask)
   {
      k_printf("k_getTaskMessage: can't find current task\n");
      return;
   }
   //k_printf("current id: 0x%x\n", pTask->taskID);

   if (pTask->msgQueue.Count() == 0)
   {
      k_setTaskAsPending(pTask, TASK_STATE_BLOCKED_ON_MSG_QUEUE);

      // this task is now pending, so get the scheduler to pick a new one
      __ASM("int $48\n");
   }

   KOSMessage* pTemp = pTask->msgQueue.PopFront();
   pMsg->message = pTemp->message;
   pMsg->param1 = pTemp->param1;
   pMsg->param2 = pTemp->param2;
   delete pTemp;
}

unsigned char k_getTaskBasePriority(unsigned int taskID)
{
   return k_getTask(taskID)->priority;
}

void k_setTaskPriority(unsigned int taskID, unsigned char base)
{
   task_t* pTask = k_getTask(taskID);
   if (!pTask)
   {
      return;
   }

   pTask->priority = base;
}

void k_boostTaskPriority(unsigned int taskID, taskPriorityBoost boost)
{
   task_t* pTask = k_getTask(taskID);
   if (!pTask)
   {
      return;
   }

   k_printf("boosting priority\n");
   pTask->priority -= boost;
}

void k_unboostTaskPriority(unsigned int taskID, taskPriorityBoost boost)
{
   task_t* pTask = k_getTask(taskID);
   if (!pTask)
   {
      return;
   }

   k_printf("unboosting priority\n");
   pTask->priority += boost;
}

void k_sleep(unsigned int taskID, unsigned int ms)
{
   task_t* pTask = k_getTask(taskID);
   if (!pTask)
   {
      while(1);
   }

   k_setTaskAsPending(pTask, TASK_STATE_SLEEPING);

	// set up the timer
   k_startTimer(ms, NULL, NULL, taskID);

 	__ASM("int $48\n");	// give up the timeslice
}

extern "C" void k_cooperate(regs_t* pRegs)
{
	//k_printf("task 0x%x is now blocked\n", pCurrentTask->taskID);

	k_scheduler(pRegs);
}

