#!/bin/bash
set -e

################################################################
# Name:        ~./concat_bootfix_files.sh
# Purpose:    concat bootfix signed files
# Run by:    cmadmin@UB05
# By:        Eli Guetta
# Date:        20-Dec-2022
# Arguments:
#        1 - full path to secured bootfix loader
#        2 - full path to secured bootfix
########################################################################################

 

if [ "$#" != "2" ]
then
    echo ""
    echo "Syntax:  ./concat_bootfix_files.sh <secured bootfix loader> <secured bootfix>"
    echo "Example: ./concat_bootfix_files.sh ../bin/bootfix_components/bootfix_loader-nu4100-4_1_1_25.bin ../bin/bootfix_components/bootfix_app-nu4100-4_1_1_25.bin"
    echo "Exit..."
    exit 1
fi

BOOTFIX_LOADER_BIN=$1
BOOTFIX_BIN=$2

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
BOOTFIX_DIR=$SCRIPT_DIR/../
BOOTFIX_LOADER_DIR=$SCRIPT_DIR/../../bootfix_loader  
  

MAJOR_VERSION="$(grep -m 1 MAJOR_VERSION $BOOTFIX_DIR/build/version/version.h |tr -d '\r'| awk '{print $3}')"
MINOR_VERSION="$(grep -m 1 MINOR_VERSION $BOOTFIX_DIR/build/version/version.h |tr -d '\r'| awk '{print $3}')"
BUILD_VERSION="$(grep -m 1 BUILD_VERSION $BOOTFIX_DIR/build/version/version.h |tr -d '\r'| awk '{print $3}')"
SUB_BUILD_VERSION="$(grep -m 1 SUB_BUILD_VERSION $BOOTFIX_DIR/build/version/version.h |tr -d '\r'| awk '{print $3}')"

param_board=nu4100

SECURED_BOOTFIX_FILE=secured_bootfix-$param_board"-"$MAJOR_VERSION"_"$MINOR_VERSION"_"$BUILD_VERSION"_"$SUB_BUILD_VERSION.bin


if [ -z $BOOTFIX_LOADER_BIN ]; then
	echo "bootfix_loader binary file is missing. Aborting."
	exit 1
fi

if [ -z $BOOTFIX_BIN ]; then
	echo "bootfix binary file is missing. Aborting."
	exit 2
fi

BOOTFIX_SIZE=$(stat -c%s  $BOOTFIX_BIN)
BOOTFIX_LOADER_SIZE=$(stat -c%s  $BOOTFIX_LOADER_BIN)

printf "0: %.8x" $BOOTFIX_LOADER_SIZE | sed -e 's/0: \(..\)\(..\)\(..\)\(..\)/0: \4\3\2\1/' | xxd -r -g0 > tmpfile_size1
printf "0: %.8x" $BOOTFIX_SIZE | sed -e 's/0: \(..\)\(..\)\(..\)\(..\)/0: \4\3\2\1/' | xxd -r -g0 > tmpfile_size2
cat tmpfile_size1 $BOOTFIX_LOADER_BIN tmpfile_size2 $BOOTFIX_BIN > $BOOTFIX_DIR/bin/bootfix_components/$SECURED_BOOTFIX_FILE
rm tmpfile_size1 tmpfile_size2
