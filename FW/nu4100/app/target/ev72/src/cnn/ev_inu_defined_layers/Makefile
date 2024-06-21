
ifndef EV_CNNSDK_HOME
    $(error V_CNNSDK_HOME is not defined, please set point it to <your EV CNN SDK installation> directory)
endif

include $(EV_CNNSDK_HOME)/config/env.mk
PREREQUISITES_COMMAND=+$(MAKE) -C $(EV_CNNSDK_HOME) evlayers-prerequisites

# Common rules and configuration
include $(EVSS_RT_HOME)/config/config.mk
CMAKE_ARGS += -DCMAKE_MODULE_PATH=$(EV_CNNSDK_HOME)/config/cmake/Modules
