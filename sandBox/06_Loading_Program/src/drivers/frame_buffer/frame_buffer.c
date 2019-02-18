#include "frame_buffer.h"
#include "io.h"

static s8int *fb = (s8int *)FB_BASE_ADDRESS;
static u16int cursor_pos = 0;

/** fb_move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 *
 *  @param pos The new position of the cursor
 */
void fb_move_cursor() {
  outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
  outb(FB_DATA_PORT, ((cursor_pos >> 8) & 0x00FF));
  outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
  outb(FB_DATA_PORT, cursor_pos & 0x00FF);
}

void fb_clear(u16int start, u16int end) {
  for (u16int i = start; i < end; i++) {
    fb[2 * i] = ' ';
    fb[2 * i + 1] = ((0 & 0x0F) << 4) | (15 & 0x0F);
  }
}

/** fb_write_cell:
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
    u16int cursor_temp_pos = cursor_pos;
    cursor_pos = ((cursor_pos / 79) + 1) * 80;
    if (cursor_pos >= 2000) {
      fb_clear(0, 2000);
      cursor_pos = 0;
    } else {
      fb_clear(cursor_temp_pos, cursor_pos);
    }
  } else if (c != '\0') {
    fb[2 * cursor_pos] = c;
    fb[2 * cursor_pos + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);

    cursor_pos++;
    if (cursor_pos >= 2000) {
      fb_clear(0, 2000);
      cursor_pos = 0;
    }
  }
}

s32int fb_write(s8int *buf, u32int len) {
  u32int index_to_buffer = 0;
  while (index_to_buffer < len) {
    fb_write_cell(buf[index_to_buffer], FB_GREEN, FB_DARK_GREY);
    fb_move_cursor();
    index_to_buffer++;
  }
  return 0;
}
