###############################################################################
# Copyright (C) Inuitive, 2013. All rights reserved.
# 
# Author: Konstantin Sinyuk
# This makefile is intended for use with GNU make
###############################################################################

###############################################################################
#	Common build defines
###############################################################################
TOPDIR=$(shell pwd)

BOOTROM_PATH=../include/bootrom-$(CHIP_ID)
TARGET_NAME=bootflash_$(CHIP_ID)
BOOTROM_SRC_PATH=$(BOOTROM_PATH)/inc
BOOTROM_BIN_PATH=$(BOOTROM_PATH)/bin
COMMON_INCLUDE_PATH=../common/include
TARGET=$(TARGET_NAME).elf
BIN=$(TARGET_NAME).bin
LDS=build/bootflash.lds

###############################################################################
#	build defines
###############################################################################
include build/common.mk

###############################################################################
#	Common include directories
###############################################################################
# Common assembler includes 
ASINCLUDE=-I$(BOOTROM_SRC_PATH)/include
ASINCLUDE=-I$(BOOTROM_SRC_PATH)/include/inc
# Common C includes 
CINCLUDE:=-I$(BOOTROM_SRC_PATH)/include
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/synopsis/include
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/synopsis/uart/include
# GME include needed by uart driver
# include for usb
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/zoro/include -I$(BOOTROM_SRC_PATH)/drivers/zoro/usb/include -I$(BOOTROM_SRC_PATH)/drivers/zoro/usb/usb_ll/include 

CINCLUDE+=-I$(COMMON_INCLUDE_PATH)
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/include/regs/nu4000C0/
CINCLUDE+=-I$(TOPDIR)/nand/
CINCLUDE+=-I$(TOPDIR)/flash/

CINCLUDE+=-I$(BOOTROM_SRC_PATH)/include/regs/

CINCLUDE+=-I$(BOOTROM_SRC_PATH)/include/asm/

###############################################################################
#	Source tree selection (by find all .c,.S files)
###############################################################################
# specific test code
SRC-SPL-C := $(shell find main  -name '*.c')
SRC-FLASH-C := $(shell find flash -name '*.c')
SRC-NAND-C := $(shell find nand -name '*.c')
#SRC-BOOTROM-C := 

# specific bootrom code
SRC-C := $(SRC-SPL-C) $(SRC-FLASH-C)  $(SRC-NAND-C)
OBJ-C := $(SRC-C:.c=.o)

###############################################################################
#	Custom make rules
###############################################################################
all: bootrom_include $(TARGET) binary array

###############################################################################
#	Common make rules
###############################################################################
include build/rules.mk
