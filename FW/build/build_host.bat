REM @echo off

set SCRIPT_DIR=%~dp0%
set BIN_DIR="%SCRIPT_DIR%bin"
set PROJECT_DIR="%SCRIPT_DIR%..\app\build\host\"
pushd .

rd %BIN_DIR%\host /s /q
mkdir %BIN_DIR%\host
cd %PROJECT_DIR%
call build.bat || exit /b
xcopy "bin\*" "%BIN_DIR%\host\" /S /I /Y || exit /b
popd
:end
