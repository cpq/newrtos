PROG ?= firmware
ARCH ?= stm32f1
ROOT = $(realpath $(CURDIR)/../..)
ARCH_PATH = $(ROOT)/arch/$(ARCH)

ifeq "$(ARCH)" "stm32f1"
MCU = -mcpu=cortex-m3 -mthumb -mfloat-abi=soft
else ifeq "$(ARCH)" "stm32f7"
MCU = -mcpu=cortex-m7 -mthumb -mfpu=fpv5-sp-d16 -mfloat-abi=hard
endif

INCS ?= -I$(ARCH_PATH) -I.
COPT ?= -W -Wall -Werror -Os -g
CFLAGS += $(COPT) $(MCU) -fdata-sections -ffunction-sections $(INCS) $(EXTRA)
LDFLAGS += $(MCU) -specs=nano.specs -Tobj/link.ld -nostartfiles -lgcc
SRCS += $(wildcard src/*.c)
OBJS = obj/boot.o $(SRCS:%.c=obj/%.o) # ORDER MATTERS: boot first (interrupt vector table is there)

all: $(PROG).hex

$(PROG).bin: $(PROG).elf
	arm-none-eabi-objcopy -O binary $< $@

$(PROG).hex: $(PROG).bin
	arm-none-eabi-objcopy -I binary -O ihex --change-address 0x8000000 $< $@

obj/link.ld: $(ARCH_PATH)/link.ld
	arm-none-eabi-cpp -P -imacros $(ARCH_PATH)/device.h $< > $@

$(PROG).elf: $(OBJS) obj/link.ld
	arm-none-eabi-gcc $(OBJS) $(LDFLAGS) -o $@

obj/%.o: %.c
	@mkdir -p $(dir $@)
	arm-none-eabi-gcc $(CFLAGS) -c $< -o $@

obj/boot.o:
	@mkdir -p $(dir $@)
	arm-none-eabi-as --warn --fatal-warnings $(MCU) $(ARCH_PATH)/boot.s -o $@

flash: $(PROG).bin
	st-flash --reset write $< 0x8000000

# Before running this command, run `st-util` in a separate terminal
gdb: $(PROG).elf
	arm-none-eabi-gdb \
		-ex 'set confirm off' \
        -ex 'target extended-remote :4242' \
        -ex 'monitor reset halt' \
        -ex 'monitor reset init' \
        $<

clean:
	@rm -rf *.{bin,elf,map,lst,tgz,zip,hex} obj
