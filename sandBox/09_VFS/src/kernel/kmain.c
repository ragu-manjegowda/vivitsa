#include "multiboot.h"
#include <fs.h>
#include <gdt.h>
#include <helpers.h>
#include <idt.h>
#include <initrd.h>
#include <kb.h>
#include <kheap.h>
#include <logger.h>
#include <paging.h>
#include <serial_port.h>
#include <tests.h>
#include <timer.h>
#include <types.h>

/* Function to initialize */
void init(u32int kernelPhysicalEnd) {
  /* Initialize segment descriptor tables */
  init_gdt();

  /* Initialize segment descriptor tables */
  init_idt();

  /* Initialize timer interrupt */
  init_timer(TIMER_FREQUENCY);

  /* Initialize display */
  // init_display();

  /* Configure serial port */
  serial_configure(SERIAL_COM1_BASE, Baud_115200);

  /* Initialize paging */
  init_paging(kernelPhysicalEnd);

  /* Initialize keyboard */
  init_keyboard();
}

/* Kernel Main */
/* GRUB stores a pointer to a struct in the register ebx that,
 * describes at which addresses the modules are loaded.
 */
s32int kmain(u32int kernelPhysicalEnd, u32int mboot_ptr) {
  print_serial("\nKernel end address = ");
  print_serial(integer_to_string(kernelPhysicalEnd));

  print_serial("\nMboot address = ");
  print_serial(integer_to_string(mboot_ptr));

  multiboot_info_t *mbinfo = (multiboot_info_t *)mboot_ptr;
  u32int mods_count = mbinfo->mods_count;
  print_serial("\nMods count = ");
  print_serial(integer_to_string(mods_count));

  print_serial("\nMB info flags = ");
  print_serial(integer_to_string(mbinfo->flags));
  multiboot_module_t *mod = (multiboot_module_t *)mbinfo->mods_addr;
  u32int *multibootPhysicalEnd;
  u32int *initrdPhysicalStart;
  u32int i;
  for (i = 0; i < mbinfo->mods_count; i++, mod += sizeof(multiboot_module_t)) {
    print_serial("\nInitrd start location = ");
    print_serial(integer_to_string(mod->mod_start));
    initrdPhysicalStart = (u32int *)mod->mod_start;
    print_serial("\nInitrd numfiles = ");
    print_serial(integer_to_string(*(u32int *)mod->mod_start));
    print_serial("\nInitrd end location = ");
    print_serial(integer_to_string(mod->mod_end));
    multibootPhysicalEnd = (u32int *)mod->mod_end;
    print_serial("\nInitrd string = ");
    print_serial(integer_to_string(mod->cmdline));
  }

  // Initialize all modules
  init((u32int)multibootPhysicalEnd);

  // Initialise the initial ramdisk, and set it as the filesystem root.
  fs_node_t *fs_root = initialise_initrd((u32int)initrdPhysicalStart);

  print_screen("Found file ");

  // list the contents of /
  i = 0;
  dir_entry_t *node = 0;
  while ((node = readdir_fs(fs_root, i)) != 0) {
    print_screen("Found file ");
    print_screen(node->name);
    fs_node_t *fsnode = finddir_fs(fs_root, node->name);

    if ((fsnode->type & 0x7) == FS_DIRECTORY) {
      print_screen("\n\t(directory)\n");
    } else {
      print_screen("\n\t contents: \"");
      char buf[256];
      u32int sz = read_fs(fsnode, 0, 256, (u8int *)&(buf[0]));
      u32int j;
      for (j = 0; j < sz; j++)
        print_screen_ch(buf[j]);

      print_screen("\"\n");
    }
    i++;
  }

  // Run init tests defined in tests.h
  // run_all_tests();*/

  return 0;
}
