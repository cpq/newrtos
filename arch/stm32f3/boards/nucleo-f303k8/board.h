// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#pragma once

#include "mcu.h"

#define RAM_SIZE 12288    // RAM size on this device, needed by link.ld
#define ROM_SIZE 65536    // Flash size for this device, needed by link.ld
#define LED1 PIN('B', 3)  // On-board LED pin

static inline void led_toggle(void) {
  gpio_toggle(LED1);
}

static inline void init_hardware(void) {
  RCC->CR |= RCC_CR_HSION;
  while (!(RCC->CR & RCC_CR_HSIRDY)) (void) 0;
  RCC->CR &= ~RCC_CR_PLLON;
  while (RCC->CR & RCC_CR_PLLRDY) (void) 0;
  RCC->CFGR &= ~RCC_CFGR_PLLSRC;
  RCC->CFGR &= ~RCC_CFGR_PLLMUL;
  RCC->CFGR |= RCC_CFGR_PLLMUL9;
  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY)) (void) 0;
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;

  RCC->AHBENR |= BIT(17) | BIT(18) | BIT(19);  // Initialise GPIO banks A,B,C
  gpio_init(LED1, OUTPUT);                     // Initialise LED
}
