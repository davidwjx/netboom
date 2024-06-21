cmake_minimum_required(VERSION 3.8 FATAL_ERROR)

include("${EVSS_RT_HOME}/config/cmake/Modules/evrt.cmake")
set(project_name "evinulayers")

project("${project_name}")
set(project_version "1.0")

# mischa change
set (CMAKE_INSTALL_PREFIX ${CNNSDK_INSTALL_DIR})

find_package(evthreads 1.0 REQUIRED)
find_package(openvx    1.0 REQUIRED)
find_package(evlayers  1.0 REQUIRED)

file(GLOB EVLAYERS_USER_SRC_FILES "src/*.cc")

if(DEFINED ENV{LRN_IMPLEMENTATION})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D$ENV{LRN_IMPLEMENTATION}")
endif()

if(DEFINED EVSS_CFG_HAS_OCL)
file(GLOB EVLAYERS_OCL_USER_SRC_FILES "src/kernels/*.cl")
endif()

add_library(${project_name}  ${EVLAYERS_USER_SRC_FILES}  ${EVLAYERS_OCL_USER_SRC_FILES})
target_link_libraries(${project_name} 
    evthreads::evthreads
    evlayers::evlayers
    openvx::openvx
)

set(EV_LAYERS_SRC_DIR ${EV_CNNSDK_HOME}/evlayers/src)
target_include_directories(${project_name}
    PUBLIC
        $<BUILD_INTERFACE:${EV_LAYERS_SRC_DIR}>
        $<BUILD_INTERFACE:${EV_LAYERS_SRC_DIR}/ev_ocl/kernels>
)
export_library()
