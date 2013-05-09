#include "l4.h"

/* This is the root task.  It executes in user space even though it is
   compiled as part of the kernel.  In fact, it is the first user space
   task created as part of the kernel initialization.  The job of the root
   task is 2-fold:
   1) boot the system by loading all the appropriate drivers from initrd
   2) allocate the system resources during steady state
*/

void itoa (char* buf, int base, int d)
{
   // push   %ebp
   // mov    %esp,%ebp
   // sub    $0x24,%esp
   // mov    0x8(%ebp),%eax
   // mov    %eax,-0x4(%ebp)
   char *p = buf;
   char *p1, *p2;
   // mov    0x10(%ebp),%eax
   // mov    %eax,-0x10(%ebp)
   unsigned long ud = d;
   // movl   $0xa,-0x14(%ebp)
   int divisor = 10;

   // If %d is specified and D is minus, put `-' in the head. 
   // cmpl   $0x64,0xc(%ebp)
   // jne    101544 <k_itoa+0x44>
   // cmpl   $0x0,0x10(%ebp)
   // jns    101544 <k_itoa+0x44>
   if (base == 'd' && d < 0)
   {
      // mov    -0x4(%ebp),%eax
      // movb   $0x2d,(%eax)
      *p++ = '-';
      // lea    -0x4(%ebp),%eax
      // incl   (%eax)
      buf++;
      // mov    0x10(%ebp),%eax
      // neg    %eax
      ud = -d;
      // mov    %eax,-0x10(%ebp)
      // movl   $0xa,-0x14(%ebp)
      divisor = 10;
      // jmp    101551 <k_itoa+0x51>
   }
   // cmpl   $0x78,0xc(%ebp)
   // jne    101551 <k_itoa+0x51>
   else if (base == 'x')
      // movl   $0x10,-0x14(%ebp)
      divisor = 16;
   
   // Divide UD by DIVISOR until UD == 0.   
   do
   {
      // mov    -0x10(%ebp),%edx
      // lea    -0x14(%ebp),%eax
      // mov    %eax,-0x24(%ebp)
      // mov    %edx,%eax
      // mov    -0x24(%ebp),%ecx
      // mov    $0x0,%edx
      // divl   (%ecx)
      // mov    %edx,-0x18(%ebp)
      int remainder = ud % divisor;
      // mov    -0x4(%ebp),%eax
      // mov    %eax,-0x1c(%ebp)
      // lea    -0x4(%ebp),%eax
      // incl   (%eax)
      // cmpl   $0x9,-0x18(%ebp)
      // jg     101586 <k_itoa+0x86>
      // mov    -0x18(%ebp),%eax
      // mov    %al,-0x1d(%ebp)
      // addb   $0x30,-0x1d(%ebp)
      // jmp    101590 <k_itoa+0x90>
      // mov    -0x18(%ebp),%eax
      // mov    %al,-0x1d(%ebp)
      // addb   $0x57,-0x1d(%ebp)
      // mov    -0x1d(%ebp),%al
      // mov    -0x1c(%ebp),%edx
      // mov    %al,(%edx)
      // mov    -0x10(%ebp),%edx
      // lea    -0x14(%ebp),%ecx
      *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;

   // mov    %ecx,-0x24(%ebp)
   // mov    %edx,%eax
   // mov    -0x24(%ebp),%ecx
   // mov    $0x0,%edx
   // divl   (%ecx)   
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

void root_task_main(void)
{
   uint32 smash2;
   uint32 smash1;
   //char c = 'A';
   //int i;

   //for (i=0; i<10; i++)
   //   L4_DebugPutChar(c++);

   // first, some boot responsibilities
   printf("smash1: 0x%x\n", smash1);
   printf("smash2: 0x%x\n", smash2);
   while(1);
}

