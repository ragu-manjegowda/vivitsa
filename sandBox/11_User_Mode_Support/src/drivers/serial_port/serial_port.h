#ifndef INCLUDE_SERIALPORT_H
#define INCLUDE_SERIALPORT_H

#pragma once
#include <types.h>

/* The I/O ports */

/* All the I/O ports are calculated relative to the data port. This is because
 * all serial ports (COM1, COM2, COM3, COM4) have their ports in the same
 * order, but they start at different values.
 */

#define SERIAL_COM1_BASE 0x3F8 /* COM1 base port */

#define SERIAL_DATA_PORT(base) (base)
#define SERIAL_FIFO_COMMAND_PORT(base) (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base) (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base) (base + 5)

/* Divisor for different baud rates */
enum BaudRate { Baud_115200 = 1, Baud_57600, Baud_19200, Baud_9600 };

/* The I/O port commands */

/* SERIAL_LINE_ENABLE_DLAB:
 * Tells the serial port to expect first the highest 8 bits on the data port,
 * then the lowest 8 bits will follow
 */
#define SERIAL_LINE_ENABLE_DLAB 0x80

/* SERIAL_FIFO_BUFFER_LENGTH:
 * Define the macro for FIFO queue length
 */
#define SERIAL_FIFO_BUFFER_LENGTH 14

/** serial_configure_baud_rate:
 *  Sets the speed of the data being sent. The default speed of a serial
 *  port is 115200 bits/s. The argument is a divisor of that number, hence
 *  the resulting speed becomes (115200 / divisor) bits/s.
 *
 *  @param com      The COM port to configure
 *  @param divisor  The divisor
 */
void serial_configure_baud_rate(uint16_t com, uint16_t divisor);

/** serial_configure_line:
 *  Configures the line of the given serial port. The port is set to have a
 *  data length of 8 bits, no parity bits, one stop bit and break control
 *  disabled.
 *
 *  @param com  The serial port to configure
 */
void serial_configure_line(uint16_t com);

/** serial_configure_fifo_buffer:
 * 14 bytes as size of queue, clear both receiver and transmission FIFO
 * queues, DMA bit ignored, r is reserved bit, bs is 16 byte FIFO, 14 bytes
 * stored in FIFO which is lvl 11
 *
 *  @param com  The serial port to configure
 */
void serial_configure_fifo_buffer(uint16_t com);

/** serial_configure_modem:
 *  The modem control register is used for very simple hardware flow control via
 * the Ready To Transmit (RTS) and Data Terminal Ready (DTR) pins. When
 * configuring the serial port we want RTS and DTR to be 1, which means that we
 * are ready to send data.
 *
 *  @param com  The serial port to configure
 */
void serial_configure_modem(uint16_t com);

/** serial_is_transmit_fifo_empty:
 *  Checks whether the transmit FIFO queue is empty or not for the given COM
 *  port.
 *
 *  @param  com The COM port
 *  @return 0 if the transmit FIFO queue is not empty
 *          1 if the transmit FIFO queue is empty
 */
int32_t serial_is_transmit_fifo_empty(uint16_t com);

/** serial_write:
 *  writes the contents of the buffer buf of length len to the screen
 *
 *  @param buf  Buffer that has contents to be written to screen
 *  @param len  Length of buffer
 */
int32_t serial_write(uint16_t com, int8_t *buf, uint32_t len);

/** serial_write_byte:
 *  Write byte data to given serial port
 *
 *  @param port Serial port to which data has to be written
 * @param byte_data 8 bit data
 */
void serial_write_byte(uint16_t port, int8_t byteData);

/** serial_configure:
 *  Configure serial port
 *
 *  @param port Serial port which needs to be configured
 * @param baudRate rate at which data needs to be transmitted
 */
void serial_configure(uint16_t port, uint16_t baudRate);

#endif /* INCLUDE_SERIALPORT_H */
