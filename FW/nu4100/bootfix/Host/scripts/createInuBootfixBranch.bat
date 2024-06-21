REMREM ***************************
REM Usage: createInuBootfixBranch.bat <VersionName>
REM This script creates a release branch of InuBootfix at
REM  http://svn/repos/inuitive/db/NU3000/project_ver0/sw/branches/Versions/<VersionName>
REM ***************************

@echo on

if "%1"=="" goto _usage
 
set VERSION_NAME=%1
set EXIT_FLAG=0
set SVN_URL=http://svn/repos/inuitive/db/NU3000/project_ver0/sw/
set TRUNK_URL=%SVN_URL%/trunk
set VERSION_URL=%SVN_URL%/branches/Versions/%VERSION_NAME%

set MESSAGE="JIRA-ID:TEST-25 Version %VERSION_NAME%"

svn copy --parents %TRUNK_URL%/Host/scripts %VERSION_URL%/Host/scripts -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

svn copy --parents %TRUNK_URL%/FW/bootfix %VERSION_URL%/FW/bootfix -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1
svn copy --parents %TRUNK_URL%/FW/common %VERSION_URL%/FW/common -m %MESSAGE%
if not %errorlevel%==0 set EXIT_FLAG=1

exit /b %EXIT_FLAG%

:_usage
@echo off
echo Usage: createInuBootfixBranch.bat <VersionName>
exit /b 1
