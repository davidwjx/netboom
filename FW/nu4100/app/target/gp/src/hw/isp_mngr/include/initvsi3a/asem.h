/******************************************************************************\
|* Copyright (c) 2020 by VeriSilicon Holdings Co., Ltd. ("VeriSilicon")       *|
|* All Rights Reserved.                                                       *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of VeriSilicon.  This is proprietary information owned or licensed by      *|
|* VeriSilicon.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of VeriSilicon.                                         *|
\******************************************************************************/
#ifndef _VSI_3AV2_ASEM_H_
#define _VSI_3AV2_ASEM_H_
#include <cmath>
#include "Common3ADefs.h"

// asem
namespace vsi3av2 {

typedef map<int, vector<vsi3av2::Point<int>>> CclTree;
void checkConnectedComponent(Matrix<uchar>& mat, CclTree& tree);
float thresholdOtsu(const std::vector<float>& luma);
float getObjectRegion(Matrix<uchar>& LL, vector<float>& luma, CclTree& tree);
float getObjectTarget(Matrix<uchar>& object, std::vector<float>& luma, float T, float& z, float c1, float c2);

}

#endif
