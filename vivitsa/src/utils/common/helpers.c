#include "helpers.h"
#include "multiboot.h"

/* global variable to store pointer to multiboot info */
uint32_t g_MULTIBOOT_PTR;

int8_t g_BUFFER[LEN_10];

int8_t *integer_to_string(uint32_t number) {
  uint32_t i = 0;
  while (i < LEN_10) {
    g_BUFFER[i] = '\0';
    i++;
  }

  if (number == 0) {
    g_BUFFER[0] = '0';
    return g_BUFFER;
  }

  uint32_t temp_number = number;
  int8_t buffer_rev[10] = "";
  i = 0;
  while (temp_number > 0) {
    buffer_rev[i] = '0' + (temp_number % 10);
    temp_number /= 10;
    i++;
  }

  uint32_t j = 0;
  while (i > 1) {
    g_BUFFER[--i] = buffer_rev[j++];
  }

  g_BUFFER[--i] = buffer_rev[j];

  return g_BUFFER;
}

void get_multiboot_info(uint32_t mboot_ptr, uint32_t *initrdPhysicalStart,
                        uint32_t *multibootPhysicalEnd, uint32_t *modsCount) {
  g_MULTIBOOT_PTR = mboot_ptr;
  multiboot_info_t *mbinfo = (multiboot_info_t *)mboot_ptr;
  multiboot_module_t *mod = (multiboot_module_t *)mbinfo->mods_addr;

  *initrdPhysicalStart = (uint32_t)((uint32_t *)mod->mod_start);
  *multibootPhysicalEnd =
      (uint32_t)((uint32_t *)mod[mbinfo->mods_count - 1].mod_end);
  *modsCount = mbinfo->mods_count;
}

uint32_t get_multiboot_address() { return g_MULTIBOOT_PTR; }

uint32_t custom_strlen(const int8_t *str) {
  uint32_t i;
  for (i = 0; str[i]; i++)
    ;

  return i;
}

void custom_memset(uint8_t *address, uint32_t val, uint32_t size) {
  for (uint32_t i = 0; i < size; ++i) {
    *address = val;
    ++address;
  }
}

void custom_memcpy(uint8_t *destination, const uint8_t *source, uint32_t size) {
  uint8_t *pdest = (uint8_t *)destination;
  uint8_t *psrc = (uint8_t *)source;
  uint32_t loops = (size / sizeof(uint32_t));
  for (uint32_t index = 0; index < loops; ++index) {
    *((uint32_t *)pdest) = *((uint32_t *)psrc);
    pdest += sizeof(uint32_t);
    psrc += sizeof(uint32_t);
  }
  loops = (size % sizeof(uint32_t));
  for (uint32_t index = 0; index < loops; ++index) {
    *pdest = *psrc;
    ++pdest;
    ++psrc;
  }
}

void custom_strcpy(int8_t *destination, const int8_t *source) {
  uint32_t i = 0;
  for (i = 0; source[i] != '\0'; ++i) {
    destination[i] = source[i];
  }
  destination[i] = '\0';
}

uint8_t custom_strcmp(const int8_t *stringFirst, const int8_t *stringSecond,
                    uint8_t ignoreCase) {
  uint32_t i = 0;
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
