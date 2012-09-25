# loader.s

#.INCLUDE "kernel/memory.h"

.global _loader           # making entry point visible to linker
.extern _kernelStack

# setting up the Multiboot header - see GRUB docs for details
.set ALIGN,    1<<0             # align loaded modules on page boundaries
.set MEMINFO,  1<<1             # provide memory map
.set VIDEO,    1<<2              # video mode table
.set LOAD,     1<<16             # load addresses are available

.set FLAGS,    ALIGN | MEMINFO  # this is the Multiboot 'flag' field
.set MAGIC,    0x1BADB002       # 'magic number' lets bootloader find the header
.set CHECKSUM, -(MAGIC + FLAGS) # checksum required
.set HEADER_ADDR, _startKernel
.set LOAD_ADDR, _startKernel
.set LOAD_END_ADDR, 0
.set BSS_END_ADDR, _ebss
.set ENTRY_ADDR, _loader

.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.long HEADER_ADDR
.long LOAD_ADDR
.long LOAD_END_ADDR
.long BSS_END_ADDR
.long ENTRY_ADDR

_loader:       lea _kernelStack, %esp  # set the stack (don't forget to mark this as used later)
               push  %ebx              # Multiboot data structure
               push  %eax              # Multiboot magic number

               call  _main             # call kernel proper
               hlt                     # halt machine should kernel return
