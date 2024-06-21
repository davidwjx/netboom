@echo off

if "%1"== "2018_09" (
	echo ERROR: trying to use non supported version 2018_09
	goto ERROR1
	)

	
rem CALL C:\ARC_2018_12\ev_runtime\setup.bat
if NOT "%EVSS_HOME%"=="" (
	echo %EVSS_HOME%
	CALL %EVSS_HOME%\setup.bat
	GOTO CONT1
	)
CALL C:\ARC\ev_runtime\setup.bat	
echo Pay Attention - EVSS_HOME is NOT DEFINED
:CONT1


SET EVSS_CFG=nu4000b0
SET EVSS_DBG=0
SET EVSS_PROFILE=
SET NOHOSTLIB=1
SET mypath=%cd%



SET EVSS_CFG_LNK_PATH=%mypath%\arc.met
SET EVSS_CFG_LNK_PATH=%EVSS_CFG_LNK_PATH:\=/%

SET GRAPH_NAME=classify_multi_lib
IF NOT "%~2"=="" (
	SET GRAPH_NAME=%2
	)

make clean
make all -j8
color
echo Graph used: %GRAPH_NAME%


cd build_nu4000b0_release\build_nu4000b0_release
if not exist ev6x.out goto ERROR1
striparc ev6x.out
elf2bin -s0 -e0 ev6x.out cnn_boot_loader.bin
rem elf2bin -s1 -e1 ev6x.out cnn_boot_app.bin
elf2bin -s1 ev6x.out cnn_boot_app.bin
cd ..\..

@echo.
@echo.
@echo.
@echo  ########   #           #          ########     #######     #######    #######
@echo  #           #         #           #       #   #       #   #          #
@echo  #            #       #            #       #   #       #   #          #
@echo  ########      #     #             ########    #########    #######    #######
@echo  #              #   #              #           #       #           #          # 
@echo  #               # #               #           #       #           #          #  
@echo  ########         #                #           #       #    #######    #######    
@echo.

@echo.
@echo on
@GOTO:EOF
 
:ERROR1
@echo.
@echo.
@echo.
@echo  ########   #           #          ########    #######    ###   #
@echo  #           #         #           #          #       #    #    #
@echo  #            #       #            #          #       #    #    #
@echo  ########      #     #             #######    #########    #    #
@echo  #              #   #              #          #       #    #    #
@echo  #               # #               #          #       #    #    #
@echo  ########         #                #          #       #   ###   #######
@echo.
@echo.
@echo on
cd ..\..
:END

