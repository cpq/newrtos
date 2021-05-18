// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include <stdio.h>

#include "board.h"

static void task1(void *param) {
  (void) param;  // Unused
  for (;;) {
    gpio_toggle(LED1);
    rtos_msleep(500);
    DEBUG(("RAM: %u\n", (unsigned) rtos_heap_available()));
  }
}

int main(void) {
  init_hardware();
  DEBUG(("free RAM: %u\n", (unsigned) rtos_heap_available()));
  rtos_task_create(task1, NULL, 256, 10);
  rtos_schedule();
  return 0;
}
