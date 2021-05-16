# RTOS for embedded systems

1. Install packages `arm-none-eabi-gcc`, `st-link`
2. Run `make -C examples/blinky clean all flash`

To build for a specific board, set `ARCH` and `BOARD` in firmware Makefile.
Possible combinations are listed below:

| ARCH | BOARD | Notes |
| ---- | ----- | ----- |
| stm32f1 | nucleo-f103rb | 72Mhz, 20k RAM, 128k Flash |
| stm32f3 | nucleo-f303k8 | 72MHz, 12k RAM, 64k Flash |
| stm32f746 | nucleo-f746zg | 216MHz, 320k RAM, 1024k Flash |

Example:
```sh
make -C examples/blinky clean all flash ARCH=stm32f746 BOARD=nucleo-f746zg
```

# Roadmap

The plan is to:
- implement a stub systick interrupt handler
- implement a simple task scheduler
- make blinky example use a task rather than a super loop
