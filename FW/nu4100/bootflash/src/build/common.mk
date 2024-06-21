# Copyright (C) Inuitive, 2013. All rights reserved.
# 
# Author: Konstantin S. Ram B
# Common build flags 
###############################################################################

###############################################################################
#	Common build defines
###############################################################################
SW_MAJOR_VERSION=1
SW_MINOR_VERSION=1
SW_BUILD_VERSION=1

###############################################################################
#	ARM cross-compiler selection
###############################################################################
TARGET_BAREMETAL_COMPILER_PATH ?= /opt/inuitive/toolchain/linaro/gcc-arm-none-eabi-9-2019-q4-major
TARGET_BAREMETAL_COMPILER_BIN_PATH ?= $(TARGET_BAREMETAL_COMPILER_PATH)/bin
TARGET_BAREMETAL_COMPILER_LIB_PATH ?= $(TARGET_BAREMETAL_COMPILER_PATH)/lib/gcc/arm-none-eabi/*
TARGET_BAREMETAL_COMPILER_INCLUDE_PATH ?= $(TARGET_BAREMETAL_COMPILER_PATH)/lib/gcc/arm-none-eabi/4.6.2/include
TARGET_BAREMETAL_COMPILER_PREFIX ?= arm-none-eabi-
###############################################################################
#	Cross-compiler common defines
###############################################################################
CROSS_COMPILE=$(TARGET_BAREMETAL_COMPILER_BIN_PATH)/$(TARGET_BAREMETAL_COMPILER_PREFIX)
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
ifdef RELEASE
	ENVFLAGS+=-DRELEASE
else ifdef VCS
   ENVFLAGS+= -DVCS	
else
   $(error Error: Environment variable not set)
endif

ifdef NU4000B0
	ENVFLAGS+=-DNU4000_B0
else ifdef NU3000B0
	ENVFLAGS+=-DNU3000_B0
else
	ENVFLAGS+=-DNU4000_A0
endif

###############################################################################
#	Compialtion Flags
###############################################################################
# common gcc flags for assembler file builds
ASFLAGS=-g -O0 -Wall -mcpu=cortex-a5
ASFLAGS+=-D__ASSEMBLY__ -D__KERNEL__ -DMAJOR_VERSION=$(SW_MAJOR_VERSION) -DMINOR_VERSION=$(SW_MINOR_VERSION) -DBUILD_VERSION=$(SW_BUILD_VERSION)
#ASFLAGS+=-fdata-sections -ffunction-sections
ASFLAGS+=$(ENVFLAGS)

# common gcc flags for c file builds
CPPFLAGS=-g -O2 -Wall -mcpu=cortex-a5 -fno-builtin-memcpy -fno-builtin-memset
# compile with code stripping
#CPPFLAGS+=-fdata-sections -ffunction-sections

# defines for u-boot originated code
CDEFINES=-D__ARM__ -D__KERNEL__ 
CDEFINES+=$(ENVFLAGS)
CDEFINES+=-DUSB_FOR_NU3000
# define for version
CDEFINES+=-DMAJOR_VERSION=$(SW_MAJOR_VERSION) 
CDEFINES+=-DMINOR_VERSION=$(SW_MINOR_VERSION) 
CDEFINES+=-DBUILD_VERSION=$(SW_BUILD_VERSION)

# linker flags
LDFLAGS=-Map=$(TOPDIR)/$(TARGET_NAME).map --cref -static -EL -L $(TARGET_BAREMETAL_COMPILER_LIB_PATH)  -T $(LDS)
# garbage collect unused sections
#LDFLAGS+= --gc-sections --print-gc-sections

