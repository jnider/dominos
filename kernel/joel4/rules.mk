# rules.mk
#
# makefile rules for building certain types of files
#
# jnider 17-08-2012

C_OBJS=$(C_SRC:%.c=objs/%.o)
S_OBJS=$(S_SRC:%.s=objs/%.o)
OBJS=$(S_OBJS) $(C_OBJS)

# don't delete my .o files!
.PRECIOUS: objs/%.o

objs/%.o: src/%.s
	@echo "Assembling $<"
	@$(AS) $(ASMFLAGS) $(INCLUDE_PATHS) -c $< -o $@

objs/%.o: src/%.c
	@echo "Compiling $<"
	@$(MAKEDEPEND); \
      cp $(*F).d $(*F).P; \
      sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
          -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> objs/$(*F).P; \
      rm -f objs/$(*F).d
	@$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

MAKEDEPEND=$(CC) -M $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $(*F).d
