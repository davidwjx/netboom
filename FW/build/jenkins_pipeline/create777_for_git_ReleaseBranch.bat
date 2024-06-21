REM REM ***************************
REM Usage: createReleaseBranch.bat <VersionName>
REM This script creates a release branch at
REM  http://svn/repos/inuitive/db/NU3000/project_ver0/sw/branches/Versions/<VersionName>
REM ***************************

@echo on

if "%1"=="" goto _usage
 
set VERSION_NAME=%1
set FROM_BRUNCH=%2
set REVISION=%3
set EXIT_FLAG=0
set SVN_URL=http://svn/repos/inuitive/db/NU3000/project_ver0/sw


if '%FROM_BRUNCH%' == '' set FROM_BRUNCH=trunk

if '%FROM_BRUNCH%' == 'trunk' (
	set TRUNK_URL=%SVN_URL%/trunk
) else (
	set TRUNK_URL=%SVN_URL%/branches/Versions/%FROM_BRUNCH%
) 

set VERSION_URL=%SVN_URL%/branches/Versions/%VERSION_NAME%
set MESSAGE="JIRA-ID:TEST-25 Version %VERSION_NAME%"

if '%REVISION%' == '' set SVN_REVISION=
if not '%REVISION%' == '' set SVN_REVISION=@%REVISION%
if '%REVISION%' == 'HEAD' set SVN_REVISION=

echo SVN_REVISION - %SVN_REVISION%

svn delete %VERSION_URL%/FW -m %MESSAGE%
svn copy --parents %TRUNK_URL%/FW/build%SVN_REVISION% %VERSION_URL%/FW/build -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

svn delete %VERSION_URL%/FW/common -m %MESSAGE%
svn copy --parents %TRUNK_URL%/FW/common%SVN_REVISION% %VERSION_URL%/FW -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

svn delete %VERSION_URL%/FW/nu4100 -m %MESSAGE%
svn copy --parents %TRUNK_URL%/FW/nu4100%SVN_REVISION% %VERSION_URL%/FW -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

svn delete %VERSION_URL%/FW/nu4000c0 -m %MESSAGE%
svn copy --parents %TRUNK_URL%/FW/nu4000c0%SVN_REVISION% %VERSION_URL%/FW -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

svn delete %VERSION_URL%/FW/kernel -m %MESSAGE%
svn copy --parents %TRUNK_URL%/FW/kernel%SVN_REVISION% %VERSION_URL%/FW -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

svn delete %VERSION_URL%/FW/inu_storage_lib -m %MESSAGE%
svn copy --parents %TRUNK_URL%/FW/inu_storage_lib%SVN_REVISION% %VERSION_URL%/FW -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

svn delete %VERSION_URL%/FW/inu_usb_lib -m %MESSAGE%
svn copy --parents %TRUNK_URL%/FW/inu_usb_lib%SVN_REVISION% %VERSION_URL%/FW -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1


svn delete %VERSION_URL%/FW -m %MESSAGE%
svn copy --parents %TRUNK_URL%/FW%SVN_REVISION% %VERSION_URL%/FW -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1



svn delete %VERSION_URL%/AIF/WINDOWS/AI_Lib -m %MESSAGE%
svn copy --parents %TRUNK_URL%/AIF/WINDOWS/AI_Lib%SVN_REVISION% %VERSION_URL%/AIF/WINDOWS/AI_Lib -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

svn delete %VERSION_URL%/AIF/WINDOWS/example_ev_multi -m %MESSAGE%
svn copy --parents %TRUNK_URL%/AIF/WINDOWS/example_ev_multi%SVN_REVISION% %VERSION_URL%/AIF/WINDOWS/example_ev_multi -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

svn delete %VERSION_URL%/AIF/WINDOWS/example_ev_ssd -m %MESSAGE%
svn copy --parents %TRUNK_URL%/AIF/WINDOWS/example_ev_ssd%SVN_REVISION% %VERSION_URL%/AIF/WINDOWS/example_ev_ssd -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

svn delete %VERSION_URL%/AIF/WINDOWS/build.mk -m %MESSAGE%
svn copy --parents %TRUNK_URL%/AIF/WINDOWS/build.mk%SVN_REVISION% %VERSION_URL%/AIF/WINDOWS/build.mk -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

svn delete %VERSION_URL%/AIF/WINDOWS/face_det -m %MESSAGE%
svn copy --parents %TRUNK_URL%/AIF/WINDOWS/face_det%SVN_REVISION% %VERSION_URL%/AIF/WINDOWS/face_det -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

svn delete %VERSION_URL%/AIF/WINDOWS/sanity_ev_multi -m %MESSAGE%
svn copy --parents %TRUNK_URL%/AIF/WINDOWS/sanity_ev_multi%SVN_REVISION% %VERSION_URL%/AIF/WINDOWS/sanity_ev_multi -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

svn delete %VERSION_URL%/AIF/WINDOWS/VS_2019/vai_bokeh_demo -m %MESSAGE%
svn copy --parents %TRUNK_URL%/AIF/WINDOWS/VS_2019/vai_bokeh_demo%SVN_REVISION% %VERSION_URL%/AIF/WINDOWS/VS_2019/vai_bokeh_demo -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

svn delete %VERSION_URL%/Host/dev/InuDev/include/Version.h -m %MESSAGE%
svn copy --parents %TRUNK_URL%/Host/dev/InuDev/include/Version.h%SVN_REVISION% %VERSION_URL%/Host/dev/InuDev/include/Version.h -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

exit /b %EXIT_FLAG%

:_usage
@echo off
echo Usage: chkoutReleaseBranch.bat <VersionName> [revisionNumber]
exit /b 1
