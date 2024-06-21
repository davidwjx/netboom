CURRENT_DIR=$(pwd)
INU=/mnt/c/Program\ Files/Inuitive/InuDev/bin/NU4100/boot0/
./build_target.sh -a -r nu4100 
sleep 1
cp initramfs/NU4100/initramfs.gz "$INU"
cp ../nu4100/xmldb/Luxor/SLAVE/CVA/* "$INU"
cp ../nu4100/xmldb/Luxor/MASTER/CVA/* "$INU"
cp bin/nu4100/target/boot0/zip/nu4100.zip "$INU"
#Builds the EV binary as well
make -C ../nu4100/client_demo/stereo_demo_master/gp/ -B
cp ../nu4100/client_demo/stereo_demo_master/gp/inu_target.out bin/nu4100/target/boot0/targetfs
cd bin/nu4100/target/boot0/targetfs
zip -r ../zip/nu4100-EV.zip  *
cp ../zip/nu4100-EV.zip "$INU"/nu4100.zip