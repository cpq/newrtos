// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include <stdio.h>

#include "mcu.h"

static void taskfunc(void *param) {
  unsigned long delay = *(unsigned long *) param;

  for (;;) {
    rtos_msleep(delay);
    gpio_toggle(LED1);
    DEBUG(("%lu, RAM: %d\n", delay, rtos_ram_free()));
  }
}

int main(void) {
  rtos_init();
  DEBUG(("free RAM: %d\n", rtos_ram_free()));

  gpio_output(LED1);
  // for (;;) rtos_msleep(500), gpio_toggle(LED1);

  // Run two blinking tasks with different intervals
  unsigned long arg1 = 500, arg2 = 700;
  rtos_task_create(taskfunc, &arg1, 512);
  rtos_task_create(taskfunc, &arg2, 512);

  rtos_schedule();
  return 0;
}
