kerneldir=kernel/joel4

all:
	make -C $(kerneldir)
	./makeiso.sh grub2

init.cpio: initfiles.txt
	cpio -o < initfiles.txt > init.cpio
