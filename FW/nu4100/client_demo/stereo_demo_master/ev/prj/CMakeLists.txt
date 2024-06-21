# ===============================================================================
# Copyright 2015 - 2017  Synopsys, Inc.
# This file and the associated documentation are proprietary to Synopsys,
# Inc., and may only be used in accordance with the terms and conditions of
# a written license agreement with Synopsys, Inc.
# Notwithstanding contrary terms in the DFPUC, Licensee may provide the
# binaries of the EV Runtime and Utilities Option to its end-customer that
# purchase Licensee ICs that incorporate the Synopsys EV processor core,
# subject to confidentiality terms no less restrictive than those contained in
# the DFPUC.  All other use, reproduction, or distribution of this file
# is strictly prohibited.
# ===============================================================================

# set minimum required version for cmake
cmake_minimum_required(VERSION 3.8)

include(evrt)
set(project_name "customer_app")
set(project_version "1.0")
project("${project_name}")
set(ingraph_name "$ENV{GRAPH_NAME}")
message("Graph used: " ${ingraph_name})
#set(CMAKE_INSTALL_PREFIX ${CNNSDK_INSTALL_DIR})

find_package(inu_infra 1.0 REQUIRED)
find_package(${ingraph_name} 1.0 REQUIRED)
find_package(evthreads 1.0 REQUIRED)

# -- executable targets
#
add_executable(${project_name} 
	${CMAKE_CURRENT_SOURCE_DIR}/../src/stereo_demo_ev.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../src/ocl/ocl_gaussian3x3.cl
)

target_include_directories(${project_name} 
	PUBLIC 
		${CMAKE_CURRENT_SOURCE_DIR}/../../common/
		${CMAKE_CURRENT_SOURCE_DIR}/../src/ocl/
)

if(DEFINED CNN_USE_OPENCV)
	find_package(cnnimageopencv)
	target_link_libraries(${project_name} cnnimageopencv::cnnimageopencv)
else()
    find_package(cnnimagebmetal)
    target_link_libraries(${project_name} cnnimagebmetal::cnnimagebmetal)
endif()

# link to libraries used by graph
target_link_libraries( ${project_name} openvx::openvx )
target_link_libraries(${project_name}
   inu_infra::inu_infra
    -Ball_archive ${ingraph_name}::${ingraph_name} -Bno_all_archive
   evthreads::evthreads
)
target_compile_options(${project_name} PUBLIC "-DEV61")
target_compile_options(${project_name} PUBLIC "-Wno-int-conversion")
target_compile_options(${project_name} PUBLIC "-Wno-macro-redefined")

install(CODE "message(\"Nothing to do\")")