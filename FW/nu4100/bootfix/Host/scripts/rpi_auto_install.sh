#!/bin/bash

########################################################################################
# Name:		/home/pi/scripts/rpi_auto_install.sh
# Purpose:	Automatic Installation of Raspberry PI InuDev tar.gz pacakge
# Run by:	pi@raspberrypi
# By:		Yariv Sheizaf
# Date:		27-Dec-2015
# Arguments:
########################################################################################

CUR_VER="0"
PREV_VER="0"

SCRIPTS_DIR=/home/pi/scripts
cd $SCRIPTS_DIR

IS_MOUNTED_T=`df | grep rpiinst | wc -l`
if [ "$IS_MOUNTED" = "0" ]
then
	./mountT.sh
fi

RPI_TMP_DIR=/home/pi/Desktop/T
RPI_TMP_QAA_DIR=/home/pi/Desktop/T/qaa

while true
do
	sleep 15
	NEW_VER=`ls $RPI_TMP_DIR/InuSW*.gz | awk -F_ '{print $2}' | tail -1`
	if [ "$NEW_VER" != "$CUR_VER" ]
	then
		INUSERVICE_JOB_NUM=`ps e | grep InuService | awk '{print $1}'`
		kill -9 $INUSERVICE_JOB_NUM 1>/dev/null 2>&1

		PREV_VER=$CUR_VER
		CUR_VER=$NEW_VER

		cd $RPI_TMP_DIR
		./inudev_rpi_install.sh $CUR_VER

# Copy config files
		cd /usr/local/Inuitive/InuDev/config
		if [ "$?" != "0" ]
		then
			echo ""
			echo "/usr/local/Inuitive/InuDev/config directory not found. "
			echo ""
			exit 1
		fi
	
		cp $RPI_TMP_QAA_DIR/*.txt .
		if [ "$?" != "0" ]
		then
			echo ""
			echo "txt files not found. "
			echo ""
			exit 1
		fi

# Copy bin files
		cd /usr/local/Inuitive/InuDev/bin
		if [ "$?" != "0" ]
		then
			echo ""
			echo "/usr/local/Inuitive/InuDev/bin directory not found. "
			echo ""
			exit 1
		fi

		cp $RPI_TMP_QAA_DIR/*.so .
		if [ "$?" != "0" ]
		then
			echo ""
			echo "Shared Object files not found. "
			echo ""
			exit 1
		fi


		cp $RPI_TMP_QAA_DIR/RemoteAdamController .
		if [ "$?" != "0" ]
		then
			echo ""
			echo "RemoteAdamController file not found. "
			echo ""
			exit 1
		fi

		cp $RPI_TMP_QAA_DIR/OnHostRunner .
		if [ "$?" != "0" ]
		then
			echo ""
			echo "OnHostRunner file not found. "
			echo ""
			exit 1
		fi

# Run InuService
		./InuService &
		echo "Now will sleep 16 seconds.... Please wait"
		sleep 16
		echo "Finished post-InuService sleep interval"
	
# Reset controller
		./RemoteAdamController 6
		if [ "$?" != "0" ]
		then
			echo ""
			echo "RemoteAdamController action failed "
			echo ""
			exit 1
		else
			echo "RemoteAdamController: reset controller done successfully"
		fi
		
		echo "Now will sleep 16 seconds for reset USB.... Please wait"
		sleep 16
		echo "Finished post-reset sleep interval"


# Run OnHostRunner 
		su - pi -c "cd /usr/local/Inuitive/InuDev/bin;./OnHostRunner"

		echo "=============================================================="
		echo "InuSW "$CUR_VER" installation completed successfully"
		echo "=============================================================="

		cd $SCRIPTS_DIR		
	fi
done
	
exit $?






