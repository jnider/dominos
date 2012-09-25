#include "timer.h"
#include "kstub.h"
#include "kstdio.h"
#include "ktimer.h"
#include "task.h"       // GetTaskID

extern unsigned int csos_segment;

//extern void __setScheduler(pSchedulerFN pScheduler, unsigned int time);
extern int __startTimer(unsigned int ms, timer_callback cb, void* param, unsigned int taskID);
extern int __stopTimer(int timerId);
extern unsigned int __getTickCount(void);
extern unsigned short __getTickDivisor(void);

/*
void SetScheduler(pSchedulerFN pScheduler, unsigned int time)
{
   // change to OS data segment
   __USE_DSOS();

   unsigned int callGate[2];

   // call the kernel function
   callGate[0] = (unsigned int)__setScheduler;
   callGate[1] = SEGMENT_INDEX(csos_segment, 0, 3); //0x1b;

   // push parameters in reverse order
   __ASM("pushl %0\n" :: "m" (time));
   __ASM("pushl %0\n" :: "m" (pScheduler));
   __ASM("lcall *%0" : : "m" (*callGate));
   __ASM("add $0x10, %esp\n");

   // change data segment back
   __USE_DSAPP();
}
*/

int StartTimer(unsigned int ms, timer_callback cb, void* param)
{
   // since this is a userspace function, it must be called before changing to DSOS
   unsigned int taskID = GetTaskID();

   // change to OS data segment
   __USE_DSOS();

   unsigned int callGate[2];
   int retval;

   //printf("StartTimer taskID: 0x%x\n", taskID);

   // call the kernel function
   callGate[0] = (unsigned int)__startTimer;
   callGate[1] = SEGMENT_INDEX(csos_segment, 0, 3); //0x1b;

   // parameters are pushed in reverse order so they look right on the stack (which grows down)
   __ASM("pushl %0\n" :: "m" (taskID));
   __ASM("pushl %0\n" :: "m" (param));
   __ASM("pushl %0\n" :: "m" (cb));
   __ASM("pushl %0\n" :: "m" (ms));
   __ASM("lcall *%0" : : "m" (*callGate));
   __ASM("add $0x10, %esp\n");
   __ASM("movl %%eax, %0" :: "am" (retval));

   // change data segment back
   __USE_DSAPP();

   // return what is in eax - the returned value from __startTimer
   return retval;
}

int StopTimer(int timerId)
{
   // change to OS data segment
   __USE_DSOS();

   unsigned int callGate[2];
   int retval;

   //printf("StartTimer taskID: 0x%x\n", taskID);

   // call the kernel function
   callGate[0] = (unsigned int)__stopTimer;
   callGate[1] = SEGMENT_INDEX(csos_segment, 0, 3); //0x1b;

   // parameters are pushed in reverse order so they look right on the stack (which grows down)
   __ASM("pushl %0\n" :: "m" (timerId));
   __ASM("lcall *%0" : : "m" (*callGate));
   __ASM("add $0x10, %esp\n");
   __ASM("movl %%eax, %0" :: "am" (retval));

   // change data segment back
   __USE_DSAPP();

   // return what is in eax - the returned value from __startTimer
   return retval;
}

unsigned int GetTickCount(void)
{
   // change to OS data segment
   __USE_DSOS();

   unsigned int callGate[2];
   unsigned int retval;

   // call the kernel function
   callGate[0] = (unsigned int)__getTickCount;
   callGate[1] = SEGMENT_INDEX(csos_segment, 0, 3); //0x1b;

   __ASM("lcall *%0" : : "m" (*callGate));
   __ASM("add $0x10, %esp\n");
   __ASM("movl %%eax, %0" :: "am" (retval));

   // change data segment back
   __USE_DSAPP();

   // return what is in eax
   return retval;
}

unsigned short GetTickDivisor(void)
{
   // change to OS data segment
   __USE_DSOS();

   unsigned int callGate[2];
   unsigned short retval;

   // call the kernel function
   callGate[0] = (unsigned int)__getTickDivisor;
   callGate[1] = SEGMENT_INDEX(csos_segment, 0, 3); //0x1b;

   __ASM("lcall *%0" : : "m" (*callGate));
   __ASM("add $0x10, %esp\n");
   __ASM("mov %%ax, %0" :: "am" (retval));

   // change data segment back
   __USE_DSAPP();

   // return what is in eax
   return retval;
}

