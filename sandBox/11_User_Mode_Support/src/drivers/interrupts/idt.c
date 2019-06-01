#include "idt.h"
#include <io.h>

/* Function call to load IDT implemented in assembly.
 * Extern allows to access ASM from this C code.
 */
extern void idt_flush(uint32_t);

/* 256 interrupts!!! */
idt_entry_t idt_entries[256];

/* When processor enters protected mode the first command you will need to give
 * the two PICs is the initialise command (code 0x11). This command makes the
 * PIC wait for 3 extra "initialisation words" on the data port.
 */
void init_pic() {
  /* Start the initialization sequence */
  outb(PIC1, INITIALISE_COMMAND);
  outb(PIC2, INITIALISE_COMMAND);

  /* Set new interrupt vector offset instead of default oofset */
  outb(PIC1_DATA, PIC1_VECTOR_OFFSET);
  outb(PIC2_DATA, PIC2_VECTOR_OFFSET);

  /* Tell PIC1 there is Slave at PIN 2 (0000 0100) */
  outb(PIC1_DATA, PIC2_PORT_IN_PIC1);

  /* Tell Slave PIC its cascade identity (0000 0010) */
  outb(PIC2_DATA, PIC2_CASCADED_IDEN);

  /* Set PICs to operate in 8086/88 (MCS-80/85) mode */
  outb(PIC1_DATA, ICW4_8086);
  outb(PIC2_DATA, ICW4_8086);

  /* Null out the data registers */
  outb(PIC1_DATA, 0x0);
  outb(PIC2_DATA, 0x0);
}

/* Set the value of IDT entry. */
static void idt_set_gate(uint8_t num, uint32_t addressISR, uint16_t segmentSelector,
                         uint8_t accessGran) {
  idt_entries[num].offset_low = (addressISR & 0xFFFF);
  idt_entries[num].segment_selector = segmentSelector;
  idt_entries[num].alwaysZero = 0;

  idt_entries[num].access_gran = accessGran;

  idt_entries[num].offset_high = addressISR >> 16;
}

void init_idt() {
  idt_ptr_t idt_ptr;
  idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
  idt_ptr.base = (uint32_t)&idt_entries;

  /* Initialise PIC */
  init_pic();

  /* Setting everything to 0 as handler for interrupts above 32 are not set */
  // memset(&idt_entries, 0, sizeof(idt_entry_t) * 256);

  /* Define handlers for first 32 which are required by processor */
  idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);   // ISR 0
  idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);   // ISR 1
  idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);   // ISR 2
  idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);   // ISR 3
  idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E);   // ISR 4
  idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E);   // ISR 5
  idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E);   // ISR 6
  idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8E);   // ISR 7
  idt_set_gate(8, (uint32_t)isr8, 0x08, 0x8E);   // ISR 8
  idt_set_gate(9, (uint32_t)isr9, 0x08, 0x8E);   // ISR 9
  idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E); // ISR 10
  idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E); // ISR 11
  idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E); // ISR 12
  idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E); // ISR 13
  idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E); // ISR 14
  idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E); // ISR 15
  idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E); // ISR 16
  idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E); // ISR 17
  idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E); // ISR 18
  idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E); // ISR 19
  idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E); // ISR 20
  idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E); // ISR 21
  idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E); // ISR 22
  idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E); // ISR 23
  idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E); // ISR 24
  idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E); // ISR 25
  idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E); // ISR 26
  idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E); // ISR 27
  idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E); // ISR 28
  idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E); // ISR 29
  idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E); // ISR 30
  idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E); // ISR 31

  /* Define handlers for 16 interrupt requests by pic */
  idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);  // IRQ 0
  idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);  // IRQ 1
  idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);  // IRQ 2
  idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);  // IRQ 3
  idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);  // IRQ 4
  idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);  // IRQ 5
  idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);  // IRQ 6
  idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);  // IRQ 7
  idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);  // IRQ 8
  idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);  // IRQ 9
  idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E); // IRQ 10
  idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E); // IRQ 11
  idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E); // IRQ 12
  idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E); // IRQ 13
  idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E); // IRQ 14
  idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E); // IRQ 15

  idt_flush((uint32_t)&idt_ptr);
}
