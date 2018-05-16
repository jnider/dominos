#!/bin/bash

# set up the files for an iso image
ISOROOT=iso-temp
mkdir -p $ISOROOT

# put the kernel and initrd in the right locations
echo "Copying files for image"
cp kernel/joel4/joel4.elf $ISOROOT
cp init.cpio $ISOROOT

echo "Using GRUB 2"

# make sure the correct tools are available - grub-mkrescue fails silently if xorriso is not installed
xorriso=$(which xorriso)
if [[ ! -x $xorriso ]]; then
	echo "You must install xorriso for this to work"
	exit
fi

if [[ -d $ISOROOT/boot/grub ]]; then
	rm -f $ISOROOT/boot/grub/*
else
	mkdir -p $ISOROOT/boot/grub
fi

cp image/grub2.cfg $ISOROOT/boot/grub/grub.cfg
grub-mkrescue -o dominos.iso -- --eltorito-id JOEL $ISOROOT
