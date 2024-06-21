This XML allows for the CVA to process both the left and the right images from the tracking sensors at 800w,800h resolution.
The below flow is implemented:
AXI_RD2 -> AXI_WRITE1 -> PPU7 -> CVA -> CVA_RD6
AXI_RD3 -> AXI_WRITE1 -> PPU7 -> CVA -> CVA_RD6

Images are then tagged to say if each image came from the left or the right stream

Copy the files to the location where the NU4100 SDK has been installed and rename them as default boot10065 xml files.
for ex: Linux - 
$ sudo cp <PATH_TO_>/CVA/nu4100_boot10065_CVA.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/nu4100_boot10065.xml
$ sudo cp <PATH_TO_>/CVA/nu4100_boot10065_CVA_b.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/nu4100_boot10065_Slave_CVA_b.xml
$ sudo cp <PATH_TO_>/CVA/sw_graph_boot10065_CVA_b.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/sw_graph_boot10065_Slave_CVA_b
$ sudo cp <PATH_TO_>/CVA/sw_graph_boot10065_CVA.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/sw_graph_boot10065.xml

$PROJECT_LOCATION must the set to be the root of the helsinki repository

To run the CVA on its own with DSR calibration:
./inu_sandbox -p -T Stream_Track_Stereo,s,q,fsg -z $PROJECT_LOCATION/FW/nu4100/calibration_slave -d 10000 -k 10065 -N "boot10065_b" "nu4100_boot10065_Slave_CVA_b.xml" "sw_graph_boot10065_Slave_CVA_b" -T Stream_Cva_0,s,q,y,m  -B "main" "IMAGE_2" "boot10065_b" "WRITER_1" -B "main" "IMAGE_3" "boot10065_b" "WRITER_1" -J 19 800 800 -H "INU_CVA_CHANNEL_19"

To run the CVA and AF VST ISP streams at the same time:
./inu_sandbox -p -k 10065 -F 2 -d -1  -T Stream_mp_isp0,q,s -T Stream_mp_isp1,q,s,fsg -r 2 3  -T Stream_Track_Stereo,s,q -N "boot10065_b" "nu4100_boot10065_Slave_CVA_b.xml" "sw_graph_boot10065_Slave_CVA_b.xml" -T Stream_Cva_0,s,q,y,m  -B "main" "IMAGE_2" "boot10065_b" "WRITER_1" -B "main" "IMAGE_3" "boot10065_b" "WRITER_1" -J 19 800 800 -H "INU_CVA_CHANNEL_19" 
Known issues: 
1. Sandbox will report frames to be dropped even if they haven't been because we are injecting frame IDs from the input to the CVA.
