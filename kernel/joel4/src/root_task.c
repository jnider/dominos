#include "l4.h"

/* This is the root task.  It executes in user space even though it is
   compiled as part of the kernel.  In fact, it is the first user space
   task created as part of the kernel initialization */

void root_task_main(void)
{
   char c = 'A';
   int i;

   for (i=0; i<10; i++)
      L4_DebugPutChar(c++);
}

