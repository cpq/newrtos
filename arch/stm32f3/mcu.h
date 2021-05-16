// Copyright (c) 2021 Sergey Lyubka
// All rights reserved
//
// Settings common to all stm32f3xx, TRM:
// https://www.st.com/resource/en/reference_manual/dm00043574-stm32f303xb-c-d-e-stm32f303x6-8-stm32f328x8-stm32f358xc-stm32f398xe-advanced-arm-based-mcus-stmicroelectronics.pdf

#pragma once

#include <stdint.h>
#include <string.h>

static inline void spin(uint32_t count) {
  while (count--) asm("nop");
}

#define BIT(x) ((uint32_t) 1 << (x))
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))

// RCC registers, TRM section 9.4, memory map section 3.2
struct rcc {
  volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR, APB2ENR, APB1ENR,
      BDCR, CSR, AHBRSTR, CFGR2, CFGR3;
};
#define RCC ((struct rcc *) 0x40021000)  // Same address as for the f1 series
enum {
  RCC_CR_HSION = BIT(0),
  RCC_CR_HSIRDY = BIT(1),
  RCC_CR_PLLON = BIT(24),
  RCC_CR_PLLRDY = BIT(25),
  RCC_CFGR_PLLSRC = BIT(16),
  RCC_CFGR_PLLMUL = (15U << 18U),
  RCC_CFGR_PLLMUL9 = 0x001C0000U,
  RCC_CFGR_SW = 3,
  RCC_CFGR_SW_PLL = 2,
};

// GPIO registers, TRM section 11.4, memory map section 3.2
struct gpio {
  volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR0,
      AFR1, BRR;
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

// GPIO state is a bit mask:
// 2xPULL | 2xSPEED | 1xTYPE | 2xMODE
enum { GPIO_MODE_IN, GPIO_MODE_OUT, GPIO_MODE_AF, GPIO_MODE_ANALOG };
enum { GPIO_TYPE_PP, GPIO_TYPE_OD };
enum { GPIO_SPEED_LOW, GPIO_SPEED_MEDIUM, GPIO_SPEED_HIGH = 3 };
enum { GPIO_PULL_NONE, GPIO_PULL_UP, GPIO_PULL_DOWN };
enum {
  OUTPUT = GPIO_MODE_OUT | (GPIO_TYPE_PP << 2) | (GPIO_SPEED_MEDIUM << 3) |
           (GPIO_PULL_NONE << 5),
  INPUT = GPIO_MODE_IN | (GPIO_TYPE_PP << 2) | (GPIO_SPEED_MEDIUM << 3) |
          (GPIO_PULL_NONE << 5),
  INPUT_PULLUP = GPIO_MODE_IN | (GPIO_TYPE_PP << 2) | (GPIO_SPEED_MEDIUM << 3) |
                 (GPIO_PULL_UP << 5),
};
static inline void gpio_init(uint16_t pin, uint8_t state) {
  struct gpio *gpio = gpio_bank(pin);
  uint8_t n = pin & 255, mode = state & 3, type = (state >> 2) & 1,
          speed = (state >> 3) & 3, pupdr = (state >> 5) & 3;
  gpio->MODER &= ~(3 << (n * 2));
  gpio->MODER |= (mode << (n * 2));
  gpio->OTYPER &= ~(1 << n);
  gpio->OTYPER |= (type << n);
  gpio->OSPEEDR &= ~(3 << (n * 2));
  gpio->OSPEEDR |= (speed << (n * 2));
  gpio->PUPDR &= ~(3 << (n * 2));
  gpio->PUPDR |= (pupdr << (n * 2));
}

static inline void init_ram(void) {
  extern uint32_t _sbss, _ebss;
  extern uint32_t _sdata, _edata, _sidata;
  memset(&_sbss, 0, ((char *) &_ebss - (char *) &_sbss));
  memcpy(&_sdata, &_sidata, ((char *) &_edata - (char *) &_sdata));
}

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
}
