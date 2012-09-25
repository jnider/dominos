////////////////////////////////////////////////////////////////////////////////
/// @file   kernel.cpp
/// @short  Kernel implementation file
/// @author J.Nider
/// @date   01-Jun-2007 12:31:31 PM
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "gdt.h"                       // for managing global descriptor table
#include "idt.h"                       // for managing interrupt descriptor table
#include "isr.h"                       // interrupt routines
//#include "kernel.h"                    // kernel object definitions
#include "keyboard.h"                  // keyboard routines
#include "kstdio.h"                    // kernel versions of standard functions
//#include "memory.h"                    // memory manager
#include "multiboot.h"                 // multiboot related functions
//#include "process.h"
//#include "ktimer.h"                    // k_setScheduler
//#include "scheduler.h"
#include "version.h"
#include "kvideo.h"
#include "ktask.h"
#include "layout.h"					// segment layout in the system
//#include "kstdio.h"


#define LOAD_TASK_REGISTER(_index)  __ASM("ltr %0\n" :: "am"(_index))
//#define TASK_SWITCH_TIME 100
//#define MAX_MODULES  5  // maximum number of loadable modules

unsigned char bootDevice;

unsigned short csos_segment = CSOS_SEGMENT;
unsigned short dsos_segment = DSOS_SEGMENT;
unsigned short dsapp_segment = DSAPP_SEGMENT;

static tss_t osTSS __attribute__((aligned(128)));
static tss_t userTSS __attribute__((aligned(128)));
task_t osTask;

//Kernel* g_pKernel = NULL;

//module_t module[MAX_MODULES];
unsigned char moduleCount;

extern unsigned int _endKernel;

//////////
///@author  J.Nider
///@date    11-Jun-2007 01:01:11 PM
///@brief   Entry point to the kernel
///
/// This function must have C linkage because it is called from an
/// assembly routine (boot loader)
//////////
void _main(unsigned long magic, multiboot_info_t *pInfo)
{
   k_cls();   // Clear the screen

   k_printf("KOS version 1.0.%i\n", _KOS_BUILD);

   // Am I booted by a Multiboot-compliant boot loader?
   if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
   {
      k_printf("Not a multiboot bootloader\n");
      HALT();
   }
   //print_multiboot(pInfo);
   //while(1);

   // save the boot device - we will need this later when loading the
   // rest of the files 
   bootDevice = (unsigned char)(pInfo->boot_device >> 24);

   // save the loaded module locations
   if (pInfo->flags & MULTIBOOT_MODULES)
   {
      module_t *mod;
      unsigned int i;

      moduleCount = pInfo->mods_count;
      //k_printf("found %d boot modules\n", moduleCount);
      for (i = 0, mod = (module_t *) pInfo->mods_addr; i < moduleCount; i++, mod++)
      {
         //k_printf("start = 0x%x, end = 0x%x, name = %s\n", mod->mod_start, mod->mod_end, mod->string);
         //k_memcpy(&module[i], mod, sizeof(module_t));
      }
   }

   keyboard_install();  // install keyboard handler

   //k_printf("GDT_Init\n");
   GDT_Init(); //initialize global descriptor table

   // here we set the DPL (descriptor privilege level) of each descriptor (vol3A 4.5)
   // the DPL specifies the minimum privilege level that can access the descriptor (data segments)
   // or the exact CPL for non-conforming code segments
   // or the maximum privilege level for conforming code segments
   GDT_SetSegment(CSINT_SEGMENT, CSOS_BASE_ADDRESS,  CSOS_LIMIT,  DESCRIPTOR_CODE_EO,  PRIVILEGE_LEVEL_KERNEL);
   GDT_SetSegment(DSINT_SEGMENT, DSOS_BASE_ADDRESS,  DSOS_LIMIT,  DESCRIPTOR_DATA_RW,  PRIVILEGE_LEVEL_KERNEL);
   GDT_SetSegment(CSOS_SEGMENT,  CSOS_BASE_ADDRESS,  CSOS_LIMIT,  DESCRIPTOR_CODE_EO_C, PRIVILEGE_LEVEL_KERNEL);  // kernel or user can access this conforming code segment
   GDT_SetSegment(DSOS_SEGMENT,  DSOS_BASE_ADDRESS,  DSOS_LIMIT,  DESCRIPTOR_DATA_RW,  PRIVILEGE_LEVEL_USER);     // kernel or user can access this data segment
   GDT_SetSegment(CSAPP_SEGMENT, CSAPP_BASE_ADDRESS, CSAPP_LIMIT, DESCRIPTOR_CODE_EO,  PRIVILEGE_LEVEL_USER);
   GDT_SetSegment(DSAPP_SEGMENT, DSAPP_BASE_ADDRESS, DSAPP_LIMIT, DESCRIPTOR_DATA_RW,  PRIVILEGE_LEVEL_USER);
   GDT_SetTSS(OSTSS_SEGMENT, &osTSS, PRIVILEGE_LEVEL_KERNEL); // set the task state segment for the os
   GDT_SetTSS(APPTSS_SEGMENT, &userTSS, PRIVILEGE_LEVEL_USER); // set the task state segment for user mode

   //k_printf("csos: 0x%x dsos: 0x%x csapp: 0x%x dsapp 0x%x\n", CSOS_SEGMENT, DSOS_SEGMENT, CSAPP_SEGMENT, DSAPP_SEGMENT);
   //k_printf("osTSS: 0x%x userTSS: 0x%x\n", &osTSS, &userTSS);

   //k_printf("InitTask\n");
   k_memset(&osTask, 0, sizeof(osTask));    // this must be before k_initMemory, because osTask.membreak is used
   k_initTask(SEGMENT_INDEX(CSAPP_SEGMENT, 0, PRIVILEGE_LEVEL_USER),
               SEGMENT_INDEX(DSAPP_SEGMENT, 0, PRIVILEGE_LEVEL_USER),
               SEGMENT_INDEX(DSINT_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL));

   //k_printf("k_initMemory\n");
   // if there are modules to load, set the starting address of the
   // OS task (logical address) to start after the list
   //if (moduleCount > 0)
   //{
   //   osTask.membreak = (module[moduleCount-1].mod_end + 0x1000) & ~(PAGE_SIZE-1);
   //}
   //else
   {
      osTask.membreak = (unsigned int)&_endKernel;
   }
   
   //k_printf("end of kernel memory: 0x%x\n", osTask.membreak);
   //k_initMemory((pInfo->mem_lower + pInfo->mem_upper) * 1024);

   // initialize interrupts - privilege level 0 means a stack switch will occur on interrupt
   // JKN - does that mean we can cause an interrupt without a stack switch??
   ISR_Init();
   IDT_Init(SEGMENT_INDEX(CSINT_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL));
   //k_printf("Interrupts loaded\n");

   GDT_Load(SEGMENT_INDEX(CSOS_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL),
            SEGMENT_INDEX(DSOS_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL));
   //k_printf("GDT loaded\n");

   // load the task segment for the first task
   unsigned int ostss_index = SEGMENT_INDEX(OSTSS_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL);
   LOAD_TASK_REGISTER(ostss_index);

   //k_printf("loading 0x%x to 0x%x\n", module[0].mod_start, module[0].mod_end);
   /*task_t* pTask1 = LoadApp((char*)module[0].string, module[0].mod_start, module[0].mod_end);
   if (!pTask1)
   {
      k_printf("task not created\n");
      return;
   }
   k_setTaskAsReady(pTask1);   // this should be done inside CreateTask automatically
   k_memcpy(&userTSS, &pTask1->task, sizeof(tss_t));

   k_printf("setting current task to 0x%x\n", pTask1->taskID);
   k_setCurrentTask(pTask1);
   
   // run the scheduler
   //k_printf("jumping to 0x%x:0x%x\n", CSAPP_BASE_ADDRESS, userTSS.eip);
*/
   unsigned int task_sel[2];
   task_sel[0] = 0;
   task_sel[1] = SEGMENT_INDEX(APPTSS_SEGMENT, 0, PRIVILEGE_LEVEL_USER);
   
   //k_initVGA();
   //k_getSVGAInfo();
   //while(1);
   __ASM("ljmp *(%0)\n" :: "am"(task_sel));

   // should never get here
   k_printf("ERROR!\n");
   while(1);
}

/*
void print_multiboot(const multiboot_info_t *pInfo)
{
   // How much memory is available?
   if (pInfo->flags & MULTIBOOT_MEMORY)
      k_printf ("mem_lower = %uKB, mem_upper = %uKB\n", pInfo->mem_lower, pInfo->mem_upper);

   // which device did we boot from?
   if (pInfo->flags & MULTIBOOT_BOOT_DEVICE)
   {
      unsigned long drive = pInfo->boot_device >> 24;
      k_printf ("boot_device = 0x%x\n", drive);
   }

   // Is the command line passed?
   if (pInfo->flags & MULTIBOOT_CMD_LINE)
     k_printf ("cmdline = %s\n", pInfo->cmdline);

   // Are mods_* valid?
   if (pInfo->flags & MULTIBOOT_MODULES)
   {
      module_t *mod;
      unsigned int i;

      k_printf ("Boot modules: count=0x%x, mods_addr=0x%x\n", pInfo->mods_count, (int) pInfo->mods_addr);
      for (i = 0, mod = (module_t *) pInfo->mods_addr; i < pInfo->mods_count; i++, mod++)
      {
         k_printf("mod_start = 0x%x, mod_end = 0x%x, string = %s\n", mod->mod_start, mod->mod_end, mod->string);
      }
   }

   // Bits 4 and 5 are mutually exclusive!
   if ((pInfo->flags & MULTIBOOT_SYMBOL_AOUT) && (pInfo->flags & MULTIBOOT_SYMBOL_ELF))
   {
      k_printf ("ERROR: I can't tell if this image is AOUT or ELF format!\n");
      return;
   }

   // print AOUT symbol table
   if (pInfo->flags & MULTIBOOT_SYMBOL_AOUT)
   {
      k_printf ("aout symbol table: tabsize = 0x%0x, "
              "strsize = 0x%x, addr = 0x%x\n",
               pInfo->symbol.aout.tabsize,
               pInfo->symbol.aout.strsize,
               pInfo->symbol.aout.addr);
   }

   // Is the section header table of ELF valid?
   if (pInfo->flags & MULTIBOOT_SYMBOL_ELF)
   {
       k_printf ("elf section: num = %u, size = 0x%x,"
               " addr = 0x%x, shndx = 0x%x\n",
               pInfo->symbol.elf.num, pInfo->symbol.elf.size,
               pInfo->symbol.elf.addr, pInfo->symbol.elf.shndx);
   }

   // Are mmap_* valid?
   if (pInfo->flags & MULTIBOOT_MEMORY_MAP)
   {
      memory_map_t *mmap;

      k_printf ("mmap_addr = 0x%x, mmap_length = 0x%x\n",
              (unsigned) pInfo->mmap_addr, (unsigned) pInfo->mmap_length);
       for (mmap = (memory_map_t *) pInfo->mmap_addr;
            (unsigned long) mmap < pInfo->mmap_addr + pInfo->mmap_length;
            mmap = (memory_map_t *) ((unsigned long) mmap
                                     + mmap->size + sizeof (mmap->size)))
         k_printf (" size = 0x%x, base_addr = 0x%x%x,"
                 " length = 0x%x%x, type = 0x%x\n",
                 mmap->size,
                 mmap->base_addr_high,
                 mmap->base_addr_low,
                 mmap->length_high,
                 mmap->length_low,
                 mmap->type);
   }

}
*/

