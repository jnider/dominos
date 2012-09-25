////////////////////////////////////////////////////////////////////////////////
/// @file   idt.cpp
/// @short  Interrupt descriptor table definitions
/// @author J.Nider
/// @date   25/08/2008
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "isr.h"
#include "idt.h"
#include "gdt.h"
#include "kstdio.h"

#define IDT_FLAG_OTHERS    0x0E
#define IDT_FLAG_DPL       0x70
#define IDT_FLAG_PRESENT   0x80

/* Defines an IDT entry */
typedef struct idt_entry_t
{
    unsigned short base_low;
    unsigned short segment;
    unsigned char always0;     /* This will ALWAYS be set to 0 */
    unsigned char flags;
    unsigned short base_high;
} idt_entry_t __PACKED;

typedef struct idt_ptr_t
{
    unsigned short limit;
    idt_entry_t* base;
} idt_ptr_t __PACKED;

#define _LOAD_IDT()   __ASM("lidt idtp\n")

idt_entry_t idt[_MAX_INTERRUPTS] __attribute__((aligned(8))); /* the interrupt descriptor table */
idt_ptr_t idtp;

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

void irqCooperate();
void irqV8086();

// adds an ISR to the IDT
void idt_set_gate(unsigned char index, unsigned long base, unsigned short segment, PrivilegeLevel dpl)
{
   idt[index].base_low = base & 0xFFFF;
   idt[index].base_high = (base >> 16) & 0xFFFF;
   idt[index].segment = segment;
   idt[index].always0 = 0;
   idt[index].flags = IDT_FLAG_PRESENT | (dpl << 5) | IDT_FLAG_OTHERS;
}

// Installs the IDT
void IDT_Init(unsigned short segment)
{

   // Clear out the entire IDT, initializing it to zeros
   k_memset(idt, 0, sizeof(idt_entry_t) * _MAX_INTERRUPTS);

   // register the isr stubs from isr_stub.s
   // they are responsible for calling the real handler
   idt_set_gate(0, (unsigned)isr0, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(1, (unsigned)isr1, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(2, (unsigned)isr2, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(3, (unsigned)isr3, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(4, (unsigned)isr4, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(5, (unsigned)isr5, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(6, (unsigned)isr6, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(7, (unsigned)isr7, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(8, (unsigned)isr8, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(9, (unsigned)isr9, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(10, (unsigned)isr10, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(11, (unsigned)isr11, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(12, (unsigned)isr12, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(13, (unsigned)isr13, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(14, (unsigned)isr14, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(15, (unsigned)isr15, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(16, (unsigned)isr16, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(17, (unsigned)isr17, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(18, (unsigned)isr18, segment, PRIVILEGE_LEVEL_KERNEL);

   idt_set_gate(32, (unsigned)irq0, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(33, (unsigned)irq1, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(34, (unsigned)irq2, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(35, (unsigned)irq3, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(36, (unsigned)irq4, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(37, (unsigned)irq5, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(38, (unsigned)irq6, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(39, (unsigned)irq7, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(40, (unsigned)irq8, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(41, (unsigned)irq9, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(42, (unsigned)irq10, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(43, (unsigned)irq11, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(44, (unsigned)irq12, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(45, (unsigned)irq13, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(46, (unsigned)irq14, segment, PRIVILEGE_LEVEL_KERNEL);
   idt_set_gate(47, (unsigned)irq15, segment, PRIVILEGE_LEVEL_KERNEL);

   idt_set_gate(48, (unsigned)irqCooperate, segment, PRIVILEGE_LEVEL_USER); // can be called from user code
   idt_set_gate(49, (unsigned)irqV8086, segment, PRIVILEGE_LEVEL_USER); // can be called from user code

   // Sets the special IDT pointer up
   idtp.limit = (sizeof (idt_entry_t) * _MAX_INTERRUPTS) - 1;
   idtp.base = idt;

   // load the IDT into its register
   _LOAD_IDT(); // must be in privileged mode
}

