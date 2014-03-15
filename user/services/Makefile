# Makefile
#
# makefile for building Dominos services
#
# jnider 14-03-2014

include ../tools.mk
include ../rules.mk
include files.mk

CONFIG?=release
SRC_DIR=src

SERVICE_TARGETS=$(SERVICES:%=%.service)

all: output_dir $(SERVICE_TARGETS) 
	@echo "Build complete"

define PROGRAM_template
$(SERVICES).service: ../objs/crt0.o $$($(1).service)
	@echo building $$@
	$(LD) $(LDFLAGS) $$^ -T ../user.ld ../../kernel/joel4/joel4.lib -o $$@

$(1).service: $(call $(1).OBJS)
endef

$(foreach service,$(SERVICES),$(eval $(call PROGRAM_template,$(service))))

output_dir:
	@if [ ! -d objs ]; then mkdir objs; fi
	@if [ ! -d $(OUTPUT_DIR) ]; then mkdir -p $(OUTPUT_DIR); fi

clean:
	rm -rf $(OUTPUT_DIR)
	rm *.service

tags:
	$(CTAGS) -f tags src/* include/*
