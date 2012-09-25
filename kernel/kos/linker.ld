/* define the entry point */
ENTRY (_loader)

/* declare the memory map so the linker can help find errors */
/*
MEMORY
{
   kernel :    ORIGIN = 0, len = 0x00200000
   libraries : org = 0x00200000, LENGTH = 14M
   user :      org = 0x01000000, len = 0xFE000000
}
*/

/* now the actual code/data */
SECTIONS
{
    . = 0x00100000;
    _startKernel = .;

   .loader :
   {
      kernel/loader.o (.text)
   }


   /* leave 4K for the kernel stack - counts down*/
   . = ALIGN(0x1000);
   _kernelStackStart = .;
   . += 0x1000;
   _kernelStack = .;

   /* leave 4K for the interrupt stack - counts down*/
   . += 0x1000;
   _intStack = .;

   /* leave 4K for page directory - counts up*/
   _kernelPageDirectory = .;
   . += 0x1000;

   _kernelCodeStart = .;
   .text :
   {
      *(EXCLUDE_FILE (kernel/kstub.o kernel/timer.o kernel/task.o kernel/rpc.o) .text)
   }

   .rodata ALIGN (0x1000) :
   {
      *(EXCLUDE_FILE (kernel/kstub.o kernel/timer.o kernel/task.o kernel/rpc.o) .rodata)
   }

   .data ALIGN (0x1000) :
   {
      *(EXCLUDE_FILE (kernel/kstub.o kernel/timer.o kernel/task.o kernel/rpc.o) .data)
   }

   .bss :
   {
      _sbss = .;
      *(COMMON)
      *(EXCLUDE_FILE (kernel/kstub.o kernel/timer.o kernel/task.o kernel/rpc.o) .bss)

      /* reserve 4K for the memory map */
      . = ALIGN(0x1000);
      _memoryMap = ABSOLUTE(.);
      . += 0x1000;

      _ebss = .;
   }

   /* kernel function stub code & data */
   .stub_text 0x00000000 : AT(LOADADDR(.bss) + SIZEOF(.bss))
   {
      kernel/kstub.o(.text) kernel/timer.o(.text) kernel/task.o(.text) kernel/rpc.o(.text)
      kernel/kstub.o(.rodata .bss .data) kernel/timer.o(.rodata .bss .data) kernel/task.o(.rodata .bss .data) kernel/rpc.o(.rodata .bss .data)
   }

   _stubCode = LOADADDR(.stub_text);

   . = LOADADDR(.stub_text) + SIZEOF(.stub_text);
   
   _endKernel = .;
}
