################################################################################
# CommonAlgo cmake project 
# 
# Copyright (C) 2014 All rights reserved to Inuitive
################################################################################

cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 8.1 CACHE STRING "" FORCE)

message("INUCOMMON_VERSION  ${INUCOMMON_VERSION} ")

# Need to read the version from Version.h or overwrite it in the build script
project(InuSensorsDB LANGUAGES CXX VERSION 1.0)

################################################################################
# Source groups
################################################################################
if(CMAKE_VS_PLATFORM_NAME)
set(GeneralVS
    "resource.h"
    "dllmain.cpp"
)
else()
set(GeneralVS )
endif()

set(HeaderFiles
    "../../include/CalibrationDataAll.h"
    "../../include/InuDevice.h"
    "../../include/InuModel.h"
    "../../include/InuModelDB.h"
    "../../include/InuSensorDBDefs.h"
    "../../include/LutsDataSet.h"
    "../../include/RegistersSet.h"
    "../../include/ResolutionDataSet.h"
    "../../include/TemperatureDataSet.h"
)

source_group("HeaderFiles" FILES ${HeaderFiles})

set(SourceFiles
    "CalibrationDataAll.cpp"
    "InuDevice.cpp"
    "InuModel.cpp"
    "InuModelDB.cpp"
    "LutsDataSet.cpp"
    "MissingCalibrationsAlg.cpp"
    "RegistersSet.cpp"
    "ResolutionDataSet.cpp"
    "TemperatureDataSet.cpp"
)
source_group("SourceFiles" FILES ${SourceFiles})

set(Resources
    "InuSensorsDB.rc"
)
source_group("Resources" FILES ${Resources})

set(ALL_FILES
    ${HeaderFiles}
    ${SourceFiles}
    ${Resources}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

add_dependencies(${PROJECT_NAME} InuCommonUtilities)

#use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE InuSensorsDB)

################################################################################
# Output directory
################################################################################
if(CMAKE_VS_PLATFORM_NAME)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        # On Windows need to set the Runtime folder in order to control the build path
        RUNTIME_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        ARCHIVE_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        DEBUG_POSTFIX "d"
    )  
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_PLATFORM_NAME}"
    )
endif()


################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(${INUCOMMON_ROOT}/cmake/global_settings.cmake)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<PLATFORM_ID:Windows>: _IG_6231 >)
        
################################################################################
# Boost
################################################################################
set(Boost_Required_Libs system filesystem thread chrono atomic)
include(${INUCOMMON_ROOT}/cmake/boost.cmake)

################################################################################
# OpenCV
################################################################################
set(OpenCV_Required_Libs opencv_core opencv_imgproc opencv_calib3d opencv_imgcodecs)
include(${INUCOMMON_ROOT}/cmake/opencv.cmake)


################################################################################
# Compile and link common defintions 
################################################################################
include(${INUCOMMON_ROOT}/cmake/compile_link_general.cmake)

target_compile_definitions(${PROJECT_NAME} PRIVATE INUSENSORSDB_EXPORTS)


################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/.
        ${INUCOMMON_ROOT}/include
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
        ${INUCOMMON_ROOT}/lib/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG> >
    $<$<PLATFORM_ID:Linux>:
        ${INUCOMMON_ROOT}/bin/${CMAKE_PLATFORM_NAME}/ >
)
