#include "l4.h"
#include "cpio.h"

/* This is the root task. The jobs of the root task:
   1. Physical memory manager - allocate physical memory pages for userspace
   2. Pager - map virtual pages into memory space
   3. I/O space management
   4. scheduler
*/

#define MAX_FILENAME 31
#define READ_BUF_SIZE 512
#define INVALID_HANDLE 0xFFFFFFFF

#define DEFAULT_UTCB 0x801000

static void* cpioArchive;
static Word threadId=2;

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

cpio_handle cpio_open_archive(void* buffer)
{
   // read image & determine type
   char *magic = ((cpio_newc_header*)buffer)->c_magic;

   // if type is known, return handle
   if (magic[0] == '0' &&
      magic[1] == '7' &&
      magic[2] == '0' &&
      magic[3] == '7')
   {
      cpioArchive = buffer;
      return 0;
   }

   return INVALID_HANDLE;
}

char* cpio_open_file(cpio_handle h, const char* filename, Word* filesize)
{
   cpio_newc_header* pFileHdr=(cpio_newc_header*)cpioArchive;

   while (pFileHdr)
   {
      char temp[MAX_FILENAME+1];
      Word filenamesize;

      memcpy(temp, pFileHdr->c_namesize, 8); // name size is always 8 chars (ASCII)
      temp[8] = 0;
      filenamesize = atoh(temp);

      memcpy(temp, pFileHdr->c_filesize, 8);
      temp[8] = 0;
      *filesize = atoh(temp);

      if (filenamesize > MAX_FILENAME)
      {
         printf("File name too long (max=%i)\n", MAX_FILENAME);
         return 0;
      }

      // read the filename
      char* tmpFilename = (char*)pFileHdr + sizeof(cpio_newc_header);
      memcpy(temp, tmpFilename, filenamesize);
      temp[filenamesize] = 0;

      // filename is padded with 0's to a multiple of 2
      char* buffer = (char*)(((Word)pFileHdr + sizeof(cpio_newc_header) + filenamesize + 3) & 0xFFFFFFFC);

      // compare it
      if (strcmp(filename, temp) == 0)
      {
         printf("Found %s!\n", filename);
         return buffer;
      }

      if (strcmp("TRAILER!!!", temp) == 0)
      {
         printf("End of archive\n");
         return 0;
      }

      // skip to the next file (file size is also padded to a multiple of 2
      pFileHdr = (cpio_newc_header*)(buffer + ((*filesize + 3) & 0xFFFFFFFC));
   }

   return 0;
}

/* this needs to be improved to support reused thread ID's */
static L4_ThreadId GetFreeThreadId(void)
{
   return threadId++;
}

static int LoadProgram(const char* name, const char* buffer, Word size)
{
   Word rc;
   L4_ThreadId newThread;

   printf("Loading program %s from 0x%x size %i bytes\n", name, buffer, size);

   // loading consists of creating a task, and a memory space. The memory space is created
	// automatically by using a thread id that does not have an address space associated with it
	// yet (in fact, it doesn't even exist).
   newThread = GetFreeThreadId();
   printf("Creating thread %i\n", newThread);
   rc = L4_ThreadControl(newThread, newThread, L4_Myself(), L4_Myself(), (void*)DEFAULT_UTCB);

   return 0;
}

void main(void)
{
   char progFilename[MAX_FILENAME+1];
   char temp[20];
   char* archBuffer;
   char* progBuffer;
   BootInfo* pInfo;
   Word archFilesize; // size of the archive file
   Word progFilesize; // size of the program file

   Word apiVersion;
   Word apiFlags;
   Word kernelId;

   L4_KIP* pKip = L4_KernelInterface(&apiVersion, &apiFlags, &kernelId);
   if (L4_ValidateKIPMagic(pKip) != 0)
   {
      printf("KIP magic number is incorrect: %c%c%c%c\n", pKip->magic[0], pKip->magic[1], pKip->magic[2], pKip->magic[3]);
      while(1);
   }
   pInfo = (BootInfo*)(pKip->bootInfo + (Word)pKip);

   printf("Boot info @ %x\n", pInfo);
   if (L4_GET_API_VERSION(apiVersion) != L4_API_VERSION_X2)
   {
      printf("Unknown API Version: 0x%x\n", apiVersion);
      while(1);
   }
   printf("API Flags: 0x%x\n", apiFlags);
   printf("Kernel ID: 0x%x\n", kernelId);

   if (pInfo->magic != BOOT_INFO_MAGIC)
   {
      printf("Boot info magic wrong!\n");
      while(1);
   }

   GenericBootRecord* pRec = (GenericBootRecord*)(pInfo->first + (Word)pKip);
   if (pRec->type != BOOT_RECORD_TYPE_SIMPLE_MODULE)
   {
      printf("Boot record type is wrong %i\n", pRec->type);
      while(1);
   }

   SimpleModule* pMod = (SimpleModule*)pRec;

   // first, some boot responsibilities
   cpio_handle archive = cpio_open_archive((void*)pMod->start);
   if (archive == INVALID_HANDLE)
   {
      printf("Init module is in an unknown file format - expected CPIO newc format\n");
      while(1);
   }

   // set up memory management

   // set up timer/scheduler

   // find the file containing the list of modules
   archBuffer = cpio_open_file(archive, "boot.txt", &archFilesize);
   if (!archBuffer)
   {
      printf("Cannot find 'boot.txt' in the init module - don't know how to boot!\n");
      while(1);
   }
   //printf("archFilesize: %i bytes\n", archFilesize);

   // run through the list of all modules and load them one by one
   char* src = archBuffer; 
   char* dest = progFilename;
   int i;

   for (i=0; i<MAX_FILENAME+1; i++)
      progFilename[i] = 0;
   
   while (src <= (archBuffer + archFilesize))
   {
      if ((dest-progFilename) == MAX_FILENAME)
      {
         *dest = 0;
         dest = progFilename;
         printf("Filename too long: %s\n", progFilename);
      }

      switch(*src)
      {
      case '\n':
      case '\r': 
         *dest = 0;
         dest = progFilename;
         printf("Loading: %s\r\n", progFilename);
         
         // get the program image from the archive
         progBuffer = cpio_open_file(archive, progFilename, &progFilesize);
         if (!progBuffer)
            printf("Can't find %s in archive\n", progFilename);
         else
            LoadProgram(progFilename, progBuffer, progFilesize);
         break;

      default:
         *dest++ = *src;
      }

      src++;
   }
   // enable the scheduler

   printf("Root task complete\n");

   while(1);
}

