SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
MCU_FLAGS ?= -mcpu=cortex-m3 -mthumb

include $(SELF_DIR)/../common.mk