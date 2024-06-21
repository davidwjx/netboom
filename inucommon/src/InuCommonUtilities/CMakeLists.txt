################################################################################
# InuCommonUtilies cmake project 
# 
# Copyright (C) 2014 All rights reserved to Inuitive
################################################################################

cmake_minimum_required(VERSION 3.15.0 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 8.1 CACHE STRING "" FORCE)

# Need to read the version from Version.h or overwrite it in the build script
project(InuCommonUtilities LANGUAGES CXX VERSION 1.0)

################################################################################
# Additional Global Settings(add specific info there)
################################################################################
include(../../cmake/global_settings.cmake)


################################################################################
# Source groups
################################################################################

set(BoostFix
    "../../include/BoostFix/condition_algorithm_8a_fix.hpp"
    "../../include/BoostFix/condition_fix.hpp"
    "../../include/BoostFix/interprocess_condition_fix.hpp"
    "../../include/BoostFix/message_queue_fix.hpp"
    "../../include/BoostFix/os_file_functions_fix.hpp"
    "../../include/BoostFix/wait_fix.hpp"
    "../../include/BoostFix/win32_api.hpp"
    "../../include/BoostFix/windows_intermodule_singleton_fix.hpp"
)
source_group("BoostFix" FILES ${BoostFix})

set(Calibration
    "../../include/CalibrationTablesUtilities.h"
    "../../include/TemperatureMetaData.h"
    "../../include/CalibrationData2Yaml.h"
    "../../include/CalibrationDataImp.h"
    "Calibration/CalibrationTablesUtilities.cpp"
    "Calibration/TemperatureMetaData.cpp"
    "Calibration/CalibrationData2Yaml.cpp"
    "Calibration/CalibrationDataImp.cpp"
)

source_group("Calibration" FILES ${Calibration})

set(FileFormats
    "../../include/CiifFile.h"
    "../../include/CsvFile.h"
    "../../include/ImageFile.h"
    "../../include/IniFile.h"
    "../../include/IregFile.h"
    "../../include/JpegFile.h"
    "../../include/PngFile.h"
    "../../include/TiffFile.h"
    "../../include/FeaturesTrackingCiif.h"
    "FileFormats/CiifFile.cpp"
    "FileFormats/CsvFile.cpp"
    "FileFormats/IniFile.cpp"
    "FileFormats/IregFile.cpp"
    "FileFormats/JpegFile.cpp"
    "FileFormats/jpgd.cpp"
    "FileFormats/jpgd.h"
    "FileFormats/jpge.cpp"
    "FileFormats/jpge.h"
    "FileFormats/lodepng.cpp"
    "FileFormats/lodepng.h"
    "FileFormats/PngFile.cpp"
    "FileFormats/TiffFile.cpp"
    "FileFormats/FeaturesTrackingCiif.cpp"
)
source_group("FileFormats" FILES ${FileFormats})

set(General
    "../../include/CalibrationData.h"
    "../../include/CalibrationData2Yaml.h"
    "../../include/CommonUtilitiesDefs.h"
    "../../include/FeaturesTrackingDataCommon.h"
    "../../include/HexHelper.h"
    "../../include/LocalSocketConnection.h"
    "../../include/Locks.h"
    "../../include/RecordingMetaData.h"
    "../../include/TCPConnection.h"
    "../../include/Version.h"
    "LocalSocketConnection.cpp"
    "RecordingMetaData.cpp"
    "TCPConnection.cpp"
    "HexHelper.cpp"
)

if(CMAKE_VS_PLATFORM_NAME)
set(GeneralVS
    "resource.h"
    "dllmain.cpp"
    "CommonUtilities.rc"
)
else()
set(GeneralVS )
endif()
source_group("General" FILES ${General} ${GeneralVS})

set(Logger
    "../../include/BaseLogger.h"
    "../../include/Logger.h"
    "../../include/TextFileLogger.h"
    "Logger/ConsoleLogger.cpp"
    "Logger/ConsoleLogger.h"
    "Logger/DebugViewLogger.cpp"
    "Logger/DebugViewLogger.h"
    "Logger/Logger.cpp"
    "Logger/TextFileLogger.cpp"
)
source_group("Logger" FILES ${Logger})

set(OS
    "../../include/OSUtilities.h"
    "../../include/OSUtilitiesGCC.h"
    "../../include/OSUtilitiesVS.h"
    "OS/OSUtilitiesCommon.cpp"
    "OS/OSUtilitiesGCC.cpp"
    "OS/OSUtilitiesVS.cpp"
)
source_group("OS" FILES ${OS})

set(Persistent
    "../../include/FileSystemStorage.h"
    "../../include/PersistentData.h"
    "../../include/Storage.h"
    "../../include/XmlStorage.h"
    "Persistent/FileSystemStorage.cpp"
    "Persistent/Storage.cpp"
    "Persistent/XmlParser.cpp"
    "Persistent/XmlStorage.cpp"
)
source_group("Persistent" FILES ${Persistent})

set(Process
    "../../include/ProcessWrapper.h"
    "Process/ProcessWrapper.cpp"
)
source_group("Process" FILES ${Process})

set(ALL_FILES
    ${no_group_source_files}
    ${BoostFix}
    ${Calibration}
    ${FileFormats}
    ${General}
    ${GeneralVS}
    ${Logger}
    ${OS}
    ${Persistent}
    ${Process}
)

################################################################################
# Target
################################################################################
if (NOT TARGET ${PROJECT_NAME})
    add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

    # Add output of Generator expressions that are used in the build.
    add_custom_command(TARGET InuCommonUtilities POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo
        "platform dir = $<PLATFORM_ID>"
)
else()
    message(WARNING "${PROJECT_NAME} was already added by another project")
endif()

#use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE InuCommonUtilities)

################################################################################
# Boost
################################################################################
set(Boost_Required_Libs system filesystem chrono regex thread)
include(../../cmake/boost.cmake)

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
# Compile and link common defintions 
################################################################################
include(../../cmake/compile_link_general.cmake)

add_compile_definitions(${PROJECT_NAME} PRIVATE COMMONUTILITIES_EXPORTS)

################################################################################
# 3rdParty packages
################################################################################
set(THIRD_PARTY_INCLUDE_DIRS ${TIFF_INCLUDE} ${ARCHIVE_INCLUDE})
set(THIRD_PARTY_LIB_DIRS ${YAML_LIBS} ${TIFF_LIBS} ${ARCHIVE_LIBS})
    
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/.
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${THIRD_PARTY_INCLUDE_DIRS}
		${YAML_INCLUDE}
 )

################################################################################
# Link
################################################################################

set(ADDITIONAL_LIBRARY_DEPENDENCIES
    $<$<PLATFORM_ID:Windows>:
        $<$<CONFIG:Debug>:  libyaml-cppmdd>
        $<$<CONFIG:Release>: libyaml-cppmd>
        LibTiff
        version >
    # Linux and Android
    $<$<OR:$<PLATFORM_ID:Linux>,$<PLATFORM_ID:Android>>:
        yaml-cpp 
        tiff >
)

target_link_libraries(${PROJECT_NAME} PUBLIC "${ADDITIONAL_LIBRARY_DEPENDENCIES}" "${BASIC_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PUBLIC ${THIRD_PARTY_LIB_DIRS})


################################################################################


