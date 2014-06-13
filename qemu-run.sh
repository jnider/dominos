#!/bin/bash

# As usual, I never seem to remember all the options required for all of these different
# tools, so this script is meant to help my failing memory by running a QEMU instance
# with all of the correct parameters needed to test Dominos.

# This includes redirecting the serial output of the VM to a text file

if [[ $1 == "gdb" ]]; then
   debug="-gdb tcp::1234 -S"
   echo "GDB on"
fi

qemu-system-i386 -cdrom dominos.iso -sdl -serial file:serial1.txt $debug -D qemu.log
