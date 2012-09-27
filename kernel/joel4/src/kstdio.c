////////////////////////////////////////////////////////////////////////////////
/// @file   kstdio.cpp
/// @short  Kernel implementation of standard C functions
/// @author J.Nider
/// @date   01/07/2007
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "kstdio.h"
#include "kvideo.h"
//#include "uart.h"

#ifdef WIN32
#include <windows.h>
#include <stdio.h>
#endif // WIN32

char _digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

//////////
///@author  J.Nider
///@date    11-Jun-2007 03:51:23 PM
///@brief   Kernel version of memset()
///@param   ptr Buffer to set
///@param   value Value to set the buffer to
///@param   size Length of the buffer
//////////
void k_memset(void* ptr, char value, int size)
{
   int i;
   char* pChar = (char*)ptr;
   for (i=0; i < size; i++)
   {
      *pChar++ = value;
   }
}

//////////
///@author  J.Nider
///@date    11-Jun-2007 03:52:06 PM
///@brief   Kernel version of memcpy
///@param   dest Destination buffer
///@param   src Source buffer
///@param   size Size of the source buffer
///
/// Copies memory from src to dest
//////////
void k_memcpy(void* dest, const void* src, int size)
{
   int i;
   char* pDest = (char*)dest;
   char* pSrc = (char*)src;
   for (i=0; i < size; i++)
   {
      *pDest++ = *pSrc++;
   }
}

//////////
///@author  J.Nider
///@date    11-Jun-2007 03:53:34 PM
///@brief   Outputs a byte to memory mapped I/O
//////////
void outportb (unsigned short _port, unsigned char _data)
{
    __ASM __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}

//////////
///@author  J.Nider
///@date    11-Jun-2007 03:53:54 PM
///@brief   Reads a byte from memory mapped I/O
//////////
unsigned char inportb(unsigned short port)
{
   unsigned char rv;
   __ASM __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (port));
   return rv;
}

//////////
///@author  J.Nider
///@date    11-Jun-2007 03:53:34 PM
///@brief   Outputs a dword to memory mapped I/O
//////////
void outportl (unsigned short _port, unsigned long _data)
{
    __ASM __volatile__ ("outl %1, %0" : : "dN" (_port), "a" (_data));
}

//////////
///@author  J.Nider
///@date    11-Jun-2007 03:53:54 PM
///@brief   Reads a dword from memory mapped I/O
//////////
unsigned long inportl(unsigned short port)
{
   unsigned long rv;
   __ASM __volatile__ ("inl %1, %0" : "=a" (rv) : "dN" (port));
   return rv;
}

////////////////////////////////////////////////////////////////////////////////
///@author  J.Nider
///@date    11-Jun-2007 03:54:07 PM
///@brief   Kernel version of printf()
///@param   format Formatting string
///@param   ... variable parameters
////////////////////////////////////////////////////////////////////////////////
void k_printf (const char *format, ...)
{
#ifdef WIN32
   char temp[1024];
   va_list list;
   va_start(list, format);
   vsprintf(temp, format, list);
   OutputDebugString(temp);
   va_end(list);
#else
   char **arg = (char **) (&format);
   int c;
   char buf[20];

   arg++;

   while ((c = *format++) != 0)
   {
      if (c != '%')
      {
         //UARTTransmit(COM1, c);
         k_putchar(c);
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
                  //UARTTransmit(COM1, *p);
                  k_putchar(*p++);
               }
            break;

         default:
            k_putchar(*((int *) arg++));
            break;
         }
      }
   }
#endif // WIN32
}

/*
void k_vprintf (const char *format, va_list args)
{ 
}
*/

//////////
///@author  J.Nider
///@date    11-Jun-2007 03:54:58 PM
///@brief   int to ascii
///@param   buf Destination buffer - must already be allocated
///@param   base Radix base (i.e. 10 for decimal)
///@param   d  Integer to be converted
//////////
void k_itoa (char* buf, int base, int d)
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

//////////
///@author  J.Nider
///@date    11-Jun-2007 03:56:11 PM
///@brief   Kernel version of strcpy()
///@param   dest Destination string
///@param   src Source string
///
/// Copies a null-terminated string from src to dest.
//////////
void k_strcpy(char* dest, const char* src)
{
   while (*src)
      *dest++ = *src++;
   *dest = 0;  // null terminate the new string
}

char* k_strncpy(char* s1, const char* s2, size_t n)
{
    char * rc = s1;
    while ( ( n > 0 ) && ( *s1++ = *s2++ ) )
    {
        /* Cannot do "n--" in the conditional as size_t is unsigned and we have
        to check it again for >0 in the next loop.
        */
        --n;
    }
    while ( --n )
    {
        *s1++ = '\0';
    }
    return rc;
}

int k_strlen(const char* a)
{
   int len = 0;
   while (*a++)
      len++;
   return len;
}

int k_strcmp(const char* a, const char* b)
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

int k_strncmp(const char* s1, const char* s2, size_t n)
{
    while ( n && ( *s1 == *s2 ) )
    {
        ++s1;
        ++s2;
        --n;
    }
    if ( ( n == 0 ) )
    {
        return 0;
    }
    else
    {
        return ( *s1 - *s2 );
    }
}

int k_isspace(char c)
{
   if (c == ' ' || c== '\r' || c== '\n' || c== '\t')
      return 1;
   return 0;
}

int substring (const char *s1, const char *s2)
{
  while (*s1 == *s2)
    {
      /* The strings match exactly. */
      if (! *(s1++))
	return 0;
      s2 ++;
    }

  /* S1 is a substring of S2. */
  if (*s1 == 0)
    return -1;

  /* S1 isn't a substring. */
  return 1;
}

void k_hexout(const char* pBuffer, unsigned int length)
{
   const char hex[] = "0123456789ABCDEF";
   char summary[16];

   if (length > 1024)
      length = 1024;

   // first print the address
   //k_printf("pBuffer: 0x%x\n", pBuffer);

   // print rows of 16 single byte hex chars
   unsigned int i, j;
   for (i=0; i < length; i++)
   {
      char c1 = hex[((*pBuffer) >> 4) & 0x0F];
      char c2 = hex[(*pBuffer) & 0x0F];
      k_putchar(c1);
      k_putchar(c2);
      k_putchar(' ');
      if (*pBuffer > 0x30 && (unsigned char)*pBuffer < 0x80)
         summary[i%16] = *pBuffer;
      else
         summary[i%16] = '.';

      if (i % 16 == 15)
      {
         for (j = 0; j < 16; j++)
         {
            k_putchar(summary[j]);
         }
         k_putchar('\n');
      }
      pBuffer++;
   }
}

char* k_strtok(char* s1, const char* s2)
{
    static char * tmp = NULL;
    const char * p = s2;

    if ( s1 != NULL )
    {
        /* new string */
        tmp = s1;
    }
    else
    {
        /* old string continued */
        if ( tmp == NULL )
        {
            /* No old string, no new string, nothing to do */
            return NULL;
        }
        s1 = tmp;
    }

    /* skipping leading s2 characters */
    while ( *p && *s1 )
    {
        if ( *s1 == *p )
        {
            /* found seperator; skip and start over */
            ++s1;
            p = s2;
            continue;
        }
        ++p;
    }

    if ( ! *s1 )
    {
        /* no more to parse */
        return ( tmp = NULL );
    }

    /* skipping non-s2 characters */
    tmp = s1;
    while ( *tmp )
    {
        p = s2;
        while ( *p )
        {
            if ( *tmp == *p++ )
            {
                /* found seperator; overwrite with '\0', position tmp, return */
                *tmp++ = '\0';
                return s1;
            }
        }
        ++tmp;
    }

    /* parsed to end of string */
    tmp = NULL;
    return s1;
}

void * k_memchr(const void * s, int c, size_t n)
{
    const unsigned char * p = (const unsigned char *) s;
    while ( n-- )
    {
        if ( *p == (unsigned char) c )
        {
            return (void *) p;
        }
        ++p;
    }
    return NULL;
}

int isspace(char c)
{
   return c==' ';
}

int k_atoi(const char* s)
{
   int rc = 0;
   char sign = '+';
   const char * x;
   /* TODO: In other than "C" locale, additional patterns may be defined     */
   while ( isspace( *s ) ) ++s;
   if ( *s == '+' ) ++s;
   else if ( *s == '-' ) sign = *(s++);
   while ( ( x = (char*)k_memchr( _digits, *(s++), 10 ) ) != NULL )
   {
      rc = rc * 10 + ( x - _digits );
   }
   return ( sign == '+' ) ? rc : -rc;
}

inline void halt()
{
   for (;;);
}

