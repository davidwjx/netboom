#PROJECT (inu_host C CXX)
PROJECT (inu_host)
set(ROOT_DIR ${PROJECT_SOURCE_DIR}/../../../../../../..)
set(ROOT_BIN_DIR ${ROOT_DIR}/bin/${CMAKE_PLATFORM_NAME})
set(PREBUILT_BIN_DIR ${ROOT_DIR}/prebuilt/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR})
set(FW_DIR ${ROOT_DIR}/FW)
set(APP_DIR ${PROJECT_SOURCE_DIR}/../../../..)
set(HOST_DIR ${APP_DIR}/host)
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
SET(COMMON_SOURCE_DIR ${APP_DIR}/common)
SET(COMMON_HOST_SOURCE_DIR ${APP_DIR}/host/common)
SET(API_SOURCE_DIR ${APP_DIR}/api)

#IF(!CMAKE_CROSSCOMPILING)
#message("Cross compilation is detected")
#ELSE()
#message("Native compilation is detected")
#ENDIF()
IF("i686" MATCHES "${CMAKE_SYSTEM_PROCESSOR}")
	set(CMAKE_SYSTEM_PROCESSOR "x86")
	set(CMAKE_PLATFORM_NAME "linux_gcc-4.8_x86")
	message("updated CMAKE_SYSTEM_PROCESSOR : ${CMAKE_SYSTEM_PROCESSOR}")
ENDIF()

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Xlinker -Bsymbolic -Bsymbolic-functions")

IF (("Linux" MATCHES "${CMAKE_SYSTEM_NAME}") AND ("x86" MATCHES "${CMAKE_SYSTEM_PROCESSOR}") )	
	set(CMAKE_C_FLAGS_PLATFORM_SPECIFIC ${CMAKE_C_FLAGS_PLATFORM_SPECIFIC} "-m32")	
ENDIF()

file(GLOB manager_csrc ${COMMON_SOURCE_DIR}/application/manager/src/*.c)
file(GLOB nucfg_csrc ${COMMON_SOURCE_DIR}/application/nucfg/*.c)
file(GLOB_RECURSE infrastructure_csrc ${COMMON_SOURCE_DIR}/infrastructure/*.c)
SET ( inu_host_lib_SRCS        
       ${COMMON_SOURCE_DIR}/application/manager/src/svc_cfg.c
       ${COMMON_SOURCE_DIR}/application/manager/src/svc_com.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu2.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_comm.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_ref.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_device.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_logger.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_graph.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_node.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_data.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_function.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_sensor.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_sensors_group.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_soc_channel.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_cva_channel.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_cva_data.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_mipi_channel.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_soc_writer.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_streamer.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_imu.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_temperature.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_compress.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_fast_data.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_fast_orb.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_metadata_injector.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_cdnn.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_cdnn_data.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_slam.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_slam_data.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_dpe_pp.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_pp.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_point_cloud_data.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_load_network.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_load_background.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_fdk.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_factory.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_histogram.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_image.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_imu_data.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_temperature_data.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_histogram_data.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_tsnr_data.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_tsnr_calc.c
       ${COMMON_SOURCE_DIR}/application/manager/src/helsinki.c
       ${COMMON_SOURCE_DIR}/application/nucfg/nucfg.c
       ${COMMON_SOURCE_DIR}/application/nucfg/xml_db_modes.c
       ${COMMON_SOURCE_DIR}/application/nucfg/calibration.c
       ${COMMON_SOURCE_DIR}/application/nucfg/libini.c
       ${COMMON_SOURCE_DIR}/application/nucfg/nu_soc.c
       ${COMMON_SOURCE_DIR}/application/nucfg/nu_graph.c
       ${COMMON_SOURCE_DIR}/application/nucfg/nu4k.c
       ${COMMON_SOURCE_DIR}/application/nucfg/nufld.c
       ${COMMON_SOURCE_DIR}/application/nucfg/inu_graph_creator.c
       ${COMMON_SOURCE_DIR}/infrastructure/cls/src/infrastructure/serial.c     
       ${COMMON_SOURCE_DIR}/infrastructure/cls/src/infrastructure/socket.c     
       ${COMMON_SOURCE_DIR}/infrastructure/cls/src/interface/uart_com.c
       ${COMMON_SOURCE_DIR}/infrastructure/cls/src/interface/tcp_com.c
       ${COMMON_SOURCE_DIR}/infrastructure/cls/src/interface/cdc_com.c
       ${COMMON_SOURCE_DIR}/infrastructure/cls/src/interface/stub_com.c
       ${COMMON_SOURCE_DIR}/infrastructure/cls/src/interface/usb_ctrl_com_host.c       
       ${COMMON_SOURCE_DIR}/infrastructure/cls/src/network/conn_lyr.c
       ${COMMON_SOURCE_DIR}/infrastructure/cls/src/network/queue_lyr.c 
       ${COMMON_SOURCE_DIR}/infrastructure/lm/src/lm.c
       ${COMMON_SOURCE_DIR}/infrastructure/lz4/lz4.c           
       ${COMMON_SOURCE_DIR}/infrastructure/lz4/lz4frame.c
       ${COMMON_SOURCE_DIR}/infrastructure/lz4/lz4hc.c 
       ${COMMON_SOURCE_DIR}/infrastructure/lz4/xxhash.c        
       ${COMMON_SOURCE_DIR}/infrastructure/marshal/src/marshal.c
       ${COMMON_SOURCE_DIR}/infrastructure/marshal/src/m_tbl.c
       ${COMMON_SOURCE_DIR}/infrastructure/marshal/src/m_ioctl.c
       ${COMMON_SOURCE_DIR}/infrastructure/os/src/inu_queue.c
       ${COMMON_SOURCE_DIR}/infrastructure/os/src/os_lyr_linux.c
       ${COMMON_SOURCE_DIR}/infrastructure/os/src/mem_pool.c   
       ${COMMON_SOURCE_DIR}/infrastructure/os/src/linked_list.c
       ${COMMON_SOURCE_DIR}/infrastructure/time/src/inu_time.c
       ${COMMON_SOURCE_DIR}/infrastructure/utils/src/inu_str.c
       ${COMMON_SOURCE_DIR}/infrastructure/utils/src/log.c
       ${COMMON_SOURCE_DIR}/infrastructure/utils/src/utils.c
       ${COMMON_SOURCE_DIR}/infrastructure/utils/src/inu_profiler.c
       ${COMMON_SOURCE_DIR}/infrastructure/utils/src/config.c
       ${COMMON_SOURCE_DIR}/infrastructure/utils/src/hash.c
       ${COMMON_SOURCE_DIR}/infrastructure/utils/src/inu_defs_params.c
       ${COMMON_SOURCE_DIR}/infrastructure/xmldb/xml_db.c
       ${COMMON_SOURCE_DIR}/infrastructure/xmldb/sxmlc/src/sxmlc.c
       ${COMMON_SOURCE_DIR}/infrastructure/utils/src/graph.c
       ${COMMON_SOURCE_DIR}/infrastructure/metadata/src/metadata.pb.c
       ${COMMON_SOURCE_DIR}/infrastructure/metadata/src/inu_metadata_serializer.c
       ${COMMON_SOURCE_DIR}/infrastructure/sensorsync/src/inu_sensorsync_serializer.c
       ${COMMON_SOURCE_DIR}/infrastructure/sensorsync/src/sensor_sync.pb.c
       ${COMMON_SOURCE_DIR}/infrastructure/metadata/nanopb/pb_decode.c
       ${COMMON_SOURCE_DIR}/infrastructure/metadata/nanopb/pb_common.c
       ${COMMON_SOURCE_DIR}/infrastructure/metadata/nanopb/pb_encode.c
       ${COMMON_SOURCE_DIR}/application/manager/src/inu_isp_channel.c
)

SET (inu_sandbox_fw_lib_SRCS
	${HOST_DIR}/test/SandBox/SandBoxFWLib/SandBoxLib.cpp
)

SET (inu_sandbox_SRCS
	${HOST_DIR}/test/SandBox/post_process.cpp
	${HOST_DIR}/test/SandBox/runtime_commands.cpp
	${HOST_DIR}/test/SandBox/SandBox.cpp
)

if(("Linux" MATCHES ${CMAKE_SYSTEM_NAME}))

    if ("arm64" MATCHES ${CMAKE_SYSTEM_PROCESSOR} OR "armv8" MATCHES ${CMAKE_SYSTEM_PROCESSOR})
		set(OPENCV_INC_DIR ${ROOT_DIR}/Common/3rdParty/opencv451)
	else()
		find_package(OpenCV)
		if(OpenCV_FOUND)
		    set(OPENCV_INC_DIR ${OpenCV_INCLUDE_DIRS})
		else()
		    set(OPENCV_INC_DIR )
		endif()
	endif()

endif()

INCLUDE_DIRECTORIES (
	${FW_DIR}/inu_usb_lib/include
	${FW_DIR}/common/include
	${API_SOURCE_DIR}/common
	${API_SOURCE_DIR}/common/graph
	${API_SOURCE_DIR}/host
	${COMMON_HOST_SOURCE_DIR}/include
	${COMMON_SOURCE_DIR}/application/manager/inc
	${COMMON_SOURCE_DIR}/application/service/alg
	${COMMON_SOURCE_DIR}/../../../common/version
	${COMMON_SOURCE_DIR}/../../common
	${COMMON_SOURCE_DIR}/../../../common/include
	${COMMON_SOURCE_DIR}/application/nucfg
	${COMMON_SOURCE_DIR}/infrastructure/err/inc
	${COMMON_SOURCE_DIR}/infrastructure/utils/inc	
	${COMMON_SOURCE_DIR}/infrastructure/service/inc
	${COMMON_SOURCE_DIR}/infrastructure/os/inc
	${COMMON_SOURCE_DIR}/infrastructure/time/inc
	${COMMON_SOURCE_DIR}/infrastructure/cls/src/network
	${COMMON_SOURCE_DIR}/infrastructure/cls/src/interface
	${COMMON_SOURCE_DIR}/infrastructure/cls/src/infrastructure
	${COMMON_SOURCE_DIR}/infrastructure/cls/inc
	${COMMON_SOURCE_DIR}/infrastructure/inc
	${COMMON_SOURCE_DIR}/infrastructure/marshal/inc
	${COMMON_SOURCE_DIR}/infrastructure/marshal/
	${COMMON_SOURCE_DIR}/infrastructure/mem/inc/
	${COMMON_SOURCE_DIR}/infrastructure/lm/inc
	${COMMON_SOURCE_DIR}/infrastructure/lz4
	${COMMON_SOURCE_DIR}/infrastructure/xmldb
	${COMMON_SOURCE_DIR}/infrastructure/xmldb/sxmlc/src
	${COMMON_SOURCE_DIR}/../host/inu_host_lib/test
	${COMMON_SOURCE_DIR}/../target/common/mem/inc
	${COMMON_SOURCE_DIR}/../target/common/bsp/inc/regs
    ${COMMON_SOURCE_DIR}/infrastructure/metadata/nanopb
    ${COMMON_SOURCE_DIR}/infrastructure/metadata/inc
    ${COMMON_SOURCE_DIR}/target/common/sensorsync/inc 
    ${COMMON_SOURCE_DIR}/infrastructure/sensorsync/inc 
	${OPENCV_INC_DIR}
)

LINK_DIRECTORIES(
	${FW_DIR}/inu_usb_lib/prj/cmake/build
	${FW_DIR}/inu_usb_lib/driver/libusb/bin/${CMAKE_PLATFORM_NAME}
	${ROOT_DIR}/bin/${CMAKE_PLATFORM_NAME}
	${PREBUILT_BIN_DIR}
)

ADD_LIBRARY (inu_host_lib_4100 SHARED	
	${inu_host_lib_SRCS}	
)

#TARGET_LINK_LIBRARIES(inu_host_lib_4100 inu_usb_lib)

ADD_LIBRARY (inu_sandbox_fw_lib SHARED	
	${inu_sandbox_fw_lib_SRCS}	
)#set_target_properties(inu_host_lib PROPERTIES LINKER_LANGUAGE CXX)	

ADD_EXECUTABLE (inu_sandbox
	${inu_sandbox_SRCS}
)
#set_target_properties(sandbox PROPERTIES LINKER_LANGUAGE CXX)	

if(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "debug"))
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -O0 -rdynamic -funwind-tables")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0 -rdynamic -funwind-tables")
elseif(CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "release"))  
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3 -DNDEBUG")
else()
    message("else:${CMAKE_BUILD_TYPE}")
    message("else:${CMAKE_C_FLAGS_RELEASE}")
endif()
message("build configure: ${CMAKE_C_FLAGS_RELEASE}")

#target_compile_options(foo PUBLIC "$<$<CONFIG:DEBUG>:${MY_DEBUG_OPTIONS}>")
#target_compile_options(foo PUBLIC "$<$<CONFIG:RELEASE>:${MY_RELEASE_OPTIONS}>")
# Example: how to activate C++11. Uncomment one of this
#     target_compile_options(${BII_LIB_TARGET} PUBLIC "-std=c++11") # Win, or linux if not gcc problems
#     target_compile_options(${BII_LIB_TARGET} PUBLIC "-std=c++11"  "-stdlib=libc++") # MacOS
#     target_compile_options(${BII_LIB_TARGET} PUBLIC "-std=c++11" "-Wl,--no-as-needed") # Ubuntu if gcc problems

#SET(GCC_COVERAGE_COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
#SET(GCC_COVERAGE_LINK_FLAGS    "-lgcov")
#add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
#   or
#SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
#SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
find_package (Threads REQUIRED)
#if(CMAKE_USE_PTHREADS_INIT)
#    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-pthread -MD -MP")	
#endif()
# add SSE4 for ubuntu
if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")

	set(CMAKE_C_FLAGS "-DANDROID_HOST -c -emit-llvm -O3")
	set(CMAKE_CXX_FLAGS "-DANDROID_HOST -c -emit-llvm -O3")
	set(USB_LIB "usb1.0")
	set(OPENCV_LIBS_REQUIRED)
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")

	set(USB_LIB "usb-1.0")
    if("arm" MATCHES "${CMAKE_SYSTEM_PROCESSOR}" OR "arm64" MATCHES "${CMAKE_SYSTEM_PROCESSOR}" OR "armv8" MATCHES "${CMAKE_SYSTEM_PROCESSOR}")
	    set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-DLINUX_HOST -D_GNU_SOURCE -Wno-format-security -g3 ${CMAKE_C_FLAGS_PLATFORM_SPECIFIC}")
		set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-DLINUX_HOST -D_OPENCV_ -D_GNU_SOURCE -Wno-format-security -g3 ${CMAKE_C_FLAGS_PLATFORM_SPECIFIC}")
		set(OPENCV_LIBS_REQUIRED opencv_highgui opencv_imgproc opencv_core)
	else()
        set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-DLINUX_HOST -D_GNU_SOURCE -Wno-format-security -g3 -msse4 ${CMAKE_C_FLAGS_PLATFORM_SPECIFIC}")
        if(OpenCV_FOUND)
            set(OPENCV_LIBS_REQUIRED ${OpenCV_LIBS})
            set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-DLINUX_HOST -D_GNU_SOURCE -D_OPENCV_ -Wno-format-security -g3 -msse4 ${CMAKE_C_FLAGS_PLATFORM_SPECIFIC}")
        else()
            set(OPENCV_LIBS_REQUIRED)
            set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-DLINUX_HOST -D_GNU_SOURCE -Wno-format-security -g3 -msse4 ${CMAKE_C_FLAGS_PLATFORM_SPECIFIC}")
        endif()
	endif()
endif()	

TARGET_LINK_LIBRARIES(inu_sandbox
#	${CMAKE_THREAD_LIBS_INIT}	
	inu_host_lib_4100
	inu_sandbox_fw_lib
	inu_usb_lib
	${OPENCV_LIBS_REQUIRED}
	${USB_LIB}
# ensure to use -l options at the end
	rt
	pthread
	m
)

if(CMAKE_VS_PLATFORM_NAME)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        # On Windows need to set the Runtime folder in order to control the build path
        RUNTIME_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        ARCHIVE_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        DEBUG_POSTFIX "d"
    )
else()
    #set_target_properties(inu_host_lib_4100 PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${ROOT_BIN_DIR})
    #set_target_properties(inu_sandbox_fw_lib PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${ROOT_BIN_DIR})
    #set_target_properties(inu_sandbox PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ROOT_BIN_DIR})
	#file(COPY ${PROJECT_BINARY_DIR}/libinu_host_lib_4100.so DESTINATION ${ROOT_BIN_DIR} FOLLOW_SYMLINK_CHAIN)
	INSTALL(TARGETS inu_sandbox_fw_lib inu_host_lib_4100 inu_sandbox DESTINATION ${ROOT_BIN_DIR})
endif()

