#!/bin/bash -x

ADAM_MODULE=$1

HOSTN=`/usr/bin/hostname`

echo "HOSTN="$HOSTN

cd /cygdrive/c/AutomationData


NL_CURRENT_MODULE=`/usr/bin/nl StationInfo.txt | /usr/bin/grep Current | /usr/bin/awk '{print $1}'`
NL_CURRENT_MODULE_PLUS_TWO=`/usr/bin/expr $NL_CURRENT_MODULE + 2`

/usr/bin/head -$NL_CURRENT_MODULE StationInfo.txt > s1
echo $ADAM_MODULE >> s1
/usr/bin/tail -n +$NL_CURRENT_MODULE_PLUS_TWO StationInfo.txt >> s1
/usr/bin/mv s1 StationInfo.txt
echo "@@@@================@@@@@"
/usr/bin/cat StationInfo.txt
echo "@@@@================@@@@@"

cd $WORKSPACE

exit 0
