#!/bin/bash -x

########################################################################################
# Name:		~cmadmin/scripts/fw_all_platforms_compilation.sh
# Purpose:	FW build on all Linux platforms
# Run by:	cmadmin@UB02
# By:		Yariv Sheizaf
# Date:		29-Jun-2015
########################################################################################


FW_PROD_DIR=$1
PROD_DIR=$2

PROJ_NAME="InuSW"
BRANCH_NAME="trunk"

cd /home/cmadmin/jenkins/workspace/sw/trunk
CUR_BUILD_RDIR=`pwd`

VERSION_NUM=`grep "define IAF_VERSION_STR" $CUR_BUILD_RDIR/Host/dev/InuDev/include/Version.h  | cut -d\" -f2`

PROJ_VERSION=$PROJ_NAME"_"$VERSION_NUM

source ~/.profile


cd $CUR_BUILD_RDIR/FW/build

find .. -name "*.sh" -exec chmod +x "{}" \;

mkdir -p ${FW_PROD_DIR}/host

# for plt in android_ndk10d_armeabi-v7a

#for plt in linux_gcc-4.8_x86 android_ndk10d_armeabi-v7a tizen_gcc-4.8_armv7a linux_raspberry_arm

for plt in `cat $CUR_BUILD_RDIR/Host/scripts/Linux_Platforms.txt | grep -v \#`
do
	./build_host.sh -c -p $plt

	if [ $? -ne 0 ]; then
   		echo FAILURE=TRUE>${PROD_DIR}/status.properties
   		exit 1
	fi

	cp -r $CUR_BUILD_RDIR/FW/build/bin/host/$plt ${FW_PROD_DIR}/host
done

################# Removed build_client.sh call from the script, follow instructions by Kosta S., 3.8.2015 
#cd $CUR_BUILD_RDIR/FW/build
#./build_client.sh

#if [ $? -ne 0 ]; then
#	echo FAILURE=TRUE>${PROD_DIR}/status.properties
#	exit 1
#fi

exit 0

