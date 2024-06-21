###############################################################################
# Copyright (C) Inuitive, 2013. All rights reserved.
# 
# This makefile is intended for use with GNU make
#
# rules.mk - lower level makefiles include this file. Includes common rules 
#            for making targets.
###############################################################################

###############################################################################
#	Common make rules
###############################################################################
# Usefull macros
RM=rm -f $(1)
CP=cp -aLv $(1) $(2)

###############################################################################
#	Object compilation
###############################################################################	
$(OBJ-C) : %.o : %.c
	@echo "  CC        "$<	
	$(V)$(CC) $(CPPFLAGS) $(CINCLUDE) $(CDEFINES) -c -o $@  $<

.PHONY : all clean binary build rebuild md5 hex hex_8bit cram_hex eeprom bootrom_include 

###############################################################################
#	Target linkage
###############################################################################
$(TARGET): $(OBJ-C) $(LDS)
	$(V)$(LD) $(LDFLAGS) --start-group $(OBJ-C) -lgcc --end-group -o $(TARGET)	

bootrom_include:
	$(V)$(shell cat $(BOOTROM_BIN_PATH)/RELEASE/nu4000-rom.sym.ld | egrep -w 'GMEG_enableClk|board_clks_set|debug_printf|debug_writeb|board_timer_init|system_tick_delay|icache_disable|invalidate_icache_all|disable_interrupts|board_usb_init|board_usb_phy_init|xmodem_init|xmodem_rx_file|nu3000_bg_process|board_dbg_uart_init|debug_init|GMEG_setPowerMode|board_bootstrap_get|GMEG_configSysPll|GMEG_getSysPllConfig|GMEG_setGppClockDiv' > nu3000-rom.sym.ld)
	$(V)$(shell cat $(BOOTROM_BIN_PATH)/RELEASE/nu4000-rom.sym.ld |  egrep -w 'uart_boot_init|SB_ECDSA_Image_DecryptWithKeyAndVerify|disable_caches|GMEG_lockCpuPll|GMEG_setGppClockSrc|GMEG_setSysClockSrc|GMEG_setFcuClockDiv|GMEG_setSysClockDiv|GMEG_changeUnitFreq|GMEG_lockSysPll|memcpy|memset|get_board_info|board_set_mode|board_gpp_clk_set|storage_flash_init|create_descriptors_customization|board_boot_uart_init|uart_read|uart_write' >> nu3000-rom.sym.ld)

binary:
	$(V)$(EL2H) -O binary $(TARGET) $(BIN)
	$(V)$(ELD) -dsx $(TARGET) > $(TARGET_NAME).dis
	$(V)$(ELD) -dsx $(TARGET) > $(TARGET_NAME).dis
	@echo [Image Size] $(TARGET_NAME):
	$(V)$(SZ) -A -x $(TARGET)

rebuild: clean all
	
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
	$(V)$(call RM, *.md5)
	$(V)$(call RM, *.a)	

###############################################################################
#	ROM make rules
###############################################################################
ROM_CUT = 16384
ROM_CUT_WIDTH_BYTES = 8
ROM_SIZE_BYTES = $(shell echo '$(ROM_CUT) * $(ROM_CUT_WIDTH_BYTES)' | bc)

hex:
	@echo prepare 64-bit ROM hex...		
	$(V)$(shell cat $(TARGET_NAME).bin /dev/zero | hexdump -v -n $(ROM_SIZE_BYTES) -e '1/8 "%016x\t" "\n"' > $(TARGET_NAME).hex)
	
hex_8bit:
	@echo prepare 8-bit hex...		
	$(V)$(shell dd if=$(TARGET_NAME).bin bs=1  2> /dev/null | hexdump -v  -e '/1 "%02X" "\n"' > $(TARGET_NAME)_8bit.hex)	

cram_hex:
	@echo prepare CRAM hex...			
	$(V)$(shell ./build/cram_split.sh $(TARGET_NAME).hex)
	
eeprom:
	@echo prepare eeprom boot...
	$(V)$(shell ./util/eeprom-generator -S 128 -P 256 -M burst -T boot -r 0x1000000 -b $(TARGET_NAME).bin -f eeprom_$(TARGET_NAME).bin 2> /dev/null)	
	@echo convert to 8-bit RAW hex...
	$(V)$(shell dd if=eeprom_$(TARGET_NAME).bin bs=1  2> /dev/null | hexdump -v  -e '/1 "%02X" "\n"' > eeprom_$(TARGET_NAME).hex)
		
# Verbose mode - display build commands
ifndef VERBOSE
    V := @
else
    V :=
endif
