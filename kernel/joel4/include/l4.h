#ifndef _L4__H
#define _L4__H

typedef unsigned char uint8;
typedef unsigned int uint32;
typedef enum bool { FALSE, TRUE } bool;

typedef uint32 L4_CPtr;
typedef uint32 L4_Word;
typedef uint32 L4_MessageInfo;

typedef void* L4_TCB;
typedef void* L4_CNode;
typedef void* L4_CapData;
typedef void* L4_CapRights;
typedef void* L4_IRQControl;
typedef void* L4_IRQHandler;
typedef void* L4_Memory;
typedef void* L4_Untyped;

typedef struct L4_UserContext
{
   L4_Word asdf;
} L4_UserContext;

/* IPC */
static inline void L4_Send(L4_CPtr dest, L4_MessageInfo msgInfo);
static inline L4_MessageInfo L4_Wait(L4_CPtr src, L4_Word* sender);
static inline L4_MessageInfo L4_Call(L4_CPtr dest, L4_MessageInfo msgInfo);
static inline void L4_Reply(L4_MessageInfo msgInfo);
static inline void L4_SendNB(L4_CPtr dest, L4_MessageInfo msgInfo);
static inline L4_MessageInfo L4_ReplyWait(L4_CPtr dest, L4_MessageInfo msgInfo, L4_Word* sender);
static inline void L4_Yield(void);
static inline void L4_Notify(L4_CPtr dest, L4_Word msg);

/* CNode */
int L4_CNode_Copy(L4_CNode service, L4_Word destIndex, uint8 destDepth, L4_CNode srcRoot, L4_Word srcIndex, uint8 srcDepth, L4_CapRights rights);
static inline int L4_CNode_Delete(L4_CNode service, L4_Word index, uint8 depth);
static inline int L4_CNode_Mint(L4_CNode service, L4_Word destIndex, uint8 destDepth, L4_CNode srcRoot, L4_Word srcIndex, uint8 srcDepth, L4_CapRights rights, L4_CapData badge);
int L4_CNode_Move(L4_CNode service, L4_Word destIndex, uint8 destDepth, L4_CNode srcRoot, L4_Word srcIndex, uint8 srcDepth);
static inline int L4_CNode_Mutate(L4_CNode service, L4_Word destIndex, uint8 destDepth, L4_CNode srcRoot, L4_Word srcIndex, uint8 srcDepth, L4_CapData badge);
static inline int L4_CNode_Recycle(L4_CNode service, L4_Word index, uint8 depth);
static inline int L4_CNode_Revoke(L4_CNode service, L4_Word index, uint8 depth);
static inline int L4_CNode_Rotate(L4_CNode service, L4_Word destIndex, uint8 destDepth, L4_CapData destBadge, L4_CNode pivotRoot, L4_Word pivotIndex, uint8 pivotDepth, L4_CapData pivotBadge, L4_CNode srcRoot, L4_Word srcIndex, uint8 srcDepth);
static inline int L4_CNode_SaveCaller(L4_CNode service, L4_Word index, uint8 depth);

/* Debug */
static inline void L4_DebugHalt(void);
static inline void L4_DebugPutChar(char c);

/* IRQ */
static inline int L4_IRQControl_Get(L4_IRQControl service, int irq, L4_CNode root, L4_Word index, uint8 depth);
static inline int L4_IRQHandler_Ack(L4_IRQHandler service);
static inline int L4_IRQHandler_Clear(L4_IRQHandler service);
static inline int L4_IRQHandler_SetEndpoint(L4_IRQHandler service, L4_CPtr endpoint);

/* TCB */
static inline int L4_TCB_Configure(L4_TCB service, L4_Word faultEP, uint8 priority, L4_CNode cspaceRoot, L4_CapData cspaceRootData, L4_CNode vspaceRoot, L4_CapData vspaceRootData, L4_Word buffer, L4_CPtr bufferFrame);
static inline int L4_TCB_CopyRegisters(L4_TCB service, L4_TCB source, bool suspend, bool resume, bool transFrame, bool transInt, uint8 flags);
static inline int L4_TCB_ReadRegisters(L4_TCB service, bool suspend, uint8 flags, L4_Word count, L4_UserContext* regs);
static inline int L4_TCB_Resume(L4_TCB service);
static inline int L4_TCB_SetIPCBuffer(L4_TCB service, L4_Word buffer, L4_CPtr bufferFrame);
static inline int L4_TCB_SetPriority(L4_TCB service, uint8 priority);
static inline int L4_TCB_SetSpace(L4_TCB service, L4_Word faultEP, L4_CNode cspaceRoot, L4_CapData cspaceRootData, L4_CNode vspaceRoot, L4_CapData vspaceRootData);
static inline int L4_TCB_Suspend(L4_TCB service);
static inline int L4_TCB_WriteRegisters(L4_TCB service, bool resume, uint8 flags, L4_Word count, L4_UserContext* regs);
L4_Memory L4_UntypedRetype(L4_Untyped service, int type, int sizeBits, L4_CNode root, int nodeIndex, int nodeDepth, int nodeOffset, int nodeWindow);

#ifdef ARCH_IA32
static inline int L4_IA32_ASIDControl_MakePool(L4_IA32_ASIDControl service, L4_Untyped untyped, L4_CNode root, L4_Word index, uint8 depth);
static inline int L4_IA32_ASIDPool_Assign(L4_IA32_ASIDPool service, L4_IA32_PageDirectory vroot);
static inline uint8 L4_IA32_IOPort_In8(L4_IA32_IOPort service, uint16 port);
static inline uint16 L4_IA32_IOPort_In16(L4_IA32_IOPort service, uint16 port);
static inline uint32 L4_IA32_IOPort_In32(L4_IA32_IOPort service, uint16 port);
static inline uint8 L4_IA32_IOPort_Out8(L4_IA32_IOPort service, uint16 port, uint8 data);
static inline uint8 L4_IA32_IOPort_Out16(L4_IA32_IOPort service, uint16 port, uint16 data);
static inline uint8 L4_IA32_IOPort_Out32(L4_IA32_IOPort service, uint16 port, uint32 data);
static inline int L4_IA32_IOPageTable_Map(L4_IA32_IOPageTable service, L4_IA32_IOSpace iospace, L4_Word ioaddr);
static inline int L4_IA32_Page_MapIO(L4_IA32_Page service, L4_IA32_IOSpace iospace, L4_CapRights rights, L4_Word ioaddr);
static inline int L4_IA32_Page_Map(L4_IA32_Page service, L4_IA32_PageDirectory pd, L4_Word vaddr, L4_CapRights rights, L4_IA32_VMAttributes attr);
static inline int L4_IA32_Page_Remap(L4_IA32_Page service, L4_IA32_PageDirectory pd, L4_CapRights rights, L4_IA32_VMAttributes attr);
static inline int L4_IA32_Page_Unmap(L4_IA32_Page service);
static inline int L4_IA32_PageTable_Map(L4_IA32_PageTable service, L4_IA32_PageTable pd, L4_Word vaddr, L4_IA32_VMAttributes attr);
#endif

#ifdef ARCH_ARM
#endif

#endif // _L4__H

