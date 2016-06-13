# files.mk
#
# list of drivers to be compiled
#
# jnider 17-05-2013

OUTPUT_DIR=objs

# decide which targets should be built
ROOT?=y
MEM_MANAGER?=y

# here we set up each individual target, and add each one to the overall
# list of targets called SERVICES. The SERVICES variable is used in the
# main makefile

ifeq ($(strip $(ROOT)), y)
	root.SRC = root/root_task.c
	root.OBJS = root/root_task.o
	SERVICES += root
endif

ifeq ($(strip $(MEM_MANAGER)), y)
	mem_manager.SRC = mem_manager/main.c
	mem_manager.OBJS = mem_manager/main.o
	# mem_manager.OBJS = $(mem_manager_SRC:%.c/%.o)
	SERVICES += mem_manager
endif

