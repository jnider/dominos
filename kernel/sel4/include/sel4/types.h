/*
 * Copyright (C) 2010 National ICT Australia Limited (NICTA)
 *
 * For non-commercial use only. For license terms, see the file
 * LICENSE-OKL-NICTA.pdf in the top-level directory.
 */

#ifndef __SEL4TYPES_H__
#define __SEL4TYPES_H__

#include <sel4/macros.h>
#include <sel4/arch/types.h>

typedef enum {
    seL4_SysSend            = -1,
    seL4_SysNBSend          = -2,
    seL4_SysCall            = -3,
    seL4_SysWait            = -4,
    seL4_SysReply           = -5,
    seL4_SysReplyWait       = -6,
    seL4_SysYield           = -7,
    seL4_SysDebugPutChar    = -8,
    seL4_SysDebugHalt       = -9,
    seL4_SysDebugSnapshot   = -10,
    SEL4_FORCE_LONG_ENUM(seL4_Syscall_ID),
} seL4_Syscall_ID;

typedef enum {
    seL4_UntypedObject = 0,
    seL4_TCBObject,
    seL4_EndpointObject,
    seL4_AsyncEndpointObject,
    seL4_CapTableObject,
    seL4_NonArchObjectTypeCount,
    SEL4_FORCE_LONG_ENUM(seL4_ObjectType),
} seL4_ObjectType;

typedef enum {
    seL4_NoError = 0,
    seL4_InvalidArgument,
    seL4_InvalidCapability,
    seL4_IllegalOperation,
    seL4_RangeError,
    seL4_AlignmentError,
    seL4_FailedLookup,
    seL4_TruncatedMessage,
    seL4_DeleteFirst,
    seL4_RevokeFirst,
    SEL4_FORCE_LONG_ENUM(seL4_Error),
} seL4_Error;

typedef enum {
    seL4_NoFault = 0,
    seL4_CapFault,
    seL4_VMFault,
    seL4_UnknownSyscall,
    seL4_UserException,
    seL4_Interrupt,
    SEL4_FORCE_LONG_ENUM(seL4_FaultType),
} seL4_FaultType;

typedef enum {
    seL4_NoFailure = 0,
    seL4_InvalidRoot,
    seL4_MissingCapability,
    seL4_DepthMismatch,
    seL4_GuardMismatch,
    SEL4_FORCE_LONG_ENUM(seL4_LookupFailureType),
} seL4_LookupFailureType;

typedef enum {
    seL4_CanWrite = 0x01,
    seL4_CanRead = 0x02,
    seL4_CanGrant = 0x04,
    seL4_AllRights = seL4_CanWrite|seL4_CanRead|seL4_CanGrant,
    seL4_Transfer_Mint = 0x100,
    SEL4_FORCE_LONG_ENUM(seL4_CapRights),
} seL4_CapRights;

#if seL4_WordBits == 32
#define seL4_GuardSizeBits 5
#define seL4_GuardBits 18
#elif seL4_WordBits == 64
#define seL4_GuardSizeBits 6
#define seL4_GuardBits 48
#endif

#define seL4_BadgeBits (seL4_GuardBits + seL4_GuardSizeBits + 3)

typedef union {
    seL4_Word raw;
    struct {
        seL4_Word _padding : 3;
        seL4_Word guard_size : seL4_GuardSizeBits;
        seL4_Word guard_bits : seL4_GuardBits;
    } guard;
    seL4_Word badge : seL4_BadgeBits;
} seL4_CapData;

#define seL4_NilData (seL4_CapData)(seL4_Word)0

#define seL4_MsgLengthBits 7
#define seL4_MsgMaxLength 120
#define seL4_MsgExtraCapBits 2
#define seL4_MsgMaxExtraCaps ((1<<seL4_MsgExtraCapBits)-1)
#define seL4_LabelBits (seL4_WordBits-seL4_MsgLengthBits\
    -seL4_MsgExtraCapBits-seL4_MsgMaxExtraCaps)
#define seL4_UntypedRetypeMaxObjects 256

typedef union {
    struct {
        seL4_Word length : seL4_MsgLengthBits;
        seL4_Word extraCaps : seL4_MsgExtraCapBits;
        seL4_Word capsUnwrapped : seL4_MsgMaxExtraCaps;
        seL4_Word label : seL4_LabelBits;
    };
    seL4_Word raw;
} seL4_MessageInfo;

#define seL4_IPCBufferAlignment (sizeof(seL4_Word))<<(seL4_MsgLengthBits+1)

typedef struct {
    seL4_MessageInfo tag;
    seL4_Word msg[seL4_MsgMaxLength];
    seL4_Word reserved;
    union {
        seL4_CPtr caps[seL4_MsgMaxExtraCaps];
        seL4_CapData badges[seL4_MsgMaxExtraCaps];
    };
    seL4_CPtr receiveCNode;
    seL4_CPtr receiveIndex;
    seL4_Word receiveDepth;
    void* userPtr;
} seL4_IPCBuffer __attribute__ ((__aligned__ (seL4_IPCBufferAlignment)));

enum {
    seL4_NilCap = 0,
    seL4_SelfTCB,
    seL4_SelfCSpace,
    seL4_SelfVSpace,
    seL4_InterruptController,
    seL4_NonArchInitCapCount,
};

typedef struct {
    seL4_Word base;
    seL4_Word end;
    enum {
        seL4_Region_Empty = 0,
        seL4_Region_RootTask,
        seL4_Region_CapsOnly,
        seL4_Region_L1Node,
        seL4_Region_L2Node,
        seL4_Region_FreeSlots,
        seL4_Region_InitCaps,
        seL4_Region_SmallBlocks,
        seL4_Region_LargeBlocks,
        seL4_Region_DeviceCaps,
        seL4_RegionCount,
    } type;
    union {
        seL4_Word nodeRadix; /* L1Node and L2Node only */
        seL4_Word pageSize; /* SmallBlocks only */
        seL4_Word sizes; /* LargeBlocks only */
        seL4_Word deviceInfo; /* DeviceCaps only */
    };
} seL4_BootInfoRegion;

typedef struct {
    seL4_IPCBuffer* initIPCBuffer;
    unsigned int regionCount;
    seL4_BootInfoRegion regions[];
} seL4_BootInfo;

typedef seL4_CPtr seL4_CNode;
typedef seL4_CPtr seL4_IRQHandler;
typedef seL4_CPtr seL4_IRQControl;
typedef seL4_CPtr seL4_TCB;
typedef seL4_CPtr seL4_Untyped;

#include <sel4/arch/constants.h>

#endif /* __SEL4TYPES_H__ */
