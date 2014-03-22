# files.mk
#
# list of drivers to be compiled
#
# jnider 17-05-2013

OUTPUT_DIR=objs
ROOT?=y
MEM_MANAGER?=y

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

