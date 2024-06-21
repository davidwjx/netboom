#PROJECT (inu_usb C CXX)
PROJECT (inu_usb)

set(ROOT_DIR ${PROJECT_SOURCE_DIR}/../../../..)
set(ROOT_BIN_DIR ${ROOT_DIR}/bin/${CMAKE_PLATFORM_NAME})

message("Detected system info:")
message("CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message("CMAKE_SYSTEM_PROCESSOR : ${CMAKE_SYSTEM_PROCESSOR}")
message("CMAKE_HOST_SYSTEM_NAME: ${CMAKE_HOST_SYSTEM_NAME}")
message("CMAKE_HOST_SYSTEM_PROCESSOR : ${CMAKE_HOST_SYSTEM_PROCESSOR}")
message("CMAKE_HOST_SYSTEM : ${CMAKE_HOST_SYSTEM}")
message("CMAKE_PLATFORM_NAME : ${CMAKE_PLATFORM_NAME}")

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE OFF)

IF("i686" MATCHES "${CMAKE_SYSTEM_PROCESSOR}")
	set(CMAKE_SYSTEM_PROCESSOR "x86")
	set(CMAKE_PLATFORM_NAME "linux_gcc-4.8_x86")
	message("updated CMAKE_SYSTEM_PROCESSOR : ${CMAKE_SYSTEM_PROCESSOR}")
ENDIF()

IF(("Linux" MATCHES "${CMAKE_SYSTEM_NAME}") AND ("x86" MATCHES "${CMAKE_SYSTEM_PROCESSOR}"))
	set(CMAKE_C_FLAGS_PLATFORM_SPECIFIC "-m32")		
ENDIF()

SET (inu_usb_lib_SRCS
    ../../src/inu_boot.c
    ../../src/inu_monitor.c
    ../../src/inu_usb_libusb_linux.c
	../../src/inu_bootdetect.c
    ../../src/inu_os_lyr.c
	../../../common/src/inu_host_utils.c	
)

SET (inu_usb_test_SRCS
    ../../test/inu_usb_test.c
	../../../common/src/inu_host_utils.c
)

SET (inu_usb_wait_SRCS
    ../../test/inu_usb_wait.c
	../../../common/src/inu_host_utils.c
)

INCLUDE_DIRECTORIES (
    ../../include
	../../src/include	
    ../../driver/libusb/include
	../../driver/libusb/src/libusb
	../../../common/include
)

LINK_DIRECTORIES(
	../../driver/libusb/bin/${CMAKE_PLATFORM_NAME}
    ${ROOT_BIN_DIR}
)

add_library (inu_usb_lib SHARED	
	${inu_usb_lib_SRCS}	
)
#set_target_properties(inu_usb_lib PROPERTIES LINKER_LANGUAGE CXX)	

# ADD_EXECUTABLE (inu_usb_test
# 	${inu_usb_test_SRCS}
# 	${inu_usb_lib_SRCS}
# )

# ADD_EXECUTABLE (inu_usb_wait
# 	${inu_usb_wait_SRCS}
# )
#set_target_properties(inu_usb_test PROPERTIES LINKER_LANGUAGE CXX)	


find_package (Threads REQUIRED)
if(CMAKE_USE_PTHREADS_INIT)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -MD -MP")	
endif()

# use -DUSE_HOTPLUG_POLLING to support for watchdog reset, that is not dtected by libusb
if("arm" MATCHES ${CMAKE_SYSTEM_PROCESSOR} OR "arm64" MATCHES ${CMAKE_SYSTEM_PROCESSOR} OR "armv8" MATCHES ${CMAKE_SYSTEM_PROCESSOR})
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_HOTPLUG_POLLING ${CMAKE_C_FLAGS_PLATFORM_SPECIFIC}") 
else()
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_HOTPLUG_POLLING -msse4 ${CMAKE_C_FLAGS_PLATFORM_SPECIFIC}")
endif()


# These are the symbols for the linker
TARGET_LINK_LIBRARIES(inu_usb_lib
	usb-1.0	
)

# TARGET_LINK_LIBRARIES(inu_usb_test
# 	${CMAKE_THREAD_LIBS_INIT}
#         ${LINKED_LIBRARIES}
		
# )

# TARGET_LINK_LIBRARIES(inu_usb_wait
# 	${CMAKE_THREAD_LIBS_INIT}
#         ${LINKED_LIBRARIES}
# )

#ADD_CUSTOM_COMMAND(TARGET inu_usb_test
#          POST_BUILD
#          COMMAND ${CMAKE_COMMAND} -E copy ${inu_usb_SOURCE_DIR}/../../driver/libusb/bin/${CMAKE_SYSTEM_PROCESSOR}/libusb-1.0.so ${inu_usb_BINARY_DIR}
#)

#INSTALL(TARGETS inu_usb_test DESTINATION bin)
#INSTALL(TARGETS inu_usb_lib DESTINATION lib)
#INSTALL(FILES inu_usb.h DESTINATION include)
if(CMAKE_VS_PLATFORM_NAME)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        # On Windows need to set the Runtime folder in order to control the build path
        RUNTIME_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        ARCHIVE_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        DEBUG_POSTFIX "d"
    )
else()
	INSTALL(TARGETS inu_usb_lib DESTINATION ${ROOT_BIN_DIR})
endif()
