// Copyright (c) 2021 Sergey Lyubka
// All rights reserved
//
// RM0008 https://www.st.com/resource/en/reference_manual/cd00171190.pdf

#pragma once

#include "arch/common.h"

// RCC registers, TRM section 7.3, memory map section 3.3
struct rcc {
  volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR, APB2ENR, APB1ENR,
      BDCR, CSR;
};
#define RCC ((struct rcc *) 0x40021000)

struct gpio {
  volatile uint32_t CRL, CRH, IDR, ODR, BSRR, BRR, LCKR;
};

// GPIO registers, TRM section 9.2, memory map section 3.3, regs 9.1.0
static inline struct gpio *gpio_bank(uint16_t pin) {
  return (struct gpio *) (0x40010800 + 0x400 * (pin >> 8));
}

static inline void gpio_init(uint16_t pin, uint8_t mode, uint8_t type,
                             uint8_t speed, uint8_t pull, uint8_t af) {
  struct gpio *gpio = gpio_bank(pin);
  int n = pin & 255, shift = (n < 8 ? n : (n - 8)) << 2;
  volatile uint32_t mask = 0, *cr = n < 8 ? &gpio->CRL : &gpio->CRH;

  if (mode == GPIO_MODE_OUTPUT || mode == GPIO_MODE_AF) {
    mask = 2;                                  // 2MHz
    if (speed == GPIO_SPEED_MEDIUM) mask = 1;  // 10 MHz
    if (speed > GPIO_SPEED_MEDIUM) mask = 3;   // 50 MHz
    if (mode == GPIO_MODE_OUTPUT) {
      if (type == GPIO_OTYPE_PUSH_PULL) mask |= 0 << 2;   // OUT PP
      if (type == GPIO_OTYPE_OPEN_DRAIN) mask |= 1 << 2;  // OUT OD
    } else {
      if (type == GPIO_OTYPE_PUSH_PULL) mask |= 2 << 2;   // AF PP
      if (type == GPIO_OTYPE_OPEN_DRAIN) mask |= 3 << 2;  // AF OD
    }
  } else {
    if (pull == GPIO_PULL_NONE) mask |= 1 << 2;  // Floating input
    if (pull != GPIO_PULL_NONE) mask |= 2 << 2;  // Input with PU/PD
  }
  *cr &= ~(15UL << shift);
  *cr |= mask << shift;
  (void) af;
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

static inline void gpio_input(uint16_t pin) {
  gpio_init(pin, GPIO_MODE_INPUT, GPIO_OTYPE_PUSH_PULL, GPIO_SPEED_MEDIUM,
            GPIO_PULL_NONE, 0);
}

static inline void gpio_output(uint16_t pin) {
  gpio_init(pin, GPIO_MODE_OUTPUT, GPIO_OTYPE_PUSH_PULL, GPIO_SPEED_MEDIUM,
            GPIO_PULL_NONE, 0);
}

struct uart {
  volatile uint32_t SR, DR, BRR, CR1, CR2, CR3, GTPR;
};

#define UART1 ((struct uart *) 0x40013800)
#define UART2 ((struct uart *) 0x40004400)
#define UART3 ((struct uart *) 0x40004800)

static inline void uart_init(struct uart *uart, unsigned baud) {
  // https://www.st.com/resource/en/datasheet/stm32f103c8.pdf
  uint8_t af = 0;           // Alternate function
  uint16_t rx = 0, tx = 0;  // pins
  if (uart == UART1) af = 0, tx = PIN('A', 9), rx = PIN('A', 10);
  if (uart == UART2) af = 0, tx = PIN('A', 2), rx = PIN('A', 3);
  if (uart == UART3) af = 0, tx = PIN('B', 10), rx = PIN('B', 11);

  gpio_init(tx, GPIO_MODE_AF, GPIO_OTYPE_PUSH_PULL, GPIO_SPEED_MEDIUM, 0, af);
  gpio_init(rx, GPIO_MODE_AF, GPIO_OTYPE_PUSH_PULL, GPIO_SPEED_MEDIUM, 0, af);
  uart->CR1 = 0;                           // Disable this UART
  uart->BRR = rtos_freq_get() / baud;      // Set baud rate, TRM 29.5.4
  uart->CR1 = BIT(13) | BIT(2) | BIT(3);   // Enable this UART, RE, TE
}

static inline void uart_write_byte(struct uart *uart, uint8_t byte) {
  uart->DR = byte;
  while ((uart->SR & BIT(7)) == 0) (void) 0;
}

static inline void uart_write_buf(struct uart *uart, char *buf, size_t len) {
  while (len-- > 0) uart_write_byte(uart, *(uint8_t *) buf++);
}

static inline int uart_read_ready(struct uart *uart) {
  return uart->SR & BIT(5);  // If RXNE bit is set, data is ready
}

static inline uint8_t uart_read_byte(struct uart *uart) {
  return uart->DR & 255;
}

static inline void init_clock(void) {
  FLASH->ACR = 0x14;                      // Set flash wait states
  RCC->CR |= BIT(0);                      // Enable HSI
  while (!(RCC->CR & BIT(1))) (void) 0;   // Wait
  RCC->CFGR = 15UL << 18;                 // HSI | PLL_16. 4 * 16 = 64 MHz
  RCC->CR |= BIT(24);                     // Enable PLL
  while (!(RCC->CR & BIT(25))) (void) 0;  // Wait
  RCC->CFGR |= 2;                         // Set PLL as the clock source

  rtos_freq_set(64000000);                 // Set clock
  SysTick_Config(rtos_freq_get() / 1000);  // Enable 1KHz SysTick interrupt
}

#if !defined(LED1)
#define LED1 PIN('A', 5)  // On-board LED pin
#endif

#if !defined(UART)
#define UART UART2
#endif

static inline void rtos_init(void) {
  init_ram();
  init_clock();

  RCC->APB2ENR |= BIT(2) | BIT(3) | BIT(4);  // Enable GPIO bank A,B,C
  RCC->APB2ENR |= BIT(14);                   // Enable USART1
  RCC->APB1ENR |= BIT(17) | BIT(18);         // Enable USART2 and USART3
}
