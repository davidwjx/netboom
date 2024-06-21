REM @echo off

set SCRIPT_DIR=%~dp0%
set BIN_DIR="%SCRIPT_DIR%bin"
set PROJECT_DIR="%SCRIPT_DIR%..\app\build\ceva\"
set WORKING_DIR=%CD%
mkdir "%BIN_DIR%\target"

if "%~1" == "" goto default_platform
set PLATFORM=%1
if "%PLATFORM%"=="boot0" goto m3_platform
if "%PLATFORM%"=="boot20" goto m3_platform
goto other_platforms

:default_platform
set PLATFORM="boot0"

:m3_platform
mkdir "%BIN_DIR%\target\%PLATFORM%"
cd %PROJECT_DIR%
call build.bat || exit /b
mkdir "%BIN_DIR%\target\%PLATFORM%\ceva"
mkdir "%BIN_DIR%\target\%PLATFORM%\ceva\Release"
mkdir "%BIN_DIR%\target\%PLATFORM%\ceva\Debug"
xcopy "bin\CEVA_A_RELEASE\*" "%BIN_DIR%\target\%PLATFORM%\ceva\Release" /Y || exit /b
xcopy "bin\CEVA_A_DEBUG\*" "%BIN_DIR%\target\%PLATFORM%\ceva\Debug" /Y || exit /b
cd %WORKING_DIR%

:other_platforms
mkdir "%BIN_DIR%\target\%PLATFORM%"
IF "%1"=="boot100" goto client
IF "%1"=="boot300" goto client
goto other_platforms1

:client
echo %PLATFORM%
IF NOT EXIST "%SCRIPT_DIR%..\client\%PLATFORM%\ceva\ceva_proj" goto other_platforms1
cd "%SCRIPT_DIR%..\client\%PLATFORM%\ceva\ceva_proj"
@echo.
@echo Building ceva debug
make CEVA=A DEBUG=1 clean build || exit /b
@echo Building ceva release
make CEVA=A RELEASE=1 clean build || exit /b
@echo Copying files
mkdir "%BIN_DIR%\target\%PLATFORM%\client\ceva\Debug"
mkdir "%BIN_DIR%\target\%PLATFORM%\client\ceva\Release"
xcopy "CEVA_A_DEBUG\inu_app_A.cva" "%BIN_DIR%\target\%PLATFORM%\client\ceva\Debug" /Y || exit /b
xcopy "CEVA_A_RELEASE\inu_app_A.cva" "%BIN_DIR%\target\%PLATFORM%\client\ceva\Release" /Y || exit /b

:other_platforms1

cd %WORKING_DIR%
:end
