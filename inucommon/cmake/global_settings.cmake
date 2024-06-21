
####################
# Global settins for all projects
####################


################################################################################
# Set the platform name
################################################################################
if (CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_PLATFORM_NAME ${CMAKE_VS_PLATFORM_NAME})
endif()

message("platform-name:  ${CMAKE_PLATFORM_NAME} ")

################################################################################
# Error function in case undefined package
################################################################################
function(use_package TARGET PACKAGE VERSION)
    message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()


################################################################################
# Use solution folders feature
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
# Paths of include and libraries for all projects 
################################################################################
set(BUILD_INFO_PATH ${CMAKE_CURRENT_LIST_DIR}/../src/Conanfiles/${CMAKE_PLATFORM_NAME})
if (CMAKE_VS_PLATFORM_NAME)
    #message(FATAL_ERROR ${BUILD_INFO_PATH} " " ${CMAKE_BUILD_TYPE})
    set(BUILD_INFO_PATH ${BUILD_INFO_PATH}/${CMAKE_BUILD_TYPE})
endif()

if (EXISTS ${BUILD_INFO_PATH}/conanbuildinfo.cmake)
	include(${BUILD_INFO_PATH}/conanbuildinfo.cmake)
elseif (USE_CONAN)
	message(FATAL_ERROR "File not found ${BUILD_INFO_PATH}/conanbuildinfo.cmake")
else()
	message(WARNING "File not found ${BUILD_INFO_PATH}/conanbuildinfo.cmake, but USE_CONA=FALSE")
endif()
	
string(TOUPPER ${CMAKE_PLATFORM_NAME} PLATFORM)

function(CheckIfPathExists VAR_NAME CHECKED_PATH)
    if (NOT EXISTS ${CHECKED_PATH})
        message(FATAL_ERROR "Can't open  ${CHECKED_PATH}")
    else()
        message(STATUS "${VAR_NAME} Path: ${CHECKED_PATH} ")
    endif()
endfunction()

function(CheckIfAllPathExists VAR_NAME INCLUDE_PATH LIB_PATH)
    CheckIfPathExists("${VAR_NAME} Include" ${INCLUDE_PATH})
    CheckIfPathExists("${VAR_NAME} Lib"  ${LIB_PATH})
endfunction()

if (THIRD_PARTY_ROOT)
    set(YAML_INCLUDE ${THIRD_PARTY_ROOT}/include/yaml-cpp/include)
    set(YAML_LIBS ${THIRD_PARTY_ROOT}/lib/${CMAKE_PLATFORM_NAME})

    if(CMAKE_VS_PLATFORM_NAME)
        set(TIFF_INCLUDE  ${THIRD_PARTY_ROOT}/include/tiff-4.0.3/libtiff)
    else()
        set(TIFF_INCLUDE  ${THIRD_PARTY_ROOT}/include/tiff-4.0.3-${CMAKE_SYSTEM_NAME}/libtiff)
    endif()
    set(TIFF_LIBS ${THIRD_PARTY_ROOT}/lib/${CMAKE_PLATFORM_NAME})

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
        set(ARCHIVE_INCLUDE ${THIRD_PARTY_ROOT}/libarchive/libarchive-3.3.2/libarchive)
        set(ARCHIVE_LIBS ${THIRD_PARTY_ROOT}/libarchive/openssl-android-master/libs/arm64-v8a/)
    endif()
else()
    # Build from Conan
    string(TOUPPER ${CMAKE_SYSTEM_NAME} SYSTEM_STR)

    set(YAML_INCLUDE ${CONAN_INCLUDE_DIRS_YAML-CPP})
    set(YAML_LIBS ${CONAN_LIB_DIRS_YAML-CPP}/${CMAKE_PLATFORM_NAME})

    set(TIFF_INCLUDE ${CONAN_INCLUDE_DIRS_TIFF_${SYSTEM_STR}})
    if(CMAKE_VS_PLATFORM_NAME)
        set(TIFF_LIBS ${CONAN_LIB_DIRS_TIFF_${SYSTEM_STR}})
    else()
        set(TIFF_LIBS ${CONAN_LIB_DIRS_TIFF_${SYSTEM_STR}}/${CMAKE_PLATFORM_NAME})
    endif()

    if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
        set(ARCHIVE_INCLUDE ${CONAN_INCLUDE_DIRS_LIBARCHIVE})
        set(ARCHIVE_LIBS ${CONAN_LIB_DIRS_LIBARCHIVE})
    endif()
endif()

if(CMAKE_VS_PLATFORM_NAME)
    set(YAML_LIBS ${YAML_LIBS}/${CMAKE_BUILD_TYPE})
    set(ARCHIVE_LIBS ${ARCHIVE_LIBS}/${CMAKE_BUILD_TYPE})
    set(TIFF_LIBS ${TIFF_LIBS}/${CMAKE_BUILD_TYPE})
endif()

CheckIfAllPathExists(YAML ${YAML_INCLUDE} ${YAML_LIBS} )
CheckIfAllPathExists(TIFF ${TIFF_INCLUDE} ${TIFF_LIBS} )
if (${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    CheckIfAllPathExists(ARCHIVE ${ARCHIVE_INCLUDE} ${ARCHIVE_LIBS} )
    # Add libcrypto.a path from Conan
    set(ARCHIVE_LIBS ${ARCHIVE_LIBS} " " ${ARCHIVE_LIBS}/${CMAKE_PLATFORM_NAME})
endif()
