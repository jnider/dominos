# files.mk
#
# list of files to be compiled
#
# jnider 17-08-2012

C_SRC = kernel.c \
         kstdio.c \
         kvideo.c \
         serial.c \
         memory.c \
         gdt.c    \
         idt.c    \
         isr.c    \
         cpu.c

S_SRC = boot.s    \
         isr_stub.s
