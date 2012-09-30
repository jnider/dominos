#!/bin/bash

# use ~/out.img as the default name if there isn't one provided
if [ "$1" = '' ]
then IMAGE=~/out.img;
else IMAGE=$1;
fi
echo "Output file name is: "$IMAGE

if [ $BOOTLOADER = GRUB ]
then echo "Using GRUB"
else echo "Using SYSLINUX"
fi

# find syslinux, which is the bootloader
SYSLINUX=`which syslinux`
SYSLINUXDIR=`echo $SYSLINUX | sed 's:/bin/:/share/:'`
[ -d "$SYSLINUXDIR" ] || SYSLINUXDIR="`echo $SYSLINUXDIR | sed 's:/share/:/lib/:'`"
[ -d "$SYSLINUXDIR" ] || {
       echo >&2 "Cannot find syslinux data directory!"
       exit 1
}

# create an empty file if needed
if [ ! -e $IMAGE ]
then
echo "Creating an empty image file"
dd if=/dev/zero of=$IMAGE count=64260
fi

if [ ! -e $IMAGE ]
then echo $IMAGE "doesn't exist!"
exit 1
fi

# map the file as a device so linux will think its a disk
# find the first available loop device
dev=`losetup -f`
echo "Using" $dev
ret=`losetup $dev $IMAGE`

# copy pre-made partition table to image
# this fails on certain file systems, such as vboxsf
dd if=image/mbr.bin of=$dev count=1

# show what's been done so far
# blockdev --rereadpt $dev # - for some reason, the kernel team doesn't want this to work
partx -a $dev # this is a work-around for now
parted -s $dev print

# now make a file system
partition=$dev'p1'
if [ ! -e $partition ]
then echo $partition "doesn't exist!"
exit 1
fi
echo "formatting" $partition
mkdosfs -I $partition

# and put a boot loader on it
echo "adding syslinux"
syslinux -i -s $partition

# mount the boot file system
BOOTFS=/media/bootimg
mkdir -p $BOOTFS
mount $partition $BOOTFS

# put the files in the right locations
echo "Copying files for image"
BOOTDIR=$BOOTFS #/boot/syslinux
mkdir -p $BOOTDIR
cp image/syslinux.cfg $BOOTDIR
cp image/msg.txt $BOOTDIR
cp /usr/lib/syslinux/mboot.c32 $BOOTDIR
cp kernel/joel4/joel4.elf $BOOTDIR
#cp kernel/fiasco/bootstrap.elf $BOOTDIR
ls -al $BOOTDIR

# unmount
echo "Unmounting"
umount $BOOTFS

# remove partition devices
echo "Deleting partition device" $partition
partx -d $partition

# unmap the file
echo "Deleting loop device" $dev
losetup -d $dev

# set up the files for an iso image
TEMPDIR=iso-temp
mkdir -p $TEMPDIR
cp $IMAGE $TEMPDIR/out.img

# now make an ISO 8660 El Torito bootable CD image
genisoimage -o dominos.iso -A dominos -b out.img -hard-disk-boot -iso-level 3 -l $TEMPDIR

# clean up
rm -rf $TEMPDIR


# --- the old way ----
# put a partition table (MBR) on our new disk
#echo "Creating MBR"
#parted -s $dev mklabel msdos

# add a single bootable partition
#echo "Adding bootable partition"
#sfdisk $dev -u S -C 4 -H 255 -S 63 << EOF
#63,64197,0xc,*
#EOF
