////////////////////////////////////////////////////////////////////////////////
/// @file   gdt.h
/// @short  x86-specific definitions for global descriptor table
/// @author J.Nider
/// @date   30/10/2009
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef _GDT_H_
#define _GDT_H_

#include "kstdio.h"

enum DescriptorType
{
   DESCRIPTOR_DATA_RO,                 ///< data segment, read only
   DESCRIPTOR_DATA_RO_A,               ///< data segment, read only, accessed
   DESCRIPTOR_DATA_RW,                 ///< data segment, read/write
   DESCRIPTOR_DATA_RW_A,               ///< data segment, read/write, accessed
   DESCRIPTOR_DATA_RO_X,               ///< data segment, read only, expand down
   DESCRIPTOR_DATA_RO_XA,              ///< data segment, read only, expand down, accessed
   DESCRIPTOR_DATA_RW_X,               ///< data segment, read/write, expand down
   DESCRIPTOR_DATA_RW_XA,              ///< data segment, read/write, expand down, accessed

   DESCRIPTOR_CODE_EO,                 ///< code segment, execute only
   DESCRIPTOR_CODE_EO_A,               ///< code segment, execute only, accessed
   DESCRIPTOR_CODE_ER,                 ///< code segment, execute/read
   DESCRIPTOR_CODE_ER_A,               ///< code segment, execute/read, accessed
   DESCRIPTOR_CODE_EO_C,               ///< code segment, execute only, conforming
   DESCRIPTOR_CODE_EO_CA,              ///< code segment, execute only, conforming, accessed
   DESCRIPTOR_CODE_ER_C,               ///< code segment, execute/read, conforming
   DESCRIPTOR_CODE_ER_CA               ///< code segment, execute/read, conforming, accessed
};

enum SystemDescriptorType
{
   DESCRIPTOR_SYSTEM_RESERVED,         ///< reserved
   DESCRIPTOR_SYSTEM_16_TSS,           ///< 16-bit TSS, available
   DESCRIPTOR_SYSTEM_LDT,              ///< local descriptor table
   DESCRIPTOR_SYSTEM_16_TSS_BUSY,      ///< 16-bit TSS, busy
   DESCRIPTOR_SYSTEM_16_CALL_GATE,     ///< 16-bit call gate
   DESCRIPTOR_SYSTEM_TASK_GATE,        ///< task gate
   DESCRIPTOR_SYSTEM_16_INT_GATE,      ///< 16-bit interrupt gate
   DESCRIPTOR_SYSTEM_16_TRAP_GATE,     ///< 16-bit trap gate
   DESCRIPTOR_SYSTEM_RESERVED2,        ///< reserved
   DESCRIPTOR_SYSTEM_32_TSS,           ///< 32-bit TSS, available
   DESCRIPTOR_SYSTEM_RESERVED3,        ///< reserved
   DESCRIPTOR_SYSTEM_32_TSS_BUSY,      ///< 32-bit TSS, busy
   DESCRIPTOR_SYSTEM_32_CALL_GATE,     ///< 32-bit call gate
   DESCRIPTOR_SYSTEM_RESERVED4,        ///< reserved
   DESCRIPTOR_SYSTEM_32_INT_GATE,      ///< 32-bit interrupt gate
   DESCRIPTOR_SYSTEM_32_TRAP_GATE      ///< 32-bit trap gate
};

//////
/// Used for CPL, IOPL, etc
//////
enum PrivilegeLevel
{
   PRIVILEGE_LEVEL_KERNEL,
   PRIVILEGE_LEVEL_UNUSED1,
   PRIVILEGE_LEVEL_UNUSED2,
   PRIVILEGE_LEVEL_USER
};

// the tss must fit in a page, and be aligned dword
struct tss_t
{
   unsigned short previous_task;       // segment selector of the previous task
   unsigned short reserved0;
   unsigned long  esp0;
   unsigned short ss0;
   unsigned short reserved1;
   unsigned long  esp1;
   unsigned short ss1;
   unsigned short reserved2;
   unsigned long  esp2;
   unsigned short ss2;
   unsigned short reserved3;
   unsigned long  pdbr;                // page directory base register
   unsigned long  eip;                 // ip of the task
   unsigned long  eflags;              // see EFLAGS_ below
   unsigned long  eax;                 // general purpose registers
   unsigned long  ecx;
   unsigned long  edx;
   unsigned long  ebx;
   unsigned long  esp;
   unsigned long  ebp;
   unsigned long  esi;
   unsigned long  edi;
   unsigned short es;                  // segment selectors
   unsigned short reserved4;
   unsigned short cs;
   unsigned short reserved5;
   unsigned short ss;
   unsigned short reserved6;
   unsigned short ds;
   unsigned short reserved7;
   unsigned short fs;
   unsigned short reserved8;
   unsigned short gs;
   unsigned short reserved9;
   unsigned short ldt;                 // segment selector for ldt
   unsigned short reserved10;
   unsigned short debug;               // setting bit 0 causes a debug exception when this task is run
   unsigned short io_map;
}__PACKED;

// these are for the EFLAGS register 3.4.3 Volume 1
#define EFLAGS_CF       (1 << 0)       // Carry Flag
#define EFLAGS_PF       (1 << 2)       // Parity Flag
#define EFLAGS_AF       (1 << 4)       // Auxiliary Carry Flag
#define EFLAGS_ZF       (1 << 6)       // Zero Flag
#define EFLAGS_SF       (1 << 7)       // Sign Flag
#define EFLAGS_TF       (1 << 8)       // Trap Flag (set for single-step debugging)
#define EFLAGS_IF       (1 << 9)       // Interrupt Enable Flag
#define EFLAGS_DF       (1 << 10)      // Direction Flag 0: forward, 1: reverse
#define EFLAGS_OF       (1 << 11)      // Overflow Flag
#define EFLAGS_IOPL(x)  (x << 12)      // IO Privilege Level - see PrivilegeLevel above
#define EFLAGS_NT       (1 << 14)      // Nested Task
#define EFLAGS_RF       (1 << 16)      // Resume Flag
#define EFLAGS_VM       (1 << 17)      // Virtual 8086 Mode
#define EFLAGS_AC       (1 << 18)      // Alignment Check
#define EFLAGS_VIF      (1 << 19)      // Virtual Interrupt Flag
#define EFLAGS_VIP      (1 << 20)      // Virtual Interrupt Pending
#define EFLAGS_ID       (1 << 21)      // ID Flag

#ifndef __cplusplus
typedef enum DescriptorType DescriptorType;
typedef enum SystemDescriptorType SystemDescriptorType;
typedef struct tss_t tss_t;
typedef enum PrivilegeLevel PrivilegeLevel;
#endif

#define MAX_GDT_ENTRIES 20

void GDT_Init();

////////////////////////////////////////////////////////////////////////////////
/// @date   15/05/2009
/// @short  Creates a code or data descriptor in the GDT
////////////////////////////////////////////////////////////////////////////////
void GDT_SetSegment(unsigned short index, unsigned long base, unsigned long limit, DescriptorType type, PrivilegeLevel dpl);

unsigned short GDT_SetSystemGate(unsigned long base, unsigned long limit, SystemDescriptorType type, PrivilegeLevel dpl);

////////////////////////////////////////////////////////////////////////////////
/// @date   15/05/2009
/// @short  Creates an entry in the GDT for a task state segment (hardware task)
////////////////////////////////////////////////////////////////////////////////
unsigned short GDT_SetTSS(unsigned short index, const tss_t* pTSS, PrivilegeLevel dpl);
void GDT_Load(unsigned short csIndex, unsigned short dsIndex);

#endif // _GDT_H_
