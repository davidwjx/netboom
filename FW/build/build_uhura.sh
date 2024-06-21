CURRENT_DIR=$(pwd)
INU=varjodev@192.168.55.1:/opt/Inuitive/InuDev/bin/NU4100/boot0
./build_target.sh -a -r nu4100 
sleep 1
scp initramfs/NU4100/initramfs.gz "$INU"
scp ../nu4100/xmldb/Luxor/SLAVE/CVA/* "$INU"
scp ../nu4100/xmldb/Luxor/MASTER/CVA/* "$INU"
scp bin/nu4100/target/boot0/zip/nu4100.zip "$INU"

