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

#ifndef __CAM_DEVICE_CLB_ITF_HPP__
#define __CAM_DEVICE_CLB_ITF_HPP__

#include "cam_device_ispcore_defs.h"
#include "cam_device_clb_itf_common.hpp"
#include "cam_device_clb_itf_2dnr.hpp"
#include "cam_device_clb_itf_3dlut.hpp"
#include "cam_device_clb_itf_3dnr.hpp"
#include "cam_device_clb_itf_ae.hpp"
#include "cam_device_clb_itf_af.hpp"
#include "cam_device_clb_itf_ahdr.hpp"
#include "cam_device_clb_itf_avs.hpp"
#include "cam_device_clb_itf_awb.hpp"
#include "cam_device_clb_itf_bls.hpp"
#include "cam_device_clb_itf_cac.hpp"
#include "cam_device_clb_itf_ca.hpp"
#include "cam_device_clb_itf_cnr.hpp"
#include "cam_device_clb_itf_cproc.hpp"
#include "cam_device_clb_itf_dci.hpp"
#include "cam_device_clb_itf_demosaic.hpp"
#include "cam_device_clb_itf_dg.hpp"
#include "cam_device_clb_itf_dpcc.hpp"
#include "cam_device_clb_itf_dpf.hpp"
#include "cam_device_clb_itf_ee.hpp"
#include "cam_device_clb_itf_filter.hpp"
#include "cam_device_clb_itf_gc.hpp"
#include "cam_device_clb_itf_ge.hpp"
#include "cam_device_clb_itf_hdr.hpp"
#include "cam_device_clb_itf.hpp"
#include "cam_device_clb_itf_ie.hpp"
#include "cam_device_clb_itf_lsc.hpp"
#include "cam_device_clb_itf_rgbir.hpp"
#include "cam_device_clb_itf_sensor.hpp"
#include "cam_device_clb_itf_simp.hpp"
#include "cam_device_clb_itf_wb.hpp"
#include "cam_device_clb_itf_wdr.hpp"
#include "cam_device_clb_itf_flexa.hpp"
#include "cam_device_clb_itf_3a.hpp"
#include "cam_device_clb_itf_exp.hpp"
#include "cam_device_clb_itf_hist.hpp"

#define MODULE_VERSION(major, minor, patch)  \
            (((major)<<16) | ((minor)<<8) | (patch))
#define MODULE_V1      MODULE_VERSION(1, 0, 0)
#define MODULE_V2      MODULE_VERSION(2, 0, 0)
#define MODULE_V3      MODULE_VERSION(3, 0, 0)
#define MODULE_V3_2    MODULE_VERSION(3, 2, 0)
#define MODULE_V4      MODULE_VERSION(4, 0, 0)
#define MODULE_V5      MODULE_VERSION(5, 0, 0)

#endif    // __CAM_DEVICE_CLB_ITF_HPP__

