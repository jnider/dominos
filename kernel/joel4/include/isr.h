////////////////////////////////////////////////////////////////////////////////
/// @file   isr.h
/// @short  x86-specific definitions for interrupt handler routines
/// @author J.Nider
/// @date   30/10/2009
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef _ISR__H
#define _ISR__H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


// This defines what the stack looks like after an ISR was running
typedef struct regs
{
   unsigned int gs;
   unsigned int fs;
   unsigned int es;
   unsigned int ds;
   unsigned int edi;
   unsigned int esi;
   unsigned int ebp;
   //unsigned int esp;
   unsigned int ebx;
   unsigned int edx;
   unsigned int ecx;
   unsigned int eax;  
   unsigned int int_no;
   unsigned int err_code;
   unsigned int eip, cs, eflags;      /* pushed by the processor automatically */
   unsigned int esp, ss;
} regs_t;

/* typedef a pointer to a fault handler function */
typedef void(fault_handler_t)(regs_t*);

void isr0();
void isr1();
void isr2();
void isr3();
void isr4();
void isr5();
void isr6();
void isr7();
void isr8();
void isr9();
void isr10();
void isr11();
void isr12();
void isr13();
void isr14();
void isr15();
void isr16();
void isr17();
void isr18();

void irq0();
void irq1();
void irq2();
void irq3();
void irq4();
void irq5();
void irq6();
void irq7();
void irq8();
void irq9();
void irq10();
void irq11();
void irq12();
void irq13();
void irq14();
void irq15();

void ISR_Init(void);
void ISR_RegisterISRHandler(int isr, fault_handler_t callback);
void ISR_RegisterIRQHandler(int irq, fault_handler_t callback);

void k_printRegs(regs_t* pRegs);

#define _EOI_MASTER() outb(0x20, 0x20)
#define _EOI_SLAVE()  outb(0xA0, 0x20)

#define HALT() __ASM("hlt\n")

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _ISR__H */
