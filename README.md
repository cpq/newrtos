# RTOS for embedded systems

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
| stm32f303x8 | nucleo-f303k8 | 72MHz, 12k RAM, 64k Flash |
| stm32f746 | nucleo-f746zg | 216MHz, 320k RAM, 1024k Flash |

# Debugging

In order to debug, first run `openocd` in one terminal:

```console
$ make -C examples/blinky ARCH=stm32f3 BOARD=nucleo-f303k8 openocd
```

In another terminal, build with `DEBUG=1` and start a debugger:

```console
$ make -C examples/blinky ARCH=stm32f3 BOARD=nucleo-f303k8 DEBUG=1 make clean all gdb
```

# Roadmap

The plan is to:
- implement a stub systick interrupt handler
- implement a simple task scheduler
- make blinky example use a task rather than a super loop
