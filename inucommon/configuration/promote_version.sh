#!/bin/bash -x
if [ "$#" -lt "1" ]; then
    echo ""
    echo "Syntax:  promoteVersion.sh <build version>"
    echo "Example: promoteVersion.sh 4.19.0000.01"
    echo "Exit..."
    exit 1
fi 

BUILD_VERSION=$1

##############- Promote Version -##############
RIGHT_VER=$(echo "${BUILD_VERSION}" | awk -F. '{print $4}')
ISLEFTZERO=$(echo "${RIGHT_VER}" | cut -c1)
ISRIGHTNINE=$(echo "${RIGHT_VER}" | cut -c2)
if [ "$ISLEFTZERO" = "0" -a "$ISRIGHTNINE" != "9" ]
then
    VERSION_NUM=$(echo "${BUILD_VERSION}" | awk -F. '{print $1"."$2"."$3".0"$4+1}')
else
    VERSION_NUM=$(echo "${BUILD_VERSION}" | awk -F. '{print $1"."$2"."$3"."$4+1}')
fi
perl update_inucommon_version.pl "$(pwd)" "${VERSION_NUM}"

exit 0