SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
MCU_FLAGS ?= -mcpu=cortex-m0 -mthumb -mfloat-abi=soft

include $(SELF_DIR)/../../arch/common.mk
