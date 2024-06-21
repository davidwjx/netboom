
@echo off

rem Define INUITIVE_PATH
rem --------------------

echo "Start Installation" > InstallLog.txt

set "tmp_path=%path%"

if  NOT DEFINED INUITIVE_PATH (

    echo "INUITIVE_PATH is not defined"  >> InstallLog.txt
	
    setx INUITIVE_PATH %1 /m

    rem Add to Registry (otherwise it will take affect only after restart)
    reg.exe ADD "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v INUITIVE_PATH /t REG_SZ /d %1 /f
 
    rem add INUITIVE_PATH into PATH
    rem ---------------------------

    where /q InuService.dll   

    if ERRORLEVEL 1 (
	    echo "Trying to add INUITIVE_PATH into PATH"  >> InstallLog.txt
	    set "tmp_path=%path%;%~1\bin"
        set path "%path%;%~1\bin" /m  >> InstallLog.txt
    	reg.exe ADD "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path /t REG_EXPAND_SZ /d "%path%;%~1\bin" /f 
    ) else (
         echo "INUITIVE_PATH is already in path" >> InstallLog.txt
    )


) else (
    echo "INUITIVE_PATH is already defined"  >> InstallLog.txt

    rem add INUITIVE_PATH into PATH
    rem ---------------------------

    where /q InuService.dll    
    if ERRORLEVEL 1 (
        set "tmp_path=%path%;%INUITIVE_PATH%\bin" 
        set path "%path%;%INUITIVE_PATH%\bin" /m  >> InstallLog.txt
    	reg.exe ADD "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path /t REG_EXPAND_SZ /d "%path%;%INUITIVE_PATH%\bin" /f
    ) else (
         echo "INUITIVE_PATH is already in path" >> InstallLog.txt
    )
)



rem Define OPENCV_DIR
rem --------------------

IF NOT DEFINED OPENCV_DIR (

    echo "OPENCV_DIR is not defined"  >> InstallLog.txt

    setx OPENCV_DIR %2 /m

    rem Add to Registry (otherwise it will take affect only after restart)
    reg.exe ADD "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v %2 /t REG_SZ /d %1 /f

    rem add OPENCV_DIR into PATH
    rem ---------------------------

    where /q opencv_core245.dll   

    if ERRORLEVEL 1 (
        set path "%tmp_path%;%~2\build\x86\vc10\bin" /m >> InstallLog.txt
    	reg.exe ADD "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path /t REG_EXPAND_SZ /d "%tmp_path%;%~2\build\x86\vc10\bin" /f
    ) else (
         echo "OPENCV_DIR is already in path" >> InstallLog.txt
    )

) else (
    echo "OPENCV_DIR is already defined"  >> InstallLog.txt


    rem add OPENCV_DIR into PATH
    rem ---------------------------

    where /q opencv_core245.dll    
    if ERRORLEVEL 1 (
        set path "%tmp_path%;%OPENCV_DIR%\build\x86\vc10\bin" /m >> InstallLog.txt
        reg.exe ADD "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v Path /t REG_EXPAND_SZ /d "%tmp_path%;%OPENCV_DIR%\build\x86\vc10\bin" /f
    ) else (
        echo "OPENCV_DIR is already in path" >> InstallLog.txt
    )
)
