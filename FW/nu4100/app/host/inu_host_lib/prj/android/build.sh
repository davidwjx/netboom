#!/bin/bash
set -e
set -x
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"

# Usage:
function func_usage
{
	echo "Usage of $0:"
	echo -e "\t-h : usage help"
	echo -e "\t-c (--clean) : clean build"
	echo -e "\t-v (--verbose) : verbose build"
	echo -e "\t-p (--platform) : build platform (android_ndk10e_armeabi-v7a)"
	echo -e "\t-m (--connection) : connection)"
}
function func_parse_test_params
{	
	while [[ $# > 0 ]]
	do
	key="$1"
	shift

	case $key in
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
		-m|--connection)
		param_connect="$1"
		;;
		-h|--help)
		param_help=1
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

pushd . > /dev/null

[ -z "$param_platform" ] && param_platform=android_ndk10e_android-17-armeabi-v7a
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

cd $SCRIPT_DIR/jni
ndk-build NDK_TARGET_PLATFORM=$target_platform NDK_TARGET_ARCH_ABI=$target_arch_abi BUILD_PLATFORM=$param_platform $param_clean $param_verbose CONNECTION=$param_connect 
mkdir -p $SCRIPT_DIR/bin/$param_platform
cp -av $SCRIPT_DIR/libs/$target_arch_abi/* $SCRIPT_DIR/bin/$param_platform

popd > /dev/null
