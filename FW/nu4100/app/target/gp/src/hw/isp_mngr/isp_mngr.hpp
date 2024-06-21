
#ifndef ISP_MNGR_H
#define ISP_MNGR_H

//#include "inu_common.h"
#include "inu_graph.h"


#ifdef __cplusplus
extern "C" {
#endif

#define TEMP_SOLUTION_BEFORE_STOP_FIXING

#define GET_ISP_NUM(ispRdNum) (ispRdNum < 3) ? 0U : 1U

typedef void (*ISP_MNGRG_CbT)( void *pBuff, void *arg);

typedef struct
{
   UINT32 gc;
   UINT32 lsc;
   UINT32 cac;
   UINT32 dpf;
   UINT32 dpcc;
   UINT32 cnr;
   UINT32 ee;
   UINT32 dci;
   UINT32 ge;
   UINT32 cproc;
   UINT32 ca;
   UINT32 bls;
   UINT32 demosaic;
   UINT32 ie;
   UINT32 func_2dnr;
   UINT32 func_3dnr;
   UINT32 ae;
   UINT32 af;
   UINT32 awb;
   UINT32 avs;
   UINT32 wdr;
   UINT32 hdr;
}ISP_MNGRG_functionsCfgT;

typedef struct
{
	UINT32 height;
	UINT32 width;
	UINT32 bpp;
	UINT32 xStart;
	UINT32 yStart;
	UINT32 stride;
	UINT32 bufferHeght;
	UINT32 format;
	UINT32 numImages; //number of interleaved images
	UINT32 numOfBuffers;
	UINT32 numLinesPerChunk; //number of lines per each DMA frame
	ISP_MNGRG_functionsCfgT f;
	UINT32 numFramesToSkip;
#ifdef TEMP_SOLUTION_BEFORE_STOP_FIXING
	UINT32 numActiveReaders;
#endif
}ISP_MNGRG_streamReadCfgT;


/****************************************************************************
*
*  Function Name: ISP_MNGRG_setConfig
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE ISP_MNGRG_setConfig(unsigned int ispRdNum, char *senStr, ISP_MNGRG_streamReadCfgT cfg);

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setEnableSLU
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE ISP_MNGRG_setEnable(unsigned int ispRdNum, ISP_MNGRG_streamReadCfgT* cfg);

/****************************************************************************
*
*  Function Name: SEQ_MNGRP_setDisableSLU
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE ISP_MNGRG_setDisable(unsigned int ispRdNum);
ERRG_codeE ISP_MNGRG_setEnableIpe(unsigned int ispRdNum);
ERRG_codeE ISP_MNGRG_setDisableIpe(unsigned int ispRdNum);

ERRG_codeE ISP_MNGRG_setImgType(unsigned int img_type);

ERRG_codeE ISP_MNGRG_init(UINT32 isp0_base, UINT32 isp1_base, UINT32 ipe_base, UINT32 ddr_base);

int ISP_MNGRG_eventMonitor(void *monitorParams );

ERRG_codeE ISP_MNGRG_ThreadInit(void *me, unsigned int ispRdNum, void *cb);
void ISP_MNGRG_ThreadClose();
void ISP_MNGRG_DdrConfig();

ERRG_codeE ISP_MNGRG_setEnableAllAe(UINT8 enable); //Enable/disable  AE for both sensors
ERRG_codeE ISP_MNGRG_getEnableAllAe(UINT8* enable); // Enable = 0 - no ae/Enable = 1 - 5M ae/Enable = 2 - 8M ae/Enable = 3 - both ae/
ERRG_codeE ISP_MNGRG_setEnableAeByRdNum(unsigned int ispRdNum, UINT8 enable); //Enable/disable  AE for a certain sensor (According to ispNum)
ERRG_codeE ISP_MNGRG_setFlickerAe(UINT8 flickerPeriod); // Works only if AE on. Values: FLICKER_OFF = 0/ FLICKER_100HZ = 1/ FLICKER_120HZ = 2
ERRG_codeE ISP_MNGRG_setExposureParams(UINT32 ispRdNum, inu_isp_channel__exposureParamT *exposureParams);
ERRG_codeE ISP_MNGRG_getExposureParams(UINT32 ispRdNum, inu_isp_channel__exposureParamT *exposureParams);
ERRG_codeE ISP_MNGRG_setWbParams(UINT32 ispRdNum, inu_isp_channel__ispWbParamT *wbParams);
ERRG_codeE ISP_MNGRG_getWbParams(UINT32 ispRdNum, inu_isp_channel__ispWbParamT *wbParams);
ERRG_codeE ISP_MNGRG_setLscParams(UINT32 ispRdNum, inu_isp_channel__ispLscParamT *lscParam);
ERRG_codeE ISP_MNGRG_getLscParams(UINT32 ispRdNum, inu_isp_channel__ispLscParamT *lscParam);
#ifdef __cplusplus
}
#endif

#endif	//ISP_MNGR_H


