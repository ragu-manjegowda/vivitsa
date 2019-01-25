#include "helpers.h"

void integer_to_string(s8int *buffer, u32int number) {
  if (number == 0) {
    buffer[1] = '0';
    return;
  }

  u32int temp_number = number;
  s8int buffer_rev[10] = " ";
  u32int i = 0;
  while (temp_number > 0) {
    buffer_rev[i] = '0' + (temp_number % 10);
    temp_number /= 10;
    i++;
  }

  u32int j = 0;
  while (i > 0) {
    buffer[i--] = buffer_rev[j++];
  }
  buffer[i] = buffer_rev[j];
}
