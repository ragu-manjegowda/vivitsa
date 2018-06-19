#include <gdt.h>
#include <logger.h>
#include <serial_port.h>

/* Function to initialize */
void init() {
  /* Initialize segment descriptor tables */
  init_gdt();

  /* Initialize serial port */
  serial_configure(SERIAL_COM1_BASE, Baud_115200);
}

/* Kernel Main */
int kmain() {
  init();
  char buffer[14] = "This works!!!\n";
  kprint(buffer, 14);
  log(buffer, 14);
  return 0;
}
