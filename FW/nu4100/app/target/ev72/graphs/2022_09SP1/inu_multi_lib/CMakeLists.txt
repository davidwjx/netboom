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

set(CNN_NAME "inu_multi_lib")
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
# graph #1 args: GoogleNet places
#==============================================================================

set(GRAPH1_NAME googlenet)
set(GRAPH1_DIR ${NN_MODEL_EXAMPLE_DIR}/${GRAPH1_NAME}/caffe_model)
set(GRAPH1_IMAGES_DIR ${NN_MODEL_EXAMPLE_DIR}/images/ImageNet/jpeg)

# -- CMake Variables
set(CNN_NAME1 "${GRAPH1_NAME}")

set(GRAPH_ID1 1)

set(GRAPH_PROTO1   ${GRAPH1_DIR}/deploy_places.prototxt)
set(GRAPH_WEIGHTS1 ${GRAPH1_DIR}/deploy_places.caffemodel)
set(GRAPH_IMAGES1  ${GRAPH1_IMAGES_DIR})

#set(GRAPH1_CNN_LD_ARGS "--use_minmax_cache --ibs 8 --autobin  --report_verbose  --avoid_weighted_concat False ")
set(GRAPH1_CNN_LD_ARGS "--use_minmax_cache --autobin  --report_verbose ")

set(EXTRA_PARAM1 " --ini ${CMAKE_SOURCE_DIR}/${GRAPH1_NAME}.ini "  ${GRAPH1_CNN_LD_ARGS}  )

gen_obj_args(1)

#==============================================================================
# graph #2 args: mobilenetv2
#==============================================================================

set(GRAPH2_NAME mobilenet)
set(GRAPH2_DIR ${NN_MODEL_EXAMPLE_DIR}/${GRAPH2_NAME}/caffe_model)
set(GRAPH2_IMAGES_DIR ${NN_MODEL_EXAMPLE_DIR}/images/ImageNet/jpeg)

# -- CMake Variables
set(CNN_NAME2 "${GRAPH2_NAME}")

set(GRAPH_ID2 2)

set(GRAPH_PROTO2   ${GRAPH2_DIR}/mobilenet_v2_deploy.prototxt)
set(GRAPH_WEIGHTS2 ${GRAPH2_DIR}/mobilenet_v2.caffemodel)
set(GRAPH_IMAGES2  ${GRAPH2_IMAGES_DIR})


set(GRAPH2_CNN_LD_ARGS "--use_minmax_cache --ibs 8 --autobin --report_verbose  --avoid_weighted_concat False  --pixel_mean 103.94,116.78,123.68 --image_scale 4.335 ")

set(EXTRA_PARAM2 " --ini ${CMAKE_SOURCE_DIR}/${GRAPH2_NAME}.ini "  ${GRAPH2_CNN_LD_ARGS}  )

gen_obj_args(2)

#==============================================================================
# graph #3 args: resnet50
#==============================================================================

set(GRAPH3_NAME resnet_50)
set(GRAPH3_DIR ${NN_MODEL_EXAMPLE_DIR}/${GRAPH3_NAME}/caffe_model)
set(GRAPH3_IMAGES_DIR ${NN_MODEL_EXAMPLE_DIR}/images/ImageNet/jpeg)

# -- CMake Variables
set(CNN_NAME3 "${GRAPH3_NAME}")

set(GRAPH_ID3 3)

set(GRAPH_PROTO3   ${GRAPH3_DIR}/ResNet-50-deploy.prototxt)
set(GRAPH_WEIGHTS3 ${GRAPH3_DIR}/ResNet-50-model.caffemodel)
set(GRAPH_MEAN3 ${GRAPH3_DIR}/ResNet_mean.binaryproto)
set(GRAPH_IMAGES3  ${GRAPH3_IMAGES_DIR})

set(GRAPH3_CNN_LD_ARGS "--use_minmax_cache --ibs 8 --autobin --report_verbose \
                        --image_mean ${GRAPH_MEAN3}  ")


set(EXTRA_PARAM3 " --ini ${CMAKE_SOURCE_DIR}/${GRAPH3_NAME}.ini "  ${GRAPH3_CNN_LD_ARGS}  )

gen_obj_args(3)

#==============================================================================
# graph #3 args: mobilenet_ssd
#==============================================================================

set(GRAPH4_NAME mobilenet_ssd)
set(GRAPH4_DIR ${NN_MODEL_EXAMPLE_DIR}/${GRAPH4_NAME}/caffe_model)
set(GRAPH4_IMAGES_DIR ${NN_MODEL_EXAMPLE_DIR}/images/VOC2007/jpeg)

set(CNN_NAME4 "${GRAPH4_NAME}")

set(GRAPH_ID4 4)

set(GRAPH_PROTO4   ${GRAPH4_DIR}/MobileNetSSD_deploy_updated.prototxt)
set(GRAPH_WEIGHTS4 ${GRAPH4_DIR}/MobileNetSSD_deploy.caffemodel)
set(GRAPH_IMAGES4  ${GRAPH4_IMAGES_DIR})

set(GRAPH4_CNN_LD_ARGS "--use_minmax_cache --ibs 8  --pixel_mean 127.5,127.5,127.5 --image_scale 2  --avoid_weighted_concat False --report_verbose")

#for quantization with 12 bit only 
set(EXTRA_PARAM4 " --blob_size 12 detection_out --ini ${CMAKE_SOURCE_DIR}/${GRAPH4_NAME}.ini "  ${GRAPH4_CNN_LD_ARGS} )
#for mixed quantization
#set(EXTRA_PARAM4 " --blob_and_weight_size 8 bandwidth:105 --ini ${CMAKE_SOURCE_DIR}/${GRAPH4_NAME}.ini "  ${GRAPH4_CNN_LD_ARGS} )

gen_obj_args(4)


#==============================================================================
# graph #5 args: yolo_v3
#==============================================================================

set(GRAPH5_NAME yolo_v3)
set(GRAPH5_DIR ${NN_MODEL_EXAMPLE_DIR}/${GRAPH5_NAME}/caffe_model)
set(GRAPH5_IMAGES_DIR ${NN_MODEL_EXAMPLE_DIR}/images/VOC2007/jpeg)

# -- CMake Variables
set(CNN_NAME5 ${GRAPH5_NAME})
set(GRAPH_ID5 5)

set(GRAPH_PROTO5   ${GRAPH5_DIR}/yolov3_deconv_320.prototxt)
set(GRAPH_WEIGHTS5 ${GRAPH5_DIR}/yolov3_deconv.caffemodel)
set(GRAPH_IMAGES5  ${GRAPH5_IMAGES_DIR})

set(EXTRA_PARAM5 " --color_order RGB --image_scale 1  --report_verbose --use_minmax_cache \
   --support_caffe_source True --caffe_layer_type_pixel_only UpsampleDarknet \
   --letterbox 127 --blob_and_weight_size 8 bandwidth:105 --ini ${CMAKE_SOURCE_DIR}/${GRAPH5_NAME}.ini")

#set(EXTRA_PARAM5 " --color_order RGB --image_scale 1  --report_verbose --use_minmax_cache \
#   --support_caffe_source True --caffe_layer_type_pixel_only UpsampleDarknet \
#   --letterbox 127 --ini ${CMAKE_SOURCE_DIR}/${GRAPH5_NAME}.ini")
                  


# Extra python layers
set(EVGENCNN_EXTRA_INCLUDES "${EV_CNNSDK_HOME}/examples/common/custom_layers/yolo_v3")
set(PYTHONPATH_EXTRA "${EV_CNNSDK_HOME}/examples/common/python_layers/yolo_v2")
init_variable(YOLO_THRESH 0.5)
init_variable(YOLO_NMS    0.45)

gen_obj_args(5)

#==============================================================================
# graph #6 args: tiny yolo 36
#==============================================================================

set(GRAPH6_NAME tiny_yolo_36)
set(GRAPH6_DIR ${NN_MODEL_EXAMPLE_DIR}/${GRAPH6_NAME}/caffe_model)
set(GRAPH6_IMAGES_DIR ${NN_MODEL_EXAMPLE_DIR}/images/VOC2007/jpeg)

# -- CMake Variables
set(CNN_NAME6 "${GRAPH6_NAME}")

set(GRAPH_ID6 6)

set(GRAPH_PROTO6   ${GRAPH6_DIR}/deploy_36_416_13x13.prototxt)
set(GRAPH_WEIGHTS6 ${GRAPH6_DIR}/deploy_36_416_13x13.caffemodel)
set(GRAPH_IMAGES6  ${GRAPH6_IMAGES_DIR})

set(GRAPH6_CNN_LD_ARGS "--use_minmax_cache --ibs 8 --autobin --report_verbose  --color_order RGB  --image_scale 1 --avoid_weighted_concat False  ")

set(EXTRA_PARAM6 " --ini ${CMAKE_SOURCE_DIR}/${GRAPH6_NAME}.ini "  ${GRAPH6_CNN_LD_ARGS}  )

gen_obj_args(6)


#==============================================================================
# graph #7 args: Onet
#==============================================================================

set(GRAPH7_NAME onet)
set(GRAPH7_DIR ${NN_MODEL_EXAMPLE_DIR}/${GRAPH7_NAME}/caffe_model)
set(GRAPH7_IMAGES_DIR ${NN_MODEL_EXAMPLE_DIR}/images/ImageNet/jpeg)

# -- CMake Variables
set(CNN_NAME7 "${GRAPH7_NAME}")

set(GRAPH_ID7 7)

set(GRAPH_PROTO7   ${GRAPH7_DIR}/deploy.prototxt)
set(GRAPH_WEIGHTS7 ${GRAPH7_DIR}/deploy.caffemodel)
set(GRAPH_IMAGES7  ${GRAPH7_IMAGES_DIR})

set(GRAPH7_CNN_LD_ARGS "--use_minmax_cache --ibs 8 --autobin --report_verbose --toronto_tanh '1<<1|!1<<2|!1<<3|1<<25' --support_caffe_source True --avoid_weighted_concat False  ")

set(EXTRA_PARAM7 " --ini ${CMAKE_SOURCE_DIR}/${GRAPH7_NAME}.ini "  ${GRAPH7_CNN_LD_ARGS}  )

gen_obj_args(7)

#==============================================================================
# graph #8 args: inception resnet v1
#==============================================================================

set(GRAPH8_NAME inception_resnet_v1)
set(GRAPH8_DIR ${NN_MODEL_EXAMPLE_DIR}/${GRAPH8_NAME}/caffe_model)
set(GRAPH8_IMAGES_DIR ${NN_MODEL_EXAMPLE_DIR}/images/ImageNet/jpeg)

# -- CMake Variables
set(CNN_NAME8 "${GRAPH8_NAME}")

set(GRAPH_ID8 8)

set(GRAPH_PROTO8   ${GRAPH8_DIR}/inception_resnet_v1_20170512_new_convert.prototxt)
set(GRAPH_WEIGHTS8 ${GRAPH8_DIR}/inception_resnet_v1_20170512_convert.caffemodel)
set(GRAPH_IMAGES8  ${GRAPH8_IMAGES_DIR})

set(GRAPH8_CNN_LD_ARGS "--use_minmax_cache --ibs 8 --autobin --report_verbose --color_order RGB --pixel_mean 127.5,127.5,127.5 --image_scale 2 --ignore_unsupported_layers --avoid_weighted_concat False  ")

set(EXTRA_PARAM8 " --ini ${CMAKE_SOURCE_DIR}/${GRAPH8_NAME}.ini "  ${GRAPH8_CNN_LD_ARGS}  )

gen_obj_args(8)

#==============================================================================
# graph #9 args: tiny yolo v7 (coco, 80 classes)
#==============================================================================

set(GRAPH9_NAME yolov7)
set(GRAPH9_DIR ${NN_MODEL_EXAMPLE_DIR}/${GRAPH9_NAME}/caffe_model)
set(GRAPH9_IMAGES_DIR ${NN_MODEL_EXAMPLE_DIR}/images/COCO_VAL_2017)

# -- CMake Variables
set(CNN_NAME9 "${GRAPH9_NAME}_tiny")

set(GRAPH_ID9 9)

set(GRAPH_PROTO9   ${GRAPH9_DIR}/yolov7-tiny_convert_optimized_pooling.prototxt)
set(GRAPH_WEIGHTS9 ${GRAPH9_DIR}/yolov7-tiny_convert_optimized.caffemodel)
set(GRAPH_IMAGES9  ${GRAPH9_IMAGES_DIR})

set(GRAPH9_CNN_LD_ARGS " --report_verbose --use_minmax_cache --num_slices 2 --image_scale 1 --color_order RGB --support_caffe_source True")
set(EXTRA_PARAM9 " --ini ${CMAKE_SOURCE_DIR}/simple.ini "  ${GRAPH9_CNN_LD_ARGS}  )

gen_obj_args(9)

#==============================================================================
# graph #10 args: ICNet (cityscape dataset)
#==============================================================================

set(GRAPH10_NAME icnet)
set(GRAPH10_DIR ${NN_MODEL_EXAMPLE_DIR}/${GRAPH10_NAME}/caffe_model)
set(GRAPH10_IMAGES_DIR ${NN_MODEL_EXAMPLE_DIR}/images/ImageNet/jpeg)

set(CNN_NAME10 "${GRAPH10_NAME}")

set(GRAPH_ID10 10)

set(GRAPH_PROTO10   ${GRAPH10_DIR}/icnet_cityscapes.prototxt)
set(GRAPH_WEIGHTS10 ${GRAPH10_DIR}/icnet_cityscapes_train_30k.caffemodel)
set(GRAPH_IMAGES10  ${GRAPH10_IMAGES_DIR})

set(GRAPH10_CNN_LD_ARGS "--use_minmax_cache --ibs 8  --pixel_mean 103.939,116.779,123.68  --avoid_weighted_concat False --report_verbose")

set(EXTRA_PARAM_SUBGRAPH "  --subgraph G1 conv5_3_pool1 conv5_3_pool1_interp \
                             conv5_3_pool2 conv5_3_pool2_interp \
                             conv5_3_pool3 conv5_3_pool3_interp \
                             conv5_3_pool6 conv5_3_pool6_interp conv5_3_sum \
							 --blob_size 12 data data_sub2 conv3_1_sub4 conv5_4_interp sub24_sum_interp \
                             sub12_sum_interp conv3_2_1x1_increase conv5_3_pool1 \
                             conv5_3_pool1_interp conv5_3_pool2 conv5_3_pool2_interp \
                             conv5_3_pool3 conv5_3_pool3_interp \
                             conv5_3_pool6 conv5_3_pool6_interp conv5_3_sum \
						     conv3_1_sub2_proj conv3_sub1_proj	 ")
 
set(EXTRA_PARAM10 " --ini ${CMAKE_SOURCE_DIR}/${GRAPH10_NAME}.ini "  ${GRAPH10_CNN_LD_ARGS} ${EXTRA_PARAM_SUBGRAPH} )

gen_obj_args(10)

#==============================================================================
# graph #11 args: SSD mobilenet COCO31
#==============================================================================

set(GRAPH11_NAME mobilenet_ssd_coco31)
set(GRAPH11_DIR ${NN_MODEL_EXAMPLE_DIR}/${GRAPH11_NAME}/caffe_model)
set(GRAPH11_IMAGES_DIR ${NN_MODEL_EXAMPLE_DIR}/images/VOC2007/jpeg)

set(CNN_NAME11 "${GRAPH11_NAME}")

set(GRAPH_ID11 11)

set(GRAPH_PROTO11   ${GRAPH11_DIR}/MobileNetSSD_deploy_coco31_updated.prototxt)
set(GRAPH_WEIGHTS11 ${GRAPH11_DIR}/MobileNetSSD_deploy_coco31.caffemodel)
set(GRAPH_IMAGES11  ${GRAPH11_IMAGES_DIR})

set(GRAPH11_CNN_LD_ARGS " --use_minmax_cache --ibs 8  --pixel_mean 127.5,127.5,127.5 --image_scale 2  --avoid_weighted_concat False --report_verbose")
 
#For quantization with 12 bit only
set(EXTRA_PARAM11 " --blob_size 12 detection_out --ini ${CMAKE_SOURCE_DIR}/${GRAPH11_NAME}.ini "  ${GRAPH11_CNN_LD_ARGS} )
#For mixed quantization
#set(EXTRA_PARAM11 " --blob_and_weight_size 8 bandwidth:105 --ini ${CMAKE_SOURCE_DIR}/${GRAPH11_NAME}.ini "  ${GRAPH11_CNN_LD_ARGS} )

gen_obj_args(11)


#==============================================================================
# graph #12 args: yolact
#==============================================================================

set(GRAPH12_NAME yolact)
set(GRAPH12_DIR ${NN_MODEL_EXAMPLE_DIR}/${GRAPH12_NAME}/caffe_model)
set(GRAPH12_IMAGES_DIR ${NN_MODEL_EXAMPLE_DIR}/images/VOC2007/jpeg)

# -- CMake Variables
set(CNN_NAME12 "${GRAPH12_NAME}")

set(GRAPH_ID12 12)

set(GRAPH_PROTO12   ${GRAPH12_DIR}/yolact_coco_368_convert.prototxt)
set(GRAPH_WEIGHTS12 ${GRAPH12_DIR}/yolact_coco_368_convert_mod.caffemodel)
set(GRAPH_IMAGES12  ${GRAPH12_IMAGES_DIR})

set(GRAPH12_CNN_LD_ARGS "--use_minmax_cache  --ignore_unsupported_layers --support_caffe_source True --autobin --report_verbose  --pixel_mean 103.94,116.78,123.68 --image_scale 4.335 --color_order RGB  ")


set(EXTRA_PARAM12 " --ini ${CMAKE_SOURCE_DIR}/${GRAPH12_NAME}.ini "  ${GRAPH12_CNN_LD_ARGS}  )

gen_obj_args(12)


#==============================================================================
# graph #13 args: InceptionV4
#==============================================================================

set(GRAPH13_NAME inception_v4)
set(GRAPH13_DIR ${NN_MODEL_EXAMPLE_DIR}/${GRAPH13_NAME}/caffe_model)
set(GRAPH13_IMAGES_DIR ${NN_MODEL_EXAMPLE_DIR}/images/ImageNet/jpeg)

# -- CMake Variables
set(CNN_NAME13 "${GRAPH13_NAME}")

set(GRAPH_ID13 13)

set(GRAPH_PROTO13   ${GRAPH13_DIR}/inception_v4_convert.prototxt)
set(GRAPH_WEIGHTS13 ${GRAPH13_DIR}/Inception_V4_accuracy_drop_0_005_2-4x_94-10.caffemodel)
set(GRAPH_IMAGES13  ${GRAPH13_IMAGES_DIR})

set(GRAPH13_CNN_LD_ARGS "--use_minmax_cache  --ignore_unsupported_layers --support_caffe_source True --autobin --report_verbose --keep_only_layers InceptionV4__Logits__Predictions --color_order RGB --pixel_mean 127.5,127.5,127.5 --image_scale 2  ")

set(EXTRA_PARAM13 " --ini ${CMAKE_SOURCE_DIR}/${GRAPH13_NAME}.ini "  ${GRAPH13_CNN_LD_ARGS}  )

gen_obj_args(13)

#==============================================================================
# graph #14 args: openpose_thin
#==============================================================================

set(GRAPH14_NAME openpose_thin)
set(GRAPH14_DIR ${NN_MODEL_EXAMPLE_DIR}/${GRAPH14_NAME}/caffe_model)
set(GRAPH14_IMAGES_DIR ${NN_MODEL_EXAMPLE_DIR}/images/ImageNet/jpeg)

# -- CMake Variables
set(CNN_NAME14 "${GRAPH14_NAME}")

set(GRAPH_ID14 14)

set(GRAPH_PROTO14   ${GRAPH14_DIR}/convert_optimized.prototxt)
set(GRAPH_WEIGHTS14 ${GRAPH14_DIR}/convert_optimized.caffemodel)
set(GRAPH_IMAGES14  ${GRAPH14_IMAGES_DIR})

set(GRAPH14_CNN_LD_ARGS "--use_minmax_cache  --ignore_unsupported_layers --support_caffe_source True --autobin --report_verbose --discard_after_softmax --color_order RGB  ")

set(EXTRA_PARAM14 " --ini ${CMAKE_SOURCE_DIR}/${GRAPH14_NAME}.ini "  ${GRAPH14_CNN_LD_ARGS}  )

gen_obj_args(14)


#==============================================================================
# Tie them all
#==============================================================================

# Set that we work with 3 graphs
set(MULTI_GRAPH_MODE 1)

#set(NUM_GRAPHS 15)
set(NUM_GRAPHS 14)

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

#running python script to add piggyback with anchors to yolo_v3 networks binaries


find_program(PYTHON "python")
GET_FILENAME_COMPONENT (result "${dir}" REALPATH)
set(ANCHOR_UTIL_PATH "${result}/../../") 


foreach (GRAPH_NR 5)
set (CNN_BIN_GRAPH${GRAPH_NR} ${result}/build_nu4100_release_unmerged_large/generated_obj/${CNN_NAME}/${GRAPH${GRAPH_NR}_NAME}/nn_bin_${GRAPH${GRAPH_NR}_NAME}.bin)
add_custom_target(
		yolov3_anchor_${GRAPH_NR}_target ALL
		COMMAND ${CMAKE_COMMAND} -E echo "Running python script to add piggyback with anchors to yolo_v3 networks binaries"
		COMMAND ${CMAKE_COMMAND} -E env PYTHONPATH=${ANCHOR_UTIL_PATH}
                   ${PYTHON} ${ANCHOR_UTIL_PATH}/append_yolov3_anchors.py ${GRAPH_PROTO${GRAPH_NR}} ${CNN_BIN_GRAPH${GRAPH_NR}}
		DEPENDS ${project_name}
    )
	#${DISTRIBUTE_SRC_STR${GRAPH_NR}}
	#${CNN_BIN_FILE${GRAPH_NR}}
endforeach()
	

		   
