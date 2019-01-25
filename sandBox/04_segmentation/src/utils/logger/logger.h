#ifndef INCLUDE_LOGGER_H
#define INCLUDE_LOGGER_H

/** log:
 *  writes the contents of the buffer buf of length len to serial console
 *
 *  @param buffer  Buffer that has contents to be written to serial port
 *  @param len  Length of buffer
 */
void log(char *buffer, unsigned int len);

/** kprint:
 *  writes the contents of the buffer buf of length len to Screen
 *
 *  @param buffer  Buffer that has contents to be written to Screen
 *  @param len  Length of buffer
 */
void kprint(char *buffer, unsigned int len);

#endif /* INCLUDE_LOGGER_H */
