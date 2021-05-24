// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include <stdio.h>

#include "mcu.h"

static void taskfunc(void *param) {
  struct uart *uart = param;
  for (;;) {
    if (uart_read_ready(uart)) {            // Is UART readable?
      uint8_t byte = uart_read_byte(uart);  // Read one byte
      uart_write_byte(uart, byte);          // And write it back
    }
  }
}

int main(void) {
  rtos_init();
  uart_init(UART, UART_TX, UART_RX, 115200);  // UART, UART_TX and UART_RX
  rtos_task_create(taskfunc, UART, 512);      // are defined in mcu.h
  rtos_schedule();
  return 0;
}
