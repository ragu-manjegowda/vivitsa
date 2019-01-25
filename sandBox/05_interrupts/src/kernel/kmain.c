#include <gdt.h>
#include <idt.h>
#include <logger.h>
#include <serial_port.h>
#include <timer.h>
#include <types.h>

/* Function to initialize */
void init() {
  /* Initialize segment descriptor tables */
  init_gdt();

  /* Initialize segment descriptor tables */
  init_idt();

  /* Initialize timer interrupt */
  init_timer(TIMER_FREQUENCY);

  /* Initialize serial port */
  serial_configure(SERIAL_COM1_BASE, Baud_115200);
}

/* Kernel Main */
s32int kmain() {
  init();
  s8int buffer[20] = "This works!!!!!!!\n";
  print_screen(buffer, 20);
  print_serial(buffer, 20);
  asm volatile("int $0x3");
  asm volatile("int $0x22");
  return 0;
}
