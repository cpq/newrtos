// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include "device.h"

int main(void) {
  init_ram();
  init_hardware();

  for (;;) {
    led_toggle();
    spin(500000);
  }

  return 0;
}
