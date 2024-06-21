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

#pragma once

#include <cameric_reg_drv/cameric_reg_description.h>

#include <cam_engine/cam_engine_api.h>

#include <cam_engine/cam_engine_aaa_api.h>
#include <cam_engine/cam_engine_cproc_api.h>
#include <cam_engine/cam_engine_imgeffects_api.h>
#include <cam_engine/cam_engine_isp_api.h>
#include <cam_engine/cam_engine_jpe_api.h>
#include <cam_engine/cam_engine_mi_api.h>
#include <cam_engine/cam_engine_simp_api.h>

#include "commitf_citf.hpp"
#include <json/json.h>
#include "cam_device_api.hpp"

#define DECLARE_SUB_DEVICE_OPS\
    RESULT process( int ctrlId, Json::Value &jRequest, Json::Value &jResponse) override;\
    RESULT configGet(Json::Value &jRequest, Json::Value &jResponse);\
    RESULT configSet(Json::Value &jRequest, Json::Value &jResponse);\
    RESULT enableGet(Json::Value &jRequest, Json::Value &jResponse);\
    RESULT enableSet(Json::Value &jRequest, Json::Value &jResponse)\

namespace camdev {

class CitfDevice : virtual public Citf {
public:
    CitfDevice(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input) {
      subId_Begin = ISPCORE_MODULE_DEVICE_BEGIN;
      subId_End   = ISPCORE_MODULE_DEVICE_END;
    }

    ~CitfDevice() {
        if(pCitfHandle->pOperation != nullptr) {
          delete pCitfHandle->pOperation;
        }

        if(pCitfHandle->cam_config.pCalibration != nullptr) {
          delete pCitfHandle->cam_config.pCalibration;
        }
    }

    RESULT process( int ctrlId, Json::Value &jRequest, Json::Value &jResponse) override;
    RESULT calibrationSave(Json::Value &jRequest, Json::Value &jResponse);
    RESULT calibrationLoadandInit(Json::Value &jRequest, Json::Value &jResponse);
    RESULT calibrationLoad(Json::Value &jRequest, Json::Value &jResponse);
    RESULT initEngineOperation(Json::Value &jRequest, Json::Value &jResponse);
    RESULT resolutionSetsSensorAndIsp(Json::Value &jRequest, Json::Value &jResponse);
    RESULT cameraConnect(Json::Value &jRequest, Json::Value &jResponse);
    RESULT cameraDisconnect(Json::Value &jRequest, Json::Value &jResponse);
    RESULT captureDma(Json::Value &jRequest, Json::Value &jResponse);
    RESULT captureSensor(Json::Value &jRequest, Json::Value &jResponse);

    RESULT inputInfo(Json::Value &jRequest, Json::Value &jResponse);
    RESULT inputSwitch(Json::Value &jRequest, Json::Value &jResponse);

    RESULT preview(Json::Value &jRequest, Json::Value &jResponse);

    RESULT bitstreamId(Json::Value &jRequest, Json::Value &jResponse);
    RESULT camerIcId(Json::Value &jRequest, Json::Value &jResponse);
    RESULT inputConnect(Json::Value &jRequest, Json::Value &jResponse);
    RESULT inputDisconnect(Json::Value &jRequest, Json::Value &jResponse);
    RESULT reset(Json::Value &jRequest, Json::Value &jResponse);
    RESULT resolutionGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT resolutionSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT streamingStart(Json::Value &jRequest, Json::Value &jResponse);
    RESULT streamingStop(Json::Value &jRequest, Json::Value &jResponse);
    RESULT streamingStandby(Json::Value &jRequest, Json::Value &jResponse);

    RESULT cameraReset(Json::Value &jRequest, Json::Value &jResponse);
    RESULT previewStart(Json::Value &jRequest, Json::Value &jResponse);
    RESULT previewStop(Json::Value &jRequest, Json::Value &jResponse);
    RESULT getHalHandle(Json::Value &jRequest, Json::Value &jResponse);
    RESULT getMetadata(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfAe : virtual public Citf {
public:
    CitfAe(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input) {
      subId_Begin = ISPCORE_MODULE_AE_BEGIN;
      subId_End = ISPCORE_MODULE_AE_END;
    }

    DECLARE_SUB_DEVICE_OPS;
    RESULT ecmGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT ecmSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT reset(Json::Value &jRequest, Json::Value &jResponse);
    RESULT status(Json::Value &jRequest, Json::Value &jResponse);
    RESULT setpointSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT dampoverSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT dampunderSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT toleranceSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT generationSet(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfAwb : virtual public Citf {
public:
    CitfAwb(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_AWB_BEGIN;
      subId_End = ISPCORE_MODULE_AWB_END;
    };
    DECLARE_SUB_DEVICE_OPS;
    RESULT illuminanceProfilesGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT illuminanceProfilesSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT reset(Json::Value &jRequest, Json::Value &jResponse);
    RESULT statusGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT measWinSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT modeSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT dampingSet(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfWb : virtual public Citf {
public:
    CitfWb(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_WB_BEGIN;
      subId_End = ISPCORE_MODULE_WB_END;
    }
    DECLARE_SUB_DEVICE_OPS;
    RESULT gainSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT ccOffsetSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT ccMatrixSet(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfAf : virtual public Citf {
public:
    CitfAf(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_AF_BEGIN;
      subId_End = ISPCORE_MODULE_AF_END;
    }

    RESULT availableGet(Json::Value &jRequest, Json::Value &jResponse);
    DECLARE_SUB_DEVICE_OPS;
};

class CitfAvs : virtual public Citf {
public:
    CitfAvs(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_AVS_BEGIN;
      subId_End = ISPCORE_MODULE_AVS_END;
    }

    DECLARE_SUB_DEVICE_OPS;
    RESULT statusGet(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfCproc : virtual public Citf {
public:
    CitfCproc(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_CPROC_BEGIN;
      subId_End = ISPCORE_MODULE_CPROC_END;
    }

    DECLARE_SUB_DEVICE_OPS;
    RESULT lumainSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT lumaoutSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT chromaoutSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT brightnessSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT contrastSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT saturationSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT hueSet(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfBls : virtual public Citf {
public:
    CitfBls(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_BLS_BEGIN;
      subId_End = ISPCORE_MODULE_BLS_END;
    }

    DECLARE_SUB_DEVICE_OPS;
};

class CitfCac : virtual public Citf {
public:
    CitfCac(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_CAC_BEGIN;
      subId_End = ISPCORE_MODULE_CAC_END;
    }
    DECLARE_SUB_DEVICE_OPS;
    RESULT statusGet(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfCnr : virtual public Citf {
public:
    CitfCnr(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_CNR_BEGIN;
      subId_End = ISPCORE_MODULE_CNR_END;
    }
    DECLARE_SUB_DEVICE_OPS;
};

class CitfDpcc : virtual public Citf {
public:
    CitfDpcc(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_DPCC_BEGIN;
      subId_End = ISPCORE_MODULE_DPCC_END;
    }

    DECLARE_SUB_DEVICE_OPS;
};

class CitfDemosaic : virtual public Citf {
public:

    CitfDemosaic(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_DEMOSAIC_BEGIN;
      subId_End = ISPCORE_MODULE_DEMOSAIC_END;
    }
    DECLARE_SUB_DEVICE_OPS;
    RESULT reset(Json::Value &jRequest, Json::Value &jResponse);
    RESULT thresholdSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT globalInterpolationThresholdSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT globalDenoiseStrengthSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT globalSharpenSizeSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT demoireEnableSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT demoireAreaThresholdSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT demoireSaturationShrinkSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT demoireThresholdSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT demoireEdgeSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT dePurpleEnableSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT dePurpleCbCrModeSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT dePurpleSaturationShrinkSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT dePurpleThresholdSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT sharpenEnableSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT sharpenClipSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT sharpenFactorSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT sharpenThresholdSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT sharpenRatioSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT sharpenFilterSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT sharpenLineEnableSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT sharpenLineRatioSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT sharpenLineShift1Set(Json::Value &jRequest, Json::Value &jResponse);
    RESULT sharpenLineShift2Set(Json::Value &jRequest, Json::Value &jResponse);
    RESULT sharpenLineStrengthSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT sharpenLineT1Set(Json::Value &jRequest, Json::Value &jResponse);
    RESULT skinEnableSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT skinCbThresholdSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT skinCrThresholdSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT skinYThresholdSet(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfDpf : virtual public Citf {
public:
    CitfDpf(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_DPF_BEGIN;
      subId_End = ISPCORE_MODULE_DPF_END;
    };

    DECLARE_SUB_DEVICE_OPS;
};

class CitfGc : virtual public Citf {
public:
    CitfGc(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_GC_BEGIN;
      subId_End = ISPCORE_MODULE_GC_END;
    }
    DECLARE_SUB_DEVICE_OPS;
};

class CitfEe : virtual public Citf {
public:
    CitfEe(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_EE_BEGIN;
      subId_End = ISPCORE_MODULE_EE_END;
    }

    struct Config {
      int32_t strength;
      int32_t sharpen;
      int32_t depurple;
    };

    DECLARE_SUB_DEVICE_OPS;
    RESULT reset(Json::Value &jRequest, Json::Value &jResponse);
    RESULT statusGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tableGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tableSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT autoModeSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT strengthSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT yUpGainSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT yDownGainSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT uvGainSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT edgeGainSet(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfFileSystem : public Citf {
public:
    CitfFileSystem(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_FILESYSTEM_BEGIN;
      subId_End = ISPCORE_MODULE_FILESYSTEM_END;
    }

    DECLARE_SUB_DEVICE_OPS;
    RESULT remove(Json::Value &jRequest, Json::Value &jResponse);
    RESULT list(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfFilter : virtual public Citf {
public:
    CitfFilter(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_FILTER_BEGIN;
      subId_End = ISPCORE_MODULE_FILTER_END;
    }
    DECLARE_SUB_DEVICE_OPS;
    RESULT statusGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tableGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tableSet(Json::Value &jRequest, Json::Value &jResponse);
};

class Citf2Dnr : virtual public Citf {
public:
    Citf2Dnr(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_2DNR_BEGIN;
      subId_End = ISPCORE_MODULE_2DNR_END;
    }
    DECLARE_SUB_DEVICE_OPS;
    RESULT reset(Json::Value &jRequest, Json::Value &jResponse);
    RESULT statusGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tableGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tableSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT generationSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT autoModeSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT autoLevelSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT denoisePreStrengthSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT denoiseStrengthSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT sigmaSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT blendOpacityStaticSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT blendOpacityMovingSet(Json::Value &jRequest, Json::Value &jResponse);
};

class Citf3Dnr : virtual public Citf {
public:
    Citf3Dnr(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_3DNR_BEGIN;
      subId_End = ISPCORE_MODULE_3DNR_END;
    }

    DECLARE_SUB_DEVICE_OPS;
    RESULT reset(Json::Value &jRequest, Json::Value &jResponse);
    RESULT statusGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tableGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tableSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT generationSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT autoModeSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT autoLevelSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT deltaFactorSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT motionFactorSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT strengthSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tnrEnableSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tnrFilterLengthSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tnrFilterLength2Set(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tnrMotionSlopeSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tnrNoiseLevelSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tnrSadWeightSet(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfReg : virtual public Citf {
public:
    CitfReg(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_REG_BEGIN;
      subId_End = ISPCORE_MODULE_REG_END;
    }
    DECLARE_SUB_DEVICE_OPS;
    RESULT description(Json::Value &jRequest, Json::Value &jResponse);
    RESULT get(Json::Value &jRequest, Json::Value &jResponse);
    RESULT set(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfHdr : virtual public Citf {
public:
    CitfHdr(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_HDR_BEGIN;
      subId_End = ISPCORE_MODULE_HDR_END;
    }
    DECLARE_SUB_DEVICE_OPS;
    RESULT reset(Json::Value &jRequest, Json::Value &jResponse);
    RESULT statusGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT generationSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT exposureRatioSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT extensionBitSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT transRangeSet(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfIe : virtual public Citf {
public:
    CitfIe(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_IE_BEGIN;
      subId_End = ISPCORE_MODULE_IE_END;
    }
    DECLARE_SUB_DEVICE_OPS;
};

class CitfLsc : virtual public Citf {
public:
    CitfLsc(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_LSC_BEGIN;
      subId_End = ISPCORE_MODULE_LSC_END;
    }
    DECLARE_SUB_DEVICE_OPS;
    RESULT statusGet(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfWdr : virtual public Citf {
public:
    CitfWdr(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input)  {
      subId_Begin = ISPCORE_MODULE_WDR_BEGIN;
      subId_End = ISPCORE_MODULE_WDR_END;
    }
    DECLARE_SUB_DEVICE_OPS;
    RESULT reset(Json::Value &jRequest, Json::Value &jResponse);
    RESULT statusGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tableGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tableSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT generationSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT strengthSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT strengthRangeSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT gstrengthSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT autoModeSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT autoLevelSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT gainMaxSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT flatStrengthSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT flatThresholdSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT contrastSet(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfSimp : virtual public Citf {
public:
    CitfSimp(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input) {
      subId_Begin = ISPCORE_MODULE_SIMP_BEGIN;
      subId_End = ISPCORE_MODULE_SIMP_END;
    }
    DECLARE_SUB_DEVICE_OPS;
};

class CitfGe : virtual public Citf {
public:
    CitfGe(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input) {
      subId_Begin = ISPCORE_MODULE_GE_BEGIN;
      subId_End = ISPCORE_MODULE_GE_END;
    }
    DECLARE_SUB_DEVICE_OPS;
};


class CitfRgbGamma : virtual public Citf {
public:
    CitfRgbGamma(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input) {
      subId_Begin = ISPCORE_MODULE_RGBGAMMA_BEGIN;
      subId_End = ISPCORE_MODULE_RGBGAMMA_END;
    }
    DECLARE_SUB_DEVICE_OPS;
};

class CitfDci : virtual public Citf {
public:
    CitfDci(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input) {
      subId_Begin = ISPCORE_MODULE_DCI_BEGIN;
      subId_End = ISPCORE_MODULE_DCI_END;
    }
    DECLARE_SUB_DEVICE_OPS;
    RESULT statusGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tableGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tableSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT autoModeSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT modeSet(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfAhdr : virtual public Citf {
public:
    CitfAhdr(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input) {
      subId_Begin = ISPCORE_MODULE_AHDR_BEGIN;
      subId_End = ISPCORE_MODULE_AHDR_END;
    }
    DECLARE_SUB_DEVICE_OPS;
    RESULT reset(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tableGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tableSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT generationGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT autoModeSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT cSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT c2Set(Json::Value &jRequest, Json::Value &jResponse);
    RESULT ceilSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT objectFectorSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT setPoint2Set(Json::Value &jRequest, Json::Value &jResponse);
    RESULT toleranceSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT ratioMaxSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT ratioMinSet(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfCa : virtual public Citf {
public:
    CitfCa(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input) {
      subId_Begin = ISPCORE_MODULE_CA_BEGIN;
      subId_End = ISPCORE_MODULE_CA_END;
    }
    DECLARE_SUB_DEVICE_OPS;
    RESULT tableGet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT tableSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT autoModeSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT modeSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT curveSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT sInflectionSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT sExponentSet(Json::Value &jRequest, Json::Value &jResponse);
    RESULT parabolicFactorSet(Json::Value &jRequest, Json::Value &jResponse);
};

class CitfDg : virtual public Citf {
public:
    CitfDg(Citf_Handle * pCitfHandle_input) :Citf(pCitfHandle_input) {
      subId_Begin = ISPCORE_MODULE_DG_BEGIN;
      subId_End = ISPCORE_MODULE_DG_END;
    }
    DECLARE_SUB_DEVICE_OPS;

};

} // namespace camdev
