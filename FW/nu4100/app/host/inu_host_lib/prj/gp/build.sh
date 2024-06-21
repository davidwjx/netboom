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
	echo -e "\t-p (--platform) : build platform (linux_gcc-4.8_x86)"
}
function func_parse_test_params
{	
	while [[ $# > 0 ]]
	do
	key="$1"
	shift

	case $key in
		-c|--clean)
		param_clean=1		
		;;
		-v|--verbose)
		param_verbose=1		
		;;
		-p|--platform)
		param_platform="$1"
		shift
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

if [ ! -z "$param_verbose" ]; then
    make_param="$make_param AT="
fi

[ -z "$param_platform" ] && param_platform=linux_gcc-4.8_x86

platform_env_name=`echo $param_platform | awk '{print toupper($0)}' | tr -d . | tr -d -`
platform_defines=""
platform_bin_path=${platform_env_name}_BIN_PATH
platform_lib_path=${platform_env_name}_LIB_PATH
platform_include_path=${platform_env_name}_INCLUDE_PATH
platform_bin_prefix=${platform_env_name}_PREFIX
if [ ! -z "${!platform_bin_path}" ]; then
	export CROSS_COMPILE=${!platform_bin_path}/${!platform_bin_prefix}
	export CROSS_COMPILE_LIB=${!platform_lib_path}
	export CROSS_COMPILE_INCLUDE=${!platform_include_path}
	export PLATFORM_NAME=$param_platform
fi

if [ ! -z "$param_clean" ]; then
    make clean
	rm -rf build/*
fi

make $make_param
mkdir -p build
cp -av libinu_host_lib.so build/libinu_host_lib.so
cp -av libinu_host_lib.a build/libinu_host_lib.a
cp -av inu_host_test build/inu_host_test

cd $SCRIPT_DIR

