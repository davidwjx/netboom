#!/bin/sh -x

LDIR=`/usr/bin/ls -ltr C:/InuAutomationOutput | grep Automation_ | grep "_20" | /usr/bin/awk '{print $NF}' | /usr/bin/tail -1`
echo $LDIR
cd C:/InuAutomationOutput
/usr/bin/pwd
cd /cygdrive/c/InuAutomationOutput/$LDIR
/usr/bin/cat TestResults.txt