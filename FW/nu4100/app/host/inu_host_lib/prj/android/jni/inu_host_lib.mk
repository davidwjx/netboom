# Android build config for libusb examples
# Copyright © 2012-2013 RealVNC Ltd. <toby.gray@realvnc.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#

LOCAL_PATH:= $(call my-dir)

INU_USB_LIB_ROOT:=$(LOCAL_PATH)/../../../../../../../inu_usb_lib/
INU_COMMON_ROOT:=$(LOCAL_PATH)/../../../../../../../common/
INU_HOST_LIB_ROOT:=$(LOCAL_PATH)/../../..
LIBUSB_ROOT:= $(INU_USB_LIB_ROOT)/driver/libusb
INU_HOST_LIB_SRC_ROOT:=$(INU_HOST_LIB_ROOT)/src
INU_SRC_ROOT:=$(INU_HOST_LIB_ROOT)/../..
INU_SRC_API_ROOT:=$(INU_SRC_ROOT)/api
INU_SRC_COMMON_ROOT:=$(INU_SRC_ROOT)/common


# include libusb
include $(CLEAR_VARS)
LOCAL_MODULE := libusb1.0
LOCAL_SRC_FILES := $(LIBUSB_ROOT)/bin/$(BUILD_PLATFORM)/libusb1.0.so
LOCAL_EXPORT_C_INCLUDES := $(LIBUSB_ROOT)/libusb
include $(PREBUILT_SHARED_LIBRARY)

# include inu_usb_lib
include $(CLEAR_VARS)
LOCAL_MODULE := libinu_usb_lib
LOCAL_SRC_FILES := $(INU_USB_LIB_ROOT)/prj/android/bin/$(BUILD_PLATFORM)/libinu_usb_lib.so
LOCAL_EXPORT_C_INCLUDES := $(INU_USB_LIB_ROOT)/include
include $(PREBUILT_SHARED_LIBRARY)

# build inu_host_lib
include $(CLEAR_VARS)
LOCAL_CFLAGS    := -I$(LOCAL_PATH)/ -DANDROID_HOST -Wno-format-security -MD -MP -O3
LOCAL_LDFLAGS   := -Wl,-Bsymbolic -Wl,-Bsymbolic-functions
LOCAL_DISABLE_FORMAT_STRING_CHECKS := true
LOCAL_MODULE    := inu_host_lib_4100

LOCAL_C_INCLUDES := \
	$(LIBUSB_ROOT)/include										\
	$(INU_COMMON_ROOT)/include									\
	$(INU_USB_LIB_ROOT)/include 								\
	$(INU_SRC_API_ROOT)/common									\
	$(INU_SRC_API_ROOT)/common/graph							\
	$(INU_SRC_API_ROOT)/host									\
	$(INU_SRC_COMMON_ROOT)/../../../common/version				\
	$(INU_SRC_COMMON_ROOT)/../../common				            \
	$(INU_SRC_COMMON_ROOT)/infrastructure/utils/inc				\
	$(INU_SRC_COMMON_ROOT)/infrastructure/service/inc			\
	$(INU_SRC_COMMON_ROOT)/infrastructure/os/inc				\
	$(INU_SRC_COMMON_ROOT)/infrastructure/time/inc				\
	$(INU_SRC_COMMON_ROOT)/infrastructure/cls/src/network		\
	$(INU_SRC_COMMON_ROOT)/infrastructure/cls/src/interface		\
	$(INU_SRC_COMMON_ROOT)/infrastructure/cls/src/infrastructure\
	$(INU_SRC_COMMON_ROOT)/infrastructure/cls/inc				\
	$(INU_SRC_COMMON_ROOT)/infrastructure/inc					\
	$(INU_SRC_COMMON_ROOT)/infrastructure/marshal/inc			\
	$(INU_SRC_COMMON_ROOT)/infrastructure/lm/inc				\
	$(INU_SRC_COMMON_ROOT)/infrastructure/lz4					\
   	$(INU_SRC_COMMON_ROOT)/infrastructure/marshal/				\
	$(INU_SRC_COMMON_ROOT)/infrastructure/mem/inc/				\
	$(INU_SRC_COMMON_ROOT)/infrastructure/stream				\
	${INU_SRC_COMMON_ROOT}/infrastructure/xmldb					\
	${INU_SRC_COMMON_ROOT}/infrastructure/xmldb/sxmlc/src		\
	${INU_SRC_COMMON_ROOT}/application/nucfg					\
	${INU_SRC_COMMON_ROOT}/application/manager/inc				\
	
LOCAL_SRC_FILES := \
	${INU_SRC_COMMON_ROOT}/application/manager/src/svc_cfg.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/svc_com.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu2.c					\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_comm.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_ref.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_device.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_logger.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_graph.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_node.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_data.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_function.c			\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_sensor.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_sensors_group.c		\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_soc_channel.c		\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_cva_channel.c		\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_mipi_channel.c		\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_isp_channel.c		\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_cva_data.c			\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_soc_writer.c			\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_streamer.c			\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_imu.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_temperature.c		\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_compress.c			\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_fast_data.c			\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_fast_orb.c	    	\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_cdnn.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_cdnn_data.c			\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_vision_proc.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_slam.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_slam_data.c			\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_dpe_pp.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_pp.c				    \
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_point_cloud_data.c   \
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_load_network.c		\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_load_background.c	\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_fdk.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_factory.c			\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_histogram.c			\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_image.c				\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_imu_data.c			\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_temperature_data.c			\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_histogram_data.c		\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_sensor_control.c		\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_tsnr_calc.c	    	\
	${INU_SRC_COMMON_ROOT}/application/manager/src/inu_tsnr_data.c	    	\
	${INU_SRC_COMMON_ROOT}/application/nucfg/nucfg.c						\
	${INU_SRC_COMMON_ROOT}/application/nucfg/xml_db_modes.c						\
	${INU_SRC_COMMON_ROOT}/application/nucfg/calibration.c					\
	${INU_SRC_COMMON_ROOT}/application/nucfg/libini.c						\
	${INU_SRC_COMMON_ROOT}/application/nucfg/nu_soc.c						\
	${INU_SRC_COMMON_ROOT}/application/nucfg/nu_graph.c						\
	${INU_SRC_COMMON_ROOT}/application/nucfg/nu4k.c							\
	${INU_SRC_COMMON_ROOT}/application/nucfg/nufld.c						\
	${INU_SRC_COMMON_ROOT}/application/nucfg/inu_graph_creator.c			\
	$(INU_SRC_COMMON_ROOT)/infrastructure/cls/src/infrastructure/serial.c	\
	$(INU_SRC_COMMON_ROOT)/infrastructure/cls/src/infrastructure/socket.c	\
	$(INU_SRC_COMMON_ROOT)/infrastructure/cls/src/interface/uart_com.c		\
	$(INU_SRC_COMMON_ROOT)/infrastructure/cls/src/interface/tcp_com.c		\
	$(INU_SRC_COMMON_ROOT)/infrastructure/cls/src/interface/stub_com.c		\
	$(INU_SRC_COMMON_ROOT)/infrastructure/cls/src/interface/cdc_com.c		\
	$(INU_SRC_COMMON_ROOT)/infrastructure/cls/src/interface/usb_ctrl_com_host.c	\
	$(INU_SRC_COMMON_ROOT)/infrastructure/cls/src/network/conn_lyr.c		\
	$(INU_SRC_COMMON_ROOT)/infrastructure/cls/src/network/queue_lyr.c		\
	$(INU_SRC_COMMON_ROOT)/infrastructure/lm/src/lm.c						\
	$(INU_SRC_COMMON_ROOT)/infrastructure/lz4/lz4.c							\
	$(INU_SRC_COMMON_ROOT)/infrastructure/lz4/lz4frame.c					\
	$(INU_SRC_COMMON_ROOT)/infrastructure/lz4/lz4hc.c						\
	$(INU_SRC_COMMON_ROOT)/infrastructure/lz4/xxhash.c						\
	$(INU_SRC_COMMON_ROOT)/infrastructure/marshal/src/marshal.c				\
	$(INU_SRC_COMMON_ROOT)/infrastructure/marshal/src/m_ioctl.c				\
	$(INU_SRC_COMMON_ROOT)/infrastructure/marshal/src/m_tbl.c				\
	$(INU_SRC_COMMON_ROOT)/infrastructure/os/src/os_lyr_android.c			\
	$(INU_SRC_COMMON_ROOT)/infrastructure/os/src/inu_queue.c				\
	$(INU_SRC_COMMON_ROOT)/infrastructure/os/src/linked_list.c				\
	$(INU_SRC_COMMON_ROOT)/infrastructure/os/src/mem_pool.c					\
	$(INU_SRC_COMMON_ROOT)/infrastructure/time/src/inu_time.c				\
	$(INU_SRC_COMMON_ROOT)/infrastructure/utils/src/inu_str.c				\
	$(INU_SRC_COMMON_ROOT)/infrastructure/utils/src/hash.c					\
	$(INU_SRC_COMMON_ROOT)/infrastructure/utils/src/log.c					\
	$(INU_SRC_COMMON_ROOT)/infrastructure/utils/src/utils.c					\
	$(INU_SRC_COMMON_ROOT)/infrastructure/utils/src/inu_profiler.c			\
	$(INU_SRC_COMMON_ROOT)/infrastructure/utils/src/config.c				\
	$(INU_SRC_COMMON_ROOT)/infrastructure/utils/src/inu_defs_params.c		\
	${INU_SRC_COMMON_ROOT}/infrastructure/xmldb/xml_db.c					\
	${INU_SRC_COMMON_ROOT}/infrastructure/xmldb/sxmlc/src/sxmlc.c			\
	${INU_SRC_COMMON_ROOT}/infrastructure/utils/src/graph.c					\
LOCAL_SHARED_LIBRARIES += libusb1.0
LOCAL_SHARED_LIBRARIES += libinu_usb_lib


include $(BUILD_SHARED_LIBRARY)
