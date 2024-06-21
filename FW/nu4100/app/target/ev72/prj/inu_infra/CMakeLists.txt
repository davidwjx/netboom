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
set(project_name "inu_infra")
set(project_version "1.0")
project("${project_name}")

# set(CMAKE_INSTALL_PREFIX ${CNNSDK_INSTALL_DIR})

# find_package(snps_cnn_lib    1.0 REQUIRED)
find_package(evcnndev        1.0 REQUIRED)
find_package(evthreads       1.0 REQUIRED)
find_package(evmss           2.0 REQUIRED)
find_package(evOpenCV        2.4.9 QUIET)

# -- executable targets
#
add_library(${project_name}
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
	${CMAKE_CURRENT_SOURCE_DIR}/../../src/alg/vision_proc/src/VisionProc_ev.cc
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
	${CMAKE_CURRENT_SOURCE_DIR}/../../../../common/application/manager/src/inu_data.c

)

# link to libraries used by graph
target_link_libraries(${project_name} evcnndev::evcnndev)
target_link_libraries(${project_name} evthreads::evthreads)
target_link_libraries(${project_name} evmss::evmss)
target_link_libraries(${project_name} evOpenCV::evOpenCV)

# target_link_libraries(${project_name} snps_cnn_lib::snps_cnn_lib)

target_include_directories(${project_name}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../api/client>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../api/common>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../api/common/graph>
		$<INSTALL_INTERFACE:include>
		$<INSTALL_INTERFACE:include/common>
		$<INSTALL_INTERFACE:include/common/graph>
	PRIVATE
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src/ev_infrastructure/init/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src/ev_infrastructure/bsp/ictl/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src/ev_infrastructure/bsp/icc/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../src/ev_infrastructure/os/scheduler/inc>
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../../../api/client>
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

target_compile_options(${project_name} PUBLIC "-DEV72")
target_compile_options(${project_name} PUBLIC "-Wno-int-conversion")
target_compile_options(${project_name} PUBLIC "-Wno-extern-c-compat")
# -- installation targets
#
export_library()

install(FILES
		${CMAKE_CURRENT_SOURCE_DIR}/../../../../api/client/client_api.h
#		${CMAKE_CURRENT_SOURCE_DIR}/../../../../api/common/inu_types.h
#		${CMAKE_CURRENT_SOURCE_DIR}/../../../../api/common/err_defs.h
#		${CMAKE_CURRENT_SOURCE_DIR}/../../../../api/common/inu_defs.h
		DESTINATION include)

		install(DIRECTORY
		${CMAKE_CURRENT_SOURCE_DIR}/../../../../api/common
		DESTINATION include)
