# Author: Uri Shkolnik
# 
# Croos platform build tools
###############################################################################



#	Build OS Environment  
###############################################################################
ifeq  "$(OS)" "Windows_NT"
    OSE=Windows
    # Local machine ARM tools directory
    LOCAL_MACHINE_ARM_TOOLCHAIN_ROOTPATH=/tools/gcc-arm-10.3-2021.07-mingw-w64-i686-arm-none-eabi
    # Local machine BNU tools
    LOCAL_MACHINE_GNU_TOOLS_DIR=/tools/gnu/usr/local/wbin
else
    OSE=Linux
    #if you are using toolchain 4_6-2012q2 (not recommended) uncomment the next line
    USE_OLD_TOOLCHAIN=1
endif



###############################################################################
#	ARM cross-compiler selection
###############################################################################
# Mentor CodeSourcery (used for Inuitive simulation/emulation)
#export LM_LICENSE_FILE=1717@svc03
#export MGLS_HOME=/tools/gcc-arm/codesourcery/current/license/mgls/linux
# Linaro toolchain (used for Inomize simulation)
#CROSS_COMPILE_DIR=/tools/gcc-arm/gcc-linaro/gcc-arm-none-eabi-4_6-2012q2

ifdef USE_OLD_TOOLCHAIN
        # Old toolchain v4_6-2012q2 on Linux
        CROSS_COMPILE_DIR=/tools/gcc-arm/gcc-linaro/gcc-arm-none-eabi-4_6-2012q2
        CROSS_COMPILE=$(CROSS_COMPILE_DIR)/bin/arm-none-eabi-
        CROSS_LIB_INCLUDE=$(CROSS_COMPILE_DIR)/lib/gcc/arm-none-eabi/*

else  # Toolchain version 10.3-2021.07

    ifeq  "$(OS)" "Windows_NT"
        # Toolchain v10.3-2021.07 for Windows
        CROSS_COMPILE_DIR= $(LOCAL_MACHINE_ARM_TOOLCHAIN_ROOTPATH)
        CROSS_COMPILE=$(CROSS_COMPILE_DIR)/bin/arm-none-eabi-
        CROSS_LIB_INCLUDE=$(CROSS_COMPILE_DIR)/lib/gcc/arm-none-eabi/10.3.1/thumb/v7-a+fp/hard
    else
        # Toolchain v10.3-2021.07 for lINUX
        CROSS_COMPILE_DIR= 
        CROSS_COMPILE=
        CROSS_LIB_INCLUDE=
    endif

endif


###############################################################################
#	Cross-compiler common defines
###############################################################################
CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)as
AR=$(CROSS_COMPILE)ar
LD=$(CROSS_COMPILE)ld
ELD=$(CROSS_COMPILE)objdump
EL2H=$(CROSS_COMPILE)objcopy
SZ=$(CROSS_COMPILE)size
NM=$(CROSS_COMPILE)nm


###############################################################################
#	Other tools
###############################################################################

ifeq  "$(OS)" "Windows_NT"
    FIND=$(LOCAL_MACHINE_GNU_TOOLS_DIR)/find
#    SH = $(LOCAL_MACHINE_UNIX_TOOLS_DIR)sh
else # Linux
    FIND=find
endif

        
