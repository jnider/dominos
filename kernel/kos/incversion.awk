# this is an awk script to modify the build number
# in a header file automatically

{ if (FNR == 2) $3++ }
{ print >> "version.h" }
