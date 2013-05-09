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
OUTPUT_NAME=joel4

all: joel4.elf tags bochs debug
	@echo "Build complete"

%.elf: output_dir version $(OBJS)
	$(LD) $(OBJS) -T $*.ld -o $@

bochs:
	readelf -s $(OUTPUT_NAME).elf | gawk -f sym.awk > $(OUTPUT_NAME).sym

debug:
	objdump -dS $(OUTPUT_NAME).elf > $(OUTPUT_NAME).s

output_dir:
	if [ ! -d $(OUTPUT_DIR) ]; then mkdir -p $(OUTPUT_DIR); fi

version:
	./inc_version.sh

init.cpio: initfiles.txt
	cpio -o < initfiles.txt > init.cpio

clean:
	rm -rf $(OUTPUT_DIR)
	rm -rf $(OUTPUT_NAME).elf

.PHONY: tags

tags:
	$(CTAGS) -f tags src/* include/*
