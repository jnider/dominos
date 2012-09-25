KOS_TOP?=.

include $(KOS_TOP)/kos.make
include files.make

KERNEL_PATH = .
LIBS =
INCLUDES+= -I. -Iinclude 
CFLAGS+=
TARGET_NAME = kos

.PHONY: version

$(TARGET_NAME).bin: linker.ld kernel $(MODULES)
	@echo "linking $(TARGET_NAME).bin"
	@$(LD) -Map kos.map -T linker.ld -o $(TARGET_NAME).bin $(KERNEL_OBJS)
	mbchk $(TARGET_NAME).bin
	make --directory=kernel version
	@echo "build successful"
	
kernel: $(KERNEL_OBJS)

symbolfile: $(TARGET_NAME).bin
	@echo "making symbolfile"
	@./mksymbol $(TARGET_NAME).bin
	
version:
	@echo "updating version header"
	mv version.h version.old
	gawk -f incversion.awk version.old

clean:
	rm -f $(KERNEL_OBJS)

-include prerequisites.make
