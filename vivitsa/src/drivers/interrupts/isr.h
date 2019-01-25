#ifndef INCLUDE_ISR_H
#define INCLUDE_ISR_H

#pragma once
#include <types.h>

typedef struct cpu_state {
  u32int edi;
  u32int esi;
  u32int ebp;
  u32int esp;
  u32int ebx;
  u32int edx;
  u32int ecx;
  u32int eax;
} cpu_state_t;

typedef struct stack_state {
  u32int int_no;
  u32int err_code;
  u32int eip;
  u32int cs;
  u32int eflags;
  u32int useresp;
  u32int ss;
} stack_state_t;

typedef struct registers {
  /* Data segment selector */
  u32int ds;
  /* Pushed by pusha */
  cpu_state_t cpu_registers;
  /* Interrupt number and error code (if applicable). Pushed by the processor
   * automatically.
   */
  stack_state_t stack_contents;
} registers_t;

typedef void (*isr_t)(registers_t);

void register_interrupt_handler(u8int n, isr_t handler);

/* Function to initialize IDT */
void init_idt();

#endif /* INCLUDE_ISR_H */
