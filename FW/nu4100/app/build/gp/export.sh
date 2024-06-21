#!/bin/bash
#########################################################################################
# This script creates an export tree for the gp					        #
# 											#
#########################################################################################
set -e
set -x
BUILD_DIR="$(dirname "$(readlink -f "$0")")"
FW_PATH=$BUILD_DIR/../../../app
COMMON_PATH=$BUILD_DIR/../../../../common
CLIENT_PATH=$BUILD_DIR/../../../client_demo
FW_PATH_EXPORT=$BUILD_DIR/../../../app_export

dirlist=()
dirlist+='common/infrastructure/cls/ '
dirlist+='target/gp/src/hw/hw_mngr/ '	
dirlist+='target/gp/src/hw/cde_mngr/ '	
dirlist+='target/gp/src/hw/dpe_mngr/ '	
dirlist+='target/gp/src/hw/gme_mngr/ '	
dirlist+='target/gp/src/hw/hw_regs/ '	
dirlist+='target/gp/src/hw/int_mngr/ '	
dirlist+='target/gp/src/hw/iae_mngr/ '	
dirlist+='target/gp/src/hw/mipi_mngr/ '	
dirlist+='target/gp/src/hw/sensors_mngr/ '	
dirlist+='target/gp/src/main/ '	
dirlist+='target/gp/src/model/ '	
dirlist+='target/gp/src/wd/ '	
dirlist+='target/gp/src/alg_ctrl/ '	
dirlist+='target/gp/src/alt/ '	
dirlist+='target/gp/src/ccf/ '	
dirlist+='target/gp/src/alg_ctrl/ '	
dirlist+='target/common/bsp/img_sensors/ '
dirlist+='target/common/bsp/peripheral/ '	
dirlist+='target/common/bsp/i2c/ '		
dirlist+='target/common/bsp/spi/ '		
dirlist+='target/common/bsp/cde/ ' 
dirlist+='target/common/bsp/dpe/ ' 
dirlist+='target/common/bsp/gme/ ' 
dirlist+='target/common/bsp/gpio/ ' 
dirlist+='target/common/bsp/iae/ ' 
dirlist+='target/common/bsp/mipi/ ' 
dirlist+='target/common/alg/sensor_control/ ' 
dirlist+='common/application/service/alg/ ' 
dirlist+='common/application/service/client/ ' 
dirlist+='common/application/service/depth/ ' 
dirlist+='common/application/service/inject/ ' 
dirlist+='common/application/service/logger/ ' 
dirlist+='common/application/service/pos_sensors/ ' 
dirlist+='common/application/service/svc_mngr/ ' 
dirlist+='common/application/service/system/ ' 
dirlist+='common/application/service/video/ ' 
dirlist+='common/application/service/webcam/ ' 
dirlist+='target/common/alg/sensor_control/src/ ' 
dirlist+='target/common/mem/ ' 
dirlist+='target/common/meta/ ' 
dirlist+='target/common/pal/ ' 
dirlist+='target/common/utils/stack/ ' 
dirlist+='target/common/alg/sensor_control/src/ ' 
dirlist+='target/gp/src/icc/src/ ' 
dirlist+='common/api/src/ ' 
dirlist+='common/infrastructure/os/src/ '
dirlist+='common/infrastructure/inc/ '
dirlist+='common/infrastructure/marshal/ '
dirlist+='common/infrastructure/service/ '
dirlist+='common/infrastructure/stream/ '
dirlist+='common/infrastructure/utils/ '
dirlist+='host/ ' 

cp -r $FW_PATH $FW_PATH_EXPORT
mkdir -p export/nu4000
cp -r $FW_PATH_EXPORT export/nu4000/app
cp -r $COMMON_PATH export/.
cp -r $CLIENT_PATH export/nu4000/.

for item in $dirlist; do
	find export/nu4000/app/$item -name '*.c' -delete
done

cp gp_export/makelogic 	export/nu4000/app/target/gp/prj/make/Makefile
cd export/nu4000/app/target/gp/prj/make
make
mv libinu_target_logic.a ../.
make clean
mv ../libinu_target_logic.a .
cd -

find export/nu4000/app/ -name '*.c' -delete
for item in $dirlist; do
	cp -r $FW_PATH_EXPORT/$item/* export/nu4000/app/$item.
done

rm -rf $FW_PATH_EXPORT
cp gp_export/makeinfra	export/nu4000/app/target/gp/prj/make/Makefile
cd -

cd $FW_PATH/build/gp
tar -cvzf export.tar.gz export
rm -rf export
