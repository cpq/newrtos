 SELF_DIR = $(dir $(lastword $(MAKEFILE_LIST)))
 MCU_FLAGS ?= -mcpu=cortex-m7 -mthumb #-mfpu=fpv5-sp-d16 -mfloat-abi=hard

 include $(SELF_DIR)/../common.mk