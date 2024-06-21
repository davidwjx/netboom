# Copyright (C) Inuitive, 2013. All rights reserved.
#
# Author: Konstantin Sinyuk
# This makefile is intended for use with GNU make

###############################################################################
#	Common build defines
###############################################################################

SW_VERSION=0x00000000
TOPDIR=$(shell pwd)
TARGET_NAME=inu_target
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

PROJECT_LOC :=../../../../
###############################################################################
#	Cross-compiler common defines
###############################################################################
CROSS_COMPILE=$(TARGET_GP_COMPILER_BIN_PATH)/$(TARGET_GP_COMPILER_PREFIX)
CC=$(CROSS_COMPILE)gcc
CXX=$(CROSS_COMPILE)g++
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

TRACING_FLAG = -DDISABLE_TRACING # change to -DENABLE_TRACING to enable

###############################################################################
#	Compialtion Flags
###############################################################################
IDE_LIBRARIES = -static -pthread -Wl,--start-group -l$(TARGET_NAME) -l$(TARGET_NAME)  -Wl,--end-group -lrt -lm -lstdc++ -lguvc
PATH_LIBRARIES = -L $(TARGET_GP_COMPILER_LIB_PATH) -L . -L  ${PROJECT_LOC}/../../3rd_party/libguvc-main/libguvc/lib/.libs/

# common gcc flags for s file builds
ASFLAGS=-g -O0 -Werror -mcpu=cortex-a5

# common gcc flags for c file builds
# add -pg at end of both lines for profiling
# add -DENABLE_TRACING to enable trace marker messages
CCFLAGS=-g -O3 -Werror -Wextra -std=gnu11 -mcpu=cortex-a5 -mfpu=neon -ffunction-sections -fdata-sections $(TRACING_FLAG) #-pg
CPPFLAGS=-g -O3 -Werror -Wextra -std=c++11 -mcpu=cortex-a5 -mfpu=neon -ffunction-sections -fdata-sections -Wno-write-strings $(TRACING_FLAG) #-pg

# common include directories
CINCLUDE=-I$(TARGET_GP_COMPILER_INCLUDE_PATH)\
-I${PROJECT_LOC}/api\
-I${PROJECT_LOC}/api/common\
-I${PROJECT_LOC}/api/common/graph\
-I${PROJECT_LOC}/api/client\
-I${PROJECT_LOC}/api/host\
-I${PROJECT_LOC}/common/application/version\
-I${PROJECT_LOC}/common/application/nucfg\
-I${PROJECT_LOC}/common/application/service/svc_mngr\
-I${PROJECT_LOC}/common/application/manager/inc\
-I${PROJECT_LOC}/common/application/standalone_manager/inc\
-I${PROJECT_LOC}/common/infrastructure/service/inc\
-I${PROJECT_LOC}/common/infrastructure/stream\
-I${PROJECT_LOC}/common/infrastructure/time/inc\
-I${PROJECT_LOC}/common/infrastructure/utils/inc\
-I${PROJECT_LOC}/common/infrastructure/inc\
-I${PROJECT_LOC}/common/infrastructure/os/inc\
-I${PROJECT_LOC}/common/infrastructure/marshal/inc\
-I${PROJECT_LOC}/common/infrastructure/cls/inc\
-I${PROJECT_LOC}/common/infrastructure/cls/src/infrastructure\
-I${PROJECT_LOC}/common/infrastructure/cls/src/interface\
-I${PROJECT_LOC}/common/infrastructure/cls/src/network\
-I${PROJECT_LOC}/common/infrastructure/tcp\
-I${PROJECT_LOC}/common/infrastructure/xmldb\
-I${PROJECT_LOC}/common/infrastructure/xmldb/sxmlc/src\
-I${PROJECT_LOC}/common/infrastructure/lm/inc\
-I${PROJECT_LOC}/common/infrastructure/lz4\
-I${PROJECT_LOC}/target/api\
-I${PROJECT_LOC}/target/common/alg/gaze_tracking/inc\
-I${PROJECT_LOC}/target/common/alg/head_tracking/inc\
-I${PROJECT_LOC}/target/common/alg/sensor_control/inc\
-I${PROJECT_LOC}/target/common/alg/disparity_control/inc\
-I${PROJECT_LOC}/target/common/alg/depth_improvement/inc\
-I${PROJECT_LOC}/target/common/alg/disparity_2_depth/inc\
-I${PROJECT_LOC}/target/common/alg/low_light_isp/inc\
-I${PROJECT_LOC}/target/common/alg/low_light_isp/src\
-I${PROJECT_LOC}/target/common/alg/low_light_isp/src/3rd_party\
-I${PROJECT_LOC}/target/common/data_base/inc\
-I${PROJECT_LOC}/target/common/client_api\
-I${PROJECT_LOC}/target/common/mem/inc\
-I${PROJECT_LOC}/target/common/meta\
-I${PROJECT_LOC}/target/common/pal/inc\
-I${PROJECT_LOC}/target/common/icc/inc\
-I${PROJECT_LOC}/target/common/utils/stack/inc\
-I${PROJECT_LOC}/target/gp/src/hw/hw_mngr\
-I${PROJECT_LOC}/target/gp/src/hw/hw_regs\
-I${PROJECT_LOC}/target/gp/src/hw/sequence_mngr\
-I${PROJECT_LOC}/target/gp/src/hw/isp_mngr\
-I${PROJECT_LOC}/target/gp/src/hw/isp_mngr/include\
-I${PROJECT_LOC}/target/gp/src/hw/isp_mngr/include/cam_engine\
-I${PROJECT_LOC}/target/gp/src/hw/isp_mngr/include/ebase\
-I${PROJECT_LOC}/target/gp/src/hw/gme_mngr\
-I${PROJECT_LOC}/target/gp/src/hw/hcg_mngr\
-I${PROJECT_LOC}/target/gp/src/hw/hca_mngr\
-I${PROJECT_LOC}/target/gp/src/hw/iae_mngr\
-I${PROJECT_LOC}/target/gp/src/hw/ppe_mngr\
-I${PROJECT_LOC}/target/gp/src/hw/cva_mngr\
-I${PROJECT_LOC}/target/gp/src/hw/dpe_mngr\
-I${PROJECT_LOC}/target/gp/src/hw/cde_mngr\
-I${PROJECT_LOC}/target/gp/src/hw/sensors_mngr\
-I${PROJECT_LOC}/target/gp/src/hw/trigger_mngr\
-I${PROJECT_LOC}/target/gp/src/hw/projectors_mngr\
-I${PROJECT_LOC}/target/gp/src/hw/mipi_mngr\
-I${PROJECT_LOC}/target/gp/src/hw/xml_dev\
-I${PROJECT_LOC}/target/gp/src/alg_ctrl\
-I${PROJECT_LOC}/target/gp/src/icc/inc\
-I${PROJECT_LOC}/target/gp/src/nuserial/inc\
-I${PROJECT_LOC}/target/gp/src/pmm\
-I${PROJECT_LOC}/target/gp/src/pwr\
-I${PROJECT_LOC}/target/gp/src/cnn_boot/inc\
-I${PROJECT_LOC}/target/gp/src/cmem/inc\
-I${PROJECT_LOC}/target/gp/src/charger/inc\
-I${PROJECT_LOC}/target/gp/src/main\
-I${PROJECT_LOC}/target/gp/src/model\
-I${PROJECT_LOC}/target/gp/src/ccf\
-I${PROJECT_LOC}/target/gp/src/wd\
-I${PROJECT_LOC}/target/gp/src/rtc/inc\
-I${PROJECT_LOC}/target/gp/src/alt\
-I${PROJECT_LOC}/target/gp/src/charger\
-I${PROJECT_LOC}/target/gp/src/nand_raw_storage\
-I${PROJECT_LOC}/target/common/bsp/inc\
-I${PROJECT_LOC}/target/common/bsp/inc/regs\
-I${PROJECT_LOC}/target/common/bsp/cde/inc\
-I${PROJECT_LOC}/target/common/bsp/cde/src\
-I${PROJECT_LOC}/target/common/bsp/i2c/inc\
-I${PROJECT_LOC}/target/common/bsp/i2s/inc\
-I${PROJECT_LOC}/target/common/bsp/dpe/inc\
-I${PROJECT_LOC}/target/common/bsp/iae/inc\
-I${PROJECT_LOC}/target/common/bsp/ppe/inc\
-I${PROJECT_LOC}/target/common/bsp/pvt/inc\
-I${PROJECT_LOC}/target/common/bsp/pss/inc\
-I${PROJECT_LOC}/target/common/bsp/dse\
-I${PROJECT_LOC}/target/common/bsp/mipi/inc\
-I${PROJECT_LOC}/target/common/bsp/peripheral/inc\
-I${PROJECT_LOC}/target/common/bsp/peripheral/common\
-I${PROJECT_LOC}/target/common/bsp/peripheral/idma\
-I${PROJECT_LOC}/target/common/bsp/peripheral/pl330dma\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/general/inc\
-I${PROJECT_LOC}/target/common/bsp/gme/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/aptina/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/ar0234/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/ar130/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/ar135/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/ar135X/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/ov7251/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/ov2685/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/ov9282/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/ov5675/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/ov8856/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/gc2145/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/ov4689/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/ar430/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/ar2020/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/cgs132/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/xc9160/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/AMS_CGSS130/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/ov9782/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/cgs031/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/vd56g3/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/vd55g0/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/os05a10/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/generic_model/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/os05a10/inc\
-I${PROJECT_LOC}/target/common/bsp/img_sensors/ar2020/inc\
-I${PROJECT_LOC}/target/common/bsp/gpio/inc\
-I${PROJECT_LOC}/target/common/bsp/peripheral/gpio\
-I${PROJECT_LOC}/target/common/bsp/spi/inc\
-I${PROJECT_LOC}/target/common/bsp/nand/inc\
-I${PROJECT_LOC}/target/common/bsp/pos_sensors/accelerometer/lsm330dlc/inc\
-I${PROJECT_LOC}/target/common/bsp/pos_sensors/gyroscope/lsm330dlc/inc\
-I${PROJECT_LOC}/target/common/bsp/projector//inc\
-I${PROJECT_LOC}/target/common/bsp/projector/general/inc\
-I${PROJECT_LOC}/../../common/include\
-I${PROJECT_LOC}/../../common/version\
-I${PROJECT_LOC}/../common\
-I${PROJECT_LOC}/common/infrastructure/metadata/nanopb \
-I${PROJECT_LOC}/common/infrastructure/metadata/inc \
-I${PROJECT_LOC}/common/infrastructure/iio/ \
-I${PROJECT_LOC}/target/common/metadata/inc \
-I${PROJECT_LOC}/target/common/sensorsync/inc \
-I${PROJECT_LOC}/common/infrastructure/sensorsync/inc \-I${PROJECT_LOC}/../../3rd_party/secureBoot/Framework/PUBDEFS/incl\
-I${PROJECT_LOC}/../../3rd_party/secureBoot/SBCommon/SBIF_API/incl\
-I${PROJECT_LOC}/../../3rd_party/secureBoot/SBLib/SB_API/incl\
-I${PROJECT_LOC}/../../3rd_party/secureBoot/SBLib/Config\
-I${PROJECT_LOC}/../../3rd_party/secureBoot/SBCommon/Config/ECC256\
-I${PROJECT_LOC}/../../3rd_party/secureBoot/verification\
-I${PROJECT_LOC}/../../3rd_party/secureBoot/include\
-I${PROJECT_LOC}/../../3rd_party/secureBoot/verification\
-I${PROJECT_LOC}/../../3rd_party/secureBoot/SBCommon/SBIF_API/incl\
-I${PROJECT_LOC}/../../3rd_party/libguvc-main/libguvc/lib/\
-I${PROJECT_LOC}/target/common/helsinki/inc

# defines for u-boot originated code
CDEFINES=-D__ARM__ -D__KERNEL__  -D__linux__ -D_GNU_SOURCE -D__KERNEL_I2C__ -D__KERNEL_SPI__ -D__KERNEL_GPIO__ -DLINUX_TARGET -D__PL330_DMA__ -D__NU4000__ -D__HWLESS__
ifdef Adapter_Hardware
CDEFINES+=-DCONFIG_ARCH_HELSINKI_ADAPTER_HARDWARE
endif
#CDEFINES+=-D__UART_ON_FPGA__
ifdef _ZEBU
   CDEFINES+=-D__ZEBU__
endif
# linker flags
LDFLAGS+=-Wl,--gc-sections

# Usefull macros
RM=rm -f $(1)
CP=cp -f $(1) $(2)

###############################################################################
#	Source tree selection (by find all .c,.S files)
###############################################################################
SRC-PROJ-C := $(shell find ../../../../common -not -path "*/nanopb/*/*" -name '*.c')	#Exclude nanopb examples
SRC-PROJ_1-C := $(shell find ../../../common -name '*.c')
SRC-PROJ_2-C := $(shell find ../../../gp/src -name '*.c')
SRC-PROJ_4-S := $(shell find ../../../../common/infrastructure/os/src -name '*.s')		
SRC-PROJ_5-C = $(shell find ../../../gp/src/hw/isp_mngr -name '*.cpp')
SRC-PROJ_6-C = $(shell find ../../../gp/src/nuserial/src/ -name '*.cpp')
SRC-PROJ_7-C = $(shell find ../../../common -name '*.cpp') 
# Ensure to remove /test folder in the release build
SRC-C := $(SRC-PROJ-C) $(SRC-PROJ_1-C) $(SRC-PROJ_2-C) $(SRC-PROJ_3-C)
SRC-S := $(SRC-PROJ_4-S)
SRC-CXX := $(SRC-PROJ_5-C) $(SRC-PROJ_6-C) $(SRC-PROJ_7-C)

OBJ-MAIN := $(SRC-MAIN:.c=.o)
OBJ-C = $(SRC-C:.c=.obj)
OBJ-S := $(SRC-S:.s=.obj)
OBJ-ISP = $(shell find ${PROJECT_LOC}/target/gp/prj/make/ -name '*.o')
OBJ-CXX = $(SRC-CXX:.cpp=.obj)

LDFLAGS += -L${PROJECT_LOC}/target/gp/src/hw/isp_mngr/lib/
#IDE_LIBRARIES += -lcam_engine -lm
#Commented out as we don't have this source IDE_LIBRARIES += -lSecureBoot
PATH_LIBRARIES += -L ${PROJECT_LOC}/../../3rd_party/secureBoot/

DEPS-MAIN := $(OBJ-MAIN:.o=.d)
DEPS-C = $(OBJ-C:.obj=.d)
DEPS-S := $(OBJ-S:.obj=.d)
DEPS-CXX = $(OBJ-CXX:.obj=.d)

###############################################################################
#	Common make rules
###############################################################################
.PHONY : all clean
all: $(TARGET_BIN) 
# Commented out as we don't have this source all: $(TARGET_BIN) subsystem

-include $(DEPS-C)
-include $(DEPS-CXX)
-include $(DEPS-S)
###############################################################################
#	Object compilation
###############################################################################
$(OBJ-S) : %.obj : %.s
	@echo "  CC        "$<0
	$(V)$(CC) $(ASFLAGS) -c -o $@  $<

$(OBJ-C) $(OBJ-MAIN) : %.obj : %.c
	@echo "  CC  $(CROSS_COMPILE) "$<
	$(V)$(AR) -x  ${PROJECT_LOC}/target/gp/src/hw/isp_mngr/lib/libcam_engine.a
	$(V)$(CC) $(CCFLAGS) $(CINCLUDE) $(CDEFINES) -c -MD -o $@  $<

$(OBJ-CXX) : %.obj : %.cpp
	@echo "  CXX  $(CROSS_COMPILE) "$<
	$(V)$(AR) -x  ${PROJECT_LOC}/target/gp/src/hw/isp_mngr/lib/libcam_engine.a
	$(V)$(CXX) $(CPPFLAGS) $(CINCLUDE) $(CDEFINES) -c -MD -o $@  $<

###############################################################################
#	Target linkage
# add  -pg to $(V)$(CC) line for profiling
# add S to $(V)$(ELD) -dsx $(TARGET_BIN) > $(TARGET_NAME).dis to generate
# 		mixed C/ASM listing (-dsxS)
###############################################################################
$(TARGET_BIN): $(TARGET_LIB) $(OBJ-MAIN)
	@echo linking $(IDE_LIBRARIES)....
	$(V)$(CC) $(OBJ-MAIN) $(PATH_LIBRARIES) $(LDFLAGS) $(IDE_LIBRARIES) -o $(TARGET_BIN) $(TRACING_FLAG)
	$(V)$(ELD) -dsx $(TARGET_BIN) > $(TARGET_NAME).dis
	$(V)$(call CP, $(TARGET_BIN) $(TARGET_BIN).nonstripped)
	$(V)$(STRIP) $(TARGET_BIN)
	$(V)$(call RM, *.o)

ifdef RELEASE
	$(V)$(STRIP) $(TARGET_BIN)
endif

$(TARGET_LIB): $(OBJ-S) $(OBJ-C) $(OBJ-ISP) $(OBJ-CXX)
	@echo building $(TARGET_LIB)....
	$(V)$(AR) -r $(TARGET_LIB) $(OBJ-ISP) $(OBJ-C) $(OBJ-S) $(OBJ-CXX)

ifneq ($(wildcard ${PROJECT_LOC}../client_demo/libs/gp/.*),)
	$(V)$(call CP, $(TARGET_LIB) ${PROJECT_LOC}../client_demo/libs/gp/$(TARGET_LIB))
else
	@echo "Did not find ${PROJECT_LOC}../client_demo/libs/gp/."
endif
#Commented out as we don't have secure boot source
# subsystem:
# 	$(MAKE) -C ${PROJECT_LOC}/../../3rd_party/secureBoot/

rebuild: clean all

clean:
	@echo cleaning objects...
	$(V)$(call RM, $(OBJ-C) $(OBJ-S) $(OBJ-MAIN)) $(OBJ-CXX) $(OBJ-ISP)
	$(V)$(call RM, $(DEPS-C) $(DEPS-S) $(DEPS-MAIN))
	$(V)$(call RM, *.o)
	$(V)$(call RM, *.a)
	$(V)$(call RM, *.out)
	$(V)$(call RM, *.out.nonstripped)
	$(V)$(call RM, *.dis)


ifndef VERBOSE
    V := @
else
    V :=
endif
