REM @echo off

set IDE_CMD="C:\Program Files (x86)\Microsoft Visual Studio 14.0\Common7\IDE\devenv.com"
set SCRIPT_DIR=%~dp0%

pushd .

REM set PROJECT_DIR=%SCRIPT_DIR%..\..\..\..\inu_usb_lib\prj\windows
REM cd %PROJECT_DIR%
REM @echo.
REM @echo Building inu_usb_lib
REM %IDE_CMD% "%PROJECT_DIR%\inu_usb.sln" /rebuild "Release|x64" || exit /b
REM xcopy "%PROJECT_DIR%\x64\Release\inu_usb_lib.dll" "%SCRIPT_DIR%bin\windows\x64\" /Y || exit /b
REM xcopy "%PROJECT_DIR%\x64\Release\inu_usb_test.exe" "%SCRIPT_DIR%bin\windows\x64\" /Y || exit /b
REM xcopy "%PROJECT_DIR%\x64\Release\inu_usb_wait.exe" "%SCRIPT_DIR%bin\windows\x64\" /Y || exit /b

REM %IDE_CMD% "%PROJECT_DIR%\inu_usb.sln" /rebuild "Release|Win32" || exit /b
REM xcopy "%PROJECT_DIR%\Win32\Release\inu_usb_lib.dll" "%SCRIPT_DIR%bin\windows\Win32\" /Y || exit /b
REM xcopy "%PROJECT_DIR%\Win32\Release\inu_usb_test.exe" "%SCRIPT_DIR%bin\windows\Win32\" /Y || exit /b
REM xcopy "%PROJECT_DIR%\Win32\Release\inu_usb_wait.exe" "%SCRIPT_DIR%bin\windows\Win32\" /Y || exit /b

REM @echo Building inu_storage_lib
REM set PROJECT_DIR=%SCRIPT_DIR%..\..\..\..\inu_storage_lib\prj\windows
REM cd %PROJECT_DIR%

REM %IDE_CMD% "%PROJECT_DIR%\inu_storage.sln" /rebuild "Release|x64" || exit /b
REM xcopy "%PROJECT_DIR%\x64\Release\inu_storage_lib.dll" "%SCRIPT_DIR%bin\windows\x64\" /Y || exit /b
REM xcopy "%PROJECT_DIR%\x64\Release\inu_flash.exe" "%SCRIPT_DIR%bin\windows\x64\" /Y || exit /b

REM %IDE_CMD% "%PROJECT_DIR%\inu_storage.sln" /rebuild "Release|Win32" || exit /b
REM xcopy "%PROJECT_DIR%\Win32\Release\inu_storage_lib.dll" "%SCRIPT_DIR%bin\windows\Win32\" /Y || exit /b
REM xcopy "%PROJECT_DIR%\Win32\Release\inu_flash.exe" "%SCRIPT_DIR%bin\windows\Win32\" /Y || exit /b

@echo Building inu_host_lib
set PROJECT_DIR=%SCRIPT_DIR%..\..\host\inu_host_lib\prj\windows
cd %PROJECT_DIR%

%IDE_CMD% "%PROJECT_DIR%\inu_host.sln" /rebuild "Release|x64" || exit /b
xcopy "%PROJECT_DIR%\x64\Release\inu_host_lib_4000.dll" "%SCRIPT_DIR%bin\windows\x64\" /Y || exit /b
copy "%PROJECT_DIR%\x64\Release\inu_host_test.exe" "%SCRIPT_DIR%bin\windows\x64\inu_host_test_4000.exe" /Y || exit /b

%IDE_CMD% "%PROJECT_DIR%\inu_host.sln" /rebuild "Release|Win32" || exit /b
xcopy "%PROJECT_DIR%\Win32\Release\inu_host_lib_4000.dll" "%SCRIPT_DIR%bin\windows\Win32\" /Y || exit /b
copy "%PROJECT_DIR%\Win32\Release\inu_host_test.exe" "%SCRIPT_DIR%bin\windows\Win32\inu_host_test_4000.exe" /Y || exit /b

popd


:end
