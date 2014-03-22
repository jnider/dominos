# interrupt stubs in assembler

.extern interrupt_handler_table
.extern kernel_data_segment
.extern syscall_handler_table

.global isr0
.global isr1
.global isr2
.global isr3
.global isr4
.global isr5
.global isr6
.global isr7
.global isr8
.global isr9
.global isr10
.global isr11
.global isr12
.global isr13
.global isr14
.global isr15
.global isr16
.global isr17
.global isr18
.global irq0
.global irq1
.global irq2
.global irq3
.global irq4
.global irq5
.global irq6
.global irq7
.global irq8
.global irq9
.global irq10
.global irq11
.global irq12
.global irq13
.global irq14
.global irq15
.global irqCooperate
.global irqV8086
.global syscall

isr0:
   cli
   pushl $0
   pushl $0
   jmp isr_common_stub

isr1:
   cli
   pushl $0
   pushl $1
   jmp isr_common_stub

isr2:
   cli
   pushl $0
   pushl $2
   jmp isr_common_stub

isr3:
   cli
   pushl $0
   pushl $3
   jmp isr_common_stub

isr4:
   cli
   pushl $0
   pushl $4
   jmp isr_common_stub

isr5:
   cli
   pushl $0
   pushl $5
   jmp isr_common_stub

isr6:
   cli
   pushl $0
   pushl $6
   jmp isr_common_stub

isr7:
   cli
   pushl $0
   pushl $7
   jmp isr_common_stub

isr8:
   cli
   pushl $8
   jmp isr_common_stub

isr9:
   cli
   pushl $0
   pushl $9
   jmp isr_common_stub

isr10:
   cli
   pushl $10
   jmp isr_common_stub

isr11:
   cli
   pushl $11
   jmp isr_common_stub

isr12:
   cli
   pushl $12
   jmp isr_common_stub

isr13:
   cli
   pushl $13
   jmp isr_common_stub

isr14:
   cli
   pushl $14
   jmp isr_common_stub

isr15:
   cli
   pushl $0
   pushl $15
   jmp isr_common_stub

isr16:
   cli
   pushl $0
   pushl $16
   jmp isr_common_stub

isr17:
   cli
   pushl $0
   pushl $17
   jmp isr_common_stub

isr18:
   cli
   pushl $0
   pushl $18
   jmp isr_common_stub

irq0:
   cli
   pushl $0
   pushl $32
   jmp isr_common_stub

irq1:
   cli
   pushl $0
   pushl $33
   jmp isr_common_stub

irq2:
   cli
   pushl $0
   pushl $34
   jmp isr_common_stub

irq3:
   cli
   pushl $0
   pushl $35
   jmp isr_common_stub

irq4:
   cli
   pushl $0
   pushl $36
   jmp isr_common_stub

irq5:
   cli
   pushl $0
   pushl $37
   jmp isr_common_stub

irq6:
   cli
   pushl $0
   pushl $38
   jmp isr_common_stub

irq7:
   cli
   pushl $0
   pushl $39
   jmp isr_common_stub

irq8:
   cli
   pushl $0
   pushl $40
   jmp isr_common_stub

irq9:
   cli
   pushl $0
   pushl $41
   jmp isr_common_stub

irq10:
   cli
   pushl $0
   pushl $42
   jmp isr_common_stub

irq11:
   cli
   pushl $0
   pushl $43
   jmp isr_common_stub

irq12:
   cli
   pushl $0
   pushl $44
   jmp isr_common_stub

irq13:
   cli
   pushl $0
   pushl $45
   jmp isr_common_stub

irq14:
   cli
   pushl $0
   pushl $46
   jmp isr_common_stub

irq15:
   cli
   pushl $0
   pushl $47
   jmp isr_common_stub

irqCooperate:
   pushl $0
   pushl $48
   jmp isr_common_stub

irqV8086:
   pushl $0
   pushl $49
   jmp isr_common_stub

isr_common_stub:
   # save the registers on the stack
   pushl %eax
   pushl %ecx
   pushl %edx
   pushl %ebx
   pushl %ebp
   pushl %esi
   pushl %edi
   pushl %ds
   pushl %es
   pushl %fs
   pushl %gs

   # load all selector registers with the kernel's data segment (DSOS)
   mov $kernel_data_segment, %eax
   mov (%eax), %eax
   shl $3, %eax
   mov %ax, %ds
   mov %ax, %es
   mov %ax, %fs
   mov %ax, %gs

   # save the stack pointer on the stack - this becomes our variable "regs"
   mov %esp, %eax
   pushl %eax

   # call the C function handler
   mov $interrupt_handler_table, %esi # move the address of the table into ESI

   # load int_no from the stack into eax
   movl 48(%esp), %eax

   movl (%esi, %eax, 4), %ebx
   call *%ebx

   # regs get popped by the called function

   # clean the stack
   popl %eax
   popl %gs
   popl %fs
   popl %es
   popl %ds
   popl %edi
   popl %esi
   popl %ebp
   popl %ebx
   popl %edx
   popl %ecx
   popl %eax

   addl $8, %esp # gets rid of the error code and interrupt number
   iret

syscall:
   pushl %edx                       # save the user mode instruction pointer
   pushl %ecx                       # save the user mode stack pointer
   
   cmpl $3, %eax                    # make sure there is a valid handler for this index
   jle syscall_index_ok
   movl %eax, %ebx                  # set the invalid code as the first parameter
   movl $0, %eax                    # if not, call the 'invalid' handler

syscall_index_ok:
   pushl %ebx                       # address of first parameter of user's stack
   mov $syscall_handler_table, %esi # move the address of the table into ESI
   movl (%esi, %eax, 4), %ebx       # calculate the offset of the function pointer
   call *%ebx                       # call it
   
   # restore parameters & clean up the stack
   addl $4, %esp
   popl %ecx # restore the user mode stack pointer
   popl %edx # restore the user mode instruction pointer
   sysexit

#.globl GDT_Flush
#.type GDT_Flush, @function
#GDT_Flush:
#   push %eax
#   movl $task_sel, %eax
#   movl $GDT_Flush2,(%eax)
#   ljmp *(%eax)
#GDT_Flush2:
#   pop %eax
#   add $0x8, %esp
#   ret

