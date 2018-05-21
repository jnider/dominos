#!/bin/bash

# As usual, I never seem to remember all the options required for all of these different
# tools, so this script is meant to help my failing memory by running a QEMU instance
# with all of the correct parameters needed to test Dominos.

# This includes redirecting the serial output of the VM to a text file

if [[ "$1" = "debug" ]]; then
   DEBUG="-S -s -D qemu.log"
fi

QEMU=/usr/bin/qemu-system-x86_64
MEMORY=2048

$QEMU -m $MEMORY -cpu Haswell -cdrom dominos.iso -nographic # -serial file:serial1.txt $DEBUG
