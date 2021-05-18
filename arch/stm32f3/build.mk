SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
MCU_FLAGS ?= -mcpu=cortex-m4 -mthumb

include $(SELF_DIR)/../common.mk
