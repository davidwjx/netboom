################################################################################
# SensorsDB unititest cmake project 
# 
# Copyright (C) 2014 All rights reserved to Inuitive
################################################################################

cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 8.1 CACHE STRING "" FORCE)

# Need to read the version from Version.h or overwrite it in the build script
project(InuCommonUtilitiesUnittest LANGUAGES CXX VERSION 1.0)

set(INUCOMMON_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../..)

################################################################################
# CONAN
################################################################################

if (CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_PLATFORM_NAME ${CMAKE_VS_PLATFORM_NAME})
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(CMAKE_CXX_STANDARD 17)
endif()

if(USE_CONAN)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/conan.cmake")
    endif()
endif()

if (EXISTS ${INUCOMMON_ROOT}/cmake/conan.cmake)
    include(${INUCOMMON_ROOT}/cmake/conan.cmake)
endif()

# if conan builds the dependencies we want to tell it what settings to use for the build
if (EXISTS ${INUCOMMON_ROOT}/cmake/conan_settings.cmake)
    include(${INUCOMMON_ROOT}/cmake/conan_settings.cmake)
    # since at the moment, conan doesn't build the dependencies, but only imports them, the settings don't matter, 
    # so we'll let conan.cmake to detect settings automatically from build machine
    conan_cmake_autodetect(settings)
endif()

if(USE_CONAN)

    set(CONAN_BUILD_INFO_PATH ${INUCOMMON_ROOT}/src/Conanfiles/${CMAKE_PLATFORM_NAME})
    if(CMAKE_VS_PLATFORM_NAME)
        set(CONAN_BUILD_INFO_OUTPUT_PATH ${CONAN_BUILD_INFO_PATH}/${CMAKE_BUILD_TYPE})
    else()
        set(CONAN_BUILD_INFO_OUTPUT_PATH ${CONAN_BUILD_INFO_PATH})
    endif()


    # If defined to use conan, import third party packages and InuCommon from Artifactory
    set(parameter_trials -c)
    if (CMAKE_VS_PLATFORM_NAME)
        set(parameter_trials -n)
    endif()

    execute_process(COMMAND ping 192.168.80.72 ${parameter_trials} 1
                    RESULT_VARIABLE status1
                    ERROR_QUIET
                    OUTPUT_QUIET)

    if (status1 EQUAL 1)
        message(FATAL_ERROR
                "\nNo connection with Artifactory server.Restablish connection or build with USE_CONAN=FALSE.\n")
    endif()

    conan_cmake_install(
                UPDATE
                PATH_OR_REFERENCE ${INUCOMMON_ROOT}/src/Conanfiles/${CMAKE_PLATFORM_NAME}
                INSTALL_FOLDER ${CONAN_BUILD_INFO_OUTPUT_PATH}
                GENERATOR cmake
                BUILD missing
                SETTINGS ${settings})

else()
    message(WARNING "\nUSE_CONAN=FALSE - working with cache and not with latest version from Artifactory\n${ColourReset}")

	set (CONAN_EXE_FILE "/usr/bin/conan")
	if (CMAKE_VS_PLATFORM_NAME)
		set (CONAN_EXE_FILE "C:/Program Files/Conan/conan/conan.exe")
	endif()

	if ((EXISTS ${CONAN_EXE_FILE}) AND (EXISTS ${INUCOMMON_ROOT}/src/cmake/conan.cmake))
		conan_cmake_install(
                    PATH_OR_REFERENCE ${INUCOMMON_ROOT}/src/Conanfiles/${CMAKE_PLATFORM_NAME}
		    INSTALL_FOLDER ${CONAN_BUILD_INFO_OUTPUT_PATH}
		    GENERATOR cmake
		    BUILD missing
		    SETTINGS ${settings})
	else()
		message(WARNING "Conan file not found ${CONAN_EXE_FILE}")
	endif()		    
endif()

################################################################################
# Sub modules
################################################################################
if (REBUILD_SENSORSDB)
    add_subdirectory(${INUCOMMON_ROOT}/src/InuCommonUtilities  InuCommonUtilities)
    add_subdirectory(${INUCOMMON_ROOT}/src/InuSensorsDB  InuSensorsDB)
else()
    if ((CMAKE_VS_PLATFORM_NAME AND 
    ((CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT EXISTS ${INUCOMMON_ROOT}/lib/${CMAKE_VS_PLATFORM_NAME}/Release/InuSensorsDB.lib) OR
    (CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT EXISTS ${INUCOMMON_ROOT}/lib/${CMAKE_VS_PLATFORM_NAME}/Debug/InuCommonUtilities.lib))) OR
        (NOT DEFINED CMAKE_VS_PLATFORM_NAME AND NOT EXISTS ${INUCOMMON_ROOT}/bin/${CMAKE_PLATFORM_NAME}/libInuCommonUtilities.so))
        message( STATUS "======================================================================================\n----------------------adding modules---------------------\n===========================================================\n###############################################################")
        add_subdirectory(${INUCOMMON_ROOT}/InuCommonUtilities ${INUCOMMON_ROOT}/InuCommonUtilities)
    endif()
endif()

################################################################################
# Additional Global Settings(add specific info there)
# must be defined before the target definition
################################################################################
include(${INUCOMMON_ROOT}/cmake/global_settings.cmake)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME}  "Unittest.cpp")

if(REBUILD_SENSORSDB)
    add_dependencies(${PROJECT_NAME} InuCommonUtilities)

    add_dependencies(${PROJECT_NAME} InuSensorsDB)
endif()

#use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE InuSensorsDBUnittest)

# Instruct Visual Studio not to build CHECK_ZERO solution
if(CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_SUPPRESS_REGENERATION true)
endif()

################################################################################
# Output directory
################################################################################
set(OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_PLATFORM_NAME} )

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}) 
    
if(CMAKE_VS_PLATFORM_NAME)
    set_target_properties(${PROJECT_NAME} PROPERTIES PDB_NAME "${PROJECT_NAME}.exe")
endif()

################################################################################
# Boost
################################################################################
set(Boost_Required_Libs system filesystem thread chrono atomic)
include(${INUCOMMON_ROOT}/cmake/boost.cmake)

################################################################################
# OpenCV
################################################################################
#set(OpenCV_Required_Libs opencv_core opencv_imgproc opencv_calib3d opencv_imgcodecs)
#include(${INUCOMMON_ROOT}/cmake/opencv.cmake)

################################################################################
# Compile and link common defintions 
################################################################################
include(${INUCOMMON_ROOT}/cmake/compile_link_general.cmake)


################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/.
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${INUCOMMON_ROOT}/include
        )

################################################################################
# Link
################################################################################

target_link_options(${PROJECT_NAME} PRIVATE
    $<$<PLATFORM_ID:Windows>: /SUBSYSTEM:CONSOLE >
)
set(ADDITIONAL_LIBRARY_DEPENDENCIES
    $<$<PLATFORM_ID:Windows>:
        $<$<CONFIG:Debug>: InuCommonUtilitiesd.lib InuSensorsDBd.lib >
        $<$<CONFIG:Release>: InuCommonUtilities.lib InuSensorsDB.lib> >
    # Linux and Android
    $<$<OR:$<PLATFORM_ID:Linux>,$<PLATFORM_ID:Android>>:
        InuCommonUtilities InuSensorsDB >
)

target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}" "${BASIC_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PUBLIC
    $<$<PLATFORM_ID:Windows>:
        ${INUCOMMON_ROOT}/lib/${CMAKE_VS_PLATFORM_NAME}/$<CONFIG> >
    $<$<PLATFORM_ID:Linux>:
        ${INUCOMMON_ROOT}/bin/${CMAKE_PLATFORM_NAME}/ >
)

################################################################################
# Debug
################################################################################

set(BIN_PATHS1
    ${INUCOMMON_ROOT}/bin/${CMAKE_PLATFORM_NAME}/${CMAKE_BUILD_TYPE}
    ${OpenCV_BIN_DIRS}
)

cmake_path(CONVERT "${BIN_PATHS1}" TO_NATIVE_PATH_LIST BIN_PATHS2)
string(REGEX REPLACE "\\\\" "\\\\\\\\" BIN_PATHS_UNITTEST "${BIN_PATHS2}")
message ("Paths of binaries for execution: ${BIN_PATHS_UNITTEST}")

if(CMAKE_VS_PLATFORM_NAME)
    set(OUTPUT_DIRECTORY ${OUTPUT_DIRECTORY}/${CMAKE_BUILD_TYPE})
endif()

set(EXE_FILENAME1 ${OUTPUT_DIRECTORY}/${PROJECT_NAME})
if (CMAKE_VS_PLATFORM_NAME)
    set(EXE_FILENAME1 "${EXE_FILENAME1}.exe")
endif()    

cmake_path(CONVERT "${EXE_FILENAME1}" TO_NATIVE_PATH_LIST EXE_FILENAME2)
string(REGEX REPLACE "\\\\" "\\\\\\\\" EXE_FILENAME_UNITTEST "${EXE_FILENAME2}")
message ("Executabe file name EXE_FILENAME_SANDBOX: ${EXE_FILENAME_UNITTEST}")

set (PROJECT_NAME_UNITTEST ${PROJECT_NAME}.exe)

if (CMAKE_VS_PLATFORM_NAME)
    # Rmoving .vs is required in order to bypass unexpected problem in VS when switching from debug/release builds
    file(REMOVE_RECURSE "${CMAKE_CURRENT_SOURCE_DIR}/.vs")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/launch.vs.json.in" "${CMAKE_CURRENT_SOURCE_DIR}/.vs/launch.vs.json")
else()
   configure_file("${CMAKE_CURRENT_SOURCE_DIR}/launch.json.in" "${CMAKE_CURRENT_SOURCE_DIR}/.vscode/launch.json")
endif()


