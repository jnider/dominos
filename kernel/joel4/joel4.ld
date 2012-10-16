/* The GNU-style linker script for the JOEL4 kernel on x86 */
/* JNider: August 25, 2012 */


_boot_sys_start = 0x100000;

ENTRY(_entry_point)

PHDRS {
  btext PT_LOAD;
  bdata PT_LOAD;
}

SECTIONS {
   . = _boot_sys_start;

   .text_boot : ALIGN(0x1000)
   {
      objs/boot.o (.text)
   } : btext
  
   . = ALIGN(0x10);
   .bss_boot :
   {
      objs/boot.o (.bss)
      . = ALIGN(0x1000);
   } : bdata

   .text ALIGN (0x1000) :
   {
      *(EXCLUDE_FILE(objs/root_task.o) .text)
   }

   .rodata ALIGN (0x1000) :
   {
      *(EXCLUDE_FILE(objs/root_task.o) .rodata)
   }

   .data ALIGN (0x1000) :
   {
      *(EXCLUDE_FILE(objs/root_task.o) .data)
   }

   .bss :
   {
      *(COMMON)
      *(EXCLUDE_FILE(objs/root_task.o) .bss)
   }

   /* leave 4K for the kernel stack - counts down*/
   .stacks ALIGN(0x1000):
   {
   _kernelStackStart = .;
   . += 0x1000;
   _kernelStack = .;
   . += 0x1000;
   _interruptStack = .;
   }
   _endKernel = .;

   /* set the KERNEL_MEMORY_LIMIT */

   /* ROOT TASK */
   .root_task_code 0x800000: AT(LOADADDR(.stacks) + SIZEOF(.stacks))
   {
      _root_task_code_start = .;
      objs/root_task.o (.text)
      _root_task_code_end = .;
   }

   .root_task_data ALIGN(0x1000):
   {
      _root_task_data_start = .;
      objs/root_task.o (.rodata .data .bss)
      _root_task_data_end = .;
      . = ALIGN(0x1000);
      _root_task_stack_start = .;
      . += 0x1000;
      _root_task_stack_end = .;
   }

   _globalFreeSpace = .;
}

