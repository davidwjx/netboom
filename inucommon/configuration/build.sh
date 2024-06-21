#!/bin/bash -x
if [ "$#" -lt "1" ]; then
    echo ""
    echo "Syntax:  build.sh <PLATFORM>"
    echo "Example: build.sh x64"
    echo "Exit..."
    exit 1
fi

PLATFORM=$1


export CONAN_LOGIN_USERNAME_ARTIFACT_REMOTE=${CONAN_CRED_USR}
export CONAN_PASSWORD_ARTIFACT_REMOTE=${CONAN_CRED_PSW}
export INUDEV_OPEN_SOURCE=true
export FW_ROOT=/tmp/jenkins/workspace/sw/branches/Versions/${BRANCH_NAME}/FW
if [ "$PLATFORM" = "linux_gcc-7.4_x86_64" ]; then
    export JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64/
    toolchainFile=../cmake/linux_gcc-7.4_x86_64.cmake
elif [ "$PLATFORM" = "linux_gcc-7.3_armv8" ]; then
    export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64/
    export LINUX_ARMV8_BIN_PATH=/home/cmadmin/armv8/gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu/bin
    export LINUX_ARMV8_PREFIX=aarch64-linux-gnu-
    toolchainFile=../cmake/linux_gcc-7.3_armv8.cmake
elif [ "$PLATFORM" = "linux_gcc-5.4_armv8" ]; then
    export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64/
    export LINUX_ARMV8_BIN_PATH=/home/cmadmin/armv8/gcc-linaro-5.4.1-2017.01-x86_64_aarch64-linux-gnu/bin
    export LINUX_ARMV8_PREFIX=aarch64-linux-gnu-
    toolchainFile=../cmake/linux_gcc-5.4_armv8.cmake
elif [ "$PLATFORM" = "linux_gcc-5.4_x86_64" ]; then
    export JAVA_HOME=/usr/lib/jvm/java-8-openjdk-amd64/
    toolchainFile=../cmake/linux_gcc-5.4_x86_64.cmake
elif [ "$PLATFORM" = "android_ndk23b_android-27-arm64-v8a" ]; then
    export ANDROID_NDK=/home/cmadmin/android/android-ndk-r23b
    export CROSS_HOST=toolchains/llvm/prebuilt/linux-x86_64
    export PATH=$ANDROID_NDK/prebuilt/linux-x86_64:$PATH
    export ANDROID_API_LEVEL=27
    toolchainFile=../cmake/android-ndk23b-api27.cmake
fi

cd src || exit 1
/home/cmadmin/anaconda3/bin/cmake . -G Ninja -DCMAKE_MAKE_PROGRAM=ninja -DUSE_CONAN=True -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="${toolchainFile}" -DCONAN_COMMAND=/home/cmadmin/anaconda3/bin/conan -Bout/build/${PLATFORM}
cd "out/build/${PLATFORM}" || exit 1
ninja

exit 0