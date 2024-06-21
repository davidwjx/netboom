This XML allows for the CVA to process both the left and the right images from the tracking sensors at 800w,800h resolution.
The below flow is implemented:
AXI_RD2 -> AXI_WRITE1 -> PPU7 -> CVA -> CVA_RD6
AXI_RD3 -> AXI_WRITE1 -> PPU7 -> CVA -> CVA_RD6

Images are then tagged to say if each image came from the left or the right stream

Copy the files to the location where the NU4100 SDK has been installed and rename them as default boot10065 xml files.
for ex: Linux - 
$ sudo cp <PATH_TO_>/CVA/nu4100_boot10065_CVA.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/nu4100_boot10065.xml
$ sudo cp <PATH_TO_>/CVA/nu4100_boot10065_CVA_b.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/nu4100_boot10065_Master_CVA_b.xml
$ sudo cp <PATH_TO_>/CVA/sw_graph_boot10065_CVA_b.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/sw_graph_boot10065_b.xml
$ sudo cp <PATH_TO_>/CVA/sw_graph_boot10065_CVA.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/sw_graph_boot10065.xml

$PROJECT_LOCATION must the set to be the root of the helsinki repository

To run the CVA on its own with DSR calibration:
./inu_sandbox -p -F 3 -T Stream_Track_Stereo,s,q -d 10000 -k 10065 -z $PROJECT_LOCATION/FW/nu4100/calibration_master -N "boot10065_b" "nu4100_boot10065_Master_CVA_b.xml" "sw_graph_boot10065_Master_CVA_b.xml" -T Stream_Cva_0,s,q,m  -B "main" "IMAGE_2" "boot10065_b" "WRITER_1" -B "main" "IMAGE_3" "boot10065_b" "WRITER_1" -J 19 800 800 -H "INU_CVA_CHANNEL_19"
To run ISP on its own:
./inu_sandbox -p  -d 1000 -k 10065  -T Stream_mp_isp0,q,s -T Stream_mp_isp1,q,s -r 2 3 
To run ISP and the CVA:
./inu_sandbox -p -F 3  -d 1000 -k 10065 -z $PROJECT_LOCATION/FW/nu4100/calibration_master  -T Stream_Track_Stereo,s,q -T Stream_mp_isp0,q,s -T Stream_mp_isp1,q,s,fsg -r 2 3  -N "boot10065_b" "nu4100_boot10065_Master_CVA_b.xml" "sw_graph_boot10065_Master_CVA_b.xml" -T Stream_Cva_0,s,q,y,m  -B "main" "IMAGE_2" "boot10065_b" "WRITER_1" -B "main" "IMAGE_3" "boot10065_b" "WRITER_1" -J 19 800 800 -H "INU_CVA_CHANNEL_19"
To run the ISP, CVA and Gaze (All video streams apart from GAZE ROI):
./inu_sandbox -p  -F 3  -d 10000 -k 10065 -z $PROJECT_LOCATION/FW/nu4100/calibration_master  -T Stream_Track_Stereo,q -T Stream_GAZE_Stereo,q -T Stream_mp_isp0,q -T Stream_mp_isp1,q,fsg -r 2 3 -N "boot10065_b" "nu4100_boot10065_Master_CVA_b.xml" "sw_graph_boot10065_Master_CVA_b.xml" -T Stream_Cva_0,q  -B "main" "IMAGE_2" "boot10065_b" "WRITER_1" -B "main" "IMAGE_3" "boot10065_b" "WRITER_1" -J 19 800 800 -H "INU_CVA_CHANNEL_19"
To run the ISP, CVA and Gaze (All video streams):
./inu_sandbox -p -F 3  -d 10000 -k 10065 -z $PROJECT_LOCATION/FW/nu4100/calibration_master  -T Stream_Track_Stereo,q -T Stream_GAZE_ROI_Left,q -T Stream_GAZE_ROI_Right,q, -e 4 512 304 0 0 -e 5 512 304 0 0  -T Stream_mp_isp0,q -T Stream_mp_isp1,q,fsg -r 2 3 -N "boot10065_b" "nu4100_boot10065_Master_CVA_b.xml" "sw_graph_boot10065_Master_CVA_b.xml" -T Stream_Cva_0,q  -B "main" "IMAGE_2" "boot10065_b" "WRITER_1" -B "main" "IMAGE_3" "boot10065_b" "WRITER_1" -J 19 800 800 -H "INU_CVA_CHANNEL_19"
To run the Sandbox with an example where the AF VST crop window moves around:
./inu_sandbox -p -F 3  -d 1000 -k 10065  -T Stream_mp_isp0,q,s -T Stream_mp_isp1,q,s,f,fsg -r 2 3
To run the Sandbox with the Gaze ROI streams seperated out:
./inu_sandbox -p -F 3   -d 10000 -k 10065   -T Stream_GAZE_ROI_Left,q -T Stream_GAZE_ROI_Right,q, -e 4 512 304 0 0 -e 5 512 304 0 0 

./inu_sandbox -p  -d 10000 -k 10065  -T Stream_Track_Stereo,q -T Stream_GAZE_ROI,q -e 4 512 404 0 0 -T Stream_GAZE_Stereo,q,fsg -N "boot10065_b" "nu4100_boot10065_Master_CVA_b.xml" "sw_graph_boot10065_Master_CVA_b.xml" -T Stream_Cva_0,q  -B "main" "IMAGE_2" "boot10065_b" "WRITER_1" -B "main" "IMAGE_3" "boot10065_b" "WRITER_1" -J 19 800 800 -H "INU_CVA_CHANNEL_19"

To run the sandbox with DSR calibration, replace $PROJECT_LOCATION with the path to your project

/inu_sandbox  -T Stream_Track_Stereo,s,q -d 10000 -k 10065 -z $PROJECT_LOCATION/FW/nu4100/calibration_master
Known issues: 
1. Sandbox will report frames to be dropped even if they haven't been because we are injecting frame IDs from the input to the CVA.
2. Running all video streams together causes 70% target CPU usage, this is too high and the target will crash
3. Gaze ROI seems to stop other streams from working
