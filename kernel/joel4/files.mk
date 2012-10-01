# files.mk
#
# list of files to be compiled
#
# jnider 17-08-2012

C_SRC = kernel.c \
         kstdio.c \
         kvideo.c \
         serial.c \
         gdt.c    \
         idt.c    \
         isr.c

S_SRC = boot.s    \
         isr_stub.s
