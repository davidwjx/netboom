@echo off

if "%1"!= "2022_09" (
	echo ERROR: trying to use non supported version
	goto ERROR1
	)

	
rem CALL C:\ARC_2022_09\ev_runtime\setup.bat
if NOT "%EVSS_HOME%"=="" (
	echo %EVSS_HOME%
	CALL %EVSS_HOME%\setup.bat
	GOTO CONT1
	)

:CONT1


SET EVSS_CFG=nu4100
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


cd build_nu4100_release
if not exist customer_app.out goto ERROR1
striparc customer_app.out
elf2bin -s1 -e1 customer_app.out cnn_boot_loader.bin
elf2bin -s2 -e4 customer_app.out cnn_boot_app.bin
cd ..

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
cd ..
:END

