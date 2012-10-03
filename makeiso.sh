#!/bin/bash

echo "Using SYSLINUX"
# find syslinux, which is the bootloader
SYSLINUX=`which syslinux`
SYSLINUXDIR=`echo $SYSLINUX | sed 's:/bin/:/share/:'`
[ -d "$SYSLINUXDIR" ] || SYSLINUXDIR="`echo $SYSLINUXDIR | sed 's:/share/:/lib/:'`"
[ -d "$SYSLINUXDIR" ] || {
       echo >&2 "Cannot find syslinux data directory!"
       exit 1
}

# set up the files for an iso image
ISOROOT=iso-temp
mkdir -p $ISOROOT

# put the files in the right locations
echo "Copying files for image"
cp kernel/joel4/joel4.elf $ISOROOT

echo "installing ISOLINUX-specific config files"
mkdir -p $ISOROOT/isolinux
cp $SYSLINUXDIR/isolinux.bin $ISOROOT/isolinux
cp image/syslinux.cfg $ISOROOT/isolinux
cp $SYSLINUXDIR/mboot.c32 $ISOROOT/isolinux

# now make an ISO 8660 El Torito bootable CD image
genisoimage -o dominos.iso -A dominos -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table -iso-level 3 -l $ISOROOT

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
