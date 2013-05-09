#ifndef _CPU__H
#define _CPU__H

#include "intel.h"

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
   union
   {
      intel_cpu_info intel;
   };
} cpu_info;

void k_getCpuInfo(cpu_info* info);
void k_printCpuInfo(const cpu_info* info);

int k_initSystemCalls(unsigned int cs, unsigned int sp, unsigned int handler);


#endif /* _CPU__H */

