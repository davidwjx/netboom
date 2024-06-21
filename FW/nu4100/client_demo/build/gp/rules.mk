# Copyright (C) Inuitive, 2014. All rights reserved.
# 
# Author: Konstantin Sinyuk
# This makefile is intended for use with GNU make

###############################################################################
#	Common rules for client build
###############################################################################

###############################################################################
#	Source tree selection from makefile (by find all .c,.S files)
###############################################################################
# Ensure to remove /test folder in the release build
SRC-C := $(SRC-PROJ-C) $(SRC-PROJ_1-C) $(SRC-PROJ_2-C) $(SRC-PROJ_3-C) $(SRC-PROJ_4-C) $(SRC-PROJ_5-C) $(SRC-PROJ_6-C) $(SRC-PROJ_7-C) $(SRC-PROJ_8-C) $(SRC-PROJ_9-C) $(SRC-PROJ_10-C) $(SRC-PROJ_11-C) $(SRC-PROJ_12-C)
SRC-S := $(SRC-PROJ_1-S)
SRC-CPP := $(SRC-PROJ_1-CPP)

OBJ-C := $(SRC-C:.c=.o)
OBJ-S := $(SRC-S:.s=.o)
OBJ-CPP := $(SRC-CPP:.cpp=.o)

###############################################################################
#	Common make rules
###############################################################################
.PHONY : all clean
all: $(TARGET)

###############################################################################
#	Object compilation
###############################################################################
$(OBJ-S) : %.o : %.s
	@echo "  CC        "$<0
	$(V)$(CC) $(ASFLAGS) -c -o $@  $<
	
$(OBJ-C) : %.o : %.c
	@echo "  CC        "$<
	$(V)$(CC) $(CPPFLAGS) $(CINCLUDE) $(CINCLUDE_COMMON) $(CDEFINES) -c -o $@  $<

$(OBJ-CPP) : %.o : %.cpp
	@echo "  CC        "$<
	$(V)$(CC) $(CPPFLAGS) $(CDEFINES) -c -o $@  $<
###############################################################################
#	Target linkage
###############################################################################
$(TARGET): $(OBJ-S) $(OBJ-C) $(OBJ-CPP) 
	@echo linking....
	$(V)$(CC) $(OBJ-C) $(OBJ-CPP) $(OBJ-S) $(PATH_LIBRARIES) $(LDFLAGS) $(IDE_LIBRARIES) -o $(TARGET)
	$(V)$(ELD) -dsx $(TARGET) > $(TARGET_NAME).dis
	$(V)$(call CP, $(TARGET) $(TARGET).nonstripped)
	$(V)$(STRIP) $(TARGET)	

rebuild: clean all
	
clean:
	@echo cleaning objects...	
	$(V)$(call RM, $(OBJ-C) $(OBJ-CPP) $(OBJ-S) *.out)

ifndef VERBOSE
    V := @
else
    V :=
endif
