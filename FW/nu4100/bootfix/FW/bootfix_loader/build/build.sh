#!/bin/bash
set -e
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
BOOTFLASH_DIR=$SCRIPT_DIR/../


# Usage:
function func_usage
{
	echo "Usage of $0:"
	echo -e "\t-h : usage help, no parameters"
	echo -e "\t-c (--clean) : clean build"	
	echo -e "\t-b (--board) : (c0,c1)"
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
		-b|--board)
		param_board="$1"
		shift
		;;
		-c|--clean)
		param_clean=-c
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

WORKING_DIR=${PWD}

echo "Building bootfix loader"
mkdir -p $BOOTFLASH_DIR/bin
cd $BOOTFLASH_DIR/src

param_board_name=nu4000c1

RELEASE=1 CHIP_ID=$param_board_name make -j20 clean
RELEASE=1 CHIP_ID=$param_board_name make -j20 bootrom_include
RELEASE=1 CHIP_ID=$param_board_name make -j20 all
RELEASE=1 CHIP_ID=$param_board_name make -j20 binary
cp -av bootfix_loader_*.bin $BOOTFLASH_DIR/bin

cd $WORKING_DIR
