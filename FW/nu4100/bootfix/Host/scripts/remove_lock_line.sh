#!/bin/sh -x

########################################################################
# Name:		svc05:/home/cmadmin/scripts/remove_lock_line.sh
# Purpose:	Unlock branch in SVN permissions configuration file
# By:		Yariv Sheizaf
# Date:		28.4.2015
# Arguments:	1 - SVN Branch Name
########################################################################

BRANCH_NAME=$1
PERMS_FILE=/repos/svn/inuitive/conf/svnperms.conf
TMP_FILE=/tmp/svnperms.conf.tmp

# Backup old perms file
DATEN=`date +%Y%m%d%H%M`
BCK_FILE=/tmp/"svnperms.conf."$DATEN
cp $PERMS_FILE $BCK_FILE

grep -v $BRANCH_NAME $PERMS_FILE > $TMP_FILE
cat $TMP_FILE > $PERMS_FILE

exit $?
