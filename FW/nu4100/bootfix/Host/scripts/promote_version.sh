#!/bin/bash -x

# cd /cygdrive/c/svn_client/db/NU3000/project_ver0/sw/$1/Host/scripts

svn update

CUR_FULL_VER=`grep MyAppVersion InuDevPlusX64.iss  | head -1 | awk -F\" '{print $2}'`
CUR_MAJOR_MINOR=`echo $CUR_FULL_VER | awk -F. '{print $1"."$2"."}'`
CUR_BRANCH_NUM=`grep MyAppVersion InuDevPlusX64.iss | head -1 | awk -F. '{print $3}' | cut -c1-2`
CUR_VER=`grep MyAppVersion InuDevPlusX64.iss | head -1 | awk -F. '{print $3}' | cut -c3-4`
CUR_BUILD=`echo $CUR_FULL_VER | awk -F. '{print "."$4}'`
NEW_VER=`echo $CUR_VER | awk '{print $NF+1}'`
NEW_VER_WCC=`echo $NEW_VER | wc -c`
if [ "$NEW_VER_WCC" = "2" ]
then
		NEW_VER="0"$NEW_VER
fi

NEW_STR=$CUR_MAJOR_MINOR$CUR_BRANCH_NUM$NEW_VER$CUR_BUILD

for i in InuDev InuAutomation
do
	for j in `ls -l $i*.iss | awk '{print $NF}'`
	do
		
		sed 's/'$CUR_FULL_VER'/'$NEW_STR'/' $j > $j.tmp
		mv $j.tmp $j
	done
done

X=`pwd | sed 's?/Host/scripts??'`
BASEN=`basename $X`

svn commit -m "JID:TEST-25 promote $BASEN version to $NEW_STR" *.iss
