# RTOS for embedded systems

1. Install `arm-none-eabi-gcc`
1. Install `st-link`
3. Run `make -C examples/blinky clean all flash`

The plan is to:
- implement a stub systick interrupt handler
- implement a simple task scheduler
- make blinky example use a task rather than a super loop

To build for a specific board, set `ARCH` and `BOARD` in firmware Makefile.
Possible combinations are listed below:

# Supported hardware

| ARCH | BOARD | Notes |
| ---- | ----- | ----- |
| stm32f1 | nucleo-f103rb | 72Mhz, 20k RAM, 128k Flash |
| stm32f3 | nucleo-f303k8 | 72MHz, 12k RAM, 64k Flash |
