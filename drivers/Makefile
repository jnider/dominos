# Makefile
#
# makefile for building Dominos drivers
#
# jnider 17-05-2013

include tools.mk
include rules.mk
include files.mk

INCLUDE_PATHS=-Iinclude
CONFIG?=release
SRC_DIR=src

#.PHONY: DRIVERS

DRIVER_TARGETS=$(DRIVERS:%=%.driver)

all: output_dir $(DRIVER_TARGETS) 
	@echo "Build complete"

define PROGRAM_template
$(DRIVERS).driver: objs/crt0.o $$($(1).driver)
	@echo building $$@
	$(LD) $(LDFLAGS) $$^ -T user.ld -o $$@

$(1).driver: $(call $(1).OBJS)
endef

$(foreach driver,$(DRIVERS),$(eval $(call PROGRAM_template,$(driver))))

output_dir:
	@if [ ! -d objs ]; then mkdir objs; fi
	@if [ ! -d $(OUTPUT_DIR) ]; then mkdir -p $(OUTPUT_DIR); fi

clean:
	rm -rf $(OUTPUT_DIR)

tags:
	$(CTAGS) -f tags src/* include/*
