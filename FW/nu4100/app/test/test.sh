#!/bin/bash
set -x
set -e
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
HOST_BIN_DIR=$SCRIPT_DIR/../build/host/bin

# Usage:
function func_usage
{
	echo "Usage of $0:"
	echo -e "\t-h : usage help"
	echo -e "\t-c (--clean) : clean build"
	echo -e "\t-v (--verbose) : verbose build"
	echo -e "\t-d (--bindir) : bin directory with test executables"
	echo -e "\t-p (--platform) : build toolchain (linux_gcc-4.8_x86, ...)"
	echo -e "\t-t (--test) : test to run (boot, nightly, ...)"
	echo -e "\t-b (--boot) : path to boot directory "
	echo -e "\t-i (--config) : path to config.ireg file"
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
		param_clean=1		
		;;
		-v|--verbose)
		param_verbose=1		
		;;
		-d|--bindir)
		param_bindir="$1"
		shift
		;;
		-p|--platform)
		param_platform="$1"
		shift
		;;
		-t|--test)
		param_test="$1"
		shift
		;;
		-b|--boot)
		param_boot="$1"
		shift
		;;
		-i|--ip)
		param_ip="$1"
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

if [ ! -z "$param_verbose" ]; then
    build_params="$build_params -v"
fi

if [ ! -z "$param_clean" ]; then
    build_params="$build_params -c"	
fi

[ -z "$param_bindir" ] && param_bindir=$HOST_BIN_DIR
[ -z "$param_platform" ] && param_platform=linux_gcc-4.8_x86
[ ! -z "$param_ip" ] && param_ip="-i $param_ip"
# run test 
[ -e "$test/$param_test.sh" ] && test/$param_test.sh -d $param_bindir -b $param_boot -p  $param_platform $param_ip  $build_params