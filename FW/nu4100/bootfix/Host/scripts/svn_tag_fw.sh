#!/bin/bash -x

########################################################################################
# Name:		~cmadmin/scripts/svn_tag_fw.sh
# Purpose:	FTagging code tree took part in succeeded FW build on trink 
# Run by:	cmadmin@UB02
# By:		Yariv Sheizaf
# Date:		08-Jun-2015
# Arguments:
#		1 - Build Version number
#		2 - Latest Revision in SVN of used in the given build
########################################################################################

if [ "$#" != "2" ]
then
	echo ""
	echo "Syntax:  ~cmadmin/scripts/svn_tag_fw.sh <version number> <SVN revision>"
	echo "Example: ~cmadmin/scripts/svn_tag_fw.sh 1.2.8.0 63323"
	echo "Exit..."
	exit 1
fi

##################### General definitins


VERSION_NUM=$1
SVN_REVISION=$2

TAGS_RDIR=http://svn/repos/inuitive/db/NU3000/project_ver0/sw/tags/FW/builds/$VERSION_NUM

svn ls $TAGS_RDIR 1>/dev/null 2>&1
if [ "$?" = "0" ]
then
	svn delete $TAGS_RDIR -m "JID:NON-1 FW build $VERSION_NUM old tag deletion"
fi

svn copy http://svn/repos/inuitive/db/NU3000/project_ver0/sw/trunk/FW $TAGS_RDIR -m "JID:NON-1 FW Release $VERSION_NUM on revision $SVN_REVISION tag creation"
exit $?
