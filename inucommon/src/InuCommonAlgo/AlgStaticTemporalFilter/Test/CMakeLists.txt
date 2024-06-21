################################################################################
# CommonAlgo cmake project 
# 
# Copyright (C) 2014 All rights reserved to Inuitive
################################################################################

cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 8.1 CACHE STRING "" FORCE)

# Need to read the version from Version.h or overwrite it in the build script
project(AlgStaticTemporalFilterTest LANGUAGES CXX VERSION 1.0)

################################################################################
# Source groups
################################################################################
set(HeaderFiles
    "../../../../include/StaticTemporalFilterBaseDLL.h"
    "CFileInfo.h"
    "egetopt.h"
)
source_group("Header Files" FILES ${HeaderFiles})

set(SourceFiles
    "CFileInfo.cpp"
    "egetopt.cpp"
    "main.cpp"
)
source_group("Source Files" FILES ${SourceFiles})

set(ALL_FILES
    ${HeaderFiles}
    ${SourceFiles}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME}  ${ALL_FILES})

add_dependencies(${PROJECT_NAME} InuCommonAlgo)

################################################################################
# Output directory
################################################################################
if(CMAKE_VS_PLATFORM_NAME)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        # On Windows need to set the Runtime folder in order to control the build path
        RUNTIME_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../../../bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
        ARCHIVE_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    )  
else()
   set_target_properties(${PROJECT_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../../../bin/${CMAKE_PLATFORM_NAME}"
    )
endif()

################################################################################
# Additional Global Settings(add specific info there)
# must be defined before the target definition
################################################################################
include(../../../cmake/global_settings.cmake)
      
################################################################################
# Boost
################################################################################
set(Boost_Required_Libs system filesystem thread chrono atomic)
include(../../../cmake/boost.cmake)

################################################################################
# OpenCV
################################################################################
set(OpenCV_Required_Libs opencv_core opencv_imgproc opencv_calib3d opencv_imgcodecs)
include(../../../cmake/opencv.cmake)

################################################################################
# Compile and link common defintions 
################################################################################
include(../../../cmake/compile_link_general.cmake)


################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/.
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../include
        )

################################################################################
# Link
################################################################################
target_link_options(${PROJECT_NAME} PRIVATE
    $<$<PLATFORM_ID:Windows>: /SUBSYSTEM:CONSOLE >
)
set(ADDITIONAL_LIBRARY_DEPENDENCIES
        $<$<PLATFORM_ID:Windows>:
            InuCommonAlgo.lib
            $<$<CONFIG:Debug>:  InuCommonUtilitiesd.lib >
            $<$<CONFIG:Release>:  InuCommonUtilities.lib > >
        $<$<OR:$<PLATFORM_ID:Linux>,$<PLATFORM_ID:Android>>:
            InuCommonUtilities
            InuCommonAlgo.lib >
)

target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}" "${BASIC_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PUBLIC
    $<$<PLATFORM_ID:Windows>:
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../lib/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG> >
    $<$<OR:$<PLATFORM_ID:Linux>,$<PLATFORM_ID:Android>>:
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../../bin/${CMAKE_PLATFORM_NAME}/ >
)