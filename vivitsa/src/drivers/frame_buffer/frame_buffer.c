#include "frame_buffer.h"
#include <io.h>

static s8int *fb = (s8int *)FB_BASE_ADDRESS;
static u16int CURSOR_POS = 0;
static u16int CURSOR_POS_MAX = 2000;
static u16int BACKGROUND_COLOR = FB_GREEN;
static u16int FOREGROUND_COLOR = FB_DARK_GREY;

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

void fb_clear(u16int start, u16int end) {
  for (u16int i = start; i < end; i++) {
    fb[2 * i] = ' ';
    fb[2 * i + 1] = ((0 & 0x0F) << 4) | (15 & 0x0F);
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
void fb_write_cell(s8int c, u8int fg, u8int bg) {
  if (c == '\n') {
    u16int cursor_temp_pos = CURSOR_POS;
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

s32int fb_write(s8int *buf, u32int len) {
  u32int index_to_buffer = 0;
  while (index_to_buffer < len) {
    fb_write_cell(buf[index_to_buffer], BACKGROUND_COLOR, FOREGROUND_COLOR);
    fb_move_cursor();
    index_to_buffer++;
  }
  return 0;
}

void fb_set_color(u16int background, u16int foreground) {
  BACKGROUND_COLOR = background;
  FOREGROUND_COLOR = foreground;
}
