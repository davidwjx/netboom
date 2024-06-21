if  [ -z "$1" ]; then
	export inu_path=$(pwd)/../../../libs/ev
else 
	export inu_path=$1
fi

echo "Copying files and folders from ${inu_path} to cnn_tools/install:"
echo "copying $inu_path/libinu_infra.a to ${EV_CNNSDK_HOME}/install/nu4100_release/lib"
cp ${inu_path}/libinu_infra.a ${EV_CNNSDK_HOME}/install/nu4100_release/lib
echo "copying $inu_path/snps_cnn_lib to ${EV_CNNSDK_HOME}/install/nu4100_release/lib/cmake"
cp -r ${inu_path}/snps_cnn_lib ${EV_CNNSDK_HOME}/install/nu4100_release/lib/cmake
echo "copying $inu_path/inu_infra to ${EV_CNNSDK_HOME}/install/nu4100_release/lib/cmake"
cp -r ${inu_path}/inu_infra ${EV_CNNSDK_HOME}/install/nu4100_release/lib/cmake
echo "copying $inu_path/common to ${EV_CNNSDK_HOME}/install/nu4100_release/include"
cp -r ${inu_path}/common ${EV_CNNSDK_HOME}/install/nu4100_release/include
echo "copying $inu_path/client_api.h to ${EV_CNNSDK_HOME}/install/nu4100_release/include"
cp  ${inu_path}/client_api.h ${EV_CNNSDK_HOME}/install/nu4100_release/include







