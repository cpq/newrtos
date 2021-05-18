 SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
#  MCU_FLAGS ?= -mcpu=cortex-m7 -mthumb -mfloat-abi=softfp -mfpu=fpv4-sp-d16 # -mfpu=fpv5-sp-d16 -mfloat-abi=hard
MCU_FLAGS ?= -mcpu=cortex-m7 -mthumb -mfpu=vfpv4

include $(SELF_DIR)/../common.mk