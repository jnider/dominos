////////////////////////////////////////////////////////////////////////////////

/// @file   architecture.h
/// @short  x86-specific definitions
/// @author J.Nider
/// @date   30/10/2009
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef _ARCHITECTURE__H
#define _ARCHITECTURE__H

// these flags are for the CR4 register
#define CR4_VME        0x00000001
#define CR4_PVI        0x00000002
#define CR4_TSD        0x00000004
#define CR4_DE         0x00000008
#define CR4_PSE        0x00000010
#define CR4_PAE        0x00000020
#define CR4_MCE        0x00000040
#define CR4_PGE        0x00000080
#define CR4_PCE        0x00000100
#define CR4_OSFXSR     0x00000200
#define CR4_OSXMMEXCPT 0x00000400
#define CR4_RESERVED1  0x00001800
#define CR4_VMXE       0x00002000
#define CR4_SMXE       0x00004000
#define CR4_RESERVED2  0x00018000
#define CR4_OSXSAVE    0x00020000
#define CR4_RESERVED3  0xFFFC0000


#endif // _ARCHITECTURE__H

