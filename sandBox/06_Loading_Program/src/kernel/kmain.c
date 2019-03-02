#include "multiboot.h"
#include <gdt.h>
#include <helpers.h>
#include <idt.h>
#include <kb.h>
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

  /* Initialize display */
  init_display();

  /* Initialize serial port */
  serial_configure(SERIAL_COM1_BASE, Baud_115200);

  /* Initialize keyboard */
  init_keyboard();
}

/* Kernel Main */
/* GRUB stores a pointer to a struct in the register ebx that,
 * describes at which addresses the modules are loaded.
 */
s32int kmain(u32int ebx) {
  init();
  multiboot_info_t *mbinfo = (multiboot_info_t *)ebx;
  // u32int address_of_module = mbinfo->mods_addr;
  u32int mods_count = mbinfo->mods_count;
  print_screen("Number of modules loaded = ");
  print_serial("Number of modules loaded = ");
  print_screen(integer_to_string(mods_count));
  print_serial(integer_to_string(mods_count));

  asm volatile("int $0x3");
  asm volatile("int $0x22");
  return 0;
}
