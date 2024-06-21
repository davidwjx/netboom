@echo off

rem This file is located in the application's source directory

set HOST_BRANCH=trunk

IF "%1"=="" goto _doit
set HOST_BRANCH="%1"
:_doit

set WORK_DIRECTORY=%cd%

set SVN_PATH=http://svn/repos/inuitive/db/NU3000/project_ver0/sw

echo on

cd %WORK_DIRECTORY%

svn co --depth empty %SVN_PATH%/%HOST_BRANCH% %WORK_DIRECTORY%

svn update --set-depth empty     System
svn update --set-depth empty     System\Tools
svn update --set-depth infinity  System\Tools\PinMaster

svn update --set-depth empty     Host
svn update --set-depth empty     Host\scripts
svn update --set-depth infinity  Host\scripts\chkout_PinMaster.bat

