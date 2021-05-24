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
    gpio->AFR[1] &= 15 << ((n - 8) * 4);
    gpio->AFR[1] |= af << ((n - 8) * 4);
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

  rtos_freq_set(72000000);
  SysTick_Config(36000);  // Enable SysTick interrupt every 1ms
}

struct uart {
  volatile uint32_t CR1, CR2, CR3, BRR, GTPR, RTOR, RQR, ISR, ICR;
  volatile uint16_t RDR, RESERVED1, TDR, RESERVED2;
};
#define UART1 ((struct uart *) 0x40013800)
#define UART2 ((struct uart *) 0x40004400)
#define UART3 ((struct uart *) 0x40004800)

static inline void uart_init(struct uart *uart, uint16_t tx, uint16_t rx,
                             int baud) {
  gpio_init(tx, GPIO_AF, GPIO_PP, GPIO_SPEED_LOW, GPIO_PULL_NONE, 7);
  gpio_init(rx, GPIO_AF, GPIO_PP, GPIO_SPEED_LOW, GPIO_PULL_NONE, 7);
  uart->CR1 |= BIT(0) | BIT(2) | BIT(3);  // Enable this UART, RE, TE
  uart->BRR = rtos_freq_get() / baud;     // Set baud rate, TRM 29.5.4
}

static inline void uart_write_byte(struct uart *uart, uint8_t byte) {
  uart->TDR = byte;
  while ((uart->ISR & BIT(7)) == 0) (void) 0;
}

static inline void uart_write_buf(struct uart *uart, char *buf, int len) {
  while (len-- > 0) uart_write_byte(uart, *(uint8_t *) buf++);
}

static inline int uart_read_ready(struct uart *uart) {
  return uart->ISR & BIT(5);  // If RXNE bit is set, data is ready
}

static inline uint8_t uart_read_byte(struct uart *uart) {
  return uart->RDR & 255;
}

#ifndef LED1
#define LED1 PIN('B', 3)  // On-board LED pin
#endif

#define UART UART2
#define UART_TX PIN('A', 2)
#define UART_RX PIN('A', 15)

static inline void rtos_init(void) {
  init_ram();
  init_clock();

  RCC->AHBENR |= BIT(17) | BIT(18) | BIT(19);  // Enable GPIO banks A,B,C
  RCC->APB2ENR |= BIT(14);                     // Enable USART1 clock
  RCC->APB1ENR |= BIT(17) | BIT(18);           // Enable USART2 and USART3
  gpio_init(LED1, GPIO_OUT, GPIO_PP, GPIO_SPEED_LOW, GPIO_PULL_NONE, 0);
}
