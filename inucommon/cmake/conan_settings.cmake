##########################################################
# CMake file to define install settins for all platforms
##########################################################

set(ARCH x86_64)
if (CMAKE_PLATFORM_NAME STREQUAL "linux_gcc-7.3_armv8" OR CMAKE_PLATFORM_NAME STREQUAL "linux_gcc-5.4_armv8")
    set(ARCH armv8)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Android")
    include(${CMAKE_CURRENT_LIST_DIR}/conan.cmake)

    conan_cmake_autodetect(settings)
else()
    if(CMAKE_VS_PLATFORM_NAME)
        set(CMAKE_SYSTEM_NAME Windows)
        set(comp "Visual Studio")
        set(comp_ver 16)
    else()
        set(comp gcc)
        if (CMAKE_PLATFORM_NAME STREQUAL "linux_gcc-5.4_x86_64" OR CMAKE_PLATFORM_NAME STREQUAL "linux_gcc-5.4_armv8")
            set(comp_ver 5.4)
        else()
            set(comp_ver 7)
        endif()
    endif()
    set(settings 
        os=${CMAKE_SYSTEM_NAME}
        compiler=${comp}
        compiler.version=${comp_ver}
        compiler.cppstd=${CMAKE_CXX_STANDARD}
        # build_type=${CMAKE_BUILD_TYPE}
        arch=${ARCH})
endif()