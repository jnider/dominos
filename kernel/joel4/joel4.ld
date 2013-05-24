/* The GNU-style linker script for the JOEL4 kernel on x86 */
/* JNider: August 25, 2012 */


_boot_sys_start = 0x100000;

ENTRY(_entry_point)

PHDRS {
   boot_code PT_LOAD;
   boot_data PT_LOAD;
   main_code PT_LOAD;
   main_data PT_LOAD;
   k_stacks PT_LOAD;
   root_code PT_LOAD;
   root_data PT_LOAD;
}

SECTIONS {
   . = _boot_sys_start;

   .text_boot : ALIGN(0x1000)
   {
      objs/boot.o (.text)
   } : boot_code
  
   . = ALIGN(0x10);
   .data_boot :
   {
      objs/boot.o (.data .bss .rodata)
      . = ALIGN(0x1000);
   } : boot_data

   .text ALIGN (0x1000) :
   {
      *(EXCLUDE_FILE(objs/root_task.o) .text)
   } : main_code

   .rodata ALIGN (0x1000) :
   {
      *(EXCLUDE_FILE(objs/root_task.o) *)
   } : main_data

   /* leave 4K for the kernel stack - counts down*/
   .stacks ALIGN(0x1000):
   {
   _kernelStackStart = .;
   . += 0x1000;
   _kernelStack = .;
   . += 0x1000;
   _interruptStack = .;
   } : k_stacks

   /* set the KERNEL_MEMORY_LIMIT */

   /* ROOT TASK */
   .root_task_code 0x1000000: AT(ADDR(.stacks) + SIZEOF(.stacks))
   {
      objs/root_task.o (.text)
      . = ALIGN(0x1000);
   } : root_code

	/* stack is 0x1100000 */

	/* data is 0x1200000 */
   .root_task_data 0x1200000 : AT(LOADADDR(.root_task_code) + SIZEOF(.root_task_code))
   {
      objs/root_task.o (*)
      . += 0x1000;
      . = ALIGN(0x1000);
   } : root_data

   . = LOADADDR(.root_task_data) + SIZEOF(.root_task_data);
   _root_task_code_start = LOADADDR(.root_task_code);
   _root_task_code_size = SIZEOF(.root_task_code);
   _root_task_data_start = LOADADDR(.root_task_data);
   _root_task_data_size = SIZEOF(.root_task_data);
   _endKernel = .;
}

