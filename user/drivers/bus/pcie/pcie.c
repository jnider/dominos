/* PCIe root complex driver */

#include "l4.h"

/* eventually these functions should be replaced with stdclib */
static void itoa (char* buf, int base, int d)
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

int strcmp(const char* a, const char* b)
{
   while (*a && *b)
   {
      if (*a < *b)
         return -1;
      if (*a > *b)
         return 1;
      a++;
      b++;
   }

   if (*a)
      return 1;

   if (*b)
      return -1;

   return 0;
}

static void printf (const char *format, ...)
{
   char **arg = (char **) (&format);
   int c;
   char buf[20];

   arg++;

   while ((c = *format++) != 0)
   {
      if (c != '%')
         L4_DebugPutChar(c);
      else
      {
         char *p;

         c = *format++;

         // get rid of leading numbers in formatting
         while (c >= '0' && c <='9')
            c = *format++;

         switch (c)
         {
         case 'd':
         case 'i':
         case 'u':
         case 'x':
            itoa (buf, c, *((int*)arg++));
            p = buf;
            goto string;
            break;

         case 's':
            p = *arg++;
            if (! p)
               p = "(null)";

            string:
               while (*p)
                  L4_DebugPutChar(*p++);
            break;

         default:
            L4_DebugPutChar(*((int *) arg++));
            break;
         }
      }
   }
}

static int atoh(const char* str)
{
   int acc = 0;
   while (*str)
   {
      acc <<= 4;
      if (*str >= '0' && *str <= '9')
         acc += (*str) - '0';
      else if (*str >= 'A' && *str <= 'F')
         acc += (*str) - 'A' + 10;
      else if (*str >= 'a' && *str <= 'f')
         acc += (*str) - 'a' + 10;
      else
         return 0;
      
      str++;
   }
   return acc;
}

static void memcpy(void* dest, void* src, Word size)
{
   Word i;
   for (i=0; i < size; i++)
      *(char*)dest++ = *(char*)src++;
}

void main(void)
{
   volatile a = 1;

   printf("PCIe Driver Running!\n");
   a++;

   while (1);
}

