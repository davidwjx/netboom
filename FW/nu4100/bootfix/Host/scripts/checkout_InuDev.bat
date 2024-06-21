@echo off
rem This file is located in the applications's source directory


set HOST_BRANCH=trunk
set FW_BRANCH=trunk
set ALGO_BRANCH=trunk
set COMMON_BRANCH=trunk

IF "%1"=="" goto _doit

echo "1"
set HOST_BRANCH="%1"
set FW_BRANCH="%1"
set ALGO_BRANCH="%1"
set COMMON_BRANCH="%1"

IF "%2"=="" goto _doit

set FW_BRANCH="%2"
set ALGO_BRANCH="%2"
set COMMON_BRANCH="%2"

IF "%3"=="" goto _doit

set ALGO_BRANCH="%3"
set COMMON_BRANCH="%3"

IF "%4"=="" goto _doit

set COMMON_BRANCH="%4"

:_doit

set WORK_DIRECTORY=.

set SVN_PATH=http://svn/repos/inuitive/db/NU3000/project_ver0/sw

cd %WORK_DIRECTORY%

svn co --depth empty %SVN_PATH%/%FW_BRANCH%/FW %WORK_DIRECTORY%\FW
svn update --set-depth empty     FW/app
svn update --set-depth infinity  FW/app/api
svn update --set-depth infinity  FW/app/build
svn update --set-depth infinity  FW/app/common
svn update --set-depth infinity  FW/app/host

svn co --depth empty %SVN_PATH%/%HOST_BRANCH%/Host %WORK_DIRECTORY%\Host
svn update --set-depth empty     Host\dev
svn update --set-depth infinity  Host\scripts
svn update --set-depth infinity  Host\dev\3rdParty
svn update --set-depth infinity  Host\dev\InuDev

svn update --set-depth empty     Host\dev\Tools
svn update --set-depth infinity  Host\dev\Tools\InuView
svn update --set-depth infinity  Host\dev\3rdParty

svn co --depth empty %SVN_PATH%/%ALGO_BRANCH%/Algo %WORK_DIRECTORY%\Algo
svn update --set-depth empty     Algo\C++
svn update --set-depth infinity  Algo\C++\Common
svn update --set-depth infinity  Algo\C++\haarcascades
svn update --set-depth infinity  Algo\C++\DeployKit
svn update --set-depth infinity  Algo\C++\Tools

svn update --set-depth infinity  Algo\C++\CSE



svn co --depth empty %SVN_PATH%/%COMMON_BRANCH%/Common %WORK_DIRECTORY%\Common
svn update --set-depth infinity  Common
