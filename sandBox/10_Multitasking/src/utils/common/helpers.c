#include "helpers.h"
#include "multiboot.h"

/* global variable to store pointer to multiboot info */
u32int g_MULTIBOOT_PTR;

s8int g_BUFFER[LEN_10];

s8int *integer_to_string(u32int number) {
  u32int i = 0;
  while (i < LEN_10) {
    g_BUFFER[i] = '\0';
    i++;
  }

  if (number == 0) {
    g_BUFFER[0] = '0';
    return g_BUFFER;
  }

  u32int temp_number = number;
  s8int buffer_rev[10] = "";
  i = 0;
  while (temp_number > 0) {
    buffer_rev[i] = '0' + (temp_number % 10);
    temp_number /= 10;
    i++;
  }

  u32int j = 0;
  while (i > 1) {
    g_BUFFER[--i] = buffer_rev[j++];
  }

  g_BUFFER[--i] = buffer_rev[j];

  return g_BUFFER;
}

void get_multiboot_info(u32int mboot_ptr, u32int *initrdPhysicalStart,
                        u32int *multibootPhysicalEnd, u32int *modsCount) {
  g_MULTIBOOT_PTR = mboot_ptr;
  multiboot_info_t *mbinfo = (multiboot_info_t *)mboot_ptr;
  multiboot_module_t *mod = (multiboot_module_t *)mbinfo->mods_addr;

  *initrdPhysicalStart = (u32int)((u32int *)mod->mod_start);
  *multibootPhysicalEnd =
      (u32int)((u32int *)mod[mbinfo->mods_count - 1].mod_end);
  *modsCount = mbinfo->mods_count;
}

u32int get_multiboot_address() { return g_MULTIBOOT_PTR; }

u32int custom_strlen(const s8int *str) {
  u32int i;
  for (i = 0; str[i]; i++)
    ;

  return i;
}

void custom_memset(u8int *address, u32int val, u32int size) {
  for (u32int i = 0; i < size; ++i) {
    *address = val;
    ++address;
  }
}

void custom_memcpy(u8int *destination, const u8int *source, u32int size) {
  u8int *pdest = (u8int *)destination;
  u8int *psrc = (u8int *)source;
  u32int loops = (size / sizeof(u32int));
  for (u32int index = 0; index < loops; ++index) {
    *((u32int *)pdest) = *((u32int *)psrc);
    pdest += sizeof(u32int);
    psrc += sizeof(u32int);
  }
  loops = (size % sizeof(u32int));
  for (u32int index = 0; index < loops; ++index) {
    *pdest = *psrc;
    ++pdest;
    ++psrc;
  }
}

void custom_strcpy(s8int *destination, const s8int *source) {
  u32int i = 0;
  for (i = 0; source[i] != '\0'; ++i) {
    destination[i] = source[i];
  }
  destination[i] = '\0';
}

u8int custom_strcmp(const s8int *stringFirst, const s8int *stringSecond,
                    u8int ignoreCase) {
  u32int i = 0;
  for (i = 0; stringFirst[i] && stringSecond[i]; ++i) {
    if (ignoreCase) {
      /* If characters are same or inverting the 6th bit (inverting case) makes
       * them same
       */
      if (stringFirst[i] == stringSecond[i] ||
          (stringFirst[i] ^ LEN_32) == stringSecond[i])
        continue;
      else
        break;
    } else {
      if (stringFirst[i] == stringSecond[i])
        continue;
      else
        break;
    }
  }

  /* Compare the last (or first mismatching in case of not same) characters */
  if (stringFirst[i] == stringSecond[i])
    return 0;

  return 1;
}
