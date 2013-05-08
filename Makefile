kerneldir=kernel/joel4

all:
	make -C $(kerneldir)
	./makeiso.sh
