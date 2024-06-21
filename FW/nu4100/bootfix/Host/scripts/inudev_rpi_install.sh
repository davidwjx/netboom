#!/bin/bash

########################################################################################
# Name:		~cmadmin/scripts/inudev_rpi_install.sh
# Purpose:	Installation of Raspberry PI InuDev tar.gz pacakge
# Run by:	cmadmin@UB05
# By:		Yariv Sheizaf
# Date:		08-Nov-2015
# Arguments:
#		1 - version number
########################################################################################

if [ "$#" != "1" ]
then
	echo ""
	echo "Syntax:  ./inudev_rpi_install.sh <version number>"
	echo "Example:  ./inudev_rpi_install.sh 2.02.17.91"
	echo "Exit..."
	exit 1
fi

VER_NUM=$1

INUDEV_FILEN="InuSW_"$VER_NUM"_f0_linux_raspberry_arm.tar.gz"

if [ ! -f $INUDEV_FILEN ]
then
	echo ""
	echo "Installation package file "$INUDEV_FILEN" does not exist. Exit..."
	echo ""
	exit 1
fi

PWDD=`pwd`
if [ "$PWDD" != "/usr/local" ]
then
	cp  $INUDEV_FILEN /usr/local
	cd /usr/local
fi

if [ -d Inuitive ]
then
	DOMAIN_N=`grep domain /etc/resolv.conf | awk '{print $2}'`

	if [ "$DOMAIN_N" = "il.inuitive-tech.com" ]
	then
		rm -rf Inuitive_old_installations
	else
		PREV_VER=`grep "define IAF_VERSION_STR" /usr/local/Inuitive/InuDev/include/Version.h | tail -1 | cut -d\" -f2`
		OLDINST_PREV_DIR=Inuitive_old_installations/$PREV_VER
		if [ -d $OLDINST_PREV_DIR ]
		then
			rm -rf $OLDINST_PREV_DIR 
			rm -rf Inuitive_old_installations/[1-9]*.*.*.*
		fi
	fi

	mkdir -p Inuitive_old_installations/$PREV_VER
	mv Inuitive Inuitive_old_installations/$PREV_VER
fi

cd /usr/local

if [ -f $INUDEV_FILEN ]
then
	tar xvfp $INUDEV_FILEN
	rm /usr/local/$INUDEV_FILEN
else
	echo ""
	echo "Installation failed. Exit..."
	echo ""
	exit 1
fi

if [ -d /usr/local/Inuitive/InuDev/bin ]
then
	cd /usr/local/Inuitive/InuDev/bin
	./SetPermissionRT
else
	echo ""
	echo "Installation failed. Exit..."
	echo ""
	exit 1
fi

exit $?


