////////////////////////////////////////////////////////////////////////////////
///@file    kvideo.h
///@short   kernel text mode interface
///@author  J.Nider
///@date    01/11/2009
////////////////////////////////////////////////////////////////////////////////
/// These functions can only be called from kernel mode
////////////////////////////////////////////////////////////////////////////////

#ifndef _KVIDEO__H
#define _KVIDEO__H

#ifdef __cplusplus
extern "C" {
#endif

// stubs - don't call these!
void __cls(void);
#ifdef FAR_CALLS
far void __putchar(int c);
#else
void __putchar(int c);
#endif

#ifdef __cplusplus
}
#endif

// kernel mode only!

////////////////////////////////////////////////////////////////////////////////
/// @date   01/11/2009
/// @short  Clear the screen
////////////////////////////////////////////////////////////////////////////////
/// Used in kernel debugging - clears the screen
////////////////////////////////////////////////////////////////////////////////
void k_cls(void);

////////////////////////////////////////////////////////////////////////////////
/// @date   01/11/2009
/// @short  Write a character to the screen
////////////////////////////////////////////////////////////////////////////////
/// Used in kernel debugging - write a character in text mode to the current
/// cursor position.
////////////////////////////////////////////////////////////////////////////////
void k_putchar(int c);

#endif // _KVIDEO__H
