/* The GNU-style linker script for DominOS user-space apps  on x86 */
/* JNider: May 17, 2013 */

ENTRY(_entry_point)

PHDRS {
   crt_code PT_LOAD;
   crt_data PT_LOAD;
   user_code PT_LOAD;
   user_data PT_LOAD;
}

SECTIONS {
   .text_crt : ALIGN(0x1000)
   {
      objs/crt0.o (.text)
   } : crt_code
  
   . = ALIGN(0x10);
   .data_crt :
   {
      objs/crt0.o (.data .bss .rodata)
      . = ALIGN(0x1000);
   } : crt_data

   /* USER PROGRAM */
   .user_code 0x1000000:
   {
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
   } : user_data
}

