set -x 
# Make the system partition writable
mount -o remount,rw /system
# Install libraries
cp /sdcard/inu_host/*.so /system/lib/
cp /sdcard/inu_host/inu_*_test /system/bin/
chmod 0755 /system/bin/inu_*_test

# Make the system partition read only again
mount -o remount,ro /system