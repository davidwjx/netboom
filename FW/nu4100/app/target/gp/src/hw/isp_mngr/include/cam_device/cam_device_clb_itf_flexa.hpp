/******************************************************************************\
|* Copyright (c) 2020 by VeriSilicon Holdings Co., Ltd. ("VeriSilicon")       *|
|* All Rights Reserved.                                                       *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of VeriSilicon.  This is proprietary information owned or licensed by      *|
|* VeriSilicon.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of VeriSilicon.                                         *|
|*                                                                            *|
\******************************************************************************/

#ifndef __CAM_DEVICE_CLB_ITF_FLEXA_HPP__
#define __CAM_DEVICE_CLB_ITF_FLEXA_HPP__

namespace clb_itf {

struct Flexa{

    struct ezsbi_mi_cfg {
        uint32_t available;
        uint32_t slice_per_frame;
        uint32_t Y_segment_entry_size;
        uint32_t Y_segment_entry_count;
        uint32_t CB_segment_entry_size;
        uint32_t CB_segment_entry_count;
        uint32_t CR_segment_entry_size;
        uint32_t CR_segment_entry_count;
        int valid_width;
        uint32_t g_sizeY;
        uint32_t g_sizeCb;
    };

    struct Config {
        bool enable;
        bool use_flexa_cfg;
        bool input_use;
        bool sbi_mi_cfg_use;
        uint32_t sbi_segment;
        uint32_t slice_size;
        uint32_t height;
        uint32_t buffer_number;
        int input_fds[PIC_BUFFER_NUM_INPUT];
        int small_fds[PIC_BUFFER_NUM_SMALL_IMAGE];
        int large_fds[PIC_BUFFER_NUM_LARGE_IMAGE];
        struct ezsbi_mi_cfg mi_cfg;
    };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_FLEXA_HPP__

