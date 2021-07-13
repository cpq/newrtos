// Copyright (c) 2021 Sergey Lyubka
// All rights reserved
//
// RM0316 https://www.st.com/resource/en/reference_manual/DM00043574-.pdf

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

static inline void gpio_init(uint16_t pin, uint8_t mode, uint8_t type,
                             uint8_t speed, uint8_t pull, uint8_t af) {
  struct gpio *gpio = gpio_bank(pin);
  uint8_t n = (uint8_t)(pin & 255);
  gpio->MODER &= ~(3UL << (n * 2));
  gpio->MODER |= ((uint32_t) mode) << (n * 2);
  gpio->OTYPER &= ~(1UL << n);
  gpio->OTYPER |= ((uint32_t) type) << n;
  gpio->OSPEEDR &= ~(3UL << (n * 2));
  gpio->OSPEEDR |= ((uint32_t) speed) << (n * 2);
  gpio->PUPDR &= ~(3UL << (n * 2));
  gpio->PUPDR |= ((uint32_t) pull) << (n * 2);
  if (n < 8) {
    gpio->AFR[0] &= 15UL << (n * 4);
    gpio->AFR[0] |= ((uint32_t) af) << (n * 4);
  } else {
    gpio->AFR[1] &= 15UL << ((n - 8) * 4);
    gpio->AFR[1] |= ((uint32_t) af) << ((n - 8) * 4);
  }
}
static inline void gpio_input(uint16_t pin) {
  gpio_init(pin, GPIO_MODE_INPUT, GPIO_OTYPE_PUSH_PULL, GPIO_SPEED_MEDIUM,
            GPIO_PULL_NONE, 0);
}
static inline void gpio_output(uint16_t pin) {
  gpio_init(pin, GPIO_MODE_OUTPUT, GPIO_OTYPE_PUSH_PULL, GPIO_SPEED_MEDIUM,
            GPIO_PULL_NONE, 0);
}

struct rcc {
  volatile uint32_t CR, CFGR, CIR, APB2RSTR, APB1RSTR, AHBENR, APB2ENR, APB1ENR,
      BDCR, CSR, AHBRSTR, CFGR2, CFGR3;
};
#define RCC ((struct rcc *) 0x40021000)

static inline void init_clock(void) {
  FLASH->ACR = 0x12;
  RCC->CR |= BIT(0);                      // HSI ON
  while (!(RCC->CR & BIT(1))) (void) 0;   // Wait
  RCC->CFGR = 15UL << 18;                 // PLL_16. 8MHz HSI / 2 * 16 = 64MHz
  RCC->CR |= BIT(24);                     // PLL ON
  while (!(RCC->CR & BIT(25))) (void) 0;  // Wait
  RCC->CFGR |= 2;                         // Use PLL
  rtos_freq_set(64000000);
  SysTick_Config(rtos_freq_get() / 1000);  // 1KHz SysTick
}

struct uart {
  volatile uint32_t CR1, CR2, CR3, BRR, GTPR, RTOR, RQR, ISR, ICR;
  volatile uint16_t RDR, RESERVED1, TDR, RESERVED2;
};
#define UART1 ((struct uart *) 0x40013800)
#define UART2 ((struct uart *) 0x40004400)
#define UART3 ((struct uart *) 0x40004800)

static inline void uart_init(struct uart *uart, unsigned long baud) {
  // https://www.st.com/resource/en/datasheet/stm32f303k8.pdf
  uint8_t af = 0;           // Alternate function
  uint16_t rx = 0, tx = 0;  // pins
  if (uart == UART1) af = 7, tx = PIN('A', 9), rx = PIN('A', 10);
  if (uart == UART2) af = 7, tx = PIN('A', 2), rx = PIN('A', 15);
  if (uart == UART3) af = 7, tx = PIN('B', 10), rx = PIN('B', 11);
  gpio_init(tx, GPIO_MODE_AF, GPIO_OTYPE_PUSH_PULL, GPIO_SPEED_MEDIUM, 0, af);
  gpio_init(rx, GPIO_MODE_AF, GPIO_OTYPE_PUSH_PULL, GPIO_SPEED_MEDIUM, 0, af);
  uart->BRR = rtos_freq_get() / baud;     // Set baud rate, TRM 29.5.4
  uart->CR1 |= BIT(0) | BIT(2) | BIT(3);  // Enable this UART, RE, TE
}

static inline void uart_write_byte(struct uart *uart, uint8_t byte) {
  uart->TDR = byte;
  while ((uart->ISR & BIT(7)) == 0) (void) 0;
}

static inline void uart_write_buf(struct uart *uart, char *buf, size_t len) {
  while (len-- > 0) uart_write_byte(uart, *(uint8_t *) buf++);
}

static inline int uart_read_ready(struct uart *uart) {
  return uart->ISR & BIT(5);  // If RXNE bit is set, data is ready
}

static inline uint8_t uart_read_byte(struct uart *uart) {
  return (uint8_t)(uart->RDR & 255);
}

#ifndef LED1
#define LED1 PIN('B', 3)  // On-board LED pin
#endif

#ifndef UART
#define UART UART2
#endif

static inline void rtos_init(void) {
  init_ram();
  init_clock();

  RCC->AHBENR |= BIT(17) | BIT(18) | BIT(19);  // Enable GPIO banks A,B,C
  RCC->APB2ENR |= BIT(14);                     // Enable USART1
  RCC->APB1ENR |= BIT(17) | BIT(18);           // Enable USART2 and USART3
}
