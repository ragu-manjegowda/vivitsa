#include "logger.h"
#include <frame_buffer.h>
#include <helpers.h>
#include <serial_port.h>
#include <timer.h>

char message[] = "\
              __      _________      _______ _______ _____                      \
              \\ \\    / /_   _\\ \\    / /_   _|__   __/ ____|  /\\                 \
               \\ \\  / /  | |  \\ \\  / /  | |    | | | (___   /  \\                \
                \\ \\/ /   | |   \\ \\/ /   | |    | |  \\___ \\ / /\\ \\               \
                 \\  /   _| |_   \\  /   _| |_   | |  ____) / ____ \\              \
                  \\/   |_____|   \\/   |_____|  |_| |_____/_/    \\_\\            \
                                                                                ";


char message2[] = "\
     _____           _ _           _           _   _                            \
    |  __ \\         | (_)         | |         | | | |                           \
    | |  | | ___  __| |_  ___ __ _| |_ ___  __| | | |_ ___    _ __ ___  _   _   \
    | |  | |/ _ \\/ _` | |/ __/ _` | __/ _ \\/ _` | | __/ _ \\  | '_ ` _ \\| | | |  \
    | |__| |  __/ (_| | | (_| (_| | ||  __/ (_| | | || (_) | | | | | | | |_| |  \
    |_____/ \\___|\\__,_|_|\\___\\__,_|\\__\\___|\\__,_|  \\__\\___/  |_| |_| |_|\\__, |  \
              _          _                    _            _  __         __/ |  \
             | |        | |                  | |          (_)/ _|       |___/   \
             | |__   ___| | _____   _____  __| | __      ___| |_ ___            \
             | '_ \\ / _ \\ |/ _ \\ \\ / / _ \\/ _` | \\ \\ /\\ / / |  _/ _ \\           \
             | |_) |  __/ | (_) \\ V /  __/ (_| |  \\ V  V /| | ||  __/           \
             |_.__/ \\___|_|\\___/ \\_/ \\___|\\__,_|   \\_/\\_/ |_|_| \\___|           ";


char message3[] = "\
                          _____ _           _                                   \
                         / ____(_)         | |                                  \
                        | (___  _ _ __   __| |_   _                             \
                         \\___ \\| | '_ \\ / _` | | | |                            \
                         ____) | | | | | (_| | |_| |                            \
                        |_____/|_|_| |_|\\__,_|\\__,_|                          \
                                                                                ";

void print_screen(s8int buffer[]) { fb_write(buffer, strlen(buffer)); }

void print_serial(s8int buffer[]) {
  serial_write(SERIAL_COM1_BASE, buffer, strlen(buffer));
}

void print_screen_ch(s8int ch) { fb_write(&ch, 1); }

void print_serial_ch(s8int ch) { serial_write(SERIAL_COM1_BASE, &ch, 1); }

/** fb_splash:
 *  Reset screen and display splash screen
 */
void init_display() {
  fb_clear_all();
  fb_set_color(FB_RED, FB_WHITE);
  fb_write(message, 577);
  fb_set_color(FB_RED, FB_BLACK);
  fb_write(message2, 960);
  fb_set_color(FB_RED, FB_WHITE);
  fb_write(message3, 480);
  /* Sleep for 5 seconds (500 centiSeconds) */
  sleep(500);
  fb_set_color(FB_BLACK, FB_WHITE);
}
