// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#pragma once

#include "mcu.h"

#define LED1 PIN('A', 5)  // On-board LED pin

static inline void rtos_init(void) {
  init_ram();
  init_clock();

  RCC->APB2ENR |= BIT(2) | BIT(3) | BIT(4);  // Initialise GPIO banks A,B,C
  gpio_init(LED1, OUTPUT);                   // Initialise LED
}
