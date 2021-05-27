# RTOS for embedded systems

An open source firmware development framework that includes RTOS and
drivers. Features include:

- Uses GCC + newlib and no extra dependencies
- Unified API across architectures
- RTOS scheduling is round robin with 1ms time slice

# Feature support matrix

|   ARCH  | Clock   | Task | Mutex | Timer | Queue | GPIO | UART | SPI | I2C |
| ------- | ------- | ---- | ----  | ----- | ----- | ---- | ---- | --- | --- |
| stm32f0 | 48Mhz   | yes  | -     | -     | -     | yes  | yes  | -   | -   |
| stm32f1 | 72Mhz   | yes  | -     | -     | -     | yes  | yes  | -   | -   |
| stm32f3 | 72MHz   | yes  | -     | -     | -     | yes  | yes  | -   | -   |
| stm32f7 | 216MHz  | yes  | -     | -     | -     | yes  | -    | -   | -   |

# Build

Install packages `arm-none-eabi-gcc`, `st-link`, `openocd` (assuming Ubuntu):
  ```sh
  $ apt-get install gcc-arm-none-eabi binutils-arm-none-eabi gdb-arm-none-eabi openocd stlink-tools
  ```
Build example firmware:
  ```sh
  $ make -C examples/blinky clean all ARCH=stm32f1
  ```
To flash a firmware, connect your board and run:
  ```sh
  $ make -C examples/blinky flash
  ```

# API Reference

- **Task management**
  - **rtos_init();** - must be the first function called by main()
  - **rtos_task_create(fn, data, stacksize);** - create task
  - **rtos_schedule();** - start task scheduler, do not return
- **Utility**
  - **rtos_msleep(ms);** - sleep `ms` milliseconds
  - **rtos_ram_free();** - return available RAM in bytes
  - **rtos_ram_used();** - return used RAM in bytes
- **GPIO**
  - **PIN(bank, num);** - declare a GPIO pin: `uint16_t pin = PIN('A', 5);`
  - **gpio_init(pin, mode, type, speed, pull, af);** - init GPIO pin
  - **gpio_on(pin);** - set GPIO pin on
  - **gpio_off(pin);** - set GPIO pin off
  - **gpio_toggle(pin);** - toggle GPIO pin
  - **gpio_input(pin);** - (convenience) set input mode on a GPIO pin
  - **gpio_output(pin);** - (convenience) set output mode on a GPIO pin
- **UART**
  - **uart_init(uart, tx_pin, rx_pin, baud);** - initialise UART
  - **uart_write_byte(uart, byte);** - write one byte
  - **uart_write_buf(uart, buf, len);** - write buffer
  - **uart_read_ready(uart);** - return non-0 if UART can be read
  - **uart_read_byte(uart);** - read one byte

# Debugging

In order to debug, first run `openocd` in one terminal:

```sh
$ make -C examples/blinky ARCH=stm32f3 openocd
```

In another terminal, build with `DEBUG=1` and start a GDB debugger:

```sh
$ make -C examples/blinky ARCH=stm32f3 DEBUG=1 make clean all gdb
```

By default, GDB breakpoints in `main()`. Tip: use `layout split` GDB command
for a handy source + disassembly display mode.

The `DEBUG=1` adds a `--specs rdimon` linker flag, which enables a semihosting
feature on ARM. That means, all stdio functions like `printf()` will be
retargeted to an openocd console output - useful for a quick printf-debugging
