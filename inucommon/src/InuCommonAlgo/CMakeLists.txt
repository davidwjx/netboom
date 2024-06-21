################################################################################
# CommonAlgo cmake project 
# 
# Copyright (C) 2014 All rights reserved to Inuitive
################################################################################

cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 8.1 CACHE STRING "" FORCE)

# Need to read the version from Version.h or overwrite it in the build script
project(InuCommonAlgo LANGUAGES CXX VERSION 1.0)

################################################################################
# Source groups
################################################################################
if(CMAKE_VS_PLATFORM_NAME)
set(GeneralVS
    "resource.h"
    "dllmain.cpp"
    "CommonAlgo.rc"
)
else()
set(GeneralVS )
endif()

set(General
    "../../include/FpException.h"
    "../../include/SSE2NEON.h"
    "../../include/CommonAlgo.h"
)

source_group("General" FILES ${General} ${GeneralVS})

set(AlgoBase
    "../../include/AlgoAvx.h"
    "../../include/AlgoBase.h"
    "../../include/AlgoD2D.h"
    "../../include/AlgoDepthOpticalData.h"
    "../../include/RgbShufflers.h"
    "../../include/YUV2RGB.h"
    "../../include/YUV422ToRGB.h"
    "AlgoBase/AlgoAvx.cpp"
    "AlgoBase/AlgoBase.cpp"
    "AlgoBase/AlgoD2D.cpp"
    "AlgoBase/AlgoDepth2XYZ.cpp"
    "AlgoBase/YUV2RGB.cpp"
    "AlgoBase/YUV422ToRGB.cpp"
)
source_group("AlgoBase" FILES ${AlgoBase})

set(AlgStaticTemporalFilter
    "../../include/StaticTemporalFilterBaseDLL.h"
    "AlgStaticTemporalFilter/config.h"
    "AlgStaticTemporalFilter/StaticTemporalFilter.h"
    "AlgStaticTemporalFilter/StaticTemporalFilter.cpp"    
    "AlgStaticTemporalFilter/StaticTemporalFilterBase.cpp"
)
source_group("AlgStaticTemporalFilter" FILES ${AlgStaticTemporalFilter})

set(ALL_FILES
    ${General}
    ${GeneralVS}
    ${AlgoBase}
    ${AlgStaticTemporalFilter}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

add_dependencies(${PROJECT_NAME} InuCommonUtilities InuSensorsDB)

#use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE InuCommonAlgo)

################################################################################
# Output directory
################################################################################
if(CMAKE_VS_PLATFORM_NAME)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        # On Windows need to set the Runtime folder in order to control the build path
        RUNTIME_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        ARCHIVE_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )  
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_PLATFORM_NAME}"
    )
endif()


################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(../../cmake/global_settings.cmake)

################################################################################
# Boost
################################################################################
set(Boost_Required_Libs system filesystem thread)
include(../../cmake/boost.cmake)

################################################################################
# OpenCV 
################################################################################
set(OpenCV_Required_Libs opencv_core opencv_imgproc opencv_calib3d)
include(../../cmake/opencv.cmake)


################################################################################
# Compile and link common defintions 
################################################################################
include(../../cmake/compile_link_general.cmake)

target_compile_definitions(${PROJECT_NAME} PRIVATE COMMONALGO_EXPORTS)


################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/.
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        )

################################################################################
# Link
################################################################################

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    $<$<PLATFORM_ID:Windows>:
        version
        $<$<CONFIG:Debug>:InuCommonUtilitiesd.lib>
        $<$<CONFIG:Release>:InuCommonUtilities.lib> >
    # Linux and Android
    $<$<OR:$<PLATFORM_ID:Linux>,$<PLATFORM_ID:Android>>:
        InuCommonUtilities >
)

target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}" "${BASIC_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PUBLIC
    $<$<PLATFORM_ID:Windows>:
        ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG> >
    $<$<OR:$<PLATFORM_ID:Linux>,$<PLATFORM_ID:Android>>:
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_PLATFORM_NAME}/ >
)


