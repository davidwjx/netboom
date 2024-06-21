# Copyright (C) Inuitive, 2013. All rights reserved.
# 
# Author: Konstantin S. Ram B
# Common build flags 
###############################################################################

###############################################################################
#	Common build defines
###############################################################################
SW_MAJOR_VERSION=3
SW_MINOR_VERSION=2
SVN_REPO=http://svn/repos/inuitive/db/NU3000/project_ver0/fw/vlsi/bootrom_4k/trunk/src
SW_BUILD_VERSION=$(shell svn info ${SVN_REPO} | grep '^Revision:' | sed -e 's/^Revision: //')

###############################################################################
#	ARM cross-compiler selection
###############################################################################
# Mentor CodeSourcery (used for Inuitive simulation/emulation)
#export LM_LICENSE_FILE=1717@svc03
#export MGLS_HOME=/tools/gcc-arm/codesourcery/current/license/mgls/linux
# Linaro toolchain (used for Inomize simulation)
#CROSS_COMPILE_DIR=/tools/gcc-arm/gcc-linaro/gcc-arm-none-eabi-4_6-2012q2

CROSS_COMPILE_DIR=/tools/gcc-arm/gcc-linaro/gcc-arm-none-eabi-4_6-2012q2

###############################################################################
#	Cross-compiler common defines
###############################################################################
CROSS_COMPILE=$(CROSS_COMPILE_DIR)/bin/arm-none-eabi-
CROSS_LIB_INCLUDE=$(CROSS_COMPILE_DIR)/lib/gcc/arm-none-eabi/*
CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)as
AR=$(CROSS_COMPILE)ar
LD=$(CROSS_COMPILE)ld
ELD=$(CROSS_COMPILE)objdump
EL2H=$(CROSS_COMPILE)objcopy
SZ=$(CROSS_COMPILE)size
NM=$(CROSS_COMPILE)nm

###############################################################################
#	Build Environment defines
###############################################################################
ifdef RELEASE
   ENVFLAGS=-DRELEASE
else ifdef VCS
   ENVFLAGS=-DVCS -DBUILD_FOR_HW_VERIF
else ifdef B0_TEST
   ENVFLAGS=-DB0_TEST=$(B0_TEST)
else   
   $(error Error: Environment variable not set)
endif

###############################################################################
#	Compialtion Flags
###############################################################################
# common gcc flags for assembler file builds
ASFLAGS=-g -O0 -Wall -mcpu=cortex-a5 -march=armv7-a
ASFLAGS+=-D__ASSEMBLY__ -D__KERNEL__ -DMAJOR_VERSION=$(SW_MAJOR_VERSION) -DMINOR_VERSION=$(SW_MINOR_VERSION) -DBUILD_VERSION=$(SW_BUILD_VERSION)
#ASFLAGS+=-fdata-sections -ffunction-sections
ASFLAGS+=$(ENVFLAGS)

# common gcc flags for c file builds
CPPFLAGS=-g -O1 -Wall -mcpu=cortex-a5 -march=armv7-a -fno-builtin-memcpy -fno-builtin-memset
# compile with code stripping
CPPFLAGS+=-fdata-sections -ffunction-sections

# defines for u-boot originated code
CDEFINES=-D__ARM__ -D__KERNEL__ -Drestrict=__restrict -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY -DSB_SW -DSBLIB_CF_REMOVE_POLLING_CALLS -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1 -DuECC_WORD_SIZE=4 -DVERIFY_ONLY -DuECC_SUPPORTS_secp256r1=1
# defines for usb3 stack
CDEFINES+=-DUSB_FOR_NU3000 
CDEFINES+=$(ENVFLAGS)
# define for version
CDEFINES+=-DMAJOR_VERSION=$(SW_MAJOR_VERSION) 
CDEFINES+=-DMINOR_VERSION=$(SW_MINOR_VERSION) 
CDEFINES+=-DBUILD_VERSION=$(SW_BUILD_VERSION)

# linker flags
LDFLAGS=-Map=$(TOPDIR)/$(TARGET_NAME).map --cref -static -EL -L $(CROSS_LIB_INCLUDE)  -T $(LDS)
# garbage collect unused sections
#LDFLAGS+= --gc-sections --print-gc-sections
LDFLAGS+= --gc-sections

