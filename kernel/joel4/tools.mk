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

CFLAGS+=-fno-stack-protector
CFLAGS+=-ffreestanding			# ignore built-in functions, stdlib, etc
