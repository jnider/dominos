#ifndef _PROCESS__H
#define _PROCESS__H

typedef void(*pFnMain_t)(void*);

struct process_options_t
{
   bool  startSuspended;
   unsigned char priority;
   unsigned int stackSize;
   pFnMain_t mainFn;
   unsigned int cs;
   unsigned int ds;
   void* pParam;
};


unsigned int CreateProcess(const process_options_t* pOptions);

#endif // _PROCESS__H
