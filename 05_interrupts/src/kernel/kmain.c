#include <gdt.h>
#include <idt.h>
#include <logger.h>
#include <serial_port.h>

/* Function to initialize */
void init() {
  /* Initialize segment descriptor tables */
  init_gdt();

  /* Initialize segment descriptor tables */
  init_idt();

  /* Initialize serial port */
  serial_configure(SERIAL_COM1_BASE, Baud_115200);
}

/* Kernel Main */
int kmain() {
  init();
  char buffer[20] = "This works!!!!!!!\n";
  kprint(buffer, 20);
  log(buffer, 20);
  asm volatile("int $0x3");
  return 0;
}
