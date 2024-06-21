#!/bin/bash
set -e
set -x
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
PROJECT_DIR=$SCRIPT_DIR/../../target/ceva/prj/make

# Usage:
function func_usage
{
	echo "Usage of $0:"
	echo -e "\t-h : usage help"
	echo -e "\t-c (--clean) : clean build"
	echo -e "\t-v (--verbose) : verbose build"
	echo -e "\t-p (--platform) : build platform (boot0, custom, ...)"
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
		param_clean=clean
		;;
		-v|--verbose)
		param_verbose=1		
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
WORKING_DIR=${PWD}
cd $PROJECT_DIR

# set environment for CEVA tools
export CEVAXM4_ROOT="/opt/inuitive/toolchain/ceva/cevaxm4"
source ${CEVAXM4_ROOT}/CEVA-XM4_set.sh
chmod +x ceva_post_build.sh

echo "Building ceva RELREASE"
make $param_clean build
mkdir -p $SCRIPT_DIR/bin/$param_platform
echo "Copy inu_app.cva"
cp -av CEVA_RELEASE/inu_app.cva $SCRIPT_DIR/bin/$param_platform
cp -av CEVA_RELEASE/libinu_ceva.lib $SCRIPT_DIR/bin/$param_platform


cd $WORKING_DIR
