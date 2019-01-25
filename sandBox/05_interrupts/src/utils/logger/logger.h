#ifndef INCLUDE_LOGGER_H
#define INCLUDE_LOGGER_H

#pragma once
#include <types.h>

/** log:
 *  writes the char array buffer of length len to serial console
 *
 *  @param buffer  Buffer that has contents to be written to serial port
 *  @param len  Length of buffer
 */
void print_serial(s8int *buffer, u32int len);

/** kprint:
 *  writes the char buffer buf of length len to Screen
 *
 *  @param buffer  Buffer that has contents to be written to Screen
 *  @param len  Length of buffer
 */
void print_screen(s8int *buffer, u32int len);

#endif /* INCLUDE_LOGGER_H */
