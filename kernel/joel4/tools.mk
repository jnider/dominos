# tools.mk
# 
# contains executable names of all of the tools
# to be used during the building of the project
# such as compiler, assembler, linker, etc.
#
# if you add a new tool, make sure you add it to the 'tool' target
# to make sure the tool exists
#
# jnider 17-08-2012

# make sure all the needed tools are installed correctly
# this is a cheap version of this function - I'm sure someone can come up
# with something better in the future.
CC?=gcc
LD=ld
AS=as
CTAGS=ctags
AWK=awk
CLOC=cloc

CFLAGS+=-fno-stack-protector # stack protector functions are not implemented yet
CFLAGS+=-nostdinc # don't include paths other than what I tell you
CFLAGS+=-ffreestanding # ignore built-in functions, stdlib, etc
CFLAGS+=-m32 # compile for 32-bit target

ASMFLAGS=--32

LDFLAGS=-melf_i386 # link in ELF format

ifeq ($(DEBUG),y)
CFLAGS+=-g # add debugging info
CFLAGS+=-O0 # no optimizations
else
CFLAGS+=-O2 # many optimizations
endif

tools:
	@echo "Checking for the existence of required tools"
	which $(CC)
	which $(LD)
	which $(AS)
	which $(CTAGS)
	which $(AWK)
	which $(CLOC)

