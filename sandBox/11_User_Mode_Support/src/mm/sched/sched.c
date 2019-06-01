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
uint32_t g_initialStackPointer;

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
extern uint32_t read_eip();
/*
 * Function call to switch task, implemented in assembly.
 * Extern allows to access ASM from this C code.
 */
extern void task_switch(uint32_t eip, uint32_t pageDirPhysicalAddr, uint32_t ebp,
                        uint32_t esp);

/* Global variable for PID, PID of first process is 1 and incremented everytime
 * we fork a new process */
uint32_t g_PID = 1;

void initialise_multitasking(uint32_t stackPointer) {
  /* Disable interrupts */
  asm volatile("cli");

  /* Store the current stack pointer */
  g_initialStackPointer = stackPointer;

  /*
   * Relocate the stack to predefined location and mark it as non kernel memory
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

void move_stack(void *newStackAddress, uint32_t size) {
  uint32_t i;
  /* Allocate pages for the new stack, allocation works in ascending order of
   * address, stack grows in descending mem address
   */
  for (i = (uint32_t)newStackAddress;
       i >= ((uint32_t)newStackAddress - size - MEM_4KB); i -= MEM_4KB) {
    // General-purpose stack is in user-mode.
    alloc_frame(get_page(i, 1, g_currentDirectory), 0 /* User mode */,
                1 /* Is writable */);
  }

  /*
   * Since page table is modified flush the TLB, simply by writing back to cr3
   */
  uint32_t pdAddr;
  asm volatile("mov %%cr3, %0" : "=r"(pdAddr));
  asm volatile("mov %0, %%cr3" : : "r"(pdAddr));

  uint32_t oldStackPointer = 0;
  uint32_t oldBasePointer = 0;
  asm volatile("mov %%esp, %0" : "=r"(oldStackPointer));
  asm volatile("mov %%ebp, %0" : "=r"(oldBasePointer));

  /* Offset to add to old stack addresses to get a new stack address */
  uint32_t offset = (uint32_t)newStackAddress - g_initialStackPointer;

  uint32_t newStackPointer = oldStackPointer + offset;
  uint32_t newBasePointer = oldBasePointer + offset;

  /* TODO: replace size argument with size parameter*/
  custom_memcpy((void *)newStackPointer, (void *)oldStackPointer,
                g_initialStackPointer - oldStackPointer);

  /*
   * If the value of tmp is inside the range of the old stack, assume it is a
   * base pointer and remap it. This will unfortunately remap ANY value in
   * this range, whether they are base pointers or not.
   */
  for (i = (uint32_t)newStackAddress; i > (uint32_t)newStackAddress - size;
       i -= 4) {
    uint32_t tmp = *(uint32_t *)i;
    if ((oldStackPointer < tmp) && (tmp < g_initialStackPointer)) {
      tmp = tmp + offset;
      uint32_t *tmp2 = (uint32_t *)i;
      *tmp2 = tmp;
    }
  }

  /* Change stack */
  asm volatile("mov %0, %%esp" : : "r"(newStackPointer));
  asm volatile("mov %0, %%ebp" : : "r"(newBasePointer));
}

void schedule() {
  /* If we haven't initialised tasking yet, return */
  if (!g_currentTask)
    return;

  uint32_t esp, ebp, eip;
  asm volatile("mov %%esp, %0" : "=r"(esp));
  asm volatile("mov %%ebp, %0" : "=r"(ebp));

  /*
   * Read the instruction pointer:
   * One of two things could have happened when this function exits -
   *   (a) We called the function and it returned the EIP as requested.
   *   (b) We have just switched tasks, and because the saved EIP is essentially
   *       the instruction after read_eip(), it will seem as if read_eip has
   *       just returned.
   * In the second case we need to return immediately. To detect it we put a
   * dummy value in EAX further down at the end of this function. As C returns
   * values in EAX, it will look like the return value is this dummy value!
   * (0x12345).
   */
  eip = read_eip();

  /* Have we just switched tasks? */
  if (eip == 0x12345)
    return;

  g_currentTask->eip = eip;
  g_currentTask->esp = esp;
  g_currentTask->ebp = ebp;

  /* Since g_currentTask is initialised with g_readyQueue*/
  g_currentTask = g_currentTask->next;
  if (!g_currentTask)
    g_currentTask = g_readyQueue;

  eip = g_currentTask->eip;
  esp = g_currentTask->esp;
  ebp = g_currentTask->ebp;
  g_currentDirectory = g_currentTask->pageDirectory;

  task_switch(eip, g_currentDirectory->physicalAddr, ebp, esp);
}

uint32_t fork() {
  asm volatile("cli");

  task_t *parentTask = (task_t *)g_currentTask;
  page_directory_t *directory = clone_directory(g_currentDirectory);
  task_t *newTask = (task_t *)kmalloc(sizeof(task_t));

  newTask->pid = g_PID++;
  newTask->esp = 0;
  newTask->ebp = 0;
  newTask->eip = 0;
  newTask->pageDirectory = directory;
  newTask->next = 0;

  /* Add it to the end of the ready queue */
  task_t *tempTask = (task_t *)g_readyQueue;
  while (tempTask->next)
    tempTask = tempTask->next;
  tempTask->next = newTask;

  /* Forked process starts executing from current instruction of parent */
  uint32_t eip = read_eip();

  /* We could be the parent or the child here - check */
  if (g_currentTask == parentTask) {
    /* We are the parent, so set up the esp/ebp/eip for our child */
    uint32_t esp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    uint32_t ebp;
    asm volatile("mov %%ebp, %0" : "=r"(ebp));
    newTask->esp = esp;
    newTask->ebp = ebp;
    newTask->eip = eip;
    asm volatile("sti");
    return newTask->pid;
  } else {
    /* We are the child */
    return 0;
  }
}

uint32_t getpid() { return g_currentTask->pid; }
