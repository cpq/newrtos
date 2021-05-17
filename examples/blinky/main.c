// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include <stdio.h>
#include "board.h"

int main(void) {
  init_hardware();
  printf("free RAM: %u\n", (unsigned) rtos_heap_available());

  for (;;) {
    led_toggle();
    spin(1500000);
  }

  return 0;
}
