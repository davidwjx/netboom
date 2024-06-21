#!/bin/sh -x

########################################################################
# Name:		svc05:/home/cmadmin/scripts/add_lock_line.sh
# Purpose:	Lock branch in SVN permissions configuration file
# By:		Yariv Sheizaf
# Date:		28.4.2015
# Arguments:	1 - SVN Branch Name
########################################################################

BRANCH_NAME=$1
PERMS_FILE=/repos/svn/inuitive/conf/svnperms.conf
TEMPLATE_FILE=/repos/svn/inuitive/conf/template_branch

sed 's/BRANCHNAME/'$BRANCH_NAME'/' $TEMPLATE_FILE >> $PERMS_FILE

exit $?
