#include "kheap.h"

static u32int g_CurrentVirtualAddressTop = 0;

void set_virtual_address_top(u32int virtualAddress) {
  g_CurrentVirtualAddressTop = (u32int)&virtualAddress;
}

u32int kmalloc_int(u32int size, int align, u32int *pAddrPtr) {
  if (align == 1) {
    if (g_CurrentVirtualAddressTop & 0x00000FFF) {
      // Align the placement address;
      g_CurrentVirtualAddressTop &= 0xFFFFF000;
      g_CurrentVirtualAddressTop += 0x1000;
    }
  }
  if (pAddrPtr) {
    *pAddrPtr = g_CurrentVirtualAddressTop;
  }
  u32int tmp = g_CurrentVirtualAddressTop;
  g_CurrentVirtualAddressTop += size;
  return tmp;
}

u32int kmalloc_a(u32int size) { return kmalloc_int(size, 1, 0); }

u32int kmalloc_p(u32int size, u32int *pAddrPtr) {
  return kmalloc_int(size, 0, pAddrPtr);
}

u32int kmalloc_ap(u32int size, u32int *pAddrPtr) {
  return kmalloc_int(size, 1, pAddrPtr);
}

u32int kmalloc(u32int size) { return kmalloc_int(size, 0, 0); }
