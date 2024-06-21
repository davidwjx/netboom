#!/bin/bash -x

CUR_VER=`grep SUB_BUILD_VERSION version.h | head -2 | tail -1 | awk -F\" '{print $(NF-1)}'`
NEW_VER=`echo $CUR_VER | awk '{print ($1+1)}'`

sed 's/SUB_BUILD_VERSION  '$CUR_VER'/SUB_BUILD_VERSION  '$NEW_VER'/' version.h > v1
head -12 v1 > v2
rm v1

grep "define SUB_BUILD_VERSION_STR" version.h | sed 's/\"/@/g' | sed 's/'$CUR_VER'/'$NEW_VER'/' | sed 's/@/\"/g' >> v2

tail -n 2 version.h >> v2

mv v2 version.h

exit 0


