/****************************************************************************
 *
 *   FileName: client_api.h
 *
 *   Author:  Benny V.
 *
 *   Date: 
 *
 *   Description: API for client process
 *
 ****************************************************************************/
#ifndef CLIENT_API_H
#define CLIENT_API_H

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
 ****************************************************************************/
#include "inu_types.h"
#include "err_defs.h"
#include "inu_defs.h"
#include "inu_cmd.h"
#include "inu2_types.h"
#include "inu2.h"
#if DEFSG_IS_CEVA
//	#include "InV_defines.h"
#include "InV_types.h"
#include "cevaxm.h"

#endif
#if DEFSG_IS_EV72
#include "evthreads.h"
//	#include "linked_list.h"
#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define  CLIENT_APIG_INVALID_BUFFER_IDX (0xFFFFFFFF)
#define  SCHED_NOT_FINISHED_JOB (99)

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/**********************************************************************/
//                                        CLIENT EVENTS COMMANDS                                              //
/**********************************************************************/

typedef enum
{
   CLIENT_APIG_EVT_OPCODE_CHAN_START_E       = 0,
   CLIENT_APIG_EVT_OPCODE_CHAN_STOP_E        = 1,
   CLIENT_APIG_NUM_OF_CHAN_ACTION_OPCODES_E
} CLIENT_APIG_chanActionE;

typedef struct
{
   void                    *bufDesc;   
   UINT32                  dVal;
}  CLIENT_APIP_eventHandleT;


typedef struct
{
   INU_DEFSG_sysChannelIdE  channelId;
   CLIENT_APIG_chanActionE  chanAction;
} CLIENT_APIG_chanActionParamsT;


// CLIENT_APIG_EVT_OPCODE_CHAN_DONE_E
typedef struct
{
   CLIENT_APIP_eventHandleT handle;
   INU_DEFSG_sysChannelIdE  channelId;
   UINT32                  frameCntr;
   UINT64                  timeStamp;
   UINT32                  physicalAddr;
   UINT32                  virtualAddr;
   UINT32                  frameSize;
   UINT32                  pixelSize;
   UINT32                  numOfImagesInFrame;  // 1 image for none-interleave frame. 2 images for interleave frame
   UINT32                  bufferWidth;
   UINT32                  bufferHeight;
   UINT32                  frameWidth;
   UINT32                  frameHeight;
   UINT32                  frameStartX;
   UINT32                  frameStartY;
} CLIENT_APIG_chanDoneParamsT;

// CLIENT_APIG_EVT_OPCODE_CHAN_DONE_E
typedef struct
{
   INU_DEFSG_sysChannelIdE  handle;
   UINT64                  timeStamp;
   UINT8*                  dataP;
   UINT32                  dataLen;
} CLIENT_APIG_i2sParamsT;


// CLIENT_APIG_EVT_OPCODE_IAE_INT_E
typedef struct
{
   UINT32   iaeStatus0;
   UINT32   iaeStatus1;
   UINT32   iaeStatus2;
   UINT64   timeStamp;
} CLIENT_APIG_iaeIntEvtParamsT;

// CLIENT_APIG_EVT_OPCODE_DPE_INT_E
typedef struct
{
   UINT32   dpeStatus0;
   UINT64   timeStamp;
} CLIENT_APIG_dpeIntEvtParamsT;

// CLIENT_APIG_EVT_OPCODE_CDE_INT_E
typedef struct
{
   UINT32   cdeStatus0;
   UINT64   timeStamp;
} CLIENT_APIG_cdeIntEvtParamsT;

/**********************************************************************/
/*                            CLIENT CONTROL MESSAGES COMMANDS                                        */
/**********************************************************************/

typedef struct
{
   INU_DEFSG_cevaIdE    cevaId;
   UINT8                msg[INU_FDK__GENERAL_MSG_MAX_SIZE];
   UINT32               msgSize;
} CLIENT_APIG_cevaMsgParamsT;


typedef struct
{
   void                 *msgP;
   UINT32               msgSize;
} CLIENT_APIG_hostMsgParamsT;


/**********************************************************************/
/*                            CLIENT CONFIGURATION COMMANDS                                             */
/**********************************************************************/
typedef enum
{
   CLIENT_APIG_OPCODE_GET_CHAN_BUF_CFG_E  = 0,
   CLIENT_APIG_OPCODE_SET_CHAN_BUF_CFG_E  = 1,
   CLIENT_APIG_CFG_OPCODE_IAE_INT_E       = 2,
   CLIENT_APIG_CFG_OPCODE_DPE_INT_E       = 3,
   CLIENT_APIG_CFG_OPCODE_CDE_INT_E       = 4,
   CLIENT_APIG_NUM_OF_CONFIG_OPCODES_E
} CLIENT_APIG_configOpcodeE;

typedef enum
{
   CLIENT_APIG_BUFFER_AUTO_RELEASE_E    = 0,
   CLIENT_APIG_BUFFER_MANUAL_RELEASE_E  = 1,
   CLIENT_APIG_NUM_OF_MODES_E
} CLIENT_APIG_bufferPolicyE;

typedef enum
{
	CLIENT_APIG_DMA_DIR_EXTERNAL_INTERNAL,					/**< External to Internal transfer */
	CLIENT_APIG_DMA_DIR_INTERNAL_EXTERNAL,					/**< Internal to External transfer */
	CLIENT_APIG_DMA_DIR_INTERNAL_INTERNAL					/**< Internal to Internal transfer */
} CLIENT_APIG_dmaTransferDirE;



// CLIENT_APIG_OPCODE_GET_CHAN_BUF_CFG_E
// CLIENT_APIG_OPCODE_SET_CHAN_BUF_CFG_E
typedef struct
{
   INU_DEFSG_sysChannelIdE    channelId;
   INU_DEFSG_cyclicBuffersE   numOfBuffers;  // Number of channel buffers for VGA resolution
   CLIENT_APIG_bufferPolicyE  bufferReleasePolicy;
   BOOL                       getChanEvt;
   BOOL                       invalidateCache;
} CLIENT_APIG_chanStreamCfgParamsT;

// CLIENT_APIG_CFG_OPCODE_IAE_INT_E
typedef struct
{
   UINT32   iaeFiqMask0;
   UINT32   iaeFiqMask1;
   UINT32   iaeFiqMask2;
   UINT32   iaeSiqMask0;
   UINT32   iaeSiqMask1;
   UINT32   iaeSiqMask2;
} CLIENT_APIG_iaeIntCfgParamsT;

// CLIENT_APIG_CFG_OPCODE_DPE_INT_E
typedef struct
{
   UINT32   dpeFiqMask0;
   UINT32   dpeSiqMask0;
} CLIENT_APIG_dpeIntCfgParamsT;

// CLIENT_APIG_CFG_OPCODE_CDE_INT_E
typedef struct
{
   UINT32   cdeFiqMask0;
   UINT32   cdeSiqMask0;
} CLIENT_APIG_cdeIntCfgParamsT;

/**********************************************************************/
/*                            CLIENT XM4 malloc options	   			 */
/**********************************************************************/
#if DEFSG_IS_CEVA
typedef enum
{
	CLIENT_APIG_XM4_MEM_ALLOCG_OK = 0,
	CLIENT_APIG_XM4_MEM_ALLOCG_NO_SUCH_MEMORY = 1,
	CLIENT_APIG_XM4_MEM_ALLOCG_BAD_ARGUMENT = 2,
	CLIENT_APIG_XM4_MEM_ALLOCG_NO_FREE_MEMORY = 3,
	CLIENT_APIG_XM4_MEM_ALLOCG_EXCEED_MAX_ALLOCATIONS = 4,
	CLIENT_APIG_XM4_MEM_ALLOCG_BAD_HANDLE = 5,
	CLIENT_APIG_XM4_MEM_ALLOCG_BAD_INIT = 6
}CLIENT_APIG_XM4_MEM_ALLOCG_retvalE;

typedef enum
{
	CLIENT_APIG_XM4_MEM_ALLOCG_ALIGN_4 = 4,
	CLIENT_APIG_XM4_MEM_ALLOCG_ALIGN_16 = 0x10,
	CLIENT_APIG_XM4_MEM_ALLOCG_ALIGN_64 = 0x40,
	CLIENT_APIG_XM4_MEM_ALLOCG_ALIGN_128 = 0x80,
	CLIENT_APIG_XM4_MEM_ALLOCG_ALIGN_256 = 0x100,
	CLIENT_APIG_XM4_MEM_ALLOCG_ALIGN_4K = 0x1000,
	CLIENT_APIG_XM4_MEM_ALLOCG_ALIGN_64K = 0x10000,
	CLIENT_APIG_XM4_MEM_ALLOCG_ALIGN_128K = 0x20000
}CLIENT_APIG_XM4_MEM_ALLOCG_startAddrAlignmentE;


typedef enum
{	
	CLIENT_APIG_XM4_MEM_ALLOCG_PERMANENT_ALLOCATION = 1,		// no free for this allocation
	CLIENT_APIG_XM4_MEM_ALLOCG_DYNAMIC_ALLOCATION = 2, 		// dynamic allocation, no specific block, can be any block 0, 1, 2, 3
	CLIENT_APIG_XM4_MEM_ALLOCG_DYNAMIC_ALLOCATION_BLOCK_0 = 10, // dynamic allocation from block #0
	CLIENT_APIG_XM4_MEM_ALLOCG_DYNAMIC_ALLOCATION_BLOCK_1 = 11, // dynamic allocation from block #1
	CLIENT_APIG_XM4_MEM_ALLOCG_DYNAMIC_ALLOCATION_BLOCK_2 = 12, // dynamic allocation from block #2
	CLIENT_APIG_XM4_MEM_ALLOCG_DYNAMIC_ALLOCATION_BLOCK_3 = 13	// dynamic allocation from block #3
}CLIENT_APIG_XM4_MEM_ALLOCG_allocationTypeE;

typedef struct
{
	UINT32 address;	
}CLIENT_APIG_XM4_MEM_ALLOCG_handleT;
#endif	//	DEFSG_IS_CEVA

#ifdef OLD_API
typedef struct
{
   INU_DEFSG_hwVersionIdU  hwVerId;
   INU_DEFSG_fwVersionIdU  fwVerId;
} CLIENT_APIG_getVersionT;
#endif

typedef struct
{
   INU_DEFSG_sysChannelIdE channelId;
   void                    *bufDescP;
   UINT32                  frameCntr;
   UINT64                  timeStamp;
   UINT32                  inPhysicalAddr;
   UINT32                  inVirtualAddr;
   UINT32                  outPhysicalAddr;
   UINT32                  outVirtualAddr;
   UINT32                  dVal;   
} CLIENT_APIG_frameToHostParamsT;


#define DDMA_DESC_SIZE_BYTES 16

typedef struct 
{
    UINT8 bytes[DDMA_DESC_SIZE_BYTES];
} CLIENT_APIG_dmaDescT;

/* 
   X0,Y0 - North west, X1,Y1 - South east
   Each one. max value - 10 bits, min value - 1
*/
typedef struct
{
   UINT32   x0;
   UINT32   x1;
   UINT32   y0;
   UINT32   y1;
} CLIENT_APIG_histRoiT;

typedef struct
{
   CLIENT_APIG_histRoiT    roi[3];
} CLIENT_APIG_histRoiCfgT;



//******************************************************************//
typedef void (*CLIENT_APIG_initCbT)       ();
typedef void (*CLIENT_APIG_ctrlMsgCbT)    (CLIENT_APIG_cevaMsgParamsT *msgCbParamsP, inu_function__operateParamsT *paramsP, UINT16 jobDescriptor);

/* HW events callback functions prototypes */
typedef void (*CLIENT_APIG_chanActionEventCbT)  (CLIENT_APIG_chanActionParamsT *chanActionParamsP);
typedef void (*CLIENT_APIG_chanDoneEventCbT)    (CLIENT_APIG_chanDoneParamsT *chanDoneParamsP);
typedef void (*CLIENT_APIG_i2sEventCbT)         (CLIENT_APIG_i2sParamsT *i2sParamsP);
typedef void (*CLIENT_APIG_iaeEventCbT)         (CLIENT_APIG_iaeIntEvtParamsT *iaeIntParamsP);
typedef void (*CLIENT_APIG_dpeEventCbT)         (CLIENT_APIG_dpeIntEvtParamsT *dpeIntParamsP);
typedef void (*CLIENT_APIG_cdeEventCbT)         (CLIENT_APIG_cdeIntEvtParamsT *cdeIntParamsP);

/* HOST control callback functions prototypes */
typedef void (*CLIENT_APIG_hostMsgCbT)          (CLIENT_APIG_hostMsgParamsT *hostMsgParamsP);

/* HOST control callback functions prototypes */
typedef void (*CLIENT_APIG_cevaEventCbT)        (CLIENT_APIG_cevaMsgParamsT *cevaMsgParamsP);

typedef struct
{
   CLIENT_APIG_i2sEventCbT        i2sCb;
   CLIENT_APIG_iaeEventCbT        iaeCb;
   CLIENT_APIG_dpeEventCbT        dpeCb;
   CLIENT_APIG_cdeEventCbT        cdeCb;
} CLIENT_APIG_hwEventCbsT;

typedef struct
{
   CLIENT_APIG_hostMsgCbT         hostMsgCb;
} CLIENT_APIG_hostCbsT;

typedef struct
{
   CLIENT_APIG_cevaEventCbT       cevaEventCb;
} CLIENT_APIG_cevaCbsT;

typedef struct
{
   CLIENT_APIG_hwEventCbsT hwEventsCbsFunc;
   CLIENT_APIG_hostCbsT    hostMsgCbsFunc;
   CLIENT_APIG_cevaCbsT    cevaMsgCbsFunc;
} CLIENT_APIG_gpCallBackFunctionsT;

#if DEFSG_IS_EV72

//Que
#define CLIENT_APIG_QUE_MAX_ELEMENTS (16) //shuold be 16

typedef void * CLIENT_APIG_queElemT;

typedef struct
{
   unsigned int maxCnt; //must be less than max elements

   //internal - don't touch
   CLIENT_APIG_queElemT que[CLIENT_APIG_QUE_MAX_ELEMENTS];
   unsigned int head;
   unsigned int tail;
   unsigned int cnt;
   
} CLIENT_APIG_queT;typedef struct 

{
    void *val;
    struct Node *next;
}CLIENT_APIG_node;


typedef struct
{
   UINT16               maxMsgs;
   UINT32               msgSize;
   CLIENT_APIG_queT     queue;
   EvMutexType          queueMutex;
   EvCondType           queueCondition;
   CLIENT_APIG_node     *head, *curr;
   UINT32               counter;
} CLIENT_APIG_msgQueT;
#endif

/****************************************************************************
 ***************     C L I E N T   A P I   F U N C T I O N S                            ***************
 ****************************************************************************/
ERRG_codeE CLIENT_APIG_deinit();
#ifdef OLD_API
ERRG_codeE CLIENT_APIG_getVersion(CLIENT_APIG_getVersionT *versionP);
#endif
ERRG_codeE CLIENT_APIG_getFormat(INU_DEFSG_sysChannelIdE channel, INU_DEFSG_formatConvertE *formatP);

#if DEFSG_IS_GP
   ERRG_codeE CLIENT_APIG_gpInit(CLIENT_APIG_gpCallBackFunctionsT gpCbFuncsRegister, int wdTimeout, UINT32 clientDdrBufSize, UINT32 *clientDdrBufPhyAddrP, UINT32 *clientDdrBufVirtAddrP,BOOL standAloneMode,inu_deviceH *devicePtr);
   ERRG_codeE CLIENT_APIG_gpGetErrString(ERRG_codeE errCode, char *stringP);
   ERRG_codeE CLIENT_APIG_gpToHostMsgSend(void *msgP, UINT32 msgSize);
   ERRG_codeE CLIENT_APIG_gpConfig(CLIENT_APIG_configOpcodeE configOpcode, void *configCbParamsP);
   ERRG_codeE CLIENT_APIG_gpToHostFrameSend(CLIENT_APIG_frameToHostParamsT *userStreamParamsP);
   ERRG_codeE CLIENT_APIG_gpBufferRelease(CLIENT_APIP_eventHandleT *handle);
   ERRG_codeE CLIENT_APIG_gpReadRegister(UINT32 regAddr, UINT32 *regValP);
   ERRG_codeE CLIENT_APIG_gpWriteRegister(UINT32 regAddr, UINT32 regVal);
   ERRG_codeE CLIENT_APIG_gpStartDma(UINT32 srcAddr, UINT32 dstAddr, UINT32 transferSize);
   ERRG_codeE CLIENT_APIG_gpPollingDma();
   ERRG_codeE CLIENT_APIG_gpCacheWr(void *ptr, UINT32 size);
   ERRG_codeE CLIENT_APIG_gpCacheInv(void *ptr, UINT32 size);
   void CLIENT_APIG_gpSendLog(INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char *fileP, const char *funcP, UINT32 lineNum, const char * fmt, ...);
   ERRG_codeE CLIENT_APIG_convertPhysicalToVirtual(UINT32 physicalAddr, UINT32 *oVirtualAddrP);
   #define CLIENT_APIG_GP_LOG(level, errCode, ...) CLIENT_APIG_gpSendLog(level, (ERRG_codeE)errCode, __FILE__, __func__, __LINE__, __VA_ARGS__)
   /* Exposure time is in [us] */
   ERRG_codeE CLIENT_APIG_gpGetExposure(UINT32 *expTimeUsP, INU_DEFSG_senSelectE sensorSelect, INU_DEFSG_sensorContextE context);
   ERRG_codeE CLIENT_APIG_gpSetExposure(UINT32 *expTimeUsP, INU_DEFSG_senSelectE sensorSelect, INU_DEFSG_sensorContextE context, UINT8 stereo);
   /* Gain value depends on the sensor type. */
	ERRG_codeE CLIENT_APIG_gpGetGain(UINT32 *analogGainP, UINT32 *digitalGainP, INU_DEFSG_senSelectE sensorSelect, INU_DEFSG_sensorContextE context);
	ERRG_codeE CLIENT_APIG_gpSetGain(UINT32 *analogGainP, UINT32 *digitalGainP, INU_DEFSG_senSelectE sensorSelect, INU_DEFSG_sensorContextE context, UINT8 stereo);
   ERRG_codeE CLIENT_APIG_cfgHistRoi(UINT32 histSelect, CLIENT_APIG_histRoiCfgT *cfgP);
#ifdef OLD_API
	UINT32 CLIENT_APIG_bufferOpen(UINT32 bufSize);
	ERRG_codeE CLIENT_APIG_bufferClose(UINT32 bufIdx);	
	void CLIENT_APIG_getBufferAddr(UINT32 bufIdx, UINT32 *physicalBaseAddrP, UINT32 *virtualBaseAddrP);
	UINT32 CLIENT_APIG_getBufferSize(UINT32 bufIdx);
   ERRG_codeE CLIENT_APIG_gpToCevaMsgSend(CLIENT_APIG_ctrlMsgOpcodeE msgOpcode, CLIENT_APIG_cevaMsgParamsT *msgParamsP);
#endif
#else // ceva

   void CLIENT_APIG_dspFromGpMsgRcv(UINT32 dataBaseSection, INU_DEFSG_cevaIdE cevaId, inu_function__operateParamsT *paramsP, UINT16 jobDescriptor);
   #if DEFSG_IS_CEVA
   void CLIENT_APIG_cevaSendLog(INU_DEFSG_logLevelE logLevel, const char * fmt, ...) PRAGMA_CSECT("client_api_LRAM_csect");


   ERRG_codeE CLIENT_APIG_cevaInit(CLIENT_APIG_ctrlMsgCbT  gpMsgCbFunc,
                                   CLIENT_APIG_initCbT     initCbFunc) PRAGMA_CSECT("client_api_LRAM_csect");
   
   //ERRG_codeE CLIENT_APIG_cevaFromGpMsgRcv(UINT32 dataBaseSection, INU_DEFSG_cevaIdE cevaId, inu_function__operateParamsT *paramsP) PRAGMA_CSECT("client_api_DDR_csect");
   
//	   ERRG_codeE CLIENT_APIG_formatConvertFrame(UINT8 *inImage, UINT8 *outImage, CLIENT_APIG_formatConvertHandleT handle);
//	   ERRG_codeE CLIENT_APIG_formatConvertInit(UINT8 *startVmemP, UINT32 vmemSize, UINT32 width, UINT32 height, UINT32 srcStride, UINT32 dstStride, INU_DEFSG_formatConvertE formatType, CLIENT_APIG_formatConvertHandleT *handleP);
//	   ERRG_codeE CLIENT_APIG_formatConvertClose(CLIENT_APIG_formatConvertHandleT handle);
//	   ERRG_codeE CLIENT_APIG_getReverseFormat(INU_DEFSG_formatConvertE format, INU_DEFSG_formatConvertE *reverseFormatP);
   void CLIENT_APIG_mssGetClock(UINT64 * tick);
   ERRG_codeE CLIENT_APIG_dma_create_1d_desc(CLIENT_APIG_dmaDescT* desc, UINT32 length ,CLIENT_APIG_dmaTransferDirE direction );
   ERRG_codeE CLIENT_APIG_dma_create_2d_desc(CLIENT_APIG_dmaDescT* desc, UINT32 twoD_transfer_width, UINT32 twoD_transfer_height,  UINT32 twoD_src_stride , UINT32 twoD_dst_stride, CLIENT_APIG_dmaTransferDirE direction );
   ERRG_codeE CLIENT_APIG_dma_enqueue_desc(CLIENT_APIG_dmaDescT* desc ,void* src, void* dst );
   ERRG_codeE CLIENT_APIG_dma_enqueue_sync_point(UINT32* value);
   void CLIENT_APIG_dma_wait_sync_point(UINT32 value);	
	CLIENT_APIG_XM4_MEM_ALLOCG_retvalE	CLIENT_APIG_XM4_MEM_ALLOCG_allocBuffer(UINT32 requestedSize, CLIENT_APIG_XM4_MEM_ALLOCG_allocationTypeE type, CLIENT_APIG_XM4_MEM_ALLOCG_startAddrAlignmentE alignment, CLIENT_APIG_XM4_MEM_ALLOCG_handleT *handleP);
	CLIENT_APIG_XM4_MEM_ALLOCG_retvalE	CLIENT_APIG_XM4_MEM_ALLOCG_freeBuffer(CLIENT_APIG_XM4_MEM_ALLOCG_handleT *handleP);
	UINT32 CLIENT_APIG_XM4_MEM_ALLOCG_getMaxFreeBuffer(CLIENT_APIG_XM4_MEM_ALLOCG_startAddrAlignmentE alignment, CLIENT_APIG_XM4_MEM_ALLOCG_allocationTypeE type);

#else	
   #if DEFSG_IS_EV72
	void CLIENT_APIG_evSendLog(INU_DEFSG_logLevelE level, const char * fmt, ...);
   ERRG_codeE CLIENT_APIG_evInit(CLIENT_APIG_ctrlMsgCbT gpMsgCbFunc,CLIENT_APIG_initCbT initCbFunc);
   void CLIENT_APIG_schedPushFinishedwJob(UINT32 jobDescriptor);
   int CLIENT_APIG_createMsgQue(CLIENT_APIG_msgQueT *msgQueP, UINT32 numOfEvents);
   int CLIENT_APIG_deleteMsgQue(CLIENT_APIG_msgQueT *msgQueP);
   int CLIENT_APIG_sendMsg(CLIENT_APIG_msgQueT *msgQueP, UINT8 *msgP, UINT32 len);
   int CLIENT_APIG_recvMsg(CLIENT_APIG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP, UINT32 timeoutMsec);
	int CLIENT_APIG_cnnHandleMessage(void *message);
	void CLIENT_APIG_cnnHandleReleaseNetMessage(void *message);
	UINT32 CLIENT_APIG_netInfoGetChannelSwap(UINT32 netId);
	UINT32 CLIENT_APIG_netInfoGetProcessingFlags(UINT32 netId);
	void CLIENT_APIG_evCnnInit(UINT32 coreId);
   #endif
   #endif

#endif

#ifdef __cplusplus
}
#endif

#endif // CLIENT_API_H



