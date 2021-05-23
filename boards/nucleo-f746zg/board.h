// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#pragma once

#include "mcu.h"

#define LED1 PIN('B', 0)   // On-board LED pin
#define LED2 PIN('B', 7)   // On-board LED pin
#define LED3 PIN('B', 14)  // On-board LED pin

static inline void rtos_init(void) {
  init_ram();
  init_clock();

  RCC->AHB1ENR |= BIT(0) | BIT(1) | BIT(2) | BIT(6);  // Init GPIO banks A,B,C,G
  gpio_init(LED1, GPIO_OUT, GPIO_PP, GPIO_SPEED_LOW, GPIO_PULL_NONE, 0);
  gpio_init(LED2, GPIO_OUT, GPIO_PP, GPIO_SPEED_LOW, GPIO_PULL_NONE, 0);
  gpio_init(LED3, GPIO_OUT, GPIO_PP, GPIO_SPEED_LOW, GPIO_PULL_NONE, 0);
  gpio_on(LED2);
}
