#!/bin/bash -x

########################################################################################
# Name:		~user/scripts/create_deb.sh
# Purpose:	Create debian package installation set
# Run by:	cmadmin@UB02
# By:		Yariv Sheizaf
# Date:		25-Mar-2015
# Arguments:
#		1 - product name
#		2 - version number, in format aa.bb.cc (major.minor.build)
########################################################################################

if [ "$#" != "2" ]
then
	echo ""
	echo "Syntax:  create_deb.sh <product name> <version number>"
	echo "Example: create_deb.sh InuSW 1.11.03"
	echo "Exit..."
	exit 1
fi

##################### General definitins

DDATE=`date | sed 's/ /@@@/g'`

PRODUCT_NAME=$1
LOWERCASE_PRODUCT_NAME=`echo $PRODUCT_NAME| tr [A-Z] [a-z]`

FULL_VER=$2
MAJOR_VER=`echo $FULL_VER | cut -d. -f1`
MINOR_VER=`echo $FULL_VER | cut -d. -f2`
BUILD_VER=`echo $FULL_VER | cut -d. -f3`

PROJ_VERSION=$PRODUCT_NAME"_"$FULL_VER
LOWERCASE_PROJ_VERSION=$LOWERCASE_PRODUCT_NAME"_"$FULL_VER

OUTPUT_TREE_MOUNT_POINT=/space/users/cmadmin/Versions

LINUX_VER_DIR=$OUTPUT_TREE_MOUNT_POINT/$PROJ_VERSION/Linux
if [ ! -d $LINUX_VER_DIR ]
then
	echo ""
	echo $LINUX_VER_DIR" directory is not exist."
	echo "Exit..."
	exit 1
fi

UBUNTU_TARGZ=$LINUX_VER_DIR/$PROJ_VERSION"_Default.tar.gz"
if [ ! -f $UBUNTU_TARGZ ]
then
	echo ""
	echo $UBUNTU_TARGZ" is not exist."
	echo "Exit..."
	exit 1
fi

LOWERCASE_UBUNTU_TARGZ=$LINUX_VER_DIR/$LOWERCASE_PROJ_VERSION"_Default.tar.gz"
ln -s $UBUNTU_TARGZ $LOWERCASE_UBUNTU_TARGZ

DEBIAN_BASE_SET=$LINUX_VER_DIR/debian_base.tar
if [ ! -f $DEBIAN_BASE_SET ]
then
	echo ""
	echo $DEBIAN_BASE_SET" is not exist."
	echo "Exit..."
	exit 1
fi

cd $LINUX_VER_DIR

PROJ_MAJOR_MINOR=$LOWERCASE_PRODUCT_NAME"-"$MAJOR_VER"."$MINOR_VER
mkdir -p pdebian/$PROJ_MAJOR_MINOR
cd pdebian
cp $UBUNTU_TARGZ ./$LOWERCASE_PRODUCT_NAME"_"$MAJOR_VER"."$MINOR_VER".orig.tar.gz"
cd $PROJ_MAJOR_MINOR

### Debian set of files editing
cp $DEBIAN_BASE_SET .
tar xvf debian_base.tar
rm -f debian_base.tar

cd debian

sed 's/EBRRNA/'$LOWERCASE_PRODUCT_NAME'/' changelog | sed 's/MAJORVER/'$MAJOR_VER'/' | sed 's/MINORVER/'$MINOR_VER'/' | sed 's/BUILDVER/'$BUILD_VER'/' | sed 's/DDDDDD/'$DDATE'/' | sed 's/@@@/ /g' > tmp_c1
mv tmp_c1 changelog

sed 's/EBRRNA/'$LOWERCASE_PRODUCT_NAME'/g' control > tmp_c2
mv tmp_c2 control

cd ..
tar xvf $UBUNTU_TARGZ

debuild -uc -us --source-option=--include-binaries --source-option=-isession
if [ "$?" != "0" ]
then
	echo patch1 | dpkg-source --commit
	debuild -uc -us --source-option=--include-binaries --source-option=-isession
fi

exit $?	

