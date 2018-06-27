#include "gdt.h"

/* Access ASM function from C code. */
extern void gdt_flush(u32int, u32int);

/* Define number of segments, including null segment */
#define NUM_OF_SEGMENTS 3

/* Define number of kernel segments */
#define NUM_OF_KERNEL_SEGMENTS 2

/* Define 3 entries each for null, kernel code and kernel data */
gdt_entry_t gdt_entries[NUM_OF_SEGMENTS];

/* Set the value of GDT entry. */
static void gdt_set_gate(s32int num, u32int base, u32int limit, u8int access,
                         u8int gran) {
  gdt_entries[num].base_low = (base & 0xFFFF);
  gdt_entries[num].base_middle = (base >> 16) & 0xFF;
  gdt_entries[num].base_high = (base >> 24) & 0xFF;

  gdt_entries[num].limit_low = (limit & 0xFFFF);

  /*
   * name | value | size | desc
   * ---------------------------
   * P    |     1 |    1 | segment present in memory
   * DPL  |    pl |    2 | privilege level
   * S    |     1 |    1 | descriptor type, 0 = system, 1 = code or data
   * Type |  type |    4 | segment type, how the segment can be accessed
   * Access = 0x9A privilege 0 code segment, Access = 0x92 for privilege 0 data
   * segment
   */
  gdt_entries[num].access = access;

  /*
   * name | value | size | desc
   * ---------------------------
   * G    |     1 |    1 | granularity, size of segment unit, 1 = 4kB
   * D/B  |     1 |    1 | size of operation size, 0 = 16 bits, 1 = 32 bits
   * L    |     0 |    1 | 1 = 64 bit code
   * AVL  |     0 |    1 | "available for use by system software"
   * LIM  |   0xF |    4 | the four highest bits of segment limit
   * Granularity = 0xCF as far as highest bits of segment limit is 0xFFFFFFFF
   */
  gdt_entries[num].granularity = gran;
}

void init_gdt() {
  gdt_ptr_t gdt_ptr;

  /* Define array to hold kernel segment offset address, excluding null entry */
  u32int gdt_kernel_segments_offset[NUM_OF_KERNEL_SEGMENTS];

  gdt_ptr.limit = (sizeof(gdt_entry_t) * NUM_OF_SEGMENTS) - 1;
  gdt_ptr.base = (u32int)&gdt_entries;

  gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
  gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Kernel Code segment
  gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Kernel Data segment

  for (u32int i = 0, j = 8; i < NUM_OF_KERNEL_SEGMENTS; i++) {
    gdt_kernel_segments_offset[i] = j;
    j += 8;
  }
  gdt_flush((u32int)&gdt_ptr, (u32int)&gdt_kernel_segments_offset);
}
