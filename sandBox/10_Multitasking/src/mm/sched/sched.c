#include "sched.h"
#include <helpers.h>
#include <isr.h>
#include <kheap.h>
#include <logger.h>

/* The pointer to current process/task */
volatile task_t *g_currentTask;
/* Pointer to queue of processess/tasks that are ready to be scheduled */
volatile task_t *g_readyQueue;
/* Placeholder to store original stack pointer */
u32int g_initialStackPointer;

/*
 * Accessing g_kernelDirectory and g_currentDirectory defined in paging.c
 * through extern since we use it for scheduling
 */
extern page_directory_t *g_kernelDirectory;
extern page_directory_t *g_currentDirectory;

/*
 * Function call to get the current instruction pointer implemented in assembly.
 * Extern allows to access ASM from this C code.
 */
extern u32int read_eip();

/* Global variable for PID, PID of first process is 1 and incremented everytime
 * we fork a new process */
u32int g_PID = 1;

void initialise_multitasking(u32int stackPointer) {
  /* Disable interrupts */
  asm volatile("cli");

  /* Store the current stack pointer */
  g_initialStackPointer = stackPointer;

  /*
   * Relocate the stack to predefined location and mark it as non kernel memort
   * so that next time we clone page directory we copy stack instead of linking
   * it (clone page directory only copies non kernel pages)
   */
  move_stack((void *)MEM_3_5GB, MEM_4KB);

  /* Initialise the first task (kernel task) */
  g_currentTask = g_readyQueue = (task_t *)kmalloc(sizeof(task_t));
  g_currentTask->pid = g_PID++;
  g_currentTask->ebp = 0;
  g_currentTask->esp = 0;
  g_currentTask->eip = 0;
  g_currentTask->pageDirectory = g_currentDirectory;
  g_currentTask->next = 0;

  /* Enable interrupts back */
  asm volatile("sti");
}

void move_stack(void *new_stack_start, u32int size) {
  u32int i;
  // Allocate some space for the new stack.
  for (i = (u32int)new_stack_start;
       i >= ((u32int)new_stack_start - size - 0x1000); i -= 0x1000) {
    // General-purpose stack is in user-mode.
    alloc_frame(get_page(i, 1, g_currentDirectory), 0 /* User mode */,
                1 /* Is writable */);
  }

  // Flush the TLB by reading and writing the page directory address again.
  u32int pd_addr;
  asm volatile("mov %%cr3, %0" : "=r"(pd_addr));
  asm volatile("mov %0, %%cr3" : : "r"(pd_addr));

  // Old ESP and EBP, read from registers.
  u32int old_stack_pointer;
  asm volatile("mov %%esp, %0" : "=r"(old_stack_pointer));
  u32int old_base_pointer;
  asm volatile("mov %%ebp, %0" : "=r"(old_base_pointer));

  // Offset to add to old stack addresses to get a new stack address.
  u32int offset = (u32int)new_stack_start - g_initialStackPointer;

  // New ESP and EBP.
  u32int new_stack_pointer = old_stack_pointer + offset;
  u32int new_base_pointer = old_base_pointer + offset;

  // Copy the stack.
  custom_memcpy((void *)new_stack_pointer, (void *)old_stack_pointer,
                g_initialStackPointer - old_stack_pointer);

  // Backtrace through the original stack, copying new values into
  // the new stack.
  for (i = (u32int)new_stack_start; i > (u32int)new_stack_start - size;
       i -= 4) {
    u32int tmp = *(u32int *)i;
    // If the value of tmp is inside the range of the old stack, assume it is a
    // base pointer and remap it. This will unfortunately remap ANY value in
    // this range, whether they are base pointers or not.
    if ((old_stack_pointer < tmp) && (tmp < g_initialStackPointer)) {
      tmp = tmp + offset;
      u32int *tmp2 = (u32int *)i;
      *tmp2 = tmp;
    }
  }

  // Change stacks.
  asm volatile("mov %0, %%esp" : : "r"(new_stack_pointer));
  asm volatile("mov %0, %%ebp" : : "r"(new_base_pointer));
}

void schedule() {
  // If we haven't initialised tasking yet, just return.
  if (!g_currentTask)
    return;

  // Read esp, ebp now for saving later on.
  u32int esp, ebp, eip;
  asm volatile("mov %%esp, %0" : "=r"(esp));
  asm volatile("mov %%ebp, %0" : "=r"(ebp));

  // Read the instruction pointer. We do some cunning logic here:
  // One of two things could have happened when this function exits -
  //   (a) We called the function and it returned the EIP as requested.
  //   (b) We have just switched tasks, and because the saved EIP is essentially
  //       the instruction after read_eip(), it will seem as if read_eip has
  //       just returned.
  // In the second case we need to return immediately. To detect it we put a
  // dummy value in EAX further down at the end of this function. As C returns
  // values in EAX, it will look like the return value is this dummy value!
  // (0x12345).
  eip = read_eip();

  // Have we just switched tasks?
  if (eip == 0x12345)
    return;

  // No, we didn't switch tasks. Let's save some register values and switch.
  g_currentTask->eip = eip;
  g_currentTask->esp = esp;
  g_currentTask->ebp = ebp;

  // Get the next task to run.
  g_currentTask = g_currentTask->next;
  // If we fell off the end of the linked list start again at the beginning.
  if (!g_currentTask)
    g_currentTask = g_readyQueue;

  eip = g_currentTask->eip;
  esp = g_currentTask->esp;
  ebp = g_currentTask->ebp;

  // Make sure the memory manager knows we've changed page directory.
  g_currentDirectory = g_currentTask->pageDirectory;
  // Here we:
  // * Stop interrupts so we don't get interrupted.
  // * Temporarily puts the new EIP location in ECX.
  // * Loads the stack and base pointers from the new task struct.
  // * Changes page directory to the physical address (physicalAddr) of the new
  // directory.
  // * Puts a dummy value (0x12345) in EAX so that above we can recognise that
  // we've just
  //   switched task.
  // * Restarts interrupts. The STI instruction has a delay - it doesn't take
  // effect until after
  //   the next instruction.
  // * Jumps to the location in ECX (remember we put the new EIP in there).
  asm volatile("         \
      cli;                 \
      mov %0, %%ecx;       \
      mov %1, %%esp;       \
      mov %2, %%ebp;       \
      mov %3, %%cr3;       \
      mov $0x12345, %%eax; \
      sti;                 \
      jmp *%%ecx           "
               :
               : "r"(eip), "r"(esp), "r"(ebp),
                 "r"(g_currentDirectory->physicalAddr));
}

u32int fork() {
  // We are modifying kernel structures, and so cannot
  asm volatile("cli");

  // Take a pointer to this process' task struct for later reference.
  task_t *parent_task = (task_t *)g_currentTask;

  // Clone the address space.
  page_directory_t *directory = clone_directory(g_currentDirectory);

  // Create a new process.
  task_t *new_task = (task_t *)kmalloc(sizeof(task_t));

  new_task->pid = g_PID++;
  new_task->esp = new_task->ebp = 0;
  new_task->eip = 0;
  new_task->pageDirectory = directory;
  new_task->next = 0;

  // Add it to the end of the ready queue.
  task_t *tmp_task = (task_t *)g_readyQueue;
  while (tmp_task->next)
    tmp_task = tmp_task->next;
  tmp_task->next = new_task;

  // This will be the entry point for the new process.
  u32int eip = read_eip();

  // We could be the parent or the child here - check.
  if (g_currentTask == parent_task) {
    // We are the parent, so set up the esp/ebp/eip for our child.
    u32int esp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    u32int ebp;
    asm volatile("mov %%ebp, %0" : "=r"(ebp));
    new_task->esp = esp;
    new_task->ebp = ebp;
    new_task->eip = eip;
    asm volatile("sti");

    return new_task->pid;
  } else {
    // We are the child.
    return 0;
  }
}

int getpid() { return g_currentTask->pid; }
