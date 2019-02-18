#include "logger.h"
#include "frame_buffer.h"
#include "serial_port.h"

u32int strlen(const char *str)
{
  u32int i;
  for (i = 0; str[i]; i++);

  return i;
}

void print_screen(s8int buffer[]) {
  fb_write(buffer, strlen(buffer));
}

void print_serial(s8int buffer[]) {
  serial_write(SERIAL_COM1_BASE, buffer, strlen(buffer));
}
