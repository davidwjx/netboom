#!/bin/sh
#
# unload.sh
#
# This script unloads the UMDevXS Driver and deletes the communication
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

# print and clear messages
dmesg -c 2>/dev/null

# check that we run with root rights
# (this assumes dmesg -c is root-only)
if [ $? -ne 0 ]; then
    echo "Not enough rights. Try sudo."
    exit;
fi

# remove the communication point
rm -f /dev/umdevxs_c

# unload the driver
rmmod umdevxs_k

# print and clear the shutdown-messages
dmesg -c

# end of script unload.sh
