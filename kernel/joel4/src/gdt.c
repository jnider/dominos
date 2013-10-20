////////////////////////////////////////////////////////////////////////////////
/// @file   gdt.cpp
/// @short  Implementation of global descriptor table functions
/// @author J.Nider
/// @date   25/08/2008
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "gdt.h"

typedef struct __PACKED gdt_entry_t
{
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char base_middle;
    unsigned char seg_type;
    unsigned char limit_high;
    unsigned char base_high;
} gdt_entry_t;

typedef struct __PACKED gdt_ptr_t
{
    unsigned short limit;
    gdt_entry_t* pBase;
} gdt_ptr_t;

#define GDT_TYPE_SEG_TYPE     0x0F     ///< segment type
#define GDT_TYPE_S            0x10     ///< descriptor type
#define GDT_TYPE_DPL          0x60     ///< descriptor privilege level
#define GDT_TYPE_SEG_PRESENT  0x80     ///< segment present

#define GDT_LIMIT_HIGH        0x0F     ///< high nibble of limit
#define GDT_LIMIT_AVL         0x10     ///< available for system software
#define GDT_LIMIT_L           0x20     ///< 64-bit code segment
#define GDT_LIMIT_DB          0x40     ///< default operation size (0=16-bit segment, 1=32-bit segment)
#define GDT_LIMIT_G           0x80     ///< 0=byte granularity, 1=4KB granularity

static gdt_entry_t gdt[MAX_GDT_ENTRIES] __attribute__((aligned(8)));   ///< Global descriptor table - holds segment descriptors
static gdt_ptr_t gp;                   ///< pointer to the GDT to be loaded into the processor
static unsigned short numEntries = 0;  ///< the number of used entries in the GDT

// Set up a descriptor
void GDT_SetSegment(unsigned short index, unsigned long base, unsigned long limit, DescriptorType type, PrivilegeLevel dpl)
{
   // Set up the descriptor base address
   gdt[index].base_low = (base & 0xFFFF);
   gdt[index].base_middle = (base >> 16) & 0xFF;
   gdt[index].base_high = (base >> 24) & 0xFF;

   // Set up the descriptor limits
   gdt[index].limit_low = (limit & 0xFFFF);
   gdt[index].limit_high = ((limit >> 16) & GDT_LIMIT_HIGH) | GDT_LIMIT_DB | GDT_LIMIT_G;

   gdt[index].seg_type = GDT_TYPE_SEG_PRESENT | (dpl << 5) | type | GDT_TYPE_S;
   numEntries++;
}

unsigned short GDT_SetSystemGate(unsigned long base, unsigned long limit, SystemDescriptorType type, PrivilegeLevel dpl)
{
   // Set up the descriptor base address
   unsigned short index = numEntries;

   gdt[index].base_low = (base & 0xFFFF);
   gdt[index].base_middle = (base >> 16) & 0xFF;
   gdt[index].base_high = (base >> 24) & 0xFF;

   // Set up the descriptor limits
   gdt[index].limit_low = (limit & 0xFFFF);
   gdt[index].limit_high = ((limit >> 16) & GDT_LIMIT_HIGH) | GDT_LIMIT_DB | GDT_LIMIT_G;

   gdt[index].seg_type = GDT_TYPE_SEG_PRESENT | (dpl << 5) | type;
   numEntries++;
   return index;
}

unsigned short GDT_SetTSS(unsigned short index, const tss_t* pTSS, PrivilegeLevel dpl)
{
   if (index == 0)
      return (unsigned short)-1;

   // Set up the descriptor base address
   unsigned long base = (unsigned long)pTSS;
   gdt[index].base_low = (base & 0xFFFF);
   gdt[index].base_middle = (base >> 16) & 0xFF;
   gdt[index].base_high = (base >> 24) & 0xFF;

   // Set up the descriptor limits
   gdt[index].limit_low = sizeof(tss_t)-1;
   gdt[index].limit_high = GDT_LIMIT_DB | GDT_LIMIT_G;

   gdt[index].seg_type = GDT_TYPE_SEG_PRESENT | (dpl << 5) | DESCRIPTOR_SYSTEM_32_TSS;
   numEntries++;
   return index;
}

// Initializes the GDT
void GDT_Init(void)
{
   // initialize the whole table
   k_memset(gdt, 0, MAX_GDT_ENTRIES * sizeof(gdt_entry_t));
   numEntries++;
}

// load the GDT
void GDT_Load(unsigned short csIndex, unsigned short dsIndex)
{
   // set up the GDT pointer
   gp.limit = (sizeof(gdt_entry_t) * numEntries) - 1;
   gp.pBase = gdt;

   __ASM("lgdtl %0\n" :: "m"(gp));
   __ASM("mov %0, %%ax\n" :: "am"(dsIndex));
   __ASM("mov %ax, %ds\n");
   __ASM("mov %ax, %es\n");
   __ASM("mov %ax, %fs\n");
   __ASM("mov %ax, %gs\n");

   unsigned int task_sel[2];
   task_sel[0] = 0;
   task_sel[1] = csIndex;

   // jump to the new CS (EIP stays the same, we just get a new code segment)
   __ASM("mov $gdt_label, %%ebx\n   \
         mov %0, %%esi\n            \
         mov %%ebx, (%%esi)\n       \
         ljmp *(%0)\ngdt_label:\n" :: "am"(task_sel));

   return;
}
