#!/bin/bash
set -e
set -x
# Usage:
function func_usage
{
	echo "Usage of $0:"
	echo -e "\t-h : usage help"
	echo -e "\t-c (--clean) : clean build"
	echo -e "\t-v (--verbose) : verbose build"
	echo -e "\t-p (--platform) : build toolchain (x86, x86_64, armv7, raspberry)"
	echo -e "\t-s (--static) : build statically"
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
		param_clean="-B"
		;;
		-v|--verbose)
		param_verbose="V=1"
		;;
		-p|--platform)
		param_platform="$1"
		shift
		;;
		-s|--static)
		param_static="--enable-static"		
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

[ -z "$param_platform" ] && param_platform="linux_gcc-4.8_x86"

target_os=`echo $param_platform | awk -F '[_]' '{print $1}'`
target_toolchain=`echo $param_platform | awk -F '[_]' '{print $2}'`
target_abi=`echo $param_platform | awk -F '[_]' '{print $3}'`
if [ -z `echo $target_abi | grep android` ]; then
	target_platform="android-17"
	target_arch_abi=$target_abi
else
	target_platform=`echo $target_abi | awk -F '[-]' '{print $1"-"$2}'`
	target_platform_len=${#target_platform}
	target_arch_abi=${target_abi:target_platform_len+1}
fi

platform_env_name=`echo $param_platform | awk '{print toupper($0)}' | tr -d . | tr -d -`
platform_defines=""
platform_bin_path=${platform_env_name}_BIN_PATH
platform_bin_prefix=${platform_env_name}_PREFIX
if [ ! -z "${!platform_bin_path}" ]; then

	export CC=${!platform_bin_path}/${!platform_bin_prefix}gcc
 	platform_defines=--host=${!platform_bin_prefix}
	#remove trailing - from the prefix
	platform_defines=${platform_defines::-1}
fi

if [ "$param_platform" = "linux_gcc-4.8_x86" ] &&  [ `uname -m` = "x86_64" ]; then
	platform_defines="--build=i686-pc-linux-gnu CFLAGS=-m32 CXXFLAGS=-m32 LDFLAGS=-m32"
fi	

if [ "$param_platform" = "tizen_gcc-6.2_armv8" ] ; then
	platform_defines="--host=aarch64-tizen-linux-gnu CC=${!platform_bin_path}/${!platform_bin_prefix}gcc "
fi	

if [ "$param_platform" = "linux_armv8" ] ; then
    platform_defines="--host=aarch64-linux-gnu CC=${!platform_bin_path}/${!platform_bin_prefix}gcc "
fi	

if [ ! -z `echo $param_platform | grep "raspberry"` ]; then 
	platform_defines="$platform_defines CFLAGS=-DDISABLE_BULK_CONTINUATION CXXFLAGS=-DDISABLE_BULK_CONTINUATION CPPFLAGS=-DDISABLE_BULK_CONTINUATION"
	#	platform_defines='--host=arm-bcm2708hardfp-linux-gnueabi --build=x86_64-unknown-linux "CFLAGS=-DDISABLE_BULK_CONTINUATION" "CXXFLAGS=-DDISABLE_BULK_CONTINUATION"'
fi

#if [ ! "$(ls -A src)" ]; then
#    git clone git://git/nu3000/libusb src
#fi

if [ "$target_os" == "android" ];then
	cd src/android/jni
	#ndk-build NDK_APPLICATION_MK=./Application-$target_abi.mk $param_clean $param_verbose
	ndk-build NDK_TARGET_PLATFORM=$target_platform NDK_TARGET_ARCH_ABI=$target_arch_abi $param_clean $param_verbose 
	mkdir -p ../../../bin/$param_platform/
	[ ! -z "$param_clean" ] && rm -rf ../../../bin/$param_platform/* && rm -rf ../../../lib/*
	cp -aLv ../libs/$target_arch_abi/libusb* ../../../bin/$param_platform/
	#cp -aLv ../libs/$target_arch_abi/libusb* ../../../lib/
	
else
	cd src
	
	mkdir -p src/m4
	[ ! -e "Makefile" ] && autoreconf --install	
	autoreconf -f -i
	./configure $platform_defines --prefix=${PWD}/../ --disable-udev $param_static
	
	[ ! -z $param_clean ] && make clean
	make
	make install
	mkdir -p ../bin/$param_platform
	[ ! -z "$param_clean" ] && rm -rf ../bin/$param_platform/*
	cp -aLv ../lib/libusb-1.0.so "../bin/$param_platform"
	cp -aLv ../lib/libusb-1.0.so.0 "../bin/$param_platform"
	cp -aLv ../lib/libusb-1.0.a "../bin/$param_platform"	
fi

