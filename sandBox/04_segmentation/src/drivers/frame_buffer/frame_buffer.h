#ifndef INCLUDE_FRAMEBUFFER_H
#define INCLUDE_FRAMEBUFFER_H

/* The I/O ports */
#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT 0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND 15

/* Frame buffer supported color value */
#define FB_GREEN 2
#define FB_DARK_GREY 8

/* Frame buffer base address */
#define FB_BASE_ADDRESS 0xB8000

/** write:
 *  writes the contents of the buffer buf of length len to the screen
 *
 *  @param buf  Buffer that has contents to be written to screen
 *  @param len  Length of buffer
 */
int fb_write(char *buf, unsigned int len);

#endif /* INCLUDE_FRAMEBUFFER_H */
