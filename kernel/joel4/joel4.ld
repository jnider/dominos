/* The GNU-style linker script for the JOEL4 kernel on x86 */
/* JNider: August 25, 2012 */


_boot_sys_start = 0x200000;

ENTRY(_entry_point)

PHDRS {
  btext PT_LOAD;
  bdata PT_LOAD;
}

SECTIONS {
   . = _boot_sys_start;
   .text_boot : ALIGN(0x1000)
   {
      objs/boot.o (.text .text.* .gnu.linkonce.t.*)
   } : btext
  
   . = ALIGN(0x10);
   .bss_boot :
   {
      objs/boot.o (.bss)
      . = ALIGN(0x1000);
      PROVIDE(_boot_sys_end = .);
   } : bdata

   /* leave 4K for the kernel stack - counts down*/
   . = ALIGN(0x1000);
   _kernelStackStart = .;
   . += 0x1000;
   _kernelStack = .;
}

