#!/bin/bash

########################################################################################
# Name:		/home/pi/scripts/jenkins_rpi.sh
# Purpose:	Automatic Installation of Raspberry PI InuDev tar.gz pacakge
# Run by:	pi@raspberrypi
# By:		Yariv Sheizaf
# Date:		11-Jan-2016
# Arguments:
#		1 - InuSW Branch name
#		2 - Version number
########################################################################################



BRANCH_NAME=$1
CUR_VER=$2
PROJ_NUMBER="InuSW_"$CUR_VER

IS_MOUNTED_T=`df | grep Versions_mounted | wc -l`
if [ "$IS_MOUNTED" = "0" ]
then
	mount -av
fi

MAJOR_MINOR_NUM=`echo $BRANCH_NAME | awk -F_ '{print $2$3}'`

VERSIONS_RDIR=/home/pi/Desktop/Versions_mounted/$PROJ_NUMBER/Linux/boot0
VERSIONS_QA_RDIR=/home/pi/Desktop/Versions_mounted/$PROJ_NUMBER/Linux/QA/linux_raspberry_arm

RPI_TARGZ_FILE="$PROJ_NUMBER"_f0_linux_raspberry_arm.tar.gz
RPI_INST_SCRIPT=inudev_rpi_install.sh

RPI_TMP_DIR=/home/pi/tmp
if [ -d $RPI_TMP_DIR ]
then
	rm -rf $RPI_TMP_DIR/*
else
	mkdir $RPI_TMP_DIR
fi

cd $RPI_TMP_DIR

cp $VERSIONS_RDIR/$RPI_TARGZ_FILE .
if [ "$?" != "0" ]
then
	echo ""
	echo $VERSIONS_RDIR"/"$RPI_TARGZ_FILE" file not found. "
	echo ""
	exit 1
fi

cp $VERSIONS_RDIR/$RPI_INST_SCRIPT .
if [ "$?" != "0" ]
then
	echo ""
	echo $VERSIONS_RDIR"/"$RPI_INST_SCRIPT" file not found. "
	echo ""
	exit 1
fi

INUSERVICE_JOB_NUM=`ps -ef | grep InuService | grep -v grep | awk '{print $2}'`
kill -9 $INUSERVICE_JOB_NUM 1>/dev/null 2>&1

echo ""
echo "InuService status AFTER old InuService killed:"
ps -ef | grep InuService | grep -v grep 
echo ""

./inudev_rpi_install.sh $CUR_VER

# Copy bin files
cd /usr/local/Inuitive/InuDev/bin
if [ "$?" != "0" ]
then
	echo ""
	echo "/usr/local/Inuitive/InuDev/bin directory not found. "
	echo ""
	exit 1
fi

# Copy Automation bin files
if [ -d /usr/local/Inuitive/InuAutomation/bin ]
then
	rm -rf /usr/local/Inuitive/InuAutomation/bin
else
	mkdir -p /usr/local/Inuitive/InuAutomation/bin
fi

cd /usr/local/Inuitive/InuAutomation/bin

cp $VERSIONS_QA_RDIR/*.so .
if [ "$?" != "0" ]
then
	echo ""
	echo "RPI QA Shared Object files not found. "
	echo ""
	exit 1
fi

cp $VERSIONS_QA_RDIR/RemoteAdamController .
if [ "$?" != "0" ]
then
	echo ""
	echo "RemoteAdamController file not found. "
	echo ""
	exit 1
fi

cp $VERSIONS_QA_RDIR/OnHostRunner .
if [ "$?" != "0" ]
then
	echo ""
	echo "OnHostRunner file not found. "
	echo ""
	exit 1
fi

# Copy config files
if [ ! -d /usr/local/Inuitive/InuAutomation/config ]
then
	mkdir -p /usr/local/Inuitive/InuAutomation/config
fi

if [ "$MAJOR_MINOR_NUM" -ge "28" ]
then
	cp $VERSIONS_QA_RDIR/AutomationParams.xml /usr/local/Inuitive/InuAutomation/config
	if [ "$?" != "0" ]
	then
		echo ""
		echo "AutomationParams.xml file not found. "
		echo ""
		exit 1
	fi
else
	cp $VERSIONS_QA_RDIR/AutomationParams_T.txt /usr/local/Inuitive/InuAutomation/config
	if [ "$?" != "0" ]
	then
		echo ""
		echo "AutomationParams_T.txt file not found. "
		echo ""
		exit 1
	fi
fi

cp $VERSIONS_QA_RDIR/5fps_configuration_for_RPI.ireg /usr/local/Inuitive/InuAutomation/config
if [ "$?" != "0" ]
then
	echo ""
	echo "5fps_configuration_for_RPI.ireg file not found. "
	echo ""
	exit 1
fi

# Run InuService

cd /usr/local/Inuitive/InuDev/bin

./InuService &
echo "Now will sleep 60 seconds.... Please wait"
sleep 60
echo "Finished post-InuService sleep interval"
echo ""
echo "InuService status AFTER new InuService started:"
ps -ef | grep InuService | grep -v grep 

	
# Reset controller

cd /usr/local/Inuitive/InuAutomation/bin

dos2unix /opt/AutomationData/StationInfo.txt
ADAM_HOST=`head -2 /opt/AutomationData/StationInfo.txt | tail -1`
./RemoteAdamController reset $ADAM_HOST 0
if [ "$?" != "0" ]
then
	echo ""
	echo "RemoteAdamController action failed "
	echo ""
	exit 1
else
	echo "RemoteAdamController: reset controller done successfully"
fi
		
echo "Now will sleep 60 seconds for reset USB.... Please wait"
sleep 60
echo "Finished post-reset sleep interval"

# Run OnHostRunner 
su - pi -c "cd /usr/local/Inuitive/InuAutomation/bin;./OnHostRunner"

echo "=============================================================="
echo "InuSW "$CUR_VER" installation completed successfully"
echo "=============================================================="
	
exit $?


