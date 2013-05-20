kerneldir=kernel/joel4

all: build_all init.cpio
	./makeiso.sh grub2

build_all:
	make -C $(kerneldir)
	make -C drivers

init.cpio: initfiles.txt boot.txt
	cpio --format=newc -o < initfiles.txt > init.cpio
