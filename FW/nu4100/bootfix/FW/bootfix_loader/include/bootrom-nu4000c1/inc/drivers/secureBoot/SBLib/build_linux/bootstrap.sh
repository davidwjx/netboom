#!/bin/sh
##
## File: bootstrap.sh
##
## Generates configure for configuring this package with automake+autoconf
##
## Copyright (c) 2008-2018 INSIDE Secure B.V. All Rights Reserved.
##
## This confidential and proprietary software may be used only as authorized
## by a licensing agreement from INSIDE Secure.
##
## The entire notice above must be reproduced on all authorized copies that
## may only be made to the extent permitted by a licensing agreement from
## INSIDE Secure.
##
## For more information or support, please go to our online support system at
## https://customersupport.insidesecure.com.
## In case you do not have an account for this system, please send an e-mail
## to ESSEmbeddedHW-Support@insidesecure.com.
##

# Omit warnings
aclocal -I ../../m4 2>/dev/null \
  && automake --add-missing --copy 2>/dev/null \
  && autoconf 2>/dev/null

if test "$?" != "0";
then
  # Error occured in processing, then output the error
  aclocal -I ../../m4 \
    && automake --add-missing --copy \
    && autoconf
  exit $?
fi

# end of file
