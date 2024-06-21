# Copyright (C) Inuitive, 2013. All rights reserved.
# 
# Author: Konstantin Sinyuk
# This makefile is intended for use with GNU make

###############################################################################
#	Common build defines
###############################################################################

include build/common.mk

TOPDIR=$(shell pwd)
TARGET_NAME=nu4000-rom
TARGET=$(TARGET_NAME).elf

BIN=$(TARGET_NAME).bin
ifdef B0_TEST
LDS=build/$(TARGET_NAME)_b0test.lds
else
LDS=build/$(TARGET_NAME).lds
endif

# Embed version info into file name
HEX_TARGET_NAME=$(TARGET_NAME)_$(SW_MAJOR_VERSION)_$(SW_MINOR_VERSION)_$(SW_BUILD_VERSION)

###############################################################################
#	Include Directories
###############################################################################
# assembler includes
ASINCLUDE=-Iinclude

# common include directories
CINCLUDE=-Iinclude
# usb3 stack include directories
CINCLUDE+=-Idrivers/zoro/include -Idrivers/zoro/usb/include -Idrivers/zoro/usb/usb_ll/include 
# includes for common synopsis drivers
CINCLUDE+=-Idrivers/synopsis/include
# includes for  uart driver
CINCLUDE+=-Idrivers/synopsis/uart/include
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

# Usefull macros
RM=rm -f $(1)


###############################################################################
#	Source tree selection (by find all .c,.S files)
###############################################################################
#exclude secureBoot directory from find command
SRC-PROJ-C := $(shell $(FIND) . -name '*.c' -not -path '*secureBoot*'  )
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
SRC-C := $(SRC-PROJ-C) $(SRC-PROJ_1-C)

SRC-S := $(shell $(FIND) . -name '*.S') 

OBJ-C := $(SRC-C:.c=.o)
OBJ-S := $(SRC-S:.S=.o)

###############################################################################
#	Common make rules
###############################################################################
.PHONY : all clean binary build rebuild ver eeprom

ifeq  "$(OS)" "Windows_NT"
all: $(TARGET) binary 
else
ifdef B0_TEST
all: check_lv $(TARGET) binary hex ver flash 
else
all: check_lv $(TARGET) binary hex ver
endif
endif

###############################################################################
#	Object compilation
###############################################################################
$(OBJ-S) : %.o : %.S
	@echo "  CC        "$<0
	$(V)$(CC) $(ASFLAGS) $(ASINCLUDE) -c -o $@  $<
	
$(OBJ-C) : %.o : %.c
	@echo "  CC        "$<	
	$(V)$(CC) $(CPPFLAGS) $(CINCLUDE) $(CDEFINES) -c -o $@  $<

###############################################################################
#	Target linkage
###############################################################################
$(TARGET): $(OBJ-S) $(OBJ-C) $(LDS)
	$(V)$(LD) $(LDFLAGS) --start-group boot/start.o $(OBJ-C) -lgcc --end-group -o $(TARGET)	

binary:
	@echo "create binary..."
	$(V)$(EL2H) -O binary $(TARGET) $(BIN)
	$(V)$(ELD) -dsx $(TARGET) > $(TARGET_NAME).dis
	$(V)$(NM) -n $(TARGET) | sed -n "s/\(.*\) \([A-Z]\) \(.*\)/\/*\2*\/ \3 = 0x\1;/p" > $(TARGET_NAME).sym.ld	

ifeq  "$(OS)" "Windows_NT"
rebuild: clean all binary 
else
rebuild: clean all binary hex 
endif
	
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


###############################################################################
#	ROM make rules
###############################################################################
ROM_CUT = 16384
ROM_CUT_WIDTH_BYTES = 8
ROM_SIZE_BYTES = $(shell echo '$(ROM_CUT) * $(ROM_CUT_WIDTH_BYTES)' | bc)

hex:
	@echo "bin2rom..."
	$(V)$(shell cat $(BIN) /dev/zero | hexdump -v -n $(ROM_SIZE_BYTES) -e '1/8 "%016x\t" "\n"' > $(HEX_TARGET_NAME).hex)
	$(V)$(shell ln -s $(HEX_TARGET_NAME).hex $(TARGET_NAME).hex)		
	@echo [Image Size] $(TARGET_NAME):
	$(V)$(SZ) -A -x $(TARGET)

ver:
	@echo "create ROM version file (bootrom_ver.hex)..."
	$(V)$(shell expr  "$(SW_MAJOR_VERSION)" + 0  | tr -d '\n' | hexdump -v  -e '/1 "%02X" "\n"' > bootrom_ver.hex)
	$(V)$(shell echo -n "." | hexdump -v  -e '/1 "%02X" "\n"' >> bootrom_ver.hex)
	$(V)$(shell expr  "$(SW_MINOR_VERSION)" + 0  | tr -d '\n' | hexdump -v  -e '/1 "%02X" "\n"' >> bootrom_ver.hex)
	$(V)$(shell echo -n "." | hexdump -v  -e '/1 "%02X" "\n"' >> bootrom_ver.hex)
	$(V)$(shell expr  "$(SW_BUILD_VERSION)" + 0  | tr -d '\n' | hexdump -v  -e '/1 "%02X" "\n"' >> bootrom_ver.hex)

flash:
	@echo "prepare flash bootrom override..."
ifdef B0_TEST
	$(V)$(shell wine util/eeprom-generator.exe -S 0 -P 0 -M burst -T boot -r 0x2032000 -b $(TARGET_NAME).bin -f flash_$(TARGET_NAME).bin 2> /dev/null)
else
	$(V)$(shell wine util/eeprom-generator.exe -S 0 -P 0 -M burst -T boot -r 0x2000000 -b $(TARGET_NAME).bin -f flash_$(TARGET_NAME).bin 2> /dev/null)
endif
	@echo convert to 8-bit RAW hex...
	$(V)$(shell dd if=flash_$(TARGET_NAME).bin bs=1  2> /dev/null | hexdump -v  -e '/1 "%02X" "\n"' > flash_$(TARGET_NAME).hex)
	
check_lv:
ifdef USE_OLD_TOOLCHAIN
	@echo ===========================
	@echo = Use old Linux toolchain =
	@echo ===========================
else
	@echo ==============================
	@echo = Use toolchain 10.3-2021.07 =
	@echo ==============================
endif
	
