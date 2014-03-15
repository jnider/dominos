# files.mk
#
# list of drivers to be compiled
#
# jnider 17-05-2013

OUTPUT_DIR=objs
ROOT?=y

ifeq ($(strip $(ROOT)), y)
	root_SRC = root/root_task.c
	root.OBJS = root/root_task.o
	SERVICES += root
endif

