#!/bin/bash
set -e
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
BOOTFIX_DIR=$SCRIPT_DIR/../
BOOTFIX_LOADER_DIR=$SCRIPT_DIR/../../bootfix_loader  
  

MAJOR_VERSION="$(grep -m 1 MAJOR_VERSION $BOOTFIX_DIR/build/version/version.h |tr -d '\r'| awk '{print $3}')"
MINOR_VERSION="$(grep -m 1 MINOR_VERSION $BOOTFIX_DIR/build/version/version.h |tr -d '\r'| awk '{print $3}')"
BUILD_VERSION="$(grep -m 1 BUILD_VERSION $BOOTFIX_DIR/build/version/version.h |tr -d '\r'| awk '{print $3}')"
SUB_BUILD_VERSION="$(grep -m 1 SUB_BUILD_VERSION $BOOTFIX_DIR/build/version/version.h |tr -d '\r'| awk '{print $3}')"

# Usage:
function func_usage
{
	echo "Usage of $0:"
	echo -e "\t-h : usage help, no parameters"
	echo -e "\t-c (--clean) : clean build"
	echo -e "\t-p (--platform) : build platform (boot0, ...)"
	echo -e "\t-q (--svnuse) : svn for version"
	echo -e "\t-s (--slow) : build slow spi"
	echo -e "\t-b (--board) : (c0,c1)"
	echo -e "\t-u (--usb) : usb type (2, 3)"
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
		-p|--platform)
		param_platform="$1"
		shift
		;;
		-b|--board)
		param_board="$1"
		shift
		;;
		-u|--usb)
		param_usb="$1"
		shift
		;;
		-q|--svnuse)
		param_svn=1
		;;
		-o|--appoffset)
		param_app_offset="$1"
		shift
		;;
		-s|--slowspi)
		param_slow=1
		;;
		-h|--halfpll)
		param_200mhz_pll="$1"
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

param_board_name=nu4000c1
param_board=nu4100
[ -z "$param_platform" ] &&  param_platform="boot0"
  
BOOTID="$(echo $param_platform | sed 's/boot//g')"

if [ ! -z "$param_svn" ]; then
export SVN_USE=$param_svn
fi
WORKING_DIR=${PWD}
if [ ! -z "$param_slow" ]; then
    echo "Building bootfix loader slow spi"
    cd $BOOTFIX_LOADER_DIR/src/
    RELEASE=1 CHIP_ID=$param_board_name SLOW_SPI_BUILD=1 make -j20 EXTCDEFINES="-DBOOT_$BOOTID" clean
    RELEASE=1 CHIP_ID=$param_board_name SLOW_SPI_BUILD=1 make -j20 EXTCDEFINES="-DBOOT_$BOOTID" bootrom_include
    RELEASE=1 CHIP_ID=$param_board_name SLOW_SPI_BUILD=1 make -j20 EXTCDEFINES="-DBOOT_$BOOTID" all
    RELEASE=1 CHIP_ID=$param_board_name SLOW_SPI_BUILD=1 make -j20 EXTCDEFINES="-DBOOT_$BOOTID" binary
    
    echo "Building bootfix slow spi"
    mkdir -p $BOOTFIX_DIR/bin
    cd $BOOTFIX_DIR/src
    RELEASE=1 SLOW_SPI_BUILD=1 CHIP_ID=$param_board_name make -j20 EXTCDEFINES="-DBOOT_$BOOTID" clean
    RELEASE=1 SLOW_SPI_BUILD=1 CHIP_ID=$param_board_name make -j20 EXTCDEFINES="-DBOOT_$BOOTID" all
    RELEASE=1 SLOW_SPI_BUILD=1 CHIP_ID=$param_board_name make -j20 EXTCDEFINES="-DBOOT_$BOOTID" binary
    RELEASE=1 SLOW_SPI_BUILD=1 CHIP_ID=$param_board_name make -j20 EXTCDEFINES="-DBOOT_$BOOTID" pack
                               
    cp -av flash_bootfix.bin $BOOTFIX_DIR/bin/bootfix-slowspi-$MAJOR_VERSION"_"$MINOR_VERSION"_"$BUILD_VERSION"_"$SUB_BUILD_VERSION.bin
fi

echo "Building bootfix loader"
cd $BOOTFIX_LOADER_DIR/src/
RELEASE=1 CHIP_ID=$param_board_name make -j20 EXTCDEFINES="-DBOOT_$BOOTID" clean
RELEASE=1 CHIP_ID=$param_board_name make -j20 EXTCDEFINES="-DBOOT_$BOOTID" bootrom_include
RELEASE=1 CHIP_ID=$param_board_name make -j20 EXTCDEFINES="-DBOOT_$BOOTID" all
RELEASE=1 CHIP_ID=$param_board_name make -j20 EXTCDEFINES="-DBOOT_$BOOTID" binary
echo "Building bootfix"
mkdir -p $BOOTFIX_DIR/bin
cd $BOOTFIX_DIR/src

RELEASE=1 CHIP_ID=$param_board_name USB_TYPE=$param_usb make -j20 EXTCDEFINES="-DBOOT_$BOOTID" clean
RELEASE=1 CHIP_ID=$param_board_name USB_TYPE=$param_usb make -j20 EXTCDEFINES="-DBOOT_$BOOTID" all
RELEASE=1 CHIP_ID=$param_board_name USB_TYPE=$param_usb make -j20 EXTCDEFINES="-DBOOT_$BOOTID" binary
RELEASE=1 CHIP_ID=$param_board_name USB_TYPE=$param_usb make -j20 EXTCDEFINES="-DBOOT_$BOOTID" pack

if [ "$param_usb" == "2" ]
then
	cp -av flash_bootfix.bin $BOOTFIX_DIR/bin/bootfix-$param_board"_usb"$param_usb"-"$MAJOR_VERSION"_"$MINOR_VERSION"_"$BUILD_VERSION"_"$SUB_BUILD_VERSION.bin
else
	cp -av flash_bootfix.bin $BOOTFIX_DIR/bin/bootfix-$param_board"-"$MAJOR_VERSION"_"$MINOR_VERSION"_"$BUILD_VERSION"_"$SUB_BUILD_VERSION.bin
	# Copy bootfix.bin and bootfix_loader_$param_board.bin to /bin/bootfix_components/ folder for signing.
	mkdir -p $BOOTFIX_DIR/bin/bootfix_components
	cp -av bootfix.bin $BOOTFIX_DIR/bin/bootfix_components/bootfix_app-$param_board"-"$MAJOR_VERSION"_"$MINOR_VERSION"_"$BUILD_VERSION"_"$SUB_BUILD_VERSION.bin
	cd $BOOTFIX_LOADER_DIR/src/
	cp -av bootfix_loader_$param_board.bin $BOOTFIX_DIR/bin/bootfix_components/bootfix_loader-$param_board"-"$MAJOR_VERSION"_"$MINOR_VERSION"_"$BUILD_VERSION"_"$SUB_BUILD_VERSION.bin
fi

cd $WORKING_DIR
