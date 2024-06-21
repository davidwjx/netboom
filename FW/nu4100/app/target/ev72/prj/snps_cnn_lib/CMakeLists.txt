# set minimum required version for cmake
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
include(evrt)
set(project_name "snps_cnn_lib")
project(${project_name})
set(project_version "1.0")

set(CNN_NAME "fake")
# include local cnn_examples.cmake
#
# This file is modified from the file at cnn_tools/examples/common to
# add a library instead of an executable.
#
# It also does many "target_link_libraries", to register the dependencies.
include (cnn_examples.cmake)

# Register the include directories for this library
# Note the "generator" expressions:
# depending on whether this library is being built (BUILD_INTERFACE)
# or used by another library or app (INSTALL_INTERFACE), a different
# path will be used.
#
# The INSTALL_INTERFACE cannot be at build time of this library,
# because the installation step only happens after the library is built.
#target_include_directories(${project_name}
#    PUBLIC
#        $<INSTALL_INTERFACE:include>
#        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#)

# install the header file(s). The destination will be prefixed
# with the CMAKE_INSTALL_PREFIX.
#install (
#    FILES
#        ${CMAKE_CURRENT_SOURCE_DIR}/include/inuitive_cnn.h
#    DESTINATION
#        include
#)

#target_link_libraries( ${project_name} ${EV_CNN_INSTALL_DIR}/lib/libevinulayers.a )
#target_link_libraries( ${project_name} ${EV_CNNSDK_HOME}/install/nu4000b0_release/lib/libevinulayers.a )

#set(CMAKE_INSTALL_PREFIX ${CNNSDK_INSTALL_PREFIX})
# export the CMake definitions of this library. This is a macro defined
# in "evrt.cmake" (included at the top of this file). It does the following:
#  - install targets (i.e. the squeezelib binary)
#  - export target cmake definitions and install these
#  - write package version file
#  - install <package>Config.cmake file
export_library()
