#include "cpu.h"
#include "eflags.h"


/* Machine-specific Registers (MSR) - intel vol 2B table 4-7 */
#define IA32_SYSENTER_CS   0x174
#define IA32_SYSENTER_ESP  0x175
#define IA32_SYSENTER_EIP  0x176

/* Read Machine-Specific Register */
static inline rdmsr(unsigned int address, unsigned int* low, unsigned int* high)
{
   asm volatile("rdmsr" : "=a"(*low), "=d"(*high) : "c"(address));
}

/* Write Machine-Specific Register */
static inline wrmsr(unsigned int address, unsigned int low, unsigned int high)
{
   asm volatile("wrmsr" :: "a"(low), "d"(high), "c"(address));
}

void k_getCpuInfo(cpu_info* info)
{
   unsigned int maxInputValue=0;
   unsigned int features=0;
   unsigned int featuresExt=0;
   unsigned int version=0;
   unsigned int vendor[3];
   unsigned int processor=0;

   k_memset(info, 0, sizeof(cpu_info));

   // We don't care about any processors that don't have the CPUID instruction
   //asm("pushfd; pop ecx; mov ecx, eax; xor eax 0x200000; push eax; popfd; pushfd; pop eax; xor ecx, eax; "); 

   // When EAX=0, the processor returns the highest value (maxInputValue) recognized for processor information
   asm("cpuid" : "=a"(maxInputValue), "=b"(vendor[0]), "=c"(vendor[2]), "=d"(vendor[1]) : "a" (0) :);
      
   // make sure this is an Intel processor, because that's all we can handle at the moment
   info->intel.vendorStr[0] = vendor[0] & 0xFF;
   info->intel.vendorStr[1] = (vendor[0] >> 8) & 0xFF;
   info->intel.vendorStr[2] = (vendor[0] >> 16) & 0xFF;
   info->intel.vendorStr[3] = (vendor[0] >> 24) & 0xFF;
   info->intel.vendorStr[4] = vendor[1] & 0xFF;
   info->intel.vendorStr[5] = (vendor[1] >> 8) & 0xFF;
   info->intel.vendorStr[6] = (vendor[1] >> 16) & 0xFF;
   info->intel.vendorStr[7] = (vendor[1] >> 24) & 0xFF;
   info->intel.vendorStr[8] = vendor[2] & 0xFF;
   info->intel.vendorStr[9] = (vendor[2] >> 8) & 0xFF;
   info->intel.vendorStr[10] = (vendor[2] >> 16) & 0xFF;
   info->intel.vendorStr[11] = (vendor[2] >> 24) & 0xFF;
   info->intel.vendorStr[12] = 0;
   
   info->intel.maxInputValue = maxInputValue;

   if (maxInputValue >= 1)
   {
      // get model, family, stepping info
      asm("cpuid" : "=a"(version), "=b"(processor), "=c" (featuresExt), "=d" (features) : "a" (1) :);

      // now we can get the model, stepping, brand info, and features
      info->intel.version = version;
      info->intel.features = features;
      info->intel.featuresExt = featuresExt;
      info->intel.brand = processor & 0xFF;
      info->intel.cacheLine = ((processor >> 8) & 0xFF) << 3; // x8 to get value in bytes
      info->intel.cores = (processor >> 16) & 0xFF;
      info->intel.apicID = (processor >> 24) & 0xFF;
   }
}

void k_printCpuInfo(const cpu_info* info)
{
   k_printf("Vendor: %s\n", info->intel.vendorStr);
   
   if (info->intel.maxInputValue >= 1)
   {
      k_printf("Features:");
      if (info->intel.features & INTEL_FEATURES_FPU) k_printf("fpu ");
      if (info->intel.features & INTEL_FEATURES_VME) k_printf("vme ");
      if (info->intel.features & INTEL_FEATURES_DE) k_printf("de "); 
      if (info->intel.features & INTEL_FEATURES_PSE) k_printf("pse ");
      if (info->intel.features & INTEL_FEATURES_TSC) k_printf("tsc ");
      if (info->intel.features & INTEL_FEATURES_MSR) k_printf("msr ");
      if (info->intel.features & INTEL_FEATURES_PAE) k_printf("pae ");
      if (info->intel.features & INTEL_FEATURES_MCE) k_printf("mce ");
      if (info->intel.features & INTEL_FEATURES_CX8) k_printf("cx8 ");
      if (info->intel.features & INTEL_FEATURES_APIC) k_printf("apic ");
      if (info->intel.features & INTEL_FEATURES_SEP) k_printf("sep ");
      if (info->intel.features & INTEL_FEATURES_MTRR) k_printf("mtrr ");
      if (info->intel.features & INTEL_FEATURES_PGE) k_printf("pge ");
      if (info->intel.features & INTEL_FEATURES_MCA) k_printf("mca ");
      if (info->intel.features & INTEL_FEATURES_CMOV) k_printf("cmov ");
      if (info->intel.features & INTEL_FEATURES_PAT) k_printf("pat ");
      if (info->intel.features & INTEL_FEATURES_PSE36) k_printf("pse36 ");
      if (info->intel.features & INTEL_FEATURES_PSN) k_printf("psn ");
      if (info->intel.features & INTEL_FEATURES_CLFSH) k_printf("clfsh ");
      if (info->intel.features & INTEL_FEATURES_DS) k_printf("ds ");
      if (info->intel.features & INTEL_FEATURES_ACPI) k_printf("acpi ");
      if (info->intel.features & INTEL_FEATURES_MMX) k_printf("mmx ");
      if (info->intel.features & INTEL_FEATURES_FXSR) k_printf("fxsr ");
      if (info->intel.features & INTEL_FEATURES_SSE) k_printf("sse ");
      if (info->intel.features & INTEL_FEATURES_SSE2) k_printf("sse2 ");
      if (info->intel.features & INTEL_FEATURES_SS) k_printf("ss ");
      if (info->intel.features & INTEL_FEATURES_HTT) k_printf("htt ");
      if (info->intel.features & INTEL_FEATURES_TM) k_printf("tm ");
      if (info->intel.features & INTEL_FEATURES_PBE) k_printf("pbe ");
      k_printf("\n");
      k_printf("Ext Features:");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_SSE3) k_printf("sse3 ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_PCLMULQDQ) k_printf("pclmulqdq ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_DTES64) k_printf("dtes64 ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_MONITOR) k_printf("monitor ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_DS_CPL) k_printf("ds_cpl ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_VMX) k_printf("vmx ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_SMX) k_printf("smx ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_EIST) k_printf("eist ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_TM2) k_printf("tm2 ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_SSSE3) k_printf("ssse3 ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_CNXT_ID) k_printf("cnxt_id ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_FMA) k_printf("fma ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_CMPXCHG16B) k_printf("cmpxchg16b ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_xTPR) k_printf("xtpr ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_PDCM) k_printf("pdcm ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_PCID) k_printf("pcid ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_DCA) k_printf("dca ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_SSE4_1) k_printf("sse4.1 ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_SSE4_2) k_printf("sse4.2 ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_x2APIC) k_printf("x2apic ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_MOVBE) k_printf("movbe ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_POPCNT) k_printf("popcnt ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_TSCD) k_printf("tscd ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_AESNI) k_printf("aesni ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_XSAVE) k_printf("xsave ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_OSXSAVE) k_printf("osxsave ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_AVX) k_printf("avx ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_F16C) k_printf("f16c ");
      if (info->intel.featuresExt & INTEL_FEATURES_EXT_RDRAND) k_printf("rdrand ");
      k_printf("\n");
      k_printf("brand: %i\n", info->intel.brand);
      k_printf("cores: %i\n", info->intel.cores);
      k_printf("Default APIC: %i\n", info->intel.apicID);
   }
}

int k_initSystemCalls(unsigned int cs, unsigned int sp, unsigned int handler)
{
   k_printf("Setting SYSCALL CS=0x%x SP=0x%x IP=0x%x\n", cs, sp, handler);
   wrmsr(IA32_SYSENTER_CS, cs, 0);
   wrmsr(IA32_SYSENTER_ESP, sp, 0);
   wrmsr(IA32_SYSENTER_EIP, handler, 0);
   return 1;

   //return 0;
}

