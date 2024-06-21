inu_flash.exe -T 12000000 -J bootfix-c1-4_1_0_17.bin
inu_flash.exe -T 12000000 -J bootfix-c1-4_1_1_2.bin
inu_flash.exe -T 12000000 -J bootfix-nu4100-4_1_1_8.bin
inu_flash.exe -T 12000000 -J bootfix-nu4100-4_1_1_11.bin // Updated June 08 2022
inu_flash.exe -T 12000000 -J bootfix-nu4100-4_1_1_14.bin
inu_flash.exe -T 12000000 -J bootfix-nu4100-4_1_1_18.bin

inu_flash.exe -W 0x7d0000 0x12345678
inu_flash.exe -P 0x7d0000

inu_flash.exe -Q 0x0 0x800000   //erase


inu_flash.exe -T 80000000 0 1 2 -P 0
param1=8000000-freq, param2 =0 always put 0, param3=1 spiNumber ,param4=2 chipselect, the command is -P 0
inu_flash.exe -T 12000000 0 0 3 -W 0x0 0x12345678
inu_flash.exe -T 12000000 0 0 3 -P 0x0
inu_flash.exe -T 12000000 0 0 3 -Q 0x0 0x800000

Production Data:
inu_flash.exe -E 87
inu_flash.exe -B SI1
inu_flash.exe -C 110-0265-01
inu_flash.exe -D 00

Read small flash data - Laptop:
inu_flash.exe -T 20000000 0 1 0 -Z 3 save
Save Calibration data - Laptop:
inu_flash.exe -T 20000000 0 1 0 -G C:\YYYY\ZZZZ\SECTION_CALIBRATION_DATA