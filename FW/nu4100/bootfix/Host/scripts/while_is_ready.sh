#!/bin/bash -x

IS_READY=1
IS_GUI_RUNNING=1
PROC_RUN_TIME=0

echo "===================================================="
date
echo "Now sleep one minute"
sleep 60

cd /cygdrive/c/AutomationData

while [ $IS_READY -ne 0 -o $IS_GUI_RUNNING -ne 0 ]
do
	sleep 60
	date
	dos2unix -n GUIAutomationLauncherConfig.txt gu.txt 1>/dev/null 2>&1
	IS_READY=`head -2 gu.txt | tail -1`
	IS_GUI_RUNNING=`tail -1 gu.txt`
	rm gu.txt
	
	PROC_RUN_TIME=`expr $PROC_RUN_TIME + 1`
	if [ $PROC_RUN_TIME -ge 175 ]
	then
		/cygdrive/c/Program\ Files/Inuitive/InuAutomation/bin/FileModifier.exe is_ready 0
		/cygdrive/c/Program\ Files/Inuitive/InuAutomation/bin/FileModifier.exe is_ready 0
	fi
echo "===================================================="
done

exit 0
