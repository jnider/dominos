#include "l4.h"
#include "multiboot.h"
#include "kstdio.h"
#include "serial.h"
#include "gdt.h"                       // for managing global descriptor table
#include "idt.h"                       // for managing interrupt descriptor table
#include "isr.h"                       // interrupt routines
#include "task.h"
#include "memory.h"
#include "cpu.h"
#include "version.h"
#include "eflags.h"	// for turning off interrupts in root task - should be cpu specific
#include "cpio.h"

#define LOAD_TASK_REGISTER(_index)  __ASM("ltr %0\n" :: "am"(_index))
#define ALIGN(_addr, _align) (((Word)_addr + _align - 1) - (((Word)_addr + _align - 1) % _align))

/* defines for global descriptor table (GDT) entries */
#define NULL_SEGMENT        0             /* not actually used, but must be 0 */
#define KERNEL_CODE_SEGMENT 1             /* kernel code segment index */
#define KERNEL_DATA_SEGMENT 2             /* kernel data segment index */
#define USER_CODE_SEGMENT   3             /* user code segment index */
#define USER_DATA_SEGMENT   4             /* user data segment index */
#define KERNEL_TSS_SEGMENT  5             /* kernel task state segment index */
#define BAD_TSS_TSS_SEGMENT 6             /* bad tss task state segment index */
#define USER_TSS_SEGMENT    7             /* user task state segment index */

#define KERNEL_CODE_BASE    0             /* kernel code segment base address */
#define KERNEL_CODE_LIMIT   0xFFFFFFFF    /* kernel code segment limit */
#define KERNEL_DATA_BASE    0             /* kernel data segment base address */
#define KERNEL_DATA_LIMIT   0xFFFFFFFF    /* kernel data segment limit */
#define USER_CODE_BASE      0             /* user code segment base address */
#define USER_CODE_LIMIT     0xFFFFFFFF    /* user code segment limit */
#define USER_DATA_BASE      0             /* user data segment base address */
#define USER_DATA_LIMIT     0xFFFFFFFF    /* user data segment limit */

L4_KIP* k_KIP; // address of the kernel interface page

//static unsigned int kernelPageDir[PAGE_SIZE/sizeof(unsigned int)] __attribute__((aligned(4096)));
static tss_t osTSS __attribute__((aligned(128)));
static tss_t badtssTSS __attribute__((aligned(128)));
static tss_t userTSS __attribute__((aligned(128)));
static cpu_info cpuInfo;
int kernel_data_segment = KERNEL_DATA_SEGMENT;
extern int _endKernel; /* provided by the linker script to mark the end of the loadable portions of the kernel */
extern unsigned int _kernelStack;

extern void syscall(void);
extern int _interruptStack;

static void print_multiboot(const multiboot_info_t *pInfo)
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

void _main(unsigned long magic, multiboot_info_t *pInfo)
{
   int freeHeapSize; /* length of free kernel heap, in bytes */

	// interrupts should be off by default, but make sure anyway
   _DISABLE_INTERRUPTS();

   k_cls();   // Clear the screen

   if (serial_init() != 0)
      k_printf("Serial failed\n");

   k_printf("JOEL4 version 1.0.%i\n", _JOEL4_BUILD);

   // Am I booted by a Multiboot-compliant boot loader?
   if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
   {
      k_printf("Not a multiboot bootloader\n");
      while(1);
   }
   //print_multiboot(pInfo);
   

   // calculate the end of memory used by the modules
   if (pInfo->flags & MULTIBOOT_MODULES)
   {
      module_t *mod;
      unsigned int i;

      for (i = 0, mod = (module_t *) pInfo->mods_addr; i < pInfo->mods_count; i++, mod++)
      {
         _endKernel = (mod->mod_end + PAGE_SIZE) & ~(PAGE_SIZE_MASK-1);
         k_printf("_endKernel = 0x%x\n", _endKernel);
      }
   }
   k_printf("end of memory: 0x%x\n", pInfo->mem_upper * 1024);

   /* find out who we are dealing with */
   k_getCpuInfo(&cpuInfo);
   //k_printCpuInfo(&cpuInfo);
   
   if (k_strcmp(cpuInfo.intel.vendorStr, "GenuineIntel"))
   {
      k_printf("We don't support vendors other than Intel at this time\n");
      return;
   }

   /* set up the global descriptor table */
   //k_printf("Init GDT\n");
   GDT_Init();
   /* first the memory protection segments */
   GDT_SetSegment(KERNEL_CODE_SEGMENT,
                  KERNEL_CODE_BASE,
                  KERNEL_CODE_LIMIT,
                  DESCRIPTOR_CODE_EO,
                  PRIVILEGE_LEVEL_KERNEL);
   GDT_SetSegment(KERNEL_DATA_SEGMENT,
                  KERNEL_DATA_BASE,
                  KERNEL_DATA_LIMIT,
                  DESCRIPTOR_DATA_RW,
                  PRIVILEGE_LEVEL_KERNEL);
   GDT_SetSegment(USER_CODE_SEGMENT,
                  USER_CODE_BASE,
                  USER_CODE_LIMIT,
                  DESCRIPTOR_CODE_EO,
                  PRIVILEGE_LEVEL_USER);
   GDT_SetSegment(USER_DATA_SEGMENT,
                  USER_DATA_BASE,
                  USER_DATA_LIMIT,
                  DESCRIPTOR_DATA_RW,
                  PRIVILEGE_LEVEL_USER);
   
   /* and now the task state segments */
   //k_printf("Setting TSS\n");
   GDT_SetTSS(    KERNEL_TSS_SEGMENT,
                  &osTSS,
                  PRIVILEGE_LEVEL_KERNEL);
   GDT_SetTSS(    BAD_TSS_TSS_SEGMENT,
                  &badtssTSS,
                  PRIVILEGE_LEVEL_KERNEL);
   GDT_SetTSS(    USER_TSS_SEGMENT,
                  &userTSS,
                  PRIVILEGE_LEVEL_USER);

   ISR_Init();
   //k_printf("Loading IDT\n");
   IDT_Init(SEGMENT_INDEX(KERNEL_CODE_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL));

   k_printf("Entering protected mode\n");
   GDT_Load(SEGMENT_INDEX(KERNEL_CODE_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL),
            SEGMENT_INDEX(KERNEL_DATA_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL));

   /* set up the bad TSS task in case something goes wrong during a task switch */
   badtssTSS.cs = SEGMENT_INDEX(KERNEL_CODE_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL);
   badtssTSS.ds = SEGMENT_INDEX(KERNEL_DATA_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL);
   badtssTSS.ss = SEGMENT_INDEX(KERNEL_DATA_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL);
   badtssTSS.eip = (unsigned int)&_interruptStack;
   idt_set_gate(10, 0, &badtssTSS, PRIVILEGE_LEVEL_KERNEL);

   // load the task segment for the kernel task
   k_printf("Loading TSS\n");
   unsigned short ostss_index = SEGMENT_INDEX(KERNEL_TSS_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL);
   LOAD_TASK_REGISTER(ostss_index);

   /* initialize memory, and enable paging */
   k_printf("Initializing memory\n");
   freeHeapSize = KERNEL_MEMORY_LIMIT - _endKernel - RESERVED_PAGE_COUNT * PAGE_SIZE;
   
   // check for page size extensions (4MB pages)
   if (cpuInfo.intel.features & INTEL_FEATURES_PSE)
   {
      k_printf("4MB memory pages supported\n");
      k_initMemory((void*)(_endKernel + freeHeapSize), KERNEL_MEMORY_LIMIT, USER_MEMORY_START, pInfo->mem_upper * 1024);
   }
   else
   {
      k_printf("PSE not detected - see code WARNING_KERNEL_NO_PSE on the wiki\n");
      HALT();
   }

   /* set up heap */
   k_initHeap(_endKernel, freeHeapSize);
   k_printf("Freemem: %i\n", k_freeMem());
   k_printf("Reserved pages: %i\n", RESERVED_PAGE_COUNT);


   if ((cpuInfo.intel.features & INTEL_FEATURES_MSR) && 
      (cpuInfo.intel.features & INTEL_FEATURES_SEP))
   {
      k_initSystemCalls(SEGMENT_INDEX(KERNEL_CODE_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL),
         (unsigned int)&_kernelStack,
         (unsigned int)syscall);
   }
   else
   {
      k_printf("Can't initialize SYSENTER/SYSEXIT\n");
      HALT();
   }

   /* initialize task management */
   k_initTask(SEGMENT_INDEX(USER_CODE_SEGMENT, 0, PRIVILEGE_LEVEL_USER),
      SEGMENT_INDEX(USER_DATA_SEGMENT, 0, PRIVILEGE_LEVEL_USER),
      SEGMENT_INDEX(KERNEL_DATA_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL),
      (unsigned int)&_interruptStack);

   k_printf("kernel memory limit: 0x%x\n", KERNEL_MEMORY_LIMIT);    /* how much memory is associated with the kernel */
   k_printf("user memory start: 0x%x\n", USER_MEMORY_START);

   /* Create kernel information page, boot info struct, and boot modules */
   k_KIP = (L4_KIP*)k_allocKernelPage(); /* if this causes problems, use k_allocKernel4KPage() instead */
   k_printf("Creating KIP @ 0x%x\n", k_KIP);
   BootInfo* pBootInfo = (BootInfo*)ALIGN(((char*)k_KIP + sizeof(L4_KIP)), 4);

   k_KIP->magic[0] = KIP_MAGIC_0;
   k_KIP->magic[1] = KIP_MAGIC_1;
   k_KIP->magic[2] = KIP_MAGIC_2;
   k_KIP->magic[3] = KIP_MAGIC_3;
   k_KIP->bootInfo = (Word)((char*)pBootInfo - (char*)k_KIP);

   /* Fill in the boot info struct */
   pBootInfo->magic = BOOT_INFO_MAGIC;
   pBootInfo->version = 1;
   pBootInfo->first = (Word)((char*)pBootInfo - (char*)k_KIP + sizeof(BootInfo)); // relative to the start of the BootInfo struct
   pBootInfo->count = 0;
   pBootInfo->size = sizeof(BootInfo);

   /* calculate how much free space remains in the KIP */
   Word kipFreeSpace = PAGE_SIZE - ((Word)pBootInfo - (Word)k_KIP) + sizeof(BootInfo);
   k_printf("%i bytes remaining in KIP\n", kipFreeSpace);

   char* archBuffer;
   if (!(pInfo->flags & MULTIBOOT_MODULES) || (pInfo->mods_count == 0))
   {
      k_printf("No boot loader modules found - can't find initrd\n");
   }

   Word archFilesize; // size of the archive file
   module_t *mod = (module_t*)pInfo->mods_addr;

   // make sure the module will fit in the remaining space
   if (sizeof(SimpleExecutable) > kipFreeSpace)
   {
      k_printf("Not enough room left in KIP to describe boot module\n");
      while(1);
   }

   /* initrd module */
   SimpleModule* pMod = (SimpleModule*)(pBootInfo->first + (char*)k_KIP);
   pMod->header.type = BOOT_RECORD_TYPE_SIMPLE_MODULE;
   pMod->header.version = 1;
   pMod->header.next = 0;
   pMod->start = mod->mod_start;
   pMod->size = mod->mod_end - mod->mod_start;
   pMod->cmdlineOffset = 0;
   pBootInfo->count++;
   pBootInfo->size += sizeof(SimpleModule);

   k_printf("Looking for root task\n");
   if (cpio_open_archive((void*)pMod->start) == -1)
   {
      k_printf("Init module is in an unknown file format - expected CPIO newc format\n");
      while(1);
   }
      
   // find the file containing the list of modules
   archBuffer = (char*)cpio_open_file(pMod->start, "user/services/root.service", &archFilesize);
   if (!archBuffer)
   {
      k_printf("Cannot find 'root.service' in the init module - don't know how to boot!\n");
      while(1);
   }
   k_printf("archFilesize: %i bytes\n", archFilesize);

   /* create root task - takes over responsibility from the kernel for all resources
      until a driver wants some of those responsibilities */
   k_printf("Creating root task\n");

   // create a unique address space for this new task
   unsigned int rootTaskPD = k_createMemorySpace();
   k_printf("Root task's page directory is at address 0x%x\n", rootTaskPD);

   k_setPageDirectory(rootTaskPD);

   int entry;
   k_loadELF(rootTaskPD, archBuffer, &entry);

   task_t* pRootTask = k_createThread(rootTaskPD,
                  entry,
                  APP_DATA,
                  APP_STACK_SIZE);

   //k_setPageDirectory(k_getKernelPageDirectory());

	// turn off interrupts for root task only
	pRootTask->segment.eflags &= ~EFLAGS_IF;

   // now start user space, effectively running the first task (root task) 
	// interrupts are enabled inside user-space tasks automatically (tss)
   k_memcpy(&userTSS, &pRootTask->segment, sizeof(tss_t));
   k_printf("Switching to root task (PDBR=0x%x)\n", userTSS.pdbr);
   k_setPageDirectory(userTSS.pdbr);
   //k_dumpPageDirectory((unsigned int*)userTSS.pdbr);
   unsigned int task_sel[2];
   task_sel[0] = 0;
   task_sel[1] = SEGMENT_INDEX(USER_TSS_SEGMENT, 0, PRIVILEGE_LEVEL_USER);

   __ASM("ljmp *(%0)\n" :: "am"(task_sel));

   // should never get here
   k_printf("ERROR!\n");
   while(1);
}

