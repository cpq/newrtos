ARCH ?= stm32f7
SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
MCU_FLAGS ?= -mcpu=cortex-m7 -mthumb -mfpu=vfpv4

include $(SELF_DIR)/../../arch/common.mk
