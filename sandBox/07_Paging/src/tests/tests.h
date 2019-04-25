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

  // test keyboard
  print_screen("\nKeyboard Enabled, type something!!!!\n");
  print_serial("\nKeyboard Enabled, type something!!!!\n");
}

#endif /* INCLUDE_TESTS_H */
