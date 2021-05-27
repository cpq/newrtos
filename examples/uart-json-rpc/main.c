// Copyright (c) 2021 Sergey Lyubka
// All rights reserved

#include "mcu.h"
#include "mjson.h"

static void gpio_led_rpc(struct jsonrpc_request *r) {
  int on = 0;
  if (mjson_get_bool(r->params, r->params_len, "$.on", &on)) {
    if (on) gpio_on(LED1);
    if (!on) gpio_off(LED1);
    jsonrpc_return_success(r, "{%Q:%d}", "ram", rtos_ram_free());
  } else {
    jsonrpc_return_error(r, 400, "set 'on' to true or false", NULL);
  }
}

static void math_sum_rpc(struct jsonrpc_request *r) {
  double a, b;
  if (mjson_get_number(r->params, r->params_len, "$[0]", &a) &&
      mjson_get_number(r->params, r->params_len, "$[1]", &b)) {
    jsonrpc_return_success(r, "{%Q:%g}", "result", a + b);
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
  jsonrpc_export("led", gpio_led_rpc);
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
  gpio_output(LED1);
  uart_init(UART, 115200);
#if 0
  for (;;) rtos_msleep(500), gpio_toggle(LED1), uart_write_buf(UART, "hi\n", 3);
#endif
  rtos_task_create(taskfunc, UART, 2048);     // are defined in mcu.h
  rtos_schedule();
  return 0;
}
