#include "l4.h"
#include "cpio.h"

#define MAX_FILENAME 256

typedef Word cpio_handle;

int cpio_open_archive(void* buffer)
{
   k_printf("Opening CPIO archive from memory at: 0x%x\n", buffer);
   // read image & determine type
   char *magic = ((cpio_newc_header*)buffer)->c_magic;

   // if type is known, return handle
   if (magic[0] == '0' &&
      magic[1] == '7' &&
      magic[2] == '0' &&
      magic[3] == '7')
   {
      return 0;
   }

   return -1;
}

char* cpio_open_file(void* buffer, const char* filename, Word* filesize)
{
   cpio_newc_header* pFileHdr=(cpio_newc_header*)buffer;

   while (pFileHdr)
   {
      char temp[MAX_FILENAME+1];
      Word filenamesize;

      k_memcpy(temp, pFileHdr->c_namesize, 8); // name size is always 8 chars (ASCII)
      temp[8] = 0;
      filenamesize = k_atoh(temp);

      if (filesize)
      {
         k_memcpy(temp, pFileHdr->c_filesize, 8);
         temp[8] = 0;
         *filesize = k_atoh(temp);
      }

      if (filenamesize > MAX_FILENAME)
      {
         k_printf("File name too long (max=%i)\n", MAX_FILENAME);
         return 0;
      }

      // read the filename
      char* tmpFilename = (char*)pFileHdr + sizeof(cpio_newc_header);
      k_memcpy(temp, tmpFilename, filenamesize);
      temp[filenamesize] = 0;

      // filename is padded with 0's to a multiple of 2
      char* buffer = (char*)(((Word)pFileHdr + sizeof(cpio_newc_header) + filenamesize + 3) & 0xFFFFFFFC);

      // compare it
      if (k_strcmp(filename, temp) == 0)
      {
         k_printf("Found %s!\n", filename);
         return buffer;
      }

      if (k_strcmp("TRAILER!!!", temp) == 0)
      {
         k_printf("End of archive\n");
         return 0;
      }

      // skip to the next file (file size is also padded to a multiple of 2
      pFileHdr = (cpio_newc_header*)(buffer + ((*filesize + 3) & 0xFFFFFFFC));
   }

   return 0;
}

