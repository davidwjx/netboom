@echo off

setlocal

set APP=AlgStaticTemporalFilterMain.exe

set REPEAT=20

rem TODO: remove
set APP_DIR=C:\Inuitive\work\InuSW_Trunk_git2\applications

set INPUT_PATH="%APP_DIR%\SSE2\inputs\StaticTemporalFilter_short"
set OUTPUT_DIR="%APP_DIR%\SSE2\outputs\StaticTemporalFilter_short"


:loop
if x%1 equ x goto done
:checkParam
if "%1" equ "all" goto A
if "%1" equ "compare_golden" goto B
if "%1" equ "no_output" goto C
if "%1" equ "app_help" goto D
if "%1" equ "debug" goto E
if "%1" equ "help" goto F
if "%1" equ "output_redirect" goto G
if "%1" equ "repeat" goto R
echo Parameter error: %1
exit /b

:next
shift /1
goto loop


:A
    set opt_all=x
    goto next

:B
    set opt_compare_golden=x
    goto next

:C
    set opt_no_output=x
	goto next

:D
	set opt_app_help=x
	goto next
	
:E
	set opt_debug=x
	goto next
	
:F
	echo all - run with mbAll=true
	echo compare_golden - compare results with golden
	echo no_output - do not create output files
	echo debug - ren "Debug" configuration
	echo help - this text
	echo app_help - the app's help
	echo repeat ^<n^> - set repeat value (default=%REPEAT%)
	exit /b

:G
	set opt_output_redirect=x
	goto next

:R
		shift /1
		set REPEAT=%1
		goto next
		
:done

if x%opt_debug% equ xx (
	set CONF=Debug
) else (
	set CONF=Release
)

echo Running "%CONF%" Configuration.

if x%opt_no_output% equ xx (
	set OUTPUT_PATH=
) else (
	set OUTPUT_PATH=-o %OUTPUT_DIR%
)

if x%opt_output_redirect% equ xx (
	set OUTPUT_REDIRECT="1>nul 2>&1"
) else (
	set OUTPUT_REDIRECT=
)

if x%opt_all% equ xx (
	set EXTRA_FLAGS=-A 
	set ADD_OUTPUT=_all
)

if x%opt_compare_golden% equ xx (
	set REF_PATH=-r "%APP_DIR%\SSE2\references\StaticTemporalFilter_short\0%ADD_OUTPUT%"
) else (
	set REF_PATH=
)

rem ## options:
rem ## -p - pre-read input files
rem ## -b - banchmark. Silence app output
rem ## -m - multiplicity. Golden results will change for diffrent multiplicity values
set FLAGS=-p -b -m %REPEAT% %EXTRA_FLAGS%

set PATH=.\bin\x64\%CONF%;C:\Inuitive\opencv451\build\x64\v142\bin;%APP_DIR%\SSE2\bin\x64\%CONF%;%PATH%


if x%opt_app_help% equ xx (
	%APP% -h
	exit /b
)

echo Command: %APP% %FLAGS% %REF_PATH% %OUTPUT_PATH% %INPUT_PATH% %OUTPUT_REDIRECT%

set res=0

rem ### Naive (no OpenMP, filter_worker) ###
call :doit 0 0%ADD_OUTPUT% 0 "### Naive (Not using OpenMP, filter_worker) ###"

rem ### Naive (Not using OpenMP, filter_worker1) ###
call :doit 0 0_worker1%ADD_OUTPUT% 1 "### Naive (Not using OpenMP, filter_worker1) ###"

rem ### Naive (With OpenMP, filter_worker1) ###
call :doit 0 0_worker1_openMP%ADD_OUTPUT% 1 "### Naive (With OpenMP, filter_worker1) ###" -M

rem ### Naive (Not using OpenMP, filter_worker2) ###
call :doit 0 0_worker2%ADD_OUTPUT% 2 "### Naive (Not using OpenMP, filter_worker2) ###"

rem ### Naive (With OpenMP, filter_worker2) ###
call :doit 0 0_worker2_openMP%ADD_OUTPUT% 2 "### Naive (With OpenMP, filter_worker2) ###" -M

rem ### Naive (Not using OpenMP, filter_worker3) ###
call :doit 0 0_worker3%ADD_OUTPUT% 3 "### Naive (Not using OpenMP, filter_worker3) ###"

rem ### Naive (Not using OpenMP, filter_worker3) ###
call :doit 0 0_worker3_openMP%ADD_OUTPUT% 3 "### Naive (With OpenMP, filter_worker3) ###" -M

rem ### SSE ###
call :doit 1 1%ADD_OUTPUT% 0 "### SSE ###"

rem ### AVX ###
call :doit 2 2%ADD_OUTPUT% 0 "### AVX ###"


goto :eof

rem ### doit ###
rem params:
rem - 1: algorithm
rem - 2: output dir
rem - 3: family
rem - 4: note
rem - 5: extra flags (optional)

:doit

if x%opt_no_output% equ xx (
	%APP% -a %1 %FLAGS% %5 -y %3 %REF_PATH% -n %4 %INPUT_PATH% %OUTPUT_REDIRECT%
) else (
	rmdir /s /q %OUTPUT_DIR%\%2 >nul 2>&1
	mkdir %OUTPUT_DIR%\%2
	%APP% -a %1 %FLAGS% %5 -y %3 %REF_PATH% %OUTPUT_PATH%\%2 -n %4 %INPUT_PATH% %OUTPUT_REDIRECT%
)

if errorlevel 1 (
	echo !! ERROR: Test/Reference Comparison failed.
	set res=1
)

exit /b


