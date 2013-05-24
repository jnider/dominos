export KERNEL_DIR=kernel/joel4
TOP=`pwd`

all: tools kernel drivers init.cpio
	./makeiso.sh grub2

.PHONY: kernel drivers

# check to make sure all the correct tools exist
# that are necessary to complete the build
tools:

kernel:
	make -C $(KERNEL_DIR)

drivers:
	make -C drivers

init.cpio: initfiles.txt boot.txt drivers
	cpio --format=newc -o < initfiles.txt > init.cpio
