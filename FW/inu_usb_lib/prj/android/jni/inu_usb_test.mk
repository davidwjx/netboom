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

INU_USB_LIB_ROOT:=$(LOCAL_PATH)/../../..
LIBUSB_ROOT:= $(INU_USB_LIB_ROOT)/driver/libusb
INU_USB_LIB_TEST_ROOT:=$(INU_USB_LIB_ROOT)/test

# include libusb
#include $(CLEAR_VARS)
#LOCAL_MODULE := libusb1.0
#LOCAL_SRC_FILES := $(LIBUSB_ROOT)/bin/android_ndk10d_$(TARGET_ARCH_ABI)/libusb1.0.so
#LOCAL_EXPORT_C_INCLUDES := $(LIBUSB_ROOT)/include
#include $(PREBUILT_SHARED_LIBRARY)
# NOTE: libusb1.0.so and libinu_usb_lib.so are compiled by inu_usb_lib.mk


# build inu_usb_test
include $(CLEAR_VARS)
LOCAL_CFLAGS    := -I$(LOCAL_PATH)/ -DANDROID_HOST -Wno-format-security -MD -MP
LOCAL_DISABLE_FORMAT_STRING_CHECKS := true
LOCAL_MODULE    := inu_usb_test

LOCAL_C_INCLUDES := \
	$(NDK_ROOT)/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86/lib/gcc/arm-linux-androideabi/4.8/include/ \
	$(LIBUSB_ROOT)/include 								\
	$(INU_USB_LIB_ROOT)/include 						\
	$(INU_USB_LIB_ROOT)/src							\
	$(INU_USB_LIB_ROOT)/../common/include			\
	
LOCAL_SRC_FILES := \
	$(INU_USB_LIB_TEST_ROOT)/inu_usb_test.c \
	$(INU_USB_LIB_ROOT)/../common/src/inu_host_utils.c \
	
LOCAL_SHARED_LIBRARIES += libusb1.0
LOCAL_SHARED_LIBRARIES += inu_usb_lib

LOCAL_MODULE:= inu_usb_test

include $(BUILD_EXECUTABLE)
