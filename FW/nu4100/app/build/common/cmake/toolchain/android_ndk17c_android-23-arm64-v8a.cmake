set(CMAKE_SYSTEM_VERSION 23 CACHE TYPE INTERNAL FORCE)
set(CMAKE_ANDROID_NDK $ENV{ANDROID_NDK})
set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)
set(CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION clang)
set(CMAKE_ANDROID_API 23)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_ANDROID_STL_TYPE gnustl_static)

message("CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER}")
SET(CMAKE_PLATFORM_NAME android_ndk17c_android-23-arm64-v8a)

