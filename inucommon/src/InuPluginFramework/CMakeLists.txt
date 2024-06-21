################################################################################
# CommonAlgo cmake project 
# 
# Copyright (C) 2014 All rights reserved to Inuitive
################################################################################

cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 8.1 CACHE STRING "" FORCE)

# Need to read the version from Version.h or overwrite it in the build script
project(InuPluginFramework LANGUAGES CXX VERSION 1.0)

################################################################################
# Source groups
################################################################################
set(HeaderFiles
    "../../include/PluginFramework/PluginManager.h"
    "../../include/PluginFramework/PluginHelper.h"
    "../../include/PluginFramework/ObjectAdapter.h"
    "../../include/PluginFramework/plugin.h"
    "DynamicLibrary.h"
)
source_group("Header Files" FILES ${HeaderFiles})

set(SourceFiles
    "PluginManager.cpp"
    "DynamicLibrary.cpp"
)
source_group("Source Files" FILES ${SourceFiles})

set(ALL_FILES
    ${HeaderFiles}
    ${SourceFiles}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

set(ROOT_NAMESPACE InuPluginFramework)

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
        ARCHIVE_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_PLATFORM_NAME}"
    )
endif()


################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(../../cmake/global_settings.cmake)
       
################################################################################
# Boost
################################################################################
set(Boost_Required_Libs system filesystem thread chrono atomic)
include(../../cmake/boost.cmake)

################################################################################
# Compile and link common defintions 
################################################################################
include(../../cmake/compile_link_general.cmake)

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/.
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include/PluginFramework
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        )

################################################################################
# Link
################################################################################
