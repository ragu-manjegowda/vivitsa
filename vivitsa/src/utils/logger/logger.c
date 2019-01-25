#include "logger.h"
#include "frame_buffer.h"
#include "serial_port.h"

void print_screen(s8int *buffer, u32int len) { fb_write(buffer, len); }

void print_serial(s8int *buffer, u32int len) {
  serial_write(SERIAL_COM1_BASE, buffer, len);
}
