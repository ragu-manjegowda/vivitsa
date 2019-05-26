#include "paging.h"
#include <helpers.h>
#include <isr.h>
#include <kheap.h>
#include <logger.h>

/* The kernel's page directory */
page_directory_t *g_kernelDirectory = 0;

/* The current page directory */
page_directory_t *g_currentDirectory = 0;

/* Variables for house keeping, whether page frame is free or used */
u32int *g_FrameIndexArray;
u32int g_NumOfFrames;

/*
 * Accessing g_CurrentPhysicalAddressTop defined in kheap.c through extern since
 * we use it in page_fault handler
 */
extern u32int g_CurrentPhysicalAddressTop;

/* Macros used in the bitset algorithms */
#define INDEX_FROM_BIT(a) (a / 32)
#define OFFSET_FROM_BIT(a) (a % 32)

/* Page Fault interrupt handler function forward declaration, definition towards
 * the end
 */
void page_fault(registers_t regs);

/* Function to mark corresponding frame in the variable as not free */
static void set_frame(u32int frameAddr) {
  u32int frame = frameAddr / 0x1000;
  u32int idx = INDEX_FROM_BIT(frame);
  u32int off = OFFSET_FROM_BIT(frame);
  g_FrameIndexArray[idx] |= (0x1 << off);
}

/* Function to mark corresponding frame in the variable as free */
static void clear_frame(u32int frameAddr) {
  u32int frame = frameAddr / 0x1000;
  u32int idx = INDEX_FROM_BIT(frame);
  u32int off = OFFSET_FROM_BIT(frame);
  g_FrameIndexArray[idx] &= ~(0x1 << off);
}

/* Utility function to get index of first free frame */
static s32int first_frame() {
  u32int i, j;
  for (i = 0; i < INDEX_FROM_BIT(g_NumOfFrames); ++i) {
    if (g_FrameIndexArray[i] != 0xFFFFFFFF) {
      for (j = 0; j < 32; ++j) {
        u32int toTest = 0x1 << j;
        if (!(g_FrameIndexArray[i] & toTest)) {
          return ((i * 32) + j);
        }
      }
    }
  }
  /* No free frames available */
  return -1;
}

void alloc_frame(page_t *page, u32int isKernel, u32int isWriteable) {
  if (page->frame != 0) {
    return;
  } else {
    u32int idx = first_frame();
    if (idx == (u32int)-1) {
      print_screen("No Free Frame, Kernel Panic");
      while (1) {
      }
    }
    set_frame(idx * 0x1000);
    page->present = 1;
    page->rw = (isWriteable) ? 1 : 0;
    page->user = (isKernel) ? 0 : 1;
    page->frame = idx;
  }
}

void free_frame(page_t *page) {
  u32int frame;
  if (!(frame = page->frame)) {
    return;
  } else {
    clear_frame(frame);
    page->frame = 0x0;
  }
}

void init_paging(u32int kernelPhysicalEnd) {

  set_physical_address_top(kernelPhysicalEnd);

  /*
   * The size of physical memory.
   * Assuming it is KHEAP_MAX_ADDRESS big
   */
  u32int memPageEnd = KHEAP_MAX_ADDRESS;

  g_NumOfFrames = (memPageEnd / 0x1000) + 1;
  g_FrameIndexArray = (u32int *)kmalloc(INDEX_FROM_BIT(g_NumOfFrames));
  custom_memset((u8int *)g_FrameIndexArray, 0, INDEX_FROM_BIT(g_NumOfFrames));

  /* Create a page directory for kernel and set it as curent page directory */
  g_kernelDirectory = (page_directory_t *)kmalloc_a(sizeof(page_directory_t));
  custom_memset((u8int *)g_kernelDirectory, 0, sizeof(page_directory_t));
  g_currentDirectory = g_kernelDirectory;

  /*
   * Call to get pages only forces page tables to be created. We will map them
   * before we actually allocate
   */
  u32int i = 0;
  for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000)
    get_page(i, 1, g_kernelDirectory);

  /*
   * We need to identity map (phys addr = virt addr) from 0x0 to the end of
   * used memory, so we can access this transparently, as if paging wasn't
   * enabled.
   */
  for (i = 0; i < g_CurrentPhysicalAddressTop + 0x1000; i += 0x1000) {
    /* Setting page readable but not writeable from userspace */
    alloc_frame(get_page(i, 1, g_kernelDirectory), 0, 0);
  }

  /* Now allocate those pages for heap */
  for (i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += 0x1000) {
    alloc_frame(get_page(i, 1, g_kernelDirectory), 0, 0);
  }

  /* Register our page fault handler */
  register_interrupt_handler(14, page_fault);

  /* Enable paging! */
  switch_page_directory(g_kernelDirectory);

  /*  Initialise the kernel heap */
  create_kernel_heap(KHEAP_START, KHEAP_START + KHEAP_INITIAL_SIZE,
                     KHEAP_MAX_ADDRESS);
}

void switch_page_directory(page_directory_t *dir) {
  g_currentDirectory = dir;
  /* Write page table physical address to cr3 */
  asm volatile("mov %0, %%cr3" ::"r"(&dir->tablesPhysical));
  u32int cr0;
  /* Read cr0 register to variable cr0 */
  asm volatile("mov %%cr0, %0" : "=r"(cr0));
  /* Set enable paging bit of the cr0 variable ! */
  cr0 |= 0x80000000;
  /* Write back cr0 variable value to cr0 register */
  asm volatile("mov %0, %%cr0" ::"r"(cr0));
}

page_t *get_page(u32int address, u8int make, page_directory_t *dir) {
  /* Turn the address into an index */
  address /= 0x1000;
  /* Find the page table containing this address */
  u32int tableIdx = address / 1024;
  /* Return page if it is already created */
  if (dir->tables[tableIdx]) {
    return &dir->tables[tableIdx]->pages[address % 1024];
  } else if (make) {
    u32int tmp;
    dir->tables[tableIdx] =
        (page_table_t *)kmalloc_ap(sizeof(page_table_t), &tmp);
    custom_memset((u8int *)dir->tables[tableIdx], 0, 0x1000);
    /* PRESENT, RW, US. */
    dir->tablesPhysical[tableIdx] = tmp | 0x7;
    return &dir->tables[tableIdx]->pages[address % 1024];
  } else {
    return 0;
  }
}

/* Page Fault interrupt handler function */
void page_fault(registers_t regs) {
  /*
   * A page fault has occurred.
   * The faulting address is stored in the CR2 register.
   */
  u32int faultingAddress;
  asm volatile("mov %%cr2, %0" : "=r"(faultingAddress));

  /* The error code gives us details of what happened. */
  /* Page not present */
  u32int present = !(regs.stack_contents.err_code & 0x1);
  /* Read Only */
  u32int rWrite = regs.stack_contents.err_code & 0x2;
  /* Accessing kernel page from User mode */
  u32int uMode = regs.stack_contents.err_code & 0x4;
  /* Overwritten CPU-reserved bits of page entry */
  u32int reserved = regs.stack_contents.err_code & 0x8;
  /* Caused by an instruction fetch */
  u32int iFetch = regs.stack_contents.err_code & 0x10;

  print_screen("Page fault! ( ");
  print_serial("Page fault! ( ");
  if (present) {
    print_screen("not present ");
  } else {
    if (rWrite) {
      print_screen("read-only ");
    }
    if (uMode) {
      print_screen("user-mode ");
    }
    if (reserved) {
      print_screen("reserved ");
    }
    if (iFetch) {
      print_screen("instruction fetch ");
    }
  }
  print_screen(") at address = ");
  print_screen(integer_to_string(faultingAddress));
  print_screen("\n");
  print_serial(") at address = ");
  print_serial(integer_to_string(faultingAddress));
  print_serial("\n");

  print_screen("Creating page at address ");
  print_screen(integer_to_string(faultingAddress));
  print_screen("\n");
  print_serial("Creating page at address ");
  print_serial(integer_to_string(faultingAddress));
  print_serial("\n");

/* For testing we will allocate frame when there is page not found fault */
#if 1
  alloc_frame(get_page(faultingAddress, 1, g_kernelDirectory), 0, 0);
#endif

  /* Optionally we can stop execution here, disabling this so that paging can
   * be tested by doing the page fault
   */
  // while (1) {}
}
