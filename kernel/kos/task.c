#include "task.h"
#include "kstub.h"
#include "ksegments.h"
#include "gdt.h"  // PRIVILEGE_LEVEL_

extern unsigned int csos_segment;

// stubs - don't call these!
extern unsigned int __getTaskID(void);
extern void __getMessage(unsigned int taskID, KOSMessage* pMsg);
extern void __sleep(unsigned int taskID, unsigned int ms);
extern void __registerForKeyPress(unsigned int taskID);

unsigned int GetTaskID(void)
{
   // change to OS data segment
   __USE_DSOS();

   unsigned int callGate[2];
   int retval;

   // call the kernel function
   callGate[0] = (unsigned int)__getTaskID;
   callGate[1] = SEGMENT_INDEX(csos_segment, 0, PRIVILEGE_LEVEL_USER); //0x1b;

   __ASM("lcall *%0" : : "m" (*callGate));
   __ASM("add $0x10, %esp\n");
   __ASM("movl %%eax, %0" :: "am" (retval));

   // change data segment back
   __USE_DSAPP();

   return retval;
}

void GetMessage(KOSMessage* pMsg)
{
   // since this is a userspace function, it must be called before changing to DSOS
   //unsigned int taskID = GetTaskID();

   // change to OS data segment
   __USE_DSOS();

   unsigned int callGate[2];
   unsigned int temp;

   temp = (unsigned int)pMsg + DSAPP_BASE_ADDRESS;

   // call the kernel function
   callGate[0] = (unsigned int)__getMessage;
   callGate[1] = SEGMENT_INDEX(csos_segment, 0, 3); //0x1b;

   __ASM("pushl %0\n" :: "m" (temp));
   //__ASM("pushl %0\n" :: "m" (taskID));
   __ASM("lcall *%0" : : "m" (*callGate));
   __ASM("add $0x10, %esp\n");
   //__ASM("movl %%eax, %0" :: "am" (retval));

   // change data segment back
   __USE_DSAPP();
}

void Sleep(unsigned int ms)
{
   /*
   // since this is a userspace function, it must be called before changing to DSOS
   unsigned int taskID = GetTaskID();

   // change to OS data segment
   __USE_DSOS();

   unsigned int callGate[2];

   // call the kernel function
   callGate[0] = (unsigned int)__sleep;
   callGate[1] = SEGMENT_INDEX(csos_segment, 0, 3); //0x1b;

   __ASM("pushl %0\n" :: "m" (taskID));
   __ASM("lcall *%0" : : "m" (*callGate));
   __ASM("add $0x10, %esp\n");

   // change data segment back
   __USE_DSAPP();
   */

   //StartTimer(ms, SleepCB, NULL);

   __ASM("int $48\n");  // special interrupt to give up CPU time
}

void CallbackStub(void)
{
   __ASM("int $48\n");  // special interrupt to give up CPU time
}

void RegisterForKeyPress(void)
{
   // since this is a userspace function, it must be called before changing to DSOS
   unsigned int taskID = GetTaskID();

   // change to OS data segment
   __USE_DSOS();

   unsigned int callGate[2];

   // call the kernel function
   callGate[0] = (unsigned int)__registerForKeyPress;
   callGate[1] = SEGMENT_INDEX(csos_segment, 0, 3); //0x1b;

   __ASM("pushl %0\n" :: "m" (taskID));
   __ASM("lcall *%0" : : "m" (*callGate));
   __ASM("add $0x10, %esp\n");

   // change data segment back
   __USE_DSAPP();
}
