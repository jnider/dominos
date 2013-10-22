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
struct __PACKED tss_t
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
   unsigned long  eip;                 // instruction pointer of the task
   unsigned long  eflags;              // see EFLAGS_ in eflags.h
   unsigned long  eax;                 // general purpose registers
   unsigned long  ecx;
   unsigned long  edx;
   unsigned long  ebx;
   unsigned long  esp;
   unsigned long  ebp;
   unsigned long  esi;                 // source index
   unsigned long  edi;                 // destination index
   unsigned short es;                  // segment selectors
   unsigned short reserved4;
   unsigned short cs;                  // code segment
   unsigned short reserved5;
   unsigned short ss;                  // stack segment
   unsigned short reserved6;
   unsigned short ds;                  // data segment
   unsigned short reserved7;
   unsigned short fs;                  // other segment
   unsigned short reserved8;
   unsigned short gs;                  // another segment
   unsigned short reserved9;
   unsigned short ldt;                 // segment selector for local descriptor table
   unsigned short reserved10;
   unsigned short debug;               // setting bit 0 causes a debug exception when this task is run
   unsigned short io_map;
};

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
///
/// index 0 is always the NULL segment, and cannot be used
////////////////////////////////////////////////////////////////////////////////
void GDT_SetSegment(unsigned short index, unsigned long base, unsigned long limit, DescriptorType type, PrivilegeLevel dpl);

unsigned short GDT_SetSystemGate(unsigned long base, unsigned long limit, SystemDescriptorType type, PrivilegeLevel dpl);

////////////////////////////////////////////////////////////////////////////////
/// @date   15/05/2009
/// @short  Creates an entry in the GDT for a task state segment (hardware task)
////////////////////////////////////////////////////////////////////////////////
unsigned short GDT_SetTSS(unsigned short index, const tss_t* pTSS, PrivilegeLevel dpl);

/**
 * Loads the global descriptor table into the processor, thus enabling segmentation
 */
int GDT_Load(unsigned short csIndex, unsigned short dsIndex);

#endif // _GDT_H_
