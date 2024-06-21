##
## File: configure.ac
##
## Source file for building configure with automake+autoconf
##

##############################################################################
## Copyright (c) 2009-2018 INSIDE Secure B.V. All Rights Reserved.
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
##############################################################################

AC_INIT([SafeZone-SecureBoot], [3.9])

AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([foreign no-define])

AC_MSG_CHECKING([available SafeZone features])
SafeZone_feat="`find ${TOP_SRC_DIR}meta -maxdepth 1 -name "*.features" -exec cat {} ';' | cut -f 2 -d ' '`"
SafeZone_feat_dup="`find ${TOP_SRC_DIR}meta -maxdepth 1 -name "*.features" -exec cat {} ';' | cut -f 2 -d ' ' | sort | uniq -d`"
if test "X$SafeZone_feat_dup" != "X"; then
    SafeZone_feat_1=`echo $SafeZone_feat`
    AC_MSG_RESULT([$SafeZone_feat_1])
    AC_MSG_ERROR([Conflicting packages available.])
fi
if test "X$SafeZone_feat" == "X"; then
    AC_MSG_RESULT([none available])
    AC_MSG_ERROR([Please, untar more SafeZone packages.])
fi
SafeZone_feat_1=`echo $SafeZone_feat`
AC_MSG_RESULT([$SafeZone_feat_1])

# Scanning for subdirectories
if test "X$SAFEZONE_SUBDIRS" == "X"; then
    safezone_subdirs="`find ${TOP_SRC_DIR}meta -maxdepth 1 -name "*.subdir" -exec cat {} ';' | LC_ALL=C sort`"
    SAFEZONE_SUBDIRS=`echo $safezone_subdirs`
fi

# Build rules for subdirectories
find ${TOP_SRC_DIR}meta -maxdepth 1 -name "*.mk" -exec cat {} ';' >meta/combined.rules

# Build envfiles
for envfile in `find ${TOP_SRC_DIR}meta -maxdepth 1 -name "*.env" | LC_ALL=C sort`
do
    # Set ac_pwd if it is not set, older autoconf versions need it
    if test "X$ac_pwd" == "X"; then
      ac_pwd=`pwd`
    fi

    echo "Executing $envfile"
    . $envfile
done

# Substitute subdirectories and rules
AC_SUBST([SAFEZONE_SUBDIRS])
# Notice: Workaround for multiline variable
AC_SUBST_FILE([SAFEZONE_RULES])
SAFEZONE_RULES=$ac_pwd/meta/combined.rules

# Configure subdirectories
AC_CONFIG_SUBDIRS([$SAFEZONE_SUBDIRS])

AC_CONFIG_FILES(Makefile)

AC_OUTPUT

# end of file configure.ac
