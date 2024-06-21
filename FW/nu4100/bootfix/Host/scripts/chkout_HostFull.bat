@echo off

rem This file is located in the applications's source directory


set WORK_DIRECTORY=trunk
set BRANCH=trunk

IF "%1"=="" goto _doit

echo "1"
set WORK_DIRECTORY="%1"

IF "%2"=="" goto _doit

echo "2"
set WORK_DIRECTORY="%1"
set BRANCH="%2"

:_doit


set SVN_PATH=http://svn/repos/inuitive/db/NU3000/project_ver0/sw

echo on

svn co --depth empty %SVN_PATH%/%BRANCH% %WORK_DIRECTORY%

svn update --set-depth empty  %BRANCH%

svn update --set-depth infinity  %BRANCH%\Common

svn update --set-depth empty     %BRANCH%\FW
svn update --set-depth empty     %BRANCH%\FW\app
svn update --set-depth infinity  %BRANCH%\FW\app\api
svn update --set-depth infinity  %BRANCH%\FW\app\build
svn update --set-depth infinity  %BRANCH%\FW\app\common
svn update --set-depth infinity  %BRANCH%\FW\app\host

svn update --set-depth empty     %BRANCH%\Algo
svn update --set-depth empty     %BRANCH%\Algo\C++
svn update --set-depth infinity  %BRANCH%\Algo\C++\Common
svn update --set-depth infinity  %BRANCH%\Algo\C++\CSE
svn update --set-depth infinity  %BRANCH%\Algo\C++\haarcascades
svn update --set-depth infinity  %BRANCH%\Algo\C++\DeployKit
svn update --set-depth infinity  %BRANCH%\Algo\C++\Tools

svn update --set-depth infinity  %BRANCH%\Host








