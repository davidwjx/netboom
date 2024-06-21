/****************************************************************************
 *
 *   FileName: cde.h
 *
 *   Author: Benny V.
 *
 *   Date: 
 *
 *   Description: driver to CDE (Central DMA Engine) HW
 *   
 ****************************************************************************/
#ifndef CDE_H
#define CDE_H


#ifdef __cplusplus
      extern "C" {
#endif
#include "IDMA_200_tasks.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define CDEG_MAX_DMA_DESCRIPTORS_PER_CHANNEL    (INU_DEFSG_MAX_NUM_CYC_BUFFERS)
#define CDEG_NUM_OF_DMA_CHANNELS                (IDMA_200_NUM_OF_CORES_E) * (IDMA_200_NUM_OF_CORE_CHANNELS_E)

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef  enum
{
   CDEG_INTERRUPT_FALSE_E         = 0,
   CDEG_INTERRUPT_TRUE_E          = 1,
   CDEG_INTERRUPT_OPTION_LAST_E
} CDEG_interruptOptionsE;

typedef  enum
{
   CDEG_LAST_DESCRIPTOR_FALSE_E     = 0,
   CDEG_LAST_DESCRIPTOR_TRUE_E      = 1,
   CDEG_LAST_DESCRIPTOR_FLAG_LAST_E
} CDEG_lastDescriptorFlagE;

typedef  enum
{
   CDEG_LINK_OPT_SELF_E     = 0,
   CDEG_LINK_OPT_NEXT_E     = 1,
   CDEG_LINK_OPT_LAST_E
} CDEG_linkOptionE;

typedef  enum
{
   CDEG_NONE_BLOCK_TRANSFER_MODE_E = 0,
   CDEG_BLOCK_TRANSFER_MODE_E = 1,
   CDEG_TRANSFER_MODE_LAST_E
} CDEG_transferModeE;

typedef struct
{
   UINT32   srcAddr;
   UINT32   dstAddr;
   UINT32   xSize;
   UINT32   ySize;
   CDEG_interruptOptionsE    interruptFlag;
   CDEG_lastDescriptorFlagE  lastDescriptorFlag;
   CDEG_linkOptionE           linkOpt;
} CDEG_transferParamsT;

typedef struct
{
   UINT32                     descriptorIndex;
   UINT32                     srcAddr;
   UINT32                     dstAddr;
   UINT32                     size;
   UINT32                     linkAddr;
   CDEG_interruptOptionsE     interruptFlag;
   CDEG_lastDescriptorFlagE   lastDescriptorFlag;
} CDEG_regsCfgParamsT;

typedef struct
{
   UINT32               numOfBuffers;
   UINT32               numOfDescPerBuf;
   CDEG_transferParamsT transferParams[CDEG_MAX_DMA_DESCRIPTORS_PER_CHANNEL];
} CDEG_transferRequestCfgT;

typedef struct
{
   IDMA_200_rxPeriphClientIdE rxPeriphId;
   IDMA_200_txPeriphClientIdE txPeriphId;
   CDEG_transferModeE         transferMode;
   UINT32                     transferStride;
   UINT32                     firstDescriptorAddr;
   UINT32                     drainAddr;
} CDEG_generalCfgT;

typedef void (*CDEG_channelCbT) (void *arg, UINT32 doneCnt, UINT32 intStatus, UINT64 timeStamp);

typedef struct
{
   CDEG_channelCbT  chanCb;
   void *arg;
} CDEG_chInterruptCbCfgT ;

typedef struct
{
   CDEG_generalCfgT           generalCfg;
   CDEG_transferRequestCfgT   transferCfg;
   CDEG_chInterruptCbCfgT     intCbCfg;
} CDEG_dmaRequestCfgT;

// CDE descriptor status
typedef enum
{
   CDEP_DESCRIPTOR_IS_FREE_E     = 0,
   CDEP_DESCRIPTOR_IS_OCCUPIED_E = 1,
   CDEP_DESCRIPTOR_STATUS_LAST_E
} CDE_MDRVP_descriptorStatusE;

typedef struct
{
   CDE_MDRVP_descriptorStatusE   descriptorStatus;
   UINT32                        nextDescriptorId;
   void                          *nextDescriptorPtr;
   UINT32                        savedWrAddr;
   IDMA_200_cmd                  idmaCmdParams;
} CDEP_dmaDescriptorT;
/****************************************************************************
 ***************      E X T E R N A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
void        *CDEG_channelOpen(UINT32 coreSelect);
void        *CDEG_channelOpen7(void);
ERRG_codeE  CDEG_channelCfg(void *channelH, CDEG_dmaRequestCfgT *dmaRequestCfgP);
ERRG_codeE  CDEG_activateSyncChannels(void *channelH[]);
ERRG_codeE  CDEG_channelActivate(void *channelH);
ERRG_codeE  CDEG_channelDeactivate(void *channelH);
void        CDEG_channelReady(void   *channelH);
void        CDEG_channelStart(void   *channelH);
ERRG_codeE  CDEG_channelClose(void *channelH);
UINT32      CDEG_disableChInt(void *channelH);
UINT32      CDEG_getChIntEnable(void *channelH);
void        CDEG_enableChInt(void *channelH, UINT32 intMask);
ERRG_codeE  CDEG_channelTransferPolling(void *channelH, UINT32 *numOfNewFrames);
ERRG_codeE  CDEG_printChannelDebugInfo(void *channelH);
ERRG_codeE  CDEG_open(UINT32 dmaRegistersBaseAddr, UINT32 dmaDescriptorsVirtualAddr, UINT32 dmaDescriptorsPhysicalAddr);
ERRG_codeE  CDEG_close(void);
ERRG_codeE  CDEG_init(void);
void        CDEG_isr(UINT64 timeStamp, UINT64 counter);
int         CDEG_getCoreNum(void *channelH);
int         CDEG_getChNum(void *channelH);
void        CDEG_setDescDrain(void *handle, unsigned int bufIdx);
void        CDEG_setDescNormal(void *handle, unsigned int bufIdx);
int         CDEG_isChDraining(void *handle);
int         CDEG_getNumOfDrainFrames(void *handle);
void        CDEG_getChannelDescriptor(void *channelH, CDEG_regsCfgParamsT *regsCfgParamsP);
void        CDEG_setChannelDescriptor(void *channelH, CDEG_regsCfgParamsT *regsCfgParamsP);
CDEP_dmaDescriptorT *CDEG_getDescriptorAddr(void *channelH);
void CDEG_updateDescSize(void *handle, unsigned int bufIdx, UINT32 size);

#ifdef __cplusplus
}
#endif

#endif //   CDE_H


