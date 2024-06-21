cmake_minimum_required(VERSION 3.6)
project(inu_usb_lib)

set(CMAKE_CXX_STANDARD 11)
add_definitions(-D__ANDROID__ -DANDROID_HOST -Wno-format-security -MD -MP -O3)
include_directories(.
include/
src/
src/include
../../../common/include
../../../app/host/common/include
./driver/libusb/include/
driver/libusb/src/libusb/
)

set(SOURCE_FILES
        include/inu_usb.h
        src/include/bootdetect_nu3000a0.h
        src/include/bootdetect_nu3000b0.h
        src/include/inu_boot.h
        src/include/inu_bootdetect.h
        src/inu_boot.c
        src/inu_bootdetect.c
        src/inu_monitor.c
        ../common/src/inu_host_utils.c
        src/inu_usb_libusb.c)

find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )



add_library(inu_usb_lib SHARED ${SOURCE_FILES})

target_link_libraries(inu_usb_lib usb-1.0 ${log-lib})