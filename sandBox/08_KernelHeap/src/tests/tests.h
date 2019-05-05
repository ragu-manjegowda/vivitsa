#ifndef INCLUDE_TESTS_H
#define INCLUDE_TESTS_H

// Defined in kheap.c
extern u32int g_CurrentPhysicalAddressTop;

void run_all_tests() {
  // test logger
  print_screen("\nTesting Printf... it works!!!\n");
  print_serial("\nTesting COM port... it works!!!\n");

  // test interrupts
  print_screen("\nTesting Interrupt by raising interrupt 34\n");
  print_serial("\nTesting Interrupt by raising interrupt 34\n");
  asm volatile("int $0x22");

  // test paging
  print_screen("\nTesting paging, trying to access address ");
  print_screen(integer_to_string(g_CurrentPhysicalAddressTop));
  print_serial("\nTesting paging, trying to access address ");
  print_serial(integer_to_string(g_CurrentPhysicalAddressTop));
  print_screen("\n");
  print_serial("\n");
  u32int *ptr = (u32int *)g_CurrentPhysicalAddressTop;
  *ptr = 10;
  print_screen("Out of page fault, page allocation works!!!\n");
  print_serial("Out of page fault, page allocation works!!!\n");

  // test heap
  print_screen("\nTesting kernel heap.... \n");
  print_screen("\nCalling kmalloc() for allocating 8 bytes");
  u32int x = kmalloc(8);
  print_screen("\nkmalloc() allocated 8 bytes at address: ");
  print_screen(integer_to_string(x));
  print_screen("\nCalling kmalloc() for allocating 10 bytes");
  u32int y = kmalloc(10);
  print_screen("\nkmalloc() allocated 10 bytes at address: ");
  print_screen(integer_to_string(y));
  print_screen("\nCalling kmalloc() for allocating 8 bytes");
  u32int z = kmalloc(8);
  print_screen("\nkmalloc() allocated 8 bytes at address: ");
  print_screen(integer_to_string(z));
  print_screen("\n");

  // test keyboard
  print_screen("\nKeyboard Enabled, type something!!!!\n");
  print_serial("\nKeyboard Enabled, type something!!!!\n");
}

#endif /* INCLUDE_TESTS_H */
