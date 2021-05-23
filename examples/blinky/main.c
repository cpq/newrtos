// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include <stdio.h>

#include "mcu.h"

static void blink(void) {
  gpio_on(LED1);
  rtos_msleep(5);
  gpio_off(LED1);
  rtos_msleep(100);
}

static void taskfunc(void *param) {
  int delay = *(int *) param;
  for (;;) {
    blink();
    if (delay > 1000) blink();
    rtos_msleep(delay);
    DEBUG(("%d, RAM: %u\n", delay, (unsigned) rtos_heap_available()));
  }
}

int main(void) {
  rtos_init();
  DEBUG(("free RAM: %u\n", (unsigned) rtos_heap_available()));

  // Start two blinking tasks with different delays
  int arg1 = 700, arg2 = 1300;
  rtos_task_create(taskfunc, &arg1, 512);
  rtos_task_create(taskfunc, &arg2, 512);

  rtos_schedule();
  return 0;
}
