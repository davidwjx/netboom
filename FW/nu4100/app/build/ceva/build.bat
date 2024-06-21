@echo off

set SCRIPT_DIR=%~dp0%
set PROJECT_DIR="%SCRIPT_DIR%..\..\target\ceva\prj\make"
set WORKING_DIR=%CD%

cd %PROJECT_DIR%
@echo.
@echo Building ceva RELREASE
make  clean build || exit /b
xcopy "CEVA_RELEASE\inu_app.cva" "%SCRIPT_DIR%bin\CEVA_RELEASE" /Y || exit /b



@echo. 
@echo Building ceva DEBUG
make clean build || exit /b
@echo.
@echo Copying files
xcopy "CEVA_DEBUG\inu_app.cva" "%SCRIPT_DIR%bin\CEVA_DEBUG" /Y || exit /b

cd %WORKING_DIR%
:end
