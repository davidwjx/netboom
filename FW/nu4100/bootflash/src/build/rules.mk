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
	$(V)$(shell cat $(BOOTROM_BIN_PATH)/RELEASE/nu4000-rom.sym.ld | egrep -w 'debug_printf|board_timer_init|system_tick_delay|icache_disable|invalidate_icache_all|disable_interrupts|board_usb_init|board_usb_phy_init|xmodem_init|xmodem_rx_file|nu3000_bg_process|board_dbg_uart_init|debug_init|GMEG_setPowerMode' > nu3000-rom.sym.ld)	
	$(V)$(shell cat $(BOOTROM_BIN_PATH)/RELEASE/nu4000-rom.sym.ld | egrep -w 'GMEG_enableClk|GMEG_setFcuClockDiv|GMEG_changeUnitFreq|board_i2c_init|get_board_info|memcpy|memset|system_udelay|board_set_mode|board_gpp_clk_set|get_strap_info|board_spi_init' >> nu3000-rom.sym.ld)	


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
	
array:
	@echo prepare convertion to .h file array ...
	$(V)$(shell xxd -i -u -c 11  $(TARGET_NAME).bin | sed 's/_bin//g' > $(TARGET_NAME).h )	
		
# Verbose mode - display build commands
ifndef VERBOSE
    V := @
else
    V :=
endif
