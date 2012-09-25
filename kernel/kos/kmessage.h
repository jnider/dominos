#ifndef _KMESSAGE__H
#define _KMESSAGE__H

#include "task.h"

///
/// post a message to a task's message queue
///
int k_postMessage(unsigned int taskID, KOSMessageValue message, unsigned int param1, unsigned int param2);

#endif // _KMESSAGE__H
