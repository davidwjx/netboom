
export TOOLS_VERSION="$1"

if [[ "$TOOLS_VERSION" != "2021_12" && "$TOOLS_VERSION" != "2022_09" && "$TOOLS_VERSION" != "2022_09SP1" ]]; then
    echo "ERROR: trying to use non supported version"
	exit
fi

BUILD_PRODUCT_PATH="build_nu4100_release/" 
if  [ -z "$2" ]; then
	export GRAPH_NAME=inu_multi_lib
	export G=inu_multi_lib
else 
	export GRAPH_NAME=$2
	export G=$2
fi
	

pwd1=$(pwd)
make clean EVSS_CFG=nu4100
make install EVSS_CFG=nu4100 CNN_ABSTRACTION=unmerged_large NOHOSTLIB=1 EV_EVGENCNN_EXCEPTIONS=yes OPENVX_DEFINES+=-DEVTH_MAX_PRIORITIES=10 EVSS_CFG_LNK_PATH=$pwd1/arc.met -j40

cd $BUILD_PRODUCT_PATH 
#elfdump -z -S ev72.out > disasm.txt
elfdump -z  ev72.out > disasm.txt
striparc ev72.out

elf2bin -s1 -e1 ev72.out cnn_boot_loader.bin
elf2bin -s2 -e4 ev72.out cnn_boot_app.bin
#elf2bin -s0 -e0 ev72.out cnn_boot_loader.bin
#elf2bin -s1 -e3 ev72.out cnn_boot_app.bin

cd -


echo Graph used: $GRAPH_NAME


#!/bin/bash
file="$BUILD_PRODUCT_PATH/ev72.out"


if [ -f "$file" ]
then
    #echo "$0: File '${file}' found ***************."
	echo "                                                                                   "
	echo "                                                                                   "
	echo "  ########   #           #          ########     #######     #######    #######    "
	echo "  #           #         #           #       #   #       #   #          #           "
	echo "  #            #       #            #       #   #       #   #          #           "
	echo "  ########      #     #             ########    #########    #######    #######    "
	echo "  #              #   #              #           #       #           #          #   "
	echo "  #               # #               #           #       #           #          #   "
	echo "  ########         #                #           #       #    #######    #######    "
	echo "                                                                                   "
	echo "                                                                                   "
else 
	#echo "$0: File '${file}' not found ????????????????."
	echo "                                                                          "
	echo "                                                                          "
	echo " ########   #           #          ########    #######    ###   #         "
	echo " #           #         #           #          #       #    #    #         "
	echo " #            #       #            #          #       #    #    #         "
	echo " ########      #     #             #######    #########    #    #         "
	echo " #              #   #              #          #       #    #    #         "
	echo " #               # #               #          #       #    #    #         "
	echo " ########         #                #          #       #   ###   #######   "
	echo "                                                                          "
	echo "                                                                          "
fi
