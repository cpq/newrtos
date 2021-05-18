// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#pragma once
#include "mcu.h"

#define LED1 PIN('B', 3)  // On-board LED pin

static inline void init_hardware(void) {
  init_ram();
  init_clock();
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;
  gpio_init(LED1, GPIO_OUT, GPIO_PP, GPIO_SPEED_LOW, GPIO_PULL_NONE, 0);
}
