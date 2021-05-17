// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#pragma once
#include "mcu.h"

#define LED1 PIN('B', 3)  // On-board LED pin

static inline void led_toggle(void) {
  gpio_toggle(LED1);
}

static inline void init_hardware(void) {
  init_ram();
  init_clock();

  RCC->AHBENR |= BIT(17) | BIT(18) | BIT(19);  // Initialise GPIO banks A,B,C
  gpio_init(LED1, GPIO_OUT, GPIO_PP, GPIO_SPEED_LOW, GPIO_PULL_NONE, 0);
}
