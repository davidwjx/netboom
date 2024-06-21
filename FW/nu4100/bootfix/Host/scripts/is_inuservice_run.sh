#!/bin/sh
IS_RUN=`ps -ef | grep InuService | grep -v grep | wc -l`
if [ "$IS_RUN" = "0" ]
then
	/etc/init.d/inuservice start
fi

exit 0
