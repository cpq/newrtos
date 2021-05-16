// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#pragma once

#include "mcu.h"

#define LED1 PIN('A', 5)  // On-board LED pin

static inline void led_toggle(void) {
  gpio_toggle(LED1);
}

static inline void init_hardware(void) {
  // Setup clocks
  RCC->CR |= (RCC_CR_HSEON);
  while (!(RCC->CR & RCC_CR_HSERDY)) (void) 0;
  RCC->CFGR &= ~(RCC_CFGR_SW);
  RCC->CFGR |= (RCC_CFGR_SW_HSE);
  RCC->CFGR &= ~(RCC_CFGR_PLLMULL);
  RCC->CFGR |= (RCC_CFGR_PLLMULL9);
  RCC->CR |= (RCC_CR_PLLON);
  while (!(RCC->CR & RCC_CR_PLLRDY)) (void) 0;
  RCC->CFGR &= ~(RCC_CFGR_SW);
  RCC->CFGR |= (RCC_CFGR_SW_PLL);

  RCC->APB2ENR |= BIT(2) | BIT(3) | BIT(4);  // Initialise GPIO banks A,B,C
  gpio_init(LED1, OUTPUT);                   // Initialise LED
}
