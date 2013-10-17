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
#include "root_task.h"

#define LOAD_TASK_REGISTER(_index)  __ASM("ltr %0\n" :: "am"(_index))

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

//static unsigned int kernelPageDir[PAGE_SIZE/sizeof(unsigned int)] __attribute__((aligned(4096)));
static tss_t osTSS __attribute__((aligned(128)));
static tss_t badtssTSS __attribute__((aligned(128)));
static tss_t userTSS __attribute__((aligned(128)));
static cpu_info cpuInfo;
int kernel_data_segment = KERNEL_DATA_SEGMENT;
extern int _endKernel; /* provided by the linker script to mark the end of the loadable portions of the kernel */
extern unsigned int _kernelStack;

extern void root_task_main(void);
extern void syscall(void);
extern int _root_task_code_start;
extern int _root_task_code_size;
extern int _root_task_data_start;
extern int _root_task_data_size;
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
   int freeHeap;
   void* freePages;
   task_t* rootTask;

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
   print_multiboot(pInfo);

   /* find out who we are dealing with */
   k_getCpuInfo(&cpuInfo);
   k_printCpuInfo(&cpuInfo);
   
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
   //k_printf("Loading GDT\n");
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
   unsigned int ostss_index = SEGMENT_INDEX(KERNEL_TSS_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL);
   LOAD_TASK_REGISTER(ostss_index);

   /* calculate free space, and set up heap */
   freePages = (void*)KERNEL_MEMORY_LIMIT - FREE_PAGE_COUNT * PAGE_SIZE;
   freeHeap = freePages - (void*)&_endKernel;
   k_initHeap((int)&_endKernel, freeHeap);
   k_printf("Freemem: %i\n", k_freeMem());

   /* initialize memory, and enable paging */
   k_printf("Initializing memory\n");
   
   // check for page size extensions (4MB pages)
   if (cpuInfo.intel.features & INTEL_FEATURES_PSE)
   {
      k_printf("4MB memory pages supported\n");
      k_initMemory(freePages);
   }
   else
   {
      k_printf("PSE not detected - see code WARNING_KERNEL_NO_PSE on the wiki\n");
      HALT();
   }

   if ((cpuInfo.intel.features & INTEL_FEATURES_MSR) && 
      (cpuInfo.intel.features & INTEL_FEATURES_SEP))
   {
      k_initSystemCalls(SEGMENT_INDEX(KERNEL_CODE_SEGMENT, 0, PRIVILEGE_LEVEL_KERNEL), (unsigned int)&_kernelStack, (unsigned int)syscall);
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

   /* create root task - takes over responsibility from the kernel for all resources
      until a driver wants some of those responsibilities */
   k_printf("Creating root task\n");
   rootTask = k_createTask();
	if (!rootTask)
	{
		k_printf("Failed\n");
		HALT();
	}

   k_createThread(rootTask, &_root_task_code_start,
                  (unsigned int)&_root_task_code_size,
                  (void*)&_root_task_data_start,
                  (unsigned int)&_root_task_data_size,
                  (unsigned int)root_task_main);

	// turn off interrupts for root task only
	rootTask->segment.eflags &= ~EFLAGS_IF;

   // copy the init module into the root task
   if ((pInfo->flags & MULTIBOOT_MODULES) && (pInfo->mods_count > 0))
   {
		uint32 i;
      module_t *mod = (module_t*)pInfo->mods_addr;

		// determine how many more pages need to be mapped
		uint32 freeSpace = (unsigned int)&_root_task_data_size % PAGE_SIZE;
		uint32 modSize = mod->mod_end - mod->mod_start;
   	k_printf("free space: %i needed space %i\n", freeSpace, modSize);

		// map the needed pages
		uint32 numMappedPages = (uint32)&_root_task_data_size / PAGE_SIZE;
		for (i=0; i < modSize; i+= PAGE_SIZE)
		{
			void* newpage = k_allocKernelPage();
      	k_map4KPage((unsigned int*)rootTask->segment.pdbr, (unsigned int)newpage, (unsigned int)APP_DATA + (numMappedPages * PAGE_SIZE) + i,
         	MEMORY_PAGE_WRITE | MEMORY_PAGE_USER_MODE);
			
			// now copy the data to the user-accessible space
			k_printf("copying from: 0x%x to 0x%x size: %i\n", ((void*)mod->mod_start) + i, newpage, PAGE_SIZE);
			k_memcpy(newpage, ((void*)mod->mod_start) + i, modSize);
		}

   	/* save some information for the boot task on it's stack (as a parameter) */
		rootTask->segment.esp -= sizeof(BootInfo) + 4; //adjust the stack so as to leave room for the parameter
   	BootInfo* pBootInfo = (BootInfo*)(0x706000 - sizeof(BootInfo)); //0x705ff0
		k_printf("boot info @ 0x%x\n", pBootInfo);
   	pBootInfo->initData = APP_DATA + (unsigned int)&_root_task_data_size; // set the pointer to the virtual address for the data part + the offset
		pBootInfo->initDataSize = modSize;
		pBootInfo->freeMem = pInfo->mem_upper;
   }

   // now start user space, effectively running the first task (root task) 
	// interrupts are enabled inside user-space tasks automatically (tss)
   k_printf("switching to root task\n");
   k_memcpy(&userTSS, &rootTask->segment, sizeof(tss_t));
   unsigned int task_sel[2];
   task_sel[0] = 0;
   task_sel[1] = SEGMENT_INDEX(USER_TSS_SEGMENT, 0, PRIVILEGE_LEVEL_USER);

   __ASM("ljmp *(%0)\n" :: "am"(task_sel));

   // should never get here
   k_printf("ERROR!\n");
   while(1);
}

