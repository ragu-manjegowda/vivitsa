#include "frame_buffer.h"
#include "logger.h"
#include "serial_port.h"

void kprint(char *buffer, unsigned int len) {
  fb_write(buffer, len);
}

void log(char *buffer, unsigned int len) {
  serial_write(SERIAL_COM1_BASE, buffer, len);
}
