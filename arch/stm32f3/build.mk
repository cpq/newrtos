SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
MCU_FLAGS ?= -mfloat-abi=soft -mcpu=cortex-m4 -mthumb

include $(SELF_DIR)/../../arch/common.mk
