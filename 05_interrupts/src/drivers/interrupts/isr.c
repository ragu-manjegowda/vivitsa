#include <io.h>
#include <logger.h>
#include <types.h>

typedef struct cpu_state {
  u32int edi;
  u32int esi;
  u32int ebp;
  u32int esp;
  u32int ebx;
  u32int edx;
  u32int ecx;
  u32int eax;
} cpu_state_t;

typedef struct stack_state {
  u32int int_no;
  u32int err_code;
  u32int eip;
  u32int cs;
  u32int eflags;
  u32int useresp;
  u32int ss;
} stack_state_t;

typedef struct registers {
  /* Data segment selector */
  u32int ds;
  /* Pushed by pusha */
  cpu_state_t cpu_registers;
  /* Interrupt number and error code (if applicable). Pushed by the processor
   * automatically.
   */
  stack_state_t stack_contents;
} registers_t;

/* This gets called from our ASM interrupt handler stub. */
void interrupt_handler(registers_t regs) {
  char buffer[27] = "recieved interrupt!!!!!!!\n";
  print_serial(buffer, 27);
  print_screen(buffer, 27);

  char buffer2[10] = " ";
  integer_to_string(buffer2, regs.stack_contents.int_no);
  print_serial(buffer2, 10);
  print_screen(buffer2, 10);
}

// This gets called from our ASM interrupt handler stub.
void irq_interrupt_handler(registers_t regs) {
  // Send an EOI (end of interrupt) signal to the PICs.
  // If this interrupt involved the slave.
  if (regs.stack_contents.int_no >= 40) {
    // Send reset signal to slave.
    outb(0xA0, 0x20);
  }
  // Send reset signal to master. (As well as slave, if necessary).
  outb(0x20, 0x20);

  char buffer[27] = "recieved interrupt!!!!!!!\n";
  print_serial(buffer, 27);
  print_screen(buffer, 27);

  char buffer2[10] = " ";
  integer_to_string(buffer2, regs.stack_contents.int_no);
  print_serial(buffer2, 10);
  print_screen(buffer2, 10);

  /*if (interrupt_handlers[regs.stack_contents.int_no] != 0) {
    isr_t handler = interrupt_handlers[regs.int_no];
    handler(regs);
  }*/
}
