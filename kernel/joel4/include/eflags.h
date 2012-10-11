#ifndef _EFLAGS__H
#define _EFLAGS__H

// these are for the EFLAGS register 3.4.3 Volume 1
#define EFLAGS_CF       (1 << 0)       // Carry Flag
#define EFLAGS_PF       (1 << 2)       // Parity Flag
#define EFLAGS_AF       (1 << 4)       // Auxiliary Carry Flag
#define EFLAGS_ZF       (1 << 6)       // Zero Flag
#define EFLAGS_SF       (1 << 7)       // Sign Flag
#define EFLAGS_TF       (1 << 8)       // Trap Flag (set for single-step debugging)
#define EFLAGS_IF       (1 << 9)       // Interrupt Enable Flag
#define EFLAGS_DF       (1 << 10)      // Direction Flag 0: forward, 1: reverse
#define EFLAGS_OF       (1 << 11)      // Overflow Flag
#define EFLAGS_IOPL(x)  (x << 12)      // IO Privilege Level - see PrivilegeLevel above
#define EFLAGS_NT       (1 << 14)      // Nested Task
#define EFLAGS_RF       (1 << 16)      // Resume Flag
#define EFLAGS_VM       (1 << 17)      // Virtual 8086 Mode
#define EFLAGS_AC       (1 << 18)      // Alignment Check
#define EFLAGS_VIF      (1 << 19)      // Virtual Interrupt Flag
#define EFLAGS_VIP      (1 << 20)      // Virtual Interrupt Pending
#define EFLAGS_ID       (1 << 21)      // ID Flag

#endif /* _EFLAGS__H */

