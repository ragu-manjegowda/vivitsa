#include "kheap.h"
#include <logger.h>
#include <ordered_array.h>
#include <paging.h>

typedef struct {
  /* Array of holes sorted by size (ordered array) */
  ordered_array_t index;
  /* The start of heap. */
  u32int start_address;
  /* The end of heap. Can be expanded up to max_address. */
  u32int end_address;
  /* The maximum address the heap can be expanded to. */
  u32int max_address;
  /* If set, accesible only in supervisor mode */
  u8int supervisor;
  /* Page permission (if set, read only) */
  u8int readonly;
} heap_t;

typedef struct {
  u32int magic;  // Magic number, used for error checking and identification.
  u8int is_hole; // 1 if this is a hole. 0 if this is a block.
  u32int size;   // size of the block, including the end footer.
} header_t;

typedef struct {
  u32int magic;     // Magic number, same as in header_t.
  header_t *header; // Pointer to the block header.
} footer_t;

u32int g_CurrentPhysicalAddressTop = 0;
heap_t *g_KernelHeap = 0;

/* Defined in paging.c (using extern for the sake of security, so that these are
 * not exposed in header)
 */
extern page_directory_t *g_kernelDirectory;

void set_physical_address_top(u32int kernelPhysicalEnd) {
  g_CurrentPhysicalAddressTop = kernelPhysicalEnd;
}

static s32int find_smallest_hole(u32int size, u8int page_align, heap_t *heap) {
  /* Find the smallest hole that will fit. */
  u32int iterator = 0;

  while (iterator < heap->index.size) {
    header_t *header = (header_t *)peek_ordered_array(iterator, &heap->index);

    /* If the user has requested the memory be page-aligned */
    if (page_align > 0) {
      /* Page-align the starting point of this header. */
      u32int location = (u32int)header;
      s32int offset = 0;

      if (((location + sizeof(header_t)) & 0xFFFFF000) != 0) {
        offset = 0x1000 - ((location + sizeof(header_t)) % 0x1000);
      }

      s32int hole_size = (s32int)header->size - offset;

      /* Can we fit now? */
      if (hole_size >= (s32int)size)
        break;
    } else if (header->size >= size) {
      break;
    }
    iterator++;
  }

  /* We got to the end and didn't find anything. */
  if (iterator == heap->index.size) {
    return -1;
  } else {
    return iterator;
  }
}

static void expand(u32int new_size, heap_t *heap) {
  if (new_size <= heap->end_address - heap->start_address) {
    print_screen("\nError: Expand: New Size is not greater than old size!!!\n");
    return;
  }

  /* Get the nearest following page boundary. */
  if (new_size & 0x00000FFF) {
    new_size &= 0xFFFFF000;
    new_size += 0x1000;
  }

  /* Make sure we are not overreaching ourselves. */
  if (heap->start_address + new_size > heap->max_address) {
    print_screen("\nError: Expand: New Size is greater than max size!!!\n");
    return;
  }

  u32int old_size = heap->end_address - heap->start_address;

  u32int i = old_size;
  /* Allocate frames for new memory area */
  while (i < new_size) {
    alloc_frame(get_page(heap->start_address + i, 1, g_kernelDirectory),
                (heap->supervisor) ? 1 : 0, (heap->readonly) ? 0 : 1);
    i += 0x1000;
  }
  heap->end_address = heap->start_address + new_size;
}

void *alloc(u32int size, u8int page_align, heap_t *heap) {

  /* Make sure we take the size of header/footer into account. */
  u32int new_size = size + sizeof(header_t) + sizeof(footer_t);

  /* Find the smallest hole that will fit. */
  s32int iterator = find_smallest_hole(new_size, page_align, heap);

  /* If we didn't find a suitable hole */
  if (iterator == -1) {
    /* Save previous data. */
    u32int old_length = heap->end_address - heap->start_address;
    u32int old_end_address = heap->end_address;

    /* We need to allocate some more space. */
    expand(old_length + new_size, heap);
    u32int new_length = heap->end_address - heap->start_address;

    /*
     * Since there is not hole available and we have now expanded our heap, we
     * need either add the new hole to list of available holes or create a new
     * entry on sorted array of holes if this is first insertion
     */
    iterator = 0;
    s32int idx = -1;
    u32int value = 0x0;

    /* TODO: Refactor to code below */
    /*u32int tmp =
        (u32int)peek_ordered_array((heap->index.size - 1), &heap->index);
    if (tmp > value) {
      value = tmp;
      idx = iterator;
    }*/

    while ((u32int)iterator < heap->index.size) {
      u32int tmp = (u32int)peek_ordered_array(iterator, &heap->index);
      if (tmp > value) {
        value = tmp;
        idx = iterator;
      }
      iterator++;
    }

    /* If we didn't find ANY headers, we need to add one. */
    if (idx == -1) {
      header_t *header = (header_t *)old_end_address;
      header->magic = HEAP_MAGIC;
      header->size = new_length - old_length;
      header->is_hole = 1;
      footer_t *footer =
          (footer_t *)(old_end_address + header->size - sizeof(footer_t));
      footer->magic = HEAP_MAGIC;
      footer->header = header;
      insert_ordered_array((type_t)header, &heap->index);
    } else {
      /*
       * This will probably be the last header that has size less han required
       * and hence needs adjusting.
       */
      header_t *header = peek_ordered_array(idx, &heap->index);
      header->size += new_length - old_length;
      // Rewrite the footer.
      footer_t *footer =
          (footer_t *)((u32int)header + header->size - sizeof(footer_t));
      footer->header = header;
      footer->magic = HEAP_MAGIC;
    }
    // TODO: Check if can simply declare header outside if block and return it
    // We now have enough space. Recurse, and call the function again.
    return alloc(size, page_align, heap);
  }

  /* If we find the smallest hole that will fit */
  header_t *orig_hole_header =
      (header_t *)peek_ordered_array(iterator, &heap->index);
  u32int orig_hole_pos = (u32int)orig_hole_header;
  u32int orig_hole_size = orig_hole_header->size;

  /*
   * Here we work out if we should split the hole we found into two parts.
   * We will skip splitting the hole into two if the original hole size -
   * requested hole size less than the overhead for adding a new hole as we will
   * not have any space left to allocate in the new block.
   */
  if ((orig_hole_size - new_size) < (sizeof(header_t) + sizeof(footer_t))) {
    size += orig_hole_size - new_size;
    new_size = orig_hole_size;
  }

  /*
   * If we need to page-align the data, do it now and make a new hole before
   * our new block.
   */
  if (page_align && (orig_hole_pos & 0x00000FFF)) {
    u32int new_location =
        orig_hole_pos + 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(header_t);
    header_t *hole_header = (header_t *)orig_hole_pos;
    hole_header->size = 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(header_t);
    hole_header->magic = HEAP_MAGIC;
    hole_header->is_hole = 1;
    footer_t *hole_footer =
        (footer_t *)((u32int)new_location - sizeof(footer_t));
    hole_footer->magic = HEAP_MAGIC;
    hole_footer->header = hole_header;
    /* TODO: Check new hole size and don't add if it crossess boundary of next
     * header, and check if we are adding this hole to our list of holes
     */
    orig_hole_pos = new_location;
    orig_hole_size = orig_hole_size - hole_header->size;
  } else {
    /*
     * Else we don't need this hole any more as we will be allocating it, delete
     * it from the index.
     */
    remove_ordered_array(iterator, &heap->index);
  }

  /*
   * Create header and footer if this is newly allocated block or Overwrite the
   * original if we expanded the last hole.
   */
  header_t *block_header = (header_t *)orig_hole_pos;
  block_header->magic = HEAP_MAGIC;
  block_header->is_hole = 0;
  block_header->size = new_size;
  footer_t *block_footer =
      (footer_t *)(orig_hole_pos + sizeof(header_t) + size);
  block_footer->magic = HEAP_MAGIC;
  block_footer->header = block_header;

  // We may need to write a new hole after the allocated block.
  // We do this only if the new hole would have positive size...
  if (orig_hole_size - new_size > 0) {
    header_t *hole_header = (header_t *)(orig_hole_pos + sizeof(header_t) +
                                         size + sizeof(footer_t));
    hole_header->magic = HEAP_MAGIC;
    hole_header->is_hole = 1;
    hole_header->size = orig_hole_size - new_size;
    footer_t *hole_footer = (footer_t *)((u32int)hole_header + orig_hole_size -
                                         new_size - sizeof(footer_t));
    if ((u32int)hole_footer < heap->end_address) {
      hole_footer->magic = HEAP_MAGIC;
      hole_footer->header = hole_header;
    }
    /*
     * Put the new hole in the index. If the footer is beyond the heap end
     * address we will still add the hole to index so that we can expand the
     * last hole without fragmentation.
     */
    insert_ordered_array((void *)hole_header, &heap->index);
  }

  return (void *)((u32int)block_header + sizeof(header_t));
}

u32int kmalloc_int(u32int size, u32int align, u32int *pAddrPtr) {
  /* if Kernel Heap is created and allocated */
  if (g_KernelHeap != 0) {
    void *addr = alloc(size, (u8int)align, g_KernelHeap);
    if (pAddrPtr != 0) {
      page_t *page = get_page((u32int)addr, 0, g_kernelDirectory);
      // TODO: Addr is already aligned, check if physicalAddr needs this offset
      *pAddrPtr = page->frame * 0x1000 + ((u32int)addr & 0xFFF);
    }
    return (u32int)addr;
  } else {
    if (align == 1) {
      if (g_CurrentPhysicalAddressTop & 0x00000FFF) {
        // Align the placement address;
        g_CurrentPhysicalAddressTop &= 0xFFFFF000;
        g_CurrentPhysicalAddressTop += 0x1000;
      }
    }
    if (pAddrPtr) {
      *pAddrPtr = g_CurrentPhysicalAddressTop;
    }
    u32int tmp = g_CurrentPhysicalAddressTop;
    g_CurrentPhysicalAddressTop += size;
    return tmp;
  }
}

u32int kmalloc_a(u32int size) { return kmalloc_int(size, 1, 0); }

u32int kmalloc_p(u32int size, u32int *pAddrPtr) {
  return kmalloc_int(size, 0, pAddrPtr);
}

u32int kmalloc_ap(u32int size, u32int *pAddrPtr) {
  return kmalloc_int(size, 1, pAddrPtr);
}

u32int kmalloc(u32int size) { return kmalloc_int(size, 0, 0); }

/*
 * Compare function for ordered array, sorted by header size ascending order
 */
static s8int header_t_less_than(void *a, void *b) {
  return (((header_t *)a)->size < ((header_t *)b)->size) ? 1 : 0;
}

void create_heap(u32int start, u32int end_addr, u32int max, u8int supervisor,
                 u8int readonly) {
  heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));

  if (start % 0x1000 != 0) {
    print_screen("\nError: create_heap: Start Address is not page aligned\n");
    return;
  }

  if (end_addr % 0x1000 != 0) {
    print_screen("\nError: create_heap: End Address is not page aligned\n");
    return;
  }

  /* Initialise the index. */
  heap->index = place_ordered_array((void *)start, HEAP_INDEX_SIZE,
                                    (compare_predicate_t)&header_t_less_than);

  /*
   * Shift the start address forward to resemble where we can start putting
   * data.
   */
  start += sizeof(type_t) * HEAP_INDEX_SIZE;

  /* Make sure the start address is page-aligned. */
  if (start & 0x00000FFF) {
    start &= 0xFFFFF000;
    start += 0x1000;
  }

  /* Write the start, end and max addresses into the heap structure. */
  heap->start_address = start;
  heap->end_address = end_addr;
  heap->max_address = max;
  heap->supervisor = supervisor;
  heap->readonly = readonly;

  /* We start off with one large hole in the index. */
  header_t *hole = (header_t *)start;
  hole->size = end_addr - start;
  hole->magic = HEAP_MAGIC;
  hole->is_hole = 1;
  insert_ordered_array((void *)hole, &heap->index);

  g_KernelHeap = heap;
}
