/******************************************************************************\
|* Copyright 2010, Dream Chip Technologies GmbH. used with permission by      *|
|* VeriSilicon.                                                               *|
|* Copyright (c) <2020> by VeriSilicon Holdings Co., Ltd. ("VeriSilicon")     *|
|* All Rights Reserved.                                                       *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of VeriSilicon.  This is proprietary information owned or licensed by      *|
|* VeriSilicon.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of VeriSilicon.                                         *|
|*                                                                            *|
\******************************************************************************/
#ifndef _ISP_SBI_MI_H
#define _ISP_SBI_MI_H

#include "isp_version.h"
#include "vvdefs.h"


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

int ezsbi_mi_configuration(struct isp_ic_dev *dev, void *args);


#endif

