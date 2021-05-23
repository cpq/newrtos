# RTOS for embedded systems

An open source firmware development framework that includes RTOS and
drivers. Features include:

- Uses GCC toolchain and no extra dependencies
- All RTOS tasks are of equal priority, with 1ms context switch interval

# Build

Install packages `arm-none-eabi-gcc`, `st-link`, `openocd` (assuming Ubuntu):
  ```console
  $ apt-get install gcc-arm-none-eabi binutils-arm-none-eabi gdb-arm-none-eabi openocd stlink-tools
  ```
Build example firmware:
  ```console
  $ make -C examples/blinky clean all ARCH=stm32f1
  ```
To flash a firmware, connect your board and run:
  ```console
  $ make -C examples/blinky flash
  ```

Possible values for ARCH are listed below:

| ARCH | Notes |
| ----- | ----- |
| stm32f1 | 72Mhz, 20k RAM, 128k Flash |
| stm32f3 | 72MHz, 12k RAM, 64k Flash |
| stm32f7 | 216MHz, 320k RAM, 1024k Flash |

# Debugging

In order to debug, first run `openocd` in one terminal:

```console
$ make -C examples/blinky ARCH=stm32f3 openocd
```

In another terminal, build with `DEBUG=1` and start a GDB debugger:

```console
$ make -C examples/blinky ARCH=stm32f3 DEBUG=1 make clean all gdb
```

By default, GDB breakpoints in `main()`. Tip: use `layout split` GDB command
for a handy source + disassembly display mode.

The `DEBUG=1` adds a `--specs rdimon` linker flag, which enables a semihosting
feature on ARM. That means, all stdio functions like `printf()` will be
retargeted to an openocd console output - useful for a quick printf-debugging.

# API Reference

- **rtos_init(void);** - must be the first function called by main()
- **rtos_task_create(fn, data, stacksize);** - create task
- **rtos_schedule(void);** - start task scheduler, do not return
- **rtos_msleep(ms);** - sleep `ms` milliseconds

This simple GPIO API is just for demo LED blinking

- **uint16_t led = PIN('A', 5);** - declare a GPIO pin
- **gpio_on(pin);** - set GPIO pin on
- **gpio_off(pin);** - set GPIO pin off
- **gpio_toggle(pin);** - toggle GPIO pin
