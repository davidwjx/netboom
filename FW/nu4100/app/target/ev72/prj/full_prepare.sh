cd build_nu4000b0_release/build_nu4000b0_release 
mv ev6x.out cnn.out

striparc cnn.out
elfdump -z cnn.out > disasm.txt
elf2bin -s0 -e0 cnn.out cnn_boot_loader.bin
elf2bin -s1 cnn.out cnn_boot_app.bin
cd ../../

