#include "kb.h"
#include <io.h>
#include <isr.h>
#include <logger.h>

#define IRQ1 33

/*
 *  uskbd means US Keyboard Layout. This is a scancode table
 *  used to layout a standard US keyboard.
 */
int8_t uskbd[LEN_256] = {
    0,    27,                                            /* Escape */
    '1',  '2', '3', '4', '5',  '6', '7', '8',            /* 9 */
    '9',  '0', '-', '=', '\b',                           /* Backspace */
    '\t',                                                /* Tab */
    'q',  'w', 'e', 'r',                                 /* 19 */
    't',  'y', 'u', 'i', 'o',  'p', '[', ']', '\n',      /* Enter key */
    0,                                                   /* Control key */
    'a',  's', 'd', 'f', 'g',  'h', 'j', 'k', 'l',  ';', /* 39 */
    '\'', '`', 0,                                        /* Left shift */
    '\\', 'z', 'x', 'c', 'v',  'b', 'n',                 /* 49 */
    'm',  ',', '.', '/', 0,                              /* Right shift */
    '*',                                                 /* Num pad '*' */
    0,                                                   /* Alt */
    ' ',                                                 /* Space bar */
    0,                                                   /* Caps lock */
    0,                                                   /* 59 - F1 key ... > */
    0,    0,   0,   0,   0,    0,   0,   0,   0,         /* < ... F10 */
    0,                                                   /* 69 - Num lock*/
    0,                                                   /* Scroll Lock */
    0,                                                   /* Home key */
    0,                                                   /* Up Arrow */
    0,                                                   /* Page Up */
    '-',                                                 /* Num pad '-' */
    0,                                                   /* Left Arrow */
    0,    0,                                             /* Right Arrow */
    '+',                                                 /* Num pad '+' */
    0,                                                   /* 79 - End key*/
    0,                                                   /* Down Arrow */
    0,                                                   /* Page Down */
    0,                                                   /* Insert Key */
    0,                                                   /* Delete Key */
    0,    0,   0,   0,                                   /* F11 Key */
    0,                                                   /* F12 Key */
    0, /* All other keys are undefined */
};

int8_t uskbdShifted[LEN_256] = {
    0,    27,                                            /* Escape */
    '!',  '@', '#', '$', '%',  '^', '&', '*',            /* 9 */
    '(',  ')', '_', '+', '\b',                           /* Backspace */
    '\t',                                                /* Tab */
    'Q',  'W', 'E', 'R',                                 /* 19 */
    'T',  'Y', 'U', 'I', 'O',  'P', '{', '}', '\n',      /* Enter key */
    0,                                                   /* Control key */
    'A',  'S', 'D', 'F', 'G',  'H', 'J', 'K', 'L',  ':', /* 39 */
    '\"', '~', 0,                                        /* Left shift */
    '|',  'Z', 'X', 'C', 'V',  'B', 'N',                 /* 49 */
    'M',  '<', '>', '?', 0,                              /* Right shift */
    '*',                                                 /* Num pad '*' */
    0,                                                   /* Alt */
    ' ',                                                 /* Space bar */
    0,                                                   /* Caps lock */
    0,                                                   /* 59 - F1 key ... > */
    0,    0,   0,   0,   0,    0,   0,   0,   0,         /* < ... F10 */
    0,                                                   /* 69 - Num lock*/
    0,                                                   /* Scroll Lock */
    0,                                                   /* Home key */
    0,                                                   /* Up Arrow */
    0,                                                   /* Page Up */
    '-',                                                 /* Num pad '-' */
    0,                                                   /* Left Arrow */
    0,    0,                                             /* Right Arrow */
    '+',                                                 /* Num pad '+' */
    0,                                                   /* 79 - End key*/
    0,                                                   /* Down Arrow */
    0,                                                   /* Page Down */
    0,                                                   /* Insert Key */
    0,                                                   /* Delete Key */
    0,    0,   0,   0,                                   /* F11 Key */
    0,                                                   /* F12 Key */
    0, /* All other keys are undefined */

};

static volatile uint8_t g_shiftPressed = 0;
static volatile uint8_t g_capsLockON = 0;

/* Handles the keyboard interrupt */
/* TODO: Test all characters, tab prints unknown character */
void keyboard_callback(registers_t regs __attribute__((unused))) {
  uint8_t scancode;

  /* Read from the keyboard's data buffer */
  scancode = inb(KEYBOARD_DATA_PORT);

  /* If the top bit of the byte we read from the keyboard is
   *  set, that means that a key has just been released */
  if (scancode & MASK_BIT8) {
    /* Check if shift key was released */
    if (scancode == RELEASED_SHIFT_LEFT || scancode == RELEASED_SHIFT_RIGHT) {
      g_shiftPressed = 0;
    }
  } else {
    /* Check if it is capslock */
    if (scancode == PRESSED_CAPS_LOCK) {
      g_capsLockON = !g_capsLockON;
      return;
    }

    if (g_capsLockON) {
      print_serial_ch(uskbdShifted[scancode]);
      print_screen_ch(uskbdShifted[scancode]);
    } else {
      /* Check if it is shift key */
      if (scancode == PRESSED_SHIFT_LEFT || scancode == PRESSED_SHIFT_RIGHT) {
        g_shiftPressed = 1;
      } else {
        if (g_shiftPressed) {
          print_serial_ch(uskbdShifted[scancode]);
          print_screen_ch(uskbdShifted[scancode]);
        } else {
          print_serial_ch(uskbd[scancode]);
          print_screen_ch(uskbd[scancode]);
        }
      }
    }
  }
}

/* Installs the keyboard handler into IRQ1 */
void init_keyboard() {
  /* Register callback for keyboard interrupt (Interrupt 1) */
  register_interrupt_handler(IRQ1, keyboard_callback);
}
