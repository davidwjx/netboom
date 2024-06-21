#!/bin/bash

# the first input parameter: the directory of the graph
# thr second input parameter is: clean for rebuild, None for just make.
source $EV_CNNSDK_HOME/../setup.sh
source $EV_CNNSDK_HOME/../setup.sh

cd $1
echo Start building graph: $1

if [ "$2" ]; then
	make $2 EVSS_CFG=nu4100 CNN_ABSTRACTION=unmerged_large NOHOSTLIB=1 EV_EVGENCNN_EXCEPTIONS=yes
fi
make -j20 install EVSS_CFG=nu4100 CNN_ABSTRACTION=unmerged_large NOHOSTLIB=1 EV_EVGENCNN_EXCEPTIONS=yes
#make $2 install EVSS_CFG=nu4100 CNN_ABSTRACTION=unmerged_large NOHOSTLIB=1 EV_EVGENCNN_EXCEPTIONS=yes 


cd - 


echo Finished building graph: $1