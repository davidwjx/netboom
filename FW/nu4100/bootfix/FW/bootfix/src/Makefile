# Copyright (C) Inuitive, 2013. All rights reserved.
# 
# Author: Konstantin Sinyuk
# This makefile is intended for use with GNU make

###############################################################################
#	Common build defines
###############################################################################

ifndef PLATFORM_NAME
	PLATFORM_NAME=boot0
endif


include build/common.mk
ifdef SLOW_SPI_BUILD
	CDEFINES += -DSLOW_SPI -DBOARD_$(CHIP_ID)
endif 
CDEFINES += -DBOARD_$(CHIP_ID) -DUSB_TYPE$(USB_TYPE) $(EXTCDEFINES)

TOPDIR=$(shell pwd)
TARGET_NAME=bootfix
TARGET=$(TARGET_NAME).elf
BIN=$(TARGET_NAME).bin
LDS=build/$(TARGET_NAME).lds
COMMON_INCLUDE_PATH=../../common/include
APP_SRC_PATH=../../app

BOOTFIX_LOADER_BIN=../../bootfix_loader/src/bootfix_loader_nu4100.bin
BOOTFIX_BIN=../src/bootfix.bin
BOOTFIX_LOADER_SIZE=$(shell stat -c%s  $(BOOTFIX_LOADER_BIN))

###############################################################################
#	Include Directories
###############################################################################
# assembler includes
ASINCLUDE=-Iinclude

# common include directories
CINCLUDE=-Iinclude
#version.h directory
CINCLUDE+=-I../build/version
# usb3 stack include directories
CINCLUDE+=-Idrivers/zoro/include -Idrivers/zoro/usb/include -Idrivers/zoro/usb/usb_ll/include 
# includes for common synopsis drivers
CINCLUDE+=-Idrivers/synopsis/include
# includes for  uart driver
CINCLUDE+=-Idrivers/synopsis/uart/include
# includes for i2c driver
CINCLUDE+=-Idrivers/synopsis/i2c/include
# includes for timer driver
CINCLUDE+=-Idrivers/inuitive/timer
# includes for interrupt controller driver
CINCLUDE+=-Idrivers/inuitive/ictl
# includes for GME driver
CINCLUDE+=-Idrivers/inuitive/gme
# includes for STORAGE driver
CINCLUDE+=-Idrivers/inuitive/storage/include
# includes for SPI FLASH driver
CINCLUDE+=-Idrivers/inuitive/flash
# includes for Secure boot driver
CINCLUDE+=-Idrivers/secureBoot/SBCommon/Config/ECC256 
CINCLUDE+=-Idrivers/secureBoot/SBLib/Config 
CINCLUDE+=-Idrivers/secureBoot/Config
CINCLUDE+=-Idrivers/secureBoot/SBCommon/Config 
CINCLUDE+=-Idrivers/secureBoot/Framework/PUBDEFS/incl 
CINCLUDE+=-Idrivers/secureBoot/Framework/IMPLDEFS/incl
CINCLUDE+=-Idrivers/secureBoot/Framework/CLIB/incl 
CINCLUDE+=-Idrivers/secureBoot/SBLib/SB_API/incl 
CINCLUDE+=-Idrivers/secureBoot/SBLib/SB_HYBRID/src 
CINCLUDE+=-Idrivers/secureBoot/SBCommon/SBIF_API/incl 
CINCLUDE+=-Idrivers/secureBoot/SBCommon/SBIF_TOOLS/incl 
CINCLUDE+=-Idrivers/secureBoot/SBCommon/SB_SW_CRYPTO/incl 
CINCLUDE+=-Idrivers/secureBoot/Framework/CLIB/incl 
CINCLUDE+=-Idrivers/secureBoot/SBCommon/SB_AES_API/incl 
CINCLUDE+=-Idrivers/secureBoot/micro-ecc  
CINCLUDE+=-Idrivers/secureBoot/Kit/DriverFramework/v4_safezone/CLib_Abstraction/incl
CINCLUDE+=-Idrivers/secureBoot/Kit/DriverFramework/v4/Device_API/incl 
CINCLUDE+=-Idrivers/secureBoot/Kit/DriverFramework/v4/DMAResource_API/incl 
CINCLUDE+=-Idrivers/secureBoot/Integration/DMARes_Record/incl 
CINCLUDE+=-Idrivers/secureBoot/Integration/DMARes_Record_bare/incl
CINCLUDE+=-Idrivers/secureBoot/Integration/UMDevXS/UserPart/incl
CINCLUDE+=-Idrivers/secureBoot/Kit/EIP28_SL/incl
CINCLUDE+=-Idrivers/secureBoot/Kit/EIP93_SL/incl
CINCLUDE+=-Idrivers/secureBoot/Kit/EIP123_SL/incl 
CINCLUDE+=-Idrivers/secureBoot/Kit/EIP123_CM_Tokens/incl 
CINCLUDE+=-Idrivers/secureBoot/Kit/EIP130/MailboxControl/incl
CINCLUDE+=-Idrivers/secureBoot/Kit/EIP130/MailboxControl/src
CINCLUDE+=-Idrivers/secureBoot/Kit/EIP130/TokenHelper/incl 
CINCLUDE+=-Idrivers/secureBoot/Kit/EIP130/TokenHelper/src 
CINCLUDE+=-Idrivers/secureBoot/Kit/RingHelper/incl 
CINCLUDE+=-Idrivers/secureBoot/Kit/Log/incl
CINCLUDE+=-Idrivers/secureBoot/Kit/Log/src/safezone
CINCLUDE+=-Idrivers/secureBoot/verification

# includes for xmodem protocol
CINCLUDE+=-Iprotocol/xmodem
# includes for dft code
CINCLUDE+=-Idft
CINCLUDE+=-I$(COMMON_INCLUDE_PATH)
CINCLUDE+=-Iinclude/regs/nu4000C0/

# Usefull macros
RM=rm -f $(1)


###############################################################################
#	Source tree selection (by find all .c,.S files)
###############################################################################
SRC-BOOTFIX-C := $(shell find . -name '*.c' -not -path "./drivers/secureBoot/*")
SRC-PROJ_1-C =  drivers/secureBoot/SBCommon/SBIF/src/sbif_attributes.c \
		drivers/secureBoot/SBLib/SB_SW/src/sbsw_api_decrypt.c \
    		drivers/secureBoot/SBLib/SB_SW/src/sbsw_sbif_tools_convert.c \
	    	drivers/secureBoot/SBLib/SB_SW/src/sbsw_hw_init.c \
    		drivers/secureBoot/SBLib/SB_SW/src/sbsw_sw_aes_if.c \
	    	drivers/secureBoot/SBLib/SB_SW/src/sbsw_sw_ecdsa_verify.c \
	    	drivers/secureBoot/SBLib/SB_SW/src/sbsw_sw_publicdata.c \
	    	drivers/secureBoot/Framework/CLIB/src/c_lib.c \
	    	drivers/secureBoot/SBCommon/SB_SW_CRYPTO/src/sbcr_key.c \
	    	drivers/secureBoot/SBCommon/SB_SW_CRYPTO/src/rijndael.c \
		drivers/secureBoot/SBCommon/SB_SW_CRYPTO/src/minsha_init.c \
    		drivers/secureBoot/SBCommon/SB_SW_CRYPTO/src/minsha_update.c \
    		drivers/secureBoot/SBCommon/SB_SW_CRYPTO/src/minsha_core.c \
    		drivers/secureBoot/micro-ecc/uECC.c \
    		drivers/secureBoot/SBCommon/SB_SW_CRYPTO/src/sb_sw_ecdsa256.c \
		drivers/secureBoot/verification/verify.c
SRC-C 	  := $(SRC-BOOTFIX-C) $(SRC-PROJ_1-C)
SRC-S := $(shell find . -name '*.S') 

OBJ-C := $(SRC-C:.c=.o)
OBJ-S := $(SRC-S:.S=.o)



###############################################################################
#	Common make rules
###############################################################################
.PHONY : all clean binary build rebuild pack

all: $(TARGET)

###############################################################################
#	Object compilation
###############################################################################
$(OBJ-S) : %.o : %.S
	@echo "  CC        "$<0
	$(V)$(CC) $(ASFLAGS) $(ASINCLUDE) -c -o $@  $<
	
$(OBJ-C) : %.o : %.c
	@echo "CC      $(CC)  "$<	
	$(V)$(CC) $(CPPFLAGS) $(CINCLUDE) $(CDEFINES) $(CFLAGS) -c -o $@  $<

###############################################################################
#	Target linkage
###############################################################################
$(TARGET): $(OBJ-S) $(OBJ-C) $(LDS)
	$(V)$(LD) $(LDFLAGS) --start-group boot/start.o $(OBJ-C) -lgcc --end-group -o $(TARGET)	

binary:
	$(V)$(EL2H) -O binary $(TARGET) $(BIN)
	$(V)$(ELD) -dsx $(TARGET) > $(TARGET_NAME).dis
	$(V)$(NM) -n $(TARGET) | sed -n "s/\(.*\) \([A-Z]\) \(.*\)/\/*\2*\/ \3 = 0x\1;/p" > $(TARGET_NAME).sym.ld
	#$(V)$(call CP, $(BIN) $(TARGET).nonstripped)
	#$(V)$(STRIP) $(TARGET)

TARGET_SIZE=$(shell stat -c%s  $(BIN))	
pack:
	@echo "add bootfix_loader to bootfix binary..."	
	$(V)$(shell printf "0: %.8x" $(BOOTFIX_LOADER_SIZE) | sed -e 's/0: \(..\)\(..\)\(..\)\(..\)/0: \4\3\2\1/' | xxd -r -g0 > tmpfile_size1)	
	$(V)$(shell printf "0: %.8x" $(TARGET_SIZE) | sed -e 's/0: \(..\)\(..\)\(..\)\(..\)/0: \4\3\2\1/' | xxd -r -g0 > tmpfile_size2)		
	$(V)$(shell cat tmpfile_size1 $(BOOTFIX_LOADER_BIN) tmpfile_size2 $(TARGET_NAME).bin > flash_$(TARGET_NAME).bin)	
	$(V)$(shell rm tmpfile_size1 tmpfile_size2)

rebuild: clean all binary pack
	
clean:
	@echo cleaning objects...	
	$(V)$(call RM, $(OBJ-C))
	$(V)$(call RM, $(OBJ-S))
	$(V)$(call RM, *.hex)
	$(V)$(call RM, *.bin)
	$(V)$(call RM, *.elf)
	$(V)$(call RM, *.map)
	$(V)$(call RM, *.dis)
	$(V)$(call RM, *.sym.ld)
	$(V)$(call RM, *.a)

# Verbose mode - display build commands
ifndef VERBOSE
    V := @
else
    V :=
endif
