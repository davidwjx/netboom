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

set WORK_DIRECTORY=%cd%

set SVN_PATH=http://svn/repos/inuitive/db/NU3000/project_ver0/sw

echo on

cd %WORK_DIRECTORY%

svn co --depth empty %SVN_PATH%/%FW_BRANCH% %WORK_DIRECTORY%

rem ######################## FW ###############################

svn update --set-depth empty     FW
svn update --set-depth empty     FW\app
svn update --set-depth infinity  FW\app\api
svn update --set-depth infinity  FW\app\build
svn update --set-depth infinity  FW\app\common
svn update --set-depth infinity  FW\app\host
svn update --set-depth empty     FW\common
svn update --set-depth infinity  FW\common\include


rem ######################## Host ###############################

svn update --set-depth empty     Host
svn update --set-depth infinity  Host\scripts

svn update --set-depth infinity  Host\doc

svn update --set-depth empty     Host\dev
svn update --set-depth empty     Host\dev\InuDev
svn update --set-depth infinity  Host\dev\InuDev\include
svn update --set-depth empty     Host\dev\InuDev\src

svn update --set-depth infinity  Host\dev\InuDev\src\InuService
svn update --set-depth infinity  Host\dev\InuDev\src\InuStreams
svn update --set-depth infinity  Host\dev\InuDev\src\InuStreamsNet
svn update --set-depth infinity  Host\dev\InuDev\src\InuStreamsNetExt
svn update --set-depth infinity  Host\dev\InuDev\src\InuDevUtilities
svn update --set-depth infinity  Host\dev\InuDev\src\PropertySheets
svn update                       Host\dev\InuDev\src\InuDev.sln

svn update --set-depth empty     Host\dev\Tools
svn update --set-depth infinity  Host\dev\Tools\InuView

svn update --set-depth empty     Host\dev\3rdParty
svn update --set-depth infinity  Host\dev\3rdParty\3DTools
svn update --set-depth infinity  Host\dev\3rdParty\EMotionFX
svn update --set-depth infinity  Host\dev\3rdParty\ensenso
svn update --set-depth infinity  Host\dev\3rdParty\InsertIcons
svn update --set-depth infinity  Host\dev\3rdParty\libtiff.net_bin-2.3.641.0
svn update --set-depth infinity  Host\dev\3rdParty\Renci.SshNet
svn update --set-depth infinity  Host\dev\3rdParty\Triclops
svn update --set-depth infinity  Host\dev\3rdParty\IniFileParser

svn update --set-depth empty     Host\dev\3rdParty\OpenTK
svn update --set-depth empty     Host\dev\3rdParty\OpenTK\Binaries
svn update --set-depth empty     Host\dev\3rdParty\OpenTK\Binaries\OpenTK
svn update --set-depth infinity  Host\dev\3rdParty\OpenTK\Binaries\OpenTK\Release
svn update --set-depth infinity  Host\dev\3rdParty\OpenTK\Binaries\OpenTK\Debug

rem ######################## Algo ###############################

svn update --set-depth empty     Algo
svn update --set-depth empty     Algo\C++
svn update --set-depth infinity  Algo\C++\Common
svn update --set-depth infinity  Algo\C++\haarcascades
svn update --set-depth infinity  Algo\C++\Tools
svn update --set-depth empty     Algo\C++\PatternProcessing
svn update --set-depth infinity  Algo\C++\PatternProcessing\PatternProcess

svn update --set-depth empty     Algo\C++\CSE
svn update --set-depth infinity  Algo\C++\CSE\AlgDepthImprovements
svn update --set-depth infinity  Algo\C++\CSE\AlgDPERegTune
svn update --set-depth infinity  Algo\C++\CSE\AlgFaceRecog
svn update --set-depth infinity  Algo\C++\CSE\AlgGazeTracking
svn update --set-depth infinity  Algo\C++\CSE\AlgHands
svn update --set-depth infinity  Algo\C++\CSE\AlgHeadTracking
svn update --set-depth infinity  Algo\C++\CSE\HostAlgGazeCalc
svn update --set-depth infinity  Algo\C++\CSE\NU3000CVESim
svn update --set-depth infinity  Algo\C++\CSE\AlgWebcamIntegration

svn update --set-depth empty     Algo\C++\CSE\3rdParty
svn update --set-depth infinity  Algo\C++\CSE\3rdParty\ve
svn update --set-depth infinity  Algo\C++\CSE\3rdParty\il
svn update --set-depth infinity  Algo\C++\CSE\3rdParty\Intraface
svn update --set-depth infinity  Algo\C++\CSE\Utilities

svn update --set-depth empty     Algo\C++\CSE\NU3000Sim
svn update --set-depth infinity  Algo\C++\CSE\NU3000Sim\Global

svn update                       Algo\C++\CSE\post_build.bat

svn update --set-depth empty     Algo\C++\CSE\CSE_CM.sln

rem ######################## Common ###############################

svn update --set-depth infinity  Common






