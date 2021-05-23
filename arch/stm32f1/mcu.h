// Copyright (c) 2021 Sergey Lyubka
// All rights reserved
//
// Settings common to all stm32f1xx
// TRM: https://www.st.com/resource/en/reference_manual/cd00171190.pdf

#pragma once

#include "common.h"
#include "stm32f103xb.h"

// GPIO registers, TRM section 9.2, memory map section 3.3, regs 9.1.0
enum {
  GPIO_OUT_PP,
  GPIO_OUT_OD,
  GPIO_AF_PP,
  GPIO_AF_OD,
  GPIO_ANA = 0,
  GPIO_FLOATING,
  GPIO_PULLED
};
enum { GPIO_IN, GPIO_OUT_10MHZ, GPIO_OUT_2MHZ, GPIO_OUT_50MHZ };

enum {
  OUTPUT = (GPIO_OUT_PP << 2) | GPIO_OUT_2MHZ,
  INPUT = (GPIO_FLOATING << 2) | GPIO_IN,
  INPUT_PULLUP = (GPIO_PULLED << 2) | GPIO_IN
};

static inline GPIO_TypeDef *gpio_bank(uint16_t pin) {
  return (GPIO_TypeDef *) (0x40010800 + 0x400 * (pin >> 8));
}

static inline void gpio_on(uint16_t pin) {
  gpio_bank(pin)->ODR |= BIT(pin & 255);
}

static inline void gpio_off(uint16_t pin) {
  gpio_bank(pin)->ODR &= ~BIT(pin & 255);
}

static inline void gpio_toggle(uint16_t pin) {
  gpio_bank(pin)->ODR ^= BIT(pin & 255);
}

static inline void gpio_init(uint16_t pin, uint32_t mode) {
  uint8_t n = pin & 255, shift = (n < 8 ? n : (n - 8)) * 4;
  volatile uint32_t *reg = n < 8 ? &gpio_bank(pin)->CRL : &gpio_bank(pin)->CRH;
  *reg &= ~(15 << shift);
  *reg |= mode << shift;
}

static inline void init_clock(void) {
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

  NVIC_SetPriority(PendSV_IRQn, 0xFF);  // PendSV has lowest prio
  SysTick_Config(72000);                // Enable SysTick interrupt
}
