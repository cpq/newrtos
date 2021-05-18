PROG ?= firmware
ROOT_PATH = $(realpath $(dir $(lastword $(MAKEFILE_LIST)))/..)
ARCH_PATH = $(ROOT_PATH)/arch/$(ARCH)
BOARD_PATH = $(ARCH_PATH)/boards/$(BOARD)
TOOLCHAIN ?= arm-none-eabi
OBJ_PATH = obj

INCLUDES ?= -I. -I$(ROOT_PATH) -I$(ROOT_PATH)/arch -I$(ARCH_PATH) -I$(BOARD_PATH) -I$(ROOT_PATH)/arch/cmsis
COPT ?= -W -Wall -Werror -Os -g
CFLAGS += $(COPT) $(MCU_FLAGS) -fdata-sections -ffunction-sections $(INCLUDES) $(EXTRA)

LINKFLAGS ?= $(MCU_FLAGS) -T$(ARCH_PATH)/link.ld -T$(ROOT_PATH)/arch/common.ld -specs=nano.specs
ifeq "$(DEBUG)" "1"
LINKFLAGS += -specs=rdimon.specs -lrdimon
CFLAGS += -D'DEBUG(x)=printf x'
else
CFLAGS += -D'DEBUG(x)='
endif

SOURCES += $(ROOT_PATH)/rtos.c
OBJECTS = $(OBJ_PATH)/boot.o $(SOURCES:%.c=$(OBJ_PATH)/%.o)

all: $(OBJ_PATH)/$(PROG).hex

$(OBJ_PATH)/$(PROG).bin: $(OBJ_PATH)/$(PROG).elf
	$(TOOLCHAIN)-objcopy -O binary $< $@

$(OBJ_PATH)/$(PROG).hex: $(OBJ_PATH)/$(PROG).bin
	$(TOOLCHAIN)-objcopy -I binary -O ihex --change-address 0x8000000 $< $@

$(OBJ_PATH)/$(PROG).elf: $(OBJECTS) $(ARCH_PATH)/link.ld
	$(TOOLCHAIN)-gcc $(OBJECTS) $(LINKFLAGS) -o $@
	$(TOOLCHAIN)-size $@

$(OBJ_PATH)/%.o: %.c
	@mkdir -p $(dir $@)
	$(TOOLCHAIN)-gcc $(CFLAGS) -c $< -o $@

$(OBJ_PATH)/boot.o: $(ROOT_PATH)/arch/boot_arm.s
	@mkdir -p $(dir $@)
	$(TOOLCHAIN)-as -g --warn --fatal-warnings $(MCU_FLAGS) $< -o $@

flash: $(OBJ_PATH)/$(PROG).bin
	st-flash --reset write $< 0x8000000

openocd:
	openocd -f $(ARCH_PATH)/openocd.cfg

ELF ?= $(OBJ_PATH)/$(PROG).elf
GDBCMD ?= b main
gdb:
	$(TOOLCHAIN)-gdb \
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
	@rm -rf *.{bin,elf,map,lst,tgz,zip,hex} $(OBJ_PATH)
