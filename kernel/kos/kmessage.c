////////////////////////////////////////////////////////////////////////////////
///@file    kmessage.cpp
///@short   implementation of message passing between tasks
///@author  J.Nider
///@date    11-Jun-2007 03:48:06 PM
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "kmessage.h"
#include "ktask.h"      // task_t

void __getMessage(KOSMessage* pMsg)
{
   __ASM("add $4, %ebp\n");
   k_getTaskMessage(pMsg);
   __ASM("sub $4, %ebp\n");
   __KERNEL_RETURN();
}

/*
int k_postMessage(unsigned int taskID, KOSMessageValue message, unsigned int param1, unsigned int param2)
{
   k_printf("k_postMessage\n");

   // find the target task by ID
   task_t* pTask = k_getTask(taskID);

   if (!pTask)
   {
      // can't find the task!
      k_printf("can't find task 0x%x\n", taskID);
      while(1);
      return false;
   }

   // add the message to the queue
   //k_printf("creating message\n");
   KOSMessage* pMessage = new KOSMessage;
   pMessage->message = message;
   pMessage->param1 = param1;
   pMessage->param2 = param2;
   pTask->msgQueue.PushBack(pMessage);
   
   //k_printf("message added (%i, %i, %i)\n", message, param1, param2);

   if (pTask->state == TASK_STATE_BLOCKED_ON_MSG_QUEUE)
   {
      //k_printf("setting task 0x%x as ready\n", pTask->taskID);
      k_setTaskAsReady(pTask);
   }

   return true;
}
*/
