# Makefile
#
# makefile for building Dominos userspace
#
# jnider 14-03-2014

include tools.mk
include rules.mk

S_SRC=crt0.s
SRC_DIR=.

.PHONY: drivers services

all: output_dir objs/crt0.o drivers services
	@echo "Build complete"

drivers:
	@echo "Building drivers"
	@make -C drivers

services:
	@echo "Building services"
	@make -C services

output_dir:
	@if [ ! -d objs ]; then mkdir objs; fi
	@if [ ! -d $(OUTPUT_DIR) ]; then mkdir -p $(OUTPUT_DIR); fi

clean:
	@rm -rf objs/
	@make -C drivers clean
	@make -C services clean

