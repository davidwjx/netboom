****** SLAVE ******
nu4100_boot10065.xml/sw_graph_boot10065.xml

Implement below flows:

sens_0 (bottom track left, VD56G3)  -> SLU2 -> axi_read0 (Stream_Track_Stereo) RAW8
sens_1 (bottom track right, VD56G3) -> SLU3 -> axi_read1 

sens_3 (VST_FF right, OS05A10) -> SLU5 -> /*IAU0 (dms) bypassed*/ -> axi_read9 (Stream_VST_FF_Stereo) RAW10
sens_2 (VST_FF left, OS05A10)  -> SLU0 -> /*IAU1 (dms) bypassed*/ -> axi_read8 

sens_4 (AF VST downsaled and no VC configure, AR2020)	-> SLU1 -> axi_read2 (Stream_VST_AF_0) RAW10
sens_5 (AF VST Cropped, AR2020)  						-> SLU4 -> axi_read3 (Stream_VST_AF_1) RAW10

sens_0 (bottom track left, VD56G3)  -> SLU2 -> IAU2 (dsr) -> PPU7_0 -> CVA (Stream_Cva_0)
sens_1 (bottom track right, VD56G3) -> SLU3 -> IAU3 (dsr) -> PPU7_1 -> CVA 

****** SandBox commands ****** 
For Top Tracking and CVA (800x800 Left Sensor Stream):
SandBoxFW_4100.exe -T Stream_Track_Stereo,s,q -T Stream_Cva_0,q -r 0 0 -d 10000 -k 10065 -p

Activate AF streams with run time commands:
SandBoxFW_4100.exe -T Stream_VST_AF_0,s -T Stream_VST_AF_1,s -r 1 0 -d 3000 -k 10065 -C

Access register of AF lens (PD50LE):
Write command example:
W 2 0x18 1 0x2 1 0x81
W 2 0x18 1 0x3 1 0xea

Read command example:
R 2 0x18 1 0x2 1
R 2 0x18 1 0x3 1
