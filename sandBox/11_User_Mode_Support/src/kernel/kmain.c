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
void init(uint32_t mbootPointer, uint32_t stackPointer) {
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
  uint32_t initrdPhysicalStart;
  uint32_t multibootPhysicalEnd;
  uint32_t modsCount;
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
int32_t kmain(uint32_t mbootPointer, uint32_t stackPointer) {
  // Initialize all modules
  init(mbootPointer, stackPointer);

  // Run init tests defined in tests.h
  run_all_tests();

  return 0;
}
