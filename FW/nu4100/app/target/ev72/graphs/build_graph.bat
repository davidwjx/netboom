@echo off

CALL %EVSS_HOME%\setup.bat
CALL %EV_CNNSDK_HOME%\setup.bat

cd %1

@echo Start building graph: %1
make install EVSS_CFG=nu4000b0 CNN_ABSTRACTION=unmerged_large export EV_EVGENCNN_EXCEPTIONS=yes
@echo Finished building graph: %1

cd ..\.. 