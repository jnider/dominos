# files.mk
#
# list of files to be compiled
#
# jnider 17-08-2012

INTEL?=y

C_SRC = kernel.c \
         kstdio.c \
         kvideo.c \
         serial.c \
         memory.c \
         malloc.c \
         cpu.c    \
         task.c   \
         root_task.c

ifeq ($(strip $(INTEL)), y)
	C_SRC += intel.c \
			gdt.c \
			idt.c \
			isr.c \
			syscall.c
   CFLAGS += -DARCH_IA32
endif

S_SRC = boot.s    \
         isr_stub.s
