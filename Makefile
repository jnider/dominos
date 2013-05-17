kerneldir=kernel/joel4

all:
	make -C $(kerneldir)
	./makeiso.sh grub2

init.cpio: initfiles.txt boot.txt
	cpio --format=newc -o < initfiles.txt > init.cpio
