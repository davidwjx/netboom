#!/bin/sh

#set -x
cd ../../../target/gp/prj/make/
make -j16
ret=$(echo $?)
if [ $ret -ne 0 ]; then
    echo "ERROR"
    exit 1
fi

cd ../../../../host/test/SandBox/SandBoxFWLib/
#cd SandBoxFWLib/
make clean
make all

cd ..
make clean
make all