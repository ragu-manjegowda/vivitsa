#include "serial_port.h"
#include "io.h"

void serial_configure_baud_rate(uint16_t com, uint16_t divisor) {
  /* Tell the serial port to first expect the highest 8 bits, then the lowest
   * 8 bits. This is done by sending 0x80 to the line command port
   */
  outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
  outb(SERIAL_DATA_PORT(com), (divisor >> 8) & 0x00FF);
  outb(SERIAL_DATA_PORT(com), divisor & 0x00FF);
}

void serial_configure_line(uint16_t com) {
  /* Bit:     | 7 | 6 | 5 4 3 | 2 | 1 0 |
   * Content: | d | b | prty  | s | dl  |
   * Value:   | 0 | 0 | 0 0 0 | 0 | 1 1 | = 0x03
   * data length of 8 bits, one stop bit, no parity bit, break control
   * disabled and DLAB disabled
   */
  outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
}

void serial_configure_fifo_buffer(uint16_t com) {
  /* Bit:     | 7 6 | 5  | 4 | 3   | 2   | 1   | 0 |
   * Content: | lvl | bs | r | dma | clt | clr | e |
   * Value:   | 1 1 | 0  | 0 | 0   | 1   | 1   | 1 | = 0xC7
   */
  outb(SERIAL_FIFO_COMMAND_PORT(com), 0xC7);
}

void serial_configure_modem(uint16_t com) {
  /* Bit:     | 7 | 6 | 5  | 4  | 3   | 2   | 1   | 0   |
   * Content: | r | r | af | lb | ao2 | ao1 | rts | dtr |
   * Value:   | 0 | 0 | 0  | 0  | 0   | 0   | 1   | 1 | = 0x03
   */
  outb(SERIAL_MODEM_COMMAND_PORT(com), 0x03);
}

int32_t serial_is_transmit_fifo_empty(uint16_t com) {
  /* 0x20 = 0010 0000 */
  return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

/** serial_write:
 *  writes the contents of the buffer buf of length len to the screen, since the
 * serial port FIFO queue can hold 14 bytes, we will check if transmit buffer is
 * empty only if the index is multiple of 8, by doing this we are avoiding
 * uneccesary spin in checking fifo empty
 *
 *  @param buf  Buffer that has contents to be written to screen
 *  @param len  Length of buffer
 */
int32_t serial_write(uint16_t com, int8_t *buf, uint32_t len) {
  uint32_t indexToBuffer = 0, count = 0;
  while (indexToBuffer < len) {
    if (indexToBuffer == count) {
      while (!serial_is_transmit_fifo_empty(com))
        ;
      count += SERIAL_FIFO_BUFFER_LENGTH;
    }
    serial_write_byte(com, buf[indexToBuffer]);
    indexToBuffer++;
  }
  return 0;
}

void serial_write_byte(uint16_t port, int8_t byteData) { outb(port, byteData); }

void serial_configure(uint16_t port, uint16_t baudRate) {
  serial_configure_baud_rate(port, baudRate);
  serial_configure_line(port);
  serial_configure_fifo_buffer(port);
  serial_configure_modem(port);
}
