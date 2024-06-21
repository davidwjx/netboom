/****************************************************************************
 *
 *   FileName: dma_mngr.h
 *
 *   Author:  Benny V.
 *
 *   Date:
 *
 *   Description: dma control
 *
 ****************************************************************************/

#ifndef DMA_MNGR_H
#define DMA_MNGR_H

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

typedef struct
{
   UINT32               dmaChanId;
   UINT32               systemFrameCntr;
   UINT64               timeStamp;
   MEM_POOLG_bufDescT   *currBufDescP;
   UINT32               currPhysAddr;
   UINT8                *currVirtPtr;
   UINT16               numBuffers;
} CDE_MNGRG_chanCbParamsT;

typedef void (*CDE_MNGRG_initChCbT) (void *arg);
//typedef void (*CDE_MNGRG_chCbT) (CDE_MNGRG_chanCbParamsT *chanCbParams, void *arg);

// CDE interupts
typedef enum
{
   CDE_MNGRG_INT_GROUP_FRAME_READY_E   = 0,
   CDE_MNGRG_INT_GROUP_GENERAL_ERR_E   = 1,
   CDE_MNGRG_NUM_OF_INT_GROUPS_E
}  CDE_MNGRG_interruptGroupsE;

typedef struct
{
   UINT32   chMask[INU_DEFSG_NUM_OF_SYS_CHANNELS_E];
   BOOL     chStatus[INU_DEFSG_NUM_OF_SYS_CHANNELS_E];
} CDE_MNGRG_intParams;

typedef enum
{
   CDE_MNGRG_RESOLUTION_TYPE_BINNING_E  = 0,
   CDE_MNGRG_RESOLUTION_TYPE_FULL_E   = 1,
   CDE_MNGRG_NUM_OF_RESOLUTION_TYPES_E
}  CDE_MNGRG_resolutionTypeE;

typedef struct {
   INU_DEFSG_cyclicBuffersE   numBuffers[CDE_MNGRG_NUM_OF_RESOLUTION_TYPES_E];
   INU_DEFSG_streamMethodE    streamMethod;
   UINT32                     streamId;
   void                       *streamerH;
} CDE_MNGRG_chanStreamConfigT;

typedef struct {
   UINT8                      *streamVirtPtr[INU_DEFSG_MAX_NUM_CYC_BUFFERS];
   UINT32                     streamPhysPtr[INU_DEFSG_MAX_NUM_CYC_BUFFERS];
   MEM_POOLG_handleT          bufDescPoolH;
   UINT16                     currBuffIdx;
   UINT16                     prevBuffIdx;
   UINT16                     numFrames;
   UINT32                     frameSize;
} CDE_MNGRG_streamBufParmsT;

typedef struct
{
   BOOL                       dmaActivity;
   INU_DEFSG_frameModeIdE     dmaFrameMode;
   CDE_MNGRG_resolutionTypeE  resolutionType;
   UINT32                     dmaWidth;
   UINT32                     dmaHeight;
   UINT32                     dmaStartX;
   UINT32                     dmaStartY;
   UINT32                     bufferWidth;
   UINT32                     bufferHeight;
   UINT32                     chSyncCntr;
   UINT32                     dmaFrameCntr;
   UINT32                     dmaPrevFrameCnt;
   UINT32                     dmaMissCnt;
   UINT32                     dmaErrors;
   UINT32                     dmaStartTimeSec;
   UINT16                     dmaStartTimeMs;
   UINT32                     dmaPrevFrameTimeSec;
   UINT16                     dmaPrevFrameTimeMs;
   UINT16                     dmaMaxFrameTimeMs;
   UINT16                     dmaMinFrameTimeMs;
   CDE_MNGRG_streamBufParmsT  streamBuffersParms;
} CDE_MNGRG_dmaChanInfoT;

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

CDE_MNGRG_dmaChanInfoT  *CDE_MNGRG_getChanInfo(INU_DEFSG_sysChannelIdE dmaChanId);
CDE_MNGRG_intParams     *CDE_MNGRG_getCdeIntParams();

ERRG_codeE  CDE_MNGRG_init();
void        CDE_MNGRG_setChannelStreamConfig(INU_DEFSG_sysChannelIdE    dmaChanId,
                                                            CDE_MNGRG_chanStreamConfigT   *streamCfgP);
void        CDE_MNGRG_getChannelStreamConfig(INU_DEFSG_sysChannelIdE       dmaChanId,
                                                            CDE_MNGRG_chanStreamConfigT   *streamCfgP);

ERRG_codeE  CDE_MNGRG_startDmaStats(INU_DEFSG_sysChannelIdE dmaIdx);
ERRG_codeE  CDE_MNGRG_getCdeChanBufDesc(INU_DEFSG_sysChannelIdE dmaChanId, UINT32 *systemFrameCntrP, MEM_POOLG_bufDescT **bufDescP, UINT32 *physAddrP, UINT8 **currVirtPtr, UINT16 *numBuffersP);
ERRG_codeE  CDE_MNGRG_closeOutChBufTbl(INU_DEFSG_sysChannelIdE dmaChanId);
ERRG_codeE  CDE_MNGRG_openCdeMngrDrv();
ERRG_codeE  CDE_MNGRG_startCde();
ERRG_codeE  CDE_MNGRG_deactivateCde();
ERRG_codeE  CDE_MNGRG_activateCdeChannel(INU_DEFSG_sysChannelIdE dmaChanId);
ERRG_codeE CDE_MNGRG_chanPolling(INU_DEFSG_sysChannelIdE dmaChanId, int maxRetry);
ERRG_codeE  CDE_MNGRG_updateDescriptor(INU_DEFSG_sysChannelIdE dmaChanId,UINT32 descriptorIndex, UINT32 bufAddr, UINT32 dstAddr, UINT32 bufSize, UINT32 interruptFlag, UINT32 lastDescriptorFlag, UINT32 linkAddr);
ERRG_codeE  CDE_MNGRG_closeCdeChannel(INU_DEFSG_sysChannelIdE dmaChanId);
ERRG_codeE  CDE_MNGRG_deactivateCdeChannel(INU_DEFSG_sysChannelIdE dmaChanId);
ERRG_codeE  CDE_MNGRG_closeCdeDrv();
ERRG_codeE  CDE_MNGRG_configCde();
ERRG_codeE  CDE_MNGRG_openChan(INU_DEFSG_sysChannelIdE dmaChanId);
ERRG_codeE  CDE_MNGRG_getChanDim(INU_DEFSG_sysChannelIdE dmaChanId, UINT32 *widthP, UINT32 *heightP, UINT32 *bufferWidthP, UINT32 *bufferHeightP, UINT32 *xStartP, UINT32 *yStartP, UINT32 *numOfFrameImagesP, UINT32 *pixelSizeP);
ERRG_codeE  CDE_MNGRG_getChanBufDim(INU_DEFSG_sysChannelIdE dmaChanId, UINT32 *numFramesInBufP);
ERRG_codeE  CDE_MNGRG_handleCdeFrameReady(UINT32 dmaChannel, UINT32 status, UINT64 timeStamp, MEM_POOLG_bufDescT *bufDescP);
ERRG_codeE  CDE_MNGRG_handleCdeChannelError(UINT32 dmaChannel);
ERRG_codeE  CDE_MNGRG_registerChanInitFunc(CDE_MNGRG_initChCbT initCb, void *arg, UINT32 dmaChannel, INT32 *cbNum);
ERRG_codeE  CDE_MNGRG_unRegisterChanInitFunc(UINT32 dmaChannel, INT32 cbNum);
//ERRG_codeE  CDE_MNGRG_registerChannelCb(CDE_MNGRG_chCbT cb, void *arg, UINT32 dmaCh, INT32 *cbNum);
//ERRG_codeE  CDE_MNGRG_registerPriorityChannelCb(CDE_MNGRG_chCbT cb, void *arg, UINT32 dmaCh, INT32 *cbNum, UINT32 priority);
ERRG_codeE  CDE_MNGRG_unRegisterChannelCb(UINT32 dmaCh, INT32 chNum);
UINT8 *     CDE_MNGRG_getChanBufPtr(INU_DEFSG_sysChannelIdE dmaChanId, UINT32 index);
void        CDE_MNGRG_initInjectionMemPoolHandle();
//void        CDE_MNGRG_getOverwriteTimestamp(INU_DEFSG_sysChannelIdE dmaChanId, UINT64 *timeStamp);
//void        CDE_MNGRG_overwriteTimestamp(INU_DEFSG_sysChannelIdE dmaChanId, UINT64 timeStamp);
//void        CDE_MNGRG_setFiqCounter(INU_DEFSG_sysChannelIdE dmaChanId, UINT64 count);
//void        CDE_MNGRG_getFiqCounter(INU_DEFSG_sysChannelIdE dmaChanId, UINT32 *count);



void CDE_MNGRG_channelReady(INU_DEFSG_sysChannelIdE dmaChanId);
void CDE_MNGRG_channelStart(INU_DEFSG_sysChannelIdE dmaChanId);
void *CDE_MNGRG_getDescriptorAddr(INU_DEFSG_sysChannelIdE dmaChanId);
void CDE_MNGRG_invokeCallbacks(UINT32 dmaChannel, CDE_MNGRG_chanCbParamsT *chanCbParamsP);

ERRG_codeE CDE_MNGRG_registerNode(void *cb, void *arg, UINT32 dmaCh, const void *name, INT32 flags, INT32 *cid);
ERRG_codeE CDE_MNGRG_unRegisterNode(UINT32 dmaCh, INT32 cid, int flags);
ERRG_codeE CDE_MNGRG_notifyDone(UINT32 dval);
int CDE_MNGRG_isCcfEnabled(UINT32 dmaCh);

#ifdef __cplusplus
}
#endif

#endif //   DMA_MNGR_H

