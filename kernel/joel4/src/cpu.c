#include "cpu.h"
#include "eflags.h"

#define INTEL_PROC_FAMILY_SHIFT 8
#define INTEL_PROC_FAMILY_MASK (0xF<<INTEL_PROC_FAMILY_SHIFT)
#define INTEL_PROC_FAMILY_PENTIUM_4 0xF

#define INTEL_FEATURES_FPU    (1<<0)
#define INTEL_FEATURES_VME    (1<<1)
#define INTEL_FEATURES_DE     (1<<2)
#define INTEL_FEATURES_PSE    (1<<3)
#define INTEL_FEATURES_TSC    (1<<4)
#define INTEL_FEATURES_MSR    (1<<5)
#define INTEL_FEATURES_PAE    (1<<6)
#define INTEL_FEATURES_MCE    (1<<7)
#define INTEL_FEATURES_CX8    (1<<8)
#define INTEL_FEATURES_APIC   (1<<9)
#define INTEL_FEATURES_SEP    (1<<11)
#define INTEL_FEATURES_MTRR   (1<<12)
#define INTEL_FEATURES_PGE    (1<<13)
#define INTEL_FEATURES_MCA    (1<<14)
#define INTEL_FEATURES_CMOV   (1<<15)
#define INTEL_FEATURES_PAT    (1<<16)
#define INTEL_FEATURES_PSE36  (1<<17)
#define INTEL_FEATURES_PSN    (1<<18)
#define INTEL_FEATURES_CLFSH  (1<<19)
#define INTEL_FEATURES_DS     (1<<21)
#define INTEL_FEATURES_ACPI   (1<<22)
#define INTEL_FEATURES_MMX    (1<<23)
#define INTEL_FEATURES_FXSR   (1<<24)
#define INTEL_FEATURES_SSE    (1<<25)
#define INTEL_FEATURES_SSE2   (1<<26)
#define INTEL_FEATURES_SS     (1<<27)
#define INTEL_FEATURES_HTT    (1<<28)
#define INTEL_FEATURES_TM     (1<<29)
#define INTEL_FEATURES_PBE    (1<<31)

/* Machine-specific Registers (MSR) - intel vol 2B table 4-7 */
#define IA32_SYSENTER_CS   0x174
#define IA32_SYSENTER_ESP  0x175
#define IA32_SYSENTER_EIP  0x176

static inline rdmsr(unsigned int address, unsigned int* low, unsigned int* high)
{
   asm volatile("rdmsr" : "=a"(*low), "=d"(*high) : "c"(address));
}

static inline wrmsr(unsigned int address, unsigned int low, unsigned int high)
{
   asm volatile("wrmsr" :: "a"(low), "d"(high), "c"(address));
}

void k_identifyCPU(cpu_info* info)
{
   unsigned int ext=0;
   unsigned int features=0;
   unsigned int processor=0;

   /* are there any 486's still alive?? */

   /* When EAX=0, the processor returns the highest value recognized for
   extended processor information. */
   asm("cpuid" : "=a"(ext) : "a" (0) : "ebx", "ecx", "edx");
   
   switch(ext)
   {
   case 1:
      info->family = INTEL_PENTIUM;
      break;
   case 2:
      info->family = INTEL_PENTIUM_PRO; /* or Pentium II or celeron */
      break;
   case 3:
      info->family = INTEL_PENTIUM_III;
      break;
   case 5:
      info->family = INTEL_PENTIUM_4;  /* or Pentium D 8xx */
      //info->ht = 1; /* w/ hyperthreading */
      break;
   case 6:
      info->family = INTEL_PENTIUM_D;  /* */
      break;
   case 0xA:
      info->family = INTEL_CORE_DUO; /* or Duo 2 or Xeon 3000/5100/5300 */
      break;
   }

   if (info->family >= INTEL_PENTIUM_4)
   {
      asm("cpuid" : "=a"(processor), "=d" (features) : "a" (1) : "ebx", "ecx");
      // k_printf("processor: 0x%x\n", processor);
      if (((processor & INTEL_PROC_FAMILY_MASK) >> INTEL_PROC_FAMILY_SHIFT) ==
         INTEL_PROC_FAMILY_PENTIUM_4)
      {
         /* we're good - it really is a Pentium 4 (or better) */
      }
      /* now we can get the model, stepping, brand info, cflush, local APIC ID */
      /* and a whole bunch of stuff about virtual machines, SS3, etc */
      /* come back and look at this again later */
      /* its in Intel manual 2A, under CPUID */
      info->fpu = features & INTEL_FEATURES_FPU;
      info->vme = features & INTEL_FEATURES_VME;
      info->de = features & INTEL_FEATURES_DE;
      info->pse = features & INTEL_FEATURES_PSE;
      info->tsc = features & INTEL_FEATURES_TSC;
      info->msr = features & INTEL_FEATURES_MSR;
      info->pae = features & INTEL_FEATURES_PAE;
      info->mce = features & INTEL_FEATURES_MCE;
      info->cx8 = features & INTEL_FEATURES_CX8;
      info->apic = features & INTEL_FEATURES_APIC;
      info->sep = features & INTEL_FEATURES_SEP;
      info->mtrr = features & INTEL_FEATURES_MTRR;
      info->pge = features & INTEL_FEATURES_PGE;
      info->mca = features & INTEL_FEATURES_MCA;
      info->cmov = features & INTEL_FEATURES_CMOV;
      info->pat = features & INTEL_FEATURES_PAT;
      info->pse36 = features & INTEL_FEATURES_PSE36;
      info->psn = features & INTEL_FEATURES_PSN;
      info->clfsh = features & INTEL_FEATURES_CLFSH;
      info->ds = features & INTEL_FEATURES_DS;
      info->acpi = features & INTEL_FEATURES_ACPI;
      info->mmx = features & INTEL_FEATURES_MMX;
      info->fxsr = features & INTEL_FEATURES_FXSR;
      info->sse = features & INTEL_FEATURES_SSE;
      if (features & INTEL_FEATURES_SSE2)
         info->sse = 2;
      info->ss = features & INTEL_FEATURES_SS;
      info->htt = features & INTEL_FEATURES_HTT;
      info->tm = features & INTEL_FEATURES_TM;
      info->pbe = features & INTEL_FEATURES_PBE;
   }
}

int k_initSystemCalls(unsigned int cs, unsigned int sp, unsigned int handler)
{
   //if (info->msr && info->sep)
   {
      wrmsr(cs, 0, IA32_SYSENTER_CS);
      wrmsr(sp, 0, IA32_SYSENTER_ESP);
      wrmsr(handler, 0, IA32_SYSENTER_EIP);
      return 1;
   }

   return 0;
}

