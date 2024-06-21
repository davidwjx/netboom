
****** Overview ****** 
nu4100_boot10065.xml/sw_graph_boot10065.xml
This folder contains the hardware graph for the Luxor slave configured to run the slave tracking sensors in a mode where they are triggered based off the trigger from the master tracking sensors
The graph is the same as the normal main graph but the FSG is configured so that it's triggered by the Tracking_Sync signal from the master.

This main graph is a temporary workaround until the sensor sync API is integrated into the SDK. 

Implements below flows:

sens_3 (top track right, VG563) -> SLU3 -> axi_read0 (Stream_Track_Stereo)
sens_2 (top track left, VG563)  -> SLU2 -> axi_read1

sens_4 (AF VST downsaled, AR2020)	-> SLU1 -> axi_read2 (Stream_VST_AF_0) RAW10
sens_5 (AF VST Cropped, AR2020)  	-> SLU4 -> axi_read3 (Stream_VST_AF_1) RAW10

sens_2 (top track left, VD56G3)  -> SLU2 -> IAU2 (dsr) -> PPU7_0 -> CVA (Stream_Cva_0)

****** SandBox commands ****** 
Start the Luxor slave first using the nu4100_boot10065.xml located in this folder:

SandBoxFW_4100.exe -T Stream_Track_Stereo,q,s,m -d 30 -k 10065

Then start the Luxor master:

SandBoxFW_4100.exe -T Stream_Track_Stereo,q,s,m -d 30 -k 10065

Other commands:

SandBoxFW_4100.exe -T Stream_Track_Stereo,q -d 30 -k 10065
SandBoxFW_4100.exe -T Stream_VST_AF_0,q -T Stream_VST_AF_1,q -r 1 3 -d 30 -k 10065


****** ISP Streaming commands ****** 
To Stream VST_AF_0 and Stream_VST_AF1 through the ISP please use the zImage provided within the 4.21 SDK release
Known issues: 
AWB Coefficients cause a pink hue so I have disabled AWB in the Hardware XML
High CPU usage with AE and AWB enabled
Testing still needed on the slave

./inu_sandbox -k 10065 -T Stream_mp_isp0,q,s -T Stream_mp_isp1,q,s -r 1 3 -d 1000 -p 

