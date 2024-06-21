#!/bin/bash

FW_VER_FILE=$1

MAJORN=`/usr/bin/grep MAJOR_VERSION $FW_VER_FILE  | /usr/bin/head -1 | /usr/bin/awk '{print $NF}'`
MINORN=`/usr/bin/grep MINOR_VERSION $FW_VER_FILE  | /usr/bin/head -1 | /usr/bin/awk '{print $NF}'`
BUILDN=`/usr/bin/grep BUILD_VERSION $FW_VER_FILE  | /usr/bin/head -1 | /usr/bin/awk '{print $NF}'`
SUBBUN=`/usr/bin/grep SUB_BUILD_VERSION $FW_VER_FILE  | /usr/bin/head -1 | /usr/bin/awk '{print $NF}'`

CUR_VER=$MAJORN"."$MINORN"."$BUILDN"."$SUBBUN

echo $CUR_VER > curver.txt
echo $CUR_VER

exit 0