#include <logger.h>
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

/* This gets called from our ASM interrupt handler stub. */
void interrupt_handler(registers_t regs) {
  char buffer[27] = "recieved interrupt!!!!!!!\n";
  log(buffer, 27);
  char buffer2[2];
  u8int number = regs.stack_contents.int_no % 10;
  buffer2[0] = '0' + number;
  log(buffer2, 2);
  kprint(buffer, 27);
  kprint(buffer2, 2);
}
