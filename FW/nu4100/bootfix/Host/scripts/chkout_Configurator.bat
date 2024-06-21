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

svn update --set-depth empty     Host
svn update --set-depth empty     Host\dev
svn update --set-depth empty     Host\dev\Tools
svn update --set-depth empty     Host\dev\Tools\InuCalibration
svn update --set-depth empty     Host\dev\Tools\InuCalibration\InuCalibration

svn update --set-depth infinity  Host\dev\Tools\InuCalibration\InuCalibration\Version.cs

svn update --set-depth empty     Host\scripts
svn update --set-depth infinity  Host\scripts\chkout_Configurator.bat

svn update --set-depth empty     System
svn update --set-depth empty     System\Code
svn update --set-depth empty     System\Code\C#
svn update --set-depth infinity  System\Code\C#\GuiKontrols

svn update --set-depth empty     System\Tools
svn update --set-depth infinity  System\Tools\Configurator

svn update --set-depth empty     Common
svn update --set-depth empty     Common\src
svn update --set-depth empty     Common\src\InuSensorsDB

svn update --set-depth infinity  Common\src\InuSensorsDB\InuModelDB.csv



