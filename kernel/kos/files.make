# define the .c source files
KERNEL_C_SRC =   $(KERNEL_PATH)/isr.c \
                  $(KERNEL_PATH)/kstub.c \
                  $(KERNEL_PATH)/kvideo.c \
                  $(KERNEL_PATH)/kernel.c \
                  $(KERNEL_PATH)/keyboard.c \
                  $(KERNEL_PATH)/gdt.c \
                  $(KERNEL_PATH)/idt.c \
                  $(KERNEL_PATH)/kstdio.c \
                  #$(KERNEL_PATH)/task.c  \
                  
# define the assembly files
KERNEL_ASM_SRC = $(KERNEL_PATH)/loader.s \
                 $(KERNEL_PATH)/isr_stub.s

# this rule automatically finds the .o targets of the .c files
KERNEL_C_OBJS = $(KERNEL_C_SRC:.c=.o)

# find the .o targets automatically
KERNEL_ASM_OBJS = $(KERNEL_ASM_SRC:.s=.o)

# all target object files
KERNEL_OBJS = $(KERNEL_ASM_OBJS) $(KERNEL_C_OBJS)

STUB_C_SRC =      $(KERNEL_PATH)/kstub.c \
                  $(KERNEL_PATH)/timer.c \
                  $(KERNEL_PATH)/task.c

STUB_C_OBJS =     $(STUB_C_SRC:.c=.o)

