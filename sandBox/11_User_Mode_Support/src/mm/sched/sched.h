#ifndef INCLUDE_SCHED_H
#define INCLUDE_SCHED_H

#pragma once
#include <paging.h>
#include <types.h>

typedef struct task {
  /* TODO: Add parent pid information and implement join */
  /* Process ID */
  uint32_t pid;
  /* Base pointer */
  uint32_t ebp;
  /* Stack pointer */
  uint32_t esp;
  /* Instruction pointer */
  uint32_t eip;
  /* Pointer to process page directory */
  page_directory_t *pageDirectory;
  /* Pointer to next task */
  struct task *next;
} task_t;

/** initialise_tasking:
 *  Function to initialize multitasking
 *  @param stackPointer Initial address of the stack when kernel is loaded
 */
void initialise_multitasking(uint32_t stackPointer);

/** schedule:
 *  Schedule next task when the timer elapsed (Based of frequency variable
 *  TIMER_FREQUENCY in timer.h)
 */
void schedule();

/** fork:
 *  API to fork the process and create new one out of it
 */
uint32_t fork();

/** move_stack:
 *  Relocate stack to new address
 *  @param newStackAddress  Pointer to address where the stack need to be moved
 *  @param size             Size of the stack
 */
void move_stack(void *newStackAddress, uint32_t size);

/** get_pid:
 *  returns PID of the process
 *  @return  Process ID
 */
uint32_t getpid();

#endif /* INCLUDE_SCHED_H */
