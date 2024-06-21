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
project("main_project")
# set(CMAKE_INSTALL_PREFIX ${CNNSDK_INSTALL_DIR})
#list (APPEND as_subproject inu_app snps_cnn_lib inu_infra customer_app)
list (APPEND as_subproject inu_app snps_cnn_lib inu_infra )
#list (APPEND as_subproject customer_app)
add_subdirectory(snps_cnn_lib)
add_subdirectory(inu_infra)
add_subdirectory(inu_app)
#add_subdirectory(customer_app)