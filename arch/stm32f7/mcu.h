// Copyright (c) 2021 Sergey Lyubka
// All rights reserved
// https://www.st.com/resource/en/reference_manual/dm00124865-stm32f75xxx-and-stm32f74xxx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf
// Memory map: 2.2.2

#pragma once

#include "arch/common.h"

struct gpio {
  volatile uint32_t MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2];
};

static inline struct gpio *gpio_bank(uint16_t pin) {
  return (struct gpio *) (0x40020000 + 0x400 * (pin >> 8));
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
enum { GPIO_SPEED_LOW, GPIO_SPEED_MEDIUM, GPIO_SPEED_HIGH, GPIO_SPEED_INSANE };
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
    gpio->AFR[1] &= 15 << ((n - 8) * 4);
    gpio->AFR[1] |= af << ((n - 8) * 4);
  }
}

struct rcc {
  volatile uint32_t CR, PLLCFGR, CFGR, CIR, AHB1RSTR, AHB2RSTR, AHB3RSTR,
      RESERVED0, APB1RSTR, APB2RSTR, RESERVED1[2], AHB1ENR, AHB2ENR, AHB3ENR,
      RESERVED2, APB1ENR, APB2ENR, RESERVED3[2], AHB1LPENR, AHB2LPENR,
      AHB3LPENR, RESERVED4, APB1LPENR, APB2LPENR, RESERVED5[2], BDCR, CSR,
      RESERVED6[2], SSCGR, PLLI2SCFGR, PLLSAICFGR, DCKCFGR1, DCKCFGR2;
};
#define RCC ((struct rcc *) 0x40023800)
#define RCC_CR_HSEBYP 0x40000
#define RCC_CR_HSEON 0x10000
#define RCC_CFGR_SWS 0xc
#define RCC_CFGR_SW_HSI 0
#define RCC_CFGR_SW_HSE 1
#define RCC_CFGR_SW_PLL 2
#define RCC_CFGR_SW 3
#define RCC_CR_HSERDY 0x20000
#define RCC_CFGR_HPRE_DIV1 0
#define RCC_CFGR_PPRE1_DIV4 0x1400
#define RCC_CFGR_PPRE2_DIV2 0x8000
#define RCC_CFGR_PPRE2_DIV4 0xa000
#define RCC_CFGR_PPRE2_DIV8 0xc000
#define RCC_CFGR_PPRE2_DIV16 0xe000
#define RCC_CR_PLLRDY 0x02000000
#define RCC_PLLCFGR_PLLSRC_HSE 0x00400000
#define RCC_CR_PLLON 0x01000000

static inline void init_clock(void) {
  RCC->CR |= RCC_CR_HSEBYP | RCC_CR_HSEON;  // Start HSE
  while ((RCC->CR & RCC_CR_HSERDY) == 0) (void) 0;

#define PLL_M 8    // 8MHz HSE down-to 1MHz PLL input
#define PLL_N 432  // 432 MHz VCO output
#define PLL_P 2    // 216 MHz PLL output
#define PLL_Q 9    // 48  MHz (USB)
  RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) - 1) << 16) |
                 (RCC_PLLCFGR_PLLSRC_HSE) | (PLL_Q << 24);
  RCC->CR |= RCC_CR_PLLON;  // Enable the main PLL
#if 0
  FLASH->ACR = FLASH_ACR_ARTEN | FLASH_ACR_PRFTEN | FLASH_ACR_LATENCY_4WS;
#endif

  // Configure AHB/APB prescalers
  // AHB  Prescaler = /1  -> 216 MHz
  // APB1 Prescaler = /4  -> 54  MHz
  // APB2 Prescaler = /2  -> 108 MHz
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
  while ((RCC->CR & RCC_CR_PLLRDY) == 0) (void) 0;

#if 0
  RCC->CFGR &= ~RCC_CFGR_SW;     // Select the main PLL
  RCC->CFGR |= RCC_CFGR_SW_PLL;  // as system clock source
  while ((RCC->CFGR & RCC_CFGR_SWS) == 0) (void) 0;
#endif

  NVIC_SetPriority(-2, 255);  // PendSV has lowest prio
  SysTick_Config(216000);     // Enable SysTick interrupt
}

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
