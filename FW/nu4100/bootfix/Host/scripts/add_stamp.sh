#!/bin/sh -x
########################################################################################
# Name:		~cmadmin/scripts/add_stamp.sh
# Purpose:	Add Inuitive CopyRight stamp as file header for *.h and *.cpp files
# Run by:	cmadmin@UB05
# By:		Yariv Sheizaf
# Date:		07-Jan-2016
# Arguments:
########################################################################################

RDIR=`pwd`

for i in cpp h
do
	for j in `find . -type f -name *.$i`
	do
		DIRN=`dirname $j`
		BASEN=`basename $j`
		cd $DIRN
		echo "// Copyright (C) 2016 All rights reserved to Inuitive Tech. r2.03.0008.69-1" > xxx.$i
		cat $BASEN >> xxx.$i
		mv xxx.$i $BASEN
		cd $RDIR
	done
done

exit
