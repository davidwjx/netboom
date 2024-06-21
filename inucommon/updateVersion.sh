#!/bin/bash

BUILD_VERSION=$(echo $(cat include/Version.h | grep "define INUCOMMON_VERSION_STR" | awk -F '\"' '{print $2}') | sed -e 's/[[:space:]]*$//')
RIGHT_VER=`echo $BUILD_VERSION | awk -F. '{print $4}'`
ISLEFTZERO=`echo $RIGHT_VER | cut -c1`
ISRIGHTNINE=`echo $RIGHT_VER | cut -c2`
if [ "$ISLEFTZERO" = "0" -a "$ISRIGHTNINE" != "9" ]
then
    VERSION_NUM=`echo $BUILD_VERSION | awk -F. '{print $1"."$2"."$3".0"$4+1}'`
else
    VERSION_NUM=`echo $BUILD_VERSION | awk -F. '{print $1"."$2"."$3"."$4+1}'`
fi
perl updateInuCommonVersion.pl ${PWD} $VERSION_NUM