# The main makefile for Dominos
# Builds the kernel, drivers, and builds a boot image

# one day, we may support other kernels
export KERNEL_DIR=kernel/joel4
TOP=`pwd`

# default target - build it all
all: tools kernel drivers init.cpio
	./makeiso.sh grub2

.PHONY: kernel drivers

# check to make sure all the correct tools exist
# that are necessary to complete the build
tools:

# make the selected kernel (so far, only JOEL4 has been tried)
kernel:
	make -C $(KERNEL_DIR)

# drivers are not part of the kernel - build them separately
drivers:
	make -C drivers

# this is like linux's 'initrd' - needed for booting
init.cpio: initfiles.txt boot.txt drivers
	cpio --format=newc -o < initfiles.txt > init.cpio
