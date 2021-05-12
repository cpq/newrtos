// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include "board.h"  // Set ARCH and BOARD in the Makefile

int main(void) {
  init_ram();
  init_hardware();

  for (;;) {
    led_toggle();
    spin(500000);
  }

  return 0;
}
