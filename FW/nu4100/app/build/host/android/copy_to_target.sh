#!/bin/bash

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
BIN_DIR=$SCRIPT_DIR/../bin/android_ndk10d_armeabi-v7a

adb shell "su -c rm -rf /sdcard/inu_host"
adb shell "su -c mkdir -p /sdcard/inu_host"
find $BIN_DIR/* -exec adb push {} /sdcard/inu_host \;

adb push $SCRIPT_DIR/target_install.sh /sdcard/inu_host
adb shell "su -c sh /sdcard/inu_host/target_install.sh"