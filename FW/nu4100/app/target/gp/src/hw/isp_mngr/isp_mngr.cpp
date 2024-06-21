
#include "log.h"
#include "mem_pool.h"

#include "isp_mngr.hpp"
#include "cam_device/cam_device_api.hpp"
#include "cam_device/cam_device_clb_itf.hpp"
#include "cam_device/base64.hpp"

#include "isi/isi_common.h"

#include "nu4100_isp_0_regs.h"
#include "nu4100_ipe_regs.h"

#include "nu4100_ddrc_mp_regs.h"
#include <fstream>
#include <iostream>
#include "helsinki_isp_synchronizer.h" /*Helsinki specific change*/
#ifdef ENABLE_TRACING
//this is here as an example for use in .cpp file
extern "C"{
#include "trace-marker.h"
}
#endif
//#define __REGISTER_TEST__

#define dbg_printf printf
#define NUM_ISPS              2
#define NUM_OF_ISP_READERS    6
#define ISP_READERS_OFFSET    23
#define ISP_NUM_BUFFERS       10
#define ISP_BUFIO_MAX         3

#define ISP_MNGR_AE_FLICKER_MODE_OFF    (1)
#define ISP_MNGR_AE_FLICKER_MODE_50HZ   (50)
#define ISP_MNGR_AE_FLICKER_MODE_60HZ   (60)
#define ISP_MNGR_LSC_LENGTH             (289)
ERRG_codeE ISP_MNGRG_dumpRegs(void);
extern "C" UINT32 ISP_MNGRG_getIspRegBase(UINT32 ispid);


typedef struct
{
    UINT32 cmd;
    char* funcName;
}ISP_MNGRG_funcGetT;

ISP_MNGRG_funcGetT getCommands[] =
{

    { ISPCORE_MODULE_AWB_ENABLE_GET   , "ISPCORE_MODULE_AWB_ENABLE_GET     "},
//  { ISPCORE_MODULE_WB_CFG_GET       , "ISPCORE_MODULE_WB_CFG_GET         "},
    { ISPCORE_MODULE_AE_ENABLE_GET    , "ISPCORE_MODULE_AE_ENABLE_GET      "},
    { ISPCORE_MODULE_WDR_CFG_GET      , "ISPCORE_MODULE_WDR_ENABLE_GET     "},
//  { ISPCORE_MODULE_HDR_ENABLE_GET   , "ISPCORE_MODULE_HDR_ENABLE_GET     "},
    { ISPCORE_MODULE_2DNR_CFG_GET     , "ISPCORE_MODULE_2DNR_ENABLE_GET    "},
    { ISPCORE_MODULE_3DNR_CFG_GET     , "ISPCORE_MODULE_3DNR_ENABLE_GET    "},
    { ISPCORE_MODULE_BLS_CFG_GET      , "ISPCORE_MODULE_BLS_CFG_GET        "},
    { ISPCORE_MODULE_CAC_ENABLE_GET   , "ISPCORE_MODULE_CAC_ENABLE_GET     "},
#if 0
    { ISPCORE_MODULE_GC_CFG_GET       , "ISPCORE_MODULE_GC_CFG_GET_GET      "},
    { ISPCORE_MODULE_LSC_CFG_GET      , "ISPCORE_MODULE_LSC_CFG_GET_GET     "},
    { ISPCORE_MODULE_DPF_CFG_GET      , "ISPCORE_MODULE_DPF_CFG_GET_GET     "},
    { ISPCORE_MODULE_DPCC_CFG_GET     , "ISPCORE_MODULE_DPCC_CFG_GET_GET    "},
    { ISPCORE_MODULE_DEMOSAIC_CFG_GET , "ISPCORE_MODULE_DEMOSAIC_CFG_GET_GET"},
    { ISPCORE_MODULE_CNR_CFG_GET      , "ISPCORE_MODULE_CNR_CFG_GET_GET     "},
    { ISPCORE_MODULE_EE_CFG_GET       , "ISPCORE_MODULE_EE_CFG_GET_GET      "},
    { ISPCORE_MODULE_CA_CFG_GET       , "ISPCORE_MODULE_CA_CFG_GET_GET      "},
    { ISPCORE_MODULE_DCI_CFG_GET      , "ISPCORE_MODULE_DCI_CFG_GET_GET     "},
    { ISPCORE_MODULE_GE_CFG_GET       , "ISPCORE_MODULE_GE_ENABLE_GET       "},
#endif
//    { ISPCORE_MODULE_CPROC_CFG_GET     , "ISPCORE_MODULE_CPROC_ENABLE_GET       "}

//     { ISPCORE_MODULE_3DNR_TNR_ENABLE_GET  , "ISPCORE_MODULE_3DNR_TNR_ENABLE_GET"},
//     { ISPCORE_MODULE_FILTER_ENABLE_GET    , "ISPCORE_MODULE_FILTER_ENABLE_GET  "},
//     { ISPCORE_MODULE_IE_ENABLE_GET        , "ISPCORE_MODULE_IE_ENABLE_GET      "},
//     { ISPCORE_MODULE_SIMP_ENABLE_GET      , "ISPCORE_MODULE_SIMP_ENABLE_GET    "},
//     { ISPCORE_MODULE_DMSC2_ENABLE_GET     , "ISPCORE_MODULE_DMSC2_ENABLE_GET   "},
//     { ISPCORE_MODULE_RGBGAMMA_ENABLE_GET  , "ISPCORE_MODULE_RGBGAMMA_ENABLE_GET"},
//     { ISPCORE_MODULE_TDNR_ENABLE_GET      , "ISPCORE_MODULE_TDNR_ENABLE_GET    "},
//     { ISPCORE_MODULE_AHDR_ENABLE_GET      , "ISPCORE_MODULE_AHDR_ENABLE_GET    "},
//     { ISPCORE_MODULE_DG_ENABLE_GET        , "ISPCORE_MODULE_DG_ENABLE_GET      " }
//   { ISPCORE_MODULE_AF_ENABLE_GET       , "ISPCORE_MODULE_AF_ENABLE_GET      "},
//   { ISPCORE_MODULE_AVS_ENABLE_GET        , "ISPCORE_MODULE_AVS_ENABLE_GET     " }

};

UINT32 fmtConvert[NUCFG_FORMAT_NUM_FORMATS_E];

typedef enum ISP_MNGRG_ISP_STATUS_E
{
    ISP_MNGRG_ISP_IDLE_E       = 0,
    ISP_MNGRG_ISP_CONFIGURED_E = 1,
    ISP_MNGRG_ISP_ENABLED_E    = 2,
} ISP_MNGRG_ISP_STATUS_E;


typedef struct
{
    CAM_HARDWARE_PIPELINE_ID hwid;
    CAM_VIRTUAL_DEVICE_ID    virtid;
    ISPCORE_BUFIO_ID bufio;
}ISP_MNGRG_bufioT;

typedef struct
{
    void *me;
    unsigned int ispRdNum;
    unsigned int ispNum;
    ISP_MNGRG_CbT cb;
}ISP_MNGRG_threadParamsT;

typedef struct
{
    UINT32              phyAddress;
    MEM_POOLG_bufDescT  *bufDescP;
}ISP_MNGRG_bufT;

typedef struct
{
    UINT32                  ispBaseAddress;
    ISP_MNGRG_threadParamsT threadParams[ISP_BUFIO_MAX];
    ISP_MNGRG_ISP_STATUS_E  ispStatus;
    CAM_DEVICE cam_dev = CAM_DEVICE();
    MEM_POOLG_handleT       memPoolHandle[ISP_BUFIO_MAX];
    ISP_MNGRG_bufT          bufList[ISP_BUFIO_MAX][ISP_NUM_BUFFERS];
    UINT32                  runningFlag[ISP_BUFIO_MAX];
    UINT32                  numFramesToSkip;
#ifdef TEMP_SOLUTION_BEFORE_STOP_FIXING
    UINT32                  numActivteReadersCtr;
#endif
    UINT32                  frameCnt[ISP_BUFIO_MAX];
    char                    sensName[10];

}ISP_MNGRG_privT;

typedef struct
{
    UINT32 addr;
    UINT32 data;
}ISP_MNGRG_regT;

static UINT32 ipeBaseAddress=0;
static UINT32 ddrcMpBaseAddress=0;
static ISP_MNGRG_privT ispPriv[NUM_ISPS];


ISP_MNGRG_bufioT reader2bufio[NUM_OF_ISP_READERS] =
{
    {
        .hwid = CAM_HWPPLINE_ID_0,
        .virtid = CAM_VDEV_ID_0,
        .bufio = ISPCORE_BUFIO_MP,
    },
    {
        .hwid = CAM_HWPPLINE_ID_0,
        .virtid = CAM_VDEV_ID_0,
        .bufio = ISPCORE_BUFIO_SP1,
    },
    {
        .hwid = CAM_HWPPLINE_ID_0,
        .virtid = CAM_VDEV_ID_0,
        .bufio = ISPCORE_BUFIO_SP2,
    },
    {
        .hwid = CAM_HWPPLINE_ID_0,
        .virtid = CAM_VDEV_ID_1,
        .bufio = ISPCORE_BUFIO_MP,
    },
    {
        .hwid = CAM_HWPPLINE_ID_0,
        .virtid = CAM_VDEV_ID_1,
        .bufio = ISPCORE_BUFIO_SP1,
    },
    {
        .hwid = CAM_HWPPLINE_ID_0,
        .virtid = CAM_VDEV_ID_1,
        .bufio = ISPCORE_BUFIO_SP2,
    },

};

#ifdef __REGISTER_TEST__

ISP_MNGRG_regT generator_config[] =
{
    {0x00001200, 0x00000000,},
    {0x00000010, 0x00019f7b,},
    {0x00000014, 0x00020000,},
    {0x00000704, 0x0b13a3cd,},
    {0x00000708, 0x01e00438,},
    {0x0000070c, 0x0a0d20e0,},
    {0x00000710, 0x02df9333,},
    {0x00000714, 0x0f38e935,},
    {0x00000718, 0x0382f243,},
    {0x0000071c, 0x000033ff,},
    {0x00000720, 0xd554db9e,},
    {0x00000730, 0x00002c71,},
    {0x00000704, 0x022a45b9,},
    {0x00000708, 0x01e00438,},
    {0x0000070c, 0x002cc07a,},
    {0x00000710, 0x000f40cf,},
    {0x00000714, 0x000e4038,},
    {0x00000718, 0x00a00168,},
    {0x0000071c, 0x000000f0,},
    {0x00000720, 0x0d06ed9e,},
    {0x00000730, 0x000115bd,},
    {0x00001200, 0x00000000,},
    {0x00001204, 0x04380780,},
    {0x00001208, 0x04380780,},
    {0x00001300, 0x00000001,},
    {0x00001308, 0x6ce60608,},
    {0x00001310, 0x00000030,},
    {0x00001314, 0x000008c4,},
    {0x00001318, 0x00000001,},
    {0x0000131c, 0x03a2013b,},
    {0x00001320, 0x00000000,},
    {0x00001324, 0x00000000,},
    {0x00001328, 0x001fa400,},
    {0x0000132c, 0x00000000,},
    {0x00001330, 0x00000780,},
    {0x00001334, 0x00000780,},
    {0x00001338, 0x00000438,},
    {0x0000133c, 0x001fa400,},
    {0x00001340, 0x001fa400,},
    {0x00001344, 0x001fa400,},
    {0x00001348, 0x00000000,},
    {0x0000134c, 0x003f4800,},
    {0x00001350, 0x0d050750,},
    {0x00001354, 0x00000000,},
    {0x00001358, 0x00000000,},
    {0x0000135c, 0x00000000,},
    {0x00001360, 0x00000000,},
    {0x00001394, 0x10000000,},
    {0x00001398, 0x007e9000,},
    {0x0000139c, 0x00000000,},
    {0x000013a0, 0x00000f00,},
    {0x000013a4, 0x00000780,},
    {0x000013a8, 0x00000438,},
    {0x000013ac, 0x003f4800,},
    {0x000013c0, 0x20000000,},
    {0x000013c4, 0x00001000,},
    {0x000013c8, 0x00000000,},
    {0x000013cc, 0x00001000,},
    {0x000013d0, 0x00000400,},
    {0x000013d4, 0x00000001,},
    {0x000013d8, 0x00001000,},
    {0x000013ec, 0x00000000,},
    {0x000016c0, 0x07ffffff,},
    {0x000016c4, 0x052c4e39,},
    {0x000016c8, 0x00000000,},
    {0x000016cc, 0x00000000,},
    {0x00001700, 0x1c283878,},
    {0x00001704, 0x1ff06370,},
    {0x00001708, 0x1eca9530,},
    {0x00000404, 0x00d00018,},
    {0x00000408, 0x00000000,},
    {0x0000040c, 0x00000000,},
    {0x00000410, 0x00000780,},
    {0x00000414, 0x00000438,},
    {0x00000400, 0x80106817,},
    {0x00000538, 0x01000100,},
    {0x0000053c, 0x02270220,},
    {0x00000594, 0x00000000,},
    {0x00000598, 0x00000000,},
    {0x0000059c, 0x00000780,},
    {0x000005a0, 0x00000438,},
    {0x00002310, 0x00000780,},
    {0x00002314, 0x00000438,},
    {0x0000295c, 0x00000070,},
    {0x00003e00, 0x040128be,},
    {0x00003e04, 0x00000000,},
    {0x00003e08, 0x00001f08,},
    {0x00003e0c, 0x200003ff,},
    {0x00003e10, 0x0c968628,},
    {0x00003e14, 0x00008008,},
    {0x00003e18, 0x007d07d0,},
    {0x00003e1c, 0x301a3012,},
    {0x00003e20, 0x04010000,},
    {0x00003e24, 0x22018000,},
    {0x00003e28, 0x00020000,},
    {0x00003e2c, 0x0210210a,},
    {0x00003e30, 0x00102102,},
    {0x00003e34, 0x0000388c,},
    {0x00003e38, 0x00000000,},
    {0x00003e3c, 0x00000000,},
    {0x00003e40, 0x00000000,},
    {0x00003e44, 0x00000001,},
    {0x00003e48, 0x10001000,},
    {0x00003e4c, 0x00000000,},
    {0x00003e50, 0x00000000,},
    {0x00003e54, 0x00000000,},
    {0x00003e58, 0x00080010,},
    {0x00003e5c, 0x00080010,},
    {0x00003e60, 0x039c0780,},
    {0x0000166c, 0x70000000,},
    {0x000015bc, 0x60000000,},
    {0x00001568, 0x60000000,},
    {0x00000018, 0x00007000,},
    {0x00001200, 0x00000001,},
    {0x00001310, 0x00000038,},
    {0x000014e4, 0x00000238,},
    {0x00001600, 0x0000005c,},
    {0x00000400, 0x80100697,},
    {0x00001300, 0x00000001,},
    {0x00000700, 0x000101a1,},
    {0x00005D00, 0x00000001,},
    {0x00001300, 0x00100821,}
};

#endif //__REGISTER_TEST__


///////////////////////////////////////////////////
//////////    Sample thread for test only
///////////////////////////////////////////////////

static OS_LYRG_threadHandle Sample_thrdH = NULL;
static OS_LYRG_threadHandle thrIspHandle = NULL;

void SampleCallback( void *pBuff, void *arg)
{
    printf("Hello from sample Callback Buff = %p\n", pBuff);

}


void initFormatConvertTbl()
{
    /************
    TODO: learn the ISP foramts and fill the right convert
    *************/
    //fmtConvert[NUCFG_FORMAT_GREY_16_E]     = ?
    fmtConvert[NUCFG_FORMAT_BAYER_16_E]    = CAMERA_PIX_FMT_RAW10;
    fmtConvert[NUCFG_FORMAT_RGB888_E]      = CAMERA_PIX_FMT_RGB888;
    fmtConvert[NUCFG_FORMAT_YUV422_8BIT_E] = CAMERA_PIX_FMT_YUV422I;//10
    fmtConvert[NUCFG_FORMAT_RAW8_E]        = CAMERA_PIX_FMT_RAW8;
    fmtConvert[NUCFG_FORMAT_RAW10_E]       = CAMERA_PIX_FMT_RAW10;
    fmtConvert[NUCFG_FORMAT_RAW12_E]       = CAMERA_PIX_FMT_RAW12;
    fmtConvert[NUCFG_FORMAT_YUV420_SEMI_PLANAR_E] = CAMERA_PIX_FMT_YUV420SP; // 7
}


ISP_MNGRG_privT* ISP_MNGRP_getIspPriv(unsigned int ispRdNum)
{
    return &(ispPriv[ispRdNum]);
}

ERRG_codeE ISP_MNGRG_ThreadInit(void *me, unsigned int ispRdNum, void *cb)
{
    char thread_name[OS_LYR_GP_MAX_THREAD_NAME_LEN];
    ERRG_codeE ret = OS_LYR__RET_SUCCESS;
    OS_LYRG_threadParams thrParams;
    UINT32 ispReader = ispRdNum - ISP_READERS_OFFSET;
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(GET_ISP_NUM(ispReader));
    ISPCORE_BUFIO_ID bufio = reader2bufio[ispReader].bufio;
    priv->threadParams[bufio] = {.me = me, .ispRdNum = ispReader, .ispNum = GET_ISP_NUM(ispReader), .cb = (ISP_MNGRG_CbT)cb,};

    //Launch interrupt handling thread
    thrParams.func    = (OS_LYRG_threadFunction)ISP_MNGRG_eventMonitor;
    thrParams.id      = OS_LYRG_ISP_MONITOR_THREAD_ID_E;
    thrParams.event   = NULL;
    thrParams.param   = &priv->threadParams[bufio];
    thrIspHandle   =  OS_LYRG_createThread(&thrParams);

    if(!thrIspHandle)
    {
        ret = ISP_MNGR__ERR_UNEXPECTED;
    }
    //set thread name for ps and trace with ISP id and reader number
    snprintf(thread_name, OS_LYR_GP_MAX_THREAD_NAME_LEN, "IspEvMon_%1d_%1d", priv->threadParams[bufio].ispNum, priv->threadParams[bufio].ispRdNum);
    pthread_setname_np((pthread_t)thrIspHandle, thread_name);
    return ret;
}

void ISP_MNGRG_ThreadClose()
{
    OS_LYRG_closeThread(thrIspHandle);
}

////////////////////////////////////////////////////////

//ddr_ports_12_13_config
void ISP_MNGRG_DdrConfig()
{
    DDRC_MP_PCFGR_12_RD_PORT_PRIORITY_W(0x40);
    DDRC_MP_PCFGR_12_RD_PORT_AGING_EN_W(0x1);
    DDRC_MP_PCFGR_12_RD_PORT_URGENT_EN_W(0x0);
    DDRC_MP_PCFGR_12_RD_PORT_PAGEMATCH_EN_W(0x1);

    DDRC_MP_PCFGW_12_WR_PORT_PRIORITY_W(0x40);
    DDRC_MP_PCFGW_12_WR_PORT_AGING_EN_W(0x1);
    DDRC_MP_PCFGW_12_WR_PORT_URGENT_EN_W(0x0);
    DDRC_MP_PCFGW_12_WR_PORT_PAGEMATCH_EN_W(0x1);


    DDRC_MP_PCFGR_13_RD_PORT_PRIORITY_W(0x40);
    DDRC_MP_PCFGR_13_RD_PORT_AGING_EN_W(0x1);
    DDRC_MP_PCFGR_13_RD_PORT_URGENT_EN_W(0x0);
    DDRC_MP_PCFGR_13_RD_PORT_PAGEMATCH_EN_W(0x1);

    DDRC_MP_PCFGW_13_WR_PORT_PRIORITY_W(0x40);
    DDRC_MP_PCFGW_13_WR_PORT_AGING_EN_W(0x1);
    DDRC_MP_PCFGW_13_WR_PORT_URGENT_EN_W(0x0);
    DDRC_MP_PCFGW_13_WR_PORT_PAGEMATCH_EN_W(0x1);

    // from Yossi
    DDRC_MP_PCFGWQOS0_12_WQOS_MAP_LEVEL1_W(0x0);  // enlarge region 2
    DDRC_MP_PCFGWQOS0_12_WQOS_MAP_LEVEL2_W(0x0);
    DDRC_MP_PCFGWQOS0_12_WQOS_MAP_REGION0_W(0x1); // VPW
    DDRC_MP_PCFGWQOS0_12_WQOS_MAP_REGION1_W(0x1); // VPW
    DDRC_MP_PCFGWQOS0_12_WQOS_MAP_REGION2_W(0x1); // VPW
    DDRC_MP_PCFGWQOS0_13_WQOS_MAP_LEVEL1_W(0x0);  // enlarge region 2
    DDRC_MP_PCFGWQOS0_13_WQOS_MAP_LEVEL2_W(0x0);
    DDRC_MP_PCFGWQOS0_13_WQOS_MAP_REGION0_W(0x1); // VPW
    DDRC_MP_PCFGWQOS0_13_WQOS_MAP_REGION1_W(0x1); // VPW
    DDRC_MP_PCFGWQOS0_13_WQOS_MAP_REGION2_W(0x1); // VPW


    DDRC_MP_PCTRL_12_VAL = 0x00000001; // port_en=1
    DDRC_MP_PCTRL_13_VAL = 0x00000001; // port_en=1

}


void ISP_W (UINT32 address,UINT32 value, UINT32 ispNum)
{
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(ispNum);

    printf("ISP write: address 0x%08x = 0x%08x\n", address, value);
    *(UINT32*)(priv->ispBaseAddress + ((UINT32)address & 0x00FFFFFF))=(UINT32)value;
}


UINT32 ISP_R (UINT32 address, UINT32 ispNums)
{
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(ispNums);
    volatile UINT32 rvalue = *(UINT32*)(priv->ispBaseAddress + ((UINT32)address & 0x00FFFFFF));

    return rvalue;
}



int get_hw_resources(CAM_HARDWARE_PIPELINE_ID isp_id)
{
    int result;
    Json::Value request;
    Json::Value response;
    result = CAM_DEVICE::getHwResources(request, response);
    if(result != 0)
    {
        printf("getHwResources failed\n");
        return -1;
    }

    if(response["resource"].isNull() == 1)
    {
        printf("getHwResources failed\n");
        return -1;
    }

    int isp_max = response["resource"].asUInt();
    if (isp_id > isp_max)
    {
        printf("resource count failed\n");
        return -1;
    }
    return result;
}

int check_hw_status(CAM_HARDWARE_PIPELINE_ID isp_id)
{
    int result;
    Json::Value request;
    Json::Value response;
    result = CAM_DEVICE::getHwStatus(request, response);
    if(result != 0)
    {
        printf("getHwStatus failed\n");
        return -1;
    }

    if(response["status"].isNull() == 1)
    {
        printf("getHwStatus failed\n");
        return -1;
    }

    int isp_status = response["status"].asUInt();
    if (isp_status != 1)
    {
        printf("resource inuse,return\n");
        return -1;
    }

    return result;
}

int get_isp_features(CAM_HARDWARE_PIPELINE_ID isp_id)
{
    int result;
    Json::Value request;
    Json::Value response;
    result = CAM_DEVICE::getIspFeature(request, response);
    if(result != 0)
    {
        printf("getIspFeature failed\n");
        return -1;
    }

    if(response["feature"].isNull() == 1)
    {
        printf("getIspFeature failed\n");
        return -1;
    }

    unsigned int isp_feature = response["status"].asUInt();
    //printf("isp feature: %d\n", isp_feature);

    //Todo: print mask bits.

    return result;
}


int get_isp_versions(void)
{
    int result;
    Json::Value request;
    Json::Value response;

    result = CAM_DEVICE::getVersions(request, response);
    if(result != 0)
    {
        printf("getIspFeature failed\n");
        return -1;
    }

    if(response["native_api_version"].isNull() == 1)
    {
        printf("getIspFeature failed\n");
        return -1;
    }

    std::string versions = response["native_api_version"].asString();
    //printf("ISP_Versions: %s\n", versions.c_str());

    return result;
}



ERRG_codeE ISP_MNGRG_init(UINT32 isp0_base, UINT32 isp1_base, UINT32 ipe_base, UINT32 ddr_base)
{
    int ret = 0;

    memset(ispPriv,0,sizeof(ispPriv));
    ispPriv[0].ispBaseAddress = isp0_base;
    ispPriv[1].ispBaseAddress = isp1_base;
    ipeBaseAddress = ipe_base;
    ddrcMpBaseAddress = ddr_base;
    initFormatConvertTbl();
    //printf("ISP_MNGRG_init:: Setting reg. ISP0 Base = 0x%x ISP1 Base = 0x%x.  DDr Base = 0x%x\n", ispPriv[0].ispBaseAddress, ispPriv[1].ispBaseAddress,ddrcMpBaseAddress);


    return (ret == 0) ? ISP_MNGR__RET_SUCCESS: ISP_MNGR__ERR_OUT_OF_RSRCS;
}

ERRG_codeE ISP_MNGRG_setEnableIpe(unsigned int ispRdNum)
{

    LOGG_PRINT(LOG_DEBUG_E, NULL, "enable ipe\n");

    IPE_ENABLE_IPE_EN_W(1);

    return ISP_MNGR__RET_SUCCESS;
}

ERRG_codeE ISP_MNGRG_setDisableIpe(unsigned int ispRdNum)
{
//in case the connectivity is from slu, it is enough to disable with IPE_EN register only.
//at the other cases of inputs, it should be with IPE_ENABLE_ISP_EN as well, per isp instance.

    LOGG_PRINT(LOG_DEBUG_E, NULL, "disable ipe \n");

    IPE_ENABLE_IPE_EN_W(0);

    return ISP_MNGR__RET_SUCCESS;
}

static int ar2020_img_type = -1;

ERRG_codeE ISP_MNGRG_setImgType(unsigned int img_type)
{
    LOGG_PRINT(LOG_DEBUG_E, NULL, "disable ipe \n");
    ar2020_img_type = img_type;

    return ISP_MNGR__RET_SUCCESS;
}

ERRG_codeE ISP_MNGRG_setConfig(unsigned int ispRdNum, char *senStr, ISP_MNGRG_streamReadCfgT cfg)
{
    ERRG_codeE retval = ISP_MNGR__RET_SUCCESS;
#if !defined(__REGISTER_TEST__)
    void * dev_handle;
    int result;
    char calibStr[60];
    CAM_HARDWARE_PIPELINE_ID ispNum = reader2bufio[ispRdNum].hwid;
    ISPCORE_BUFIO_ID bufio = reader2bufio[ispRdNum].bufio;
    CAM_VIRTUAL_DEVICE_ID virtId = reader2bufio[ispRdNum].virtid;
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(GET_ISP_NUM(ispRdNum));
    CAM_DEVICE *cam_dev = &priv->cam_dev;
#ifdef ISP_BUFFER_ALIGN_SIZE
    UINT32 metadata_size;
#endif

    priv->numFramesToSkip = cfg.numFramesToSkip;
    strcpy(priv->sensName, senStr);
    if (ar2020_img_type >= 0)
    	sprintf(calibStr,"/media/inuitive/%s_%d.xml",senStr,ar2020_img_type );
    else
    	sprintf(calibStr,"/media/inuitive/%s.xml",senStr );

#ifdef TEMP_SOLUTION_BEFORE_STOP_FIXING
    LOGG_PRINT(LOG_DEBUG_E, NULL, "ispRdNum %d isp num %d virtId %d cfg.numActiveReaders %d num frames to skip %d\n",ispRdNum, ispNum, virtId,cfg.numActiveReaders, priv->numFramesToSkip);

    if (priv->numActivteReadersCtr == 0)//it means this is first channel for this ISP
#endif
    {
        //ddr_ports_12_13_config
        ISP_MNGRG_DdrConfig();
#if 1
        get_hw_resources(ispNum);

        check_hw_status(ispNum);
        get_isp_features(ispNum);
        get_isp_versions();
#endif
        //Configure device
        camdev_work_cfg devcfg;
        devcfg.work_mode = CAMDEV_STREAM_MODE;
        result = cam_dev->initHardware(ispNum, virtId, &dev_handle, &devcfg);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "cam_dev.initHardwarex DONE. Result = %d ispNum %d virtId %d\n", result, ispNum, virtId);

        if (result != 0)
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "cam_dev.initHardware return error %d\n", result);
            return ISP_MNGR__ERR_UNEXPECTED;
        }

        {
            Json::Value jRequest, jResponse;
            jRequest[KEY_CALIBRATION_FILE] = calibStr;
            result = cam_dev->ioctl(ISPCORE_MODULE_DEVICE_CALIBRATIONLOADANDINIT, jRequest, jResponse);
            LOGG_PRINT(LOG_DEBUG_E, NULL, "ISPCORE_MODULE_DEVICE_CALIBRATIONLOADANDINIT DONE. Result = %d\n", result);
        }
    }
#endif //!defined(__REGISTER_TEST__)
    MEM_POOLG_cfgT      poolCfg;
    MEM_POOLG_handleT   memPoolHandle;
    MEM_POOLG_bufDescT  *bufDescP = NULL;
    UINT32              phyAddress, bpp;
    bpp =  (cfg.bpp % 8 == 0) ? (cfg.bpp / 8) : ((cfg.bpp / 8) + 1);
    //printf("ISP_MNGRG_setConfig width %d h %d bpp %d Bpp %d\n",cfg.width , cfg.height , cfg.bpp, bpp);
    poolCfg.numBuffers = ISP_NUM_BUFFERS;//TBD: we have to measure and define numOfBuff
    poolCfg.bufferSize = (cfg.width * cfg.height * bpp);
#ifdef ISP_BUFFER_ALIGN_SIZE
    // isp buffer requires 1024 bytes align?? wood@2023.12.27
    metadata_size = inu_metadata__getNumberMetadataRows() * cfg.width * bpp;
    metadata_size = METADATA_SIZE_ALIGN(metadata_size);
    poolCfg.bufferSize += metadata_size;
    printf("ISP_MNGRG_setConfig width %d h %d bpp %d Bpp %d meta size:%u buffersize:%u\n",cfg.width , cfg.height , cfg.bpp, bpp, metadata_size, poolCfg.bufferSize);
#endif
    //poolCfg.bufferSize = (1920 * 1080 * 2);
    poolCfg.freeCb = NULL;
    poolCfg.type = MEM_POOLG_TYPE_ALLOC_CMEM_E;
    poolCfg.memP = NULL;
    poolCfg.freeArg = NULL;
    poolCfg.resetBufPtrInAlloc = 0;
    retval = MEM_POOLG_initPool(&memPoolHandle, &poolCfg);
    if (ERRG_FAILED(retval))
    {
        LOGG_PRINT(LOG_INFO_E, NULL,"pool init failed. ret = %x \n", retval);
        return retval;
    }
    priv->memPoolHandle[bufio] = memPoolHandle;

#if !defined(__REGISTER_TEST__)
    BUFF_MODE buff_mode = BUFF_MODE_USRPTR; //BUFF_MODE_USRPTR //BUFF_MODE_PHYLINEAR
    result = cam_dev->setBufferParameters(buff_mode);

    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_BUFIO_ID] = bufio;
        jRequest[KEY_WIDTH] = cfg.width;
        jRequest[KEY_HEIGHT] = cfg.height;
        jRequest[KEY_FORMAT] = fmtConvert[cfg.format];
        result = cam_dev->ioctl(ISPCORE_MODULE_DEVICE_FORMAT_SET, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "ISPCORE_MODULE_DEVICE_FORMAT_SET DONE. Result = %d\n", result);
    }

    cam_dev->initBufferPoolCtrl(bufio);

    for(UINT32 buf_idx = 0; buf_idx < poolCfg.numBuffers; buf_idx++)
    {
        BufIdentity * p_buf = new BufIdentity;
        //unsigned int allocate_buf_size = ALIGN_UP(poolCfg.bufferSize, align_mask);// [Dima] - What alignment do we need on the buffer

        //unsigned int buffer_address_reg = buffer_base_address + offset_used;
        //offset_used = offset_used + allocate_buf_size;

        retval = MEM_POOLG_alloc(memPoolHandle, MEM_POOLG_getBufSize(memPoolHandle), &bufDescP);
        if (ERRG_FAILED(retval))
        {
            LOGG_PRINT(LOG_INFO_E, NULL,"allocation failed ret = %x \n", retval);
            return retval;
        }
        MEM_POOLG_getDataPhyAddr(bufDescP,&phyAddress);
#ifdef ISP_BUFFER_ALIGN_SIZE
        phyAddress += metadata_size;
#endif
        priv->bufList[bufio][buf_idx].phyAddress = phyAddress;
        priv->bufList[bufio][buf_idx].bufDescP = bufDescP;


        p_buf->buff_size = MEM_POOLG_getBufSize(memPoolHandle);
        p_buf->buff_address = phyAddress;
#ifdef ISP_BUFFER_ALIGN_SIZE
        p_buf->buff_size -= metadata_size;
#endif


        result = cam_dev->bufferPoolAddEntity(bufio, p_buf);

        LOGG_PRINT(LOG_DEBUG_E, NULL, "bufferPoolAddEntity DONE. Result = %d\n", result);
    }
    result = cam_dev->bufferPoolIplMap(bufio, true);
    LOGG_PRINT(LOG_DEBUG_E, NULL, "bufferPoolIplMap DONE. Result = %d\n", result);
    result = cam_dev->bufferPoolSetBufToEngine(bufio);
    LOGG_PRINT(LOG_DEBUG_E, NULL, "bufferPoolSetBufToEngine DONE. Result = %d\n", result);


    {
        Json::Value jRequest, jResponse;
        //printf("ISP_MNGRG_setConfig sen %s calib str %s\n",senStr,calibStr);

         //we added to VSI code, in open() function new arguments: senName and calib path.
        jRequest[KEY_DRIVER_FILE] = senStr;
        jRequest[KEY_CALIBRATION_FILE] = calibStr;
        jRequest[KEY_SENSOR_DEVICEID] = 0;//dev_id (????);
        jRequest[KEY_SENSOR_I2C_NUMBER] = 0; //tbd remove
        result = cam_dev->ioctl(ISPCORE_MODULE_SENSOR_DRV_CHANGE, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "ISPCORE_MODULE_SENSOR_DRV_CHANGE DONE. Result = %d\n", result);
    }

#if 0
    //get sensor capabilities
    {
        Json::Value jRequest, jResponse;
        result = cam_dev.ioctl(ISPCORE_MODULE_SENSOR_CAPS, jRequest, jResponse);
        LOGG_PRINT((LOG_DEBUG_E, NULL, "ISPCORE_MODULE_SENSOR_CAPS DONE. Result = %d\n"), result);
    }

    {
        Json::Value jRequest, jResponse;
        result = cam_dev.ioctl(ISPCORE_MODULE_DEVICE_INPUTINFO, jRequest, jResponse);
        int sensor_size = jResponse[DEVICE_COUNT_PARAMS].asInt();
        int input_type = jRequest[DEVICE_INDEX_PARAMS].asInt();
        LOGG_PRINT((LOG_DEBUG_E, NULL, "ISPCORE_MODULE_DEVICE_INPUTINFO DONE. Result = %d\n"), result);
    }
#endif


    int frame_skip_interval = 0;
    result = cam_dev->initBufChain(bufio, frame_skip_interval);
    result = cam_dev->startBufChain(bufio);
    result = cam_dev->attachBufChain(bufio);
    LOGG_PRINT(LOG_DEBUG_E, NULL, "attachChain DONE. Result = %d\n", result);

    priv->ispStatus = ISP_MNGRG_ISP_CONFIGURED_E;



#else    //defined(__REGISTER_TEST__)
    UINT32 reg_count = sizeof(generator_config)>>3;
    UINT32 j = 0;
    printf(GREEN("ISP_MNGRG_setConfig:: Start = %d\n"), reg_count);

    while( j < reg_count )
    {
#if 1    //Write allocated buffer addr to the hw
        if((generator_config[j].addr == 0x00001324) || (generator_config[j].addr == 0x00001340))    // Y and Cb buffer
        {
            retval = MEM_POOLG_alloc(memPoolHandle, MEM_POOLG_getBufSize(memPoolHandle), &bufDescP);
            if (ERRG_FAILED(retval))
            {
                LOGG_PRINT(LOG_INFO_E, NULL,"allocation failed ret = %x \n", retval);
                return retval;
            }
            MEM_POOLG_getDataPhyAddr(bufDescP,&phyAddress);

            ISP_W(generator_config[j].addr, phyAddress, ispNum);


        }
        else
#endif
        {
            ISP_W(generator_config[j].addr, generator_config[j].data, ispNum);
        }
        j++;
    }

#endif //!defined(__REGISTER_TEST__)

    priv->runningFlag[bufio] = 1;

    return retval;
}


static ERRG_codeE ISP_MNGRG_getAllEnables(unsigned int ispRdNum)
{
    ERRG_codeE retval = ISP_MNGR__RET_SUCCESS;
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(GET_ISP_NUM(ispRdNum));
    if ( (priv==NULL) ||  (priv->ispStatus < ISP_MNGRG_ISP_CONFIGURED_E) )
    {
        LOGG_PRINT(LOG_DEBUG_E, NULL, "could not set flicker, too early\n");
        return retval;
    }
    CAM_DEVICE* cam_dev = &priv->cam_dev;

    int result;
    UINT32 i = 0;

    for (i = 0; i < (sizeof(getCommands)/sizeof(ISP_MNGRG_funcGetT)); i++)
    {
        /*Json::Value jRequest, jResponse;

        result = cam_dev->ioctl(getCommands[i].cmd, jRequest, jResponse);

        switch(getCommands[i].cmd)
        {
            case ISPCORE_MODULE_BLS_CFG_GET:
            {
                clb_itf::Bls::Config config;
                t_common::Base64::decode(jResponse[KEY_CONFIG].asString(), config);
                LOGG_PRINT(LOG_INFO_E, NULL, "%s DONE. ISPCORE_MODULE_BLS_CFG_GET:red[%d],greenR[%d],greenB[%d],blue[%d]  Result = %d\n",
                    getCommands[i].funcName, config.red, config.greenR, config.greenB, config.blue, result);
            }
            break;

            case ISPCORE_MODULE_2DNR_CFG_GET:
                LOGG_PRINT(LOG_INFO_E, NULL, "%s DONE. Enable = %d, Gen = %d  Result = %d\n",
                    getCommands[i].funcName, jResponse[KEY_ENABLE].asBool(), jResponse[KEY_GENERATION].asInt(), result);
            break;

            default:
                LOGG_PRINT(LOG_INFO_E, NULL, "%s DONE. Enable = %d, Result = %d\n",
                    getCommands[i].funcName, jResponse[KEY_ENABLE].asInt(), result);
            break;
        }
*/
    }

    return retval;

}


static ERRG_codeE ISP_MNGRP_aeGetEnable(unsigned int ispNum, UINT8* enable)
{
    ERRG_codeE retval = ISP_MNGR__RET_SUCCESS;
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(ispNum);
    if ( (priv==NULL) ||  (priv->ispStatus < ISP_MNGRG_ISP_CONFIGURED_E) )
    {
        LOGG_PRINT(LOG_DEBUG_E, NULL, "could not set flicker, too early\n");
        return retval;
    }

    CAM_DEVICE* cam_dev = &priv->cam_dev;

    int result;
    if( cam_dev != 0 )
    {
        Json::Value jRequest, jResponse;
        result = cam_dev->ioctl(ISPCORE_MODULE_AE_ENABLE_GET, jRequest, jResponse);
        LOGG_PRINT(LOG_INFO_E, NULL, "activate ISPCORE_MODULE_AE_ENABLE_GET DONE. Result = %d. Enable = %d\n", result, jResponse[KEY_ENABLE].asInt());
        * enable = jResponse[KEY_ENABLE].asInt();
        if(result != 0)
        {
            return ISP_MNGR__ERR_UNEXPECTED;
        }
    }
    return retval;

}

ERRG_codeE ISP_MNGRG_getEnableAllAe(UINT8* enable)
{
    ERRG_codeE retval = ISP_MNGR__RET_SUCCESS;
    UINT8 aeEnable = 0;

    for(UINT8 i = 0; i < NUM_ISPS; i++)
    {
        retval = ISP_MNGRP_aeGetEnable(i, &aeEnable);
        if(aeEnable)
        {
            *enable |= 1 << i;
        }
    }
    return retval;

}


static ERRG_codeE ISP_MNGRP_aeSetEnable(unsigned int ispNum, UINT8 enable)
{
    ERRG_codeE retval = ISP_MNGR__RET_SUCCESS;
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(ispNum);
    if ( (priv==NULL) ||  (priv->ispStatus < ISP_MNGRG_ISP_CONFIGURED_E) )
    {
        LOGG_PRINT(LOG_DEBUG_E, NULL, "could not set flicker, too early\n");
        return retval;
    }
    CAM_DEVICE* cam_dev = &priv->cam_dev;

    int result;
    if( cam_dev != 0 )
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = enable;
        result = cam_dev->ioctl(ISPCORE_MODULE_AE_ENABLE_SET, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_AE_ENABLE_SET DONE. Result = %d. Enable = %d\n", result, enable);
        if(result != 0)
        {
            return ISP_MNGR__ERR_UNEXPECTED;
        }
    }
    return retval;

}

ERRG_codeE ISP_MNGRG_setEnableAeByRdNum(unsigned int ispRdNum, UINT8 enable)
{
    UINT32 ispReader = ispRdNum - ISP_READERS_OFFSET;
    LOGG_PRINT(LOG_DEBUG_E, NULL, "ispRdNum = %d. Enable = %d\n", ispRdNum, enable);

    return ISP_MNGRP_aeSetEnable(GET_ISP_NUM(ispReader), enable);
}

ERRG_codeE ISP_MNGRG_setEnableAllAe(UINT8 enable)
{
    ERRG_codeE retval = ISP_MNGR__RET_SUCCESS;

    for(UINT8 i = 0; i < NUM_ISPS; i++)
    {
        retval = ISP_MNGRP_aeSetEnable(i, enable);
    }
    return retval;

}


ERRG_codeE ISP_MNGRG_setFlickerAe(UINT8 flickerPeriod)
{
    ERRG_codeE retval = ISP_MNGR__RET_SUCCESS;
    int result;

    for(UINT8 i = 0; i < NUM_ISPS; i++)
    {

        ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(i);
        if ( (priv==NULL) ||  (priv->ispStatus < ISP_MNGRG_ISP_CONFIGURED_E) )
        {
            LOGG_PRINT(LOG_DEBUG_E, NULL, "could not set flicker, too early\n");
            continue;
        }
        CAM_DEVICE* cam_dev = &priv->cam_dev;

        clb_itf::Ae::Config config;
        Json::Value jRequest, jResponse;

        retval = ISP_MNGRP_aeSetEnable(i, 0);

        result =  cam_dev->ioctl(ISPCORE_MODULE_AE_CFG_GET, jRequest, jResponse);
        t_common::Base64::decode(jResponse[KEY_CONFIG].asString(), config);

        config.v1.ecm.flickerPeriod = (clb_itf::Ae::Config::V2::Ecm::FlickerPeriod)flickerPeriod;

        jRequest[KEY_CONFIG] = t_common::Base64::encode(config);

        result =  cam_dev->ioctl(ISPCORE_MODULE_AE_CFG_SET, jRequest, jResponse);
        retval = ISP_MNGRP_aeSetEnable(i, 1);

    }

    return retval;

}

ERRG_codeE ISP_MNGRP_setDg(CAM_DEVICE* cam_dev, float gain)
{
    int result = 0;
    ERRG_codeE retVal = ISP_MNGR__RET_SUCCESS;
    clb_itf::Dg::Config config;
    Json::Value jRequest, jResponse;

    jRequest[KEY_GAIN] = gain;

    result = cam_dev->ioctl(ISPCORE_MODULE_DG_CFG_SET, jRequest, jResponse);
    if(result != 0)
    {
        retVal = ISP_MNGR__ERR_UNEXPECTED;
        LOGG_PRINT(LOG_ERROR_E, retVal,"ISPCORE_MODULE_DG_CFG_SET failed. result = %d\n", result);
    }
    else
    {
        LOGG_PRINT(LOG_DEBUG_E, NULL,"ISPCORE_MODULE_DG_CFG_SET sent successfully\n");
    }
    result = cam_dev->ioctl(ISPCORE_MODULE_DG_ENABLE, jRequest, jResponse);
    if(result != 0)
    {
        retVal = ISP_MNGR__ERR_UNEXPECTED;
        LOGG_PRINT(LOG_ERROR_E, retVal,"ISPCORE_MODULE_DG_ENABLE failed. result = %d\n", result);
    }
    return retVal;
}

ERRG_codeE ISP_MNGRP_getDg(CAM_DEVICE* cam_dev, float* gain)
{
    int result = 0;
    ERRG_codeE retVal = ISP_MNGR__RET_SUCCESS;
    clb_itf::Dg::Config config;
    Json::Value jRequest, jResponse;

    result = cam_dev->ioctl(ISPCORE_MODULE_DG_CFG_GET, jRequest, jResponse);
    if(result != 0)
    {
        retVal = ISP_MNGR__ERR_UNEXPECTED;
        LOGG_PRINT(LOG_ERROR_E, retVal,"ISPCORE_MODULE_DG_CFG_GET failed. result = %d\n", result);
    }
    else
    {
        t_common::Base64::decode(jResponse[KEY_CONFIG].asString(), config);
        LOGG_PRINT(LOG_DEBUG_E, retVal,"ISPCORE_MODULE_DG_CFG_GET: gain[%.2f]\n", config.gain);
        *gain = config.gain;
    }

}

ERRG_codeE ISP_MNGRP_setExposure(CAM_DEVICE* cam_dev, float integrationTime, float gain)
{
    int result = 0;
    ERRG_codeE retVal = ISP_MNGR__RET_SUCCESS;
    clb_itf::Sensor::Config config;
    Json::Value jRequest, jResponse;

    config._long.integrationTime = integrationTime;
    config._long.gain = gain;
    jRequest[KEY_CONFIG] = t_common::Base64::encode(config);

    result = cam_dev->ioctl(ISPCORE_MODULE_SENSOR_CFG_SET, jRequest, jResponse);
    if(result != 0)
    {
        retVal = ISP_MNGR__ERR_UNEXPECTED;
        LOGG_PRINT(LOG_ERROR_E, retVal,"ISPCORE_MODULE_SENSOR_CFG_SET failed. result = %d\n", result);
    }
    else
    {
        LOGG_PRINT(LOG_DEBUG_E, NULL,"ISPCORE_MODULE_SENSOR_CFG_SET sent successfully\n");
    }

    return retVal;
}

ERRG_codeE ISP_MNGRP_getExposure(CAM_DEVICE* cam_dev, inu_isp_channel__exposureParamT *exposureParams)
{
    int result = 0;
    ERRG_codeE retVal = ISP_MNGR__RET_SUCCESS;
    clb_itf::Sensor::Config config;
    Json::Value jRequest, jResponse;

    result = cam_dev->ioctl(ISPCORE_MODULE_SENSOR_CFG_GET, jRequest, jResponse);
    if(result != 0){
        retVal = ISP_MNGR__ERR_UNEXPECTED;
        LOGG_PRINT(LOG_ERROR_E, retVal,"ISPCORE_MODULE_SENSOR_CFG_GET failed. result = %d\n", result);
    }
    else
    {
        t_common::Base64::decode(jResponse[KEY_CONFIG].asString(), config);
        LOGG_PRINT(LOG_DEBUG_E, NULL,"ISPCORE_MODULE_SENSOR_CFG_GET:integrationTime[%.4f],gain[%.2f]\n", config._long.integrationTime, config._long.gain);

        exposureParams->analogGain = config._long.gain;
        exposureParams->integrationTime = config._long.integrationTime;
    }

}

ERRG_codeE ISP_MNGRG_setExposureParams(UINT32 ispRdNum, inu_isp_channel__exposureParamT *exposureParams)
{
    int result = 0;
    ERRG_codeE retVal = ISP_MNGR__RET_SUCCESS;
    CAM_DEVICE* cam_dev;
    UINT32 ispReader = ispRdNum - ISP_READERS_OFFSET;
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(GET_ISP_NUM(ispReader));
    MetaData3A metaData,response;

    if (priv == NULL)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "priv is NULL. Aborting.\n");
        return ISP_MNGR__ERR_UNEXPECTED;
    }

    cam_dev = &priv->cam_dev;
    memset(&metaData, 0, sizeof(metaData));

    {
        Json::Value jRequest, jResponse;
        metaData.aeMetaData.expsoureTime = exposureParams->integrationTime;
        metaData.aeMetaData.expsoureGain = exposureParams->analogGain;
        metaData.aeMetaData.ispDgain = exposureParams->ispGain;
        metaData.aeMetaData.aeSet = true;
        metaData.aeMetaData.metaDataEnableAE = exposureParams->metadataEnableAE;
        metaData.aeMetaData.hasmetaDataEnableAE = exposureParams->hasmetadataEnableAE;
        metaData.has_ae_cb = exposureParams->has_ae_cb;
        metaData.ae_cb = exposureParams->ae_cb;
        jRequest[KEY_METADATA] = t_common::Base64::encode(metaData);
        LOGG_PRINT(LOG_DEBUG_E,NULL," AE has Flag %lu, value= %lu \n", metaData.aeMetaData.hasmetaDataEnableAE,exposureParams->hasmetadataEnableAE );

        result = cam_dev->ioctl(ISPCORE_MODULE_3A_METADATA_SET, metaData, response);
        LOGG_PRINT(LOG_DEBUG_E, result, "ISPCORE_MODULE_3A_METADATA_SET done. result = %d\n", result);
        // Enable isp gain module
        result = cam_dev->ioctl(ISPCORE_MODULE_DG_ENABLE, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, result, "ISPCORE_MODULE_DG_ENABLE done. result = %d\n", result);
    }
    return retVal;
}
template<typename T_SRC, typename T_DST>
void copy_expo_Params(T_SRC &src, T_DST *dst )
{
    dst->integrationTime = src.aeMetaData.expsoureTime;
    dst->analogGain = src.aeMetaData.expsoureGain;
    dst->ispGain = src.aeMetaData.ispDgain;
}
ERRG_codeE ISP_MNGRG_getExposureParams(UINT32 ispRdNum, inu_isp_channel__exposureParamT *exposureParams)
{
    int result = 0;
    ERRG_codeE retVal = ISP_MNGR__RET_SUCCESS;
    CAM_DEVICE* cam_dev;
    UINT32 ispReader = ispRdNum - ISP_READERS_OFFSET;
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(GET_ISP_NUM(ispReader));

    if (priv == NULL)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "priv is NULL. Aborting.\n");
        return ISP_MNGR__ERR_UNEXPECTED;
    }

    cam_dev = &priv->cam_dev;
    {
        MetaData3A  request,response;
        clb_itf::MetaData3A metaData;
        memset(&metaData, 0, sizeof(metaData));
        request.aeMetaData.aeSet=true;
        result = cam_dev->ioctl(ISPCORE_MODULE_3A_METADATA_GET, request, response);
        copy_expo_Params<MetaData3A,inu_isp_channel__exposureParamT>(response,exposureParams);
        LOGG_PRINT(LOG_DEBUG_E, result, "ISPCORE_MODULE_3A_METADATA_GET done. result = %d, integrationTime=%.3f\n", result, response.aeMetaData.expsoureTime);
    }
    return retVal;
}

ERRG_codeE ISP_MNGRG_setWbParams(UINT32 ispRdNum, inu_isp_channel__ispWbParamT *wbParams)
{
    int result = 0;
    ERRG_codeE retVal = ISP_MNGR__RET_SUCCESS;
    CAM_DEVICE* cam_dev;
    UINT32 ispReader = ispRdNum - ISP_READERS_OFFSET;
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(GET_ISP_NUM(ispReader));
    clb_itf::Wb::Config config;

    if (priv == NULL)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "priv is NULL. Aborting.\n");
        return ISP_MNGR__ERR_UNEXPECTED;
    }

    cam_dev = &priv->cam_dev;

    {
        MetaData3A metaData,response;
        memset(&metaData, 0, sizeof(metaData));
        metaData.awbMetaData.awbGain[0] = wbParams->wbGain.gainR;
        metaData.awbMetaData.awbGain[1] = wbParams->wbGain.gainGr;
        metaData.awbMetaData.awbGain[2] = wbParams->wbGain.gainGb;
        metaData.awbMetaData.awbGain[3] = wbParams->wbGain.gainB;

        metaData.awbMetaData.offset[0] = wbParams->ccOffset.blue;
        metaData.awbMetaData.offset[1] = wbParams->ccOffset.green;
        metaData.awbMetaData.offset[2] = wbParams->ccOffset.red;

        uint32_t size = sizeof(metaData.awbMetaData.CCM)/sizeof(metaData.awbMetaData.CCM[0]);
        for (uint32_t i = 0; i< size; i++) {
            metaData.awbMetaData.CCM[i] = wbParams->ccMatrix.coeff[i];
        }
        metaData.awbMetaData.awbSet = true;
        metaData.awbMetaData.metaDataEnableAWB = wbParams->metadataEnableAWB;
        metaData.awbMetaData.hasmetaDataEnableAWB = wbParams->hasmetadataEnableAWB;
        metaData.has_awb_cb = wbParams->has_awb_cb;
        metaData.awb_cb = wbParams->awb_cb;
        LOGG_PRINT(LOG_DEBUG_E,NULL," AWB has Flag %lu, value= %lu \n", wbParams->hasmetadataEnableAWB,wbParams->metadataEnableAWB );

        //printf("ISP_MNGRG_setWbParams: wbGains.Blue:[%.3f], wbGains.GreenB:[%.3f], wbGains.GreenR:[%.3f], wbGains.Red:[%.3f]\n",
        //    metaData.awbMetaData.awbGain[0],metaData.awbMetaData.awbGain[1],metaData.awbMetaData.awbGain[2],metaData.awbMetaData.awbGain[3]);
        //printf("ISP_MNGRG_setWbParams: ccOffset.Blue:[%.3f], ccOffset.Green:[%.3f], ccOffset.Red:[%.3f]\n",
        //    metaData.awbMetaData.offset[0], metaData.awbMetaData.offset[1], metaData.awbMetaData.offset[2]);
        //size = sizeof(metaData.awbMetaData.CCM)/sizeof(metaData.awbMetaData.CCM[0]);
        //for (uint32_t i = 0; i< size; i++) {
        //    printf("ISP_MNGRG_setWbParams: cc_coeff[%d]: %.3f\n",i, metaData.awbMetaData.CCM[i]);
        //}


        result = cam_dev->ioctl(ISPCORE_MODULE_3A_METADATA_SET, metaData, response);
        if(result != 0){
            retVal = ISP_MNGR__ERR_UNEXPECTED;
            LOGG_PRINT(LOG_ERROR_E, retVal,"ISPCORE_MODULE_3A_METADATA_SET failed. result = %d\n", result);
        }
        else
        {
            LOGG_PRINT(LOG_DEBUG_E, NULL, "ioctl ISPCORE_MODULE_3A_METADATA_SET DONE. Result = %d\n", result);
        }
    }
    return retVal;
}
template<typename T_SRC, typename T_DST>
void copy_wb_Params(T_SRC &src, T_DST *dst )
{
    dst->wbGain.gainR    =   src.awbMetaData.awbGain[0];
    dst->wbGain.gainGr   =   src.awbMetaData.awbGain[1];
    dst->wbGain.gainGb   =   src.awbMetaData.awbGain[2];
    dst->wbGain.gainB    =   src.awbMetaData.awbGain[3];

    dst->ccOffset.blue   =   src.awbMetaData.offset[0] ;
    dst->ccOffset.green  =   src.awbMetaData.offset[1] ;
    dst->ccOffset.red    =   src.awbMetaData.offset[2] ;

    uint32_t size = sizeof(src.awbMetaData.CCM)/sizeof(src.awbMetaData.CCM[0]);
    for (uint32_t i = 0; i< size; i++) {
        dst->ccMatrix.coeff[i] = src.awbMetaData.CCM[i];
    }
}

ERRG_codeE ISP_MNGRG_getWbParams(UINT32 ispRdNum, inu_isp_channel__ispWbParamT *wbParams)
{
    int result = 0;
    ERRG_codeE retVal = ISP_MNGR__RET_SUCCESS;
    CAM_DEVICE* cam_dev;
    UINT32 ispReader = ispRdNum - ISP_READERS_OFFSET;
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(GET_ISP_NUM(ispReader));
    clb_itf::Wb::Config config;

    if (priv == NULL)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "priv is NULL. Aborting.\n");
        return ISP_MNGR__ERR_UNEXPECTED;
    }

    cam_dev = &priv->cam_dev;

    {
        MetaData3A  request,response;
        clb_itf::MetaData3A metaData;
        memset(&metaData, 0, sizeof(metaData));
        request.awbMetaData.awbSet=true;
        result = cam_dev->ioctl(ISPCORE_MODULE_3A_METADATA_GET, request, response);
        copy_wb_Params<MetaData3A,inu_isp_channel__ispWbParamT>(response,wbParams);

    
        LOGG_PRINT(LOG_DEBUG_E, NULL, "ioctl ISPCORE_MODULE_3A_METADATA_GET DONE. Result = %d\n", result);
    }

    return retVal;
}

ERRG_codeE ISP_MNGRG_setLscParams(UINT32 ispRdNum, inu_isp_channel__ispLscParamT *lscParams)
{
    int result = 0;
    Json::Value jRequest, jResponse;
    ERRG_codeE retVal = ISP_MNGR__RET_SUCCESS;
    CAM_DEVICE* cam_dev;
    UINT32 ispReader = ispRdNum - ISP_READERS_OFFSET;
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(GET_ISP_NUM(ispReader));

    if (priv == NULL)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "priv is NULL. Aborting.\n");
        return ISP_MNGR__ERR_UNEXPECTED;
    }

    // Configure LSC
    clb_itf::MetaData3A metaData;
    if (1/*lscParams->set_by_file*/)
    {
        char lscFilePath[60];
        memset(&metaData, 0, sizeof(metaData));
        sprintf(lscFilePath,"/media/inuitive/%s", lscParams->file_name);
        LOGG_PRINT(LOG_INFO_E, NULL, "lscFilePath %s\n",lscFilePath);
        std::ifstream lsccfg(lscFilePath, std::ifstream::binary);
        if (priv->sensName !=NULL)
            LOGG_PRINT(LOG_DEBUG_E, NULL, "%s",priv->sensName);
        else
            LOGG_PRINT(LOG_ERROR_E, NULL, "null");

        if (lsccfg.fail()) {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to open the JSON file." );
            return ISP_MNGR__ERR_UNEXPECTED;
        }
        Json::Value rootNode;
        lsccfg >> rootNode;

        if (!rootNode.isObject()) {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Invalid JSON structure: The root is not an object.");
            return ISP_MNGR__ERR_UNEXPECTED;
        }

        Json::Value lscConfig = rootNode["lscConfig"];
        if (!lscConfig.isObject()) {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Invalid JSON structure: 'lscConfig' is not an object.");
            return ISP_MNGR__ERR_UNEXPECTED;
        }
        Json::Value red = lscConfig["LSC_SAMPLES_red"];
        Json::Value greenR = lscConfig["LSC_SAMPLES_greenR"];
        Json::Value greenB = lscConfig["LSC_SAMPLES_greenB"];
        Json::Value blue = lscConfig["LSC_SAMPLES_blue"];

        /*// print debug:
        std::cout << "Red Value: " << red << std::endl;
        std::cout << "GreenR Value: " << greenR << std::endl;
        std::cout << "GreenB Value: " << greenB << std::endl;
        std::cout << "Blue Value: " << blue << std::endl;*/

        if (!red.isArray() || !greenR.isArray() || !greenB.isArray() || !blue.isArray()) {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Invalid JSON structure: One or more arrays not found in the JSON data.");
            return ISP_MNGR__ERR_UNEXPECTED;
        }

        for (unsigned int index = 0; index < ISP_MNGR_LSC_LENGTH; ++index) {
            metaData.lscMetaData.lscTable[index] = red[index].asUInt();
            metaData.lscMetaData.lscTable[ISP_MNGR_LSC_LENGTH+index] = greenR[index].asUInt();
            metaData.lscMetaData.lscTable[ISP_MNGR_LSC_LENGTH*2+index] = greenB[index].asUInt();
            metaData.lscMetaData.lscTable[ISP_MNGR_LSC_LENGTH*3+index] = blue[index].asUInt();
        }
        /*for(uint32_t i = 0; i < 8; i++) //tbd - pavel- grid values
        {
                lsc.grid.LscXGradTbl[i] =0.asUInt();
                lsc.grid.LscYGradTbl[i] =0.asUInt();
                lsc.grid.LscXSizeTbl[i] =0.asUInt();
                lsc.grid.LscYSizeTbl[i] =0.asUInt();
        }*/
        // Clean up
        lsccfg.close();
        LOGG_PRINT(LOG_INFO_E, retVal,"set_by_file value = %d, file name =%s\n", lscParams->set_by_file, lscParams->file_name);
    }
	else
	{
		//buffer treatment
	}
    metaData.lscMetaData.lscSet = true;
    cam_dev = &priv->cam_dev;

    //apply the ioctl to ISP
    jRequest[KEY_METADATA] = t_common::Base64::encode(metaData);
    result = cam_dev->ioctl(ISPCORE_MODULE_3A_METADATA_SET, jRequest, jResponse);
    LOGG_PRINT(LOG_INFO_E, result, "ISPCORE_MODULE_3A_METADATA_SET  (LSC)   done. result = %d\n", result);

    result = cam_dev->ioctl(ISPCORE_MODULE_LSC_ENABLE, jRequest, jResponse);
    LOGG_PRINT(LOG_INFO_E, result, "ISPCORE_MODULE_LSC_ENABLE  (LSC)   done. result = %d\n", result);

    return retVal;
}

ERRG_codeE ISP_MNGRG_getLscParams(UINT32 ispRdNum, inu_isp_channel__ispLscParamT *lscParams)
{
    int result = 0;
    ERRG_codeE retVal = ISP_MNGR__RET_SUCCESS;
    CAM_DEVICE* cam_dev;
    UINT32 ispReader = ispRdNum - ISP_READERS_OFFSET;
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(GET_ISP_NUM(ispReader));
    clb_itf::Lsc::Config lsc_get;
    clb_itf::MetaData3A metaData;
    unsigned int index=0;
    UINT32 i = 0;

    memset(&metaData, 0, sizeof(metaData));

    if (priv == NULL)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "priv is NULL. Aborting.\n");
        return ISP_MNGR__ERR_UNEXPECTED;
    }

    Json::Value jRequest, jResponse;
    cam_dev = &priv->cam_dev;
    metaData.lscMetaData.lscSet=true;

    jRequest[KEY_METADATA] = t_common::Base64::encode(metaData);
    result = cam_dev->ioctl(ISPCORE_MODULE_3A_METADATA_GET, jRequest, jResponse);
    t_common::Base64::decode(jResponse[KEY_METADATA].asString(), metaData);
    LOGG_PRINT(LOG_INFO_E, result, "ISPCORE_MODULE_3A_METADATA_GET done. result = %d\n", result);

    printf("LSC Get <<<<<<<<<<<<<--\n");
    for( i = 0; i < ISP_MNGR_LSC_LENGTH*4 ; i+=4 )
    {
        printf("\t%06d\t%06d\t%06d\t%06d\t\n",metaData.lscMetaData.lscTable[i], metaData.lscMetaData.lscTable[i+1], metaData.lscMetaData.lscTable[i+2], metaData.lscMetaData.lscTable[i+3]);
    }
    printf("\n");
    if (1/*lscParams->set_by_file*/)
    {
        char lscFilePath[32];
        sprintf(lscFilePath,"/media/inuitive/lsc_values_get.json");
        FILE *file;
        // Open a file for writing. If the file doesn't exist, it will be created.
        file = fopen(lscFilePath, "w");

        // Check if the file was opened successfully
        if (file == NULL) {
            LOGG_PRINT(LOG_ERROR_E, result, "Failed to open the file.\n");
            return ISP_MNGR__ERR_UNEXPECTED;
        }
        // Print values to the file
        fprintf(file, "{\n\t\"lscConfig\":\n\t{\n\t\t\"LSC_SAMPLES_red\":\n\t\t[");
        for(i = 0; i < (ISP_MNGR_LSC_LENGTH-1); i++ )
        {
            fprintf(file, "%d, ", metaData.lscMetaData.lscTable[i]);
        }
        fprintf(file, "%d],\n\t\t\"LSC_SAMPLES_greenR\":\n\t\t[ ", metaData.lscMetaData.lscTable[ISP_MNGR_LSC_LENGTH-1]);
        for(i = ISP_MNGR_LSC_LENGTH; i < (ISP_MNGR_LSC_LENGTH*2-1); i++ )
        {
            fprintf(file, "%d, ", metaData.lscMetaData.lscTable[i]);
        }
        fprintf(file, "%d],\n\t\t\"LSC_SAMPLES_greenB\":\n\t\t[ ", metaData.lscMetaData.lscTable[ISP_MNGR_LSC_LENGTH*2-1]);
        for(i = ISP_MNGR_LSC_LENGTH*2; i < (ISP_MNGR_LSC_LENGTH*3-1); i++ )
        {
            fprintf(file, "%d, ", metaData.lscMetaData.lscTable[i]);
        }
        fprintf(file, "%d],\n\t\t\"LSC_SAMPLES_blue\":\n\t\t[ ", metaData.lscMetaData.lscTable[ISP_MNGR_LSC_LENGTH*3-1]);
        for(i = ISP_MNGR_LSC_LENGTH*3; i < (ISP_MNGR_LSC_LENGTH*4-1); i++ )
        {
            fprintf(file, "%d, ", metaData.lscMetaData.lscTable[i]);
        }
        fprintf(file, "%d]\n\t}\n}", metaData.lscMetaData.lscTable[ISP_MNGR_LSC_LENGTH*4-1]);
        /*for(uint32_t i = 0; i < 8; i++) //tbd - pavel- values?
        {
                lsc_get.grid.LscXGradTbl[i]
                lsc_get.grid.LscYGradTbl[i]
                lsc_get.grid.LscXSizeTbl[i]
                lsc_get.grid.LscYSizeTbl[i]
        }*/

        // Close the file
        fclose(file);
        LOGG_PRINT(LOG_INFO_E, NULL, "ioctl LSC_CFG_GET DONE. Result = %d, file name=%s\n", result,lscFilePath);
    }
    return retVal;
}

static ERRG_codeE ISP_MNGRG_activateIspFunc(unsigned int ispRdNum, ISP_MNGRG_streamReadCfgT *cfg)
{
    ERRG_codeE retval = ISP_MNGR__RET_SUCCESS;
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(GET_ISP_NUM(ispRdNum));
    CAM_DEVICE* cam_dev = &priv->cam_dev;
    int result;

    if(cfg->f.gc == 1)
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_GC_ENABLE, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_GC_ENABLE_SET DONE. Result = %d\n", result);
    }
    if(cfg->f.lsc == 1)
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_LSC_ENABLE, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_LSC_ENABLE_SET DONE. Result = %d\n", result);
        // add init config
    }
    if(cfg->f.dpf == 1)
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_DPF_ENABLE, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_DPF_ENABLE_SET DONE. Result = %d\n", result);
    }
    if(cfg->f.dpcc == 1)
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_DPCC_ENABLE, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_DPCC_ENABLE_SET DONE. Result = %d\n", result);
    }
    if(cfg->f.demosaic == 1)
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        jRequest[KEY_GENERATION] = 2;
        result = cam_dev->ioctl(ISPCORE_MODULE_DEMOSAIC_ENABLE, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_DEMOSAIC_ENABLE_SET DONE. Result = %d\n", result);
    }
    if(cfg->f.cnr == 1)
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_CNR_ENABLE, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_CNR_ENABLE_SET DONE. Result = %d\n", result);
    }
    if(cfg->f.ee == 1)
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_EE_ENABLE, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_EE_ENABLE_SET DONE. Result = %d\n", result);
    }
    if(cfg->f.ca == 1)
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_CA_ENABLE, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_CA_ENABLE_SET DONE. Result = %d\n", result);
    }
    if(cfg->f.dci == 1)
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_DCI_ENABLE, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_DCI_ENABLE_SET DONE. Result = %d\n", result);
    }
    if(cfg->f.ge == 1)
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_GE_ENABLE, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_GE_ENABLE_SET DONE. Result = %d\n", result);
    }
#if 0
    if(cfg->f.cproc == 1)
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_CPROC_ENABLE, jRequest, jResponse);
        printf(GREEN("activate ISPCORE_MODULE_CPROC_ENABLE_SET DONE. Result = %d\n", result);
    }
#endif
    if(cfg->f.cac == 1)
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_CAC_ENABLE_SET, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_CAC_ENABLE_SET DONE. Result = %d\n", result);
    }
    if(cfg->f.bls == 1)
    {
        Json::Value jRequest, jResponse;
        clb_itf::Bls::Config config;

        config.red = 256;
        config.greenR = 256;
        config.greenB = 256;
        config.blue = 256;
        jRequest[KEY_CONFIG] = t_common::Base64::encode(config);
        result = cam_dev->ioctl(ISPCORE_MODULE_BLS_CFG_SET, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_BLS_CFG_SET DONE. Result = %d\n", result);
    }
    if(cfg->f.ie == 1)
    {
//        Json::Value jRequest, jResponse;
//        jRequest[KEY_ENABLE] = 1;
//        result = cam_dev->ioctl(ISPCORE_MODULE_IE_ENABLE_SET, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate IE NOT IMPLEMENTED\n");
    }
    if(cfg->f.func_2dnr == 1)
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_2DNR_ENABLE, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_2DNR_ENABLE_SET DONE. Result = %d\n", result);
    }
    if(cfg->f.func_3dnr == 1)
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_3DNR_ENABLE, jRequest, jResponse);

        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_3DNR_ENABLE_SET DONE. Result = %d\n", result);

        result = cam_dev->ioctl(ISPCORE_MODULE_3DNR_CFG_GET, jRequest, jResponse);

        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_3DNR_CFG_GET DONE. KEY_GENERATION %d. Result = %d\n", jResponse[KEY_GENERATION].asInt(), result);

    }
    if((cfg->f.ae == ISP_MNGR_AE_FLICKER_MODE_OFF) || (cfg->f.ae == ISP_MNGR_AE_FLICKER_MODE_50HZ) || (cfg->f.ae == ISP_MNGR_AE_FLICKER_MODE_60HZ))
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_AE_ENABLE_SET, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_AE_ENABLE_SET DONE. Result = %d\n", result);

        // Configure ROI
        clb_itf::Ae::AeRoi roi;
        char roiFilePath[60];
        memset((void *)&roi, 0x00,sizeof(roi));
        sprintf(roiFilePath,"/media/inuitive/roi_%s.json",priv->sensName);
        std::ifstream roicfg(roiFilePath, std::ifstream::binary);
        if(roicfg.fail())
        {
            LOGG_PRINT(LOG_WARN_E, NULL, "There is no ROI configuration file for %s \n", priv->sensName);
        }
        else
        {
            Json::Value rootNode;
            roicfg >> rootNode;
            //std::cout<<rootNode<<"\n";
            Json::Value roiConfig = rootNode["roiConfig"];
            //std::cout<<roiConfig<<"\n";
            Json::Value windows = roiConfig["windows"];
            roi.windowNum = windows.size();

            for (unsigned int index = 0; index < roi.windowNum; ++index )
            {
                const Json::Value window = windows[index];

                roi.roiWindow[index].xStart = window[0].asUInt();
                roi.roiWindow[index].yStart = window[1].asUInt();
                roi.roiWindow[index].width  = window[2].asUInt();
                roi.roiWindow[index].height = window[3].asUInt();
                roi.roiWindow[index].weight = window[4].asFloat();
            }
        }
        {
            Json::Value jRequest, jResponse;
            jRequest[KEY_ROI] = t_common::Base64::encode(roi);
            result = cam_dev->ioctl(ISPCORE_MODULE_AE_ROI_SET, jRequest, jResponse);
            LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_AE_ROI_SET DONE. Result = %d\n", result);
        }

        //Flickering settings
        {
            ISP_MNGRP_aeSetEnable(GET_ISP_NUM(ispRdNum), 0);

            clb_itf::Ae::Config config;
            Json::Value jRequest, jResponse;
            result =  cam_dev->ioctl(ISPCORE_MODULE_AE_CFG_GET, jRequest, jResponse);
            t_common::Base64::decode(jResponse[KEY_CONFIG].asString(), config);
            switch(cfg->f.ae)
            {
                case ISP_MNGR_AE_FLICKER_MODE_OFF:
                default:
                    config.v1.ecm.flickerPeriod = clb_itf::Ae::Config::V2::Ecm::FlickerPeriod::FlickerPeriodOff; //(clb_itf::Ae::Config::V2::Ecm::FlickerPeriod)0;
                break;

                case ISP_MNGR_AE_FLICKER_MODE_50HZ:
                    config.v1.ecm.flickerPeriod = clb_itf::Ae::Config::V2::Ecm::FlickerPeriod::FlickerPeriod100Hz; //(clb_itf::Ae::Config::V2::Ecm::FlickerPeriod)1;
                break;

                case ISP_MNGR_AE_FLICKER_MODE_60HZ:
                    config.v1.ecm.flickerPeriod = clb_itf::Ae::Config::V2::Ecm::FlickerPeriod::FlickerPeriod120Hz; //(clb_itf::Ae::Config::V2::Ecm::FlickerPeriod)2;
                break;
            }
            jRequest[KEY_CONFIG] = t_common::Base64::encode(config);

            result =  cam_dev->ioctl(ISPCORE_MODULE_AE_CFG_SET, jRequest, jResponse);
            LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_AE_CFG_SET DONE. Result = %d. Mode %d, Flicker %d\n", result, config.v1.mode, config.v1.ecm.flickerPeriod);
            ISP_MNGRP_aeSetEnable(GET_ISP_NUM(ispRdNum), 1);

        }


#if 0
    {
        Json::Value jRequest, jResponse;
        clb_itf::Ae::AeRoi roiGet;
        result = cam_dev->ioctl(ISPCORE_MODULE_AE_ROI_GET, jRequest, jResponse);
        printf("ISPCORE_MODULE_AE_ROI_GET ae roi windows :::\n");
        t_common::Base64::decode(jResponse[KEY_ROI].asString(), roiGet);
        for(uint32_t i = 0; i < roiGet.windowNum ;i++)
        {
            printf("Window (%d):: width:[%3d] height:[%3d] xstart:[%4d] ystart:[%4d] weight:[%.2f]\n", i,
                roiGet.roiWindow[i].width,roiGet.roiWindow[i].height,roiGet.roiWindow[i].xStart,
                roiGet.roiWindow[i].yStart,roiGet.roiWindow[i].weight);
        }
    }
#endif

    }
    else if(cfg->f.ae == 2)
    {

        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_AE_ENABLE_SET, jRequest, jResponse);
        LOGG_PRINT(LOG_INFO_E, NULL, "activate ISPCORE_MODULE_AE_ENABLE_SET DONE. Result = %d\n", result);
    }

    if(cfg->f.af == 1)
    {
//        Json::Value jRequest, jResponse;
//        jRequest[KEY_ENABLE] = 1;
//        result = cam_dev->ioctl(ISPCORE_MODULE_AF_ENABLE_SET, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate AF NOT IMPLEMENTED\n");
    }
    if(cfg->f.awb == 1)
    {
        Json::Value jRequest, jResponse;
        jRequest[KEY_ENABLE] = 1;
        result = cam_dev->ioctl(ISPCORE_MODULE_AWB_ENABLE_SET, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_AWB_ENABLE_SET DONE. Result = %d\n", result);
    }
    if(cfg->f.avs == 1)
    {
//        Json::Value jRequest, jResponse;
//        jRequest[KEY_ENABLE] = 1;
//        result = cam_dev->ioctl(ISPCORE_MODULE_AVS_ENABLE_SET, jRequest, jResponse);
//        printf(GREEN("ISP_MNGRG_activateIspFunc :: ISPCORE_MODULE_AVS_ENABLE_SET DONE. Result = %d\n"), result);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate AVS NOT IMPLEMENTED\n");

    }
    if(cfg->f.wdr == 1)
    {
        Json::Value jRequest, jResponse;
        result = cam_dev->ioctl(ISPCORE_MODULE_WDR_ENABLE, jRequest, jResponse);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate ISPCORE_MODULE_WDR_ENABLE DONE. Result = %d\n", result);
    }
    if(cfg->f.hdr == 1)
    {
//        Json::Value jRequest, jResponse;
//        jRequest[KEY_ENABLE] = 1;
//        result = cam_dev->ioctl(ISPCORE_MODULE_HDR_ENABLE_SET, jRequest, jResponse);
//        printf(GREEN("ISP_MNGRG_activateIspFunc :: ISPCORE_MODULE_HDR_ENABLE_SET DONE. Result = %d\n"), result);
        LOGG_PRINT(LOG_DEBUG_E, NULL, "activate HDR NOT IMPLEMENTED\n");

    }

    return retval;
}

ERRG_codeE ISP_MNGRG_setEnable(unsigned int ispRdNum, ISP_MNGRG_streamReadCfgT* cfg)
{
    ERRG_codeE retval = ISP_MNGR__RET_SUCCESS;
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(GET_ISP_NUM(ispRdNum));
    CAM_DEVICE* cam_dev = &priv->cam_dev;
    CAM_VIRTUAL_DEVICE_ID virtId = reader2bufio[ispRdNum].virtid;
    //printf("ISP_MNGRG_setEnable ispRdNum %d virtId %d priv->numActivteReadersCtr %d cfg.numActiveReaders %d\n",ispRdNum,virtId,priv->numActivteReadersCtr,cfg.numActiveReaders);
    ISP_MNGRG_bufioT bufio = reader2bufio[ispRdNum];
    priv->frameCnt[bufio.bufio] = 0;

#ifdef TEMP_SOLUTION_BEFORE_STOP_FIXING
    if (priv->numActivteReadersCtr == cfg->numActiveReaders - 1 /*ctrIsp[virtId] == NUM_ISP_DEBUG*/)//it means the last channel on this isp
#endif
    {
#if !defined(__REGISTER_TEST__)
        int result;
        //Start device
        {
            Json::Value jRequest, jResponse;
            jRequest[KEY_CAMCONNECT_PREVIEW] = true;
            jRequest[KEY_WIDTH] = cfg->width;// img_sensor_out_width;
            jRequest[KEY_HEIGHT] = cfg->height;// img_sensor_out_height;
            jRequest[KEY_FORMAT] = fmtConvert[cfg->format];//CAMERA_PIX_FMT_YUV422SP;//fmtConvert[cfg.format];//0;//img_sensor_out_format;
            jRequest[KEY_FRAME] = 0;
            jRequest[KEY_ENABLE] = false;
            jRequest[KEY_BAYER_PATTERN] = ISI_BPAT_BGBGGRGR;

            result = cam_dev->ioctl(ISPCORE_MODULE_DEVICE_CAMERA_CONNECT, jRequest, jResponse);
            LOGG_PRINT(LOG_INFO_E, NULL, "ISPCORE_MODULE_DEVICE_CAMERA_CONNECT DONE. Result = %d nu format %d convert to format %d\n", result, cfg->format, fmtConvert[cfg->format]);
        }

        ISP_MNGRG_activateIspFunc(ispRdNum, cfg);

        {
            Json::Value jRequest, jResponse;
            result = cam_dev->ioctl(ISPCORE_MODULE_DEVICE_STREAMING_START, jRequest, jResponse);
            LOGG_PRINT(LOG_INFO_E, NULL, "ISPCORE_MODULE_DEVICE_STREAMING_START DONE. Result = %d\n", result);
        }

        ISP_MNGRG_getAllEnables(ispRdNum);

#endif //!defined(__REGISTER_TEST__)
        priv->ispStatus = ISP_MNGRG_ISP_ENABLED_E;
    }
#ifdef TEMP_SOLUTION_BEFORE_STOP_FIXING
        priv->numActivteReadersCtr++;
#endif

    //priv->ispStatus = ISP_MNGRG_ISP_CONFIGURED_E;
    return retval;

}

ERRG_codeE ISP_MNGRG_setDisable(unsigned int ispRdNum )
{
    ERRG_codeE retval = ISP_MNGR__RET_SUCCESS;
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(GET_ISP_NUM(ispRdNum));
    CAM_DEVICE* cam_dev = &priv->cam_dev;
    ISP_MNGRG_bufioT bufio = reader2bufio[ispRdNum];
    int result;
#ifdef TEMP_SOLUTION_BEFORE_STOP_FIXING
    LOGG_PRINT(LOG_INFO_E, NULL, "Stop stream ISP %d, camdev %p ActiveRd %d\n", GET_ISP_NUM(ispRdNum) , cam_dev, priv->numActivteReadersCtr);

    if(priv->numActivteReadersCtr == 1)
#endif //TEMP_SOLUTION_BEFORE_STOP_FIXING
    //Stop device
    {
        {
            Json::Value jRequest, jResponse;

            result = cam_dev->ioctl(ISPCORE_MODULE_DEVICE_STREAMING_STOP, jRequest, jResponse);
            LOGG_PRINT(LOG_DEBUG_E, NULL, "ISPCORE_MODULE_DEVICE_STREAMING_STOP DONE. Result = %d \n", result);
        }

        {
            Json::Value jRequest, jResponse;
            result = cam_dev->ioctl(ISPCORE_MODULE_DEVICE_CAMERA_DISCONNECT, jRequest, jResponse);
            LOGG_PRINT(LOG_DEBUG_E, NULL, "ISPCORE_MODULE_DEVICE_CAMERA_DISCONNECT DONE. Result = %d\n", result);
        }
    }

    result = cam_dev->detachBufChain(bufio.bufio);
    result = cam_dev->stopBufChain(bufio.bufio);
    result = cam_dev->deInitBufChain(bufio.bufio);

    result = cam_dev->bufferPoolIplUnMap(bufio.bufio, true);
    result = cam_dev->bufferPoolClearBufList(bufio.bufio);
    result = cam_dev->deInitBufferPoolCtrl(bufio.bufio);

    priv->runningFlag[bufio.bufio] = 0;

    UINT32 numBuff =  MEM_POOLG_getNumBufs(priv->memPoolHandle[bufio.bufio]);

    for(UINT32 i = 0; i < numBuff; i++)
    {
        MEM_POOLG_free(priv->bufList[bufio.bufio][i].bufDescP);
    }

    MEM_POOLG_waitBuffReleaseClosePool(priv->memPoolHandle[bufio.bufio]);
#ifdef TEMP_SOLUTION_BEFORE_STOP_FIXING
    if(priv->numActivteReadersCtr == 1)
    {

        cam_dev->releaseHardware();

        priv->ispStatus = ISP_MNGRG_ISP_IDLE_E;
        priv->numActivteReadersCtr = 0;
    }
    else
    {
        priv->numActivteReadersCtr--;
    }
#else
    // Not implemented. Need to consdier a solution when TEMP_SOLUTION_BEFORE_STOP_FIXING is undefined.
#endif //TEMP_SOLUTION_BEFORE_STOP_FIXING
    return retval;

}

MediaBuffer_t static_buff_info;
int ISP_MNGRG_eventMonitor(void *monitorParams )
{
    ERRG_codeE retval = ISP_MNGR__RET_SUCCESS;
    ISP_MNGRG_threadParamsT *paramsP = (ISP_MNGRG_threadParamsT *)monitorParams;
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(paramsP->ispNum);
    if ( (priv==NULL) ||  (priv->ispStatus < ISP_MNGRG_ISP_CONFIGURED_E) )
    {
        LOGG_PRINT(LOG_DEBUG_E, NULL, "could not set flicker, too early\n");
        return retval;
    }
    CAM_DEVICE* cam_dev = &priv->cam_dev;

    //Runing device

    int result;
    buffCtrlEvent_t buffEvent;
#if !defined(__REGISTER_TEST__)
    MediaBuffer_t * pBuf;
    MEM_POOLG_bufDescT  *bufDescP;
#else   // defined __REGISTER_TEST__
    MediaBuffer_t * pBuf = &static_buff_info;
    UINT32 loop_counter = 2500;
    UINT32 mis_isr = 0,  mis1_isr = 0;
#endif

    ISP_MNGRG_bufioT bufio = reader2bufio[paramsP->ispRdNum];

    while(priv->runningFlag[bufio.bufio])
    {
#if !defined(__REGISTER_TEST__)

        result = cam_dev->waitForBufferEvent(bufio.bufio, &buffEvent, 100);
        if(!result && buffEvent.eventID == BUFF_CTRL_CMD_BUFFER_READY)
        {
            result = cam_dev->DQBUF(bufio.bufio, &pBuf);
            if(priv->frameCnt[bufio.bufio]++ > priv->numFramesToSkip)
            {
                //LOGG_PRINT(LOG_INFO_E, NULL, " ISP_MNGRG_eventMonitor:: Got BUFFER READY event on hw %d virt %d BuffIO %d. ddr add 0x%x cb pointer %p\n", bufio.hwid, bufio.virtid, bufio.bufio,pBuf->baseAddress,paramsP->cb);
                //Process buffer: user callback
                bufDescP = NULL;
                for(UINT32 i = 0; i < sizeof(priv->bufList[bufio.bufio])/sizeof(ISP_MNGRG_bufT); i++)
                {
                //printf("ISP_MNGRG_eventMonitor i=%d phyAddress 0x%x pBuf->baseAddress 0x%x\n",i,priv->bufList[bufio.bufio][i].phyAddress,pBuf->baseAddress);
                    if(priv->bufList[bufio.bufio][i].phyAddress == pBuf->baseAddress)
                    {
                        bufDescP = priv->bufList[bufio.bufio][i].bufDescP;
                        //printf("ISP_MNGRG_eventMonitor duplicate %p refCntr %d\n",bufDescP,bufDescP->refCntr);
                        MEM_POOLG_duplicate(bufDescP);
                       //printf(CYAN("found i=%d %s::Got BUFFER READY BuffIO %d hp %d vid %d. i = %d descr = %p refC %d\n"),i, __func__,  bufio.bufio,bufio.hwid,bufio.virtid, i , bufDescP,bufDescP->refCntr);
                    }
                }
                //paramsP->cb((void*)pBuf->baseAddress, paramsP->me);
                if(bufDescP != NULL)
                    paramsP->cb((void*)bufDescP, paramsP->me);
            }

            result = cam_dev->QBUF(bufio.bufio, pBuf);

        }
        if(!result && buffEvent.eventID == BUFF_CTRL_CMD_STOP)
        {
            LOGG_PRINT(LOG_INFO_E, NULL, "Got STOP event on BuffIO %d hp %d vid %d.\n", bufio.bufio,bufio.hwid,bufio.virtid);
        }
        if(!result && buffEvent.eventID == BUFF_CTRL_CMD_START)
        {
            LOGG_PRINT(LOG_INFO_E, NULL, "Got START event on BuffIO %d hp %d vid %d.\n", bufio.bufio,bufio.hwid,bufio.virtid);
        }
#else   // defined __REGISTER_TEST__
        mis_isr = ISP_R(0x30016E0, paramsP->ispNum);
        if((mis_isr & 0x01) == 0x01)
        {
            printf(MAGENTA("ISP_MNGRG_eventMonitor:: Got frame. loop %d, MI_RIS =  0x%08x\n"), loop_counter, mis_isr);
            pBuf->baseAddress = ISP_R(0x3001324, paramsP->ispNum);            //Y buffer
            pBuf->baseSize = ISP_R(0x3001340, paramsP->ispNum);                //Cb buffer
            printf(MAGENTA("ISP_MNGRG_eventMonitor:: Y 0x%08x, Cb 0x%08x\n"), pBuf->baseAddress, pBuf->baseSize);

            //params->cb((void*)pBuf, NULL);
            ISP_W(0x30016d8, mis_isr, paramsP->ispNum);
        }
#if 0    // Monitor mi_mis1 interrupt
        mis1_isr = ISP_R(0x30016E4);
        if(mis1_isr != 0 )
        {
            printf(RED("ISP_MNGRG_eventMonitor:: loop %d, MI_RIS1 =  0x%08x\n"), loop_counter, mis1_isr);

            ISP_W(0x30016dc, mis1_isr );
        }
#endif
        if(loop_counter-- == 0)
        {
            ISP_MNGRG_dumpRegs();
        }
#endif

        OS_LYRG_usleep(100);
    }

    return retval;

}


ERRG_codeE ISP_MNGRG_dumpRegs()
{
    UINT32 regsOffset[] = {isp_0_offset_tbl_values};//assuming that isp0 & isp1 with the same offset and reset values.
    UINT32 regsResetVal[] = {isp_0_regs_reset_val};
    UINT32 reg;
    UINT8  buf[2048];
    ISP_MNGRG_privT *priv;


    for (int ispNum =0; ispNum < 2; ispNum++)
    {
        priv = ISP_MNGRP_getIspPriv(ispNum);
        LOGG_PRINT(LOG_INFO_E, NULL, "Dumping ISP %d registers:\n",ispNum);
        for (reg = 0; reg < (sizeof(regsOffset[ispNum])/4); reg++)
        {
            LOGG_PRINT(LOG_INFO_E, NULL, "0x%04x = 0x%08x\n",regsOffset[reg],*(volatile UINT32 *)(priv->ispBaseAddress + regsOffset[reg]));
        }
    }

#if 0
    UINT32 regsOffset[] = {isp_0_offset_tbl_values};
    UINT32 regsResetVal[] = {isp_0_regs_reset_val};
    UINT32 reg;
    UINT8  buf[2048];

    //memset(buf,0,sizeof(buf));
    //sprintf(buf + strlen((const char*)buf), "Modified IAE registers:\n");
    LOGG_PRINT(LOG_INFO_E, NULL, "Dumping ISP registers:\n");
    //compare against reset val
    for (reg = 0; reg < (sizeof(regsOffset)/4); reg++)
    {
//      if (*(volatile UINT32 *)(ispBaseAddress + regsOffset[reg]) != regsResetVal[reg])
        {
            LOGG_PRINT(LOG_INFO_E, NULL, "0x%04x = 0x%08x\n",regsOffset[reg],*(volatile UINT32 *)(ispBaseAddress + regsOffset[reg]));
//          sprintf(buf + strlen((const char*)buf), "0x%04x = 0x%08x\n",regsOffset[reg],*(volatile UINT32 *)(ispBaseAddress + regsOffset[reg]));
        }
    }
    //printf("%s",buf);
#endif
    return ISP_MNGR__RET_SUCCESS;
}

UINT32 ISP_MNGRG_getIspRegBase(UINT32 ispid)
{
    ISP_MNGRG_privT *priv = ISP_MNGRP_getIspPriv(ispid);
    return priv->ispBaseAddress;
}




