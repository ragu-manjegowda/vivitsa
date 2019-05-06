#ifndef INCLUDE_TESTS_H
#define INCLUDE_TESTS_H

#pragma once
#include <timer.h>

// Defined in kheap.c
extern u32int g_CurrentPhysicalAddressTop;

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
  u32int *ptr = (u32int *)(g_CurrentPhysicalAddressTop + 0x1000);
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
  u32int *x = (u32int *)kmalloc(8);
  print_screen("\nkmalloc() allocated 8 bytes at address: ");
  print_screen(integer_to_string((u32int)x));
  print_screen("\n");
  print_serial("\nkmalloc() allocated 8 bytes at address: ");
  print_serial(integer_to_string((u32int)x));
  print_serial("\n");

  print_screen("\nCalling kmalloc() for allocating 10 bytes");
  print_serial("\nCalling kmalloc() for allocating 10 bytes");
  u32int y = kmalloc(10);
  print_screen("\nkmalloc() allocated 10 bytes at address: ");
  print_screen(integer_to_string(y));
  print_screen("\n");
  print_serial("\nkmalloc() allocated 10 bytes at address: ");
  print_serial(integer_to_string(y));
  print_serial("\n");

  print_screen("\nCalling kmalloc() for allocating 8 bytes");
  print_serial("\nCalling kmalloc() for allocating 8 bytes");
  u32int z = kmalloc(8);
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
  print_screen(integer_to_string((u32int)x));
  print_serial(integer_to_string((u32int)x));
  print_screen("\n");
  print_serial("\n");
  kfree((void *)x);

  print_screen("\nCalling kmalloc() for allocating 20 bytes");
  print_serial("\nCalling kmalloc() for allocating 20 bytes");
  x = (u32int *)kmalloc(20);
  print_screen("\nkmalloc() allocated 20 bytes at address: ");
  print_screen(integer_to_string((u32int)x));
  print_screen("\n");
  print_serial("\nkmalloc() allocated 20 bytes at address: ");
  print_serial(integer_to_string((u32int)x));
  print_serial("\n");

  print_screen("\nCalling kmalloc() for allocating 8 bytes");
  print_serial("\nCalling kmalloc() for allocating 8 bytes");
  u32int *p = (u32int *)kmalloc(8);
  print_screen("\nkmalloc() allocated 8 bytes at address: ");
  print_screen(integer_to_string((u32int)p));
  print_screen("\n");
  print_serial("\nkmalloc() allocated 8 bytes at address: ");
  print_serial(integer_to_string((u32int)p));
  print_serial("\n");

  print_screen("\nTesting continues please wait...");
  /* Sleep for 10 seconds (1000 centiSeconds) */
  sleep(1000);
  clear_screen();

  // test keyboard
  print_screen("\n==============================");
  print_serial("\n==============================");
  print_screen("\nKeyboard Enabled, type something!!!!\n");
  print_serial("\nKeyboard Enabled, type something!!!!\n");
}

#endif /* INCLUDE_TESTS_H */
