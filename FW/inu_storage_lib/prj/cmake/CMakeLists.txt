#PROJECT (inu_storage C CXX)
PROJECT (inu_storage)

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

IF(("Linux" MATCHES "${CMAKE_SYSTEM_NAME}") AND ("x86" MATCHES "${CMAKE_SYSTEM_PROCESSOR}") AND ("x86_64" MATCHES "${CMAKE_HOST_SYSTEM_PROCESSOR}"))
	set(CMAKE_C_FLAGS_PLATFORM_SPECIFIC "-m32")		
ENDIF(("Linux" MATCHES "${CMAKE_SYSTEM_NAME}") AND ("x86" MATCHES "${CMAKE_SYSTEM_PROCESSOR}") AND ("x86_64" MATCHES "${CMAKE_HOST_SYSTEM_PROCESSOR}"))

SET (inu_storage_lib_SRCS
    ../../src/inu_storage.c	
	../../../common/src/inu_host_utils.c	
)

# removing until supported in the code
SET (inu_flash_SRCS
    ../../test/inu_flash.c
	../../../common/src/inu_host_utils.c	
	../../../common/src/getopt.c	
)


INCLUDE_DIRECTORIES (
    ../../include
    ../../src/include
    ../../driver/libusb/include
    ../../../inu_usb_lib/include
    ../../../inu_usb_lib/src/include
    ../../../common/include
    ../../../../../common/include
)

LINK_DIRECTORIES(
    ../../../inu_usb_lib/driver/libusb/bin/${CMAKE_PLATFORM_NAME}
    ../../../inu_usb_lib/prj/cmake/build/
)


ADD_LIBRARY (inu_storage_lib SHARED	
	${inu_storage_lib_SRCS}	
)

#set_target_properties(inu_storage_lib PROPERTIES LINKER_LANGUAGE CXX)	

# removing until supported in the code
#ADD_EXECUTABLE (inu_flash
#	${inu_flash_SRCS}
#)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
	set(CMAKE_C_FLAGS "-DANDROID_HOST -c -emit-llvm -O3 ")
	set(CMAKE_CXX_FLAGS "-DANDROID_HOST -c -emit-llvm -O3 ")
else()
    if("arm" MATCHES "${CMAKE_SYSTEM_PROCESSOR}" OR "arm64" MATCHES "${CMAKE_SYSTEM_PROCESSOR}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_PLATFORM_SPECIFIC}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -msse4 ${CMAKE_C_FLAGS_PLATFORM_SPECIFIC}")
    endif()
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    set(USB_LIB "usb1.0")
else()
    set(USB_LIB "usb-1.0")
endif()

find_package (Threads REQUIRED)
if(CMAKE_USE_PTHREADS_INIT)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -MD -MP")	
endif()

# These are the symbols for the linker
TARGET_LINK_LIBRARIES(
#inu_flash - removing until supported in the code
#	${CMAKE_THREAD_LIBS_INIT}	
    inu_storage_lib
	inu_usb_lib
	${USB_LIB}
# ensure to use -l options at the end
	-lrt
	-lpthread
)

#INSTALL(TARGETS inu_usb_test DESTINATION bin)
#INSTALL(TARGETS inu_usb_lib DESTINATION lib)
#INSTALL(FILES inu_usb.h DESTINATION include)
