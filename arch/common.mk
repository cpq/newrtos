PROG ?= firmware
ROOT_PATH = $(realpath $(dir $(lastword $(MAKEFILE_LIST)))/..)
ARCH_PATH = $(ROOT_PATH)/arch/$(ARCH)
BOARD_PATH = $(ARCH_PATH)/boards/$(BOARD)

INCLUDES ?= -I. -I$(ARCH_PATH) -I$(BOARD_PATH) -I$(ROOT_PATH)/include/cmsis
COPT ?= -W -Wall -Werror -Os -g
CFLAGS += $(COPT) $(MCU_FLAGS) -fdata-sections -ffunction-sections $(INCLUDES) $(EXTRA)
LINKFLAGS += $(MCU_FLAGS) -T$(ARCH_PATH)/link.ld -specs=rdimon.specs -lc -lgcc -lrdimon -u _printf_float
# LINKFLAGS += $(MCU_FLAGS) -T$(ARCH_PATH)/link.ld -specs=nosys.specs
SOURCES += $(wildcard src/*.c)
OBJECTS = obj/boot.o $(SOURCES:%.c=obj/%.o)

all: $(PROG).hex

$(PROG).bin: $(PROG).elf
	arm-none-eabi-objcopy -O binary $< $@

$(PROG).hex: $(PROG).bin
	arm-none-eabi-objcopy -I binary -O ihex --change-address 0x8000000 $< $@

$(PROG).elf: $(OBJECTS) $(ARCH_PATH)/link.ld
	arm-none-eabi-gcc $(OBJECTS) $(LINKFLAGS) -o $@
	arm-none-eabi-size -A $@

obj/%.o: %.c
	@mkdir -p $(dir $@)
	arm-none-eabi-gcc $(CFLAGS) -c $< -o $@

obj/boot.o: $(ARCH_PATH)/boot.s
	@mkdir -p $(dir $@)
	arm-none-eabi-as -g --warn --fatal-warnings $(MCU_FLAGS) $< -o $@

flash: $(PROG).bin
	st-flash write $< 0x8000000

openocd:
	openocd -f $(ARCH)/openocd.cfg

ELF ?= $(PROG).elf
gdb:
	arm-none-eabi-gdb \
		-ex 'set confirm off' \
        -ex 'target extended-remote :3333' \
        -ex 'monitor arm semihosting enable' \
        -ex 'monitor reset halt' \
        -ex 'load' \
        -ex 'monitor reset init' \
        -ex '$(GDBCMD)' \
        -ex 'r' \
        $(ELF)

clean:
	@rm -rf *.{bin,elf,map,lst,tgz,zip,hex} obj
