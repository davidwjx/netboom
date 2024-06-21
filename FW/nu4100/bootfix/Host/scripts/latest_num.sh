#!/bin/bash -x

echo $WORKSPACE

##  The workspace path @@@@   C:\jenkins\workspace\24x_Auto-Install-Win-X64-GuiAutomation-inu-mic-101

LATEST_NUM=`ls -ltr /cygdrive/c/InuAutomationOutput | grep "GUI_Automation_" | grep drwx | tail -1 | gawk '{print $NF}'`
echo "LATEST_BUILD="$LATEST_NUM > $WORKSPACE/num.properties

exit 0
