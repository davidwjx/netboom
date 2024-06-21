################################################################################
# CommonUtilies cmake project 
# 
# Copyright (C) 2014 All rights reserved to Inuitive
################################################################################

cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 8.1 CACHE STRING "" FORCE)

if (CMAKE_VS_PLATFORM_NAME)
    return()
endif()

# Need to read the version from Version.h or overwrite it in the build script
project(InuCommonUtilsJNI LANGUAGES CXX VERSION 1.0)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(../../cmake/global_settings.cmake)

set(CMAKE_CXX17_STANDARD_COMPILE_OPTION "-std=c++17")

################################################################################
# Source groups
################################################################################

set(JNIWrapper
    "JniWrapper/JavaClass.h"
    "JniWrapper/JavaClassSpec.h"
    "JniWrapper/JavaObject.h"
    "JniWrapper/JavaObjectSpec.h"
    "JniWrapper/JniWrapper.h"
    "JniWrapper/JvmEnvironment.h"
    "JniWrapper/JvmEnvironment.cpp"
)
source_group("JNIWrapper" FILES ${JNIWrapper})

set(General
    "CallBackDispatcher.h"
    "dllmain.cpp"
    "InuModelDBJNI.h"
    "InuModelDBJNI.cpp"
    "InuModelJNI.h"
    "InuModelJNI.cpp"
    "Logger.h"
    "targetver.h"
    "WrapperStructsJNI.h"
)

source_group("General" FILES ${General})

set(ALL_FILES
    ${JNIWrapper}
    ${General}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

add_dependencies(${PROJECT_NAME} InuCommonUtilities InuSensorsDB)

#use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE InuCommonUtilsJNI)

################################################################################
# Boost
################################################################################
set(Boost_Required_Libs system filesystem chrono regex thread)
include(../../cmake/boost.cmake)

################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY   "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_PLATFORM_NAME}" )

################################################################################
# Compile and link common defintions 
################################################################################
include(../../cmake/compile_link_general.cmake)

################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/.
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        $<$<PLATFORM_ID:Linux>:
            $ENV{JAVA_HOME}/include
            $ENV{JAVA_HOME}/include/linux >
        )

################################################################################
# Link
################################################################################

set(ADDITIONAL_LIBRARY_DEPENDENCIES InuCommonUtilities InuSensorsDB)

target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}" "${BASIC_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/${CMAKE_PLATFORM_NAME}/ >
)
