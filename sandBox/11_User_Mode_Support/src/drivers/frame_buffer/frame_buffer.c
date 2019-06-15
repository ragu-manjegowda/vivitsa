#include "frame_buffer.h"
#include <io.h>

static int8_t *fb = (int8_t *)FB_BASE_ADDRESS;
static uint16_t CURSOR_POS = 0;
static uint16_t CURSOR_POS_MAX = 2000;
static uint16_t BACKGROUND_COLOR = FB_GREEN;
static uint16_t FOREGROUND_COLOR = FB_DARK_GREY;

/* fb_move_cursor:
 *  Moves the cursor of the framebuffer to the CURSOR_POS global variable
 */
void fb_move_cursor() {
  outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
  outb(FB_DATA_PORT, ((CURSOR_POS >> 8) & 0x00FF));
  outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
  outb(FB_DATA_PORT, CURSOR_POS & 0x00FF);
}

/* fb_clear:
 *  Clear the contents on screen between start position and end position
 *
 *  @param start Starting position
 *  @param end End position
 */

void fb_clear(uint16_t start, uint16_t end) {
  for (uint16_t i = start; i < end; i++) {
    fb[2 * i] = ' ';
    fb[2 * i + 1] = ((FB_BLACK & 0x0F) << 4) | (FB_WHITE & 0x0F);
  }
}

void fb_clear_all() {
  fb_clear(0, CURSOR_POS_MAX);
  CURSOR_POS = 0;
  fb_move_cursor();
}

/* fb_write_cell:
 *  Writes a character with the given foreground and background to position i
 *  in the framebuffer.
 *
 *  @param i  The location in the framebuffer
 *  @param c  The character
 *  @param fg The foreground color
 *  @param bg The background color
 */
void fb_write_cell(int8_t c, uint8_t fg, uint8_t bg) {
  if (c == '\n') {
    uint16_t cursor_temp_pos = CURSOR_POS;
    CURSOR_POS = ((CURSOR_POS / 79) + 1) * 80;
    if (CURSOR_POS >= CURSOR_POS_MAX) {
      fb_clear(0, CURSOR_POS_MAX);
      CURSOR_POS = 0;
    } else {
      fb_clear(cursor_temp_pos, CURSOR_POS);
    }
  } else if (c != '\0') {
    if (CURSOR_POS >= CURSOR_POS_MAX) {
      fb_clear(0, CURSOR_POS_MAX);
      CURSOR_POS = 0;
    }
    fb[2 * CURSOR_POS] = c;
    fb[2 * CURSOR_POS + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
    CURSOR_POS++;
  }
}

int32_t fb_write(int8_t *buf, uint32_t len) {
  uint32_t index_to_buffer = 0;
  while (index_to_buffer < len) {
    fb_write_cell(buf[index_to_buffer], BACKGROUND_COLOR, FOREGROUND_COLOR);
    fb_move_cursor();
    index_to_buffer++;
  }
  return 0;
}

void fb_set_color(uint16_t background, uint16_t foreground) {
  BACKGROUND_COLOR = background;
  FOREGROUND_COLOR = foreground;
}
