# Copyright (C) Inuitive, 2014. All rights reserved.
# 
# Author: Konstantin Sinyuk
# This makefile is intended for use with GNU make

###############################################################################
#	Common build defines
###############################################################################
include ../../build/gp/common.mk

# common include directories
CINCLUDE= 										\
-I../common										\

# Usefull macros
RM=rm -f $(1)

###############################################################################
#	Source tree selection (by find all .c,.S files)
###############################################################################
SRC-PROJ_1-C := $(shell find . -name '*.c')

# Ensure to remove /test folder in the release build
SRC-C := $(SRC-PROJ-C) $(SRC-PROJ_1-C) 
SRC-S := $(SRC-PROJ_1-S)

OBJ-C := $(SRC-C:.c=.o)
OBJ-S := $(SRC-S:.s=.o)

###############################################################################
#	Common make rules
###############################################################################
include ../../build/gp/rules.mk