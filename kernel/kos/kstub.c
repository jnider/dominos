////////////////////////////////////////////////////////////////////////////////
/// @file   kstub.c
/// @short  contains stubs of kernel functions
/// @author J.Nider
/// @date   30/10/2009
////////////////////////////////////////////////////////////////////////////////
/// user code must access kernel functions through these stubs!
////////////////////////////////////////////////////////////////////////////////

#include "kstdio.h"
#include "kvideo.h"
#include "timer.h"
#include "ktimer.h"
#include "kstub.h"

extern unsigned int csos_segment;

void putchar(int c)
{
   // change to OS data segment
   __USE_DSOS();

#ifdef FAR_CALLS
   __putchar(c);
#else
   unsigned int callGate[2];

   // call the kernel function
   callGate[0] = (unsigned int)__putchar;
   callGate[1] = SEGMENT_INDEX(csos_segment, 0, 3); //0x1b;

   __ASM("pushl %0\n" :: "m" (c));
   __ASM("lcall *%0" : : "m" (*callGate));
   __ASM("add $0x10, %esp\n");
#endif

   // change data segment back
   __USE_DSAPP();
}

int puts(const char *c)
{
   while (*c)
   {
      putchar(*c);
      c++;
   }

   return 0;
}
