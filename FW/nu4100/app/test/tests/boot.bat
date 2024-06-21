REM @echo off
REM %1 - BIN directory
REM %2 - ADAM port
REM %3 - ADAM ip
set SCRIPT_DIR=%~dp0%
set UTILS_DIR=%SCRIPT_DIR%..\utils
set HOST_BIN_DIR="%SCRIPT_DIR%..\..\..\build\bin"
set UTILS_USB_CONNECT="%UTILS_DIR%\usb_connect\bin\Usb_connect.exe"
pushd .

cd %1\host\windows\x64
REM Reconnect the sensor 
%UTILS_USB_CONNECT% reset %2 %3 || exit /b
inu_usb_wait 30 %1\target\bootall\boot\boot0 || exit /b
REM Disable USB device
%UTILS_USB_CONNECT% off %2 %3 || exit /b
popd