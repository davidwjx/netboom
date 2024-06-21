# ============================================================================= 
# Copyright 2020  Synopsys, Inc.
# This file and the associated documentation are proprietary to Synopsys, 
# Inc., and may only be used in accordance with the terms and conditions of 
# a written license agreement with Synopsys, Inc.
# Notwithstanding contrary terms in the DFPUC, Licensee may provide the
# binaries of the EV Runtime and Utilities Option to its end-customer that
# purchase Licensee ICs that incorporate the Synopsys EV processor core,
# subject to confidentiality terms no less restrictive than those contained in
# the DFPUC.  All other use, reproduction, or distribution of this file
# is strictly prohibited.
# =============================================================================

cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
include(evrt)

set(CNN_NAME "mobilenet_ssd_lib")
set(project_name ${CNN_NAME})
project("${project_name}")

find_package(evinulayers 1.0 REQUIRED)
if(DEFINED ENV{BUILD_VARIANT})
    set(BUILD_VARIANT "$ENV{BUILD_VARIANT}")
endif()

if(DEFINED BUILD_VARIANT)
    set(project_name "${CNN_NAME}_${BUILD_VARIANT}")
project("${project_name}")
    set(BUILD__PREFIX "_${BUILD_VARIANT}")
else()
    set(project_name ${CNN_NAME})
project("${project_name}")
    set(BUILD__PREFIX "")
endif()

set(project_version "1.0")

# include local cnn_examples.cmake
#
# This file is modified from the file at cnn_tools/examples/common to
# add a library instead of an executable.
#
# It also does many "target_link_libraries", to register the dependencies.
# include (cnn_examples.cmake)
set(CNN_EXAMPLE_BUILD_LIBRARY "ON")

if(DEFINED CNNAPPCOMMONLIB)
   set(COMMON_PREFIX "_common")
else()
   set(COMMON_PREFIX "")
endif()

configure_file(
    cmake/${CNN_NAME}Config${COMMON_PREFIX}.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake/${CNN_NAME}${BUILD__PREFIX}Config.cmake
    @ONLY
)

include(${EV_CNNSDK_HOME}/examples/common/cnn_examples_header.cmake)



#==============================================================================
# graph #1 args: mobilenet_ssd
#==============================================================================

set(GRAPH1_NAME mobilenet_ssd)
set(GRAPH1_DIR ${NN_MODEL_EXAMPLE_DIR}/${GRAPH1_NAME}/caffe_model)
set(GRAPH1_IMAGES_DIR ${NN_MODEL_EXAMPLE_DIR}/images/VOC2007/jpeg)

set(CNN_NAME1 "${GRAPH1_NAME}")

set(GRAPH_ID1 1)

set(GRAPH_PROTO1   ${GRAPH1_DIR}/MobileNetSSD_deploy_updated.prototxt)
set(GRAPH_WEIGHTS1 ${GRAPH1_DIR}/MobileNetSSD_deploy.caffemodel)
set(GRAPH_IMAGES1  ${GRAPH1_IMAGES_DIR})

set(GRAPH1_CNN_LD_ARGS "--use_minmax_cache --ibs 8  --pixel_mean 127.5,127.5,127.5 --image_scale 2  --avoid_weighted_concat False --report_verbose")

#for quantization with 12 bit only 
set(EXTRA_PARAM1 " --blob_size 12 detection_out --ini ${CMAKE_SOURCE_DIR}/${GRAPH1_NAME}.ini "  ${GRAPH1_CNN_LD_ARGS} )
#for mixed quantization
#set(EXTRA_PARAM4 " --blob_and_weight_size 8 bandwidth:105 --ini ${CMAKE_SOURCE_DIR}/${GRAPH4_NAME}.ini "  ${GRAPH4_CNN_LD_ARGS} )

gen_obj_args(1)



#==============================================================================
# Tie them all
#==============================================================================

# Set that we work with 3 graphs
set(MULTI_GRAPH_MODE 1)

#set(NUM_GRAPHS 15)
set(NUM_GRAPHS 1)

# The appliction manages of using different graphs on multi-slice HW
# but each graph should think that it is 1-slice HW
unset(NUM_SLICES)
set(NUM_SLICES 2)

# special set of program arguments
# see run_arguments(.mk.in/.cmake) in this folder
set(SPECIAL_RUN_ARGUMENTS ON)

# Build not application, but library
set(SEPARATE_LIB ON)


include(${EV_CNNSDK_HOME}/examples/common/cnn_examples.cmake)

# Register the include directories for this library
# Note the "generator" expressions:
# depending on whether this library is being built (BUILD_INTERFACE)
# or used by another library or app (INSTALL_INTERFACE), a different
# path will be used.
#
# The INSTALL_INTERFACE cannot be at build time of this library,
# because the installation step only happens after the library is built.
target_include_directories(${project_name}
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# install the header file(s). The destination will be prefixed
# with the CMAKE_INSTALL_PREFIX.
install (
    FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/inuitive_cnn.h
    DESTINATION
        include
)


target_link_libraries(${project_name} evinulayers::evinulayers)
# export the CMake definitions of this library. This is a macro defined
# in ev_runtime "evrt.cmake" (included at the top of this file). It does the following:
#  - install targets (i.e. the inu_multi_lib binary)
#  - export target cmake definitions and install these
#  - write package version file
#  - install <package>Config.cmake file
export_library("ON")	
	

		   
