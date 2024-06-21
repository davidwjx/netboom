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

#ifndef __CAMERA_DEVICE_MODULE_IDS__
#define __CAMERA_DEVICE_MODULE_IDS__
#include "keys.hpp"

// Cam_device API header definitions
const uint32_t ISPCORE_MODULE_GROUPID_PREFIX_MASK =  0xFFFFF000;

typedef enum __MODULE_ID__ {
    ISPCORE_MODULE_DEFAULT = 0,
    ISPCORE_MODULE_AE_BEGIN                         = 0x00001000,
    ISPCORE_MODULE_AE_PROCESS                       = 0x00001001,
    ISPCORE_MODULE_AE_CFG_GET                       = 0x00001002,
    ISPCORE_MODULE_AE_CFG_SET                       = 0x00001003,
    ISPCORE_MODULE_AE_ENABLE_GET                    = 0x00001006,
    ISPCORE_MODULE_AE_ENABLE_SET                    = 0x00001007,
    ISPCORE_MODULE_AE_RESET                         = 0x00001008,
    ISPCORE_MODULE_AE_SETPOINT_SET                  = 0x00001009,
    ISPCORE_MODULE_AE_DAMPOVER_SET                  = 0x0000100A,
    ISPCORE_MODULE_AE_DAMPUNDER_SET                 = 0x0000100B,
    ISPCORE_MODULE_AE_TOLERANCE_SET                 = 0x0000100C,
    ISPCORE_MODULE_AE_GENERATION_SET                = 0x0000100D,
    ISPCORE_MODULE_AE_ROI_SET                       = 0x0000100E,
    ISPCORE_MODULE_AE_ROI_GET                       = 0x0000100F,
    ISPCORE_MODULE_AE_END,

    ISPCORE_MODULE_AF_BEGIN                         = 0x00002000,
    ISPCORE_MODULE_AF_END,

    ISPCORE_MODULE_AVS_BEGIN                        = 0x00003000,
    ISPCORE_MODULE_AVS_END,

    ISPCORE_MODULE_AWB_BEGIN                        = 0x00004000,
    ISPCORE_MODULE_AWB_PROCESS                      = 0x00004001,
    ISPCORE_MODULE_AWB_CFG_GET                      = 0x00004002,
    ISPCORE_MODULE_AWB_ENABLE_GET                   = 0x00004004,
    ISPCORE_MODULE_AWB_ENABLE_SET                   = 0x00004005,
    ISPCORE_MODULE_AWB_ILLUMPRO_GET                 = 0x00004006,
    ISPCORE_MODULE_AWB_RESET                        = 0x00004008,
    ISPCORE_MODULE_AWB_STATUS_GET                   = 0x00004009,
    ISPCORE_MODULE_AWB_MEAS_WIN_SET                 = 0x0000400A,
    ISPCORE_MODULE_AWB_END,

    ISPCORE_MODULE_3A_BEGIN                         = 0x00004100,
    ISPCORE_MODULE_3A_METADATA_SET                  = 0x00004101,
    ISPCORE_MODULE_3A_METADATA_GET                  = 0x00004102,
    ISPCORE_MODULE_3A_END,

    ISPCORE_MODULE_BLS_BEGIN                        = 0x00005000,
    ISPCORE_MODULE_BLS_PROCESS                      = 0x00005001,
    ISPCORE_MODULE_BLS_CFG_GET                      = 0x00005002,
    ISPCORE_MODULE_BLS_CFG_SET                      = 0x00005003,
    ISPCORE_MODULE_BLS_END,

    ISPCORE_MODULE_CAC_BEGIN                        = 0x00006000,
    ISPCORE_MODULE_CAC_PROCESS                      = 0x00006001,
    ISPCORE_MODULE_CAC_ENABLE_GET                   = 0x00006002,
    ISPCORE_MODULE_CAC_ENABLE_SET                   = 0x00006003,
    ISPCORE_MODULE_CAC_STATUS                       = 0x00006004,
    ISPCORE_MODULE_CAC_END,

    ISPCORE_MODULE_CNR_BEGIN                        = 0x00007000,
    ISPCORE_MODULE_CNR_PROCESS                      = 0x00007001,
    ISPCORE_MODULE_CNR_CFG_GET                      = 0x00007002,
    ISPCORE_MODULE_CNR_CFG_SET                      = 0x00007003,
    ISPCORE_MODULE_CNR_ENABLE                       = 0x00007004,
    ISPCORE_MODULE_CNR_DISABLE                      = 0x00007005,
    ISPCORE_MODULE_CNR_END,

    ISPCORE_MODULE_CPROC_BEGIN                      = 0x00008000,
    ISPCORE_MODULE_CPROC_END,

    ISPCORE_MODULE_DEMOSAIC_BEGIN                       = 0x00009000,
    ISPCORE_MODULE_DEMOSAIC_PROCESS                     = 0x00009001,
    ISPCORE_MODULE_DEMOSAIC_CFG_GET                     = 0x00009002,
    ISPCORE_MODULE_DEMOSAIC_CFG_SET                     = 0x00009003,
    ISPCORE_MODULE_DEMOSAIC_ENABLE                      = 0x00009004,
    ISPCORE_MODULE_DEMOSAIC_DISABLE                     = 0x00009005,
    ISPCORE_MODULE_DEMOSAIC_END,

    ISPCORE_MODULE_DEVICE_BEGIN                     = 0x0000A000,
    ISPCORE_MODULE_DEVICE_PROCESS                   = 0x0000A001,
    ISPCORE_MODULE_DEVICE_CALIBRATIONLOADANDINIT    = 0x0000A003,
    ISPCORE_MODULE_DEVICE_CAMERA_CONNECT            = 0x0000A007,
    ISPCORE_MODULE_DEVICE_INPUTINFO                 = 0x0000A00A,
    ISPCORE_MODULE_DEVICE_INPUT_CONNECT             = 0x0000A00F,
    ISPCORE_MODULE_DEVICE_INPUT_DISCONNECT          = 0x0000A010,
    ISPCORE_MODULE_DEVICE_STREAMING_START           = 0x0000A014,
    ISPCORE_MODULE_DEVICE_STREAMING_STOP            = 0x0000A015,
    ISPCORE_MODULE_DEVICE_CAMERA_DISCONNECT         = 0x0000A016,
    ISPCORE_MODULE_DEVICE_FORMAT_GET                = 0x0000A01E,
    ISPCORE_MODULE_DEVICE_FORMAT_SET                = 0x0000A01F,
    ISPCORE_MODULE_DEVICE_BUFFER_SIZE_GET           = 0x0000A020,
    ISPCORE_MODULE_DEVICE_ISPWINDOW_SET             = 0x0000A021,
    ISPCORE_MODULE_DEVICE_END,

    ISPCORE_MODULE_2DNR_BEGIN                       = 0x0000B000,
    ISPCORE_MODULE_2DNR_PROCESS                     = 0x0000B001,
    ISPCORE_MODULE_2DNR_CFG_GET                     = 0x0000B002,
    ISPCORE_MODULE_2DNR_CFG_SET                     = 0x0000B003,
    ISPCORE_MODULE_2DNR_DISABLE                     = 0x0000B004,
    ISPCORE_MODULE_2DNR_ENABLE                      = 0x0000B005,
    ISPCORE_MODULE_2DNR_END,

    ISPCORE_MODULE_3DNR_BEGIN                       = 0x0000C000,
    ISPCORE_MODULE_3DNR_PROCESS                     = 0x0000C001,
    ISPCORE_MODULE_3DNR_CFG_GET                     = 0x0000C002,
    ISPCORE_MODULE_3DNR_CFG_SET                     = 0x0000C003,
    ISPCORE_MODULE_3DNR_DISABLE                     = 0x0000C004,
    ISPCORE_MODULE_3DNR_ENABLE                      = 0x0000C005,
    ISPCORE_MODULE_3DNR_TNR_ENABLE                  = 0x0000C008,
    ISPCORE_MODULE_3DNR_TNR_DISABLE                 = 0x0000C009,
    ISPCORE_MODULE_3DNR_END,

    ISPCORE_MODULE_DPCC_BEGIN                       = 0x0000D000,
    ISPCORE_MODULE_DPCC_PROCESS                     = 0x0000D001,
    ISPCORE_MODULE_DPCC_CFG_SET                     = 0x0000D002,
    ISPCORE_MODULE_DPCC_CFG_GET                     = 0x0000D003,
    ISPCORE_MODULE_DPCC_ENABLE                      = 0x0000D004,
    ISPCORE_MODULE_DPCC_DISABLE                     = 0x0000D005,
    ISPCORE_MODULE_DPCC_END,

    ISPCORE_MODULE_DPF_BEGIN                        = 0x0000E000,
    ISPCORE_MODULE_DPF_PROCESS                      = 0x0000E001,
    ISPCORE_MODULE_DPF_CFG_GET                      = 0x0000E002,
    ISPCORE_MODULE_DPF_CFG_SET                      = 0x0000E003,
    ISPCORE_MODULE_DPF_ENABLE                       = 0x0000E004,
    ISPCORE_MODULE_DPF_DISABLE                      = 0x0000E005,
    ISPCORE_MODULE_DPF_END,


    ISPCORE_MODULE_EE_BEGIN                         = 0x00010000,
    ISPCORE_MODULE_EE_PROCESS                       = 0x00010001,
    ISPCORE_MODULE_EE_CFG_GET                       = 0x00010002,
    ISPCORE_MODULE_EE_CFG_SET                       = 0x00010003,
    ISPCORE_MODULE_EE_ENABLE                        = 0x00010004,
    ISPCORE_MODULE_EE_DISABLE                       = 0x00010005,
    ISPCORE_MODULE_EE_RESET                         = 0x00010006,
    ISPCORE_MODULE_EE_END,

    ISPCORE_MODULE_FILTER_BEGIN                     = 0x00012000,
    ISPCORE_MODULE_FILTER_PROCESS                   = 0x00012001,
    ISPCORE_MODULE_FILTER_CFG_GET                   = 0x00012002,
    ISPCORE_MODULE_FILTER_CFG_SET                   = 0x00012003,
    ISPCORE_MODULE_FILTER_ENABLE                    = 0x00012004,
    ISPCORE_MODULE_FILTER_DISABLE                   = 0x00012005,
    ISPCORE_MODULE_FILTER_END,

    ISPCORE_MODULE_GC_BEGIN                         = 0x00013000,
    ISPCORE_MODULE_GC_PROCESS                       = 0x00013001,
    ISPCORE_MODULE_GC_CFG_GET                       = 0x00013002,
    ISPCORE_MODULE_GC_CFG_SET                       = 0x00013003,
    ISPCORE_MODULE_GC_ENABLE                        = 0x00013004,
    ISPCORE_MODULE_GC_DISABLE                       = 0x00013005,
    ISPCORE_MODULE_GC_END,

    ISPCORE_MODULE_HDR_BEGIN                        = 0x00014000,
    ISPCORE_MODULE_HDR_PROCESS                      = 0x00014001,
    ISPCORE_MODULE_HDR_ENABLE_SET                   = 0x00014005,
    ISPCORE_MODULE_HDR_END,

    ISPCORE_MODULE_IE_BEGIN                         = 0x00015000,
    ISPCORE_MODULE_IE_END,

    ISPCORE_MODULE_LSC_BEGIN                        = 0x00016000,
    ISPCORE_MODULE_LSC_PROCESS                      = 0x00016001,
    ISPCORE_MODULE_LSC_CFG_GET                      = 0x00016002,
    ISPCORE_MODULE_LSC_CFG_SET                      = 0x00016003,
    ISPCORE_MODULE_LSC_ENABLE                       = 0x00016004,
    ISPCORE_MODULE_LSC_DISABLE                      = 0x00016005,
    ISPCORE_MODULE_LSC_END,

    ISPCORE_MODULE_REG_BEGIN                        = 0x00017000,
    ISPCORE_MODULE_REG_END,

    ISPCORE_MODULE_SENSOR_BEGIN                     = 0x00018000,
    ISPCORE_MODULE_SENSOR_PROCESS                   = 0x00018001,
    ISPCORE_MODULE_SENSOR_CAPS                      = 0x00018002,
    ISPCORE_MODULE_SENSOR_CFG_GET                   = 0x00018003,
    ISPCORE_MODULE_SENSOR_CFG_SET                   = 0x00018004,
    ISPCORE_MODULE_SENSOR_NAME_GET                  = 0x00018005,
    ISPCORE_MODULE_SENSOR_REVISION_GET              = 0x00018006,
    ISPCORE_MODULE_SENSOR_OPEN                      = 0x00018007,
    ISPCORE_MODULE_SENSOR_IS_CONNECTED              = 0x00018008,
    ISPCORE_MODULE_SENSOR_TEST_PATTERN_SET          = 0x00018009,
    ISPCORE_MODULE_SENSOR_DRV_CHANGE                = 0x0001800A,
    ISPCORE_MODULE_SENSOR_DRV_LIST                  = 0x0001800B,
    ISPCORE_MODULE_SENSOR_INFO                      = 0x0001800C,
    ISPCORE_MODULE_SENSOR_RESOLUTION_GET            = 0x0001800D,
    ISPCORE_MODULE_SENSOR_REG_GET                   = 0x00018010,
    ISPCORE_MODULE_SENSOR_REG_SET                   = 0x00018011,
    ISPCORE_MODULE_SENSOR_REG_TABLE                 = 0x00018012,
    ISPCORE_MODULE_SENSOR_TEST_PATTERN_GET          = 0x00018013,
    ISPCORE_MODULE_SENSOR_FPS_GET                   = 0x00018016,
    ISPCORE_MODULE_SENSOR_FPS_SET                   = 0x00018017,
    ISPCORE_MODULE_SENSOR_QUERY                     = 0x00018018,
    ISPCORE_MODULE_SENSOR_MODE_GET                  = 0x00018019,
    ISPCORE_MODULE_SENSOR_STATUS                    = 0x0001801E,
    ISPCORE_MODULE_SENSOR_EXPOSURETIME_SET          = 0x0001801F,
    ISPCORE_MODULE_SENSOR_GAIN_GET                  = 0x00018020,
    ISPCORE_MODULE_SENSOR_GAIN_SET                  = 0x00018021,
    ISPCORE_MODULE_SENSOR_OTP_INFO_GET              = 0x00018022,
    ISPCORE_MODULE_SENSOR_END,

    ISPCORE_MODULE_WB_BEGIN                         = 0x0001A000,
    ISPCORE_MODULE_WB_PROCESS                       = 0x0001A001,
    ISPCORE_MODULE_WB_CFG_GET                       = 0x0001A002,
    ISPCORE_MODULE_WB_CFG_SET                       = 0x0001A003,
    ISPCORE_MODULE_WB_GAIN_SET                      = 0x0001A004,
    ISPCORE_MODULE_WB_CCOFFSET_SET                  = 0x0001A005,
    ISPCORE_MODULE_WB_CCMATRIX_SET                  = 0x0001A006,
    ISPCORE_MODULE_WB_END,

    ISPCORE_MODULE_WDR_BEGIN                        = 0x0001B000,
    ISPCORE_MODULE_WDR_PROCESS                      = 0x0001B001,
    ISPCORE_MODULE_WDR_CFG_GET                      = 0x0001B002,
    ISPCORE_MODULE_WDR_CFG_SET                      = 0x0001B003,
    ISPCORE_MODULE_WDR_DISABLE                      = 0x0001B004,
    ISPCORE_MODULE_WDR_ENABLE                       = 0x0001B005,
    ISPCORE_MODULE_WDR_END,

    ISPCORE_MODULE_PIPELINE_BEGIN                   = 0x0001C000,
    ISPCORE_MODULE_PIPELINE_END,

    ISPCORE_MODULE_GE_BEGIN                         = 0x0001E000,
    ISPCORE_MODULE_GE_PROCESS                       = 0x0001E001,
    ISPCORE_MODULE_GE_CFG_SET                       = 0x0001E002,
    ISPCORE_MODULE_GE_CFG_GET                       = 0x0001E003,
    ISPCORE_MODULE_GE_ENABLE                        = 0x0001E004,
    ISPCORE_MODULE_GE_DISABLE                       = 0x0001E005,
    ISPCORE_MODULE_GE_END,

    ISPCORE_MODULE_RGBGAMMA_BEGIN                   = 0x0001F000,
    ISPCORE_MODULE_RGBGAMMA_END,

    ISPCORE_MODULE_CA_BEGIN                         = 0x00020000,
    ISPCORE_MODULE_CA_PROCESS                       = 0x00020001,
    ISPCORE_MODULE_CA_CFG_SET                       = 0x00020002,
    ISPCORE_MODULE_CA_CFG_GET                       = 0x00020003,
    ISPCORE_MODULE_CA_ENABLE                        = 0x00020004,
    ISPCORE_MODULE_CA_DISABLE                       = 0x00020005,
    ISPCORE_MODULE_CA_END,

    ISPCORE_MODULE_DCI_BEGIN                      = 0x00023000,
    ISPCORE_MODULE_DCI_PROCESS                    = 0x00023001,
    ISPCORE_MODULE_DCI_CFG_SET                    = 0x00023002,
    ISPCORE_MODULE_DCI_CFG_GET                    = 0x00023003,
    ISPCORE_MODULE_DCI_ENABLE                     = 0x00023004,
    ISPCORE_MODULE_DCI_DISABLE                    = 0x00023005,
    ISPCORE_MODULE_DCI_END,

    ISPCORE_MODULE_AHDR_BEGIN               = 0x00024000,
    ISPCORE_MODULE_AHDR_END,

    ISPCORE_MODULE_DG_BEGIN                 = 0x00025000,
    ISPCORE_MODULE_DG_PROCESS               = 0x00025001,
    ISPCORE_MODULE_DG_CFG_SET               = 0x00025002,
    ISPCORE_MODULE_DG_CFG_GET               = 0x00025003,
    ISPCORE_MODULE_DG_ENABLE                = 0x00025004,
    ISPCORE_MODULE_DG_DISABLE               = 0x00025005,
    ISPCORE_MODULE_DG_END,

    ISPCORE_MODULE_CNR2_BEGIN                        = 0x00026000,
    ISPCORE_MODULE_CNR2_PROCESS                      = 0x00026001,
    ISPCORE_MODULE_CNR2_CFG_GET                      = 0x00026002,
    ISPCORE_MODULE_CNR2_CFG_SET                      = 0x00026003,
    ISPCORE_MODULE_CNR2_ENABLE                       = 0x00026004,
    ISPCORE_MODULE_CNR2_DISABLE                      = 0x00026005,
    ISPCORE_MODULE_CNR2_END,

    ISPCORE_MODULE_RGBIR_BEGIN                       = 0x00027000,
    ISPCORE_MODULE_RGBIR_PROCESS                     = 0x00027001,
    ISPCORE_MODULE_RGBIR_CFG_GET                     = 0x00027002,
    ISPCORE_MODULE_RGBIR_CFG_SET                     = 0x00027003,
    ISPCORE_MODULE_RGBIR_ENABLE                      = 0x00027004,
    ISPCORE_MODULE_RGBIR_DISABLE                     = 0x00027005,
    ISPCORE_MODULE_RGBIR_END,

    ISPCORE_MODULE_FLEXA_BEGIN                       = 0x00028000,
    ISPCORE_MODULE_FLEXA_PROCESS                     = 0x00028001,
    ISPCORE_MODULE_FLEXA_CFG_GET                     = 0x00028002,
    ISPCORE_MODULE_FLEXA_CFG_SET                     = 0x00028003,
    ISPCORE_MODULE_FLEXA_END,

    ISPCORE_MODULE_HIST_BEGIN                       = 0x00030000,
    ISPCORE_MODULE_HIST_CFG_SET                     = 0x00030001,
    ISPCORE_MODULE_HIST_STATUS_GET                  = 0x00030002,
    ISPCORE_MODULE_HIST_END,

    ISPCORE_MODULE_EXP_BEGIN                       = 0x00040000,
    ISPCORE_MODULE_EXP_CFG_SET                     = 0x00040001,
    ISPCORE_MODULE_EXP_STATUS_GET                  = 0x00040002,
    ISPCORE_MODULE_EXP_END,

    ISPCORE_MODULE_END,
} ISPCORE_MODULE_ID;


#endif   // __CAMERA_DEVICE_MODULE_IDS__

