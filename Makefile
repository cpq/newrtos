# Build firmwares for all supported architectures
all:
	make -C examples/blinky BOARD=nucleo-f103rb clean all
	make -C examples/blinky BOARD=nucleo-f303k8 clean all
	make -C examples/blinky BOARD=nucleo-f746zg clean all

clean:
	make -C examples/blinky clean
