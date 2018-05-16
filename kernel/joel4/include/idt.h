////////////////////////////////////////////////////////////////////////////////
/// @file   idt.h
/// @short  x86-specific definitions for interrupt descriptor table
/// @author J.Nider
/// @date   30/10/2009
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef _IDT__H
#define _IDT__H

#define _MAX_INTERRUPTS    50

//////
/// Used for CPL, IOPL, etc
//////
typedef enum PrivilegeLevel
{
   PRIVILEGE_LEVEL_KERNEL,
   PRIVILEGE_LEVEL_UNUSED1,
   PRIVILEGE_LEVEL_UNUSED2,
   PRIVILEGE_LEVEL_USER
} PrivilegeLevel;

void IDT_Init(unsigned short segment);
void idt_set_gate(unsigned char index, unsigned long base, unsigned short segment, PrivilegeLevel dpl);

#endif // _IDT__H
