# RTOS for embedded systems

A RTOS designed for performance and ease of integration. Uses GCC tools,
and no extra dependencies.

Install packages `arm-none-eabi-gcc`, `st-link`, `openocd` (assuming Ubuntu):
  ```console
  $ apt-get install gcc-arm-none-eabi binutils-arm-none-eabi gdb-arm-none-eabi openocd stlink-tools
  ```
Build example firmware:
  ```console
  $ make -C examples/blinky clean all ARCH=stm32f3 BOARD=nucleo-f303k8
  ```
To flash a firmware, connect your board and run:
  ```console
  $ make -C examples/blinky flash
  ```

Possible combinations for ARCH and BOARD are listed below:

| ARCH | BOARD | Notes |
| ---- | ----- | ----- |
| stm32f1 | nucleo-f103rb | 72Mhz, 20k RAM, 128k Flash |
| stm32f3 | nucleo-f303k8 | 72MHz, 12k RAM, 64k Flash |
| stm32f7 | nucleo-f746zg | 216MHz, 320k RAM, 1024k Flash |

# Debugging

In order to debug, first run `openocd` in one terminal:

```console
$ make -C examples/blinky ARCH=stm32f3 BOARD=nucleo-f303k8 openocd
```

In another terminal, build with `DEBUG=1` and start a GDB debugger:

```console
$ make -C examples/blinky ARCH=stm32f3 BOARD=nucleo-f303k8 DEBUG=1 make clean all gdb
```

By default, GDB breakpoints in `main()`. Tip: use `layout split` GDB command
for a handy source + disassembly display mode.

The `DEBUG=1` adds a `--specs rdimon` linker flag, which enables a semihosting
feature on ARM. That means, all stdio functions like `printf()` will be
retargeted to an openocd console output - useful for a quick printf-debugging.

# Roadmap

The plan is to:
- implement a stub systick interrupt handler
- implement a simple task scheduler
- make blinky example use a task rather than a super loop
