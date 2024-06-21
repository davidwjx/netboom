################################################################################
# Common package cmake 
# 
# Copyright (C) 2021 All rights reserved to Inuitive
################################################################################

cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 8.1 CACHE STRING "" FORCE)

# Need to read the version from Version.h or overwrite it in the build script
project(InuCommon LANGUAGES CXX VERSION 1.0)

set(INUCOMMON_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

################################################################################
# Conan
################################################################################
 
if(CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_PLATFORM_NAME ${CMAKE_VS_PLATFORM_NAME})
    set(CMAKE_SUPPRESS_REGENERATION true)

    set(CMAKE_CXX_STANDARD_REQUIRED True)
    set(CMAKE_CXX_STANDARD 17)
endif()

if(USE_CONAN)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake" "${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan.cmake")
    endif()
endif()

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan.cmake)

    # if conan builds the dependencies we want to tell it what settings to use for the build
    if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan_settings.cmake)
        include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan_settings.cmake)

        # since at the moment, conan doesn't build the dependencies, but only imports them, the settings don't matter, 
        # so we'll let conan.cmake to detect settings automatically from build machine
        conan_cmake_autodetect(settings)
    endif()
endif()

set(CONAN_BUILD_INFO_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/Conanfiles/${CMAKE_PLATFORM_NAME})
if(CMAKE_VS_PLATFORM_NAME)
    set(CONAN_BUILD_INFO_OUTPUT_PATH ${CONAN_BUILD_INFO_PATH}/${CMAKE_BUILD_TYPE})
else()
    set(CONAN_BUILD_INFO_OUTPUT_PATH ${CONAN_BUILD_INFO_PATH})
endif()

if(USE_CONAN)
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
                PATH_OR_REFERENCE ${CMAKE_CURRENT_SOURCE_DIR}/src/Conanfiles/${CMAKE_PLATFORM_NAME}
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
    if ((EXISTS ${CONAN_EXE_FILE}) AND (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan.cmake))
	    conan_cmake_install(
	        PATH_OR_REFERENCE ${CMAKE_CURRENT_SOURCE_DIR}/src/Conanfiles/${CMAKE_PLATFORM_NAME}
	        INSTALL_FOLDER ${CONAN_BUILD_INFO_OUTPUT_PATH}
	        GENERATOR cmake
	        BUILD missing
	        SETTINGS ${settings})
	else()
		message(WARNING "\nUSE_CONAN=FALSE - Conan is not installed, local files are used\n${ColourReset}")
	endif()
endif()

# Need to read the version from Version.h or overwrite it in the build script
add_subdirectory(src/InuCommonUtilities)

if (NOT INUSTREAMS_ONLY)
    add_subdirectory(src/InuCommonAlgo)

    add_subdirectory(src/InuSensorsDB)

    add_subdirectory(src/InuPluginFramework)

    add_subdirectory(src/AlgWebcamIntegration)


    if(CMAKE_VS_PLATFORM_NAME)
        add_subdirectory(src/InuCommonCSharpWrapper)
    else()
        add_subdirectory(src/InuCommonUtilsJNI)
    endif()

endif()

# Open the comment if you need to build the static temporal test 
#add_subdirectory(InuCommonAlgo/AlgStaticTemporalFilter/Test)



