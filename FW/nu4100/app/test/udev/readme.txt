On Ubuntu:
1) copy 50-persistent-usb.rules to /etc/udev/rules.d/
2) run "sudo udevadm control --reload-rules" to reload udev rules
3) enjoy libusb based code without sudo