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
NU4100_PATH=../include/bootrom-nu4100
ifeq ($(DDR_FREQ),$(DEFAULT_DDR_FREQ))
	TARGET_NAME=bootspl-$(CHIP_ID)
else
	TARGET_NAME=bootspl-$(CHIP_ID)_freq$(DDR_FREQ)
endif



ifndef PLATFORM_NAME
	PLATFORM_NAME=boot0
endif
BOOTROM_SRC_PATH=$(BOOTROM_PATH)/inc
BOOTROM_BIN_PATH=$(BOOTROM_PATH)/bin
COMMON_INCLUDE_PATH=../../../common/include
COMMON_CHIP_INCLUDE_PATH=../../common/

TARGET=$(TARGET_NAME).elf
BIN=$(TARGET_NAME).bin
LDS=build/bootspl.lds

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
CINCLUDE:=-I$(BOOTROM_SRC_PATH)/include
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/synopsis/include
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/synopsis/uart/include
# GME include needed by uart driver
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/inuitive/gme
# includes for timer driver
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/inuitive/timer
# include for ddr
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/inuitive/ddr
# include for usb
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/zoro/include -I$(BOOTROM_SRC_PATH)/drivers/zoro/usb/include -I$(BOOTROM_SRC_PATH)/drivers/zoro/usb/usb_ll/include
# include for xmodem
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/protocol/xmodem

CINCLUDE+=-I$(TOPDIR)/ddr/include
# include platform specific ddr headers
CINCLUDE+=-I$(TOPDIR)/ddr/$(PLATFORM_NAME)

CINCLUDE+=-I$(COMMON_INCLUDE_PATH)

CINCLUDE+=-I$(COMMON_CHIP_INCLUDE_PATH)

CINCLUDE+=-I$(TOPDIR)/flash/

CINCLUDE+=-I$(TOPDIR)/memtest/screening/common


CINCLUDE+=-I$(NU4100_PATH)/include/regs/nu4100/
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/include/regs/nu4000C0/

CINCLUDE+=-I$(BOOTROM_SRC_PATH)/include/asm/
CINCLUDE+=-I$(TOPDIR)/nand/
CINCLUDE+=-I$(TOPDIR)/../src/
CINCLUDE+=-I$(TOPDIR)/../src/pm 
CINCLUDE+= -DPM_DEBUG

CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/secureBoot/Framework/PUBDEFS/incl/
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/secureBoot/SBCommon/SBIF_API/incl/
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/secureBoot/SBLib/SB_API/incl/
CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/secureBoot/SBLib/Config/

CINCLUDE+=-I$(BOOTROM_SRC_PATH)/drivers/secureBoot/SBCommon/Config/ECC256/
###############################################################################
#	Source tree selection (by find all .c,.S files)
###############################################################################
# specific test code
SRC-SPL-C := $(shell find . -maxdepth 1  -name '*.c')
SRC-DDR-C := $(shell find ddr/$(PLATFORM_NAME) -name '*.c')
SRC-FLASH-C := $(shell find flash -name '*.c')
SRC-NAND-C := $(shell find nand -name '*.c')
SRC-MEMTEST-C := $(shell find memtest/screening -name '*.c')
SRC-PM-C := $(shell find pm -name '*.c')

#SRC-BOOTROM-C :=
CDEFINES+=$(EXTCDEFINES)

# specific bootrom code
ifeq ($(BOOT_ID),51)
	SRC-C := $(SRC-SPL-C) $(SRC-DDR-C) $(SRC-FLASH-C) $(SRC-MEMTEST-C) $(SRC-NAND-C)
else ifeq ($(BOOT_ID),500)
	SRC-C := $(SRC-SPL-C) $(SRC-DDR-C) $(SRC-FLASH-C) $(SRC-MEMTEST-C)
else
	SRC-C := $(SRC-SPL-C) $(SRC-DDR-C) $(SRC-FLASH-C) $(SRC-MEMTEST-C) $(SRC-PM-C)
endif

OBJ-C := $(SRC-C:.c=.o)

TARGET_SIZE=$(shell stat -c%s  $(BIN))
pack:
	@echo "create spl loader..."
	$(V)$(shell printf "0: %.8x" $(TARGET_SIZE) | sed -e 's/0: \(..\)\(..\)\(..\)\(..\)/0: \4\3\2\1/' | xxd -r -g0 > tmpfile_size2)
	$(V)$(shell cat tmpfile_size2 $(TARGET_NAME).bin > splloader.bin)


###############################################################################
#	Custom make rules
###############################################################################
all: $(TARGET)


###############################################################################
#	Common make rules
###############################################################################
include build/rules.mk
