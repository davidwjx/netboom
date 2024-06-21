PROJ_BASE=../../../../

TARGET_=libinu_host_lib_4100
TARGET_TEST=inu_host_test
#CROSS_COMPILE_DIR ?= /tools/gcc-arm/gcc-linaro/4.7-2012.12
CROSS_COMPILE_DIR ?= /opt/tools/gcc-linaro/gcc-linaro-arm-linux-gnueabihf-4.7-2013.04-20130415_linux
PLATFORM_NAME ?= tizen_gcc-4.8_armv7a
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
IDE_LIBRARIES = -lusb-1.0 -lrt -lpthread 
#IDE_LIBRARIES_TEST = -Wl,-rpath,./ -static -linu_host_lib $(IDE_LIBRARIES)
IDE_LIBRARIES_TEST = -linu_host_lib_4100 $(IDE_LIBRARIES)
PATH_LIBRARIES = -L $(CROSS_COMPILE_LIB) -L $(PROJ_BASE)../../inu_usb_lib/driver/libusb/bin/$(PLATFORM_NAME)
PATH_LIBRARIES_TEST = -L . $(PATH_LIBRARIES) 
ifeq ($(AT),)
AT:=@
else
AT:=
endif

INCLUDE_DIRS=													\
	$(PROJ_BASE)api/common										\
	$(PROJ_BASE)api/common/graph								\
	$(PROJ_BASE)api/host										\
	$(PROJ_BASE)common/application/manager/inc					\
	$(PROJ_BASE)common/application/alg/infra/inc				\
	$(PROJ_BASE)common/application/periph_ctrl/inc				\
	$(PROJ_BASE)/../../common/version							\
	$(PROJ_BASE)/../../../common/include					    \
	$(PROJ_BASE)/../common										\
	$(PROJ_BASE)common/application/data_base/inc				\
	$(PROJ_BASE)common/application/ceva_boot_params				\
	$(PROJ_BASE)common/application/nucfg						\
	$(PROJ_BASE)common/infrastructure/err/inc					\
	$(PROJ_BASE)common/infrastructure/lm/inc					\
	$(PROJ_BASE)common/infrastructure/lz4						\
	$(PROJ_BASE)common/infrastructure/utils/inc					\
	$(PROJ_BASE)common/infrastructure/mem/inc					\
	$(PROJ_BASE)common/infrastructure/service/inc				\
	$(PROJ_BASE)common/infrastructure/bsp/cde/inc				\
	$(PROJ_BASE)common/infrastructure/bsp/gme/inc				\
	$(PROJ_BASE)common/infrastructure/bsp/img_sensors/general/inc\
	$(PROJ_BASE)common/infrastructure/bsp/dpe/inc				\
	$(PROJ_BASE)common/infrastructure/bsp/iae/inc				\
	$(PROJ_BASE)common/infrastructure/bsp/i2c/inc				\
	$(PROJ_BASE)common/infrastructure/bsp/peripheral/common		\
	$(PROJ_BASE)common/infrastructure/bsp/peripheral/inc		\
	$(PROJ_BASE)common/infrastructure/pal/inc					\
	$(PROJ_BASE)common/infrastructure/os/inc					\
	$(PROJ_BASE)common/infrastructure/time/inc					\
	$(PROJ_BASE)common/infrastructure/cls/src/network			\
	$(PROJ_BASE)common/infrastructure/cls/src/interface			\
	$(PROJ_BASE)common/infrastructure/cls/src/infrastructure	\
	$(PROJ_BASE)common/infrastructure/cls/inc					\
	$(PROJ_BASE)common/infrastructure/inc						\
	$(PROJ_BASE)common/infrastructure/marshal/inc				\
	$(PROJ_BASE)common/infrastructure/bsp/peripheral/idma		\
	$(PROJ_BASE)common/export									\
	$(PROJ_BASE)common/infrastructure/stream					\
	$(PROJ_BASE)common/infrastructure/xmldb						\
	$(PROJ_BASE)common/infrastructure/xmldb/sxmlc/src			\
	$(PROJ_BASE)../../inu_usb_lib/include						\
	$(PROJ_BASE)../../inu_usb_lib/src/include					\
	$(PROJ_BASE)host/											\
	$(PROJ_BASE)../../inu_usb_lib/driver/libusb/include/		\
	$(PROJ_BASE)host/common/include/							\
	$(PROJ_BASE)../../common/include/							\
	$(PROJ_BASE)../../inu_usb_lib/driver/libusb/src/libusb		\

	
SRC_DIRS=														\
	$(PROJ_BASE)api												\
	$(PROJ_BASE)common/api/src									\
	$(PROJ_BASE)common/application/manager/src					\
	$(PROJ_BASE)common/application/nucfg/						\
	$(PROJ_BASE)common/infrastructure/cls/src/infrastructure	\
	$(PROJ_BASE)common/infrastructure/cls/src/interface			\
	$(PROJ_BASE)common/infrastructure/cls/src/network			\
	$(PROJ_BASE)common/infrastructure/marshal/src/				\
	$(PROJ_BASE)common/infrastructure/os/src					\
	$(PROJ_BASE)common/infrastructure/time/src					\
	$(PROJ_BASE)common/infrastructure/lm/src					\
	$(PROJ_BASE)common/infrastructure/lz4						\
	$(PROJ_BASE)common/infrastructure/service/src				\
	$(PROJ_BASE)common/infrastructure/utils/src					\
	$(PROJ_BASE)common/infrastructure/xmldb/					\
	$(PROJ_BASE)common/infrastructure/xmldb/sxmlc/src			\
	$(PROJ_BASE)host/inu_host_lib/src							\
	$(PROJ_BASE)common/infrastructure/stream					\
	$(PROJ_BASE)../../inu_usb_lib/src							\
	$(PROJ_BASE)host/inu_host_lib/test							\
	$(PROJ_BASE)host/common/src									\
	$(PROJ_BASE)../../common/src								\

vpath %.c $(SRC_DIRS)
vpath %.h $(SRC_DIRS)
vpath %.h $(INCLUDE_DIRS)

#CPPFLAGS=$(addprefix -I,$(INCLUDE_DIRS)) -DLINUX_HOST -D_GNU_SOURCE -Wno-format-security -MD -MP -Wno-unused-parameter -mcpu=cortex-a7 -fPIC -Wall
CPPFLAGS_BASIC=$(addprefix -I,$(INCLUDE_DIRS)) -DLINUX_HOST -DUSE_HOTPLUG_POLLING -D_GNU_SOURCE -DLINUX_MAKE_BUILD -Wno-format-security -MD -MP -fPIC -Wno-unused-parameter -Wall
ifeq ($(PLATFORM_NAME),linux_gcc-4.9_armv7a_cortex-a9)
CPPFLAGS=$(CPPFLAGS_BASIC) -mcpu=cortex-a9 -march=armv7-a -mthumb -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3
else
	ifeq ($(PLATFORM_NAME),$(filter $(PLATFORM_NAME), linux_gcc-4.9_armv8 linux_gcc-5.4_armv7 linux_gcc-5.4_armv8))
	CPPFLAGS=$(CPPFLAGS_BASIC) -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3
	else
	CPPFLAGS=$(CPPFLAGS_BASIC)
	endif
endif

OBJS=$(patsubst %.c,%.o,$(filter-out $(SRCS_FILTER_OUT),$(SRCS)))
OBJS_TEST=$(patsubst %.c,%.o,$(filter-out $(SRCS_FILTER_OUT_TEST),$(SRCS_TEST)))

COMPILE.c = $(AT)echo "Compiling $(notdir $<)"; $(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c


# Full list of files to compile

SRCS=										\
	inu_comm.c				\
	inu_ref.c				\
	inu_device.c			\
	inu_logger.c			\
	inu_graph.c				\
	inu_graph_creator.c		\
	inu_node.c				\
	inu_data.c				\
	inu_function.c			\
	inu_sensor.c			\
	inu_sensors_group.c		\
	inu_soc_channel.c		\
	inu_isp_channel.c		\
	inu_soc_writer.c		\
	inu_streamer.c			\
	inu_imu.c				\
	inu_temperature.c		\
	inu_compress.c			\
	inu_fdk.c				\
	inu_factory.c			\
	inu_histogram.c			\
	inu_image.c				\
	inu_imu_data.c			\
	inu_temperature_data.c	\
	inu_fast_data.c			\
	inu_fast_orb.c			\
	inu_cdnn.c				\
	inu_cdnn_data.c			\
	inu_vision_proc.c			\
	inu_slam.c				\
	inu_slam_data.c			\
	inu_dpe_pp.c			\
	inu_pp.c			    \
	inu_point_cloud_data.c  \
	inu_load_network.c		\
	inu_load_background.c	\
	inu_histogram_data.c	\
	inu_sensor_control.c	\
	inu_cva_channel.c		\
	inu_mipi_channel.c		\
	inu_cva_data.c			\
	inu_time.c				\
	inu_tsnr_data.c			\
	inu_tsnr_calc.c			\
	lm.c				\
	inu2.c									\
	config.c								\
	svc_cfg.c								\
	serial.c								\
	uart_com.c								\
	cdc_com.c								\
	stub_com.c								\
	usb_ctrl_com_host.c						\
	conn_lyr.c								\
	queue_lyr.c								\
	marshal.c								\
	m_ioctl.c								\
	m_tbl.c									\
	mem_pool.c								\
	os_lyr_linux.c							\
	inu_queue.c								\
	svc_com.c								\
	inu_str.c								\
	log.c									\
	utils.c									\
	hash.c									\
	inu_profiler.c							\
	inu_usb_libusb_linux.c					\
	inu_boot.c								\
	inu_monitor.c							\
	inu_bootdetect.c						\
	inu_host_utils.c						\
	xml_db.c								\
	xml_db_modes.c								\
	sxmlc.c									\
	graph.c									\
	nucfg.c									\
	calibration.c							\
	libini.c							    \
	nu_soc.c								\
	nu_graph.c								\
	nu4k.c									\
	nufld.c									\
    inu_defs_params.c                       \
	linked_list.c							\
	lz4.c									\
	lz4frame.c								\
	lz4hc.c									\
	xxhash.c								\
	tcp_com.c								\
	socket.c								\

SRCS_TEST=									\

rebuild: clean all

#all: $(TARGET_).a $(TARGET_).so $(TARGET_TEST)
all: $(TARGET_).a $(TARGET_).so
	echo $(TARGET_)
	
$(TARGET_).a: $(OBJS)
	@echo "Updating $@"
	$(AT)$(AR) r $@ $?

$(TARGET_).so: $(OBJS)
	@echo "Creating $@"
	$(CC) $(OBJS) $(PATH_LIBRARIES) $(IDE_LIBRARIES) -Wl,-Bsymbolic -Wl,-Bsymbolic-functions -shared -Wl,-soname,$(TARGET_).so -o $(TARGET_).so 
#	$(AT)$(CC) $(OBJS) $(PATH_LIBRARIES) $(IDE_LIBRARIES) -Wl,-soname,$(TARGET_).so.1 -shared -o $(TARGET_).so.1.0.0 

#$(TARGET_TEST): $(OBJS_TEST)
#	@echo "Creating $@"	
#	$(AT)$(CC) -static $(PATH_LIBRARIES_TEST) $(OBJS_TEST) $(IDE_LIBRARIES_TEST) -o $(TARGET_TEST) 

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
