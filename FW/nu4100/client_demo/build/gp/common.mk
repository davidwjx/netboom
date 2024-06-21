# Copyright (C) Inuitive, 2014. All rights reserved.
# 
# Author: Konstantin Sinyuk
# This makefile is intended for use with GNU make

###############################################################################
#	Common build defines
###############################################################################

SW_VERSION=0x00000000
TOPDIR=$(shell pwd)
TARGET_NAME=inu_target
TARGET_LIB=../../libs/gp/libinu_target
TARGET=$(TARGET_NAME).out
BIN=$(TARGET_NAME).bin

###############################################################################
#	ARM cross-compiler selection
###############################################################################
TARGET_GP_COMPILER_PATH ?= /opt/inuitive/toolchain/linaro/gcc-linaro-4.9.4-2017.01-x86_64_arm-linux-gnueabihf
TARGET_GP_COMPILER_BIN_PATH ?= $(TARGET_GP_COMPILER_PATH)/bin
TARGET_GP_COMPILER_LIB_PATH ?= $(TARGET_GP_COMPILER_PATH)/arm-linux-gnueabihf/libc/lib/arm-linux-gnueabihf
TARGET_GP_COMPILER_INCLUDE_PATH ?= $(TARGET_GP_COMPILER_PATH)/lib/gcc/arm-linux-gnueabihf/4.9.4/include/
TARGET_GP_COMPILER_PREFIX ?= arm-linux-gnueabihf-

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

###############################################################################
#	Build Environment defines
###############################################################################


###############################################################################
#	Compialtion Flags
###############################################################################
IDE_LIBRARIES = -static $(TARGET_LIB).a -lrt -lpthread -lm -ldl -lstdc++ -lguvc -lSecureBoot
PATH_LIBRARIES = -L $(TARGET_GP_COMPILER_LIB_PATH) -L ../../libs/gp/

# common gcc flags for s file builds
ASFLAGS=-g -O0 -Wall -mcpu=cortex-a5 -march=armv7-a

# common gcc flags for c file builds
CPPFLAGS=-g -O3 -Wall -mcpu=cortex-a5 -march=armv7-a -mtune=cortex-a5 -mfpu=neon -ftree-vectorizer-verbose=2

# compile with code stripping
CPPFLAGS+=-fdata-sections -ffunction-sections

CPPFLAGS+=-Wchar-subscripts  -Wcomment -Wformat  -Wmain -Wmissing-braces -Wnonnull  -Wparentheses  -Wreturn-type -Wsequence-point   -Wsign-compare  -Wstrict-aliasing  -Wtrigraphs -Wunknown-pragmas  -Wunused-label -Wunused-value   -Wno-unused-variable  -Wvolatile-register-var -Wno-switch -Wno-enum-compare -Wno-unused-function -Wno-unused-but-set-variable -Wno-uninitialized

# common include directories
CINCLUDE_COMMON=-I$(TARGET_GP_COMPILER_INCLUDE_PATH)	\
-I../../../app/api/common								\
-I../../../app/api/host 								\
-I../../../app/api/client								\
-I../../../app/api/common/graph							\

# defines for u-boot originated code
CDEFINES=-D__ARM__ -D__KERNEL__  -D__linux__ -D_GNU_SOURCE -DLINUX_TARGET

# linker flags
LDFLAGS+=-mthumb -mtune=cortex-a5 -Wl,-O1 -Wl,--hash-style=gnu -march=armv7-a  -mtune=cortex-a5 -g -Wl,--gc-sections

# Usefull macros
RM=rm -f $(1)
CP=cp -f $(1) $(2)
