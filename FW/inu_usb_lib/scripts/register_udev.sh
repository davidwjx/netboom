sudo /sbin/udevd -d
skill udedv

/etc/udev/rules.d/70-persistent-net.rules
SUBSYSTEMS=="usb", ATTRS{idVendor}=="2959", ATTRS{idProduct}=="3000", SYMLINK+="nu3000", GROUP="usb", MODE="660