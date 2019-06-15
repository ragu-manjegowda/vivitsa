#ifndef INCLUDE_TYPES_H
#define INCLUDE_TYPES_H

/* Typedefs, to standardise sizes across platforms.
 * These typedefs are written for 32-bit X86.
 */
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned char uint8_t;
typedef char int8_t;

/* Frame buffer supported color value */
#define FB_BLACK 0
#define FB_BLUE 1
#define FB_GREEN 2
#define FB_CYAN 3
#define FB_RED 4
#define FB_MAGENTA 5
#define FB_BROWN 6
#define FB_LIGHT_GREY 7
#define FB_DARK_GREY 8
#define FB_LIGHT_BLUE 9
#define FB_LIGHT_GREEN 10
#define FB_LIGHT_CYAN 11
#define FB_LIGHT_RED 12
#define FB_LIGHT_MAGENTA 13
#define FB_LIGHT_BROWN 14
#define FB_WHITE 15

/* Keyboard driver constants */
#define KEYBOARD_DATA_PORT 0x60
#define PRESSED_CAPS_LOCK 58
#define PRESSED_SHIFT_LEFT 42
#define PRESSED_SHIFT_RIGHT 54
#define RELEASED_SHIFT_LEFT 170
#define RELEASED_SHIFT_RIGHT 182

/* Length constants */
enum Size {
  LEN_7 = 7,
  LEN_10 = 10,
  LEN_32 = 32,
  LEN_64 = 64,
  LEN_256 = 256,
  LEN_512 = 512
};

enum memSize { MEM_4KB = 0x1000, MEM_3_5GB = 0xE0000000 };

enum mask { MASK_BIT8 = 0x80 };

#endif /* INCLUDE_TYPES_H */
