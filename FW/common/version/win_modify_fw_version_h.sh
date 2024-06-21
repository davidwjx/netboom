#!/bin/bash -x

CUR_VER=`/usr/bin/grep SUB_BUILD_VERSION version.h | /usr/bin/head -2 | /usr/bin/tail -1 | /usr/bin/awk -F\" '{print $(NF-1)}'`
NEW_VER=`echo $CUR_VER | /usr/bin/awk '{print ($1+1)}'`

/usr/bin/sed 's/SUB_BUILD_VERSION  '$CUR_VER'/SUB_BUILD_VERSION  '$NEW_VER'/' version.h > v1
/usr/bin/head -12 v1 > v2
/usr/bin/rm v1

/usr/bin/grep "define SUB_BUILD_VERSION_STR" version.h | /usr/bin/sed 's/\"/@/g' | /usr/bin/sed 's/'$CUR_VER'/'$NEW_VER'/' | /usr/bin/sed 's/@/\"/g' >> v2

/usr/bin/tail -n 7 version.h >> v2

/usr/bin/mv v2 version.h

exit 0


