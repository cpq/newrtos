// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include <stdio.h>

#include "mcu.h"
#include "mjson.h"

static void gpio_write_rpc(struct jsonrpc_request *r) {
  double pin = -1, val = -1;
  mjson_get_number(r->params, r->params_len, "$.pin", &pin);
  mjson_get_number(r->params, r->params_len, "$.val", &val);
  if (pin >= 0 && val >= 0) {
    gpio_init(pin, GPIO_OUT, GPIO_PP, GPIO_SPEED_LOW, GPIO_PULL_NONE, 0);
    if (val) gpio_on(pin);
    if (!val) gpio_off(pin);
    jsonrpc_return_success(r, "true");
  } else {
    jsonrpc_return_error(r, 400, "set pin and val", NULL);
  }
}

static void math_sum_rpc(struct jsonrpc_request *r) {
  double a, b;
  if (mjson_get_number(r->params, r->params_len, "$[0]", &a) &&
      mjson_get_number(r->params, r->params_len, "$[1]", &b)) {
    jsonrpc_return_success(r, "{%Q:%d}", "result", (int) (a + b));
  } else {
    jsonrpc_return_error(r, 400, "pass [NUM1, NUM2]", NULL);
  }
}

static void taskfunc(void *param) {
  struct uart *uart = param;
  char buf[1024];  // UART input buffer
  int len = 0;     // Number of bytes already read

  // Set up JSON-RPC server
  jsonrpc_init(NULL, NULL);
  jsonrpc_export("gpio.write", gpio_write_rpc);
  jsonrpc_export("math.sum", math_sum_rpc);

  for (;;) {
    if (uart_read_ready(uart)) {            // Is UART readable?
      uint8_t byte = uart_read_byte(uart);  // Read one byte
      if (byte == '\n') {                   // Newline? We've got an RPC frame
        char *response = NULL;              // Prepare response
        jsonrpc_process(buf, len, mjson_print_dynamic_buf, &response, NULL);
        if (response != NULL) {
          uart_write_buf(uart, response, strlen(response));
          free(response);
        }
        len = 0;
      } else {
        buf[len++] = byte;                      // Append to the input buffer
        if (len >= (int) sizeof(buf)) len = 0;  // Flush on overflow
      }
    }
  }
}

int main(void) {
  rtos_init();
  uart_init(UART, UART_TX, UART_RX, 115200);  // UART, UART_TX and UART_RX
  rtos_task_create(taskfunc, UART, 2048);     // are defined in mcu.h
  rtos_schedule();
  return 0;
}
