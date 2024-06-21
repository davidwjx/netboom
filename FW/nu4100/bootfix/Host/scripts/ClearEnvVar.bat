
@echo off

rem remove INUITIVE_PATH
rem --------------------


if  DEFINED INUITIVE_PATH (
	
  	reg.exe DELETE "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment" /v INUITIVE_PATH /f	
)

