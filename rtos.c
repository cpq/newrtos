// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include <errno.h>
#include <stdlib.h>

#include "rtos.h"

static char *s_heap, *s_brk;
size_t s_heap_size;

size_t rtos_heap_used(void) {
  return s_brk - s_heap;
}

size_t rtos_heap_available(void) {
  return &s_heap[s_heap_size] - s_brk;
}

void rtos_heap_init(void *ptr, size_t size) {
  s_heap = ptr;
  s_heap_size = size;
}

void *_sbrk(ptrdiff_t diff) {
  char *old = s_brk;
  if (&s_brk[diff] > &s_heap[s_heap_size]) {
    errno = ENOMEM;
    return NULL;
  }
  s_brk += diff;
  return old;
}

void rtos_task_create(void (*fn)(void *), void *data, size_t ssize, int prio) {
  (void) prio;
  (void) ssize;
  fn(data);
}

void rtos_schedule(void) {
  for (;;) (void) 0;
}
