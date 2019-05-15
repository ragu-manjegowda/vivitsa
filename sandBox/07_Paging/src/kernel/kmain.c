#include "multiboot.h"
#include <gdt.h>
#include <helpers.h>
#include <idt.h>
#include <kb.h>
#include <logger.h>
#include <paging.h>
#include <serial_port.h>
#include <tests.h>
#include <timer.h>
#include <types.h>

/* Function to initialize */
void init(u32int kernelPhysicalStart, u32int kernelPhysicalEnd) {
  /* Initialize segment descriptor tables */
  init_gdt();

  /* Initialize segment descriptor tables */
  init_idt();

  /* Initialize timer interrupt */
  init_timer(TIMER_FREQUENCY);

  /* Initialize display */
  init_display();

  /* Configure serial port */
  serial_configure(SERIAL_COM1_BASE, Baud_115200);

  /* Initialize paging */
  init_paging(kernelPhysicalStart, kernelPhysicalEnd);

  /* Initialize keyboard */
  init_keyboard();
}

/* Kernel Main */
/* GRUB stores a pointer to a struct in the register ebx that,
 * describes at which addresses the modules are loaded.
 */
s32int kmain(u32int kernelPhysicalStart, u32int kernelPhysicalEnd) {
  // Initialize all modules
  init(kernelPhysicalStart, kernelPhysicalEnd);

  // Run init tests defined in tests.h
  run_all_tests();

  return 0;
}
