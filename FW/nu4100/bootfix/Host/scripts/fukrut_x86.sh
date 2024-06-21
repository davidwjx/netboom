#!/bin/sh

########################################################################################
# Name:		~cmadmin/scripts/fukrut_x86.sh
# Purpose:	Actions that should run using fakeroot - debian creation.
# Run by:	cmadmin@UB05
# By:		Yariv Sheizaf
# Date:		15-Aug-2016
# Arguments:
########################################################################################

cd /tmp/for_debian
chown -R root Inuitive
chgrp -R root Inuitive

for i in h dtb bin zip chm 3.0 desktop filters csv
do
	find Inuitive -name "*.$i" -exec chmod -x "{}" \;
done

find Inuitive/InuDev/docs/Depth2File -type f -exec chmod -x "{}" \;
chmod 755 Inuitive/InuDev/bin/boot*/zImage

INSTALLBUILDER_RDIR=/home/cmadmin/installbuilder-15.1.0
cd $INSTALLBUILDER_RDIR/bin
setarch i386 $INSTALLBUILDER_RDIR/bin/builder build $INSTALLBUILDER_RDIR/projects/inudev.xml deb

exit $?
