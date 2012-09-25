////////////////////////////////////////////////////////////////////////////////
/// @file   isr.c
/// @short  contains stubs for interrupt service routines
/// @author J.Nider
/// @date   25/08/2008
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "kstdio.h"        /* standard functions - almost */
#include "timer.h"         /* timer functions */
#include "isr.h"           /* my own functions & definitions */
#include "idt.h"           /* interrupt descriptor table stuff */

static void _fh_handler(regs_t* pRegs);
static void _fh_default(regs_t* pRegs);
static void _irq0to7(regs_t* pRegs);
static void _irq8to15(regs_t* pRegs);

const char *exception_messages[_MAX_INTERRUPTS] =
{
   "Division By Zero",                 // 0
   "Debug",                            // 1
   "Non Maskable Interrupt",           // 2
   "Breakpoint",                       // 3
   "Overflow",                         // 4
   "Out Of Bounds",                    // 5
   "Invalid OpCode",                   // 6
   "No co-processor",                  // 7
   "Double fault",                     // 8
   "Co-processor segment overrun",     // 9
   "Bad TSS",                          // 10
   "Segment Not Present",              // 11
   "Stack Fault Exception",            // 12
   "General Protection Fault",         // 13
   "Page Fault",                       // 14
   "Unknown Interrupt",                // 15
   "Co-processor fault",               // 16
   "Alignment check",                  // 17
   "Machine check",                    // 18
   "Reserved19",
   "Reserved20",
   "Reserved21",
   "Reserved22",
   "Reserved23",
   "Reserved24",
   "Reserved25",
   "Reserved26",
   "Reserved27",
   "Reserved28",
   "Reserved29",
   "Reserved30",
   "Reserved31",
   "",                                 // 32
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "",                                 // 40
   "",
   "",
   "",
   "",
   "",
   "",
   "",
   "Cooperate",                        // 48
   "V8086"
};

void _fh_dbz(regs_t* pRegs)
{
   k_printf("Divide by zero\n");
   k_printRegs(pRegs);
   HALT(); // halt for now
}


fault_handler_t* interrupt_handler_table[_MAX_INTERRUPTS] =
{
   _fh_dbz,                        // Division By Zero
   _fh_handler,                        // Debug
   _fh_handler,                        // Non Maskable Interrupt
   _fh_handler,                        // Breakpoint
   _fh_handler,                        // Overflow
   _fh_handler,                        // Out Of Bounds
   _fh_handler,                        // Invalid OpCode
   _fh_handler,                        // No co-processor
   _fh_handler,                        // Double fault
   _fh_handler,                        // Co-processor segment overrun
   _fh_handler,                        // Bad TSS
   _fh_handler,                        // Segment Not Present
   _fh_handler,                        // Stack Fault Exception
   _fh_handler,                        // General Protection Fault
   _fh_handler,                        // Page Fault
   _fh_handler,                        // Unknown Interrupt
   _fh_handler,                        // Co-processor fault
   _fh_handler,                        // Alignment check
   _fh_handler,                        // Machine check
   _fh_default,                        // Reserved19
   _fh_default,                        // Reserved20
   _fh_default,                        // Reserved21
   _fh_default,                        // Reserved22
   _fh_default,                        // Reserved23
   _fh_default,                        // Reserved24
   _fh_default,                        // Reserved25
   _fh_default,                        // Reserved26
   _fh_default,                        // Reserved27
   _fh_default,                        // Reserved28
   _fh_default,                        // Reserved29
   _fh_default,                        // Reserved30
   _fh_default,                        // Reserved31
   _irq0to7,                           // timer
   _irq0to7,
   _irq0to7,
   _irq0to7,
   _irq0to7,
   _irq0to7,
   _irq0to7,
   _irq0to7,
   _irq8to15,
   _irq8to15,
   _irq8to15,
   _irq8to15,
   _irq8to15,
   _irq8to15,
   _irq8to15,
   _irq8to15,
   _fh_handler,
   _fh_handler
};


void ISR_RegisterISRHandler(int isr, fault_handler_t callback)
{
   interrupt_handler_table[isr] = callback;
}

void ISR_RegisterIRQHandler(int irq, fault_handler_t callback)
{
   interrupt_handler_table[irq+32] = callback;
}

void _fh_handler(regs_t* pRegs)
{
   unsigned int dir = 0;
   k_printf("code %i: %s at 0x%x\n", pRegs->err_code, exception_messages[pRegs->int_no], pRegs->eip);
   k_printRegs(pRegs);
   __ASM __volatile__ ("movl %%cr3, %0": "=a" (dir));
   k_printf("page dir: 0x%x", dir);
   HALT(); // halt for now
}

void _fh_default(regs_t* pRegs)
{
   // use this to catch unexpected exceptions
   k_printf("Exception: default handler (code %i)\n", pRegs->err_code);
   HALT(); // halt for now
}

// IRQ handlers - IRQ 0-7 must send 'end of interrupt' to the master controller
void _irq0to7(regs_t* pRegs)
{
   k_printf("unhandled IRQ %i\n", pRegs->err_code);
   _EOI_MASTER();
}

// IRQ 8-15 must send EOI to both controllers
void _irq8to15(regs_t* pRegs)
{
   k_printf("unhandled IRQ %i\n", pRegs->err_code);
   _EOI_MASTER();
   _EOI_SLAVE();
}

void ISR_Init(void)
{
   // program the PIC
   outportb(0x20, 0x11);
   outportb(0xA0, 0x11);
   outportb(0x21, 0x20);
   outportb(0xA1, 0x28);
   outportb(0x21, 0x04);
   outportb(0xA1, 0x02);
   outportb(0x21, 0x01);
   outportb(0xA1, 0x01);
   outportb(0x21, 0x0);
   outportb(0xA1, 0x0);
}

void k_printRegs(regs_t* pRegs)
{
   k_printf("esp: 0x%x   ss: 0x%x   \n", pRegs->esp, pRegs->ss);
   k_printf("eflags: 0x%x   cs: 0x%x   eip: 0x%x   \n", pRegs->eflags, pRegs->cs, pRegs->eip);
   k_printf("int_no: 0x%x   err_code: 0x%x   \n", pRegs->int_no, pRegs->err_code);
   k_printf("edi: 0x%x   esi: 0x%x   ebp: 0x%x   \n", pRegs->edi, pRegs->esi, pRegs->ebp);
   k_printf("eax: 0x%x   ecx: 0x%x   edx: 0x%x   ebx: 0x%x   \n", pRegs->eax, pRegs->ecx, pRegs->edx, pRegs->ebx);
   k_printf("ds: 0x%x   es: 0x%x   \n", pRegs->ds, pRegs->es);
   //k_printf("fs: 0x%x   \ngs: 0x%x   \n", pRegs->fs, pRegs->gs);
}
