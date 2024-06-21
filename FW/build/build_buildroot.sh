CURRENT_DIR=$(pwd)
INU=/mnt/c/Program\ Files/Inuitive/InuDev/bin/NU4100/boot0
BUILDROOT=~/Projects/Buildroot_NU4k
export PATH=$/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
./build_target.sh -d -a -r nu4100
sleep 1
mkdir -p $BUILDROOT/output/target/media/inuitive
rm $BUILDROOT/output/target/media/*
rm $BUILDROOT/output/target/media/inuitive/*
cp bin/nu4100/target/boot0/targetfs/* $BUILDROOT/output/target/media/inuitive
chmod +x bin/nu4100/target/boot0/gp/inu_target.out
cp -v bin/nu4100/target/boot0/gp/inu_target.out $BUILDROOT/output/target/media/inuitive
cd $BUILDROOT
make -j 12
cp output/images/rootfs.cpio.gz "$INU"/initramfs.gz