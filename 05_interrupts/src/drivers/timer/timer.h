#ifndef INCLUDE_TIMER_H
#define INCLUDE_TIMER_H

#pragma once
#include <types.h>

/* Programmable Interval Timer's default frequency is 1.1931MHz, this macro
 * defines desired frequenct
 */
#define TIMER_FREQUENCY 1

void init_timer(u32int frequency);

#endif /* INCLUDE_TIMER_H */
