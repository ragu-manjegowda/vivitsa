#ifndef INCLUDE_IDT_H
#define INCLUDE_IDT_H

#pragma once
#include <types.h>

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

/* Common Definitions for PIC */
#define PIC1 0x20 /* IO base address for master PIC */
#define PIC2 0xA0 /* IO base address for slave PIC */
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

/* reinitialize the PIC controllers, giving them specified vector offsets
 * rather than 8h and 70h, as configured by default
 */
#define INITIALISE_COMMAND 0x11
#define PIC1_VECTOR_OFFSET 0x20 /* re-map to ISR 32 */
#define PIC2_VECTOR_OFFSET 0x28 /* re-map to ISR 40 */
#define PIC2_PORT_IN_PIC1 0x04  /* PIC1's port address for PIC2 - 00000100b */
#define PIC2_CASCADED_IDEN 0x02 /* PIC2's identity is 2 for Master (PIC1)*/
#define ICW4_8086 0x01          /* 8086/88 (MCS-80/85) mode */

/* To start using hardware interrupts Programmable Interrupt Controller (PIC)
 * needs to be configured. The PIC makes it possible to map signals from the
 * hardware to interrupts. The reasons for configuring the PIC are: Remap the
 * interrupts. The PIC uses interrupts 0 - 15 for hardware interrupts by
 * default, which conflicts with the CPU interrupts. Therefore the PIC
 * interrupts must be remapped to another interval. Select which interrupts to
 * receive. Set up the correct mode for the PIC.
 *
 * The hardware interrupts are shown in the table below:
 *
 * PIC1:
 *      0 - Timer
 *      1 - Keyboard
 *      2 - PIC 2
 *      3 - COM 2
 *      4 - COM 1
 *      5 - LPT 2
 *      6 - Floppy disk
 *      7 - LPT 1
 *
 * PIC2:
 *      8 - Real Time Clock
 *      9 - General I/O
 *     10 - General I/O
 *     11 - General I/O
 *     12 - General I/O
 *     13 - Coprocessor
 *     14 - IDE Bus
 *     15 - IDE Bus
 *
 * Following are the function call to above 16 interrupt requests.
 * Extern allows to access ASM from this C code.
 */
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

/* This structure contains the value of one IDT entry.
 * We use the attribute 'packed' to tell GCC not to change
 * any of the alignment in the structure.
 */
struct idt_entry_struct {
  uint16_t offset_low;       /* The lowest 16 bits of the 32 bit ISR address. */
  uint16_t segment_selector; /* Segment Selector. */
  uint8_t alwaysZero;        /* This 8 bits are always 0. */
  uint8_t access_gran;  /* Access flags, granularity and few reserved bits. */
  uint16_t offset_high; /* The highest 16 bits of the 32 bit ISR address. */
} __attribute__((packed));

typedef struct idt_entry_struct idt_entry_t;

/* This struct describes a IDT pointer. It points to the start of
 * our array of IDT entries, and is in the format required by the
 * lidt instruction.
 */
struct idt_ptr_struct {
  uint16_t limit; /* The upper 16 bits of the table with entries. */
  uint32_t base;  /* The address of the first idt_entry_t struct. */
} __attribute__((packed));

typedef struct idt_ptr_struct idt_ptr_t;

/* Function to initialize IDT */
void init_idt();

#endif /* INCLUDE_IDT_H */
