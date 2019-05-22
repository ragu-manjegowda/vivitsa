#include "helpers.h"

#define STRING_LEN 10

char buffer[STRING_LEN];

s8int *integer_to_string(u32int number) {
  u32int i = 0;
  while (i < STRING_LEN) {
    buffer[i] = '\0';
    i++;
  }

  if (number == 0) {
    buffer[1] = '0';
    return buffer;
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
    buffer[--i] = buffer_rev[j++];
  }

  buffer[--i] = buffer_rev[j];

  return buffer;
}

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

void custom_memcpy(u8int *destination, u8int *source, u32int size) {
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

void custom_strcpy(s8int *destination, s8int *source) {
  u32int i = 0;
  for (i = 0; source[i] != '\0'; ++i) {
    destination[i] = source[i];
  }
  destination[i] = '\0';
}

u8int custom_strcmp(s8int *stringFirst, s8int *stringSecond, u8int ignoreCase) {
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
