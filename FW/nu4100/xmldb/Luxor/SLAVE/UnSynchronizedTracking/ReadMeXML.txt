This folder contains an unsynchronized version of the normal main graph where the tracking sensors run independently and don't need the tracking sync signal from the master.

****** SandBox commands ****** 
Start the Luxor slave first using the nu4100_boot10065.xml located in this folder:

SandBoxFW_4100.exe -T Stream_Track_Stereo,q,s,m -d 30 -k 10065