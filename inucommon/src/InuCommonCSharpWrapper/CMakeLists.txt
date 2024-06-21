################################################################################
# CommonAlgo cmake project 
# 
# Copyright (C) 2014 All rights reserved to Inuitive
################################################################################

cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 8.1 CACHE STRING "" FORCE)

# Need to read the version from Version.h or overwrite it in the build script
project(InuCommonCSharpWrapper LANGUAGES CXX VERSION 1.0)

################################################################################
# Source groups
################################################################################
set(HeaderFiles
    "InuModelDBWrapper.h"
    "InuModelWrapper.h"
    "RecordingMetaDataWrapper.h"
)
source_group("Header Files" FILES ${HeaderFiles})

set(SourceFiles
    "InuModelDBWrapper.cpp"
    "InuModelWrapper.cpp"
    "RecordingMetaDataWrapper.cpp"
)
source_group("Source Files" FILES ${SourceFiles})

if(CMAKE_VS_PLATFORM_NAME)
    set(VersionFiles
        "resource.h"
        "CommonUtilitiesCSharpWrapper.rc"
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
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

add_dependencies(${PROJECT_NAME} InuCommonUtilities InuSensorsDB)

set(ROOT_NAMESPACE InuCommonCSharpWrapper)

################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
    # On Windows need to set the Runtime folder in order to control the build path
    RUNTIME_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
    ARCHIVE_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG>/"
)


################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(../../cmake/global_settings.cmake)

################################################################################
# Compile and link common defintions 
################################################################################
include(../../cmake/compile_link_general.cmake)

# add managed c++ flag
target_compile_options(${PROJECT_NAME} PRIVATE /clr)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<PLATFORM_ID:Windows>: _IG_6231 >)

################################################################################
# Boost
################################################################################
set(Boost_Required_Libs system filesystem thread chrono atomic)
include(../../cmake/boost.cmake)

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
    $<$<CONFIG:Debug>:InuCommonUtilitiesd.lib InuSensorsDBd.lib > 
    $<$<CONFIG:Release>:InuCommonUtilities.lib InuSensorsDB.lib> >
)

target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}" "${BASIC_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../../lib/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG> >
)
