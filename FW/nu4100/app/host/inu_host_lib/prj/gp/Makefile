PROJ_BASE=../../../../

TARGET_=libinu_host_lib
TARGET_TEST=inu_host_test
CROSS_COMPILE_DIR = /tools/gcc-arm/gcc-linaro/4.7-2012.12
#CROSS_COMPILE_DIR ?= /opt/tools/gcc-linaro/gcc-linaro-arm-linux-gnueabihf-4.7-2013.04-20130415_linux
PLATFORM_NAME ?= tizen_gcc-4.8_armv7a
CROSS_COMPILE_LIB = /tools/gcc-arm/gcc-linaro/4.7-2012.12/arm-linux-gnueabihf/libc/lib/arm-linux-gnueabihf
###############################################################################
#	Cross-compiler common defines
###############################################################################
CROSS_COMPILE ?= $(CROSS_COMPILE_DIR)/bin/arm-linux-gnueabihf-
CC=$(CROSS_COMPILE)gcc
AS=$(CROSS_COMPILE)as
AR=$(CROSS_COMPILE)ar
LD=$(CROSS_COMPILE)ld
ELD=$(CROSS_COMPILE)objdump
EL2H=$(CROSS_COMPILE)objcopy
SZ=$(CROSS_COMPILE)size
NM=$(CROSS_COMPILE)nm
STRIP=$(CROSS_COMPILE)strip

#IDE_LIBRARIES = -static -pthread -static -lrt
IDE_LIBRARIES = -lrt -lpthread
#IDE_LIBRARIES_TEST = -Wl,-rpath,./ -static -linu_host_lib $(IDE_LIBRARIES)
IDE_LIBRARIES_TEST = -linu_host_lib $(IDE_LIBRARIES)
PATH_LIBRARIES = -L $(CROSS_COMPILE_LIB)
PATH_LIBRARIES_TEST = -L . $(PATH_LIBRARIES) 
ifeq ($(AT),)
AT:=@
else
AT:=
endif

INCLUDE_DIRS=								\
	$(PROJ_BASE)api/common					\
	$(PROJ_BASE)api/host					\
	$(PROJ_BASE)common/application/service/alg			\
	$(PROJ_BASE)common/application/service/depth			\
	$(PROJ_BASE)common/application/service/video			\
	$(PROJ_BASE)common/application/service/webcam			\
	$(PROJ_BASE)common/application/service/logger			\
	$(PROJ_BASE)common/application/service/inject			\
	$(PROJ_BASE)common/application/service/auxilary			\
	$(PROJ_BASE)common/application/service/svc_mngr			\
	$(PROJ_BASE)common/application/service/system			\
	$(PROJ_BASE)common/application/service/client			\
	$(PROJ_BASE)common/application/service/pos_sensors		\
	$(PROJ_BASE)common/application/alg/infra/inc			\
	$(PROJ_BASE)common/application/periph_ctrl/inc			\
	$(PROJ_BASE)common/application/version				\
	$(PROJ_BASE)common/application/data_base/inc			\
	$(PROJ_BASE)common/application/ceva_boot_params			\
	$(PROJ_BASE)common/infrastructure/err/inc			\
	$(PROJ_BASE)common/infrastructure/utils/inc			\
	$(PROJ_BASE)common/infrastructure/mem/inc			\
	$(PROJ_BASE)common/infrastructure/service/inc			\
	$(PROJ_BASE)common/infrastructure/bsp/cde/inc			\
	$(PROJ_BASE)common/infrastructure/bsp/gme/inc			\
	$(PROJ_BASE)common/infrastructure/bsp/img_sensors/general/inc	\
	$(PROJ_BASE)common/infrastructure/bsp/dpe/inc			\
	$(PROJ_BASE)common/infrastructure/bsp/iae/inc			\
	$(PROJ_BASE)common/infrastructure/bsp/i2c/inc			\
	$(PROJ_BASE)common/infrastructure/bsp/peripheral/common		\
	$(PROJ_BASE)common/infrastructure/bsp/peripheral/inc		\
	$(PROJ_BASE)common/infrastructure/pal/inc			\
	$(PROJ_BASE)common/infrastructure/os/inc			\
	$(PROJ_BASE)common/infrastructure/cls/src/network		\
	$(PROJ_BASE)common/infrastructure/cls/src/interface		\
	$(PROJ_BASE)common/infrastructure/cls/src/infrastructure	\
	$(PROJ_BASE)common/infrastructure/cls/inc			\
	$(PROJ_BASE)common/infrastructure/inc				\
	$(PROJ_BASE)common/infrastructure/marshal/inc			\
	$(PROJ_BASE)common/infrastructure/bsp/peripheral/idma		\
	$(PROJ_BASE)common/export					\
	$(PROJ_BASE)common/infrastructure/stream			\
	$(PROJ_BASE)host/						\

	
SRC_DIRS=								\
        $(PROJ_BASE)api                                                 \
        $(PROJ_BASE)common/application/service/inject             	\
        $(PROJ_BASE)common/application/service/depth              	\
        $(PROJ_BASE)common/application/service/alg               	\
        $(PROJ_BASE)common/application/service/logger             	\
        $(PROJ_BASE)common/application/service/svc_mngr			\
        $(PROJ_BASE)common/application/service/video               	\
	$(PROJ_BASE)common/application/service/webcam               	\
	$(PROJ_BASE)common/application/service/system               	\
	$(PROJ_BASE)common/application/service/client               	\
	$(PROJ_BASE)common/application/service/pos_sensors            	\
        $(PROJ_BASE)common/infrastructure/cls/src/infrastructure        \
        $(PROJ_BASE)common/infrastructure/cls/src/interface             \
        $(PROJ_BASE)common/infrastructure/cls/src/network               \
        $(PROJ_BASE)common/infrastructure/marshal/src/                  \
        $(PROJ_BASE)common/infrastructure/os/src                        \
        $(PROJ_BASE)common/infrastructure/service/src                   \
        $(PROJ_BASE)common/infrastructure/utils/src                     \
	$(PROJ_BASE)host/inu_host_lib/src				\
	$(PROJ_BASE)common/infrastructure/stream			\
	$(PROJ_BASE)host/inu_host_lib/test

vpath %.c $(SRC_DIRS)
vpath %.h $(SRC_DIRS)
vpath %.h $(INCLUDE_DIRS)

#CPPFLAGS=$(addprefix -I,$(INCLUDE_DIRS)) -DLINUX_HOST -D_GNU_SOURCE -Wno-format-security -MD -MP -Wno-unused-parameter -mcpu=cortex-a7 -fPIC -Wall
CPPFLAGS=$(addprefix -I,$(INCLUDE_DIRS)) -DLINUX_GP_HOST -DDEFSG_IS_UART_MIPI -D_GNU_SOURCE -DLINUX_MAKE_BUILD -Wno-format-security -MD -MP -fPIC -Wno-unused-parameter -Wall

OBJS=$(patsubst %.c,%.o,$(filter-out $(SRCS_FILTER_OUT),$(SRCS)))
OBJS_TEST=$(patsubst %.c,%.o,$(filter-out $(SRCS_FILTER_OUT_TEST),$(SRCS_TEST)))

COMPILE.c = $(AT)echo "Compiling $(notdir $<)"; $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c


# Full list of files to compile

SRCS=										\
	system.c								\
	client.c                                				\
	pos_sensors.c                           				\
	depth.c									\
	video.c									\
	inject.c								\
	inu.c									\
	inu_sdk.c								\
	config.c								\
	alg.c									\
	logger.c								\
	svc_mngr.c								\
	serial.c								\
	uart_com.c								\
	stub_com.c								\
	udp_com.c								\
	cdc_com.c								\
	conn_lyr.c								\
	queue_lyr.c								\
	marshal.c								\
	mem_pool.c								\
	os_lyr_host_gp.c						\
	inu_queue.c								\
	service.c								\
	svc_com.c								\
	inu_str.c								\
	log.c									\
	utils.c									\
	webcam.c								\
	stream.c								\

SRCS_TEST=									\
	inu_host_test.c \

rebuild: clean all

all: $(TARGET_).a $(TARGET_).so $(TARGET_TEST)
	echo $(TARGET_)
	
$(TARGET_).a: $(OBJS)
	@echo "Updating $@"
	$(AT)$(AR) r $@ $?

$(TARGET_).so: $(OBJS)
	@echo "Creating $@"
	$(AT)$(CC) $(OBJS) $(PATH_LIBRARIES) $(IDE_LIBRARIES) -Wl,-soname,$(TARGET_).so -shared -o $(TARGET_).so 
#	$(AT)$(CC) $(OBJS) $(PATH_LIBRARIES) $(IDE_LIBRARIES) -Wl,-soname,$(TARGET_).so.1 -shared -o $(TARGET_).so.1.0.0 

$(TARGET_TEST): $(OBJS_TEST)
	@echo "Creating $@"	
	$(AT)$(CC) -static $(PATH_LIBRARIES_TEST) $(OBJS_TEST) $(IDE_LIBRARIES_TEST) -o $(TARGET_TEST) 

clean: 
	@echo "Cleaning..."
	@echo $(SRC_DIRS)
	-$(AT)$(RM) $(OBJS) *.d
	-$(AT)$(RM) $(TARGET_).a $(TARGET_).so*
	-$(AT)$(RM) $(OBJS_TEST) $(TARGET_TEST)	

clean_all: clean
	@echo "Cleanning more..."
	-$(AT)$(RM) $(TARGET_).a $(TARGET_).so.*
	-$(AT)$(RM) $(OBJS_TEST) $(TARGET_TEST)	
	
-include $(patsubst %.c,%.d,$(filter-out $(SRCS_FILTER_OUT),$(SRCS)))
