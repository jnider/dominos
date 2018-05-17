# Makefile
#
# makefile for building the joel4 kernel
#
# jnider 17-08-2012

# build the kernel and update tags
# this target must be defined before the other makefile fragments are included below
# specifically 'tools.mk' defines the 'tools' rule which would become the default rule
all: joel4.elf joel4.lib tags
	@echo "Build complete"

include tools.mk
include rules.mk
include files.mk

INCLUDE_PATHS=-Iinclude -Iuser
SRC_DIR=src
OUTPUT_DIR=objs
OUTPUT_NAME=joel4
DEBUG?=n
VERBOSE?=@
OPTIONS?=n

# if the OPTIONS option is set, print out all the variables in use
ifeq ($(OPTIONS),y)
all: options
endif

# if the DEBUG option is set, build additional debug files
ifeq ($(DEBUG),y)
all: debug
endif

# link the ELF - this should be moved to rules
%.elf: output_dir version $(OBJS)
	@echo "Linking $@"
	@$(LD) $(LDFLAGS) $(OBJS) -T $*.ld -o $@

# print all the variables
options:
	@echo "CC=$(CC)"
	@echo "CFLAGS=$(CFLAGS)"
	@echo "ASMFLAGS=$(ASMFLAGS)"
	@echo "LDFLAGS=$(LDFLAGS)"
	@echo "DEBUG=$(DEBUG)"

debug:
	@echo "Dumping assembler to $(OUTPUT_NAME).s"
	@objdump -dS $(OUTPUT_NAME).elf > $(OUTPUT_NAME).s
	@echo "Creating BOCHS debug symbol file $(OUTPUT_NAME).sym"
	@readelf -s $(OUTPUT_NAME).elf | $(AWK) -f sym.awk > $(OUTPUT_NAME).sym

output_dir:
	@if [ ! -d $(OUTPUT_DIR) ]; then echo "Creating output directory"; mkdir -p $(OUTPUT_DIR); fi

%.lib: 
	@echo "Creating joel4.lib"
	ar -ru joel4.lib objs/l4.o

version:
	@echo "Incrementing kernel version"
	@./inc_version.sh

init.cpio: initfiles.txt
	cpio -o < initfiles.txt > init.cpio

clean:
	rm -rf $(OUTPUT_DIR)
	rm -rf $(OUTPUT_NAME).elf

.PHONY: tags cloc

tags:
	$(CTAGS) -R -f tags $(SRC_DIR)/* include/*

cloc:
	$(CLOC) $(SRC_DIR) include

# include any P (prerequisite files) but don't freak out if they don't exist
-include $(C_SRC:%.c=objs/%.P)
