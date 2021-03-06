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




void IDT_Init(unsigned short segment);

#endif // _IDT__H
