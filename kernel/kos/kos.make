#CC=i586-kos-gcc
#CPP=i586-kos-c++
#AS=i586-kos-as
#LD=i586-kos-ld
#OBJDUMP=i586-kos-objdump
CC=gcc
CPP=gcc
AS=as
LD=ld
OBJDUMP=objdump
AR=ar

CFLAGS+=-Wall -nostartfiles -D__KOS__
CPPFLAGS+=-Wall -nostdlib -nostartfiles -nodefaultlibs -fconserve-space -fno-exceptions -fno-rtti -fno-implicit-templates -D__KOS__

# build object files from c source
%.o: %.c
	@echo "compiling $<"
	$(CC) -o $@ -c $< $(INCLUDES) $(CFLAGS)

#	@$(CC) -o $@ -c $< $(INCLUDES) -Wall -nostdlib -nostartfiles -nodefaultlibs

# build object files from cpp source
%.o: %.cpp
	@echo "compiling $<"
	@$(CPP) -o $@ -c $< $(INCLUDES) $(CPPFLAGS)

# build object files from assembler source
%.o: %.s
	@echo "Assembling $<"
	@$(AS) -o $@ $<
