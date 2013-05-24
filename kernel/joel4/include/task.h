////////////////////////////////////////////////////////////////////////////////
///@file    ktask.h
///@short   task management API
///@author  J.Nider
///@date    30/10/2009
////////////////////////////////////////////////////////////////////////////////

#ifndef _KTASK__H
#define _KTASK__H

#include "l4.h"
#include "gdt.h"
#include "isr.h"        // regs_t

#define MAX_TASKS    10 ///< how many tasks are allowed to run simultaneously
#define MAX_TASK_NAME   15 ///< maximum length of task name

enum taskPriorityBoost
{
   TASK_PRIORITY_BOOST_NONE,
   TASK_PRIORITY_BOOST_TIMER
};

////////////////////////////////////////////////
/// @brief Is this task running or not
////////////////////////////////////////////////
typedef enum taskState
{
   TASK_STATE_READY,
   TASK_STATE_SLEEPING,
   TASK_STATE_BLOCKED_ON_MSG_QUEUE,
   TASK_STATE_BLOCKED_ON_RPC
} taskState;

// pointer to entry point for the task
typedef void(*pFnMain_t)(void*);

////////////////////////////////////////////////
/// @brief all the things that are unique to a task
///
/// This structure completely describes a task.  A task is a unit of execution
/// AKA process or thread.  I do not use the terms process or thread in KOS,
/// because there are connotations of scheduling expectations and memory usage
/// associated with those terms.  Here, a task has its own memory space and
/// its own message queue for incoming requests from other tasks.  Tasks also
/// have a state which tells us if it is running or not, and if not, why.
////////////////////////////////////////////////
typedef struct task_t
{
   tss_t segment;
   char name[MAX_TASK_NAME+1];      ///< task name used for RPC
   unsigned int taskID;             ///< task identifier
   unsigned int membreak;           ///< the end of the logical memory
   unsigned int affinity;           ///< which processor this task wants to run on
   unsigned char priority;          ///< task priority level
   taskState state;                 ///< task state (see TASK_STATE_)
} task_t;

typedef struct task_node
{
   struct task_node* pNext;
   task_t* pData;
} task_node;

typedef struct task_list
{
   int count;
   task_node* head;
} task_list;

static inline void k_taskListInit(task_list* list)
{
   list->count = 0;
   list->head = 0;
}

static inline int k_taskListAdd(task_list* list, task_t* t)
{
   k_printf("k_taskListAdd\n");
   task_node* pNewNode = (task_node*)kmalloc(sizeof(task_node));

   /* if the memory allocation fails, return */
   if (!pNewNode)
      return 0;

   /* fill in the new node */
   k_printf("create node at 0x%x\n", pNewNode);
   pNewNode->pNext = 0;
   pNewNode->pData = t;

   task_node* pLast = list->head;

   /* simple case: if this is the first node, set it */
   k_printf("list head: 0x%x\n", list->head);
   if (!list->head)
   {
      list->head = pNewNode;
      return ++list->count;
   }

   /* otherwise, run through the list until we hit the end */
   k_printf("search\n");
   while (pLast->pNext)
      pLast = pLast->pNext;

   /* set the new last node */
   k_printf("set last node\n");
   pLast->pNext = pNewNode;
   return ++list->count;
}

static inline int k_taskListCount(task_list* list)
{
   return list->count;
}

static inline task_t* k_taskListHead(task_list* list)
{
   return list->head->pData;
}

static inline k_taskListDeleteByID(task_list* list, int id)
{
   unsigned int i;
   task_node* pNode = list->head;
   task_node* pPrev = list->head;

   /* make sure there is a node */
   if (!pNode)
      return;

   /* head node is a special case */
   if (pNode->pData->taskID == id)
   {
      list->head = pNode->pNext;
      return;
   }

   /* general case */
   pNode = pNode->pNext;
   while (pNode)
   {
      if (pNode->pData->taskID == id)
      {
         pPrev->pNext = pNode->pNext;
         kfree(pNode);
         return;
      }
      pNode = pNode->pNext;
   }
}

static inline task_t* k_taskListFindByID(task_list* list, int id)
{
   unsigned int i;
   task_node* pNode = list->head;

   /* make sure there is a node */
   if (!pNode)
      return;

   while (pNode)
   {
      if (pNode->pData->taskID == id)
      {
         return pNode->pData;
      }
      pNode = pNode->pNext;
   }
   return 0;
}

static inline task_t* k_taskListFindByName(task_list* list, const char* name)
{
   unsigned int i;
   task_node* pNode = list->head;

   /* make sure there is a node */
   if (!pNode)
      return;

   while (pNode)
   {
      if (k_strcmp(pNode->pData->name, name) == 0)
      {
         return pNode->pData;
      }
      pNode = pNode->pNext;
   }
   return 0;
}

extern task_t osTask;

// kernel mode only!
////////////////////////////////////////////////////////////////////////////////
/// @author J.Nider
/// @date   21-Apr-2010 10:48:58 AM
/// @brief  initializes the task library
////////////////////////////////////////////////////////////////////////////////
void k_initTask(unsigned short codeSeg, unsigned short dataSeg, unsigned short stackSegInt, unsigned int intstack);

task_t* k_createTask(void);              ///< creates a new task
task_t* k_createThread(task_t* pTask, uint32* code, uint32 codeSize, uint32* data, uint32 dataSize, uint32 entryPoint);              ///< creates a new thread
void k_setTaskAsPending(task_t* pTask, taskState state);    ///< adds a task to the pending list
void k_setTaskAsReady(task_t* pTask);                       ///< adds a task to the ready list
task_t* k_getTaskByID(unsigned int taskID);                 ///< gets a task by its id
task_t* k_getTask(const char* name);                        ///< gets a task by its name
task_t* k_getCurrentTask();                                 ///< gets the current task
void k_setCurrentTask(task_t* pTask);                       ///< use this sparingly - overrides the scheduler/dispatcher
unsigned int k_getCurrentTaskID();                          ///< gets the id of the current task
//void k_getTaskMessage(KOSMessage* pMsg);///< gets the first message of the specified task
unsigned char k_getTaskBasePriority(unsigned int taskID);   ///< returns base priority of a task
void k_setTaskPriority(unsigned int taskID, unsigned char base);
void k_boostTaskPriority(unsigned int taskID, enum taskPriorityBoost boost);
void k_unboostTaskPriority(unsigned int taskID, enum taskPriorityBoost boost);
void k_switchTask(regs_t* pRegs, task_t* pNextTask);        ///< switches tasks
void k_scheduler(regs_t* pRegs);

void k_sleep(unsigned int taskID, unsigned int ms);
#endif // _KTASK__H

