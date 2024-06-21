
TOOLS_VERSION="$1"

if [[ "$TOOLS_VERSION" != "2021_12" && "$TOOLS_VERSION" != "2022_09" && "$TOOLS_VERSION" != "2022_09SP1" ]]; then
    echo "ERROR: trying to use non supported version"
	exit
fi


if  [ -z "$2" ]; then
	export GRAPH_NAME=inu_multi_lib
else 
	export GRAPH_NAME=$2
fi
	

pwd1=$(pwd)
make clean EVSS_CFG=nu4100
make install EVSS_CFG=nu4100 CNN_ABSTRACTION=unmerged_large NOHOSTLIB=1 OPENVX_DEFINES+=-DEVTH_MAX_PRIORITIES=10 EVSS_CFG_LNK_PATH=$pwd1/arc.met -j40
cd build_nu4100_release 

elfdump -z customer_app.out > disasm.txt
striparc customer_app.out

elf2bin -s1 -e1 customer_app.out cnn_boot_loader.bin
elf2bin -s2 -e4 customer_app.out cnn_boot_app.bin

cd ../
echo Graph used: $GRAPH_NAME




