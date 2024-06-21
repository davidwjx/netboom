Implements the below flow: 
sens_4 (AF VST downsaled and no VC configure, AR2020 1024x948 2BPP)	-> SLU1 -> IAU0 -> PPU9 ->  axi_read2 (Stream_VST_AF_0) YUV422
sens_5 (AF VST Cropped, AR2020 1024x948 2BPP)  						-> SLU4 -> IAU1 -> PPU10 -> axi_read3 (Stream_VST_AF_1) YUV422


Copy the files to the location where the NU4100 SDK has been installed and rename them as default boot10065 xml files.
for ex: Linux - 
$ sudo cp <PATH_TO_>/AF_VST_YUV422/nu4100_boot10065_AF_VST_YUV422_8Bit.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/nu4100_boot10065.xml
$ sudo cp <PATH_TO_>/AF_VST_YUV422/nu4100_boot10065_AF_VST_YUV422_8Bit_b.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/nu4100_boot10065_b.xml
$ sudo cp <PATH_TO_>/AF_VST_YUV422/sw_graph_boot10065_AF_VST_YUV422_8Bit_b.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/sw_graph_boot10065_b.xml
$ sudo cp <PATH_TO_>/sw_graph_boot10065.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/sw_graph_boot10065.xml

Instructions:
1. Start the master AF VST sensors:
$ sudo cp FW/nu4100/xmldb/MASTER/nu4100_boot10065.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/nu4100_boot10065.xml
$ sudo cp FW/nu4100/xmldb/MASTER/sw_graph_boot10065.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/sw_graph_boot10065.xml
$ sudo gpioset 3 12=0 # Disable the master
$ sudo gpioset 3 14=0 # Disable the slave
$ sudo gpioset 3 12=1 # Enable the master
$ ./inu_sandbox -T Stream_VST_AF_0_RAW,y -T Stream_VST_AF_1_RAW,y -r 1 0 -d 30 -k 10065
2. Copy over the AF VST sensor files
$ sudo cp FW/AF_VST_YUV422/nu4100_boot10065_AF_VST_YUV422_8Bit.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/nu4100_boot10065.xml
$ sudo cp FW/AF_VST_YUV422/nu4100_boot10065_AF_VST_YUV422_8Bit_b.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/nu4100_boot10065_b.xml
$ sudo cp FW/AF_VST_YUV422/sw_graph_boot10065_AF_VST_YUV422_8Bit_b.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/sw_graph_boot10065_b.xml
$ sudo cp FW/sw_graph_boot10065.xml /opt/Inuitive/InuDev/bin/NU4100/boot0/sw_graph_boot10065.xml
3. Enable the slave GPIO
$ sudo gpioset 3 14=1
4. Then start the slave AF VST sensors:
$ ./inu_sandbox -T Stream_VST_AF_0,q,s -T Stream_VST_AF_1,q.s  -r 1 0  -d 3000 -k 10065  -N "boot10065_b" "nu4100_boot10065_b.xml" "sw_graph_boot10065_b.xml" -B "main" "IMAGE_2" "boot10065_b" "WRITER_0" -J 23 1024 948 -H "INU_MIPI_CHANNEL_2"

Known issues:
The Y, U and V components need bit shifting by 1 to the left

Example Jetson device tree:
                mode0 {
                    readout_orientation = "0";
                    num_lanes = "2";
                    mclk_khz = "24000";
                    pix_clk_hz = "160000000";
                    cil_settletime = "0";
                    discontinuous_clk = "no";
                    dpcm_enable = "false";
                    tegra_sinterface = "serial_c";
                    phy_mode = "DPHY";
                    active_w = "1024"; /* 1024, 948 sized image */
                    active_h = "948";
                    line_length = "3840"; /*3840*/
                    embedded_metadata_height = "0";
                    pixel_phase = "uyvy"; /*Ignore as this is for the debayering in the ISP */
                    mode_type = "yuv";  /*Ignore as this is for the debayering in the ISP */
                    csi_pixel_bit_depth = "16"; /*16 bits per pixel */
                };