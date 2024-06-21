cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
include(evrt)
project("${project_name}")
set(project_version "1.0")

#set(CMAKE_INSTALL_PREFIX ${CNNSDK_INSTALL_DIR})

#==============================================================================
# General CNN CMake configuration file
#==============================================================================

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_INSTALL_PREFIX}/${EVSS_CFG_NAME}/lib/cmake ${EVSS_INSTALL_PREFIX}/${EVSS_CFG_NAME}/lib/cmake")

find_package(evcnndev        1.0 REQUIRED)
find_package(evlayers        1.0 REQUIRED)
# find_package(evcnnutil       1.0 REQUIRED)

if(DEFINED EVSS_CFG_HAS_OCL)
    find_package(ocl_kernels     1.0 REQUIRED)
endif()

if(DEFINED USE_OPENVX)
    find_package(evcnnovx        1.0 REQUIRED)
endif()

if(DEFINED ARC)
    find_package(evcnnhwutil     1.0 REQUIRED)
endif()

set(CNN_C_FLAGS "${CNN_C_FLAGS} -DCNN_NAME=${CNN_NAME}")
set(CNN_C_FLAGS "${CNN_C_FLAGS} -DHASH_FILE_NAME=${HASH_FILE_NAME}")
set(CNN_C_FLAGS "${CNN_C_FLAGS} -DCNN_EVRT")

if(DEFINED HASH_DEBUG)
    set(CNN_C_FLAGS "${CNN_C_FLAGS} -DHASH_DEBUG")
endif()

if(DEFINED INSTRUMENT)
    set(CNN_C_FLAGS "${CNN_C_FLAGS} -DINSTRUMENT")
endif()

if(DEFINED EVTRACE)
    set(CNN_C_FLAGS "${CNN_C_FLAGS} -DEVTRACE")
endif()

if(DEFINED IMG_UMR)
    set(CNN_C_FLAGS "${CNN_C_FLAGS} -DIMG_UMR")
    set(CNN_C_FLAGS "${CNN_C_FLAGS} -DCNNIMGADDR=${CNNIMGADDR}")
    set(CNN_C_FLAGS "${CNN_C_FLAGS} -DCNNIMGSIZE=${CNNIMGSIZE}")
endif()

if(DEFINED NOHOSTLIB_CNN)
    set(CNN_C_FLAGS "${CNN_C_FLAGS} -DNOHOSTLIB -Hhostlib=")
    set(CNN_C_FLAGS "${CNN_C_FLAGS} -DNOHOSTLIB_CNN")
endif()

if(DEFINED EMBED_ARGS)
    set(CNN_C_FLAGS "${CNN_C_FLAGS} -DEMBED_ARGS=\"${EMBED_ARGS}\"")
    set(CNN_C_FLAGS "${CNN_C_FLAGS} -DMY_ARGV=\"${MY_ARGV}\"")
    set(CNN_C_FLAGS "${CNN_C_FLAGS} -DMY_ARGC=\"${MY_ARGC}\"")
    set(CNN_C_FLAGS "${CNN_C_FLAGS} -DCNN_BIN_SIZE=${CNN_BIN_SIZE}")
endif()

if(DEFINED USE_OPENCV)
    set(CNN_C_FLAGS "${CNN_C_FLAGS} -DUSE_OPENCV ")
endif()

file(GLOB CNN_TEST_SRC_FILES src/*.cc)


if(DEFINED EVSS_CFG_HAS_OCL)
    file(GLOB CNN_OCL_SRC_FILES "src/ocl/*.cl")
endif()

file(GLOB CNN_OFFLOAD_SRC_FILES ${CNN_SRC_DIR_CFG}/*/distribute/*.cc)

# -- executable targets
#

# New build system has the conflict with interface library
# We create fake empty dummy.cc to convert library from interface
set(DUMMY_FILE "${CMAKE_CURRENT_BINARY_DIR}/dummy.cc")
if(NOT EXISTS ${DUMMY_FILE})
    file(TOUCH ${DUMMY_FILE})
endif()

add_library(${project_name} STATIC ${DUMMY_FILE})


#add_library(${project_name} INTERFACE)

#Golovkin: separate_arguments() is used to remove quotes from ${CNN_C_FLAGS} 
separate_arguments(CNN_C_FLAGS WINDOWS_COMMAND  ${CNN_C_FLAGS})
target_compile_definitions(${project_name} INTERFACE ${CNN_C_FLAGS} )

#==============================================================================
# On the ARC, the CNN implementation is in the APEX extension, or we are on
# real HW, and require ELF loader libs & utils
#==============================================================================

if(NOT DEFINED ARC)

file(GLOB CNN_NATIVE_OBJS ${CNN_OBJ_PATH}/*.o )

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if( "${EVSS_CFG_NAME}" MATCHES "ev_native" )
        file(GLOB CNN_NATIVE_OBJS ${CNN_OBJ_PATH}/*.obj)
    endif()
    set(CNN_NATIVE_OBJS_NEEDED ${CNN_NATIVE_OBJS})
else()
    set(CNN_NATIVE_OBJS_NEEDED -Wl,--no-as-needed ${CNN_NATIVE_OBJS} -Wl,--as-needed)
endif()

target_link_libraries(${project_name} INTERFACE ${CNN_NATIVE_OBJS_NEEDED})

endif()

# -- Link CNN and EV_RUNTIME libraries
target_link_libraries(${project_name} INTERFACE evcnndev::evcnndev)
target_link_libraries(${project_name} INTERFACE evlayers::evlayers)
# target_link_libraries(${project_name} INTERFACE evcnnutil::evcnnutil)

if(CNN_USE_OPENCV)
    find_package(cnnimageopencv)
    target_link_libraries(${project_name} INTERFACE cnnimageopencv::cnnimageopencv)
else()
    find_package(cnnimagebmetal)
    target_link_libraries(${project_name} INTERFACE cnnimagebmetal::cnnimagebmetal)
endif()

if(DEFINED EVSS_CFG_HAS_OCL)
target_link_libraries(${project_name} INTERFACE ocl_kernels::ocl_kernels)
endif()

if(DEFINED USE_OPENVX)
target_link_libraries(${project_name} INTERFACE evcnnovx::evcnnovx)
endif()

if(DEFINED ARC)
target_link_libraries(${project_name} INTERFACE evcnnhwutil::evcnnhwutil)
endif()

if((${CNN_ABSTRACTION} STREQUAL "unmerged_large") AND ("${EVSS_CFG_NAME}" MATCHES "ev_native"))
    target_link_libraries(${project_name} INTERFACE ${EV_CNNENGINE_NATIVELIB})
endif()

if(DEFINED USE_OPENCV)
    #target_link_libraries(${project_name} INTERFACE ${EVSS_OCV_LIBS} ${EVSS_OCV_3P_LIBS})
	find_package(evOpenCV 2.4.9 REQUIRED)
    target_link_libraries(${project_name} INTERFACE evOpenCV::evOpenCV)
endif()


target_include_directories(${project_name}
    INTERFACE
        $<BUILD_INTERFACE:${COMMON_APP_HDR}>
        $<BUILD_INTERFACE:${CNNSDK_INSTALL_DIR}/include>
        $<BUILD_INTERFACE:${CNNSDK_INSTALL_DIR}/include/internal>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${EVGENCNN_EXTRA_INCLUDES}
)



if(DEFINED USE_OPENCV)
    checkOpenCV(${project_name})
endif()

set(EV_CNNAPP_NAG_VARS "${HASH_DEBUG} ${INSTRUMENT} ${EVTRACE} ${CNN_LIB_DIR_CFG} ${EVSS_HOME} ${HAVE_SPP}")

install(CODE "message(\"Nothing to do\")")
