
.global _entry_point           # making entry point visible to linker
.extern _userStack

_entry_point:  #lea _userStack, %esp    # set the stack
					# eventually add argc + argv here

               call  main              # call userspace app main function
               hlt                     # should instruct the OS to kill this task
