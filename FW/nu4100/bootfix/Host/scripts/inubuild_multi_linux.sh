#!/bin/bash -x

########################################################################################
# Name:		~user/scripts/inubuild_multi_linux.sh
# Purpose:	Full Inuitive software build script for mutliple Linux operating systems
# Run by:	user@UB02
# By:		Yariv Sheizaf
# Date:		25-Mar-2015
# Arguments:
#		1 - branch name
########################################################################################

if [ "$#" != "2" ]
then
	echo ""
	echo "Syntax:  inubuild_multi_linux.sh <branch name> <version number>"
	echo "Example: inubuild_multi_linux.sh InuSW_1_11_0 1.11.03"
	echo "Exit..."
	exit 1
fi

##################### General definitins

PROJ_BRANCH=$1
PROJ_NAME=`echo $PROJ_BRANCH | cut -d_ -f1`
BRANCH_NAME=`echo $PROJ_BRANCH | cut -d_ -f2-`

VERSION_NUM=$2

PROJ_VERSION=$PROJ_NAME"_"$VERSION_NUM

CUR_BUILD_RDIR=`pwd`
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

######## Cycle number check - don't run the whole proces if it is 2nd cycle
#if [ -f $LINUX_VER_DIR/sent.txt ]
#then
#	CYCLE_NUM=`cat $LINUX_VER_DIR/sent.txt`
#	if [ "$CYCLE_NUM" = "1" ]
#	then
#		echo "2" > $LINUX_VER_DIR/sent.txt
#		echo ""
#		echo "Second cycle is not required. Exit..."
#		echo ""
#		exit 0
#	else
#		echo "1" > $LINUX_VER_DIR/sent.txt
#	fi
#else
#	echo "1" > $LINUX_VER_DIR/sent.txt
#fi


##### Cleanup bin - products tree
if [ -d $CUR_BUILD_RDIR/bin ]
then
	rm -rf $CUR_BUILD_RDIR/bin
fi
mkdir $CUR_BUILD_RDIR/bin

##################### MAIN LOOP PER Platform   

PLATFORMS_TO_BUILD=$CUR_BUILD_RDIR/Host/scripts/Linux_Platforms.txt

for plt in `grep -v \# $PLATFORMS_TO_BUILD`
do
	mkdir $CUR_BUILD_RDIR/bin/$plt

### Common build

	COMMON_RDIR=$CUR_BUILD_RDIR/Common/src

	echo "====================  COMMON BUILD START     ===================================="
	cd $COMMON_RDIR

	echo "====================  COMMON Clean START     ===================================="
	make -f build.mk TARGET=clean PLATFORM=$plt clean
	if [ "$?" != "0" ]
	then
		exit 1
	fi

	echo "====================  COMMON all START     ===================================="
	make -f build.mk TARGET=all PLATFORM=$plt all
	if [ "$?" != "0" ]
	then
		exit 1
	fi

### Algo build

	ALGO_RDIR=$CUR_BUILD_RDIR/Algo/C++/CSE

	echo "====================  Algo BUILD START     ===================================="
	cd $ALGO_RDIR

	echo "====================  ALGO Clean START     ===================================="
	make -f build.mk TARGET=clean PLATFORM=$plt clean
	if [ "$?" != "0" ]
	then
		exit 1
	fi

	echo "====================  ALGO all START     ===================================="
	make -f build.mk TARGET=all PLATFORM=$plt all
	if [ "$?" != "0" ]
	then
		exit 1
	fi


########## InuService build

	echo "====================  InuService build START     ===================================="
# InuService product files removal
	rm -f $CUR_BUILD_RDIR/bin/$plt/InuService

	INUSERVICE_RDIR=$CUR_BUILD_RDIR/Host/dev/InuDev/src
	if [ ! -d $INUSERVICE_RDIR ]
	then
		echo ""
		echo "InuService Build's root directory "$INUSERVICE_RDIR" does not exist. Exit..."
		echo ""
		exit 1
	fi

	echo "====================  InuService FW START     ===================================="

	
	cd $CUR_BUILD_RDIR/FW
	find . -name "*.sh" -print -exec chmod +x "{}" \;

	FW_RDIR=$CUR_BUILD_RDIR/FW/build
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
		echo "InuService FW build on platform "$plt" error.  Exit..."
		exit 1
	fi

	chmod +x $FW_RDIR/bin/host/$plt/*.so
echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@      END FW BUILD"


	echo "====================  InuService clean START     ===================================="

	cd $INUSERVICE_RDIR

	make -f build.mk TARGET=clean PLATFORM=$plt clean
	if [ "$?" != "0" ]
	then
		echo "InuService CLEAN build on platform "$plt" error.  Exit..."
		exit 1
	fi

	echo "====================  InuService all START     ===================================="
	make -f build.mk TARGET=all PLATFORM=$plt all
	if [ "$?" != "0" ]
	then
		echo "InuService ALL build on platform "$plt" error.  Exit..."
		exit 1
	fi



######## End of Inuviewer Andorid build


######## wxInuDevDemo demo build - on linux_gcc-4.8_x86 platform (Ubuntu) only
	if [ "$plt" = "linux_gcc-4.8_x86" ]
	then
		WXINUDEVDEMO_RDIR=$CUR_BUILD_RDIR/Host/dev/Demo/wxInuDevDemo
		if [ ! -d $WXINUDEVDEMO_RDIR ]
		then
			echo ""
			echo "WX Demo's root directory "$WXINUDEVDEMO_RDIR" does not exist. Exit..."
			echo ""
			exit 1
		fi

		cd $WXINUDEVDEMO_RDIR

		WXBV="BUILD_VERSION="`awk -F= '{print $2+1}' build_version.mk`	
		echo $WXBV > build_version.mk
		svn commit -m "JID:NON-1 promote wxInuDevDemo build number by 1" build_version.mk
		if [ "$?" != "0" ]
		then
			echo "InuDevDemo build version file svn commit failed. Exit..."
			exit 1
		fi
	
		make clean
		rm $WXINUDEVDEMO_RDIR/inudevdemo*.deb

		make build
		if [ "$?" != "0" ]
		then
			echo "InuDevDemo build on platform "$plt" error.  Exit..."
			exit 1
		fi

		make deb
		if [ "$?" != "0" ]
		then
			echo "InuDevDemo debian package creation on platform "$plt" error.  Exit..."
			exit 1
		fi

# Copy *.deb file to final storage location 
		rm $LINUX_VER_DIR/inudevdemo*.deb
		cp $WXINUDEVDEMO_RDIR/inudevdemo*.deb $LINUX_VER_DIR		
	fi

####################### End of main Compilation loop 
done

############ END OF COMPILATION PHASE (end of main loop per platform)

############  Packaging 


echo "====================  packaging process START     ===================================="
# Prepare set of files for packaging
cd $CUR_BUILD_RDIR

if [ -d pack_set ]
then
	rm -rf pack_set
fi

mkdir pack_set
cd $CUR_BUILD_RDIR/pack_set

PRD_CUR_VER_RDIR=$OUTPUT_TREE_MOUNT_POINT/$PROJ_VERSION/x64

########################################

##### Andorid packaging

for plt in `grep -v \# $PLATFORMS_TO_BUILD | grep android`
do
	INUITIVE_ROOT=$CUR_BUILD_RDIR/pack_set/$plt/Inuitive

### Start Create InuDev.zip
	INUDEV_ROOT=$INUITIVE_ROOT/InuDev
	mkdir -p $INUDEV_ROOT

#bin
	mkdir $INUDEV_ROOT/bin
	cd $INUDEV_ROOT/bin
	cp $CUR_BUILD_RDIR/bin/$plt/libInuStreams.so .
	cp $ANDROID_NDK/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi/libgnustl_shared.so .
	cd $INUDEV_ROOT

# docs
	mkdir $INUDEV_ROOT/docs
	cd $INUDEV_ROOT/docs
	cp -r $PRD_CUR_VER_RDIR/docs/InuDev.chm .
	cd $INUDEV_ROOT

# include
	cd $INUDEV_ROOT
	cp -r $PRD_CUR_VER_RDIR/include .
	rm -f include/*Ext.h
	cd $INUDEV_ROOT



##### Create not_stripped tar file and copy it to final location
	cd $CUR_BUILD_RDIR/pack_set/$plt/Inuitive
	ZIPF=$PROJ_VERSION"_InuDev_"$plt"_not_stripped.zip"
	zip -r $ZIPF InuDev
	cp $ZIPF $LINUX_NOT_STRIPPED_VER_DIR

##### Strip

	if [ "$plt" = "android_ndk10d_armeabi-v7a" ]
	then
		STRIP_EXE=$ANDROID_NDK/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86/bin/arm-linux-androideabi-strip
	fi

	cd $INUDEV_ROOT/bin
	$STRIP_EXE InuService
	$STRIP_EXE *.so

#### Strip END



# Create zip files and copy them into final location
	cd $CUR_BUILD_RDIR/pack_set/$plt/Inuitive
	ZIPF=$PROJ_VERSION"_InuDev_"$plt".zip"
	zip -r $ZIPF InuDev
	cp $ZIPF $LINUX_VER_DIR

### End create InuDev.zip


### Start Create InuDevJava.zip

	INUDEVJAVA_ROOT=$INUITIVE_ROOT/InuDevJava
	mkdir -p $INUDEVJAVA_ROOT

#bin
	mkdir $INUDEVJAVA_ROOT/bin
	cd $INUDEVJAVA_ROOT/bin
	cp $CUR_BUILD_RDIR/bin/$plt/libInuStreams.so .
	cp $CUR_BUILD_RDIR/bin/$plt/InuStreamsJava.jar .
	cp $CUR_BUILD_RDIR/bin/$plt/libInuStreamsJNI.so .
	cp $ANDROID_NDK/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi/libgnustl_shared.so .
	cd $INUDEVJAVA_ROOT

# docs
	mkdir $INUDEVJAVA_ROOT/docs
	cd $INUDEVJAVA_ROOT/docs
	cp -r $PRD_CUR_VER_RDIR/docs/InuDev.chm .
	cd $INUDEVJAVA_ROOT



##### Create not_stripped tar file and copy it to final location
	cd $CUR_BUILD_RDIR/pack_set/$plt/Inuitive
	ZIPF=$PROJ_VERSION"_InuDevJava_"$plt"_not_stripped.zip"
	zip -r $ZIPF InuDevJava
	cp $ZIPF $LINUX_NOT_STRIPPED_VER_DIR


##### Strip

	if [ "$plt" = "android_ndk10d_armeabi-v7a" ]
	then
		STRIP_EXE=$ANDROID_NDK/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86/bin/arm-linux-androideabi-strip
	fi

	cd $INUDEVJAVA_ROOT/bin
	$STRIP_EXE InuService
	$STRIP_EXE *.so

#### Strip END


# Create zip files and copy them into final location
	cd $CUR_BUILD_RDIR/pack_set/$plt/Inuitive
	ZIPF=$PROJ_VERSION"_InuDevJava_"$plt".zip"
	zip -r $ZIPF InuDevJava
	cp $ZIPF $LINUX_VER_DIR

### End create InuDevJava.zip

### Start create InuService.zip

	cd $INUITIVE_ROOT
	rm -rf $INUDEV_ROOT 
	rm -f *.zip
	mkdir $INUDEV_ROOT
	
### config
	mkdir -p $INUDEV_ROOT/config/InuSensors
	cd $INUDEV_ROOT/config

	cp $CUR_BUILD_RDIR/Host/dev/InuDev/src/InuService/InuServiceParamsPlus.xml ./InuServiceParams.xml

	cp $CUR_BUILD_RDIR/Common/src/CommonAlgo/AlgoBase/sensor_ctrl_config_file.txt .
	if [ "$?" != "0" ]
	then
		echo $CUR_BUILD_RDIR"/Common/src/CommonAlgo/AlgoBase/sensor_ctrl_config_file.txt not found  Exit..."
		exit 1
	fi

	cp $PRD_CUR_VER_RDIR/config/InuModelDB.csv .
	if [ "$?" != "0" ]
	then
		echo $PRD_CUR_VER_RDIR"/config/InuModelDB.csv not found  Exit..."
		exit 1
	fi

	cp $CUR_BUILD_RDIR/Algo/C++/CSE/AlgDepthImprovements/di_*.dat .
	if [ "$?" != "0" ]
	then
		echo $CUR_BUILD_RDIR"/Algo/C++/CSE/AlgDepthImprovements dat files not found.  Exit..."
		exit 1
	fi


### bin
	mkdir -p $INUDEV_ROOT/bin
	cd $INUDEV_ROOT/bin
	cp -r $PRD_CUR_VER_RDIR/bin/boot .
	cp $CUR_BUILD_RDIR/bin/$plt/InuService .
	cp $CUR_BUILD_RDIR/bin/$plt/*.so .
	cp $FW_RDIR/bin/host/$plt/*.so .
	cp $ANDROID_NDK/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi/libgnustl_shared.so .
	cp $CUR_BUILD_RDIR/Host/scripts/run.sh .


# Create not_stripped tar file and copy it to final location
	cd $CUR_BUILD_RDIR/pack_set/$plt
	ZIPF=$PROJ_VERSION"_InuService_"$plt"_not_stripped.zip"
	zip -r $ZIPF Inuitive/InuDev
	cp $ZIPF $LINUX_NOT_STRIPPED_VER_DIR


##### Strip

	if [ "$plt" = "android_ndk10d_armeabi-v7a" ]
	then
		STRIP_EXE=$ANDROID_NDK/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86/bin/arm-linux-androideabi-strip
	fi

	cd $INUDEV_ROOT/bin
	$STRIP_EXE InuService
	$STRIP_EXE *.so

#### Strip END


# Create prodcution (stripped) zip files and copy them into final location
	cd $CUR_BUILD_RDIR/pack_set/$plt
	ZIPF=$PROJ_VERSION"_InuService_"$plt".zip"
	zip -r $ZIPF Inuitive/InuDev
	cp $ZIPF $LINUX_VER_DIR

	
done
##### End of Android packaging

##### Ubuntu packaging

for plt in `grep -v \# $PLATFORMS_TO_BUILD | grep -v android`
do
	INUDEV_ROOT=$CUR_BUILD_RDIR/pack_set/$plt/Inuitive/InuDev
	mkdir -p $INUDEV_ROOT

# config
	cd $INUDEV_ROOT
	mkdir -p config/icons
	cd config
	
	if [ "$plt" = "linux_gcc-4.8_x86" ]
	then
		cp $CUR_BUILD_RDIR/Host/dev/InuDev/src/InuService/InuServiceParamsPlus.xml ./InuServiceParams.xml
	fi

#### Raspberry - should move to Raspberry packaging section
#	if [ "$plt" = "linux_raspberry_arm" ]
#	then
#		cp $CUR_BUILD_RDIR/Host/dev/InuDev/src/InuService/InuServiceParamsRaspberry.xml ./InuServiceParams.xml
#	fi

	cp $PRD_CUR_VER_RDIR/config/InuModelDB.csv .
	if [ "$?" != "0" ]
	then
		echo $PRD_CUR_VER_RDIR"/config/InuModelDB.csv not found  Exit..."
		exit 1
	fi

	cp $CUR_BUILD_RDIR/Host/scripts/Inuitive-Service.desktop ./icons
	if [ "$?" != "0" ]
	then
		echo $CUR_BUILD_RDIR"/Host/scripts/Inuitive-Service.desktop not found.  Exit..."
		exit 1
	fi
	chmod +x ./icons/Inuitive-Service.desktop


	cp $CUR_BUILD_RDIR/Host/dev/InuDev/docs/LOGO.bmp ./icons
	if [ "$?" != "0" ]
	then
		echo $CUR_BUILD_RDIR"/Host/dev/InuDev/docs/LOGO.bmp not found.  Exit..."
		exit 1
	fi

	cp $CUR_BUILD_RDIR/Algo/C++/CSE/AlgDepthImprovements/di_*.dat .
	if [ "$?" != "0" ]
	then
		echo $CUR_BUILD_RDIR"/Algo/C++/CSE/AlgDepthImprovements dat files not found.  Exit..."
		exit 1
	fi

	cp $CUR_BUILD_RDIR/Common/src/CommonAlgo/AlgoBase/sensor_ctrl_config_file.txt .
	if [ "$?" != "0" ]
	then
		echo $CUR_BUILD_RDIR"/Common/src/CommonAlgo/AlgoBase/sensor_ctrl_config_file.txt not found.  Exit..."
		exit 1
	fi

	mkdir InuSensors

# docs
	cd $INUDEV_ROOT
	cp -r $PRD_CUR_VER_RDIR/docs .

# include
	cd $INUDEV_ROOT
	cp -r $PRD_CUR_VER_RDIR/include .
	rm -f include/*Ext.h

# bin
	cd $INUDEV_ROOT
	mkdir bin
	cd bin
	cp $CUR_BUILD_RDIR/Host/scripts/SetPermissionRT .
	chmod +x SetPermissionRT

	cp $CUR_BUILD_RDIR/Host/scripts/inudev_post_install_deb.sh .
	chmod +x inudev_post_install_deb.sh

	cp $CUR_BUILD_RDIR/Host/scripts/inudev_pre_uninstall_deb.sh .
	chmod +x inudev_pre_uninstall_deb.sh

	cp $CUR_BUILD_RDIR/Host/scripts/inuservice .
	chmod +x inuservice

	cp $CUR_BUILD_RDIR/Host/scripts/inuservice.conf .

#	cp $CUR_BUILD_RDIR/Host/scripts/is_inuservice_run.sh .
#	chmod +x is_inuservice_run.sh

	cp -r $PRD_CUR_VER_RDIR/bin/boot .

	cp $CUR_BUILD_RDIR/bin/$plt/InuService .
	cp $CUR_BUILD_RDIR/bin/$plt/*.so .

	cp $FW_RDIR/bin/host/$plt/*.so .

	for k in libopencv_calib3d libopencv_core libopencv_features2d libopencv_flann libopencv_imgproc
	do
		cp $OPENCV_PATH/build/lib/$k.so.2.4.9 ./$k.so.2.4
		if [ "$?" != "0" ]
		then
			echo $OPENCV_PATH"/build/lib/"$k".so.2.4.9 file not found.  Exit..."
			exit 1
		fi
	done

	cp $OPENCV_PATH/build/lib/libtbb.so.2 .


# lib
	cd $INUDEV_ROOT
	mkdir lib
	cp bin/libInuStreams.so lib/libInuStreams.so


#### Create not_stripped tar file and copy it to final location
	cd $CUR_BUILD_RDIR/pack_set/$plt
	TARF=$PROJ_VERSION"_"$plt"_not_stripped.tar"
	tar cvf $TARF Inuitive
	gzip $TARF
	TARF_GZ=$TARF".gz"
	rm $LINUX_VER_DIR/$TARF_GZ
	cp $TARF_GZ $LINUX_NOT_STRIPPED_VER_DIR


##### Strip

	if [ "$plt" = "linux_gcc-4.8_x86" ]
	then
		STRIP_EXE=/usr/bin/strip
	fi

	if [ "$plt" = "linux_raspberry_arm" ]
	then
		STRIP_EXE=/home/cmadmin/rpi/tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian/bin/arm-linux-gnueabihf-strip
	fi

	cd $INUDEV_ROOT/bin
	$STRIP_EXE InuService
	$STRIP_EXE *.so
	$STRIP_EXE ../lib/*.so

#### Strip END


# Create tar file and copy it to final location
	cd $CUR_BUILD_RDIR/pack_set/$plt
	TARF=$PROJ_VERSION"_"$plt".tar"
	tar cvf $TARF Inuitive
	gzip $TARF
	TARF_GZ=$TARF".gz"
	rm $LINUX_VER_DIR/$TARF_GZ
	cp $TARF_GZ $LINUX_VER_DIR


# Debian package creation
	if [ "$plt" = "linux_gcc-4.8_x86" ]
	then
		DEBIAN_TMP_RDIR=/tmp/for_debian
		if [ -d $DEBIAN_TMP_RDIR ]
		then
			rm -rf $DEBIAN_TMP_RDIR/*
		else
			mkdir -p $DEBIAN_TMP_RDIR
		fi
	
		cp $TARF_GZ $DEBIAN_TMP_RDIR
		cd $DEBIAN_TMP_RDIR
		tar xvf $DEBIAN_TMP_RDIR/$TARF_GZ
		rm $DEBIAN_TMP_RDIR/$TARF_GZ
		cd $DEBIAN_TMP_RDIR/Inuitive/InuDev/bin

		sed 's/9.99.99/'$VERSION_NUM'/' inudev_post_install_deb.sh > i1
		mv i1 inudev_post_install_deb.sh
		chmod +x inudev_post_install_deb.sh

		IS_OLD_DEB=`ls -l $INSTALLBUILDER_RDIR/output/inudev_*.deb | awk '{print $NF}' | wc -l`		
		if [ "$IS_OLD_DEB" != "0" ]
		then
			rm $INSTALLBUILDER_RDIR/output/inudev_*.deb
		fi

		sed 's/9.99.99/'$VERSION_NUM'/' $CUR_BUILD_RDIR/Host/scripts/inudev.xml > $INSTALLBUILDER_RDIR/projects/inudev.xml

		cd $INSTALLBUILDER_RDIR/bin
		$INSTALLBUILDER_RDIR/bin/builder build $INSTALLBUILDER_RDIR/projects/inudev.xml deb

		IS_NEW_DEB=`ls -l $INSTALLBUILDER_RDIR/output/inudev_*.deb | awk '{print $NF}' | wc -l`				
		if [ "$IS_NEW_DEB" != "0" ]
		then
			cp $INSTALLBUILDER_RDIR/output/inudev_*.deb $LINUX_VER_DIR
		else
			echo "Debian file did not created. Exit..."
			exit 1
		fi
	fi


done
####################### End of main NON-Android Packaging loop



######## InuViewer Android build start
for plt in `grep -v \# $PLATFORMS_TO_BUILD | grep android`
do
	if [ "$plt" = "android_ndk10d_armeabi-v7a" ]
	then

		STRIP_EXE=$ANDROID_NDK/toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86/bin/arm-linux-androideabi-strip

		IVA_RDIR=$CUR_BUILD_RDIR/Host/dev/Tools/InuViewerAndroid
		if [ ! -d $IVA_RDIR ]
		then
			echo ""
			echo "InuViewer Android "$IVA_RDIR" does not exist. Exit..."
			echo ""
			exit 1
		fi

		cd $IVA_RDIR

		for g in app/src/main/jniLibs/armeabi app/src/main/jniLibs/armeabi-v7a lib/armeabi lib/armeabi-v7a
		do
			mkdir -p $g
			cp $CUR_BUILD_RDIR/bin/android_ndk10d_armeabi-v7a/libInuStreamsJNI.so $IVA_RDIR/$g
			cp $CUR_BUILD_RDIR/bin/android_ndk10d_armeabi-v7a/libInuStreams.so $IVA_RDIR/$g
			cp $ANDROID_NDK/sources/cxx-stl/gnu-libstdc++/4.8/libs/armeabi/libgnustl_shared.so $IVA_RDIR/$g
			$STRIP_EXE $IVA_RDIR/$g/*.so
		done

		for k in app/src/main/jniLibs/armeabi app/src/main/jniLibs/armeabi-v7a app/libs
		do
			mkdir -p $k
			cp $CUR_BUILD_RDIR/bin/android_ndk10d_armeabi-v7a/InuStreamsJava.jar $IVA_RDIR/$k
			$STRIP_EXE  $IVA_RDIR/$k/*.jar
		done

		sed 's?SDKDIR?'$ANDROID_SDK'?' local.properties > l1
		mv l1 local.properties

		chmod +x ./gradlew
		./gradlew assembleRelease
		if [ "$?" != "0" ]
		then
			echo "gradlew assembleRelease build on platform "$plt" error.  Exit..."
			exit 1
		fi

# Copy *.apk file to final storage location 
		cp $IVA_RDIR/app/build/outputs/apk/app-release.apk $LINUX_VER_DIR/InuView_$VERSION_NUM.apk
	fi
done
######## InuViewer Android build END


####################### Start of Tagging

TAGS_RDIR=http://svn/repos/inuitive/db/NU3000/project_ver0/sw/tags/InuSW/builds/$VERSION_NUM

svn ls $TAGS_RDIR 1>/dev/null 2>&1
if [ "$?" = "0" ]
then
	svn delete $TAGS_RDIR -m "InuSW Release $VERSION_NUM old tag deletion"
fi
svn copy http://svn/repos/inuitive/db/NU3000/project_ver0/sw/branches/Versions/$PROJ_BRANCH $TAGS_RDIR -m "InuSW Release $VERSION_NUM tag creation"
####################### End of Tagging

####################### Unlock branch
ssh svc05 "/home/cmadmin/scripts/remove_lock_line.sh '${PROJ_BRANCH}'"

exit 0

