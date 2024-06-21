#!/bin/sh

#set -x
# load USB pg modules
source /init.usb.pg

cd /media/inuitive

# Launch Inuitive Target process 
if [ -e "inu_target.out" ]; then
	chmod +x inu_target.out
	echo "Invoking inu_target.out"
	if [ -e "vvcam_isp.ko" ]; then
		echo "Loading ISP VVCAM Kernel module"
		insmod /media/inuitive/vvcam_isp.ko
	fi
	module="pg"
	device="pg"
	mode="664"
	major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)
	[ ! -z  "$major" ] && mknod /dev/${device}4 c $major 4 && chmod $mode  /dev/${device}4
	[ ! -z  "$major" ] && mknod /dev/${device}5 c $major 5 && chmod $mode  /dev/${device}5
	 echo "5" > /proc/sys/kernel/printk 
	 export ISP_LOG_LEVEL=1
	./inu_target.out -w 3 -t 0
fi
