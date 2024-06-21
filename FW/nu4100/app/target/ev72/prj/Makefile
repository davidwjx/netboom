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


# These variables must be defined before executing the makefile
ifndef EVSS_HOME
    $(error EVSS_HOME is not defined, please set point it to <your EV installation> directory)
endif
ifndef EVSS_RT_HOME
    $(error EVSS_RT_HOME is not defined, please set point it to <your EV installation>/software directory)
endif
# Post process and check the path variables
override EVSS_RT_HOME := $(abspath $(EVSS_RT_HOME))
override EVSS_HOME := $(abspath $(EVSS_HOME))
ifeq ($(strip $(wildcard $(EVSS_HOME))),)
    $(error EVSS_HOME variable does not point to a directory)
endif
ifeq ($(strip $(wildcard $(EVSS_RT_HOME))),)
    $(error EVSS_RT_HOME variable does not point to a directory)
endif

# include CNN SDK env.mk file, so we know the installation prefix of
# the SqueezeLib library, which is built from cnn_tools.

#include $(EV_CNNSDK_HOME)/config/env.mk
include $(EV_CNNSDK_HOME)/examples/common/cnn_examples.mk

EVSS_BUILD_DIR=build_$(EVSS_CFG_NAME)
# convey CNN installation prefix to CMake
#CMAKE_ARGS += -DEV_CNN_INSTALL_DIR=$(EV_CNN_INSTALL_DIR)

# import regular EVRT build system definitions
# include $(EVSS_RT_HOME)/config/config.mk
EVSS_EXE = ev72.out
LINUX_EXE = ev72
WIN_EXE = ev72.exe
# Default path to ImageNet set for default run arguments
#IMAGENET_PATH?=$(EV_CNNMODELS_HOME)/caffe_models/images
CL_PRJ = ev72
# Default input argument for running
# Change image extensions to use netpbm images in case of EVSS_PROFILE=small
#IMAGE_SET=ImageNet
#IMAGE_TYPE?=jpeg
#IM_EXT?=jpeg
#ifeq ($(EV_CNN_NO_OPENCV),1)
#    IMAGE_TYPE=raw_images/227x227
#    IM_EXT=ppm
#endif
ARGS ?= synopsys-logo.pgm

#Inuitive update
#include $(EVSS_RT_HOME)/config/config.mk
#override PYTHON := python
#CMAKE_ARGS += -DCMAKE_MODULE_PATH=$(EV_CNNSDK_HOME)/config/cmake/Modules
#CMAKE_ARGS += -DEVCNN_VERSION=${EVCNN_VERSION}


clean::
	$(call RM, output.pgm)	
