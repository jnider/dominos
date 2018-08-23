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
  
   . = ALIGN(0x1000);
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
.gnu.hash : { *(.gnu.hash) }: main_data

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
   _endKernel = .;
}

