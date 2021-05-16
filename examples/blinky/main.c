// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include <stdio.h>
#include "board.h"

int main(void) {
  init_hardware();

  // int c = 0;
  for (;;) {
    led_toggle();
    // printf("%s %d\n", "hi...", c++);
    spin(1500000);
  }

  return 0;
}
