# Copyright (C) Inuitive, 2013. All rights reserved.
# 
# Author: Konstantin Sinyuk
# This makefile is intended for use with GNU make

###############################################################################
#	Common build defines
###############################################################################

SW_VERSION=0x00000000
TOPDIR=$(shell pwd)
TARGET_NAME=SandBox_4100
TARGET_BIN=$(TARGET_NAME).out
TARGET_LIB=lib$(TARGET_NAME).a
BIN=$(TARGET_NAME).bin

###############################################################################
#	ARM cross-compiler selection
###############################################################################
TARGET_GP_COMPILER_PATH ?= /opt/inuitive/toolchain/linaro/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf
TARGET_GP_COMPILER_BIN_PATH ?= $(TARGET_GP_COMPILER_PATH)/bin
TARGET_GP_COMPILER_LIB_PATH ?= $(TARGET_GP_COMPILER_PATH)/arm-linux-gnueabihf/libc/lib/arm-linux-gnueabihf
TARGET_GP_COMPILER_INCLUDE_PATH ?= $(TARGET_GP_COMPILER_PATH)/lib/gcc/arm-linux-gnueabihf/4.9.4/include/
TARGET_GP_COMPILER_PREFIX ?= arm-linux-gnueabihf-

PROJECT_LOC :=../../../
###############################################################################
#	Cross-compiler common defines
###############################################################################
CROSS_COMPILE=$(TARGET_GP_COMPILER_BIN_PATH)/$(TARGET_GP_COMPILER_PREFIX)
CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)as
AR=$(CROSS_COMPILE)ar
LD=$(CROSS_COMPILE)ld
ELD=$(CROSS_COMPILE)objdump
EL2H=$(CROSS_COMPILE)objcopy
SZ=$(CROSS_COMPILE)size
NM=$(CROSS_COMPILE)nm
STRIP=$(CROSS_COMPILE)strip


TARGET_LIB2=../../../target/gp/prj/make/libinu_target
LIB_UVC=../../../../../3rd_party/libguvc-main/libguvc/lib/.libs
INC_UVC=../../../../../3rd_party/libguvc-main/libguvc/lib/
SANDBOX_LIB2=SandBoxFWLib/libsandbox_fw_lib
LIBCAM_ENGINE_LIB2=../../../target/gp/src/hw/isp_mngr/lib/libcam_engine

###############################################################################
#	Build Environment defines
###############################################################################


###############################################################################
#	Compialtion Flags
###############################################################################
IDE_LIBRARIES = -static $(SANDBOX_LIB2).a $(TARGET_LIB2).a -pthread -lrt -lm -lsupc++ -lstdc++ -lguvc
PATH_LIBRARIES = -L $(TARGET_GP_COMPILER_LIB_PATH) -L . -L $(LIB_UVC)
IDE_LIBRARIES += -lSecureBoot
PATH_LIBRARIES += -L ${PROJECT_LOC}/../../3rd_party/secureBoot/

# common gcc flags for s file builds
ASFLAGS=-g -O0 -Wall -mcpu=cortex-a5

# common gcc flags for c file builds
CPPFLAGS=-g -O3 -Wall -Wextra -mcpu=cortex-a5 -mfpu=neon 
          
# common include directories
CINCLUDE=-I$(TARGET_GP_COMPILER_INCLUDE_PATH)\
-I${PROJECT_LOC}/api\
-I${PROJECT_LOC}/api/common\
-I${PROJECT_LOC}/api/common/graph\
-I${PROJECT_LOC}/api/client\
-I${PROJECT_LOC}/api/host\
-I${PROJECT_LOC}/../../common/include\
-I${PROJECT_LOC}/../../common/version\
-I${PROJECT_LOC}/../common\
-I$(INC_UVC)

# defines for u-boot originated code
CDEFINES=-D__ARM__ -D__KERNEL__  -D__linux__ -D_GNU_SOURCE -D__KERNEL_I2C__ -D__KERNEL_SPI__ -D__KERNEL_GPIO__ -DLINUX_TARGET -D__PL330_DMA__ -D__NU4000__ -D__HWLESS__
#CDEFINES+=-D__UART_ON_FPGA__

# linker flags
LDFLAGS+=-Wl,--gc-sections

# Usefull macros
RM=rm -f $(1)
CP=cp -f $(1) $(2)

###############################################################################
#	Source tree selection (by find all .c,.S files)
###############################################################################
SRC-MAIN-C := 										\
	SandBox.cpp							\
	post_process.cpp					\
	runtime_commands.cpp					\
#SRC-PROJ_1-C := $(shell find ../../../common -name '*.c')
#SRC-PROJ_2-C := $(shell find ../../../gp/src -name '*.c')
#SRC-PROJ_4-S := $(shell find ../../../../common/infrastructure/os/src -name '*.s')

# Ensure to remove /test folder in the release build
SRC-C := $(SRC-PROJ-C) $(SRC-PROJ_1-C) $(SRC-PROJ_2-C) $(SRC-PROJ_3-C)
SRC-S := $(SRC-PROJ_4-S)

OBJ-MAIN := $(SRC-MAIN-C:.cpp=.obj)
OBJ-C := $(SRC-C:.cpp=.obj)
OBJ-S := $(SRC-S:.s=.obj)

DEPS-MAIN := $(OBJ-MAIN:.obj=.d)
DEPS-C := $(OBJ-C:.obj=.d)
DEPS-S := $(OBJ-S:.obj=.d)

###############################################################################
#	Common make rules
###############################################################################
.PHONY : all clean
all: $(TARGET_BIN)

-include $(DEPS-C)
-include $(DEPS-S)
###############################################################################
#	Object compilation
###############################################################################
$(OBJ-S) : %.obj : %.s
	@echo "  CC        "$<0
	$(V)$(CC) $(ASFLAGS) -c -o $@  $<
	
$(OBJ-MAIN) : %.obj : %.cpp
	@echo "  CC  $(CROSS_COMPILE) "$<
	$(V)$(CC) $(CPPFLAGS) $(CINCLUDE) $(CDEFINES) -c -MD -o $@  $<

###############################################################################
#	Target linkage
###############################################################################
$(TARGET_BIN): $(OBJ-MAIN)
	@echo linking $(IDE_LIBRARIES)....
	$(V)$(CC) $(OBJ-MAIN) $(PATH_LIBRARIES) $(LDFLAGS) $(IDE_LIBRARIES) -o $(TARGET_BIN)	
	$(V)$(ELD) -dsx $(TARGET_BIN) > $(TARGET_NAME).dis
	$(V)$(call CP, $(TARGET_BIN) $(TARGET_BIN).nonstripped)
	$(V)$(STRIP) $(TARGET_BIN)	
	
ifdef RELEASE	
	$(V)$(STRIP) $(TARGET_BIN)
endif

#$(TARGET_LIB): $(OBJ-S) $(OBJ-C) 
#	@echo building $(TARGET_LIB)....
#	$(V)$(AR) -r $(TARGET_LIB) $(OBJ-C) $(OBJ-S) 	
#
#ifneq ($(wildcard ${PROJECT_LOC}../client_demo/libs/gp/.*),)
#	$(V)$(call CP, $(TARGET_LIB) ${PROJECT_LOC}../client_demo/libs/gp/$(TARGET_LIB))
#else
#	@echo "Did not find ${PROJECT_LOC}../client_demo/libs/gp/."
#endif

rebuild: clean all
	
clean:
	@echo cleaning objects...	
	$(V)$(call RM, $(OBJ-C) $(OBJ-S) $(OBJ-MAIN))
	$(V)$(call RM, $(DEPS-C) $(DEPS-S) $(DEPS-MAIN))
	$(V)$(call RM, *.a)
	$(V)$(call RM, *.out)
	$(V)$(call RM, *.out.nonstripped)
	$(V)$(call RM, *.dis)


ifndef VERBOSE
    V := @
else
    V :=
endif
