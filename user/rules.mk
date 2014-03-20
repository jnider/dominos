# rules.mk
#
# makefile rules for building certain types of files
#
# jnider 17-08-2012

C_OBJS=$(C_SRC:%.c=objs/%.o)
S_OBJS=$(S_SRC:%.s=objs/%.o)
OBJS=$(S_OBJS) $(C_OBJS)

objs/%.o: %.s
	@echo "Assembling $<"
	@$(AS) $(ASMFLAGS) $(INCLUDE_PATHS) -c $< -o $@

objs/%.o: %.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# where should we look for matching source (.c and .s) files
vpath=. src
