#! /bin/bash -e
##### Host/scripts/inudev_post_install_deb.sh

rm -f /opt/Inuitive/InuDev/bin/*.0

if [ -d /opt/Inuitive/InuDev/config/InuSensors ]
then
	mv /opt/Inuitive/InuDev/config/InuSensors /opt/$$.InuSensors
fi

rm -f /opt/Inuitive

ln -fs /opt/inudev-9.99.99/Inuitive /opt/Inuitive

ln -fs /opt/Inuitive/InuDev/bin/libusb-1.0.so /opt/Inuitive/InuDev/bin/libusb-1.0.so.0

ln -fs /opt/Inuitive/InuDev/config/icons/Inuitive-Service.desktop /usr/share/applications/Inuitive-Service.desktop

ln -fs /opt/Inuitive/InuDev/config/icons/InuView.desktop /usr/share/applications/InuView.desktop

if [ ! -f /usr/share/applications/java.desktop ]
then
	ln -fs /opt/Inuitive/InuDev/config/icons/java.desktop /usr/share/applications/java.desktop
fi

if [ -d /opt/$$.InuSensors ]
then
	rmdir /opt/Inuitive/InuDev/config/InuSensors
	mv /opt/$$.InuSensors /opt/Inuitive/InuDev/config/InuSensors
else
	if [ -d /opt/Inuitive_previous/InuSensors ]
	then
		rmdir /opt/Inuitive/InuDev/config/InuSensors
		mv /opt/Inuitive_previous/InuSensors /opt/Inuitive/InuDev/config/InuSensors
	else
		mkdir -p /opt/Inuitive/InuDev/config/InuSensors
	fi
fi

cd /opt/Inuitive/InuDev
chmod 777 config
chmod 777 config/InuSensors config/icons

cd /opt/Inuitive/InuDev/config
chmod 666 *.dat *.xml *.txt
chmod 766 *.csv

cd /opt/Inuitive/InuDev/bin
./SetPermissionRT

ln -fs /opt/Inuitive/InuDev/bin/inuservice /etc/init.d/inuservice

cp /opt/Inuitive/InuDev/bin/inuservice.conf /etc/init


start inuservice


exit $?
