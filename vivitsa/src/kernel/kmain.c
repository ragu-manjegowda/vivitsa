#include <fs.h>
#include <gdt.h>
#include <helpers.h>
#include <idt.h>
#include <initrd.h>
#include <kb.h>
#include <kheap.h>
#include <logger.h>
#include <paging.h>
#include <sched.h>
#include <serial_port.h>
#include <tests.h>
#include <timer.h>
#include <types.h>

/* Function to initialize */
void init(u32int mbootPointer, u32int stackPointer) {
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

  /*
   * Get Multiboot Information like module start address and multiboot physical
   * end address
   */
  u32int initrdPhysicalStart;
  u32int multibootPhysicalEnd;
  u32int modsCount;
  get_multiboot_info(mbootPointer, &initrdPhysicalStart, &multibootPhysicalEnd,
                     &modsCount);

  /* Initialise the initial ramdisk, and set it as the filesystem root */
  initialise_initrd(initrdPhysicalStart);

  /* Initialize paging */
  init_paging(multibootPhysicalEnd);

  /* Initialize multitasking */
  initialise_multitasking(stackPointer);

  /* Initialize keyboard */
  init_keyboard();
}

/* Kernel Main */
/* GRUB stores a pointer to a struct in the register ebx that,
 * describes at which addresses the modules are loaded.
 */
s32int kmain(u32int mbootPointer, u32int stackPointer) {
  // Initialize all modules
  init(mbootPointer, stackPointer);

  // Run init tests defined in tests.h
  run_all_tests();

  return 0;
}
