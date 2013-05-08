#ifndef _L4__H
#define _L4__H

typedef unsigned char uint8;
typedef unsigned int uint32;
typedef enum bool { FALSE, TRUE } bool;

typedef uint32 L4_CPtr;
typedef uint32 L4_MessageInfo;
typedef uint32 L4_VMAttributes;

typedef uint32 L4_Page;
typedef uint32 L4_PageTable;
typedef uint32 L4_PageDirectory;

typedef void* L4_Memory;
typedef void* L4_CapData;

/**
 * @brief Capability Access Rights
 *
 * The rights associated with a capability determine the operations that
 * can be performed when the capability is invoked.
 */
typedef uint8 L4_CapRights;
#define L4_CAP_RIGHTS_READ    (1<<2)
#define L4_CAP_RIGHTS_WRITE   (1<<1)
#define L4_CAP_RIGHTS_GRANT   (1<<0)

typedef L4_CPtr L4_TCB;
typedef L4_CPtr L4_CNode;
typedef L4_CPtr L4_IRQControl;
typedef L4_CPtr L4_IRQHandler;
typedef L4_CPtr L4_Untyped;

#ifdef ARCH_IA32
typedef L4_CPtr L4_IA32_ASIDControl;
typedef L4_CPtr L4_IA32_ASIDPool;
typedef L4_CPtr L4_IA32_IOSpace;
typedef L4_CPtr L4_IA32_IOPort;
typedef L4_CPtr L4_IA32_Page;
typedef L4_CPtr L4_IA32_PageDirectory;
typedef L4_CPtr L4_IA32_PageTable;
typedef L4_CPtr L4_IA32_IOPageTable;
#endif

typedef struct L4_CapDescriptor
{
   L4_CNode service;
   uint32 index;
   uint8 depth;
} L4_CapDescriptor;

typedef struct L4_UserContext
{
   uint32 eip;
   uint32 esp;
   uint32 eflags;
   uint32 eax;
   uint32 ebx;
   uint32 ecx;
   uint32 edx;
   uint32 esi;
   uint32 edi;
   uint32 ebp;
   uint32 tls_base;
   uint32 fs;
   uint32 gs;
} L4_UserContext;

/* IPC */
inline void L4_Send(L4_CPtr dest, L4_MessageInfo msgInfo);
inline L4_MessageInfo L4_Wait(L4_CPtr src, uint32* sender);
inline L4_MessageInfo L4_Call(L4_CPtr dest, L4_MessageInfo msgInfo);
inline void L4_Reply(L4_MessageInfo msgInfo);
inline void L4_SendNB(L4_CPtr dest, L4_MessageInfo msgInfo);
inline L4_MessageInfo L4_ReplyWait(L4_CPtr dest, L4_MessageInfo msgInfo, uint32* sender);
inline void L4_Yield(void);
inline void L4_Notify(L4_CPtr dest, uint32 msg);

/* Capability Nodes */
int L4_CNode_Copy(L4_CapDescriptor* dest, L4_CapDescriptor* src, L4_CapRights rights);
int L4_CNode_Delete(L4_CapDescriptor* cap);
int L4_CNode_Mint(L4_CapDescriptor* dest, L4_CapDescriptor* src, L4_CapRights rights, L4_CapData badge);
int L4_CNode_Move(L4_CapDescriptor* dest, L4_CapDescriptor* src);
int L4_CNode_Mutate(L4_CapDescriptor* dest, L4_CapDescriptor* src, L4_CapData badge);
int L4_CNode_Recycle(L4_CapDescriptor* cap);
int L4_CNode_Revoke(L4_CapDescriptor* cap);
inline int L4_CNode_Rotate(L4_CapDescriptor* dest, L4_CapData destBadge,
            L4_CapDescriptor* pivot, L4_CapData pivotBadge, L4_CapDescriptor* src);
inline int L4_CNode_SaveCaller(L4_CapDescriptor* cap);

/* Debug */
inline void L4_DebugHalt(void);
static inline void L4_DebugPutChar(char c)
{
    asm volatile (
        "pushl %%ebp       \n"
        "movl %%ecx, %%ebp \n"
        "movl %%esp, %%ecx \n"
        "leal 1f, %%edx    \n"
        "1:                \n"
        "sysenter          \n"
        "popl %%ebp        \n"
        :
        : "a" (c),
          "b" (1),
          "S" (2),
          "D" (3),
          "c" (4)
        : "%edx"
    );
}

/* IRQ */
inline int L4_IRQControl_Get(L4_IRQControl service, int irq, L4_CNode root, uint32 index, uint8 depth);
inline int L4_IRQHandler_Ack(L4_IRQHandler service);
inline int L4_IRQHandler_Clear(L4_IRQHandler service);
inline int L4_IRQHandler_SetEndpoint(L4_IRQHandler service, L4_CPtr endpoint);

/* TCB */
inline int L4_TCB_Configure(L4_TCB service, uint32 faultEP, uint8 priority, L4_CNode cspaceRoot, L4_CapData cspaceRootData, L4_CNode vspaceRoot, L4_CapData vspaceRootData, uint32 buffer, L4_CPtr bufferFrame);
inline int L4_TCB_CopyRegisters(L4_TCB service, L4_TCB source, bool suspend, bool resume, bool transFrame, bool transInt, uint8 flags);
inline int L4_TCB_ReadRegisters(L4_TCB service, bool suspend, uint8 flags, uint32 count, L4_UserContext* regs);
inline int L4_TCB_Resume(L4_TCB service);
inline int L4_TCB_SetIPCBuffer(L4_TCB service, uint32 buffer, L4_CPtr bufferFrame);
inline int L4_TCB_SetPriority(L4_TCB service, uint8 priority);
inline int L4_TCB_SetSpace(L4_TCB service, uint32 faultEP, L4_CNode cspaceRoot, L4_CapData cspaceRootData, L4_CNode vspaceRoot, L4_CapData vspaceRootData);
inline int L4_TCB_Suspend(L4_TCB service);
inline int L4_TCB_WriteRegisters(L4_TCB service, bool resume, uint8 flags, uint32 count, L4_UserContext* regs);
L4_Memory L4_UntypedRetype(L4_Untyped service, int type, int sizeBits, L4_CNode root, int nodeIndex, int nodeDepth, int nodeOffset, int nodeWindow);

#ifdef ARCH_IA32
/* IO space */
inline int L4_IA32_ASIDControl_MakePool(L4_IA32_ASIDControl service, L4_Untyped untyped, L4_CNode root, uint32 index, uint8 depth);
inline int L4_IA32_ASIDPool_Assign(L4_IA32_ASIDPool service, L4_IA32_PageDirectory vroot);
inline uint8 L4_IA32_IOPort_In8(L4_IA32_IOPort service, uint16 port);
inline uint16 L4_IA32_IOPort_In16(L4_IA32_IOPort service, uint16 port);
inline uint32 L4_IA32_IOPort_In32(L4_IA32_IOPort service, uint16 port);
inline uint8 L4_IA32_IOPort_Out8(L4_IA32_IOPort service, uint16 port, uint8 data);
inline uint8 L4_IA32_IOPort_Out16(L4_IA32_IOPort service, uint16 port, uint16 data);
inline uint8 L4_IA32_IOPort_Out32(L4_IA32_IOPort service, uint16 port, uint32 data);
inline int L4_IA32_IOPageTable_Map(L4_IA32_IOPageTable service, L4_IA32_IOSpace iospace, uint32 ioaddr);
inline int L4_IA32_Page_MapIO(L4_IA32_Page service, L4_IA32_IOSpace iospace, L4_CapRights rights, uint32 ioaddr);
#endif

/* memory space */
inline int L4_Page_Map(L4_Page service, L4_PageDirectory pd, uint32 vaddr, L4_CapRights rights, L4_VMAttributes attr);
inline int L4_Page_Remap(L4_Page service, L4_PageDirectory pd, L4_CapRights rights, L4_VMAttributes attr);
inline int L4_Page_Unmap(L4_Page service);
inline int L4_PageTable_Map(L4_PageTable service, L4_PageTable pd, uint32 vaddr, L4_VMAttributes attr);

#ifdef ARCH_ARM
#endif

#endif // _L4__H

