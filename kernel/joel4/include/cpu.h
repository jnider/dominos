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

typedef struct cpu_info
{
   cpu_family family;
   char fpu;
   char vme;
   char de;
   char pse;
   char tsc;
   char msr;
   char pae;
   char mce;
   char cx8;
   char apic;
   char sep;
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

#endif /* _CPU__H */

