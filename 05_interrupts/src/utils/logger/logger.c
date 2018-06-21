#include "logger.h"
#include "frame_buffer.h"
#include "serial_port.h"

void print_screen(char *buffer, unsigned int len) { fb_write(buffer, len); }

void print_serial(char *buffer, unsigned int len) {
  serial_write(SERIAL_COM1_BASE, buffer, len);
}
