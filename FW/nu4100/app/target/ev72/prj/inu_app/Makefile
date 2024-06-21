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
include $(EV_CNNSDK_HOME)/config/env.mk

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
#GRAPH_IMAGES?=$(IMAGENET_PATH)/$(IMAGE_SET)/$(IMAGE_TYPE)
#ARGS ?= -image_test   5   5                                     \
#        -classify 17 $(GRAPH_IMAGES)/n01580077_10435.$(IM_EXT)  \
#        -classify 5 $(GRAPH_IMAGES)/n01496331_12107.$(IM_EXT)   \
#        -classify 40 $(GRAPH_IMAGES)/n01682714_11445.$(IM_EXT)  \
#        -classify 639 $(GRAPH_IMAGES)/n03710637_13354.$(IM_EXT) \
#        -classify 478 $(GRAPH_IMAGES)/n02971356_10501.$(IM_EXT)

PREREQUISITES_COMMAND=+$(MAKE) -C "$(EVSS_RT_HOME)/examples" prerequisites
#include $(EVSS_RT_HOME)/config/config.mk
#include $(EV_CNNSDK_HOME)/config/cnn_config.mk
include $(EV_CNNSDK_HOME)/examples/common/cnn_examples.mk

# Graph name

ifneq ("$(G)","")
    CMAKE_ARGS+=-DGRAPH_NAME=$(GRAPH_NAME) -DG=$(G)
endif

LIB_HOME=$(abspath ../../graphs/$(TOOLS_VERSION)/$(GRAPH_NAME))

CNN_OBJ=$(EVSS_BUILD_DIR)/cnn_obj
CNN_SRC=$(EVSS_BUILD_DIR)/cnn_src

override CNN_SRC_TOP_DIR:=$(LIB_HOME)/$(CNN_SRC)
override CNN_OBJ_PATH:=$(LIB_HOME)/$(CNN_OBJ)/$(G)

CNN_BIN_FILE=$(LIB_HOME)/$(CNN_OBJ)/$(G)/cnn_bin_$(G).bin

ifneq ("$(CNN_ABSTRACTION)","unmerged_large")
    ifneq ("$(EVSS_CFG)","ev_native")
        override NSIM_OPTION += -prop=nsim_ext=$(LIB_HOME)/$(CNN_OBJ)/$(G)/libcnn_nsim_model.so
    endif
endif
clean::
	$(call RM, output.pgm)	
