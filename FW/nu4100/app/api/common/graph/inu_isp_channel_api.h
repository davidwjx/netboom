#ifndef _INU_ISP_CHANNEL_API_H_
#define _INU_ISP_CHANNEL_API_H_

#include "inu2.h"

#define MAX_LSC_FILE_NAME (32)

typedef void* inu_isp_channelH;

typedef enum
{
    INU_ISP_COMMAND_NONE_E = 0,    //filler
    INU_ISP_COMMAND_FIRST_E,
    INU_ISP_COMMAND_SET_EXPOSURE_E = INU_ISP_COMMAND_FIRST_E,  // 1
    INU_ISP_COMMAND_SET_INT_TIME_E,     // 2
    INU_ISP_COMMAND_SET_GAIN_E,         // 3
    INU_ISP_COMMAND_SET_ISP_GAIN_E,     // 4
    INU_ISP_COMMAND_SET_WB_E,           // 5
    INU_ISP_COMMAND_SET_CC_OFFSET_E,    // 6
    INU_ISP_COMMAND_SET_LSC_E,          // 7
    INU_ISP_COMMAND_GET_EXPOSURE_E,     // 8
    INU_ISP_COMMAND_GET_INT_TIME_E,     // 9
    INU_ISP_COMMAND_GET_GAIN_E,         // 10
    INU_ISP_COMMAND_GET_ISP_GAIN_E,     // 11
    INU_ISP_COMMAND_GET_WB_E,           // 12
    INU_ISP_COMMAND_GET_CC_OFFSET_E,    // 13
    INU_ISP_COMMAND_GET_LSC_E,          // 14
    INU_ISP_COMMAND_LAST_E
}inu_isp_channel__commandE;


typedef struct
{
    float integrationTime;
    float analogGain;
    float ispGain;
    bool metadataEnableAE;
    bool hasmetadataEnableAE;
    int (*ae_cb)(void *) ; /*Callback when AE has been finished*/
    bool has_ae_cb; /* True = Use callback*/
}inu_isp_channel__exposureParamT;

typedef struct
{
    float integrationTime;
}inu_isp_channel__integrationTimeParamT;

typedef struct
{
    float gain;
}inu_isp_channel__gainParamT;

typedef struct
{
    float ispGain;
}inu_isp_channel__ispGainParamT;

typedef struct
{// same as 3astruct: float *awbGain;//rGain grGain gbGain bGain
    float gainR;
    float gainGr;
    float gainGb;
    float gainB;
}inu_isp_channel__wbGainT;

typedef struct
{
    int red;
    int green;
    int blue;
}inu_isp_channel__ccOffsetT;
#define ISP_CHANNEL_CCMATRIX_SIZE 9u
typedef struct
{
    float coeff[ISP_CHANNEL_CCMATRIX_SIZE];
}inu_isp_channel__ccMatrixT;


typedef struct
{
    inu_isp_channel__wbGainT   wbGain;
    inu_isp_channel__ccOffsetT ccOffset;
    inu_isp_channel__ccMatrixT ccMatrix;
    bool metadataEnableAWB;
    bool hasmetadataEnableAWB;
    int (*awb_cb)(void *) ;  /*Callback when AWB has been finished*/
    bool has_awb_cb; /* True = Use callback*/
}inu_isp_channel__ispWbParamT;


typedef struct
{
    unsigned int set_by_file;
    char         file_name[MAX_LSC_FILE_NAME];
    void        *lsc_buff; 
}inu_isp_channel__ispLscParamT;


typedef union
{
    inu_isp_channel__exposureParamT         exposureParam;
    inu_isp_channel__integrationTimeParamT  intTimeParam;
    inu_isp_channel__gainParamT             gainParam;
    inu_isp_channel__ispGainParamT          ispGainParam;
    inu_isp_channel__ispWbParamT            wbParam;
    inu_isp_channel__ispLscParamT           lscParam;
}inu_isp_channel__ispCmdParamU;






//UINT32 inu_isp_channel__getId(inu_isp_channelH meH);
ERRG_codeE inu_isp_channel__sendIspCommand(inu_isp_channelH meH, inu_isp_channel__commandE cmd, inu_isp_channel__ispCmdParamU *param);

#endif //_INU_ISP_CHANNEL_API_H_
