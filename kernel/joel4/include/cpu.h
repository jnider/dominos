#ifndef _CPU__H
#define _CPU__H

typedef enum cpu_family
{
   INTEL_PENTIUM,
   INTEL_PENTIUM_PRO,
   INTEL_PENTIUM_III,
   INTEL_PENTIUM_4,
   INTEL_PENTIUM_D,
   INTEL_CORE_DUO,
} cpu_family;

/* intel manual 2A: table 3-23 */
typedef struct cpu_info
{
   cpu_family family;
   char fpu;      ///< floating point unit on chip
   char vme;      ///< virtual 8086 extensions
   char de;       ///< debugging extensions
   char pse;      ///< page size extension
   char tsc;      ///< time stamp counter
   char msr;      ///< model-specific registers
   char pae;      ///< physical address extension
   char mce;      ///< machine check exception
   char cx8;      ///< CMPXCHG8B instruction
   char apic;
   char sep;      ///< SYSENTER and SYSEXIT support
   char mtrr;
   char pge;
   char mca;
   char cmov;
   char pat;
   char pse36;
   char psn;
   char clfsh;
   char ds;
   char acpi;
   char mmx;
   char fxsr;
   char sse;
   char ss;
   char htt;
   char tm;
   char pbe;
} cpu_info;

void k_identifyCPU(cpu_info* info);

int k_initSystemCalls(unsigned int cs, unsigned int sp, unsigned int handler);

static inline rdtsc(unsigned long long* tsc)
{
   asm volatile("rdtsc" : "=A"(*tsc));
}


#endif /* _CPU__H */

