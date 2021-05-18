// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include <stdio.h>
#include "board.h"

static void task1(void *param) {
  for (;;) {
    led_toggle();
    spin(1500000);
    printf("%s, RAM: %u\n", (char *) param, (unsigned) rtos_heap_available());
  }
}

int main(void) {
  init_hardware();
  printf("free RAM: %u\n", (unsigned) rtos_heap_available());
  rtos_task_create(task1, "task1", 256, 10);
  rtos_schedule();
  return 0;
}
