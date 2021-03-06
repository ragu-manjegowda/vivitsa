#ifndef INCLUDE_TESTS_H
#define INCLUDE_TESTS_H

/* Forward declare sleep function from timer.h */
void sleep(uint32_t centiSeconds);

/* Defined in kheap.c */
extern uint32_t g_CurrentPhysicalAddressTop;
/* Defined in initrd.c */
extern fs_node_t *g_INITRD_ROOT_DIR;

void run_all_tests() {

  // test logger
  print_screen("\n==============================");
  print_serial("\n==============================");
  print_screen("\nTesting Printf... it works!!!");
  print_serial("\nTesting COM port... it works!!!");

  // test interrupts
  print_screen("\n\n==============================");
  print_serial("\n\n==============================");
  print_screen("\nTesting Interrupt by raising interrupt 34");
  print_serial("\nTesting Interrupt by raising interrupt 34");
  asm volatile("int $0x22");

  // test paging
  print_screen("\n\n==============================");
  print_serial("\n\n==============================");
  print_screen("\nTesting paging, trying to access address ");
  print_serial("\nTesting paging, trying to access address ");
  print_screen(integer_to_string(g_CurrentPhysicalAddressTop + 0x1000));
  print_serial("\nTesting paging, trying to access address ");
  print_serial(integer_to_string(g_CurrentPhysicalAddressTop + 0x1000));
  print_screen("\n");
  print_serial("\n");
  uint32_t *ptr = (uint32_t *)(g_CurrentPhysicalAddressTop + 0x1000);
  *ptr = 10;
  print_screen("Out of page fault, page allocation works!!!\n");
  print_serial("Out of page fault, page allocation works!!!\n");

  print_screen("\nTesting continues please wait...");
  /* Sleep for 10 seconds (1000 centiSeconds) */
  sleep(1000);
  clear_screen();

  // test heap
  print_screen("\n==============================");
  print_serial("\n==============================");
  print_screen("\nTesting kernel heap.... ");
  print_serial("\nTesting kernel heap.... ");

  print_screen("\n==============================");
  print_serial("\n==============================");
  print_screen("\nTesting kernel malloc.... ");
  print_serial("\nTesting kernel malloc.... ");

  print_screen("\nCalling kmalloc() for allocating 8 bytes");
  print_serial("\nCalling kmalloc() for allocating 8 bytes");
  uint32_t *x = (uint32_t *)kmalloc(8);
  print_screen("\nkmalloc() allocated 8 bytes at address: ");
  print_screen(integer_to_string((uint32_t)x));
  print_screen("\n");
  print_serial("\nkmalloc() allocated 8 bytes at address: ");
  print_serial(integer_to_string((uint32_t)x));
  print_serial("\n");

  print_screen("\nCalling kmalloc() for allocating 10 bytes");
  print_serial("\nCalling kmalloc() for allocating 10 bytes");
  uint32_t y = kmalloc(10);
  print_screen("\nkmalloc() allocated 10 bytes at address: ");
  print_screen(integer_to_string(y));
  print_screen("\n");
  print_serial("\nkmalloc() allocated 10 bytes at address: ");
  print_serial(integer_to_string(y));
  print_serial("\n");

  print_screen("\nCalling kmalloc() for allocating 8 bytes");
  print_serial("\nCalling kmalloc() for allocating 8 bytes");
  uint32_t z = kmalloc(8);
  print_screen("\nkmalloc() allocated 8 bytes at address: ");
  print_screen(integer_to_string(z));
  print_screen("\n");
  print_serial("\nkmalloc() allocated 8 bytes at address: ");
  print_serial(integer_to_string(z));
  print_serial("\n");

  print_screen("\n==============================");
  print_serial("\n==============================");
  print_screen("\nTesting kernel free.... ");
  print_serial("\nTesting kernel free.... ");

  print_screen("\nCalling kfree() to deallocate address: ");
  print_serial("\nCalling kfree() to deallocate address: ");
  print_screen(integer_to_string((uint32_t)x));
  print_serial(integer_to_string((uint32_t)x));
  print_screen("\n");
  print_serial("\n");
  kfree((void *)x);

  print_screen("\nCalling kmalloc() for allocating 20 bytes");
  print_serial("\nCalling kmalloc() for allocating 20 bytes");
  x = (uint32_t *)kmalloc(20);
  print_screen("\nkmalloc() allocated 20 bytes at address: ");
  print_screen(integer_to_string((uint32_t)x));
  print_screen("\n");
  print_serial("\nkmalloc() allocated 20 bytes at address: ");
  print_serial(integer_to_string((uint32_t)x));
  print_serial("\n");

  print_screen("\nCalling kmalloc() for allocating 8 bytes");
  print_serial("\nCalling kmalloc() for allocating 8 bytes");
  uint32_t *p = (uint32_t *)kmalloc(8);
  print_screen("\nkmalloc() allocated 8 bytes at address: ");
  print_screen(integer_to_string((uint32_t)p));
  print_screen("\n");
  print_serial("\nkmalloc() allocated 8 bytes at address: ");
  print_serial(integer_to_string((uint32_t)p));
  print_serial("\n");

  print_screen("\nTesting continues please wait...");
  /* Sleep for 10 seconds (1000 centiSeconds) */
  sleep(1000);
  clear_screen();

  // test multiboot
  print_screen("\n==============================");
  print_serial("\n==============================");
  print_screen("\nTesting Multiboot...");
  print_serial("\nTesting Multiboot...");

  uint32_t mboot_ptr = get_multiboot_address();
  print_screen("\nMboot address loaded at ");
  print_serial("\nMboot address loaded at ");
  print_screen(integer_to_string(mboot_ptr));
  print_serial(integer_to_string(mboot_ptr));

  uint32_t initrdPhysicalStart;
  uint32_t multibootPhysicalEnd;
  uint32_t modsCount;
  get_multiboot_info(mboot_ptr, &initrdPhysicalStart, &multibootPhysicalEnd,
                     &modsCount);
  print_screen("\nInitrd loaded at address ");
  print_serial("\nInitrd loaded at address ");
  print_screen(integer_to_string(initrdPhysicalStart));
  print_serial(integer_to_string(initrdPhysicalStart));
  print_screen("\nMultiboot modules end at address ");
  print_serial("\nMultiboot modules end at address ");
  print_screen(integer_to_string(multibootPhysicalEnd));
  print_serial(integer_to_string(multibootPhysicalEnd));
  print_screen("\nNumber of modules loaded = ");
  print_serial("\nNumber of modules loaded = ");
  print_screen(integer_to_string(modsCount));
  print_serial(integer_to_string(modsCount));

  // test virtual file system
  print_screen("\n\n==============================");
  print_serial("\n\n==============================");
  print_screen("\nTesting virtual file system");
  print_screen("\nreading contents of directory /dev\n");
  print_serial("\nTesting virtual file system");
  print_serial("\nreading contents of directory /dev\n");

  // list the contents of /dev
  fs_node_t *node = (fs_node_t *)kmalloc(sizeof(fs_node_t));
  /* root (/) at this point has just /dev so directly access contents */
  fs_node_t *fs_root = g_INITRD_ROOT_DIR->contents;
  for (uint32_t i = 0; i < fs_root->size; ++i) {
    readdir_fs(fs_root, i, node);
    if (node->type == FS_FILE) {
      print_screen("\nFound file -> ");
      print_serial("\nFound file -> ");
      print_screen(node->name);
      print_serial(node->name);
      print_screen("\n\t contents: ");
      print_serial("\n\t contents: ");
      int8_t buf[LEN_256];
      uint32_t sz = read_fs(node, 0, LEN_256, (uint8_t *)&(buf[0]));
      uint32_t j;
      for (j = 0; j < sz; j++) {
        print_screen_ch(buf[j]);
        print_serial_ch(buf[j]);
      }
      print_screen("\n");
      print_serial("\n");
    } else {
      print_screen("\nFound dir ");
      print_serial("\nFound dir ");
      print_screen(node->name);
      print_serial(node->name);
      print_screen("\n");
      print_serial("\n");
    }
  }

  print_screen("\nTesting continues please wait...");
  /* Sleep for 10 seconds (1000 centiSeconds) */
  sleep(1000);
  clear_screen();

  // test multitasking
  print_screen("\n\n==============================");
  print_serial("\n\n==============================");
  print_screen("\nTesting multitasking");
  print_screen("\nCalling process fork\n");
  print_serial("\nTesting multitasking");
  print_serial("\nCalling process fork\n");

  uint32_t ret = fork();

  print_screen("\n\n==============================\n");
  print_serial("\n\n==============================\n");
  print_screen("fork() returned ");
  print_serial("fork() returned ");
  print_screen(integer_to_string(ret));
  print_serial(integer_to_string(ret));
  print_screen(", and getpid() returned ");
  print_serial(", and getpid() returned ");
  print_screen(integer_to_string(getpid()));
  print_serial(integer_to_string(getpid()));

  print_screen("\n============================================================"
               "================\n");
  print_serial("\n============================================================"
               "================\n");

  // test keyboard
  print_screen("\n==============================");
  print_serial("\n==============================");
  print_screen("\nKeyboard Enabled, type something!!!!\n");
  print_serial("\nKeyboard Enabled, type something!!!!\n");
}

#endif /* INCLUDE_TESTS_H */
