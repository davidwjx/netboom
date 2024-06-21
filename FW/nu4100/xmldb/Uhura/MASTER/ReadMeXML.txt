****** MASTER ******
nu4100_boot10065.xml/sw_graph_boot10065.xml

Implement below flows:

sens_2 (top track left, VD56G3) -> SLU2 -> axi_read0 (Stream_Track_Stereo) RAW8 
sens_3 (top track right VD56G3) -> SLU3 -> axi_read1

                                                                -> ppu2 -> axi_read4  (Stream_GAZE_ROI) RAW8
sens_0 (Gaze left, VD55G0) -> SLU0 -> /* IAU1 (dms) bypassed */ -> axi_read8 (Stream_GAZE_Stereo) RAW8
sens_1 (Gaze right, VD55G0)-> SLU5 -> /* IAU0 (dms) bypassed */ -> axi_read9
                                                                -> ppu3 -> axi_read5 

sens_4 (AF VST, AR2020) -> SLU1 -> axi_read2 (Stream_VST_AF_0) RAW10 
sens_5 (AF VST, AR2020) -> SLU4 -> axi_read3 (Stream_VST_AF_1) RAW10

sens_4 (AF VST, AR2020) -> SLU1 -> axi_read2 (Stream_VST_AF_0 1920x2700 RAW10 data for scan mode @25FPS)

Stream_Imu_0

sens_2 (top track left, VD56G3)  -> SLU2 -> IAU2 (dsr) -> PPU7_0 -> CVA (Stream_Cva_0)
sens_3 (top track right, VD56G3) -> SLU3 -> IAU3 (dsr) -> PPU7_1 -> CVA

****** SandBox commands ****** 
For RAW images (1024x948 10bpp):
SandBoxFW_4100.exe -T Stream_VST_AF_0,y -T Stream_VST_AF_1,y -r 1 0 -d 30 -k 10065

For 1920x2700 RAW (10bpp):
SandBoxFW_4100.exe -T Stream_VST_AF_0,y -d 30 -k 10065

For Top Tracking and CVA (with metadata) (800x800 Left Sensor Stream):
SandBoxFW_4100.exe -T Stream_Track_Stereo,s,q -T Stream_Cva_0,q,m -d 10000 -k 10065 -p

For the Gaze ROI RAW and Stereo Gaze Raw at the same time:

SandBoxFW_4100.exe -k 10065 -a 0 -d 10000 -T Stream_GAZE_ROI,q,s -e 4 512 304 0 0 -T Stream_GAZE_Stereo,q,s

////////////////////////////////////////////////////////

nu4100_boot10065_MMS.xml/sw_graph_boot10065_MMS.xml

Provides all possible outputs from AF VST:

sens_4 (AF VST, AR2020) -> SLU1 -> IAU0 -> axi_read2 (Stream_VST_AF_0)
sens_5 (AF VST, AR2020) -> SLU4 -> IAU1 -> axi_read3 (Stream_VST_AF_1)

sens_4 (AF VST, AR2020) -> SLU1 -> axi_read8 (Stream_VST_AF_0_RAW)
sens_5 (AF VST, AR2020) -> SLU4 -> axi_read9 (Stream_VST_AF_1_RAW)

For RAW images (1024x948 10bpp):
SandBoxFW_4100.exe -T Stream_VST_AF_0_RAW,y -T Stream_VST_AF_1_RAW,y -r 1 0 -d 30 -k 10065

For RGB images (1024x948 24bpp):
SandBoxFW_4100.exe -T Stream_VST_AF_0,y -T Stream_VST_AF_1,y -r 1 0 -d 30 -k 10065

For 1920x2700 RGB (24bpp):
SandBoxFW_4100.exe -T Stream_VST_AF_0,y -d 30 -k 10065

For 1920x2700 RAW (10bpp):
SandBoxFW_4100.exe -T Stream_VST_AF_0_RAW,y -d 30 -k 10065
