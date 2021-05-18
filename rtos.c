// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include "rtos.h"

// Initialise newlib malloc. It expects us to implement _sbrk() call,
// which should return a pointer to the requested memory.
// Our `rtos_heap_init` function sets up an available memory region.
//
//   s_heap_start                s_brk               s_heap_end
//      |--------------------------|----------------------|
//      |      (used memory)       |    (free memory)     |

static char *s_heap_start, *s_heap_end, *s_brk;  // Set up by rtos_heap_init()

int rtos_heap_used(void) {
  return s_brk - s_heap_start;
}

int rtos_heap_available(void) {
  return s_heap_end - s_brk;
}

void rtos_heap_init(void *start, void *end) {
  s_heap_start = s_brk = start, s_heap_end = end;
}

void *_sbrk(ptrdiff_t diff) {
  char *old = s_brk;
  if (&s_brk[diff] > s_heap_end) {
    errno = ENOMEM;
    return NULL;
  }
  s_brk += diff;
  return old;
}

void rtos_task_create(void (*fn)(void *), void *data, int ssize, int prio) {
  (void) prio;
  (void) ssize;
  fn(data);
}

void rtos_schedule(void) {
  for (;;) (void) 0;
}

static volatile uint32_t s_tick;
void SysTick_Handler(void) {
  s_tick++;
}

unsigned long rtos_msleep(unsigned long milliseconds) {
  uint32_t until = s_tick + milliseconds;
  while (s_tick < until) asm("nop");
  return s_tick;
}

// stdlib might require this from us, so define here as a weak symbol
__attribute__((weak)) void _exit(int code) {
  (void) code;
  for (;;) asm("nop");
}
