/****************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014-2023 Vivante Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 ****************************************************************************/

#ifndef __CAM_DEVICE_CLB_ITF_3A_HPP__
#define __CAM_DEVICE_CLB_ITF_3A_HPP__
#include  <functional>
namespace clb_itf {
struct AeMetaData {
    float expsoureTime;//sensor exposure time
    float expsoureGain;//sensor Expsoure  total Gain
    float ispDgain;  // isp Digit Gain
    bool aeSet;
    bool metaDataEnableAE;  /*True = Disable AE, false = Enable AE*/
    bool hasmetadataEnableAE;
};
struct AwbMetaData {
    #define CAMDEV_3A_METADATA_AWBGAIN_NUM 4
    #define CAMDEV_3A_METADATA_CCM_NUM 9
    #define CAMDEV_3A_METADATA_OFFSET_NUM 3
    float awbGain[CAMDEV_3A_METADATA_AWBGAIN_NUM];//rGain grGain gbGain bGain
    float CCM[CAMDEV_3A_METADATA_CCM_NUM];
    float offset[CAMDEV_3A_METADATA_OFFSET_NUM];

    //awb OTP data
    bool enableAwbOTP;
    float  colorTemp;  //color tempture

    bool awbSet;
    bool metaDataEnableAWB;  /*True = Disable AWB, false = Enable AWB*/
    bool hasmetadataEnableAWB;
};
struct LscMetaData {
    #define CAMDEV_3A_METADATA_LSCTABLE_NUM 1156  //17*17*4
    uint16_t lscTable[CAMDEV_3A_METADATA_LSCTABLE_NUM];
    bool enableLscOTP;
    bool lscSet;
};
struct MetaData3A{

    AeMetaData aeMetaData;
    AwbMetaData awbMetaData;
    LscMetaData lscMetaData;
    int (*ae_cb)(void *) = NULL;    /*Callback when AE has been finished*/
    int (*awb_cb)(void *) = NULL;   /*Callback when AWB has been finished*/
};

}// namespace clb
#endif    // __CAM_DEVICE_CLB_ITF_3A_HPP__

