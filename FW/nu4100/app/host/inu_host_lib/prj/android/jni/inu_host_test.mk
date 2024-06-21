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
INU_USB_LIB_ROOT:=$(LOCAL_PATH)/../../../../inu_usb_lib/
INU_HOST_LIB_ROOT:=$(LOCAL_PATH)/../../..
LIBUSB_ROOT:= $(INU_USB_LIB_ROOT)/driver/libusb
INU_HOST_LIB_SRC_ROOT:=$(INU_HOST_LIB_ROOT)/src
INU_HOST_LIB_TEST_ROOT:=$(INU_HOST_LIB_ROOT)/../test/SandBox
INU_SRC_ROOT:=$(INU_HOST_LIB_ROOT)/../..
INU_SRC_API_ROOT:=$(INU_SRC_ROOT)/api
INU_SRC_COMMON_ROOT:=$(INU_SRC_ROOT)/common

# NOTE: the following modules are inlcuded in inu_host_lib.mk
# include libusb
#include $(CLEAR_VARS)
#LOCAL_MODULE := libusb1.0
#LOCAL_SRC_FILES := $(LIBUSB_ROOT)/bin/android_ndk10d_$(TARGET_ARCH_ABI)/libusb1.0.so
#LOCAL_EXPORT_C_INCLUDES := $(LIBUSB_ROOT)/libusb
#include $(PREBUILT_SHARED_LIBRARY)

# include inu_usb_lib
#include $(CLEAR_VARS)
#LOCAL_MODULE := libinu_usb_lib
#LOCAL_SRC_FILES := $(INU_USB_LIB_ROOT)/prj/android/libs/$(TARGET_ARCH_ABI)/libinu_usb_lib.so
#LOCAL_EXPORT_C_INCLUDES := $(INU_USB_LIB_ROOT)/include
#include $(PREBUILT_SHARED_LIBRARY)
# build inu_host_test
include $(CLEAR_VARS)
LOCAL_CFLAGS    := -I$(LOCAL_PATH)/ -DANDROID_HOST -Wno-format-security -MD -MP
LOCAL_CFLAGS += -fPIE 
LOCAL_LDFLAGS += -fPIE -pie
LOCAL_LDFLAGS += -fuse-ld=gold 
LOCAL_DISABLE_FORMAT_STRING_CHECKS := true
LOCAL_MODULE    := inu_host_test

LOCAL_C_INCLUDES := \
	$(INU_SRC_API_ROOT)									\
	$(INU_SRC_API_ROOT)/inu_defs								\
	$(INU_SRC_API_ROOT)/common								\
	$(INU_SRC_API_ROOT)/common/graph								\
	$(INU_SRC_API_ROOT)/host								\
	$(INU_SRC_COMMON_ROOT)/infrastructure/inc						\
	$(INU_SRC_COMMON_ROOT)/infrastructure/os/inc						\
	$(INU_SRC_COMMON_ROOT)/infrastructure/cls/inc						\
	$(INU_SRC_COMMON_ROOT)/infrastructure/cls/src/infrastructure			\
	$(INU_SRC_ROOT)/host/inu_host_lib/test			\
	

LOCAL_SRC_FILES := \
	$(INU_HOST_LIB_TEST_ROOT)/SandBox.cpp	\
	
LOCAL_SHARED_LIBRARIES += libusb1.0
LOCAL_SHARED_LIBRARIES += libinu_host_lib_4000

LOCAL_MODULE:= inu_host_test

include $(BUILD_EXECUTABLE)
