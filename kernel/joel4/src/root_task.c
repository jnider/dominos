#include "l4.h"
#include "root_task.h"

/* This is the root task.  It executes in user space even though it is
   compiled as part of the kernel.  In fact, it is the first user space
   task created as part of the kernel initialization.  The job of the root
   task is 2-fold:
   1) boot the system by loading all the appropriate drivers from initrd
   2) allocate the system resources during steady state
*/

void itoa (char* buf, int base, int d)
{
   char *p = buf;
   char *p1, *p2;
   unsigned long ud = d;
   int divisor = 10;

   // If %d is specified and D is minus, put `-' in the head. 
   if (base == 'd' && d < 0)
   {
      *p++ = '-';
      buf++;
      ud = -d;
      divisor = 10;
   }
   else if (base == 'x')
      divisor = 16;
   
   // Divide UD by DIVISOR until UD == 0.   
   do
   {
      int remainder = ud % divisor;
      *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
   } while (ud /= divisor);

   // Terminate BUF
   *p = 0;

   // Reverse BUF
   p1 = buf;
   p2 = p - 1;
   while (p1 < p2)
   {
      char tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
      p1++;
      p2--;
   }
}

void printf (const char *format, ...)
{
   char **arg = (char **) (&format);
   int c;
   char buf[20];

   arg++;

   while ((c = *format++) != 0)
   {
      if (c != '%')
      {
         L4_DebugPutChar(c);
      }
      else
      {
         char *p;

         c = *format++;

         // get rid of leading numbers in formatting
         while (c >= '0' && c <='9')
         {
            c = *format++;
         }

         switch (c)
         {
         case 'd':
         case 'i':
         case 'u':
         case 'x':
            k_itoa (buf, c, *((int *) arg++));
            p = buf;
            goto string;
            break;

         case 's':
            p = *arg++;
            if (! p)
               p = "(null)";

            string:
               while (*p)
               {
                  L4_DebugPutChar(*p++);
               }
            break;

         default:
            L4_DebugPutChar(*((int *) arg++));
            break;
         }
      }
   }
}

void root_task_main(BootInfo info)
{
   // first, some boot responsibilities
   printf("init image @ 0x%x size: %i\n", info.initData, info.initDataSize);

	// read image & determine type
	char *magic = ((cpio_newc_header*)info.initData)->c_magic;
	printf("%x %x %x %x %x %x\n", magic[0], magic[1], magic[2], magic[3], magic[4], magic[5]);

	// set up memory management

	// set up timer/scheduler

   while(1);
}

