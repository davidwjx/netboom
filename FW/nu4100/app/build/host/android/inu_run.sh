#!/bin/bash

# boot test
#adb shell "su -c inu_usb_test /sdcard/boot"

# inu host test
adb shell "su -c inu_host_test -R V -F 20 -B /sdcard/boot -f /sdcard/boot/config.ireg -D 40 -T V"
