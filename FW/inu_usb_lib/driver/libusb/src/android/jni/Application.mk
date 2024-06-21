# Android application build config for libusb
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

# Workaround for MIPS toolchain linker being unable to find liblog dependency
# of shared object in NDK versions at least up to r9.
#

APP_ABI := all

APP_PLATFORM := android-21

ifneq ($(NDK_TARGET_ARCH_ABI),)
	APP_ABI := $(NDK_TARGET_ARCH_ABI)
endif

ifneq ($(NDK_TARGET_PLATFORM),)
	APP_PLATFORM := $(NDK_TARGET_PLATFORM)
endif

ifeq ($(NDK_TARGET_PLATFORM),android-29)
   APP_LDFLAGS := -llog -L${ANDROID_NDK}/${CROSS_HOST}/sysroot/usr/lib/aarch64-linux-android/29
else
   ifeq ($(NDK_TARGET_PLATFORM),android-27)
       APP_LDFLAGS := -llog -L${ANDROID_NDK}/${CROSS_HOST}/sysroot/usr/lib/aarch64-linux-android/27
   else
       APP_LDFLAGS := -llog
   endif
endif

ifneq ("$(NDK_TARGET_PLATFORM)","android-21")
	APP_LDFLAGS += -target aarch64-linux-android
endif

