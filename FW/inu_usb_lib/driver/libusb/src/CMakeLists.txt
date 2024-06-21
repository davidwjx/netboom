cmake_minimum_required(VERSION 3.6)
set(CMAKE_CXX_STANDARD 11)

project(usb-1.0)
set(CMAKE_INSTALL_PREFIX ..)
set(ROOT_DIR ../../../../..)
set(ROOT_BIN_DIR ${ROOT_DIR}/bin/${CMAKE_PLATFORM_NAME})

set(PLATFORM_ID ${CMAKE_SYSTEM_NAME})
message("Detected system info:")
message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message("CMAKE_SYSTEM_PROCESSOR : ${CMAKE_SYSTEM_PROCESSOR}")
message("CMAKE_HOST_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")
message("CMAKE_HOST_SYSTEM_PROCESSOR : ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message("CMAKE_HOST_SYSTEM : ${CMAKE_HOST_SYSTEM}")
message("CMAKE_PLATFORM_NAME : ${CMAKE_PLATFORM_NAME}")

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "debug"))
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -O0 -rdynamic -funwind-tables")
    message("Debug mode:${CMAKE_C_FLAGS_DEBUG}")
elseif(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "release"))  
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O3 -DNDEBUG")
    message("Release mode:${CMAKE_C_FLAGS_RELEASE}")
else()
    message("else:${CMAKE_BUILD_TYPE}")
    message("else:${CMAKE_C_FLAGS_RELEASE}")
endif()

if("Android" MATCHES ${PLATFORM_ID})
	add_definitions(-D__ANDROID__ -DHAVE_GETTIMEOFDAY)
	set(usb_pf_inc_dir android)
elseif("Linux" MATCHES ${PLATFORM_ID})

	set(usb_pf_inc_dir linux)
    add_definitions(-D__LINUX__ -DHAVE_GETTIMEOFDAY -DHAVE_CONFIG_H) #-DHAVE_LIBUDEV -DUSE_UDEV 
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -Wall -Wundef -Wunused -Wstrict-prototypes -Werror-implicit-function-declaration -Wno-pointer-sign -Wshadow -pthread -fvisibility=hidden")
	if("armv8" MATCHES ${CMAKE_SYSTEM_PROCESSOR} OR "arm64" MATCHES ${CMAKE_SYSTEM_PROCESSOR})
		#configure_file (linux/config.h)
	elseif("x86_64" MATCHES ${CMAKE_SYSTEM_PROCESSOR})
        #configure_file (config.h.in linux/config.h)
	endif()
endif()
message("usb platform include dir: ${usb_pf_inc_dir}")



include_directories(.
	${usb_pf_inc_dir}
	libusb
)

set(SOURCE_FILES
        libusb/os/linux_netlink.c
        libusb/os/linux_usbfs.c
        libusb/os/linux_usbfs.h
        libusb/os/poll_posix.c
        libusb/os/poll_posix.h
        libusb/os/threads_posix.c
        libusb/os/threads_posix.h
        libusb/core.c
        libusb/descriptor.c
        libusb/hotplug.c
        libusb/hotplug.h
        libusb/io.c
        libusb/libusb.h
        libusb/libusbi.h
        libusb/strerror.c
        libusb/sync.c
        libusb/version.h
        libusb/version_nano.h)

if("Android" MATCHES ${PLATFORM_ID})
find_library( # Sets the name of the path variable.
        log-lib
        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log )
else()
set(log-lib)
endif()

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
#set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".so.0")

if(CMAKE_VS_PLATFORM_NAME)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        # On Windows need to set the Runtime folder in order to control the build path
        RUNTIME_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        ARCHIVE_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        DEBUG_POSTFIX "d"
    )
else()
	INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${ROOT_BIN_DIR})
endif()

target_link_libraries(usb-1.0 ${log-lib})
