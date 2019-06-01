#include "kheap.h"
#include <logger.h>
#include <ordered_array.h>
#include <paging.h>

typedef struct {
  /* Array of holes sorted by size (ordered array) */
  ordered_array_t index;
  /* The start of heap. */
  uint32_t startAddress;
  /* The end of heap. Can be expanded up to maxAddress. */
  uint32_t endAddress;
  /* The maximum address the heap can be expanded to. */
  uint32_t maxAddress;
  /* If set, accesible only in supervisor mode */
  uint8_t supervisor;
  /* Page permission (if set, read only) */
  uint8_t readonly;
} heap_t;

typedef struct {
  uint32_t magic; // Magic number, used for error checking and identification.
  uint8_t isHole; // 1 if this is a hole. 0 if this is a block.
  uint32_t size;  // size of the block, including the end footer.
} header_t;

typedef struct {
  uint32_t magic;     // Magic number, same as in header_t.
  header_t *header; // Pointer to the block header.
} footer_t;

uint32_t g_CurrentPhysicalAddressTop = 0;
heap_t *g_KernelHeap = 0;

/*
 * Defined in paging.c (using extern for the sake of security, so that these are
 * not exposed in header)
 */
extern page_directory_t *g_kernelDirectory;

void set_physical_address_top(uint32_t kernelPhysicalEnd) {
  g_CurrentPhysicalAddressTop = kernelPhysicalEnd;
}

static int32_t find_smallest_hole(uint32_t size, uint8_t pageAlign, heap_t *heap) {
  /* Find the smallest hole that will fit. */
  uint32_t iterator = 0;

  while (iterator < heap->index.size) {
    header_t *header = (header_t *)peek_ordered_array(iterator, &heap->index);

    /* If the user has requested the memory be page-aligned */
    if (pageAlign > 0) {
      /* Page-align the starting point of this header. */
      uint32_t location = (uint32_t)header;
      int32_t offset = 0;

      if (((location + sizeof(header_t)) & 0xFFFFF000) != 0) {
        offset = 0x1000 - ((location + sizeof(header_t)) % 0x1000);
      }

      int32_t holeSize = (int32_t)header->size - offset;

      /* Can we fit now? */
      if (holeSize >= (int32_t)size)
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

static void expand(uint32_t newSize, heap_t *heap) {
  if (newSize <= heap->endAddress - heap->startAddress) {
    print_screen("\nError: Expand: New Size is not greater than old size!!!\n");
    return;
  }

  /* Get the nearest following page boundary. */
  if (newSize & 0x00000FFF) {
    newSize &= 0xFFFFF000;
    newSize += 0x1000;
  }

  /* Make sure we are not overreaching ourselves. */
  if (heap->startAddress + newSize > heap->maxAddress) {
    print_screen("\nError: Expand: New Size is greater than max size!!!\n");
    return;
  }

  uint32_t oldSize = heap->endAddress - heap->startAddress;

  uint32_t index = oldSize;
  /* Allocate frames for new memory area */
  while (index < newSize) {
    alloc_frame(get_page(heap->startAddress + index, 1, g_kernelDirectory),
                (heap->supervisor) ? 1 : 0, (heap->readonly) ? 0 : 1);
    index += 0x1000;
  }
  heap->endAddress = heap->startAddress + newSize;
}

type_t alloc(uint32_t size, uint8_t pageAlign, heap_t *heap) {

  /* Make sure we take the size of header/footer into account. */
  uint32_t newSize = size + sizeof(header_t) + sizeof(footer_t);

  /* Find the smallest hole that will fit. */
  int32_t iterator = find_smallest_hole(newSize, pageAlign, heap);

  /* If we didn't find a suitable hole */
  if (iterator == -1) {
    /* Save previous data. */
    uint32_t oldLength = heap->endAddress - heap->startAddress;
    uint32_t oldEndAddress = heap->endAddress;

    /* We need to allocate some more space. */
    expand(oldLength + newSize, heap);
    uint32_t newLength = heap->endAddress - heap->startAddress;

    /*
     * Since there is not hole available and we have now expanded our heap, we
     * need either add the new hole to list of available holes or create a new
     * entry on sorted array of holes if this is first insertion
     */
    iterator = 0;
    int32_t idx = -1;
    uint32_t value = 0x0;

    uint32_t tmp =
        (uint32_t)peek_ordered_array((heap->index.size - 1), &heap->index);
    if (tmp > value) {
      value = tmp;
      idx = iterator;
    }

    /* If we didn't find ANY headers, we need to add one. */
    if (idx == -1) {
      header_t *header = (header_t *)oldEndAddress;
      header->magic = HEAP_MAGIC;
      header->size = newLength - oldLength;
      header->isHole = 1;
      footer_t *footer =
          (footer_t *)(oldEndAddress + header->size - sizeof(footer_t));
      footer->magic = HEAP_MAGIC;
      footer->header = header;
      insert_ordered_array((type_t)header, &heap->index);
    } else {
      /*
       * This will probably be the last header that has size less han required
       * and hence needs adjusting.
       */
      header_t *header = peek_ordered_array(idx, &heap->index);
      header->size += newLength - oldLength;
      // Rewrite the footer.
      footer_t *footer =
          (footer_t *)((uint32_t)header + header->size - sizeof(footer_t));
      footer->header = header;
      footer->magic = HEAP_MAGIC;
    }
    /*
     * We now have enough space. Recurse, and call the function again, so that
     * in the next iteration we go and do the book keeping things like boundary
     * check and removing the block from holes etc...
     */
    return alloc(size, pageAlign, heap);
  }

  /* If we find the smallest hole that will fit */
  header_t *origHoleHeader =
      (header_t *)peek_ordered_array(iterator, &heap->index);
  uint32_t origHolePos = (uint32_t)origHoleHeader;
  uint32_t origHoleSize = origHoleHeader->size;

  /*
   * Here we work out if we should split the hole we found into two parts.
   * We will skip splitting the hole into two if the original hole size -
   * requested hole size less than the overhead for adding a new hole as we will
   * not have any space left to allocate in the new block.
   */
  if ((origHoleSize - newSize) <= (sizeof(header_t) + sizeof(footer_t))) {
    size += origHoleSize - newSize;
    newSize = origHoleSize;
  }

  /*
   * If we need to page-align the data, do it now and make a new hole before
   * our new block.
   */
  if (pageAlign) {
    uint32_t returnPos = origHolePos + sizeof(header_t);
    if (returnPos & 0x00000FFF) {
      uint32_t newLocation =
          returnPos + 0x1000 - (returnPos & 0xFFF) - sizeof(header_t);

      /* If left over space has at least 1 byte add new hole */
      uint32_t residualSize = newLocation - origHolePos;
      if ((origHoleSize - residualSize) >
          (sizeof(header_t) + sizeof(footer_t))) {
        header_t *holeHeader = (header_t *)origHolePos;
        holeHeader->size = residualSize;
        holeHeader->magic = HEAP_MAGIC;
        holeHeader->isHole = 1;
        footer_t *holeFooter =
            (footer_t *)((uint32_t)newLocation - sizeof(footer_t));
        holeFooter->magic = HEAP_MAGIC;
        holeFooter->header = holeHeader;
        insert_ordered_array((type_t)holeHeader, &heap->index);
      } else {
        /* TODO: Implement logic to expand block on left to avoid
         * fragmentation */
      }

      origHolePos = newLocation;
      origHoleSize = origHoleSize - residualSize;
    }
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
  header_t *blockHeader = (header_t *)origHolePos;
  blockHeader->magic = HEAP_MAGIC;
  blockHeader->isHole = 0;
  blockHeader->size = newSize;
  footer_t *blockFooter = (footer_t *)(origHolePos + sizeof(header_t) + size);
  blockFooter->magic = HEAP_MAGIC;
  blockFooter->header = blockHeader;

  /*
   * We may need to write a new hole after the allocated block.
   * We do this only if the new hole would have positive size...
   */
  if (origHoleSize - newSize > 0) {
    header_t *holeHeader =
        (header_t *)(origHolePos + sizeof(header_t) + size + sizeof(footer_t));
    holeHeader->magic = HEAP_MAGIC;
    holeHeader->isHole = 1;
    holeHeader->size = origHoleSize - newSize;
    footer_t *holeFooter = (footer_t *)((uint32_t)holeHeader + origHoleSize -
                                        newSize - sizeof(footer_t));
    if ((uint32_t)holeFooter < heap->endAddress) {
      holeFooter->magic = HEAP_MAGIC;
      holeFooter->header = holeHeader;
    }
    /*
     * Put the new hole in the index. If the footer is beyond the heap end
     * address we will still add the hole to index so that we can expand the
     * last hole without fragmentation.
     */
    insert_ordered_array((type_t)holeHeader, &heap->index);
  }

  return (type_t)((uint32_t)blockHeader + sizeof(header_t));
}

uint32_t kmalloc_int(uint32_t size, uint32_t align, uint32_t *pAddrPtr) {
  /* if Kernel Heap is created and allocated */
  if (g_KernelHeap != 0) {
    type_t addr = alloc(size, (uint8_t)align, g_KernelHeap);
    if (align && ((uint32_t)addr & 0x00000FFF)) {
      print_screen("\nError: kmalloc_int: returned address from alloc is not "
                   "page aligned!!!!\n");
    }
    if (pAddrPtr != 0) {
      page_t *page = get_page((uint32_t)addr, 0, g_kernelDirectory);

      *pAddrPtr = page->frame * 0x1000;
    }
    return (uint32_t)addr;
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
    /* Increase physical address top so that it is always aligned at 4 bytes */
    g_CurrentPhysicalAddressTop +=
        (g_CurrentPhysicalAddressTop % sizeof(uint32_t));
    uint32_t tmp = g_CurrentPhysicalAddressTop;
    g_CurrentPhysicalAddressTop += size;
    return tmp;
  }
}

uint32_t kmalloc_a(uint32_t size) { return kmalloc_int(size, 1, 0); }

uint32_t kmalloc_p(uint32_t size, uint32_t *pAddrPtr) {
  return kmalloc_int(size, 0, pAddrPtr);
}

uint32_t kmalloc_ap(uint32_t size, uint32_t *pAddrPtr) {
  return kmalloc_int(size, 1, pAddrPtr);
}

uint32_t kmalloc(uint32_t size) { return kmalloc_int(size, 0, 0); }

/*
 * Compare function for ordered array, sorted by header size ascending order
 */
static int8_t header_t_less_than(type_t a, type_t b) {
  return (((header_t *)a)->size < ((header_t *)b)->size) ? 1 : 0;
}

heap_t *create_heap(uint32_t startAddr, uint32_t endAddr, uint32_t maxAddr,
                    uint8_t supervisor, uint8_t readonly) {
  heap_t *heap = (heap_t *)kmalloc(sizeof(heap_t));

  if (startAddr % 0x1000 != 0) {
    print_screen("\nError: create_heap: Start Address is not page aligned\n");
    return 0;
  }

  if (endAddr % 0x1000 != 0) {
    print_screen("\nError: create_heap: End Address is not page aligned\n");
    return 0;
  }

  /* Initialise the index. */
  heap->index = place_ordered_array((type_t)startAddr, HEAP_INDEX_SIZE,
                                    (compare_predicate_t)&header_t_less_than);

  /*
   * Shift the start address forward to resemble where we can start putting
   * data.
   */
  startAddr += sizeof(type_t) * HEAP_INDEX_SIZE;

  /* Make sure the start address is page-aligned. */
  if (startAddr & 0x00000FFF) {
    startAddr &= 0xFFFFF000;
    startAddr += 0x1000;
  }

  /* Write the start, end and max addresses into the heap structure. */
  heap->startAddress = startAddr;
  heap->endAddress = endAddr;
  heap->maxAddress = maxAddr;
  heap->supervisor = supervisor;
  heap->readonly = readonly;

  /* We start off with one large hole in the index. */
  header_t *hole = (header_t *)startAddr;
  hole->size = endAddr - startAddr;
  hole->magic = HEAP_MAGIC;
  hole->isHole = 1;
  insert_ordered_array((type_t)hole, &heap->index);

  return heap;
}

void create_kernel_heap(uint32_t startAddr, uint32_t endAddr, uint32_t maxAddr) {
  g_KernelHeap = create_heap(startAddr, endAddr, maxAddr, 0, 0);
}

static uint32_t contract(uint32_t newSize, heap_t *heap) {
  if (newSize >= (heap->endAddress - heap->startAddress)) {
    print_screen("\nError: contract: New size greater than current heap size");
    return (heap->endAddress - heap->startAddress);
  }

  /* Page align */
  if (newSize & 0x0FFF) {
    newSize &= 0x1000;
    newSize += 0x1000;
  }

  /* Don't contract too far! */
  if (newSize < HEAP_MIN_SIZE)
    newSize = HEAP_MIN_SIZE;

  uint32_t oldSize = heap->endAddress - heap->startAddress;
  uint32_t index = oldSize - 0x1000;
  while (newSize < index) {
    free_frame(get_page(heap->startAddress + index, 0, g_kernelDirectory));
    index -= 0x1000;
  }

  heap->endAddress = heap->startAddress + newSize;
  return newSize;
}

static void free(void *ptr, heap_t *heap) {
  if (ptr == 0) {
    return;
  }

  header_t *header = (header_t *)((uint32_t)ptr - sizeof(header_t));
  footer_t *footer =
      (footer_t *)((uint32_t)header + header->size - sizeof(footer_t));

  if (header->magic != HEAP_MAGIC) {
    print_screen("\nError: free: Cannot verify header magic numer");
  }

  if (footer->magic != HEAP_MAGIC) {
    print_screen("\nError: free: Cannot verify footer magic number");
  }

  /* Make us a hole */
  header->isHole = 1;

  /* We want to add this header into the 'free holes' index */
  char doAdd = 1;

  /*
   * Unify left if the thing immediately to the left of us is a footer of free
   * block
   */
  footer_t *testFooter = (footer_t *)((uint32_t)header - sizeof(footer_t));
  if (testFooter->magic == HEAP_MAGIC && testFooter->header->isHole == 1) {
    uint32_t cacheSize = header->size;
    header = testFooter->header;
    footer->header = header;
    header->size += cacheSize;
    /* Since this header is already in the index, we don't want to add */
    doAdd = 0;
  }

  /*
   * Unify right if the thing immediately to the right of us is a header of a
   * free block
   */
  header_t *testHeader = (header_t *)((uint32_t)footer + sizeof(footer_t));
  if (testHeader->magic == HEAP_MAGIC && testHeader->isHole) {
    header->size += testHeader->size;
    testFooter =
        (footer_t *)((uint32_t)testHeader + testHeader->size - sizeof(footer_t));
    footer = testFooter;
    /*  Find and remove this header from the index. */
    uint32_t iterator = 0;
    while ((iterator < heap->index.size) &&
           (peek_ordered_array(iterator, &heap->index) != (type_t)testHeader)) {
      iterator++;
    }

    if (iterator >= heap->index.size) {
      print_screen("\nError: free: unify right found a free block that is not "
                   "present in index");
      return;
    }

    remove_ordered_array(iterator, &heap->index);
  }

  /*
   * If the footer location is the end address, we can contract if we have
   * expanded our heap beyond minimum heap size.
   */
  if ((uint32_t)footer + sizeof(footer_t) == heap->endAddress) {
    uint32_t oldLength = heap->endAddress - heap->startAddress;
    uint32_t newLength = contract((uint32_t)header - heap->startAddress, heap);

    /*
     * Check how big we will be after resizing.
     * It might be possible we still have the header but footer stays outside
     * heap end address since we contracted
     */
    if (header->size - (oldLength - newLength) > 0) {
      header->size -= oldLength - newLength;
      footer = (footer_t *)((uint32_t)header + header->size - sizeof(footer_t));
      footer->magic = HEAP_MAGIC;
      footer->header = header;
    } else {
      /* Remove temporary header before contract from the index. */
      uint32_t iterator = 0;
      while (
          (iterator < heap->index.size) &&
          (peek_ordered_array(iterator, &heap->index) != (type_t)testHeader)) {
        iterator++;
      }

      if (iterator >= heap->index.size) {
        print_screen("\nError: free: contract found a free block that is not "
                     "present in index");
        return;
      }

      if (iterator < heap->index.size) {
        remove_ordered_array(iterator, &heap->index);
      }
    }
  }

  /* If required, add us to the index. */
  if (doAdd == 1)
    insert_ordered_array((void *)header, &heap->index);
}

void kfree(void *ptr) { free(ptr, g_KernelHeap); }
