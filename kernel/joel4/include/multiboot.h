#ifndef _MULTIBOOT__H
#define _MULTIBOOT__H

/* multiboot.h - the header for Multiboot */
     /* Copyright (C) 1999, 2001  Free Software Foundation, Inc.

        This program is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 2 of the License, or
        (at your option) any later version.

        This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this program; if not, write to the Free Software
        Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA. */

/* Macros. */
#define MULTIBOOT_HEADER_MAGIC          0x1BADB002 /* The magic number for the Multiboot header. */
#define MULTIBOOT_BOOTLOADER_MAGIC      0x2BADB002/* The magic number passed by a Multiboot-compliant boot loader. */
#define MULTIBOOT_MEMORY         0x00000001 /* mem_lower and mem_upper */
#define MULTIBOOT_BOOT_DEVICE    0x00000002 /* boot_device */
#define MULTIBOOT_CMD_LINE       0x00000004 /* pCmdline */
#define MULTIBOOT_MODULES        0x00000008 /* mods_count and pModule */
#define MULTIBOOT_SYMBOL_AOUT    0x00000010 /* aout symbols */
#define MULTIBOOT_SYMBOL_ELF     0x00000020 /* elf symbols */
#define MULTIBOOT_MEMORY_MAP     0x00000040 /* memory map */
#define MULTIBOOT_DRIVES         0x00000080
#define MULTIBOOT_CONFIG_TABLE   0x00000100
#define MULTIBOOT_LOADER_NAME    0x00000200
#define MULTIBOOT_APM_TABLE      0x00000400
#define MULTIBOOT_VBE            0x00000800


/* The flags for the Multiboot header. */
#ifdef __ELF__
# define MULTIBOOT_HEADER_FLAGS         0x00000003
#else
# define MULTIBOOT_HEADER_FLAGS         0x00010003
#endif

//#define STACK_SIZE                      0x4000  /* The size of our stack (16KB). */

/* The Multiboot header. */
typedef struct multiboot_header
{
   unsigned long magic;
   unsigned long flags;
   unsigned long checksum;
   unsigned long header_addr;
   unsigned long load_addr;
   unsigned long load_end_addr;
   unsigned long bss_end_addr;
   unsigned long entry_addr;
} multiboot_header_t;

     /* The symbol table for a.out. */
     typedef struct aout_symbol_table
     {
       unsigned long tabsize;
       unsigned long strsize;
       unsigned long addr;
       unsigned long reserved;
     } aout_symbol_table_t;

     /* The section header table for ELF. */
     typedef struct elf_section_header_table
     {
       unsigned long num;
       unsigned long size;
       unsigned long addr;
       unsigned long shndx;
     } elf_section_header_table_t;

/* The Multiboot information. */
typedef struct multiboot_info
{
   unsigned long flags;
   unsigned long mem_lower;
   unsigned long mem_upper;
   unsigned long boot_device;
   char* cmdline;
   unsigned long mods_count;
   unsigned long mods_addr;
   union
   {
     aout_symbol_table_t aout;
     elf_section_header_table_t elf;
   } symbol;
   unsigned long mmap_length;
   unsigned long mmap_addr;
} multiboot_info_t;

     /* The module structure. */
     typedef struct module
     {
       unsigned long mod_start;
       unsigned long mod_end;
       unsigned long string;
       unsigned long reserved;
     } module_t;

     /* The memory map. Be careful that the offset 0 is base_addr_low
        but no size. */
     typedef struct memory_map
     {
       unsigned long size;
       unsigned long base_addr_low;
       unsigned long base_addr_high;
       unsigned long length_low;
       unsigned long length_high;
       unsigned long type;
     } memory_map_t;

#endif /* _MULTIBOOT__H */

