#include "isr.h"
#include <helpers.h>
#include <io.h>
#include <logger.h>

isr_t interrupt_handlers[256];

/* Function to register interrupt handler with custom call back function */
void register_interrupt_handler(u8int n, isr_t handler) {
  interrupt_handlers[n] = handler;
}

/* This gets called from our ASM interrupt handler stub. */
void interrupt_handler(registers_t regs) {
  /* Send an EOI (end of interrupt) signal to the PICs. */

  /* If this interrupt involved PIC2/slave. */
  if (regs.stack_contents.int_no >= 40) {
    outb(0xA0, 0x20);
  }
  /* Send reset signal to PIC1/master. */
  if (regs.stack_contents.int_no >= 32) {
    outb(0x20, 0x20);
  }

  s8int buffer[27] = "recieved interrupt!!!!!!!\n";
  print_serial(buffer, 27);
  print_screen(buffer, 27);

  /* Currently there are only 47 handlers so digit cannot be more than 2, hence
   * defining buffer of length 4 (more space to print '\n')
   */
  s8int buffer2[4] = " ";
  integer_to_string(buffer2, regs.stack_contents.int_no);
  buffer2[3] = '\n';
  print_serial(buffer2, 4);
  print_screen(buffer2, 4);

  /* If there is a callback function registered call that function */
  if (interrupt_handlers[regs.stack_contents.int_no] != 0) {
    isr_t handler = interrupt_handlers[regs.stack_contents.int_no];
    handler(regs);
  }
}
