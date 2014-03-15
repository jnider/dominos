/* The GNU-style linker script for DominOS user-space apps  on x86 */
/* JNider: May 17, 2013 */

ENTRY(_entry_point)

PHDRS {
   user_code PT_LOAD;
   user_data PT_LOAD;
}

SECTIONS {
   /* USER PROGRAM */
   .user_code 0x1000000:
   {
      ../objs/crt0.o (.text)
      *.text
      . = ALIGN(0x1000);
   } : user_code

	/* stack is 0x1100000 */

	/* data is 0x1200000 */
   .user_data 0x1200000 :
   {
      _userStack = .;
      *.o (*COMMON)
      . = ALIGN(0x1000);
      ../objs/crt0.o (.data .bss .rodata)
   } : user_data
}

