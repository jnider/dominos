# Makefile
#
# makefile for building the joel4 kernel
#
# jnider 17-08-2012

include tools.mk
include rules.mk
include files.mk

INCLUDE_PATHS=-Iinclude
CONFIG?=release
SRC_DIR=src
OUTPUT_DIR=objs

all: output_dir $(OBJS)
	$(LD) $(OBJS) -o joel4.bin


output_dir:
	if [ ! -d $(OUTPUT_DIR) ]; then mkdir -p $(OUTPUT_DIR); fi
