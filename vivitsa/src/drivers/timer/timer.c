#include "timer.h"
#include <helpers.h>
#include <io.h>
#include <isr.h>
#include <logger.h>
#include <sched.h>

/* Define a macro for timer interrupt */
#define IRQ0 32
#define PIT_DEFAULT_CLOCK 1193180
#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_DATA 0x40

/* We get approximately 100 ticks per second, so by ticks % 100 gives us time in
 * seconds. So using 16 bit counter to keep track of ticks
 */
u32int TIMER_TICKS = 0;

/* PIT command register has the following fields
 *
 * name | value | size | desc
 * ---------------------------
 * CNTR |     0 |    2 | select channel zero (for timer)
 * RW   |     3 |    2 | LS 8 bits will be sent first them MS 8 bits is sent
 * MODE |     3 |    3 | generate square wave
 * BCD  |     0 |    1 | 16 bit counter
 * COMMAND = 0x36
 */
#define PIT_COMMAND 0x36

/* Callback function that prints tick to screen when there is timer interrupt */
static void timer_callback(registers_t regs) {
  /* suppress unused parameter warning */
  (void)regs;
  /* Increment our 'tick count' */
  TIMER_TICKS++;
  /* Schedule next task */
  if (TIMER_TICKS % TIMER_FREQUENCY == 0) {
    schedule();
  }
}

void init_timer(u32int frequency) {
  /* Register callback for timer interrupt (Interrupt 0) */
  register_interrupt_handler(IRQ0, timer_callback);

  /* The value we send to the PIT is the value to divide it's input clock
   * (1193180 Hz) by, to get our required frequency. Important to note is
   * that the divisor must be small enough to fit into 16-bits.
   */
  u32int divisor = PIT_DEFAULT_CLOCK / frequency;

  /* Byte (0x36) sets the PIT to repeating mode (so that when the divisor
   * counter reaches zero it's automatically refreshed) and tells it we want to
   * set the divisor value.
   */
  outb(PIT_COMMAND_PORT, PIT_COMMAND);

  /* Divisor has to be sent byte-wise, so split here into upper/lower bytes. */
  u8int ls_bits = (u8int)(divisor & 0xFF);
  u8int ms_bits = (u8int)((divisor >> 8) & 0xFF);

  /* Send the frequency divisor. */
  outb(PIT_CHANNEL0_DATA, ls_bits);
  outb(PIT_CHANNEL0_DATA, ms_bits);

  /* Enable timer interrupt, otherwise there won't be any interrupt */
  asm("sti");
}

void sleep(u32int centiSeconds) {
  u32int ticks = TIMER_TICKS + centiSeconds;
  /* If 32 bit number overflows */
  if (ticks < TIMER_TICKS) {
    /* Wait till TIMER_TICKS to overflow */
    while (TIMER_TICKS > 20)
      ;
    while (TIMER_TICKS < ticks)
      ;
  } else {
    while (TIMER_TICKS < ticks)
      ;
  }
}
