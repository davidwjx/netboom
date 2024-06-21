# Copyright (C) Inuitive, 2013. All rights reserved.
# 
# Author: Konstantin S. Ram B
# Porting to Windows: Uri Shkolnik
# Common build flags 
###############################################################################

###############################################################################
#	User Configuration
###############################################################################

### Uncomment for verbose building
#VERBOSE = 1

### ENV
RELEASE=1 
CHIP_ID=nu4000c1

### Source Control
SW_MAJOR_VERSION=3
SW_MINOR_VERSION=2
SVN_REPO=http://svn/repos/inuitive/db/NU3000/project_ver0/sw/trunk/FW

### Assembler Addtional Flags
USER_CFLAGS=
# USER_CFLAGS=-fdata-sections -ffunction-sections

### Compiler Additional Flags
USER_CFLAGS =

### Linker Additional Flags
USER_LFLAGS = 
# USER_LFLAGS = --gc-sections --print-gc-sections
USER_LFLAGS = --gc-sections

include build/tools.mk


###############################################################################
#	DO NOT EDIT BELOW
###############################################################################

#	Build - Verbose or quite?
###############################################################################
ifndef VERBOSE
    V := @
else
    V :=
endif


#	Common build defines
###############################################################################
# SW_BUILD_VERSION=$(shell svn info ${SVN_REPO} | grep '^Revision:' | sed -e 's/^Revision: //')
SW_BUILD_VERSION=

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
ASFLAGS=-g -O0 -Wall -mcpu=cortex-a5
# -march=armv7-a
ASFLAGS+=-D__ASSEMBLY__ -D__KERNEL__ -DMAJOR_VERSION=$(SW_MAJOR_VERSION) -DMINOR_VERSION=$(SW_MINOR_VERSION) -DBUILD_VERSION=$(SW_BUILD_VERSION)
ASFLAGS+=$(ENVFLAGS) $(USER_CFLAGS)

###############################################################################
# common gcc flags for c file builds
CPPFLAGS=-g -O1 -Wall -mcpu=cortex-a5 -fno-builtin-memcpy -fno-builtin-memset
# -march=armv7-a 
# compile with code stripping
CPPFLAGS+=-fdata-sections -ffunction-sections

# defines for u-boot originated code
CDEFINES=-D__ARM__ -D__KERNEL__ -Drestrict=__restrict -DSBLIB_CF_IMAGE_TYPE_W_SBCR_KEY -DSB_SW \
    -DSBLIB_CF_REMOVE_POLLING_CALLS -DuECC_VLI_NATIVE_LITTLE_ENDIAN=1 \
    -DuECC_WORD_SIZE=4 -DVERIFY_ONLY -DuECC_SUPPORTS_secp256r1=1
	
# defines for usb3 stack
CDEFINES+=-DUSB_FOR_NU3000 
CDEFINES+=$(ENVFLAGS)

# define for version
CDEFINES+=-DMAJOR_VERSION=$(SW_MAJOR_VERSION) 
CDEFINES+=-DMINOR_VERSION=$(SW_MINOR_VERSION) 
CDEFINES+=-DBUILD_VERSION=$(SW_BUILD_VERSION)

# define User flags
CDEFINES+=$(USER_CFLAGS)

ifdef USE_OLD_TOOLCHAIN
CDEFINES+=-DUSE_OLD_TOOLCHAIN
endif


###############################################################################
# linker flags
LDFLAGS=-Map=$(TOPDIR)/$(TARGET_NAME).map --cref -static -EL -L $(CROSS_LIB_INCLUDE)  -T $(LDS) $(USER_LFLAGS)

