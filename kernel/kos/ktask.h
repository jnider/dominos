////////////////////////////////////////////////////////////////////////////////
///@file    ktask.h
///@short   task management API
///@author  J.Nider
///@date    30/10/2009
////////////////////////////////////////////////////////////////////////////////

#ifndef _KTASK__H
#define _KTASK__H

#include "gdt.h"
//#include "klist.h"      // KPtrList
#include "kmessage.h"   // KOSMessage
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
   tss_t task;
   char name[MAX_TASK_NAME+1];      ///< task name used for RPC
   unsigned int taskID;             ///< task identifier
   unsigned int membreak;           ///< the end of the logical memory
   unsigned int affinity;           ///< which processor this task wants to run on
   unsigned char priority;          ///< task priority level
   taskState state;                 ///< task state (see TASK_STATE_)
   //KPtrList<KOSMessage> msgQueue;   ///< message queue
} task_t;

extern task_t osTask;

// kernel mode only!
////////////////////////////////////////////////////////////////////////////////
/// @author J.Nider
/// @date   21-Apr-2010 10:48:58 AM
/// @brief  initializes the task library
////////////////////////////////////////////////////////////////////////////////
void k_initTask(unsigned short codeSeg, unsigned short dataSeg, unsigned short stackSegInt);

task_t* k_createTask(unsigned int entryPoint);              ///< creates a new task
void k_setTaskAsPending(task_t* pTask, taskState state);    ///< adds a task to the pending list
void k_setTaskAsReady(task_t* pTask);                       ///< adds a task to the ready list
task_t* k_getTaskByID(unsigned int taskID);                 ///< gets a task by its id
task_t* k_getTask(const char* name);                        ///< gets a task by its name
task_t* k_getCurrentTask();                                 ///< gets the current task
void k_setCurrentTask(task_t* pTask);                       ///< use this sparingly - overrides the scheduler/dispatcher
unsigned int k_getCurrentTaskID();                          ///< gets the id of the current task
void k_getTaskMessage(KOSMessage* pMsg);///< gets the first message of the specified task
unsigned char k_getTaskBasePriority(unsigned int taskID);   ///< returns base priority of a task
void k_setTaskPriority(unsigned int taskID, unsigned char base);
void k_boostTaskPriority(unsigned int taskID, enum taskPriorityBoost boost);
void k_unboostTaskPriority(unsigned int taskID, enum taskPriorityBoost boost);
void k_switchTask(regs_t* pRegs, task_t* pNextTask);        ///< switches tasks
void k_scheduler(regs_t* pRegs);

void k_sleep(unsigned int taskID, unsigned int ms);
#endif // _KTASK__H

