#!/bin/bash

# increments the version number of the kernel before each build
# I'm not too worried about if the build fails or not - there
# are lots of numbers and we won't run out

# if the file doesn't exist, we have nothing to do
if [ ! -e include/version.h ] ; then
	echo "Can't find version.h - quitting"
	exit
fi

# find out the current version number
ver=$(grep '_KOS_BUILD' include/version.h | gawk '{ print $3 }')

echo "Current version: " $ver
ver=$(($ver+1))

# now write out a new copy of the file
echo "#ifndef __VERSION_H" > include/version.h
echo "#define __VERSION_H" >> include/version.h
echo "#define _KOS_BUILD $ver" >> include/version.h
echo "#endif" >> include/version.h

