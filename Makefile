# The main makefile for Dominos
#
# Builds the kernel, drivers, and builds a boot image
# You can build everything from here (or at least you will be able to)

# Only the JOEL4 kernel is supported at this time
# one day, we may support other kernels
export KERNEL_DIR=kernel/joel4
TOP=`pwd`

# default target - build it all
# 'make all' or just 'make' will build this target
all: tools kernel drivers init.cpio
	./makeiso.sh grub2

.PHONY: kernel drivers

# set DEBUG to off by default
DEBUG?=n

# if the user called 'make debug' turn on debugging
debug: DEBUG:=y

# target for 'make debug' which just builds everything
debug: all

# check to make sure all the correct tools exist
# that are necessary to complete the build
tools:

# make the selected kernel (so far, only JOEL4 has been tried)
kernel:
	make -C $(KERNEL_DIR) DEBUG=$(DEBUG)

# drivers are not part of the kernel - build them separately
drivers:
	make -C drivers

# this is like linux's 'initrd' - needed for booting
init.cpio: initfiles.txt boot.txt drivers
	cpio --format=newc -o < initfiles.txt > init.cpio

clean:
	make -C $(KERNEL_DIR) clean
	make -C drivers clean
	@if [ -e init.cpio ] ; then rm init.cpio; fi
	@if [ -e dominos.iso ] ; then rm dominos.iso; fi
