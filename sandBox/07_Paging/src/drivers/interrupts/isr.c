#include "isr.h"
#include <helpers.h>
#include <io.h>
#include <logger.h>

isr_t interrupt_handlers[256];
const u8int TIME_INTERRUPT_NUMBER = 32;
const u8int KEYBOARD_INTERRUPT_NUMBER = 33;

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

  /* Does not print if timer interrupt,
   * (avoiding too many messages on screen).
   */
  if ((regs.stack_contents.int_no != TIME_INTERRUPT_NUMBER) &&
      regs.stack_contents.int_no != KEYBOARD_INTERRUPT_NUMBER) {
    print_serial("\nRecieved interrupt ");
    print_screen("\nRecieved interrupt ");
    print_serial(integer_to_string(regs.stack_contents.int_no));
    print_screen(integer_to_string(regs.stack_contents.int_no));
    print_serial("!!\n");
    print_screen("!!\n");
  }

  /* If there is a callback function registered call that function */
  if (interrupt_handlers[regs.stack_contents.int_no] != 0) {
    isr_t handler = interrupt_handlers[regs.stack_contents.int_no];
    handler(regs);
  }
}
