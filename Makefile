# Build firmwares for all supported architectures
all:
	make -C examples/blinky ARCH=stm32f1 clean all
	make -C examples/blinky ARCH=stm32f3 clean all
	make -C examples/blinky ARCH=stm32f7 clean all

clean:
	make -C examples/blinky clean
