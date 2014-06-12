////////////////////////////////////////////////////////////////////////////////
/// @file   ktask.c
/// @short  task related functions and data
/// @author J.Nider
/// @date   30/10/2009
////////////////////////////////////////////////////////////////////////////////

#include "kstdio.h"
#include "memory.h"                    // _BASE_ADDRESS
#include "task.h"                     // task_t, task_list
#include "eflags.h"
#include "l4.h"
#include "i386-elf.h"

extern L4_KIP* k_KIP;

static task_list allTaskList;   ///< list of all tasks
static task_list readyTaskQueue;///< queue of ready tasks
static task_t* pCurrentTask; ///< currently executing task
static task_t defaultTaskParams;       ///< new tasks copy their default values from here

static unsigned int nextTaskID=1;      ///< the last assigned task ID

static unsigned int createTaskID(void)
{
   int found=0;
   //unsigned int i;
   //unsigned int id;
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

   // add it to the all task list
   k_taskListAdd(&allTaskList, pTask);

   return pTask;
}

unsigned int k_createMemorySpace(void)
{
   int i;
   unsigned int pd = (unsigned int)k_allocPageDirectory();

   // map the kernel
   for (i=0; i < KERNEL_MEMORY_LIMIT; i+=PAGE_SIZE_4M)
      k_map4MPage((unsigned int*)pd, i, i, MEMORY_PAGE_TABLE_PRESENT | MEMORY_PAGE_TABLE_GLOBAL);

   return pd;
}

task_t* k_createThread(unsigned int memspace, unsigned int entry, unsigned int stack, unsigned int stackSize)
{
   int i;

   k_printf("k_createThread\n");
   k_printf("Entry point: 0x%x\n", entry);
   k_printf("Stack: 0x%x\n", stack);

   task_t* pTask = k_createTask();
   if (!pTask)
      return 0;

   // save known values
   pTask->segment.eip = entry;
   pTask->segment.esp = stack;

   // set up page directory
   pTask->segment.pdbr = memspace;

   // map the stack
   for (i=0; i < stackSize; i+= PAGE_SIZE)
   {
      k_map4KPage((unsigned int*)pTask->segment.pdbr, (unsigned int)k_allocUserPage(), (unsigned int)stack-stackSize+i,
         MEMORY_PAGE_WRITE | MEMORY_PAGE_USER_MODE);
   }

   // map the KIP
   k_printf("Task: map KIP from 0x%x to 0x%x\n", k_KIP, KERNEL_INTERFACE_PAGE);
   k_map4KPage((unsigned int*)pTask->segment.pdbr, (unsigned int)k_KIP, (unsigned int)KERNEL_INTERFACE_PAGE, MEMORY_PAGE_USER_MODE);

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

int k_loadELF(unsigned int pd, const char* buffer, unsigned int* entry)
{
   int i, j;
   //k_printf("k_loadELF %i\n", pd);

   Elf32_Ehdr* elfHeader = (Elf32_Ehdr*)buffer;

   if (!BOOTABLE_I386_ELF((*elfHeader)))
   {
      k_printf("not executable i386 ELF\n");
      return 1;
   }

   // set entry point & stack
   *entry = elfHeader->e_entry;

   // look for the program headers
   if (elfHeader->e_phnum > 16)
   {
      k_printf("You have got too many program headers\n");
      return 2;
   }

   Elf32_Phdr* pHeader = (Elf32_Phdr*)(elfHeader->e_phoff + buffer);
   for (i=0; i < elfHeader->e_phnum; i++)
   {
      //k_printf("ptype: %i\n", pHeader->p_type);
      if (pHeader->p_type == PT_LOAD)
      {
         // allocate the memory needed for this section and copy the program, one page at a time
         for (j=0; j < pHeader->p_filesz; j+= PAGE_SIZE)
         {
            if (!k_isMapped(pHeader->p_vaddr + j))
            {
               void* userPage = k_allocUserPage();
               k_map4KPage((unsigned int*)pd, (unsigned int)userPage, pHeader->p_vaddr + j, MEMORY_PAGE_USER_MODE);
            }

            // copy the memory to a new physical location (aligned to 4k page)
            k_memcpy((void*)(pHeader->p_vaddr + j), (void*)(buffer + pHeader->p_offset + j), MIN(PAGE_SIZE, pHeader->p_filesz));
         }
      }

      pHeader++;
   }
}

