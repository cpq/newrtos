// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "rtos.h"

struct task {
  unsigned long *sp;   // Stack pointer - must be the first attribute
  void (*fn)(void *);  // Task function
  void *arg;           // Task function argument
  int id;              // Task ID
  struct task *next;   // Next in chain
};

static struct task *s_tasks;         // Tasks list
static struct task *s_current_task;  // Current task
static uint32_t s_systick;           // System tick counter
static int s_start_scheduling;       // Marker to start scheduling
static char *s_heap_start;           // Heap start. Set up by rtos_heap_init()
static char *s_heap_end;             // Heap end
static char *s_brk;                  // Current heap usage marker
static unsigned long s_freq;         // MCU frequency in HZ

static void rtos_task_delete(struct task *t) {
  DEBUG(("task %d deleting\n", t->id));
  struct task **p = &s_tasks;
  while (*p != t) p = &(*p)->next;
  *p = s_current_task = t->next;
  free(t);
  for (;;) asm("nop");  // Loop until someone preempts us
}

// We run user function via a wrapper which deletes the task if the
// user-provided function returns
static inline void rtos_task_start(struct task *t) {
  DEBUG(("task %d starting\n", t->id));
  s_start_scheduling = 1;
  t->fn(t->arg);
  rtos_task_delete(t);
}

int rtos_task_create(void (*fn)(void *), void *arg, size_t ssize) {
  static int id;
  struct task *task = malloc(sizeof(*task) + ssize);
  if (task == NULL) return 0;
  task->id = id++;
  task->fn = fn;
  task->arg = arg;
  task->next = s_tasks;
  s_tasks = s_current_task = task;

  // Hand-craft task's cpu context, as if it was interrupted
  task->sp = (unsigned long *) (task + 1) + ssize;
  if (((uint32_t) task->sp) & 7) task->sp--;   // 8-byte align for soft float
  task->sp[8] = (uintptr_t) task;              // argv[0]
  task->sp[14] = (uintptr_t) rtos_task_start;  // return address
  task->sp[15] = 0x01000000;                   // xPSR

  DEBUG(("created task %d, sp: %p\n", task->id, task->sp));
  return task->id;
}

void rtos_task_switch(void) {
  struct task *t = s_current_task, *next = t && t->next ? t->next : s_tasks;
  // DEBUG(("switch %d -> %d\n", t ? t->id : -1, next ? next->id : -1));
  s_current_task = next;
}

void rtos_schedule(void) {
  if (s_current_task == NULL) return;  // No tasks to run

  // Set psp = first task's stack pointer
  void *sp = s_current_task->sp;
  asm volatile("mov r0, %[v]; msr psp, r0; isb;" : : [v] "r"(sp) : "memory");

  // Swith to the unprivileged mode
  // Set two low bits in the CONTROL register:
  //   bit 0 - enable unprivileged mode
  //   bit 1 - enable process stack (PSP) for threaded mode
#if 0
  asm volatile("mrs r0, control; orr r0, #3; msr control, r0; isb;");
#else
  asm volatile(
      "mrs r1, control;"
      "movs r3, #3;"
      "orr r0, r3;"
      "msr control, r0;"
      "isb;");
#endif

  // Start s_current_task
  asm volatile("ldr r0, =s_current_task; ldr r0, [r0]; bl rtos_task_start;");
}

__attribute__((naked)) void PendSV_Handler(void) {
  asm volatile(
      // If no task is running, don't save context before switching
      // if (!s_current_task) goto switch
      "ldr r3, =s_current_task;"  // r3 = &s_current_task
      "ldr r3, [r3];"             // r3 = *r3
      "cmp r3, #0;"               // if (r3 == 0)
      "beq .switch;"              // goto switch; (we don't use cbz for M0)

      // Save context of the s_current_task
      "mrs r0, psp;"  // r0 = psp. Next is:
#if 0
      "stmdb r0!, {r4-r11};"  // Save r4-r11 into PSP. Does not work on M0
      "str r0, [r3];"         // s_current_task->sp = r0
#else
      "sub r0, #32;"         // Space for another 8 registers
      "str r0, [r3];"        // s_current_task->sp = r0
      "stmia r0!, {r4-r7};"  // Store r4-r7 into the process's stack
      "mov r4, r8;"
      "mov r5, r9;"
      "mov r6, r10;"
      "mov r7, r11;"
      "stmia r0!, {r4-r7};"  // Store r8-r11 into the process's stack
#endif

      // Switch s_current_task to the next task
      ".switch:;"             // Switch marker
      "push {lr};"            // Save lr
      "bl rtos_task_switch;"  // Update s_current_task
      "pop {r2};"             // Restore lr into r2

      // Now s_current_task is different. Load its context
      "ldr r3, =s_current_task;"  // r3 = &s_current_task
      "ldr r3, [r3];"             // r3 = *r3
      "cmp r3, #0;"               // if (r3 == 0)
      "beq .done;"                // goto done;

      // s_current_task changed. Load its context
      "ldr r0, [r3];"  // r0 = psp
#if 0
      "ldmia r0!, {r4-r11};"  // Load r4-r11
#else
      "add r0, #16;"
      "ldmia r0!, {r4-r7};"  // Load r8-r11
      "mov r8, r4;"
      "mov r9, r5;"
      "mov r10, r6;"
      "mov r11, r7;"
      "sub r0, #32;"
      "ldmia r0!, {r4-r7};"  // Load r4-r7
      "add r0, #16;"
#endif
      "msr psp, r0; isb;"  // Update PSP

      ".done:;"  // Exit marker
      "bx r2"    // Return to the switched process
  );
}

void SysTick_Handler(void) {
  s_systick++;
  if (s_start_scheduling && s_current_task) {
    // Trigger PendSV_Handler
    // SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    ((uint32_t volatile *) 0xe000ed04)[0] |= 1UL << 28;
  }
}

unsigned long rtos_msleep(unsigned long milliseconds) {
  uint32_t until = s_systick + milliseconds;
  while (s_systick < until) asm("nop");
  return s_systick;
}

void _exit(int code) {
  (void) code;
  for (;;) asm("nop");
}

// Initialise newlib malloc. It expects us to implement _sbrk() call,
// which should return a pointer to the requested memory.
// Our `rtos_heap_init` function sets up an available memory region.
//
//   s_heap_start                s_brk               s_heap_end
//      |--------------------------|----------------------|
//      |      (used memory)       |    (free memory)     |

int rtos_ram_used(void) {
  return s_brk - s_heap_start;
}

int rtos_ram_free(void) {
  return s_heap_end - s_brk;
}

void rtos_heap_init(void *start, void *end) {
  s_heap_start = s_brk = start, s_heap_end = end;
}

void rtos_freq_set(unsigned long freq) {
  s_freq = freq;
}

unsigned long rtos_freq_get(void) {
  return s_freq;
}

__attribute__((weak)) void *_sbrk(ptrdiff_t diff) {
  char *old = s_brk;
  if (&s_brk[diff] > s_heap_end) {
    errno = ENOMEM;
    return NULL;
  }
  s_brk += diff;
  return old;
}
