REM @echo off
set SCRIPT_DIR=%~dp0%
set BIN_DIR="%SCRIPT_DIR%..\bin\android_ndk10d_armeabi-v7a"

REM SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
REM BIN_DIR=$SCRIPT_DIR/../bin/android_ndk10d_armeabi-v7a

adb shell "su -c rm -rf /sdcard/inu_host"
adb shell "su -c mkdir -p /sdcard/inu_host"

adb push %SCRIPT_DIR%target_install.sh /sdcard/inu_host
adb push %SCRIPT_DIR%target_run_boot.sh /sdcard/inu_host
adb push %SCRIPT_DIR%ueventd.rc /sdcard/inu_host
adb push %BIN_DIR%\libusb1.0.so /sdcard/inu_host
adb push %BIN_DIR%\libinu_usb_lib.so /sdcard/inu_host
adb push %BIN_DIR%\libinu_host_lib.so /sdcard/inu_host
adb push %BIN_DIR%\inu_usb_test /sdcard/inu_host
adb push %BIN_DIR%\inu_host_test /sdcard/inu_host

adb shell "su -c sh /sdcard/inu_host/target_install.sh"