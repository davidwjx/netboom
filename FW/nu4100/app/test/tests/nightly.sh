#!/bin/bash
set -x
set -e
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
HOST_BIN_DIR=$SCRIPT_DIR/../../build/host/bin
UTILS_DIR=$SCRIPT_DIR/../utils
CONFIG_DIR=$SCRIPT_DIR/../configurations/B0_HD
UTILS_USB_CONNECT=$UTILS_DIR/usb_connect/bin/Usb_connect.exe
TMP_DIR=`mktemp -d`
# Usage:
function func_usage
{
	echo "Usage of $0:"
	echo -e "\t-h : usage help"
	echo -e "\t-d (--bindir) : bin directory with test executables"
	echo -e "\t-p (--platform) : build toolchain (linux_gcc-4.8_x86, ...)"	
	echo -e "\t-b (--boot) : path to boot directory "	
	echo -e "\t-o (--port) : Adam port address of the unit"	
	echo -e "\t-i (--ip) : ip address of the unit"	
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
		-d|--bindir)
		param_bindir="$1"
		shift
		;;
		-p|--platform)
		param_platform="$1"
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
		-o|--port)
		param_port="$1"
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

[ -z "$param_bindir" ] && param_bindir=$HOST_BIN_DIR
[ -z "$param_platform" ] && param_platform=linux_gcc-4.8_x86
[ -z "$param_port" ] && param_port=0

# Reconnect the sensor
wine  $UTILS_USB_CONNECT reset $param_port $param_ip >/dev/null 2>&1
lsusb
# Run the test
mkdir -p $TMP_DIR/bin
mkdir -p $TMP_DIR/boot
cp -av $param_bindir/$param_platform/* $TMP_DIR/bin
cp -av $param_boot/* $TMP_DIR/boot
chmod +x $TMP_DIR/bin/inu_usb_wait
LD_LIBRARY_PATH=$TMP_DIR/bin $TMP_DIR/bin/inu_host_test -T V -R H -F 20 -D 60 -B $TMP_DIR/boot -f $CONFIG_DIR

# Reconnect the sensor
wine  $UTILS_USB_CONNECT reset $param_port $param_ip >/dev/null 2>&1
#run depth test for 1 min
LD_LIBRARY_PATH=$TMP_DIR/bin $TMP_DIR/bin/inu_host_test -T D -R H -F 20 -D 60 -B $TMP_DIR/boot -f $CONFIG_DIR

# Disable USB device
wine  $UTILS_USB_CONNECT off $param_port $param_ip >/dev/null 2>&1

