# tools.mk
# 
# contains executable names of all of the tools
# to be used during the building of the project
# such as compiler, assembler, linker, etc.
#
# jnider 17-08-2012

CC=gcc
LD=ld
AS=as
CTAGS=ctags

CFLAGS+=-fno-stack-protector # stack protector functions are not implemented yet
CFLAGS+=-nostdinc # don't include paths other than what I tell you
CFLAGS+=-ffreestanding # ignore built-in functions, stdlib, etc
CFLAGS+=-g #add debugging info
CFLAGS+=-m32 #compile for 32-bit target
CFLAGS+=-Iinclude -I../../kernel/joel4/include

ASMFLAGS= --32

LDFLAGS=-melf_i386
