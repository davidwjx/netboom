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
CHIP_ID_U=nu4000C0
BOOTROM_PATH=../include/bootrom-$(CHIP_ID)/
TARGET_NAME=bootfix_loader_nu4100
BOOTROM_SRC_PATH=$(BOOTROM_PATH)/inc/
BOOTROM_BIN_PATH=$(BOOTROM_PATH)/bin/
TARGET=$(TARGET_NAME).elf
BIN=$(TARGET_NAME).bin
LDS=build/bootfix_loader.lds

###############################################################################
#	build defines
###############################################################################
include build/common.mk

###############################################################################
#	Common include directories
###############################################################################
# Common assembler includes 
ASINCLUDE=-I$(BOOTROM_SRC_PATH)/include
# Common C includes 
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/include
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/include/regs/$(CHIP_ID_U)/
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/synopsis/include
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/synopsis/uart/include
# GME include needed by uart driver
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/inuitive/gme
# includes for timer driver
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/inuitive/timer
# include for ddr
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/inuitive/ddr
# include for flash
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/inuitive/flash
# include for usb
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/zoro/include -I$(BOOTROM_SRC_PATH)/drivers/zoro/usb/include -I$(BOOTROM_SRC_PATH)/drivers/zoro/usb/usb_ll/include 

CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/inuitive/storage/include/  
CINCLUDE+= -I../../common/include
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/secureBoot/Framework/PUBDEFS/incl/
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/secureBoot/SBCommon/SBIF_API/incl/
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/secureBoot/SBLib/SB_API/incl/
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/secureBoot/SBLib/Config/

CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/secureBoot/SBCommon/Config/ECC256/

###############################################################################
#	Source tree selection (by find all .c,.S files)
###############################################################################
# specific test code
SRC-LOADER-C := $(shell find drivers main  -name '*.c')
#SRC-BOOTROM-C := 

# specific bootrom code
SRC-C := $(SRC-LOADER-C)
OBJ-C := $(SRC-C:.c=.o)

###############################################################################
#	Custom make rules
###############################################################################
all: $(TARGET)  

###############################################################################
#	Common make rules
###############################################################################
include build/rules.mk
ifdef SLOW_SPI_BUILD
	CDEFINES += -DSLOW_SPI
endif 
