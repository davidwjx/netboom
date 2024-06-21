#!/bin/bash
set -x
set -e
SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
HOST_BIN_DIR=$SCRIPT_DIR/../../build/host/bin
UTILS_DIR=$SCRIPT_DIR/../utils
CONFIG_DIR=$HOME/configurations/A0_HD
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
	echo -e "\t-r (--raspberry) : rasberry name/ip"	
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
		-r|--raspberry)
		param_raspberry="$1"
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
[ -z "$param_raspberry" ] && param_raspberry=pi01

# Reconnect the sensor
wine  $UTILS_USB_CONNECT reset $param_port $param_ip >/dev/null 2>&1
# Run the test via SSH on Raspberry
ssh $param_raspberry "sudo lsusb"
ssh $param_raspberry "mkdir -p $HOME/fw_bin && rm -rf  $HOME/fw_bin/*"
ssh $param_raspberry "mkdir -p $HOME/fw_boot && rm -rf  $HOME/fw_boot/*"
scp -r $param_bindir/$param_platform/* $param_raspberry:$HOME/fw_bin
scp -r $param_boot/* $param_raspberry:$HOME/fw_boot
scp -r configurations $param_raspberry:$HOME/configurations
#ssh $param_raspberry "sudo chmod +x $HOME/fw_bin/inu_usb_wait"
#ssh $param_raspberry "sudo LD_LIBRARY_PATH=$HOME/fw_bin $HOME/fw_bin/inu_usb_wait 30 $HOME/fw_boot"
ssh $param_raspberry "sudo chmod +x $HOME/fw_bin/inu_host_test"
ssh $param_raspberry "sudo LD_LIBRARY_PATH=$HOME/fw_bin $HOME/fw_bin/inu_host_test -T V -R H -F 20 -D 20 -B $HOME/fw_boot -f $CONFIG_DIR"

# Disable USB device
wine  $UTILS_USB_CONNECT off $param_port $param_ip >/dev/null 2>&1
