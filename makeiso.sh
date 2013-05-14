#!/bin/bash

PS3='>'

loader=$1
if [[ ! $loader ]]; then
echo "Select a boot loader:"
select loader in grub grub2 syslinux ; do
	break
done
fi

# set up the files for an iso image
ISOROOT=iso-temp
mkdir -p $ISOROOT

# put the files in the right locations
echo "Copying files for image"
cp kernel/joel4/joel4.elf $ISOROOT
cp init.cpio $ISOROOT

if [[ $loader == "grub" ]] ; then
	echo "Using GRUB Legacy"
	mkdir -p $ISOROOT/boot/grub
	#cp image/grub.cfg $ISOROOT/boot/grub/grub.cfg
	cp /usr/lib/grub/i386-pc/* $ISOROOT/boot/grub/
	iso_options='-R -b boot/grub/boot.img'
fi

if [[ $loader == "grub2" ]] ; then
	echo "Using GRUB 2"
	mkdir -p $ISOROOT/boot/grub
	rm $ISOROOT/boot/grub/*
	cp image/grub2.cfg $ISOROOT/boot/grub/grub.cfg
	grub-mkrescue -o dominos.iso $ISOROOT
	exit
fi

if [[ $loader == "syslinux" ]] ; then
echo "Using SYSLINUX"
# find syslinux, which is the bootloader
SYSLINUX=`which syslinux`
SYSLINUXDIR=`echo $SYSLINUX | sed 's:/bin/:/share/:'`
[ -d "$SYSLINUXDIR" ] || SYSLINUXDIR="`echo $SYSLINUXDIR | sed 's:/share/:/lib/:'`"
[ -d "$SYSLINUXDIR" ] || {
       echo >&2 "Cannot find syslinux data directory!"
       exit 1
}

echo "installing ISOLINUX-specific config files"
mkdir -p $ISOROOT/isolinux
cp $SYSLINUXDIR/isolinux.bin $ISOROOT/isolinux
cp image/syslinux.cfg $ISOROOT/isolinux
cp $SYSLINUXDIR/mboot.c32 $ISOROOT/isolinux

iso_options='-b isolinux/isolinux.bin -c isolinux/boot.cat '
fi

# now make an ISO 8660 El Torito bootable CD image
genisoimage -o dominos.iso -A dominos $iso_options -no-emul-boot -boot-load-size 4 -boot-info-table -iso-level 3 -l $ISOROOT

# clean up
#rm -rf $TEMPDIR


# --- the old way ----
# put a partition table (MBR) on our new disk
#echo "Creating MBR"
#parted -s $dev mklabel msdos

# add a single bootable partition
#echo "Adding bootable partition"
#sfdisk $dev -u S -C 4 -H 255 -S 63 << EOF
#63,64197,0xc,*
#EOF
