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
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)
include(evrt)
set(project_name "ev72")
project("${project_name}")

if("${G}" STREQUAL "")
    set(G "$ENV{G}") # If G doesn't come from Makefile, try to get G from environment
else()
    message(STATUS "[DEGUG1] G is defined and G=${G}")
endif()

# If it still is no defined
if("${G}" STREQUAL "")
    message(FATAL_ERROR 
"\nG - graph name - is not defined.\n You can set it like so: export G=snps_cnn_lib")
else()
    message(STATUS "[DEGUG2] G is defined and G=${G}")
endif()
message("Graph used: " ${G})

#set(CMAKE_INSTALL_PREFIX ${CNNSDK_INSTALL_DIR})

# squeezelib is built from inside the cnn_tools repository, so it will have
# a different installation prefix. The following line adds this prefix to the
# search path used by find_package()

# find and import graph cmake definitions
set(G "$ENV{G}")
find_package(${G} 1.0 REQUIRED)
find_package(evcnnovx        1.0 REQUIRED)

if(DEFINED USE_OPENCV)
	find_package(evOpenCV 2.4.9 REQUIRED)
endif()

# -- executable targets
#
add_executable(${project_name}

	${CMAKE_CURRENT_SOURCE_DIR}/../../src/main/main.c

	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/Cdnn_ev.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/inu_app_pipe.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/inu_cnn_pipe_api.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/yolo_v3_pp.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/inu_logistic.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/ev_inu_logistic.cl
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/yolact_pp.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/ev_inu_proto_mask_mul.cl
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/inu_proto_mask_mul.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/inu_softmax.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/ev_inu_softmax.cl
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/inu_reshape_chw2hwc.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/ev_inu_reshape_chw_hwc_scale.cl
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/inu_resize_f.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/ev_inu_resize_f.cl
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/inu_combine_c.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/ev_inu_combine_c.cl
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/inu_convert2float.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/ev_inu_convert2float.cl
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/inu_matrix_max_rows.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/ev_inu_matrix_max_rows.cl
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/inu_utils.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/face_ssd_pp.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/ConvertYuvToRgb888.cl
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/copy_main.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/ev_inu_copy.cl
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/pp/pp_ev.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/pp/ev_inu_voxel_grid_filtering.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/pp/ocl_kernels/ev_inu_ocl_vgf.cl	
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/pp/ocl_kernels/ev_inu_ocl_pcl.cl	
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/ev_infrastructure/init/src/ev_init.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/ev_infrastructure/init/src/latencyTest.cc
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/ev_infrastructure/bsp/ictl/src/ictl_drv.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/ev_infrastructure/bsp/icc/src/icc.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/ev_infrastructure/os/scheduler/src/sched.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/infrastructure/os/src/os_lyr_ev72.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/infrastructure/os/src/linked_list.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/infrastructure/utils/src/log.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/infrastructure/utils/src/que.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../common/data_base/src/data_base.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../common/bsp/pss/src/pss_drv.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../../common/client_api/client_api.c
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/alg/vision_proc/src/VisionProc_ev.cc

)

# link to libraries used by graph
target_link_libraries(${project_name}
    -Ball_archive ${G}::${G} -Bno_all_archive
)

if(DEFINED CNN_USE_OPENCV)
    find_package(cnnimageopencv)
    target_link_libraries(${project_name} cnnimageopencv::cnnimageopencv)
else()
    find_package(cnnimagebmetal)
    target_link_libraries(${project_name} cnnimagebmetal::cnnimagebmetal)
endif()

# Note: -Ball_archive ... -Bno_all_archive disables some linker optimization
# for the libraries in between. This is necessary, because the dispatched
# layers are statically instantiated classes, that register themselves from
# the constructor. Since these classes are never called anywhere, the linker
# would otherwise not include these classes into the executable, which will
# yield an error message while loading the CNN binary (because dispatched
# layers in the CNN binary cannot be matched with code)
target_include_directories(${project_name}
	PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../api/client>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../api/common>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src/ev_infrastructure/init/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src/ev_infrastructure/bsp/ictl/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src/ev_infrastructure/bsp/icc/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src/ev_infrastructure/os/scheduler/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../api/client>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../api/common/graph>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/infrastructure/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/infrastructure/os/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/infrastructure/utils/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/infrastructure/xmldb>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/infrastructure/metadata/nanopb>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/application/manager/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/application/nucfg>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../common/mem/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../common/bsp/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../common/bsp/inc/regs>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../common/data_base/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../common/icc/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../common/ceva_boot_params>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../common/bsp/pss/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../../common/version>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../../common>
		$<BUILD_INTERFACE:${EV_CNNSDK_HOME}/install/include>
		$<BUILD_INTERFACE:${EV_CNNSDK_HOME}/install/include/ext>
		$<BUILD_INTERFACE:${EV_CNNSDK_HOME}/install/include/app>
		$<BUILD_INTERFACE:${EV_CNNSDK_HOME}/install/nu4000b0_release/include/internal>
		$<BUILD_INTERFACE:${EV_CNNSDK_HOME}/install/include/evutil>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src/cnn/ev_inu_defined_layers/src/>

)

#target_compile_options(${project_name} PUBLIC "-Hon=volatile_cache_bypass")
target_compile_options(${project_name} PUBLIC "-DEV72")
#target_compile_options(${project_name} PUBLIC "-Wno-error=incompatible-pointer-types")
target_compile_options(${project_name} PUBLIC "-Wno-int-conversion")
target_compile_options(${project_name} PUBLIC "-Wno-extern-c-compat")

# Generate run script for Makefile (old solution).
# We default solution for most of graphs
# If special arguments are required set SPECIAL_RUN_ARGUMENTS
# and create run_arguments.mk.in in your example folder
# See demo_dual
if(DEFINED SPECIAL_RUN_ARGUMENTS)
    set(RUN_ARG_IN ${CMAKE_SOURCE_DIR}/run_arguments.mk.in)
else()
    set(RUN_ARG_IN ${EV_CNNSDK_HOME}/examples/common/run_arguments.mk.in)
endif()

configure_file(
    ${RUN_ARG_IN}
    ${CMAKE_CURRENT_BINARY_DIR}/run_arguments.mk
    @ONLY)
# -- installation targets
#
install(CODE "message(\"Nothing to do\")")