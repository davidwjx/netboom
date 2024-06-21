#!/bin/bash -x

###############################################################################################
# Name:		~cmadmin/scripts/fw_host_compilation.sh
# Purpose:	Full Inuitive software build script for FW host_build on all operating systems
# Run by:	cmadmin@UB02
# By:		Yariv Sheizaf
# Date:		14-Jun-2015
# Arguments:
#		1 - branch name
#		2 - Version number
###############################################################################################

if [ "$#" != "2" ]
then
	echo ""
	echo "Syntax:  fw_host_compilation.sh <branch name> <version number>"
	echo "Example: fw_host_compilation.sh InuSW_1_11_0 1.11.03"
	echo "Exit..."
	exit 1
fi

##################### General definitins

PROJ_BRANCH=$1
PROJ_NAME=`echo $PROJ_BRANCH | cut -d_ -f1`
BRANCH_NAME=`echo $PROJ_BRANCH | cut -d_ -f2-`

VERSION_NUM=$2

PROJ_VERSION=$PROJ_NAME"_"$VERSION_NUM

CUR_compilation_RDIR=`pwd`
source ~/.profile

INSTALLBUILDER_RDIR=/home/cmadmin/installbuilder-15.1.0

OUTPUT_TREE_MOUNT_POINT=/space/users/cmadmin/Versions

LINUX_VER_DIR=$OUTPUT_TREE_MOUNT_POINT/$PROJ_VERSION/Linux
if [ ! -d $LINUX_VER_DIR ]
then
	mkdir -p $LINUX_VER_DIR
fi

LINUX_NOT_STRIPPED_VER_DIR=$OUTPUT_TREE_MOUNT_POINT/$PROJ_VERSION/Linux_not_stripped
if [ ! -d $LINUX_NOT_STRIPPED_VER_DIR ]
then
	mkdir $LINUX_NOT_STRIPPED_VER_DIR
fi

##### Cleanup bin - products tree
if [ ! -d $CUR_compilation_RDIR/bin ]
then
	mkdir $CUR_compilation_RDIR/bin
fi

##################### MAIN LOOP PER Platform   

PLATFORMS_TO_compilation=$CUR_compilation_RDIR/Host/scripts/FWHost_Platforms.txt

for plt in `grep -v \# $PLATFORMS_TO_compilation`
do
	if [ -d $CUR_compilation_RDIR/bin/$plt ]
	then
		rm -rf $CUR_compilation_RDIR/bin/$plt
	fi
	mkdir $CUR_compilation_RDIR/bin/$plt

	echo "====================  FW START     ===================================="

	
	cd $CUR_compilation_RDIR/FW
	find . -name "*.sh" -print -exec chmod +x "{}" \;

	FW_RDIR=$CUR_compilation_RDIR/FW/build
	if [ ! -d $FW_RDIR ]
	then
		echo ""
		echo "FW Build's root directory "$FW_RDIR" does not exist. Exit..."
		echo ""
		exit 1
	fi

	cd $FW_RDIR

	echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@     START FW BUILD"
	./build_host.sh -c -p $plt

	if [ "$?" != "0" ]
	then
		echo "FW build on platform "$plt" error.  Exit..."
		exit 1
	fi

	chmod +x $FW_RDIR/bin/host/$plt/*.so
	echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@      END FW BUILD"


####################### End of main Compilation loop 
done

############ END OF COMPILATION PHASE (end of main loop per platform)


exit 0

