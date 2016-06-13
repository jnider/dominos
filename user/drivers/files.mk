# files.mk
#
# list of drivers to be compiled
#
# jnider 17-05-2013

OUTPUT_DIR=objs
PCIE?=y

ifeq ($(strip $(PCIE)), y)
	pcie_SRC = bus/pcie/pcie.c
	pcie.OBJS = bus/pcie/pcie.o
	DRIVERS += pcie
endif

