# rules.mk
#
# makefile rules for building certain types of files
#
# jnider 17-08-2012

C_OBJS=$(C_SRC:%.c=objs/%.o)
OBJS=$(C_OBJS)

objs/%.o: src/%.c
	echo "Compiling $<"
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@
