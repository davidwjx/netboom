#########################################
# Common Compile and Link definitions 
#########################################

#########################################
# Compilation flags
#########################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
        $<$<PLATFORM_ID:Windows>:
            $<$<CONFIG:Debug>:_DEBUG>
            $<$<CONFIG:Release>:NDEBUG>
            WIN32
            _WINDOWS
            _USRDLL
            UNICODE
            _UNICODE>
        $<$<PLATFORM_ID:Linux>:
            NDEBUG
            LINUX_HOST>
)

if(CMAKE_VS_PLATFORM_NAME)
    message("Set Windows options" )

    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:
            /Od
        >
        $<$<CONFIG:Release>:
            /O2;
            /Oi;
            /Gy
        >
        /W3;
        ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
        /wd4996;
        ${DEFAULT_CXX_EXCEPTION_HANDLING};
        /Y-
        /MP
       /openmp
    )
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "Win32")
        target_compile_options(${PROJECT_NAME} PRIVATE
            /arch:SSE2;
        )
    endif()

else ()

    message("Set non-Windows options" )

    #Common to all Linux flavors
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>: -O0 -g>
        $<$<CONFIG:Release>: -O3>
        -fmessage-length=0
        -Wall
        -Wno-unknown-pragmas
        -Wno-reorder
        -Wno-unused-result
        -Wno-parentheses
        -fPIC
        -fopenmp
    )

    # Specific to each platform
    if ( (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm") OR (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "arm64") )
        target_compile_options(${PROJECT_NAME}  PRIVATE
            -fsigned-char -ffunction-sections -fdata-sections  -flax-vector-conversions)
    elseif( (${CMAKE_SYSTEM_NAME} STREQUAL "Android") AND (${CMAKE_SYSTEM_VERSION} STREQUAL "23")  )
        target_compile_options(${PROJECT_NAME}  PRIVATE -std=c++0x)
    else()
        target_compile_options(${PROJECT_NAME}  PRIVATE
            $<$<PLATFORM_ID:Linux>:
                -msse4 -mno-avx -mno-avx2 -m64 >)
    endif()
endif()

#########################################
# Include paths
#########################################
if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    if (${CMAKE_ANDROID_API} STREQUAL "23")
            target_include_directories(${PROJECT_NAME} PRIVATE
                ${CMAKE_ANDROID_NDK}/sysroot/usr/include/aarch64-linux-android
                ${CMAKE_ANDROID_NDK}/sources/cxx-stl/llvm-libc++/include
                ${CMAKE_ANDROID_NDK}/sysroot/usr/include
                ${CMAKE_ANDROID_NDK}/platforms/android-23/arch-arm64/usr/include)
    endif()
endif()

#########################################
# link options
#########################################

target_link_options(${PROJECT_NAME} PRIVATE
    $<$<PLATFORM_ID:Windows>:
        $<$<CONFIG:Debug>:
            /INCREMENTAL >
        $<$<CONFIG:Release>:
            /NODEFAULTLIB:libc;
            /OPT:REF;
            /OPT:ICF;
            /INCREMENTAL:NO  >
        /DEBUG;
        /SUBSYSTEM:WINDOWS >
    $<$<OR:$<PLATFORM_ID:Linux>,$<PLATFORM_ID:Android>>:
        -Wl,-rpath,./
        -Wl,--no-undefined
         #-Wl,--verbose
        -Wl,--unresolved-symbols=report-all
        -fopenmp
        >
)

if ( "${CMAKE_PLATFORM_NAME}" STREQUAL "linux_gcc-7.4_x86_64" )
    target_link_options(${PROJECT_NAME} PRIVATE -msse4 -m64 )
endif()

#########################################
# link paths
#########################################
if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
    if (${CMAKE_ANDROID_API} STREQUAL "23")
        target_link_directories(${PROJECT_NAME} PUBLIC
            ${CMAKE_ANDROID_NDK}/sources/cxx-stl/llvm-libc++/libs/arm64-v8a
            ${CMAKE_ANDROID_NDK}/toolchains/$ENV{CROSS_HOST} )
    else()
        target_link_directories(${PROJECT_NAME} PUBLIC
            ${CMAKE_ANDROID_NDK}/sources/cxx-stl/llvm-libc++/libs/arm64-v8a
            ${CMAKE_ANDROID_NDK}/$ENV{CROSS_HOST}/sysroot/usr/lib/aarch64-linux-android/${__ANDROID_API_LEVEL__} )
    endif()
endif()


#########################################
# linked libraries
#########################################
set(BASIC_LIBRARY_DEPENDENCIES
    # Linux and Android
    $<$<OR:$<PLATFORM_ID:Linux>,$<PLATFORM_ID:Android>>:
        dl>
    # All Linux
    $<$<PLATFORM_ID:Linux>:
        gcc
        rt
        pthread>
    # Linux GNU (x86) only
    $<$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},x86_64>:
        z >
    # Android only
    $<$<PLATFORM_ID:Android>:
        archive
        crypto.a
        log
        c++_shared
        stdc++
        m
        z >
)
