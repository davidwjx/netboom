#!/bin/sh
#
# load.sh
#
# This script loads the UMDevXS Driver and creates the communication
# point for the application (using the proxy).
#

##############################################################################
# Copyright (c) 2009-2018 INSIDE Secure B.V. All Rights Reserved.
#
# This confidential and proprietary software may be used only as authorized
# by a licensing agreement from INSIDE Secure.
#
# The entire notice above must be reproduced on all authorized copies that
# may only be made to the extent permitted by a licensing agreement from
# INSIDE Secure.
#
# For more information or support, please go to our online support system at
# https://customersupport.insidesecure.com.
# In case you do not have an account for this system, please send an e-mail
# to ESSEmbeddedHW-Support@insidesecure.com.
##############################################################################

dmesg -c > /dev/null 2>&1
if [ $? -ne 0 ];
then
    echo "Not enough rights. Try sudo."
    exit
fi

# check if already loaded (0) or not yet loaded (1)
is_loaded=$(lsmod | grep umdevxs > /dev/null; echo $?)
if [ $is_loaded -eq 0 ];
then
    echo "Already loaded; use unload.sh first";
    exit
fi

# move to the script base directory
MY_DIR=$(dirname $0);
cd $MY_DIR

if [ "x$1" = "x" ];
then
    echo "Missing argument: build configuration (like pci, mem, sim)"
    exit
fi

# check that the driver was built successfully
build=umdevxs_k-$1.ko

if [ ! -f "$build" ]; then
    echo "Cannot find driver ($build)";
    exit
fi

# load the driver
insmod $build
RES=$?
if [ $RES -ne 0 ];
then
    echo "insmod failed ($RES)";
    exit
fi

# grab the major number for the character device
major_nr=`awk "\\$2==\"umdevxs_c\" {print \\$1}" /proc/devices`
#echo "Major nr: $major_nr";

# make the communication pointer
mknod /dev/umdevxs_c c ${major_nr} 0

# make accessible to application
chmod 666 /dev/umdevxs_c

# print and clear start-up messages
dmesg -c

# end of file load.sh
