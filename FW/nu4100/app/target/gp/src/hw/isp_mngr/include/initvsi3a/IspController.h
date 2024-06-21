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
#ifndef _VSI_3AV2_ISP_CONTROLLER_H_
#define _VSI_3AV2_ISP_CONTROLLER_H_
#include "IController.h"

namespace vsi3av2 {

class IspController : public IController {
    DECLARE_DYNAMIC_CLASS()
public:
    void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea) final;

private:
    void setAev2Size(float& m);
    void setXTalkMatrix(Matf& m);
    void setXTalkOffset(Matf& m);
    void setAwbMode(Matf& m);
    void setAwbGain(Matf& m);
    void setLscMatrix(Matrix<ushort>& m);
    void setLscSector(Matrix<ushort>& m);
    void getLscEnabled(bool& enabled);
    void getHdrEnabled(bool& enabled);
    void setGc16Curve(Matrix<ushort>& m);
    void setWdr3LowlightStrength(float& m);
    void setWdr3Gammadown(Matrix<ushort>& m);
    void setWdr3SetLocalStrength(int& m);
    void setWdr3PixelSolpe(Matrix<uchar>& m);
    void setWdr3EntropySolpe(Matrix<ushort>& m);
    void setWdr3Gain(Matf& m);
    void setAfWindow(Matf& m);
    void setAfEnable(bool& enable);
    void getHdrMode(int& mode);
    void setDciCurve(Matf& m);
    void getWdr3Params(Matf& m);
    void setWdr3Params(Matf& m);
    void getWdr3Enable(bool& enable);
    void setWdr4Enable(bool& enable);
    void setWdr4Params(Matf& m);
    void setEeEnable(bool& enable);
    void setEeParams(Matf& m);
    void setCprocEnable(bool& enable);
    void setCprocParams(Matf& m);
    void setCaEnable(bool& enable);
    void setCaParams(Matf& m);
    void setGamma64Enable(bool& enable);
    void setGamma64Params(Matf& m);
    void setDmsc2SharpenEnable(bool& enable);
    void setDmsc2SharpenParams(Matf& m);
    void setDmsc2AutoParams(Matf& m);
    void setDGainEnable(bool& enable);
    void setDGainParams(Matf& m);
    void setDpfEnable(bool& enable);
    void setDpfParams(Matf& m);
    void setDpfCurve(Matf& m);
    void set3dnrv3Enable(bool& enable);
    void set3dnrv3Params(Matf& m);
    void set2dnrv5Enable(bool& enable);
    void set2dnrv5Params(Matf& m);
    void setWdr4Entropy(Matrix<uint>& m);
    void setWdr4GammaPre(Matrix<uint>& m);
    void setWdr4GammaDown(Matrix<uint>& m);
    void setWdr4GammaUp(Matrix<uint>& m);
    void setWdr4EntropySlope(Matrix<uint>& m);
    void setBlackLevel(Matf& m);
    void getBlackLevel(Matf& m);
    void getTdnrStats(Matf& m);
    void getTdnrWindow(Matf& m);

private:
    void onMessageSetHandle(void*& s);
    void* mIspHandle = NULL;
};

}

#endif