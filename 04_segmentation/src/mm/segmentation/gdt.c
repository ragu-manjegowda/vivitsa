#include "gdt.h"
#include <types.h>

/* Access ASM function from C code. */
extern void gdt_flush(u32int);

/* This structure contains the value of one GDT entry.
 * We use the attribute 'packed' to tell GCC not to change
 * any of the alignment in the structure.
 */
struct gdt_entry_struct {
  u16int limit_low;  // The lower 16 bits of the limit.
  u16int base_low;   // The lower 16 bits of the base.
  u8int base_middle; // The next 8 bits of the base.
  u8int
      access; // Access flags, determine what ring this segment can be used in.
  u8int granularity;
  u8int base_high; // The last 8 bits of the base.
} __attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t;

/* This struct describes a GDT pointer. It points to the start of
 * our array of GDT entries, and is in the format required by the
 * lgdt instruction.
 */
struct gdt_ptr_struct {
  u16int limit; // The upper 16 bits of all selector limits.
  u32int base;  // The address of the first gdt_entry_t struct.
} __attribute__((packed));

typedef struct gdt_ptr_struct gdt_ptr_t;

/* Define 3 entries each for null, kernel code and kernel code */
gdt_entry_t gdt_entries[3];

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
  gdt_ptr.limit = (sizeof(gdt_entry_t) * 3) - 1;
  gdt_ptr.base = (u32int)&gdt_entries;

  gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
  gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Kernel Code segment
  gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Kernel Data segment

  gdt_flush((u32int)&gdt_ptr);
}
