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
	echo -e "\t-t (--toolchain) : path to toolchain file (e.g ../../../build/cmake/toolchain/linux_x86.cmake)"
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
		-t|--toolchain)
		param_toolchain="$1"
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
    make_param="$make_param VERBOSE=1"
fi

if [ ! -z "$param_toolchain" ]; then
	cmake_toolchain_define="-DCMAKE_TOOLCHAIN_FILE=$param_toolchain"
fi

mkdir -p $SCRIPT_DIR/build
[ ! -z "$param_clean" ] &&  rm -rf $SCRIPT_DIR/build/*
cd $SCRIPT_DIR/build
cmake $cmake_toolchain_define ..
make $make_param
cd $SCRIPT_DIR

