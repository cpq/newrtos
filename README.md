# RTOS for embedded systems

1. Install `arm-none-eabi-gcc`
1. Install `st-link`
3. Run `make -C examples/blinky clean all flash`

That default build targets STM32 NUCLEO-F103RB board. To use any other
STM32F1 board, update `arch/stm32f1/device.h` and repeat (3)

Currently there is no actual RTOS implementation - but the bare-metal
build infrastructure works fine. 

The plan is to:
- implement a stub systick interrupt handler
- implement a simple task scheduler
- make blinky example use a task rather than a super loop
