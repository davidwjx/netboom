#! /bin/bash -e
##### Host/scripts/inudev_pre_uninstall_deb.sh

if [ -d /opt/Inuitive_previous/InuSensors ]
then
	rm -rf /opt/Inuitive_previous/InuSensors
fi

if [ ! -d /opt/Inuitive_previous ]
then
	mkdir -p /opt/Inuitive_previous
fi

if [ -d /opt/Inuitive/InuDev/config/InuSensors ]
then
	mv /opt/Inuitive/InuDev/config/InuSensors /opt/Inuitive_previous
fi

for i in `find /opt/Inuitive/InuDev/bin -type d -name "boot[0-9]*"`
do
	rm -rf $i
done

exit $?
