#!/bin/bash

set -x 
set -e

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
HOST_DIR=$SCRIPT_DIR/../../host
COMMON_LIB_DIR=$SCRIPT_DIR/../../../../
BUILD_COMMON_DIR=$SCRIPT_DIR/../common
BIN_DIR=$SCRIPT_DIR/bin

# Usage:
function func_usage
{
	echo "Usage of $0:"
	echo -e "\t-h : usage help"
	echo -e "\t-c (--clean) : clean build"
	echo -e "\t-v (--verbose) : verbose build"
	echo -e "\t-p (--platform) : build toolchain (linux_gcc-4.8_x86, ...)"
	echo -e "\t-m (--connection) : multi_usb, uart_mipi...)"
}
function func_parse_test_params
{	
	while [[ $# > 0 ]]
	do
	key="$1"
	shift

	case $key in		
		-h|--help)
		param_help=1
		;;
		-c|--clean)
		param_clean=-c
		;;
		-v|--verbose)
		param_verbose=-v
		;;
		-m|--connection)
		param_connect="$1"
		;;
		-p|--platform)
		param_platform="$1"
		shift		
		;;
		*)
				# unknown option
		;;
	esac
	done
}

func_parse_test_params "$@"

if [ ! -z "$param_help" ]; then
    func_usage
    exit 10
fi

[ -z "$param_platform" ] && param_platform=linux_gcc-9.4_x86_64
target_os=`echo $param_platform | awk -F '[_]' '{print $1}'`
target_toolchain=`echo $param_platform | awk -F '[_]' '{print $2}'`
target_abi=`echo $param_platform | awk -F '[_]' '{print $3}'`

target_build_system=cmake
[ ! -z `echo $target_os | grep "tizen"` ] && target_build_system=make
[ ! -z `echo $target_toolchain | grep "raspberry"` ] && target_build_system=make
[ ! -z `echo $target_os | grep "linux"` ] && [ ! -z `echo $target_abi | grep "armv7a"` ] && target_build_system=make
[ ! -z `echo $target_os | grep "linux"` ] && [ ! -z `echo $target_abi | grep "armv7"` ] && target_build_system=make
#[ ! -z `echo $target_os | grep "linux"` ] && [ ! -z `echo $target_abi | grep "armv8"` ] && target_build_system=make
[ ! -z `echo $target_os | grep "android"` ] && target_build_system=android

#if [ -z `echo $target_abi | grep "android-21"` ];then
#export ANDROID_NDK=/home/cmadmin/android/android-ndk-r17c
#export CROSS_HOST=/aarch64-linux-android-4.9/prebuilt/linux-x86_64/lib/gcc/aarch64-linux-android/4.9.x
#export PATH=/home/cmadmin/android/android-ndk-r17c/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64/bin/:/home/cmadmin/android/android-ndk-r17c/toolchains/llvm/prebuilt/linux-x86_64/bin:/home/cmadmin/android/android-ndk-r17c/:/home/cmadmin/android/android-ndk-r17c/ndk-build:$PATH
#fi


WORKING_DIR=${PWD}
mkdir -p $BIN_DIR/$param_platform
[ ! -z "$param_clean" ] && rm -rf $BIN_DIR/$param_platform/*

echo "Building libusb"
cd $COMMON_LIB_DIR/inu_usb_lib/driver/libusb/
bash ./build.sh -p $param_platform $param_verbose $param_clean
find bin/$param_platform/ -maxdepth 1 -type f  -name "libusb*" -exec cp -av {} $BIN_DIR/$param_platform \;

if [ "$target_build_system" == "make" ];then
	echo "Building inu_host_lib"
	cd $HOST_DIR/inu_host_lib/prj/make/
	bash -x ./build.sh  -p $param_platform $param_verbose $param_clean		
	find build/ -maxdepth 1 -type f  -name "*inu*" -exec cp -av {} $BIN_DIR/$param_platform \;
	
elif [ "$target_build_system" == "cmake" ];then
	
	toolchain_file="$BUILD_COMMON_DIR/cmake/toolchain/$param_platform.cmake"
	if [ ! -e "$toolchain_file" ]; then
		echo "Can't find  toolchain file: $toolchain_file"
		exit 20
	else
		build_params="$build_params -t $toolchain_file"	
	fi

	echo "Building inu_usb_lib"
	cd $COMMON_LIB_DIR/inu_usb_lib/prj/cmake/
	./build.sh $param_verbose $param_clean $build_params
	find build/ -maxdepth 1 -type f -name "*inu*" -exec cp -av {} $BIN_DIR/$param_platform \;

	echo "Building inu_storage_lib"
	cd $COMMON_LIB_DIR/inu_storage_lib/prj/cmake/
	./build.sh $param_verbose $param_clean $build_params
	find build/ -maxdepth 1 -type f -name "*inu*" -exec cp -av {} $BIN_DIR/$param_platform \;

	echo "Building inu_host_lib"
	cd $HOST_DIR/inu_host_lib/prj/cmake/
	bash ./build.sh  $param_verbose $param_clean $build_params
	find build/ -maxdepth 1 -type f  -name "*inu*" -exec cp -av {} $BIN_DIR/$param_platform \;
else
	echo "Building inu_usb_lib"
	cd $COMMON_LIB_DIR/inu_usb_lib/prj/android/	 
	bash ./build.sh -p $param_platform $param_verbose $param_clean -m $param_connect
	find bin/$param_platform/ -maxdepth 1 -type f  -name "*" -exec cp -av {} $BIN_DIR/$param_platform \;
	
	echo "Building inu_storage_lib"
	cd $COMMON_LIB_DIR/inu_storage_lib/prj/android/	 
	bash ./build.sh -p $param_platform $param_verbose $param_clean -m $param_connect
	find bin/$param_platform/ -maxdepth 1 -type f  -name "*" -exec cp -av {} $BIN_DIR/$param_platform \;
	echo "Building inu_host_lib"
	cd $HOST_DIR/inu_host_lib/prj/android
	bash ./build.sh -p $param_platform $param_verbose $param_clean -m $param_connect	
	find bin/$param_platform/ -maxdepth 1 -type f  -name "*" -exec cp -av {} $BIN_DIR/$param_platform \;
fi
	
cd $WORKING_DIR
