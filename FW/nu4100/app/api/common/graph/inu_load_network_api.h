#ifndef __INU_LOAD_NETWORK_API_H__
#define __INU_LOAD_NETWORK_API_H__


typedef enum
{
   INU_LOAD_NETWORK__CDNN_0_E = 0,
   INU_LOAD_NETWORK__CDNN_1_E,
   INU_LOAD_NETWORK__CDNN_2_E,
   INU_LOAD_NETWORK__CDNN_3_E,
   INU_LOAD_NETWORK__CDNN_4_E,
   INU_LOAD_NETWORK__CDNN_5_E,
   INU_LOAD_NETWORK__CDNN_6_E,
   INU_LOAD_NETWORK__CDNN_7_E,
   INU_LOAD_NETWORK__CDNN_8_E,
   INU_LOAD_NETWORK__CDNN_9_E,
   INU_LOAD_NETWORK__CDNN_10_E,
   INU_LOAD_NETWORK__CDNN_11_E,
   INU_LOAD_NETWORK__CDNN_12_E,
   INU_LOAD_NETWORK__CDNN_13_E,
   INU_LOAD_NETWORK__CDNN_LAST_E,
   INU_LOAD_NETWORK__IAE_TOTAL_CDNN_E = INU_LOAD_NETWORK__CDNN_LAST_E,

   INU_LOAD_NETWORK__CDNN_INVALID_E = 0xFFFF
} inu_load_network_cdnnIdE;

typedef enum
{
   INU_LOAD_NETWORK__CEVA_E = 0,
   INU_LOAD_NETWORK__EV61_E
} inu_load_network_cdnnEngineTypeE;

//Structure Definition for download CDNN network
typedef struct
{
   UINT32                              networkId;
   inu_load_network_cdnnEngineTypeE    engineType;
   CDNNG_inputImageRawScaleE           rawScale;
   CDNNG_channelSwapE                  channelSwap;
   CDNNG_resultsFormatE                resultsFormat;
   CDNNG_netType                       netType;
   CDNNG_bitAccuracyE                  bitAccuracy;
   UINT32                              ddrWorkspaceSize;
   UINT32                              internalworkspaceSize;
   CDNNG_version_E                     version;
   UINT32                              pipeDepth;
   UINT32                              ProcesssingFlags;
} inu_load_network_hdr_t;

typedef struct
{
   char            *bufP;
   UINT32          bufsize;
   char            *path;
} inu_load_network_memory_t;

typedef struct
{
   inu_deviceH deviceH;
   inu_graphH  graphH;
   UINT32      initWorkSpace;
} inu_load_network__initParams;


//API functions
ERRG_codeE inu_load_network__new(inu_load_networkH *meH, inu_load_network__initParams *initParamsP);
void inu_load_network__delete(inu_load_networkH meH);
ERRG_codeE inu_load_network__loadNetwork(inu_load_networkH meH, inu_load_network_memory_t *memP, inu_load_network_hdr_t *networkHdrP);
ERRG_codeE inu_load_network__releaseAll(inu_load_networkH meH);

#endif

