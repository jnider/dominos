#.INCLUDE "src/memory.h"

.global _entry_point           # making entry point visible to linker
.extern _kernelStack
.extern _boot_sys_start

# setting up the Multiboot header - see GRUB docs for details
.set ALIGN,    0<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set VIDEO,    1<<2              # video mode table
.set LOAD,     1<<16             # load addresses are available

.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum required
.set HEADER_ADDR, 0x101000
.set LOAD_ADDR, 0x100000
.set LOAD_END_ADDR, 0
.set BSS_END_ADDR, 0 #_ebss
.set ENTRY_ADDR, _entry_point

.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.long HEADER_ADDR
.long LOAD_ADDR
.long LOAD_END_ADDR
.long BSS_END_ADDR
.long ENTRY_ADDR

_entry_point:  lea _kernelStack, %esp  # set the stack (don't forget to mark this as used later)
               push  %ebx              # Multiboot data structure
               push  %eax              # Multiboot magic number

               call  _main             # call kernel proper
               hlt                     # halt machine should kernel return
