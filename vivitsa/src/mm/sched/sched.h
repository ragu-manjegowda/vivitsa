#ifndef INCLUDE_SCHED_H
#define INCLUDE_SCHED_H

#pragma once
#include <paging.h>
#include <types.h>

typedef struct task {
  /* TODO: Add parent pid information and implement join */
  /* Process ID */
  u32int pid;
  /* Base pointer */
  u32int ebp;
  /* Stack pointer */
  u32int esp;
  /* Instruction pointer */
  u32int eip;
  /* Pointer to process page directory */
  page_directory_t *pageDirectory;
  /* Pointer to next task */
  struct task *next;
} task_t;

/** initialise_tasking:
 *  Function to initialize multitasking
 *  @param stackPointer Initial address of the stack when kernel is loaded
 */
void initialise_multitasking(u32int stackPointer);

/** schedule:
 *  Schedule next task when the timer elapsed (Based of frequency variable
 *  TIMER_FREQUENCY in timer.h)
 */
void schedule();

/** fork:
 *  API to fork the process and create new one out of it
 */
u32int fork();

/** move_stack:
 *  Relocate stack to new address
 *  @param newStackAddress  Pointer to address where the stack need to be moved
 *  @param size             Size of the stack
 */
void move_stack(void *newStackAddress, u32int size);

/** get_pid:
 *  returns PID of the process
 *  @return  Process ID
 */
u32int getpid();

#endif /* INCLUDE_SCHED_H */
