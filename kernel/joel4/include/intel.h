#ifndef _INTEL__H
#define _INTEL__H

/* Vol 1 chapter 14.1: Processor Identification and Feature Determination */
/* Vol 2A chapter 3: */
#define INTEL_PROC_FAMILY_SHIFT 8
#define INTEL_PROC_FAMILY_MASK (0xF<<INTEL_PROC_FAMILY_SHIFT)
#define INTEL_PROC_FAMILY_PENTIUM_4 0xF

#define INTEL_FEATURES_FPU        (1<<0)        // floating point unit
#define INTEL_FEATURES_VME        (1<<1)        // Virtual 8086 Mode Enhancements.
#define INTEL_FEATURES_DE         (1<<2)        // Debugging Extensions.
#define INTEL_FEATURES_PSE        (1<<3)        // Page Size Extension.
#define INTEL_FEATURES_TSC        (1<<4)        // Time Stamp Counter.
#define INTEL_FEATURES_MSR        (1<<5)        // Model Specific Registers RDMSR and WRMSR Instructions.
#define INTEL_FEATURES_PAE        (1<<6)        // Physical Address Extension.
#define INTEL_FEATURES_MCE        (1<<7)        // Machine Check Exception.
#define INTEL_FEATURES_CX8        (1<<8)        // CMPXCHG8B Instruction
#define INTEL_FEATURES_APIC       (1<<9)        // APIC On-Chip
#define INTEL_FEATURES_SEP        (1<<11)       // SYSENTER and SYSEXIT Instructions
#define INTEL_FEATURES_MTRR       (1<<12)       // Memory Type Range Registers
#define INTEL_FEATURES_PGE        (1<<13)       // Page Global Bit
#define INTEL_FEATURES_MCA        (1<<14)       // Machine Check Architecture
#define INTEL_FEATURES_CMOV       (1<<15)       // Conditional Move Instructions
#define INTEL_FEATURES_PAT        (1<<16)       // Page Attribute Table
#define INTEL_FEATURES_PSE36      (1<<17)       // 36-Bit Page Size Extension
#define INTEL_FEATURES_PSN        (1<<18)       // Processor Serial Number
#define INTEL_FEATURES_CLFSH      (1<<19)       // CLFLUSH Instruction
#define INTEL_FEATURES_DS         (1<<21)       // Debug Store
#define INTEL_FEATURES_ACPI       (1<<22)       // Thermal Monitor and Software Controlled Clock Facilities
#define INTEL_FEATURES_MMX        (1<<23)       // Intel MMX Technology
#define INTEL_FEATURES_FXSR       (1<<24)       // FXSAVE and FXRSTOR Instructions
#define INTEL_FEATURES_SSE        (1<<25)       // SSE
#define INTEL_FEATURES_SSE2       (1<<26)       // SSE2
#define INTEL_FEATURES_SS         (1<<27)       // Self Snoop
#define INTEL_FEATURES_HTT        (1<<28)       // Max APIC IDs reserved field is Valid
#define INTEL_FEATURES_TM         (1<<29)       // Thermal Monitor
#define INTEL_FEATURES_PBE        (1<<31)       // Pending Break Enable

#define INTEL_FEATURES_EXT_SSE3     (1<<0)      // Streaming SIMD Extensions 3 (SSE3)
#define INTEL_FEATURES_EXT_PCLMULQDQ (1<<1)     // PCLMULQDQ
#define INTEL_FEATURES_EXT_DTES64   (1<<2)      // 64-bit DS Area
#define INTEL_FEATURES_EXT_MONITOR  (1<<3)      // MONITOR/MWAIT
#define INTEL_FEATURES_EXT_DS_CPL   (1<<4)      // CPL Qualified Debug Store
#define INTEL_FEATURES_EXT_VMX      (1<<5)      // Virtual Machine Extensions
#define INTEL_FEATURES_EXT_SMX      (1<<6)      // Safer Mode Extensions
#define INTEL_FEATURES_EXT_EIST     (1<<7)      // Enhanced Intel SpeedStep® technology
#define INTEL_FEATURES_EXT_TM2      (1<<8)      // Thermal Monitor 2
#define INTEL_FEATURES_EXT_SSSE3    (1<<9)      // Supplemental Streaming SIMD Extensions 3 (SSSE3)
#define INTEL_FEATURES_EXT_CNXT_ID  (1<<10)     // L1 Context ID
#define INTEL_FEATURES_EXT_FMA      (1<<12)     // supports FMA extensions using YMM state
#define INTEL_FEATURES_EXT_CMPXCHG16B (1<<13)   // CMPXCHG16B Available
#define INTEL_FEATURES_EXT_xTPR     (1<<14)     // xTPR Update Control
#define INTEL_FEATURES_EXT_PDCM     (1<<15)     // Perfmon and Debug Capability
#define INTEL_FEATURES_EXT_PCID     (1<<17)     // Process-context identifiers
#define INTEL_FEATURES_EXT_DCA      (1<<18)     // prefetch data from a memory mapped device
#define INTEL_FEATURES_EXT_SSE4_1   (1<<19)     // 
#define INTEL_FEATURES_EXT_SSE4_2   (1<<20)
#define INTEL_FEATURES_EXT_x2APIC   (1<<21)
#define INTEL_FEATURES_EXT_MOVBE    (1<<22)
#define INTEL_FEATURES_EXT_POPCNT   (1<<23)
#define INTEL_FEATURES_EXT_TSCD     (1<<24)     // local APIC timer supports one-shot operation using a TSC deadline value
#define INTEL_FEATURES_EXT_AESNI    (1<<25)
#define INTEL_FEATURES_EXT_XSAVE    (1<<26)     // supports the XSAVE/XRSTOR processor extended states
#define INTEL_FEATURES_EXT_OSXSAVE  (1<<27)     // the OS has enabled XSETBV/XGETBV instructions
#define INTEL_FEATURES_EXT_AVX      (1<<28)     // 
#define INTEL_FEATURES_EXT_F16C     (1<<29)     // 16-bit floating-point conversion instructions
#define INTEL_FEATURES_EXT_RDRAND   (1<<30)

#define GET_STEPPING(_a)   (_a & 0xF)
#define GET_MODEL(_a)      ((_a >> 4) & 0xF)
#define GET_FAMILY(_a)     ((_a >> 8) & 0xF)

// brand ID
typedef enum intel_brand
{
   BRAND_CELERON,
   BRAND_PENTIUM_III,
   BRAND_PENTIUM_III_XEON,
   BRAND_PENTIUM_III_2,
   BRAND_PENTIUM_III_M,
   BRAND_CELERON_M,
   BRAND_PENTIUM_4,
   BRAND_PENTIUM_4_2,
   BRAND_CELERON_2,
   BRAND_XEON,
   BRAND_XEON_MP,
   BRAND_PENTIUM_4M,
   BRAND_CELERON_M_2,
   BRAND_MOBILE,
   BRAND_CELERON_M_3,
   BRAND_CELERON_M_4,
   BRAND_CELERON_3,
   BRAND_MOBILE_2
} intel_brand;

typedef struct intel_cpu_info
{
   char vendorStr[13];
   unsigned char brand;
   unsigned char cores;    // how many logical cores in this die/package
   unsigned char apicID;   // default APIC id
   unsigned int version;   // intel: family, model, stepping
   unsigned int features;  // intel: supported features
   unsigned int featuresExt;  // intel: supported extended features
   unsigned int cacheLine;   // cache line flush size in bytes
   unsigned int maxInputValue;

} intel_cpu_info;

static inline rdtsc(unsigned long long* tsc)
{
   asm volatile("rdtsc" : "=A"(*tsc));
}

#endif // _INTEL__H
