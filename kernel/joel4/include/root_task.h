#ifndef _ROOT_TASK__H
#define _ROOT_TASK__H

typedef unsigned int uint32;

typedef struct BootInfo
{
	uint32 initData;
	uint32 initDataSize;
	uint32 freeMem;
} BootInfo;

struct header_old_cpio {
		   unsigned short   c_magic;
		   unsigned short   c_dev;
		   unsigned short   c_ino;
		   unsigned short   c_mode;
		   unsigned short   c_uid;
		   unsigned short   c_gid;
		   unsigned short   c_nlink;
		   unsigned short   c_rdev;
		   unsigned short   c_mtime[2];
		   unsigned short   c_namesize;
		   unsigned short   c_filesize[2];
	   };

struct cpio_odc_header {
		   char    c_magic[6];
		   char    c_dev[6];
		   char    c_ino[6];
		   char    c_mode[6];
		   char    c_uid[6];
		   char    c_gid[6];
		   char    c_nlink[6];
		   char    c_rdev[6];
		   char    c_mtime[11];
		   char    c_namesize[6];
		   char    c_filesize[11];
	   };

typedef struct cpio_newc_header {
		   char    c_magic[6];
		   char    c_ino[8];
		   char    c_mode[8];
		   char    c_uid[8];
		   char    c_gid[8];
		   char    c_nlink[8];
		   char    c_mtime[8];
		   char    c_filesize[8];
		   char    c_devmajor[8];
		   char    c_devminor[8];
		   char    c_rdevmajor[8];
		   char    c_rdevminor[8];
		   char    c_namesize[8];
		   char    c_check[8];
	   } cpio_newc_header;

#define CPIO_NEWC_MAGIC "070701"

#endif // _ROOT_TASK__H

