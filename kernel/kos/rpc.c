// Exposes kernel functions for IPC/RPC
// (Interprocess communication/Remote Procedure Call)

#include "stdio.h"
#include "rpc.h"
#include "kstub.h"      // __USE_DOS
#include "gdt.h"

extern unsigned short csos_segment;
extern unsigned int __RPC_GetServerHandle(char* name);
extern unsigned int __RPC_CallFunction(RPCServerHandle* pSrv, unsigned int ordinal, RPCParams* pParams);

unsigned int RPC_CallFunction(RPCServerHandle* pSrv, unsigned int ordinal, RPCParams* pParams)
{
   // change to OS data segment
   __USE_DSOS();

   unsigned int callGate[2];
   int retval = 0;

   // call the kernel function
   callGate[0] = (unsigned int)__RPC_CallFunction;
   callGate[1] = SEGMENT_INDEX(csos_segment, 0, PRIVILEGE_LEVEL_USER);
   pParams += 0x1000000;

   __ASM("pushl %0\n" :: "m" (pParams));
   __ASM("pushl %0\n" :: "m" (ordinal));
   __ASM("pushl %0\n" :: "m" (pSrv));
   __ASM("lcall *%0" : : "m" (*callGate));
   __ASM("add $0x10, %esp\n");
   __ASM("movl %%eax, %0" :: "am" (retval));

   // change data segment back
   __USE_DSAPP();

   // return what is in eax
   return retval;
}

RPCServerHandle* RPC_GetServerHandle(char* name)
{
   // change to OS data segment
   __USE_DSOS();

   unsigned int callGate[2];
   int retval = 0;

   // call the kernel function
   callGate[0] = (unsigned int)__RPC_GetServerHandle;
   callGate[1] = SEGMENT_INDEX(csos_segment, 0, PRIVILEGE_LEVEL_USER);
   name += 0x1000000;

   __ASM("pushl %0\n" :: "m" (name));
   __ASM("lcall *%0" : : "m" (*callGate));
   __ASM("add $0x10, %esp\n");
   __ASM("movl %%eax, %0" :: "am" (retval));

   // change data segment back
   __USE_DSAPP();

   // return what is in eax
   return (RPCServerHandle*)retval;
}

