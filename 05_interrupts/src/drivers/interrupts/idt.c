#include "idt.h"
#include <types.h>

/* Function call to load IDT implemented in assembly.
 * Extern allows to access ASM from this C code.
 */
extern void idt_flush(u32int);

/* This structure contains the value of one IDT entry.
 * We use the attribute 'packed' to tell GCC not to change
 * any of the alignment in the structure.
 */
struct idt_entry_struct {
  u16int offset_low;       // The lowest 16 bits of the 32 bit ISR address.
  u16int segment_selector; // Segment Selector.
  u8int alwaysZero;        // This 8 bits are always 0.
  u8int access_gran;       // Access flags, granularity and few reserved bits.
  u16int offset_high;      // The highest 16 bits of the 32 bit ISR address.
} __attribute__((packed));

typedef struct idt_entry_struct idt_entry_t;

/* This struct describes a IDT pointer. It points to the start of
 * our array of IDT entries, and is in the format required by the
 * lidt instruction.
 */
struct idt_ptr_struct {
  u16int limit; // The upper 16 bits of the table with entries.
  u32int base;  // The address of the first idt_entry_t struct.
} __attribute__((packed));

typedef struct idt_ptr_struct idt_ptr_t;

/* 256 interrupts!!! */
idt_entry_t idt_entries[1];

/* Set the value of IDT entry. */
static void idt_set_gate(u8int num, u32int addressISR, u16int segmentSelector,
                         u8int accessGran) {
  idt_entries[num].offset_low = (addressISR & 0xFFFF);
  idt_entries[num].segment_selector = segmentSelector;
  idt_entries[num].alwaysZero = 0;

  idt_entries[num].access_gran = accessGran;

  idt_entries[num].offset_high = (addressISR >> 16);
}

/* The processor will sometimes need to signal your kernel. Something major may
 * have happened, such as a divide-by-zero, or a page fault. To do this, it uses
 * the first 32 interrupts. It is therefore doubly important that all of these
 * are mapped and non-NULL - else the CPU will triple-fault and reset.
 *
 *
 * The special, CPU-dedicated interrupts are shown below.
 *
 *     0 - Division by zero exception
 *     1 - Debug exception
 *     2 - Non maskable interrupt
 *     3 - Breakpoint exception
 *     4 - 'Into detected overflow'
 *     5 - Out of bounds exception
 *     6 - Invalid opcode exception
 *     7 - No coprocessor exception
 *     8 - Double fault (pushes an error code)
 *     9 - Coprocessor segment overrun
 *     10 - Bad TSS (pushes an error code)
 *     11 - Segment not present (pushes an error code)
 *     12 - Stack fault (pushes an error code)
 *     13 - General protection fault (pushes an error code)
 *     14 - Page fault (pushes an error code)
 *     15 - Unknown interrupt exception
 *     16 - Coprocessor fault
 *     17 - Alignment check exception
 *     18 - Machine check exception
 *     19-31 - Reserved
 *
 *
 * Following are the function call to above 32 interrupt handlers.
 * Extern allows to access ASM from this C code.
 */
extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

void init_idt() {
  idt_ptr_t idt_ptr;
  idt_ptr.limit = sizeof(idt_entry_t) * 1 - 1;
  idt_ptr.base = (u32int)&idt_entries;

  /* Setting everything to 0 as handler for interrupts above 32 are not set */
  // memset(&idt_entries, 0, sizeof(idt_entry_t) * 256);

  /* Define handlers for first 32 which are required by processor */
  idt_set_gate(0, (u32int)isr0, 0x08, 0x8E);   // ISR 0
  idt_set_gate(1, (u32int)isr1, 0x08, 0x8E);   // ISR 1
  idt_set_gate(2, (u32int)isr2, 0x08, 0x8E);   // ISR 2
  idt_set_gate(3, (u32int)isr3, 0x08, 0x8E);   // ISR 3
  idt_set_gate(4, (u32int)isr4, 0x08, 0x8E);   // ISR 4
  idt_set_gate(5, (u32int)isr5, 0x08, 0x8E);   // ISR 5
  idt_set_gate(6, (u32int)isr6, 0x08, 0x8E);   // ISR 6
  idt_set_gate(7, (u32int)isr7, 0x08, 0x8E);   // ISR 7
  idt_set_gate(8, (u32int)isr8, 0x08, 0x8E);   // ISR 8
  idt_set_gate(9, (u32int)isr9, 0x08, 0x8E);   // ISR 9
  idt_set_gate(10, (u32int)isr10, 0x08, 0x8E); // ISR 10
  idt_set_gate(11, (u32int)isr11, 0x08, 0x8E); // ISR 11
  idt_set_gate(12, (u32int)isr12, 0x08, 0x8E); // ISR 12
  idt_set_gate(13, (u32int)isr13, 0x08, 0x8E); // ISR 13
  idt_set_gate(14, (u32int)isr14, 0x08, 0x8E); // ISR 14
  idt_set_gate(15, (u32int)isr15, 0x08, 0x8E); // ISR 15
  idt_set_gate(16, (u32int)isr16, 0x08, 0x8E); // ISR 16
  idt_set_gate(17, (u32int)isr17, 0x08, 0x8E); // ISR 17
  idt_set_gate(18, (u32int)isr18, 0x08, 0x8E); // ISR 18
  idt_set_gate(19, (u32int)isr19, 0x08, 0x8E); // ISR 19
  idt_set_gate(20, (u32int)isr20, 0x08, 0x8E); // ISR 20
  idt_set_gate(21, (u32int)isr21, 0x08, 0x8E); // ISR 21
  idt_set_gate(22, (u32int)isr22, 0x08, 0x8E); // ISR 22
  idt_set_gate(23, (u32int)isr23, 0x08, 0x8E); // ISR 23
  idt_set_gate(24, (u32int)isr24, 0x08, 0x8E); // ISR 24
  idt_set_gate(25, (u32int)isr25, 0x08, 0x8E); // ISR 25
  idt_set_gate(26, (u32int)isr26, 0x08, 0x8E); // ISR 26
  idt_set_gate(27, (u32int)isr27, 0x08, 0x8E); // ISR 27
  idt_set_gate(28, (u32int)isr28, 0x08, 0x8E); // ISR 28
  idt_set_gate(29, (u32int)isr29, 0x08, 0x8E); // ISR 29
  idt_set_gate(30, (u32int)isr30, 0x08, 0x8E); // ISR 30
  idt_set_gate(31, (u32int)isr31, 0x08, 0x8E); // ISR 31

  idt_flush((u32int)&idt_ptr);
}
