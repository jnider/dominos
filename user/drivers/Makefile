# Makefile
#
# makefile for building Dominos drivers
#
# jnider 17-05-2013

include ../tools.mk
include ../rules.mk
include files.mk

CONFIG?=release
SRC_DIR=src

#.PHONY: DRIVERS

DRIVER_TARGETS=$(DRIVERS:%=%.driver)

all: output_dir $(DRIVER_TARGETS) 
	@echo "Build complete"

define PROGRAM_template
$(DRIVERS).driver: $$($(1).driver)
	@echo building $$@
	$(LD) $(LDFLAGS) ../objs/crt0.o $$^ -T ../user.ld ../../kernel/joel4/joel4.lib -o $$@

$(1).driver: $(call $(1).OBJS)
endef

$(foreach driver,$(DRIVERS),$(eval $(call PROGRAM_template,$(driver))))

output_dir:
	@if [ ! -d objs ]; then mkdir objs; fi
	@if [ ! -d $(OUTPUT_DIR) ]; then mkdir -p $(OUTPUT_DIR); fi

clean:
	rm -rf $(OUTPUT_DIR)
	rm *.driver

tags:
	$(CTAGS) -f tags src/* include/*
