################################################################################
# InuAlgWebcamIntegration cmake project 
# 
# Copyright (C) 2014 All rights reserved to Inuitive
################################################################################

cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 8.1 CACHE STRING "" FORCE)

# Need to read the version from Version.h or overwrite it in the build script
project(InuAlgWebcamIntegration LANGUAGES CXX VERSION 1.0)

if(CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_SUPPRESS_REGENERATION true)
endif()
################################################################################
# Source groups
################################################################################
set(HeaderFiles
    "AuxFunctions.h"
    "basic_functions.h"
    "WebCamInterClass.h"
    "../../include/WebcamIntegrationBase.h"
    "../../include/AlgWebcamIntegrationDLL.h"
)
source_group("HeaderFiles " FILES ${HeaderFiles})

set(SourceFiles
    "WebCamInterClass.cpp"
    "AlgWebcamIntegrationDLL.cpp"
    "WebCamIntegrationImp.cpp"
)
source_group("SourceFiles" FILES ${SourceFiles})

if(CMAKE_VS_PLATFORM_NAME)
    set(VersionFiles
        "resource.h"
        "AlgWebcamIntegration.rc"
    )
else()
    set(VersionFiles )
endif()
source_group("VersionFiles" FILES ${VersionFiles})

set(ALL_FILES
    ${HeaderFiles}
    ${SourceFiles}
    ${VersionFiles}
)


################################################################################
# Additional Global Settings(add specific info there)
# must be defined before the target definition
################################################################################
include(../../cmake/global_settings.cmake)


################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

add_dependencies(${PROJECT_NAME} InuCommonUtilities InuCommonAlgo)

set(ROOT_NAMESPACE InuAlgWebcamIntegration)

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
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_PLATFORM_NAME}"
    )
endif()
      
################################################################################
# Boost
################################################################################
set(Boost_Required_Libs system filesystem chrono regex thread atomic)
include(../../cmake/boost.cmake)

################################################################################
# OpenCV
################################################################################
set(OpenCV_Required_Libs opencv_calib3d opencv_imgproc opencv_core opencv_imgcodecs)
include(../../cmake/opencv.cmake)

################################################################################
# Include directories
################################################################################
# must be defined before the include of Common
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/.
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include 
 )

################################################################################
# Compile and link common defintions 
################################################################################
include(../../cmake/compile_link_general.cmake)

target_compile_definitions(${PROJECT_NAME} PRIVATE ALG_WEBCAM_INTEGRATION_EXPORTS)

if(CMAKE_VS_PLATFORM_NAME)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>: /Od >
        $<$<CONFIG:Release>: /O2;/Oi;/Gy >
        /W3;/openmp;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
    )
else ()
    #Common to all Linux flavors
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>: -O0 -g>
        $<$<CONFIG:Release>: -O3>
        -fmessage-length=0
        -Wall
        -fopenmp
        -Wno-unknown-pragmas
        -Wno-reorder
        -Wno-unused-result
        -Wno-parentheses
        -fPIC
    )
	target_link_options(${PROJECT_NAME} PRIVATE -fopenmp)
endif()

################################################################################
# Link
################################################################################

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    $<$<PLATFORM_ID:Windows>:
        version
        $<$<CONFIG:Debug>:
            InuCommonUtilitiesd.lib
            InuCommonAlgo.lib
            InuSensorsDBd.lib >
        $<$<CONFIG:Release>:
            InuCommonUtilities.lib
            InuCommonAlgo.lib
            InuSensorsDB.lib >>
    $<$<OR:$<PLATFORM_ID:Linux>,$<PLATFORM_ID:Android>>:
        InuCommonUtilities
        InuCommonAlgo
        InuSensorsDB >
)

target_link_libraries(${PROJECT_NAME} PUBLIC "${BASIC_LIBRARY_DEPENDENCIES}" "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PUBLIC
    $<$<PLATFORM_ID:Windows>:
        ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG> >
    $<$<OR:$<PLATFORM_ID:Linux>,$<PLATFORM_ID:Android>>:
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_PLATFORM_NAME}/ >
)
