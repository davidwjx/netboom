#!/bin/bash
set -e
#set -x
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
PROJECT_DIR=$SCRIPT_DIR/../../target/gp/prj/make
CHIP_REVISION_ID=4100

# Usage:
function func_usage
{
	echo "Usage of $0:"
	echo -e "\t-h : usage help"
	echo -e "\t-c (--clean) : clean build"
	echo -e "\t-v (--verbose) : verbose build"
	echo -e "\t-p (--platform) : build platform (boot0, custom, ...)"
	echo -e "\t-a (--adapter) : build for Varjo adapter hardware)"
	echo -e "\t-z (--zip) : copy inu_target.out to zip file (default zip file is nu$CHIP_REVISION_ID.zip. Otherwise, specify it)"
}

function print_time
{
	min=$((SECONDS/60))
	sec=$((SECONDS%60))
	echo -e "\e[33mCompilation time: $min:$(printf %02d $sec)\e[0m"
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
		param_clean=rebuild
		;;
		-v|--verbose)
		param_verbose=1
		;;
		-a|--adapterhw)
		param_adapterhw=1		
		;;
		-m|--connection)
		param_connect="$1"
		;;
		-z|--zip)
		add_file_to_zip=1
		add_file_to_zip_name="$1"
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

[ -z "$param_platform" ] && param_platform="boot0"
[ -z "$add_file_to_zip_name" ] && add_file_to_zip_name="nu$CHIP_REVISION_ID.zip"

WORKING_DIR=${PWD}
cd $PROJECT_DIR

echo "Building inu_target"
if [  -z "$param_adapterhw" ]; then
	echo "Building for S1 hardware"
	make $param_clean CONNECTION=$param_connect -j20
else
	echo "Building for adapter hardware"
	make $param_clean CONNECTION=$param_connect Adapter_Hardware=1 -j20
fi

echo "Copy inu_target"
mkdir -p $SCRIPT_DIR/bin/$param_platform
cp -av inu_target.* $SCRIPT_DIR/bin/$param_platform
cp -av libinu_target.* $SCRIPT_DIR/bin/$param_platform

cd $WORKING_DIR
