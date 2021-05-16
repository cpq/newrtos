// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include <stdio.h>
#include "board.h"

int main(void) {
  init_hardware();

  // int counter = 0;
  for (;;) {
    led_toggle();
    // printf("%s %d %f\n", "hi...", counter++, 0.123); // Semihosting debug
    spin(1500000);
  }

  return 0;
}
