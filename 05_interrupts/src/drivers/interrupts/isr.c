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
  u32int ds;                 // Data segment selector
  cpu_state_t cpu_registers; // Pushed by pusha.
  stack_state_t
      stack_contents; // Interrupt number and error code (if applicable)
                      // Pushed by the processor automatically.
} registers_t;

// This gets called from our ASM interrupt handler stub.
void interrupt_handler(registers_t regs) {
  // kprint("recieved interrupt: ", 20);
  // kprint((char *)&regs.stack_contents.int_no, 4);
  // log("recieved interrupt: ", 20);
}
