# RTOS for embedded systems

1. Install `arm-none-eabi-gcc` on your system
1. Install `st-link` on your system
3. Run `make -C examples/blinky clean all flash`

That default build targets STM32 NUCLEO-F103RB board. To use any other
STM32F1 board, update `arch/stm32f1/device.h` and repeat (3)
