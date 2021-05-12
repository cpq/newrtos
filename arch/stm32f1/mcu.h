// Copyright (c) 2021 Sergey Lyubka
// All rights reserved
//
// Settings common to all stm32f1xx
// TRM: https://www.st.com/resource/en/reference_manual/cd00171190.pdf

#include <stdint.h>
#include <string.h>

static inline void spin(uint32_t count) {
  while (count--) asm("nop");
}

#define BIT(x) ((uint32_t) 1 << (x))
#define PIN(bank, num) ((((bank) - 'A') << 8) | (num))

// RCC registers, TRM section 7.3, memory map section 3.3
struct rcc {
  volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR, APB2ENR, APB1ENR,
      BDCR, CSR;
};
#define RCC ((struct rcc *) 0x40021000)
enum {
  RCC_CR_HSEON = BIT(16),
  RCC_CR_HSERDY = BIT(17),
  RCC_CR_PLLON = BIT(24),
  RCC_CR_PLLRDY = BIT(25)
};
enum {
  RCC_CFGR_SW = 3,
  RCC_CFGR_SW_HSI = 0,
  RCC_CFGR_SW_HSE = 1,
  RCC_CFGR_SW_PLL = 2,
  RCC_CFGR_PLLMULL = (15U << 18),
  RCC_CFGR_PLLMULL9 = (7U << 18)
};

static inline void init_ram(void) {
  extern uint32_t _bss_start, _bss_end;
  extern uint32_t _data_start, _data_end, _data_flash_start;
  memset(&_bss_start, 0, ((char *) &_bss_end - (char *) &_bss_start));
  memcpy(&_data_start, &_data_flash_start,
         ((char *) &_data_end - (char *) &_data_start));
}

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

struct gpio {
  volatile uint32_t CRL, CRH, IDR, ODR, BSRR, BRR, LCKR;
};

static inline struct gpio *gpio_bank(uint16_t pin) {
  return (struct gpio *) (0x40010800 + 0x400 * (pin >> 8));
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
