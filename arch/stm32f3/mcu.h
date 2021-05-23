// Copyright (c) 2021 Sergey Lyubka
// All rights reserved
//
// Settings common to all stm32f3xx, TRM:
// https://www.st.com/resource/en/reference_manual/dm00043574-stm32f303xb-c-d-e-stm32f303x6-8-stm32f328x8-stm32f358xc-stm32f398xe-advanced-arm-based-mcus-stmicroelectronics.pdf

#pragma once

#include "arch/common.h"

struct gpio {
  volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2],
      BRR;
};

static inline struct gpio *gpio_bank(uint16_t pin) {
  return (struct gpio *) (0x48000000 + 0x400 * (pin >> 8));
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
enum { GPIO_IN, GPIO_OUT, GPIO_AF, GPIO_ANALOG };
enum { GPIO_PP, GPIO_OD };
enum { GPIO_SPEED_LOW, GPIO_SPEED_MEDIUM, GPIO_SPEED_HIGH = 3 };
enum { GPIO_PULL_NONE, GPIO_PULL_UP, GPIO_PULL_DOWN };
static inline void gpio_init(uint16_t pin, uint8_t mode, uint8_t type,
                             uint8_t speed, uint8_t pull, uint8_t af) {
  struct gpio *gpio = gpio_bank(pin);
  uint8_t n = pin & 255;
  gpio->MODER &= ~(3 << (n * 2));
  gpio->MODER |= (mode << (n * 2));
  gpio->OTYPER &= ~(1 << n);
  gpio->OTYPER |= (type << n);
  gpio->OSPEEDR &= ~(3 << (n * 2));
  gpio->OSPEEDR |= (speed << (n * 2));
  gpio->PUPDR &= ~(3 << (n * 2));
  gpio->PUPDR |= (pull << (n * 2));
  if (n < 8) {
    gpio->AFR[0] &= 15 << (n * 4);
    gpio->AFR[0] |= af << (n * 4);
  } else {
    gpio->AFR[1] &= 15 << (n * 4);
    gpio->AFR[1] |= af << (n * 4);
  }
}

struct rcc {
  volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR, APB2ENR, APB1ENR,
      BDCR, CSR, AHBRSTR, CFGR2, CFGR3;
};
#define RCC ((struct rcc *) 0x40021000)
#define RCC_CR_HSION BIT(0)
#define RCC_CR_HSIRDY BIT(1)
#define RCC_CR_PLLON BIT(24)
#define RCC_CR_PLLRDY BIT(25)
#define RCC_CFGR_PLLSRC BIT(16)
#define RCC_CFGR_PLLMUL (15UL << 18)
#define RCC_CFGR_PLLMUL9 0x1c0000
#define RCC_CFGR_SW 3
#define RCC_CFGR_SW_HSI 0
#define RCC_CFGR_SW_HSE 1
#define RCC_CFGR_SW_PLL 2

static inline void init_clock(void) {
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

  SysTick_Config(36000);  // Enable SysTick interrupt every 1ms
}

#ifndef LED1
#define LED1 PIN('B', 3)  // On-board LED pin
#endif

static inline void rtos_init(void) {
  init_ram();
  init_clock();

  RCC->AHBENR |= BIT(17) | BIT(18) | BIT(19);  // Enable GPIO banks A,B,C
  gpio_init(LED1, GPIO_OUT, GPIO_PP, GPIO_SPEED_LOW, GPIO_PULL_NONE, 0);
}
