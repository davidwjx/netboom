****** LUXOR MASTER ******
nu4100_boot10065.xml/sw_graph_boot10065.xml

Implements below flows:

sens_3 (top track right, VG563) -> SLU3 -> axi_read0 (Stream_Track_Stereo)
sens_2 (top track left, VG563)  -> SLU2 -> axi_read1

                                                                -> ppu2 -> axi_read4  (Stream_GAZE_ROI) RAW8
sens_0 (Gaze left, VD55G0) -> SLU0 -> /* IAU1 (dms) bypassed */ -> axi_read8 (Stream_GAZE_Stereo) RAW8
sens_1 (Gaze right, VD55G0)-> SLU5 -> /* IAU0 (dms) bypassed */ -> axi_read9
                                                                -> ppu3 -> axi_read5 

sens_4 (AF VST, AR2020) -> SLU1 -> axi_read2 (Stream_VST_AF_0) RAW10 
sens_5 (AF VST, AR2020) -> SLU4 -> axi_read3 (Stream_VST_AF_1) RAW10
sens_4 (AF VST, AR2020) -> SLU1 -> ISP0 Channel 0  (Stream_mp_isp0) YUV422 
sens_5 (AF VST, AR2020) -> SLU4 -> ISP1 Channel 0  (Stream_mp_isp1) YUV422


sens_2 (top track left, VD56G3)  -> SLU2 -> IAU2 (dsr) -> PPU7_0 -> CVA (Stream_Cva_0)

****** SandBox commands ****** 
SandBoxFW_4100.exe -T Stream_Track_Stereo,q -d 30 -k 10065
SandBoxFW_4100.exe -T Stream_VST_AF_0,q -T Stream_VST_AF_1,q -r 1 3 -d 30 -k 10065
SandBoxFW_4100.exe -T Stream_Imu_0,q,s -d 30 -k 10065 
SandBoxFW_4100.exe -T Stream_GAZE_ROI,r,q -e 4 512 404 0 0 -T Stream_GAZE_Stereo,q -d 30 -k 10065

****** ISP Streaming commands ****** 
To Stream VST_AF_0 and Stream_VST_AF1 through the ISP please use the zImage provided within the 4.21 SDK release
Known issues: 
AWB Coefficients cause a pink hue so I have disabled AWB in the Hardware XML
High CPU usage with AE and AWB enabled

./inu_sandbox -k 10065 -T Stream_mp_isp0,q,s -T Stream_mp_isp1,q,s -r 1 3 -d 1000 -p 
