#ifndef INCLUDE_FRAMEBUFFER_H
#define INCLUDE_FRAMEBUFFER_H

#pragma once
#include <types.h>

/* The I/O ports */
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

/* Frame buffer base address */
#define FB_BASE_ADDRESS 0xC00B8000

/* fb_write:
 *  writes the contents of the buffer buf of length len to the screen
 *
 *  @param buf  Buffer that has contents to be written to screen
 *  @param len  Length of buffer
 */
s32int fb_write(s8int *buf, u32int len);

/* fb_clear_all:
 *  Clear all the contents on screen
 */
void fb_clear_all();

/* fb_set_color:
 *  Sets the color of text to be displayed on screen
 *
 *  @param background Background color
 *  @param foreground Foreground color
 */
void fb_set_color(u16int background, u16int foreground);

#endif /* INCLUDE_FRAMEBUFFER_H */
