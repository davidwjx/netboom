/****************************************************************************
 *
 *   FileName: CDE_DRV.h
 *
 *   Author: Benny V.
 *
 *   Date: 
 *
 *   Description: DMA user manager
 *   
 ****************************************************************************/
#ifndef CDE_DRV_H
#define CDE_DRV_H

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "err_defs.h"
#include "IDMA_200_tasks.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define CDE_DRVG_INT_FALSE    IDMA_200_INT_FALSE
#define CDE_DRVG_INT_TRUE     IDMA_200_INT_TRUE

#define CDE_DRVG_CORE_ANY_E   (IDMA_200_NUM_OF_CORES_E)

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/


//////////////////////////////////////////////////////////////////////////////
////////////////////////////// frame dimensions //////////////////////////////
//////////////////////////////////////////////////////////////////////////////
typedef struct
{
   UINT32   pixelSize;
   UINT32   imageWidth;
   UINT32   imageHeight;
} CDE_DRVG_imageDimensionsT;

typedef struct
{
   CDE_DRVG_imageDimensionsT  imageDim;
   UINT32                     numOfFrameImages;
   UINT32                     frameStartX;
   UINT32                     frameStartY;
   UINT32                     bufferWidth;
   UINT32                     bufferHeight;
} CDE_DRVG_frameDimensionsT;

//////////////////////////////////////////////////////////////////////////////
////////////////////////////// transfer options //////////////////////////////
//////////////////////////////////////////////////////////////////////////////
typedef  enum
{
   CDE_DRVG_NO_INTERRUPT_E            = 0,
   CDE_DRVG_INTERRUPT_EACH_TRANSFER_E = 1,
   CDE_DRVG_INTERRUPT_LAST_TRANSFER_E = 2,
   CDE_DRVG_INTERRUPT_OPTION_LAST_E
} CDE_DRVG_interruptOptionsE;

typedef  enum
{
   CDE_DRVG_NO_LINKING_E          = 0,
   CDE_DRVG_REGULAR_LINKING_E     = 1,
   CDE_DRVG_CYCLIC_LINKING_E      = 2,
   CDE_DRVG_SELF_LINKING_E       = 3,
   CDE_DRVG_DESCRIPTOR_LINK_OPTION_LAST_E
} CDE_DRVG_transferLinkOptionsE;

/*typedef  enum
{
   CDE_DRVG_1D_TRANSFER_MODE_E = 0,  // none-block mode (without stride)
   CDE_DRVG_2D_TRANSFER_MODE_E = 1,  // block mode (stride is not equal to width)
   CDE_DRVG_TRANSFER_MODE_LAST_E
} CDE_DRVG_transferModeE;*/

typedef  enum
{
   CDE_DRVG_MEM_TYPE_PERIPHERAL_E = 0,
   CDE_DRVG_MEM_TYPE_RAM_E        = 1,
   CDE_DRVG_MEM_TYPE_LAST_E
} CDE_DRVG_memTypeE;

typedef struct
{
   UINT32                        numOfBuffers;
   UINT32                        numOfDescPerBuf;
   CDE_DRVG_interruptOptionsE    interruptOpt;
   CDE_DRVG_transferLinkOptionsE linkOpt;
   //CDE_DRVG_transferModeE        transferMode;
   IDMA_200_rxPeriphClientIdE    rxPeriphId;
   IDMA_200_txPeriphClientIdE    txPeriphId;
   UINT32                        metaBytes;
   BOOL                          useMetaBytesForUsb;
   BOOL                          enableDraining;
} CDE_DRVG_transferOptionsT;

//////////////////////////////////////////////////////////////////////////////
////////////////////////////// buffers params   //////////////////////////////
//////////////////////////////////////////////////////////////////////////////

// CDE_DRV channel transfer parameters structure (src/dst buffers & transfer params)
typedef struct
{
   UINT32   bufIdx;
   UINT32   bufBaseAddr;
   UINT32   bufBaseVirtualPtr;
   UINT32   singleBufSize;
   UINT32   multiBuffersize;
   UINT32   currentFrameIdx;
} CDE_DRVG_bufParamsT;

//////////////////////////////////////////////////////////////////////////////
////////////////////////////// DMA transfer params ///////////////////////////
//////////////////////////////////////////////////////////////////////////////
typedef struct
{
   CDE_DRVG_frameDimensionsT   frameDim;
   CDE_DRVG_transferOptionsT   transferOpt;
   CDE_DRVG_bufParamsT         srcBufParams;
   CDE_DRVG_bufParamsT         dstBufParams;
} CDE_DRVG_dmaChTransferParamsT;

//////////////////////////////////////////////////////////////////////////////
/////////////////////// ioctl cmd functions params ///////////////////////////
//////////////////////////////////////////////////////////////////////////////
// open cmd params
typedef struct {
   INU_DEFSG_sysChannelIdE dmaChanId;
   UINT32                 isLut;
   UINT32                 coreSelect;
} CDE_DRVG_chOpenIoctlParamsT;

// close cmd params
typedef struct {
   INU_DEFSG_sysChannelIdE   dmaChanId;
} CDE_DRVG_chCloseIoctlParamsT;

// set params cmd params

typedef struct {
   INU_DEFSG_sysChannelIdE          dmaChanId;
   CDE_DRVG_dmaChTransferParamsT   newDmaChTransferParams;
} CDE_DRVG_chSetParamsIoctlParamsT;

// set default params cmd params
typedef struct {
   INU_DEFSG_sysChannelIdE   dmaChanId;
} CDE_DRVG_chSetDefaultParamsIoctlParamsT;

// get descriptor params cmd params
typedef struct {
   INU_DEFSG_sysChannelIdE    dmaChanId;
   UINT32                     descriptorIndex;
   UINT32                     srcAddr;
   UINT32                     dstAddr;
   UINT32                     size;
   UINT32                     linkAddr;
   UINT32                     interruptFlag;
   UINT32                     lastDescriptorFlag;
} CDE_DRVG_chGetDescriptorIoctlParamsT;

// set descriptor params cmd params
typedef struct {
   INU_DEFSG_sysChannelIdE    dmaChanId;
   UINT32                     descriptorIndex;
   UINT32                     srcAddr;
   UINT32                     dstAddr;
   UINT32                     size;
   UINT32                     linkAddr;
   UINT32                     interruptFlag;
   UINT32                     lastDescriptorFlag;
} CDE_DRVG_chSetDescriptorIoctlParamsT;

// set general purpose channel params cmd params
typedef struct
{
   UINT32                       srcAddr;
   UINT32                       dstAddr;
   CDE_DRVG_interruptOptionsE   interruptOpt;
   //CDE_DRVG_transferModeE       transferMode;
   UINT32                       transferWidth;
   UINT32                       transferHeight;

   IDMA_200_rxPeriphClientIdE   rxPeriphId;
   IDMA_200_txPeriphClientIdE   txPeriphId;
} CDE_DRVG_gpChSetParamsT;

typedef struct {
   INU_DEFSG_sysChannelIdE  dmaChanId;
   CDE_DRVG_gpChSetParamsT newDmaChTransferParams;
} CDE_DRVG_gpChSetParamsIoctlParamsT;

typedef struct
{
   INU_DEFSG_sysChannelIdE  dmaChanId;
   UINT32                   srcAddr;
   UINT32                   dstAddr;
   UINT32                   transferWidth;
   UINT32                   transferHeight;
} CDE_DRVG_gpChUpdateIoctlParamsT;

// get params cmd params
typedef struct {
   INU_DEFSG_sysChannelIdE          dmaChanId;
   CDE_DRVG_dmaChTransferParamsT    returnDmaChTransferParams;
} CDE_DRVG_chGetParamsIoctlParamsT;

// channel activate cmd params
typedef struct {
   INU_DEFSG_sysChannelIdE   dmaChanId;
} CDE_DRVG_chActivateIoctlParamsT;

// channel deactivate cmd params
typedef struct {
   INU_DEFSG_sysChannelIdE   dmaChanId;
} CDE_DRVG_chDeactivateIoctlParamsT;

typedef struct {
   INU_DEFSG_sysChannelIdE dmaChanId;
   UINT32                  returnBufIdx;
   UINT32                  returnDoneCnt;
   UINT32                  numOfDrainFrames;
} CDE_DRVG_chGetBufParamsT;

typedef struct {
   INU_DEFSG_sysChannelIdE dmaChanId;
   unsigned int coreNum;
   unsigned int channelNum;
   int isActive;
} CDE_DRVG_chGetStateT;

typedef struct {
   INU_DEFSG_sysChannelIdE dmaChanId;
   UINT32                  index;
} CDE_DRVG_setDescNormalT;

typedef enum
{
   CDE_DRVG_CH_DONE_INT_E       = 0,
   CDE_DRVG_CH_RD_SLVERR_INT_E,
   CDE_DRVG_CH_WR_SLVERR_INT_E,
   CDE_DRVG_CH_RD_DECERR_INT_E,
   CDE_DRVG_CH_WR_DECERR_INT_E,
   CDE_DRVG_CH_OVERFLOW_INT_E,
   CDE_DRVG_CH_UNDERFLOW_INT_E,
   CDE_DRVG_CH_TIMEOUT_R_INT_E,
   CDE_DRVG_CH_TIMEOUT_AR_INT_E,
   CDE_DRVG_CH_TIMEOUT_B_INT_E,
   CDE_DRVG_CH_TIMEOUT_W_INT_E,
   CDE_DRVG_CH_TIMEOUT_AW_INT_E,
   CDE_DRVG_CH_TIMEOUT_WDT_INT_E
} CDE_DRVG_chInterruptE;

typedef void (*CDE_DRVG_interruptCbT) (UINT32 intStatus, UINT32 cdeCh, UINT64 timeStamp, UINT32 bufIdx);

// channel interrupt callback registration
typedef struct
{
   CDE_DRVG_interruptCbT  cb;
} CDE_DRVG_setInterruptCbT;

typedef struct
{
   INU_DEFSG_sysChannelIdE dmaChanId;
   UINT32 bitmask;
} CDE_DRVG_chIntEnableT;

// channel transfer polling cmd params
typedef struct
{
   INU_DEFSG_sysChannelIdE dmaChanId;
   BOOL                   pollingResult;
   UINT32                 returnBufIdx;
   UINT32                 returnMissedFrames;
} CDE_DRVG_chTransPollingIoctlParamsT;

// channel print descriptors cmd params
typedef struct
{
   INU_DEFSG_sysChannelIdE   dmaChanId;
} CDE_DRVG_printChDebufInfoIoctlParamsT;

typedef union
{
   CDE_DRVG_chOpenIoctlParamsT               chOpenCmdParams;
   CDE_DRVG_chCloseIoctlParamsT              chCloseCmdParams;
   CDE_DRVG_chSetParamsIoctlParamsT          chSetParamsCmdParams;
   CDE_DRVG_chSetDefaultParamsIoctlParamsT   chSetDefaultParamsCmdParams;
   CDE_DRVG_chGetDescriptorIoctlParamsT      chGetDescriptorParams;
   CDE_DRVG_chSetDescriptorIoctlParamsT      chSetDescriptorParams;
   CDE_DRVG_gpChSetParamsIoctlParamsT        gpChSetParamsCmdParams;
   CDE_DRVG_gpChUpdateIoctlParamsT           gpChUpdateCmd;
   CDE_DRVG_chGetParamsIoctlParamsT          chGetParamsCmdParams;
   CDE_DRVG_chGetStateT                      chGetState;
   CDE_DRVG_chActivateIoctlParamsT           chActivateCmdParams;
   CDE_DRVG_chDeactivateIoctlParamsT         chDeactivateCmdParams;
   CDE_DRVG_chTransPollingIoctlParamsT       chTransPollingCmdParams;
   CDE_DRVG_chGetBufParamsT                  chGetBufParams;
   CDE_DRVG_setDescNormalT                   chSetDescNormalParams;
   CDE_DRVG_setInterruptCbT                  setInterruptCb;
   CDE_DRVG_chIntEnableT                     chIntEnable;
   CDE_DRVG_printChDebufInfoIoctlParamsT     printChDebugInfoCmdParams;
} CDE_DRVG_ioctlCmdParamsU;

// module open function params
typedef struct {
   UINT32   dmaRegistersVirtualAddr;
   UINT32   dmaDescriptorsVirtualAddr;
   UINT32   dmaDescriptorsPhysicalAddr;
   UINT32   metaDataSize;
   UINT32   metaDataVirtAddr;
} CDE_DRVG_openParamsT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
//ERRG_codeE CDE_DRVG_init(IO_PALG_apiCommandT *palP);

//void *CDE_DRVG_getDescriptorAddr(INU_DEFSG_sysChannelIdE dmaChanId);
//void CDE_DRVG_channelReady(INU_DEFSG_sysChannelIdE        dmaChanId);
//void CDE_DRVG_channelStart(INU_DEFSG_sysChannelIdE        dmaChanId);

void CDE_DRVG_updateDescSize(INU_DEFSG_sysChannelIdE dmaChanId, unsigned int bufIdx, UINT32 size);

void CDE_DRVG_setDesc(CDE_DRVG_chSetDescriptorIoctlParamsT *chSetDescriptorCmdParamsP);
void CDE_DRVG_getDesc(CDE_DRVG_chGetDescriptorIoctlParamsT *chGetDescriptorCmdParamsP);

int CDE_DRVG_isChDraining(INU_DEFSG_sysChannelIdE chId);
//ERRG_codeE CDE_DRVG_allocDrainBuffer(UINT32 reqSize);
//ERRG_codeE CDE_DRVG_deallocDrainBuffer();

#ifdef __cplusplus
}
#endif

#endif //   CDE_DRV_H


