
/*
 * Automatically generated system call stubs.
 */

#ifndef _LIBSEL4__SEL4_CLIENT_H_
#define _LIBSEL4__SEL4_CLIENT_H_

#include <stdbool.h>
#include <stddef.h>

#include "sel4/types.h"
#include "sel4/functions.h"
#include "sel4/invocation.h"
#include "sel4/arch/syscalls.h"


/*
 * The following code generates a compile-time error if the system call
 * stub generator has an incorrect understanding of how large a type is.
 *
 * If you receive a compile-time error here, you will need to adjust
 * the type information in the stub generator.
 */
#define assert_size_correct(type, expected_bytes) \
        typedef unsigned long __type_##type##_size_incorrect[ \
                (sizeof(type) == expected_bytes) ? 1 : -1];

assert_size_correct(uint8_t, 1);
assert_size_correct(uint16_t, 2);
assert_size_correct(uint32_t, 4);
assert_size_correct(int, 4);
assert_size_correct(bool, 1);
assert_size_correct(seL4_Word, 4);
assert_size_correct(seL4_CapRights, 4);
assert_size_correct(seL4_CapData, 4);
assert_size_correct(seL4_CPtr, 4);
assert_size_correct(seL4_CNode, 4);
assert_size_correct(seL4_IRQHandler, 4);
assert_size_correct(seL4_IRQControl, 4);
assert_size_correct(seL4_TCB, 4);
assert_size_correct(seL4_Untyped, 4);
assert_size_correct(seL4_IA32_VMAttributes, 4);
assert_size_correct(seL4_IA32_ASIDControl, 4);
assert_size_correct(seL4_IA32_ASIDPool, 4);
assert_size_correct(seL4_IA32_IOSpace, 4);
assert_size_correct(seL4_IA32_IOPort, 4);
assert_size_correct(seL4_IA32_Page, 4);
assert_size_correct(seL4_IA32_PageDirectory, 4);
assert_size_correct(seL4_IA32_PageTable, 4);
assert_size_correct(seL4_IA32_IOPageTable, 4);
assert_size_correct(seL4_UserContext, 52);

/*
 * Return types for generated methods.
 */
struct seL4_IA32_IOPort_In8 {
	int error;
	uint8_t result;
};
typedef struct seL4_IA32_IOPort_In8 seL4_IA32_IOPort_In8_t;

struct seL4_IA32_IOPort_In16 {
	int error;
	uint16_t result;
};
typedef struct seL4_IA32_IOPort_In16 seL4_IA32_IOPort_In16_t;

struct seL4_IA32_IOPort_In32 {
	int error;
	uint32_t result;
};
typedef struct seL4_IA32_IOPort_In32 seL4_IA32_IOPort_In32_t;

struct seL4_Untyped_Retype {
	int error;
	int result;
};
typedef struct seL4_Untyped_Retype seL4_Untyped_Retype_t;

/*
 * Generated stubs.
 */
static inline int
seL4_IA32_PageTable_Map(seL4_IA32_PageTable service, seL4_IA32_PageDirectory pd, seL4_Word vaddr, seL4_IA32_VMAttributes attr)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 2,
			.extraCaps = 1,
			.label = IA32PageTableMap,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = pd;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = vaddr;
	ipc_buffer->msg[1] = attr;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_IA32_IOPageTable_Map(seL4_IA32_IOPageTable service, seL4_IA32_IOSpace iospace, seL4_Word ioaddr)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 1,
			.extraCaps = 1,
			.label = IA32IOPageTableMap,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = iospace;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = ioaddr;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_IA32_Page_Map(seL4_IA32_Page service, seL4_IA32_PageDirectory pd, seL4_Word vaddr, seL4_CapRights rights, seL4_IA32_VMAttributes attr)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 3,
			.extraCaps = 1,
			.label = IA32PageMap,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = pd;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = vaddr;
	ipc_buffer->msg[1] = rights;
	ipc_buffer->msg[2] = attr;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_IA32_Page_Remap(seL4_IA32_Page service, seL4_IA32_PageDirectory pd, seL4_CapRights rights, seL4_IA32_VMAttributes attr)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 2,
			.extraCaps = 1,
			.label = IA32PageRemap,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = pd;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = rights;
	ipc_buffer->msg[1] = attr;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_IA32_Page_Unmap(seL4_IA32_Page service)
{
	seL4_MessageInfo tag = {{
			.length = 0,
			.extraCaps = 0,
			.label = IA32PageUnmap,
			}};
	seL4_MessageInfo output_tag;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_IA32_Page_MapIO(seL4_IA32_Page service, seL4_IA32_IOSpace iospace, seL4_CapRights rights, seL4_Word ioaddr)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 2,
			.extraCaps = 1,
			.label = IA32PageMapIO,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = iospace;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = rights;
	ipc_buffer->msg[1] = ioaddr;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_IA32_ASIDControl_MakePool(seL4_IA32_ASIDControl service, seL4_Untyped untyped, seL4_CNode root, seL4_Word index, uint8_t depth)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 2,
			.extraCaps = 2,
			.label = IA32ASIDControlMakePool,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = untyped;
	ipc_buffer->caps[1] = root;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = index;
	ipc_buffer->msg[1] = (depth & 0xff);

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_IA32_ASIDPool_Assign(seL4_IA32_ASIDPool service, seL4_IA32_PageDirectory vroot)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 0,
			.extraCaps = 1,
			.label = IA32ASIDPoolAssign,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = vroot;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline seL4_IA32_IOPort_In8_t
seL4_IA32_IOPort_In8(seL4_IA32_IOPort service, uint16_t port)
{
	seL4_IA32_IOPort_In8_t result;
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 1,
			.extraCaps = 0,
			.label = IA32IOPortIn8,
			}};
	seL4_MessageInfo output_tag;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = (port & 0xffff);

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	/* Unmarshal result. */
	result.result = (ipc_buffer->msg[0] & 0xff);

	result.error = output_tag.label;
	return result;
}

static inline seL4_IA32_IOPort_In16_t
seL4_IA32_IOPort_In16(seL4_IA32_IOPort service, uint16_t port)
{
	seL4_IA32_IOPort_In16_t result;
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 1,
			.extraCaps = 0,
			.label = IA32IOPortIn16,
			}};
	seL4_MessageInfo output_tag;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = (port & 0xffff);

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	/* Unmarshal result. */
	result.result = (ipc_buffer->msg[0] & 0xffff);

	result.error = output_tag.label;
	return result;
}

static inline seL4_IA32_IOPort_In32_t
seL4_IA32_IOPort_In32(seL4_IA32_IOPort service, uint16_t port)
{
	seL4_IA32_IOPort_In32_t result;
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 1,
			.extraCaps = 0,
			.label = IA32IOPortIn32,
			}};
	seL4_MessageInfo output_tag;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = (port & 0xffff);

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	/* Unmarshal result. */
	result.result = ipc_buffer->msg[0];

	result.error = output_tag.label;
	return result;
}

static inline int
seL4_IA32_IOPort_Out8(seL4_IA32_IOPort service, uint16_t port, uint8_t data)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 1,
			.extraCaps = 0,
			.label = IA32IOPortOut8,
			}};
	seL4_MessageInfo output_tag;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = (port & 0xffff) | ((data & 0xff) << 16);

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_IA32_IOPort_Out16(seL4_IA32_IOPort service, uint16_t port, uint16_t data)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 1,
			.extraCaps = 0,
			.label = IA32IOPortOut16,
			}};
	seL4_MessageInfo output_tag;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = (port & 0xffff) | ((data & 0xffff) << 16);

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_IA32_IOPort_Out32(seL4_IA32_IOPort service, uint16_t port, uint32_t data)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 2,
			.extraCaps = 0,
			.label = IA32IOPortOut32,
			}};
	seL4_MessageInfo output_tag;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = (port & 0xffff);
	ipc_buffer->msg[1] = data;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_CNode_Copy(seL4_CNode service, seL4_Word dest_index, uint8_t dest_depth, seL4_CNode src_root, seL4_Word src_index, uint8_t src_depth, seL4_CapRights rights)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 5,
			.extraCaps = 1,
			.label = CNodeCopy,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = src_root;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = dest_index;
	ipc_buffer->msg[1] = (dest_depth & 0xff);
	ipc_buffer->msg[2] = src_index;
	ipc_buffer->msg[3] = (src_depth & 0xff);
	ipc_buffer->msg[4] = rights;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_CNode_Mint(seL4_CNode service, seL4_Word dest_index, uint8_t dest_depth, seL4_CNode src_root, seL4_Word src_index, uint8_t src_depth, seL4_CapRights rights, seL4_CapData badge)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 6,
			.extraCaps = 1,
			.label = CNodeMint,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = src_root;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = dest_index;
	ipc_buffer->msg[1] = (dest_depth & 0xff);
	ipc_buffer->msg[2] = src_index;
	ipc_buffer->msg[3] = (src_depth & 0xff);
	ipc_buffer->msg[4] = rights;
	ipc_buffer->msg[5] = badge.raw;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_CNode_Move(seL4_CNode service, seL4_Word dest_index, uint8_t dest_depth, seL4_CNode src_root, seL4_Word src_index, uint8_t src_depth)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 4,
			.extraCaps = 1,
			.label = CNodeMove,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = src_root;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = dest_index;
	ipc_buffer->msg[1] = (dest_depth & 0xff);
	ipc_buffer->msg[2] = src_index;
	ipc_buffer->msg[3] = (src_depth & 0xff);

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_CNode_Mutate(seL4_CNode service, seL4_Word dest_index, uint8_t dest_depth, seL4_CNode src_root, seL4_Word src_index, uint8_t src_depth, seL4_CapData badge)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 5,
			.extraCaps = 1,
			.label = CNodeMutate,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = src_root;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = dest_index;
	ipc_buffer->msg[1] = (dest_depth & 0xff);
	ipc_buffer->msg[2] = src_index;
	ipc_buffer->msg[3] = (src_depth & 0xff);
	ipc_buffer->msg[4] = badge.raw;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_CNode_Revoke(seL4_CNode service, seL4_Word index, uint8_t depth)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 2,
			.extraCaps = 0,
			.label = CNodeRevoke,
			}};
	seL4_MessageInfo output_tag;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = index;
	ipc_buffer->msg[1] = (depth & 0xff);

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_CNode_Delete(seL4_CNode service, seL4_Word index, uint8_t depth)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 2,
			.extraCaps = 0,
			.label = CNodeDelete,
			}};
	seL4_MessageInfo output_tag;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = index;
	ipc_buffer->msg[1] = (depth & 0xff);

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_CNode_SaveCaller(seL4_CNode service, seL4_Word index, uint8_t depth)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 2,
			.extraCaps = 0,
			.label = CNodeSaveCaller,
			}};
	seL4_MessageInfo output_tag;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = index;
	ipc_buffer->msg[1] = (depth & 0xff);

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_CNode_Recycle(seL4_CNode service, seL4_Word index, uint8_t depth)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 2,
			.extraCaps = 0,
			.label = CNodeRecycle,
			}};
	seL4_MessageInfo output_tag;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = index;
	ipc_buffer->msg[1] = (depth & 0xff);

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_CNode_Rotate(seL4_CNode service, seL4_Word dest_index, uint8_t dest_depth, seL4_CapData dest_badge, seL4_CNode pivot_root, seL4_Word pivot_index, uint8_t pivot_depth, seL4_CapData pivot_badge, seL4_CNode src_root, seL4_Word src_index, uint8_t src_depth)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 8,
			.extraCaps = 2,
			.label = CNodeRotate,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = pivot_root;
	ipc_buffer->caps[1] = src_root;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = dest_index;
	ipc_buffer->msg[1] = (dest_depth & 0xff);
	ipc_buffer->msg[2] = dest_badge.raw;
	ipc_buffer->msg[3] = pivot_index;
	ipc_buffer->msg[4] = (pivot_depth & 0xff);
	ipc_buffer->msg[5] = pivot_badge.raw;
	ipc_buffer->msg[6] = src_index;
	ipc_buffer->msg[7] = (src_depth & 0xff);

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_TCB_ReadRegisters(seL4_TCB service, bool suspend_source, uint8_t arch_flags, seL4_Word count, seL4_UserContext *regs)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 2,
			.extraCaps = 0,
			.label = TCBReadRegisters,
			}};
	seL4_MessageInfo output_tag;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = (suspend_source & 0x1) | ((arch_flags & 0xff) << 8);
	ipc_buffer->msg[1] = count;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	/* Unmarshal result. */
	regs->raw[0] = ipc_buffer->msg[0];
	regs->raw[1] = ipc_buffer->msg[1];
	regs->raw[2] = ipc_buffer->msg[2];
	regs->raw[3] = ipc_buffer->msg[3];
	regs->raw[4] = ipc_buffer->msg[4];
	regs->raw[5] = ipc_buffer->msg[5];
	regs->raw[6] = ipc_buffer->msg[6];
	regs->raw[7] = ipc_buffer->msg[7];
	regs->raw[8] = ipc_buffer->msg[8];
	regs->raw[9] = ipc_buffer->msg[9];
	regs->raw[10] = ipc_buffer->msg[10];
	regs->raw[11] = ipc_buffer->msg[11];
	regs->raw[12] = ipc_buffer->msg[12];

	return output_tag.label;
}

static inline int
seL4_TCB_WriteRegisters(seL4_TCB service, bool resume_target, uint8_t arch_flags, seL4_Word count, seL4_UserContext *regs)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 15,
			.extraCaps = 0,
			.label = TCBWriteRegisters,
			}};
	seL4_MessageInfo output_tag;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = (resume_target & 0x1) | ((arch_flags & 0xff) << 8);
	ipc_buffer->msg[1] = count;
	ipc_buffer->msg[2] = regs->raw[0];
	ipc_buffer->msg[3] = regs->raw[1];
	ipc_buffer->msg[4] = regs->raw[2];
	ipc_buffer->msg[5] = regs->raw[3];
	ipc_buffer->msg[6] = regs->raw[4];
	ipc_buffer->msg[7] = regs->raw[5];
	ipc_buffer->msg[8] = regs->raw[6];
	ipc_buffer->msg[9] = regs->raw[7];
	ipc_buffer->msg[10] = regs->raw[8];
	ipc_buffer->msg[11] = regs->raw[9];
	ipc_buffer->msg[12] = regs->raw[10];
	ipc_buffer->msg[13] = regs->raw[11];
	ipc_buffer->msg[14] = regs->raw[12];

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_TCB_CopyRegisters(seL4_TCB service, seL4_TCB source, bool suspend_source, bool resume_target, bool transfer_frame, bool transfer_integer, uint8_t arch_flags)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 1,
			.extraCaps = 1,
			.label = TCBCopyRegisters,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = source;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = (suspend_source & 0x1) | ((resume_target & 0x1) << 1) | ((transfer_frame & 0x1) << 2) | ((transfer_integer & 0x1) << 3) | ((arch_flags & 0xff) << 8);

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_TCB_Suspend(seL4_TCB service)
{
	seL4_MessageInfo tag = {{
			.length = 0,
			.extraCaps = 0,
			.label = TCBSuspend,
			}};
	seL4_MessageInfo output_tag;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_TCB_Resume(seL4_TCB service)
{
	seL4_MessageInfo tag = {{
			.length = 0,
			.extraCaps = 0,
			.label = TCBResume,
			}};
	seL4_MessageInfo output_tag;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_TCB_Configure(seL4_TCB service, seL4_Word fault_ep, uint8_t priority, seL4_CNode cspace_root, seL4_CapData cspace_root_data, seL4_CNode vspace_root, seL4_CapData vspace_root_data, seL4_Word buffer, seL4_CPtr bufferFrame)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 5,
			.extraCaps = 3,
			.label = TCBConfigure,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = cspace_root;
	ipc_buffer->caps[1] = vspace_root;
	ipc_buffer->caps[2] = bufferFrame;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = fault_ep;
	ipc_buffer->msg[1] = (priority & 0xff);
	ipc_buffer->msg[2] = cspace_root_data.raw;
	ipc_buffer->msg[3] = vspace_root_data.raw;
	ipc_buffer->msg[4] = buffer;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_TCB_SetPriority(seL4_TCB service, uint8_t priority)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 1,
			.extraCaps = 0,
			.label = TCBSetPriority,
			}};
	seL4_MessageInfo output_tag;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = (priority & 0xff);

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_TCB_SetIPCBuffer(seL4_TCB service, seL4_Word buffer, seL4_CPtr bufferFrame)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 1,
			.extraCaps = 1,
			.label = TCBSetIPCBuffer,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = bufferFrame;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = buffer;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_TCB_SetSpace(seL4_TCB service, seL4_Word fault_ep, seL4_CNode cspace_root, seL4_CapData cspace_root_data, seL4_CNode vspace_root, seL4_CapData vspace_root_data)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 3,
			.extraCaps = 2,
			.label = TCBSetSpace,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = cspace_root;
	ipc_buffer->caps[1] = vspace_root;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = fault_ep;
	ipc_buffer->msg[1] = cspace_root_data.raw;
	ipc_buffer->msg[2] = vspace_root_data.raw;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline seL4_Untyped_Retype_t
seL4_Untyped_Retype(seL4_Untyped service, int type, int size_bits, seL4_CNode root, int node_index, int node_depth, int node_offset, int node_window)
{
	seL4_Untyped_Retype_t result;
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 6,
			.extraCaps = 1,
			.label = UntypedRetype,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = root;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = type;
	ipc_buffer->msg[1] = size_bits;
	ipc_buffer->msg[2] = node_index;
	ipc_buffer->msg[3] = node_depth;
	ipc_buffer->msg[4] = node_offset;
	ipc_buffer->msg[5] = node_window;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	/* Unmarshal result. */
	result.result = ipc_buffer->msg[0];

	result.error = output_tag.label;
	return result;
}

static inline int
seL4_IRQHandler_Ack(seL4_IRQHandler service)
{
	seL4_MessageInfo tag = {{
			.length = 0,
			.extraCaps = 0,
			.label = IRQAckIRQ,
			}};
	seL4_MessageInfo output_tag;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_IRQHandler_SetEndpoint(seL4_IRQHandler service, seL4_CPtr endpoint)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 0,
			.extraCaps = 1,
			.label = IRQSetIRQHandler,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = endpoint;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_IRQHandler_Clear(seL4_IRQHandler service)
{
	seL4_MessageInfo tag = {{
			.length = 0,
			.extraCaps = 0,
			.label = IRQClearIRQHandler,
			}};
	seL4_MessageInfo output_tag;

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

static inline int
seL4_IRQControl_Get(seL4_IRQControl service, int irq, seL4_CNode root, seL4_Word index, uint8_t depth)
{
	seL4_IPCBuffer *ipc_buffer = seL4_GetIPCBuffer();
	seL4_MessageInfo tag = {{
			.length = 3,
			.extraCaps = 1,
			.label = IRQIssueIRQHandler,
			}};
	seL4_MessageInfo output_tag;

	/* Setup input capabilities. */
	ipc_buffer->caps[0] = root;

	/* Marshal input parameters. */
	ipc_buffer->msg[0] = irq;
	ipc_buffer->msg[1] = index;
	ipc_buffer->msg[2] = (depth & 0xff);

	/* Perform the call. */
	output_tag = seL4_Call(service, tag);

	return output_tag.label;
}

#endif /* _LIBSEL4__SEL4_CLIENT_H_ */
