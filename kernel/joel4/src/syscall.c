#include "l4.h"

typedef void(syscall_handler_t)(uint32 param1);

void DebugPrintChar(uint32 param1)
{
   k_putchar((char)param1);
   serial_putc((char)param1);
}

void sch2(uint32 param1)
{
   k_printf("SysCall 2\n"); 
}

syscall_handler_t* syscall_handler_table[] =
{
   DebugPrintChar,
   sch2
};