#!/bin/bash -x

########################################################################################
# Name:		~cmadmin/scripts/setpermission.sh
# Purpose:	Run SetPermissionRT in root-owned directory. Called by *_packaging_*.sh
# Run by:	cmadmin@UB05
# By:		Yariv Sheizaf
# Date:		26-Oct-2015
# Arguments:
#		1 - Current location - "pwd"
########################################################################################

if [ "$#" != "1" ]
then
	echo ""
	echo "Syntax:  setpermission.sh <pwd>"
	echo "Exit..."
	exit 1
fi


PWDD=$1

rm -f /usr/local/forset/InuService
cp $PWDD/InuService /usr/local/forset
cd /usr/local/forset
$PWDD/SetPermissionRT
tar cvf iservice.tar InuService
cd $PWDD

exit 0
