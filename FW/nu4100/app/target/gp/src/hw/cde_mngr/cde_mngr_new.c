
/****************************************************************************
 *
 *   FileName: dma_mngr.c
 *
 *   Author:  Dima S.
 *
 *   Date:
 *
 *   Description: Management of DMA Driver
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/


#ifdef __cplusplus
   extern "C" {
#endif

#include "inu_common.h"
#include "nu4k_defs.h"
#include "mem_pool.h"
#include "cde_mngr_new.h"
#include "cde_drv_new.h"
#include "cde_scenarios.h" //for CDE_SCENARIOSG_setNewDest
#include "hcg_mngr.h"
#include "ppe_mngr.h"
#include "assert.h"
#include "cmem.h"
#ifdef CDE_DRVG_METADATA
#include "gme_mngr.h"
#include "metadata_target_config.h"
#include "helsinki.h"
#endif

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#define CDE_MNGRP_OUT_CHAN_SLEEP_USEC        (1000*20)
#define CDE_MNGRP_OUT_CHAN_TIMEOUT_USEC      (CDE_MNGRP_OUT_CHAN_SLEEP_USEC *100)
#define CDE_MNGRP_MAX_MEMCPY_SLEEP           (5000000000)

#define CDE_MNGRP_EXTRA_FRAME_BUFFER         3
/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/


/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static CDE_MNGRG_dmaInfoT     *dmaInfoDB;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/
CDE_DRVG_PeriphClientIdE ppeAxiBuffers[NU4K_NUM_AXI_READERS] =
      {CDE_DRVG_PERIPH_CLIENT_AXI_RD0_TX_E, CDE_DRVG_PERIPH_CLIENT_AXI_RD1_TX_E, CDE_DRVG_PERIPH_CLIENT_AXI_RD2_TX_E, CDE_DRVG_PERIPH_CLIENT_AXI_RD3_TX_E,
       CDE_DRVG_PERIPH_CLIENT_AXI_RD4_TX_E, CDE_DRVG_PERIPH_CLIENT_AXI_RD5_TX_E, CDE_DRVG_PERIPH_INVALID_E, CDE_DRVG_PERIPH_INVALID_E,
       CDE_DRVG_PERIPH_CLIENT_AXI_RD8_TX_E, CDE_DRVG_PERIPH_CLIENT_AXI_RD9_TX_E, CDE_DRVG_PERIPH_INVALID_E, CDE_DRVG_PERIPH_INVALID_E};
CDE_DRVG_PeriphClientIdE ppeAxiWriterBuffers[NU4K_NUM_WRITERS] =
     {CDE_DRVG_PERIPH_CLIENT_AXI_WB0_RX_E, CDE_DRVG_PERIPH_CLIENT_AXI_WB1_RX_E, CDE_DRVG_PERIPH_CLIENT_AXI_WB2_RX_E,
      CDE_DRVG_PERIPH_CLIENT_CVJ0_HD_RX_E, CDE_DRVG_PERIPH_CLIENT_CVJ1_HD_RX_E, CDE_DRVG_PERIPH_CLIENT_CVJ2_KP_RX_E};



/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/


static void                CDE_MNGRP_reset(void);
static ERRG_codeE             CDE_MNGRP_allocateAndSetupBuffers(CDE_MNGRG_channnelInfoT* channelInfo, CDE_DRVG_channelCfgT* configuration );
static ERRG_codeE          CDE_MNGRP_setupScenarioStreamFixedSize( CDE_MNGRG_channnelInfoT* channelInfo,
                                                               CDE_DRVG_channelCfgT* configuration, CDE_MNGRG_chanCbInfoT * cbInfo );
static ERRG_codeE             CDE_MNGRP_registerOpenedChannel(CDE_MNGRG_channnelInfoT** channelInfo, CDE_DRVG_channelHandleT channelHandle);

#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
static void CDE_MNGRG_perphToDdrIntCallback( void *managerInfo, CDE_DRVG_InterruptTypeE intType, UINT64 timeStamp );
static void CDE_MNGRG_perphToDdrlessIntCallback( void *managerInfo, CDE_DRVG_InterruptTypeE intType, UINT64 timeStamp );
static void CDE_MNGRG_memoryToPerphIntCallback( void *managerInfo, CDE_DRVG_InterruptTypeE intType, UINT64 timeStamp );
#else
ERRG_codeE CDE_MNGRG_channelIntCallback( void *managerInfo, CDE_DRVG_InterruptTypeE intType,  UINT64 timeStamp );
#endif

static ERRG_codeE                 CDE_MNGRP_getPeriphPhysicalAddress( CDE_DRVG_PeriphClientIdE periphId, UINT32 *address);


/****************************************************************************
***************      L O C A L       F U N C T I O N S       ***************
****************************************************************************/
static UINT8 CDE_MNGRP_isPadModeEnabled( CDE_DRVG_portCfgT *dstCfgP )
{
   UINT8 ret = 0;

   if( (dstCfgP->dimensionCfg.frameStartX != 0) || (dstCfgP->dimensionCfg.frameStartY != 0)
       || (dstCfgP->dimensionCfg.bufferHeight > dstCfgP->dimensionCfg.imageDim.imageHeight)
       || (dstCfgP->dimensionCfg.bufferWidth  > dstCfgP->dimensionCfg.imageDim.imageWidth))
   {
      //printf("pad mode detected: %d,%d,%d,%d,%d, %d\n",
      //        dstCfgP->dimensionCfg.frameStartX, dstCfgP->dimensionCfg.frameStartY, dstCfgP->dimensionCfg.bufferHeight, dstCfgP->dimensionCfg.imageDim.imageHeight,
      //        dstCfgP->dimensionCfg.bufferWidth, dstCfgP->dimensionCfg.imageDim.imageWidth);
      ret = 1;
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: CDE_MNGRP_getChannelHandle
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
CDE_DRVG_channelHandleT CDE_MNGRP_getChannelHandle(INU_DEFSG_sysChannelIdE dmaChanId)     // TODO: replace INU_DEFSG_sysChannelIdE
{
   return ((dmaInfoDB->chanInfo[dmaChanId]).dmaChannelHandle);
}

/****************************************************************************
*
*  Function Name: CDE_MNGRP_reset
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
void CDE_MNGRP_reset(void)
{
   UINT32 i =  0;
   // Reset CDE manager data structure
   memset(dmaInfoDB, 0, sizeof(CDE_MNGRG_dmaInfoT));
   for( i = 0; i < CDE_MNGRG_MAX_AVAILABLE_CHAN; i++)
   {
      dmaInfoDB->chanInfo[i].systemChanId = 0xFF;
   }
}


/****************************************************************************
*
*  Function Name: CDE_MNGRP_allocateAndSetupMemory
*
*  Description:   Alloccate buffers and setup channel with buffers phiscal addresses
*
*  Inputs:        channelInfo, configuration
*
*  Outputs:       none
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static ERRG_codeE CDE_MNGRP_allocateAndSetupBuffers(CDE_MNGRG_channnelInfoT* channelInfo, CDE_DRVG_channelCfgT* configuration )
{
   ERRG_codeE    ret = CDE_MNGR__RET_SUCCESS;
   UINT32 buffSize = 0;                                          // size of one buffer
   CDE_DRVG_channelHandleT channelH = channelInfo->dmaChannelHandle;
   CDE_DRVG_portParamsT*   port;
   MEM_POOLG_cfgT poolCfg;
   UINT32 phyAddress;
   MEM_POOLG_bufDescT *bufDescP = NULL;
   INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();
   // Allocate buffers
   switch( configuration->streamCfg.scenarioType )
   {
      case CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_E:
      case CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_EXT_E:
         // Calculate required number of chunks
         channelH->chunksNumPerFrame = configuration->dstCfg.dimensionCfg.bufferHeight / configuration->dstCfg.dimensionCfg.numLinesPerChunk;

         /* Incase we use 2D copy + chunk mode, each buffer is allocated with the extra space for padding. It can be optimized if we managed 2 pools, and when
                    providing to the DMA the address of the bufDecr, we allocate from the correct mempool according to the loopnumber */
         #ifdef CDE_DRVG_METADATA
         /*Increase buffer height by the number of metadata rows  */
         if(configuration->regToMemoryCopyParams.numberMetadataLines > 0 )
         {
            LOGG_PRINT(LOG_INFO_E,NULL,"%lu metadata lines being used \n",configuration->regToMemoryCopyParams.numberMetadataLines);
            buffSize = (configuration->dstCfg.dimensionCfg.imageDim.pixelSize *
                     (configuration->dstCfg.dimensionCfg.bufferHeight+configuration->regToMemoryCopyParams.numberMetadataLines) *
                     configuration->dstCfg.dimensionCfg.bufferWidth) / channelH->chunksNumPerFrame;
         }
         else
         {
            LOGG_PRINT(LOG_INFO_E,NULL,"0 metadata lines being used \n");
            buffSize = (configuration->dstCfg.dimensionCfg.imageDim.pixelSize *
               configuration->dstCfg.dimensionCfg.bufferHeight*
               configuration->dstCfg.dimensionCfg.bufferWidth) / channelH->chunksNumPerFrame;
         }
         buffSize += HELSINKI_USB3_EXTRA_BUFFER_PADDING_SIZE; /*Optionally we can adding padding here if needed*/
         #else
         buffSize = (configuration->dstCfg.dimensionCfg.imageDim.pixelSize *
            configuration->dstCfg.dimensionCfg.bufferHeight*
            configuration->dstCfg.dimensionCfg.bufferWidth) / channelH->chunksNumPerFrame;
         #endif


         LOGG_PRINT(LOG_INFO_E, NULL,"Stream fixed size: %dx%dx%dx%dx%d (W x H x PixelSize x NumImagesInFrame x NumBuffers), linesPerChunk %d, chunks %d, bytes: 0x%X\n", 
                                   configuration->dstCfg.dimensionCfg.bufferWidth, 
                                   configuration->dstCfg.dimensionCfg.bufferHeight,
                                   configuration->dstCfg.dimensionCfg.imageDim.pixelSize,
                                   ( configuration->streamCfg.frameMode == CDE_DRVG_FRAME_MODE_INTERLEAVE_E ) ? 2 : 1,
                                   configuration->streamCfg.numOfBuffs,
                                   configuration->dstCfg.dimensionCfg.numLinesPerChunk, channelH->chunksNumPerFrame, buffSize);

      break;
      case CDE_DRVG_SCENARIO_TYPE_INJECTION_FIXEDSIZE_E:
         buffSize = configuration->srcCfg.dimensionCfg.imageDim.pixelSize *
                    configuration->srcCfg.dimensionCfg.bufferHeight *
                    configuration->srcCfg.dimensionCfg.bufferWidth;
         LOGG_PRINT(LOG_INFO_E, NULL,"injection: %dx%dx%dx%dx%d (W x H x PixelSize x NumImagesInFrame x NumBuffers)\n",
                                   configuration->srcCfg.dimensionCfg.bufferWidth,
                                   configuration->srcCfg.dimensionCfg.bufferHeight,
                                   configuration->srcCfg.dimensionCfg.imageDim.pixelSize,
                                   ( configuration->streamCfg.frameMode == CDE_DRVG_FRAME_MODE_INTERLEAVE_E ) ? 2 : 1,
                                   configuration->streamCfg.numOfBuffs);
         channelH->srcParams.buffSize = buffSize;
      break;
      case CDE_DRVG_SCENARIO_TYPE_STREAM_CYCLIC_E:
         // Calculate required number of chunks
         // ARNON cr - test with = 1
         channelH->chunksNumPerFrame = configuration->dstCfg.dimensionCfg.bufferHeight / configuration->dstCfg.dimensionCfg.numLinesPerChunk;
         buffSize = configuration->dstCfg.dimensionCfg.imageDim.pixelSize *
                    configuration->dstCfg.dimensionCfg.bufferHeight *
                    configuration->dstCfg.dimensionCfg.bufferWidth;
         LOGG_PRINT(LOG_INFO_E, NULL,"cyclic buffer size: %dx%dx%dx%dx%d (W x H x PixelSize x NumImagesInFrame x NumBuffers)\n",
                                  configuration->dstCfg.dimensionCfg.bufferWidth,
                                  configuration->dstCfg.dimensionCfg.bufferHeight,
                                  configuration->dstCfg.dimensionCfg.imageDim.pixelSize,
                                  ( configuration->streamCfg.frameMode == CDE_DRVG_FRAME_MODE_INTERLEAVE_E ) ? 2 : 1,
                                  configuration->streamCfg.numOfBuffs);
         channelH->dstParams.buffSize = buffSize;

         poolCfg.numBuffers = configuration->streamCfg.numOfBuffs;
         poolCfg.bufferSize = buffSize;
         poolCfg.freeCb = NULL;
         poolCfg.type = MEM_POOLG_TYPE_ALLOC_CMEM_E;
         poolCfg.memP = NULL;
         poolCfg.freeArg = NULL;
         poolCfg.resetBufPtrInAlloc = 0;
         ret = MEM_POOLG_initPool(&channelInfo->memPoolHandle, &poolCfg);
         if (ERRG_FAILED(ret))
         {
             LOGG_PRINT(LOG_INFO_E, NULL,"pool init failed. ret = %x \n", ret);
            return ret;
         }

         ret = MEM_POOLG_alloc(channelInfo->memPoolHandle, MEM_POOLG_getBufSize(channelInfo->memPoolHandle), &bufDescP);
         if (ERRG_FAILED(ret))
         {
             LOGG_PRINT(LOG_INFO_E, NULL,"allocation failed ret = %x \n", ret);
            return ret;
         }
         MEM_POOLG_getDataPhyAddr(bufDescP,&phyAddress);
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
         channelH->buffDescListP[0] = bufDescP;
#else
         channelH->curBuffDesc = bufDescP;
#endif
         channelH->dstParams.address[0] = phyAddress;

      break;
      case CDE_DRVG_SCENARIO_TYPE_IIC_STREAM_DDR_LESS:
         // Calculate required number of chunks
         channelH->chunksNumPerFrame = configuration->dstCfg.dimensionCfg.bufferHeight / configuration->dstCfg.dimensionCfg.numLinesPerChunk;

         LOGG_PRINT(LOG_INFO_E, NULL,"Stream ddrless: %dx%dx%dx%dx%d (W x H x PixelSize x NumImagesInFrame x NumBuffers)\n",
                                   configuration->dstCfg.dimensionCfg.bufferWidth,
                                   configuration->dstCfg.dimensionCfg.bufferHeight,
                                   configuration->dstCfg.dimensionCfg.imageDim.pixelSize,
                                   ( configuration->streamCfg.frameMode == CDE_DRVG_FRAME_MODE_INTERLEAVE_E ) ? 2 : 1,
                                   configuration->streamCfg.numOfBuffs);
         channelH->dstParams.buffSize = configuration->dstCfg.dimensionCfg.bufferSize;
         poolCfg.numBuffers = 1;
         poolCfg.bufferSize = buffSize;
         poolCfg.freeCb = NULL;
         poolCfg.type = MEM_POOLG_TYPE_ALLOC_USER_E;
         poolCfg.memP = NULL;
         poolCfg.freeArg = NULL;
         poolCfg.resetBufPtrInAlloc = 0;
         ret = MEM_POOLG_initPool(&channelInfo->memPoolHandle, &poolCfg);
         if (ERRG_FAILED(ret))
         {
             LOGG_PRINT(LOG_INFO_E, NULL,"DDR_LESS: pool init failed. ret = %x \n", ret);
            return ret;
         }

      break;
      default:
         buffSize = 0;
         LOGG_PRINT(LOG_ERROR_E, ret, "Scenario type is not implemented\n");
      break;
   }

   buffSize = ( configuration->streamCfg.frameMode == CDE_DRVG_FRAME_MODE_INTERLEAVE_E ) ? (buffSize * 2) : (buffSize); // if interleave - buffsize * 2
#ifdef CDE_DRVG_METADATA
         /*Offset the frame buffer by number of lines of metadata */
         if(configuration->regToMemoryCopyParams.numberMetadataLines > 0 )
         {
            channelInfo->dmaChannelHandle->framebuffer_offset = configuration->regToMemoryCopyParams.numberMetadataLines * 
            (configuration->dstCfg.dimensionCfg.imageDim.pixelSize * configuration->dstCfg.dimensionCfg.bufferWidth);
            LOGG_PRINT(LOG_INFO_E, NULL,"Offsetting frame buffer by %lu bytes for metadata due to %lu lines of metadata requested \n", channelInfo->dmaChannelHandle->framebuffer_offset, configuration->regToMemoryCopyParams.numberMetadataLines);
         }
    
#endif
   switch( configuration->streamCfg.scenarioType )
   {
      case CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_E:
      case CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_EXT_E:
         /*
                  Setting numLoops define the program length. We always want to keep enough buffers when feeding the DMA new buffers.
                  The maximum program length is 10, due to the DMA loop register width which is only 8 bits.
                  We set the num loops to 1 less the number of buffers. We do this to make sure we don't use all the buffers in the program.
                  (If we do use all the buffers in the program, when the first buffer is ready, there won't be any free buffer to give, and it will
                  be set as drain)
               */
         channelH->numLoops = MIN(((configuration->streamCfg.numOfBuffs * channelH->chunksNumPerFrame) - 1) , MAX_NUM_BUFFERS_LOOPS);
         poolCfg.numBuffers = (configuration->streamCfg.numOfBuffs * channelH->chunksNumPerFrame) + 1 + CDE_MNGRP_EXTRA_FRAME_BUFFER;//+1 for drain
         poolCfg.bufferSize = buffSize;
         poolCfg.freeCb = NULL;

 //        if ( modelType == INU_DEFSG_BOOT85_E)
 //            poolCfg.type = MEM_POOLG_TYPE_ALLOC_KERSPC_E;
 //        else 
		 poolCfg.type = MEM_POOLG_TYPE_ALLOC_CMEM_E;
         poolCfg.memP = NULL;
         poolCfg.freeArg = NULL;
         poolCfg.resetBufPtrInAlloc = 0;
         ret = MEM_POOLG_initPool(&channelInfo->memPoolHandle, &poolCfg);
         if (ERRG_FAILED(ret))
         {
            return ret;
         }

         if(configuration->srcCfg.peripheral == CDE_DRVG_PERIPH_CLIENT_CVA2_IMG_TX_E) //2 NOTE: this code is part of DoG -Freak implementation
         {
            // In DOG FREAK implementation, we need to zero the first row of integral image buffer
            ret = CDE_MNGRG_colorDataBuffers( channelInfo, 0x00 );
         }

         port = &(channelH->dstParams);
         port->imgStartX        = configuration->dstCfg.dimensionCfg.frameStartX;
         port->imgStartY        = configuration->dstCfg.dimensionCfg.frameStartY;
         port->imgWidth         = configuration->dstCfg.dimensionCfg.imageDim.imageWidth;
         port->stride           = configuration->dstCfg.dimensionCfg.bufferWidth;
         port->imgBpp           = configuration->dstCfg.dimensionCfg.imageDim.pixelSize;
         port->buffSize         = buffSize;
         port->numberOfBuffers  = configuration->streamCfg.numOfBuffs;

         if (CDE_MNGRP_isPadModeEnabled(&configuration->dstCfg))
         {
            port->offset = (port->imgStartX + (port->imgStartY * port->stride)) * port->imgBpp;
            ret = CDE_MNGRG_colorDataBuffers( channelInfo, 0x00 );
            //printf ("Zeroing destination buffers for src_periph  %d \n", configuration->srcCfg.peripheral);
         }
         else
         {
            port->offset = 0;
         }

         port->periphNum        = CDE_DRVG_PERIPH_NUM_MEMORY;
         channelInfo->dmaChannelHandle->memPoolHandle = channelInfo->memPoolHandle;
         // drain allocation temporary
         ret = MEM_POOLG_alloc(channelInfo->memPoolHandle, MEM_POOLG_getBufSize(channelInfo->memPoolHandle), &channelInfo->dmaChannelHandle->drainingBuffDesc);
         if(ERRG_FAILED(ret))
         {
            printf ("fail to allocate drain buffer \n");
            return ret;
         }
         // ease access during isr
         MEM_POOLG_getDataPhyAddr(channelInfo->dmaChannelHandle->drainingBuffDesc,&channelInfo->dmaChannelHandle->drainingBuffAddr);
         channelInfo->dmaChannelHandle->drainCtr = 0;

         LOGG_PRINT(LOG_DEBUG_E, NULL,"port = %p, port->imgStartX = %d, port->imgStartY = %d, port->imgWidth = %d, port->stride = %d \n", port, port->imgStartX, port->imgStartY, port->imgWidth, port->stride );
      break;
      case CDE_DRVG_SCENARIO_TYPE_INJECTION_FIXEDSIZE_E:
     //    CDE_DRVG_setBufferParams( &channelH->srcParams, configuration->streamCfg.numOfBuffs, buffSize, physicalBaseAddress ,1);
         port = &(channelH->srcParams);
         port->imgStartX        = configuration->srcCfg.dimensionCfg.frameStartX;
         port->imgStartY        = configuration->srcCfg.dimensionCfg.frameStartY;
         port->imgWidth         = configuration->srcCfg.dimensionCfg.imageDim.imageWidth;
         port->imgHeight        = configuration->srcCfg.dimensionCfg.imageDim.imageHeight;
         port->stride           = configuration->srcCfg.dimensionCfg.stride;
         port->imgBpp           = configuration->srcCfg.dimensionCfg.imageDim.pixelSize;
         port->buffSize         = buffSize;
         port->buffWidth        = configuration->srcCfg.dimensionCfg.bufferWidth;
         port->numberOfBuffers  = configuration->streamCfg.numOfBuffs;
         port->offset           = port->imgStartX;
         port->numLoopsPerLine  = configuration->srcCfg.dimensionCfg.numLoopsPerLine;
      break;
      case CDE_DRVG_SCENARIO_TYPE_IIC_STREAM_DDR_LESS:
      case CDE_DRVG_SCENARIO_TYPE_STREAM_CYCLIC_E:
      break;
      default:
         return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                        // TODO: make a new error code
      break;

   }

   return ret;
}



/****************************************************************************
*
*  Function Name: CDE_MNGRG_registerOpenedChannel
*
*  Description:   find free place in dmaInfoDB and register onto it
*
*  Inputs:        channelHandle
*
*  Outputs:       channelInfo
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE CDE_MNGRP_registerOpenedMetadataChannel(CDE_MNGRG_channnelInfoT** channelInfo, CDE_DRVG_channelHandleT channelHandle)
{
   ERRG_codeE  ret = CDE_MNGR__RET_SUCCESS;
   UINT8 i = 0;
   for( i = 0; i < CDE_MNGRG_MAX_AVAILABLE_CHAN; i++  )
   {
      /*TODO: Understand and document this code as it's not very clear what this actually does
      I am guessing it's assigning a physical channel to this virtual channel
      */
      if(dmaInfoDB->chanInfo[i].channelStatus == CDE_MNGRG_CHAN_STATUS_CLOSED_E )
      {
      channelHandle->managerInfo = (*channelInfo)->dmaChannelHandle->managerInfo;                      // register manager handle with driver channel
      dmaInfoDB->chanInfo[i].metadataDMAChannelHandle  = channelHandle;                     // register driver handle with manager info
      dmaInfoDB->chanInfo[i].channelStatus = CDE_MNGRG_CHAN_STATUS_OPENED_E;
   #ifdef CDE_DRVG_VIRTUAL_CHANNELS
            LOGG_PRINT(METADATA_DEBUG_LEVEL, ret,"METADATA_channel %d occupied, mngrInfo = %p. Using Core %d, channel %d\n", i, channelHandle->managerInfo, channelHandle->coreNum, channelHandle->virtualChannelNum);
   #else
            LOGG_PRINT(LOG_DEBUG_E, ret,"METADATA_channel %d occupied, mngrInfo = %p. Using Core %d, channel %d\n", i, channelHandle->managerInfo, channelHandle->coreNum, channelHandle->channelNum );
   #endif
         return ret;
      }
   }
   return CDE__ERR_INIT_FAIL_MNGR_FAIL;
}

ERRG_codeE CDE_MNGRP_registerOpenedChannel(CDE_MNGRG_channnelInfoT** channelInfo, CDE_DRVG_channelHandleT channelHandle)
{
   ERRG_codeE  ret = CDE_MNGR__RET_SUCCESS;
   UINT8 i = 0;

   for( i = 0; i < CDE_MNGRG_MAX_AVAILABLE_CHAN; i++  )
   {
      if( dmaInfoDB->chanInfo[i].channelStatus == CDE_MNGRG_CHAN_STATUS_CLOSED_E )
      {
         *channelInfo = &dmaInfoDB->chanInfo[i];
         channelHandle->managerInfo = &dmaInfoDB->chanInfo[i];                      // register manager handle with driver channel
         dmaInfoDB->chanInfo[i].dmaChannelHandle  = channelHandle;                     // register driver handle with manager info
         dmaInfoDB->chanInfo[i].channelStatus = CDE_MNGRG_CHAN_STATUS_OPENED_E;

#ifdef CDE_DRVG_VIRTUAL_CHANNELS
         LOGG_PRINT(LOG_INFO_E, ret,"channel %d occupied, mngrInfo = %p. Using Core %d, channel %d\n", i, channelHandle->managerInfo, channelHandle->coreNum, channelHandle->virtualChannelNum);
#else
         LOGG_PRINT(LOG_DEBUG_E, ret,"channel %d occupied, mngrInfo = %p. Using Core %d, channel %d\n", i, channelHandle->managerInfo, channelHandle->coreNum, channelHandle->channelNum );
#endif
         return ret;
      }
   }

   return CDE__ERR_INIT_FAIL_MNGR_FAIL;
}
static ERRG_codeE CDE_MNGRP_setupScenarioRegistersToMemory( CDE_MNGRG_channnelInfoT* channelInfo, CDE_DRVG_channelCfgT* configuration, CDE_MNGRG_chanCbInfoT* cbInfo )
{
   ERRG_codeE              ret = CDE_MNGR__RET_SUCCESS;
   CDE_DRVG_PeriphClientIdU   periph = (CDE_DRVG_PeriphClientIdU)configuration->srcCfg.peripheral;
   UINT32                  periphPhyAddress = 0;            /*No peripheral used for this channel */
      CDE_DRVG_channelHandleT    paired_channel = channelInfo->dmaChannelHandle;
   CDE_DRVG_channelHandleT    channelHandle = channelInfo->metadataDMAChannelHandle;
   
   ret = CDE_DRVG_setPeripheralParams( &channelHandle->srcParams, periph.field.periphPort, periphPhyAddress );
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to set peripheral\n");
      return ret;
   }
#ifdef CDE_DRVG_METADATA
         MEM_POOLG_cfgT metadata_pool_cfg;
         const INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();
         UINT32 numBuffers = channelInfo->dmaChannelHandle->numLoops+1;
         ret = METADATA_TARGET_CONFIG_getMemPoolConfig(&metadata_pool_cfg,modelType,(numBuffers));
         if (ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, 0, "Could not get metadata pool \n");
            assert(0);
         }
         LOGG_PRINT(LOG_INFO_E, NULL, "Creating metadata pool with size %lu bytes\n", metadata_pool_cfg.bufferSize);
         ret = MEM_POOLG_initPool(&channelHandle->metadata_memPoolHandle, &metadata_pool_cfg);
         if (ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, 0, "Could not create metadata pool \n");
            return ret;
         }
         LOGG_PRINT(LOG_INFO_E, 0, "Created metadata pool with size:%d \n",metadata_pool_cfg.bufferSize );
#endif

   ret = CDE_DRVG_configureregToMemoryCopyParams(channelHandle,configuration->regToMemoryCopyParams);
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to configure Reg to memory copy\n");
      return ret;
   }
   ret = CDE_DRVG_configureScenario( channelHandle, configuration );  //change 2
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to configure scenario\n");
      return ret;
   }
   ret = CDE_MNGRG_registerUserMetadataCallBacks( channelInfo, channelInfo->metadataCallBacksInfo );
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to register callbacks\n");
      return ret;
   }
   ret = CDE_DRVG_generateChannelProgram( channelHandle, NULL );
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to configure scenario\n");
      return ret;
   }
   else
   {
      channelHandle->intCb = CDE_MNGRG_perphToDdrIntCallback;
      //channelInfo->channelStatus = CDE_MNGRG_CHAN_STATUS_CONFIGURED_E;
   }

   return ret;
   
}
/****************************************************************************
*
*  Function Name: CDE_MNGRG_registerOpenedChannel
*
*  Description:   find free place in dmaInfoDB and register onto it
*
*  Inputs:        channelHandle
*
*  Outputs:       channelInfo
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static ERRG_codeE CDE_MNGRP_setupScenarioStreamFixedSize( CDE_MNGRG_channnelInfoT* channelInfo, CDE_DRVG_channelCfgT* configuration, CDE_MNGRG_chanCbInfoT* cbInfo )
{
   ERRG_codeE              ret = CDE_MNGR__RET_SUCCESS;
   CDE_DRVG_PeriphClientIdU   periph = (CDE_DRVG_PeriphClientIdU)configuration->srcCfg.peripheral;
   UINT32                  periphPhyAddress;
   CDE_DRVG_channelHandleT    channelHandle = channelInfo->dmaChannelHandle;


   ret = CDE_MNGRP_allocateAndSetupBuffers( channelInfo, configuration );
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to allocate memory\n");
      return ret;
   }

   ret = CDE_MNGRP_getPeriphPhysicalAddress( configuration->srcCfg.peripheral, &periphPhyAddress );

   ret = CDE_DRVG_setPeripheralParams( &channelHandle->srcParams, periph.field.periphPort, periphPhyAddress );
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to set peripheral\n");
      return ret;
   }

   ret = CDE_DRVG_configureScenario( channelHandle, configuration );  //change 2
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to configure scenario\n");
      return ret;
   }
   ret = CDE_MNGRG_registerUserCallBacks( channelInfo, cbInfo );
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to register callbacks\n");
      return ret;
   }

   if (!channelInfo->extInfo.useExtIntMode)
   {
      ret = CDE_DRVG_generateChannelProgram( channelInfo->dmaChannelHandle, NULL );
   }
   else
   {
      CDE_DRVG_extIntParams drvExtIntParams;

      drvExtIntParams.numSubChannels = channelInfo->extInfo.subChannels;
      for (UINT32 i = 0; i < channelInfo->extInfo.subChannels; i++)
      {
         drvExtIntParams.subChannelHandles[i] = channelInfo->extInfo.subChannelsInfo[i]->dmaChannelHandle;
      }

      ret = CDE_DRVG_generateChannelProgram( channelInfo->dmaChannelHandle, (void *)&drvExtIntParams);
   }
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to configure scenario\n");
      return ret;
   }
   else
   {
      channelHandle->intCb = CDE_MNGRG_perphToDdrIntCallback;
      channelInfo->channelStatus = CDE_MNGRG_CHAN_STATUS_CONFIGURED_E;
   }

   return ret;

}

static ERRG_codeE CDE_MNGRP_setupScenarioDdrless( CDE_MNGRG_channnelInfoT* channelInfo, CDE_DRVG_channelCfgT* configuration, CDE_MNGRG_chanCbInfoT* cbInfo )
{
    ERRG_codeE                  ret = CDE_MNGR__RET_SUCCESS;
    CDE_DRVG_PeriphClientIdU    periph = (CDE_DRVG_PeriphClientIdU)configuration->srcCfg.peripheral;
    UINT32                      periphPhyAddress;
    CDE_DRVG_channelHandleT     channelHandle = channelInfo->dmaChannelHandle;

    ret = CDE_MNGRP_allocateAndSetupBuffers( channelInfo, configuration );
    if(ERRG_FAILED(ret))
    {
       LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to allocate memory\n");
       return ret;
    }

    ret = CDE_MNGRP_getPeriphPhysicalAddress( configuration->srcCfg.peripheral, &periphPhyAddress );

    ret = CDE_DRVG_setPeripheralParams( &channelHandle->srcParams, periph.field.periphPort, periphPhyAddress );
    if(ERRG_FAILED(ret))
    {
     LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to set peripheral\n");
     return ret;
    }

    ret = CDE_DRVG_configureScenario( channelHandle, configuration );
    if(ERRG_FAILED(ret))
    {
     LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to configure scenario\n");
     return ret;
    }

    ret = CDE_MNGRG_registerUserCallBacks( channelInfo, cbInfo );
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to register callbacks\n");
        return ret;
    }
    ret = CDE_DRVG_generateChannelProgram( channelInfo->dmaChannelHandle, NULL );
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to configure scenario\n");
        return ret;
    }
    else
    {
        channelHandle->intCb = CDE_MNGRG_perphToDdrlessIntCallback;
        channelInfo->channelStatus = CDE_MNGRG_CHAN_STATUS_CONFIGURED_E;
    }

    return ret;
}

ERRG_codeE CDE_MNGRG_generateChannelProgram( CDE_MNGRG_channnelInfoT *channelInfo )
{
   ERRG_codeE            ret = CDE_MNGR__RET_SUCCESS;
   if (channelInfo)
   {
      if (channelInfo->channelStatus < CDE_MNGRG_CHAN_STATUS_CONFIGURED_E)
      {
         ret = CDE_DRVG_generateChannelProgram( channelInfo->dmaChannelHandle, NULL );
         if(ERRG_FAILED(ret))
         {
            memset(channelInfo->callBacksInfo,0,sizeof(CDE_MNGRG_chanCbInfoT) * CDE_MNGRG_NUM_CHANNEL_CB );//for unregister callbacks from previous function
            LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to configure scenario\n");
            return ret;
         }
         else
         {
            channelInfo->channelStatus = CDE_MNGRG_CHAN_STATUS_CONFIGURED_E;
         }
      }
   }
   else
   {
      ret = CDE__ERR_DRV_CH_CFG_NULL_HANDLE;
   }

   return ret;

}


/****************************************************************************
*
*  Function Name: CDE_MNGRP_setupScenarioInjectionFixedSize
*
*  Description:   find free place in dmaInfoDB and register onto it
*
*  Inputs:        channelHandle
*
*  Outputs:       channelInfo
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static ERRG_codeE CDE_MNGRP_setupScenarioInjectionFixedSize( CDE_MNGRG_channnelInfoT *channelInfo, CDE_DRVG_channelCfgT *configuration, CDE_MNGRG_chanCbInfoT *cbInfo )
{
   ERRG_codeE              ret = CDE_MNGR__RET_SUCCESS;
   CDE_DRVG_PeriphClientIdU   periph = (CDE_DRVG_PeriphClientIdU)configuration->dstCfg.peripheral;
   UINT32                  periphPhyAddress;
   CDE_DRVG_channelHandleT    channelHandle = channelInfo->dmaChannelHandle;
   (void)cbInfo;

   ret = CDE_MNGRP_allocateAndSetupBuffers( channelInfo, configuration );
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to allocate memory\n");
      return ret;
   }
   ret = CDE_MNGRP_getPeriphPhysicalAddress( configuration->dstCfg.peripheral, &periphPhyAddress );

   ret = CDE_DRVG_setPeripheralParams( &channelHandle->dstParams, periph.field.periphPort, periphPhyAddress );
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to set peripheral\n");
      return ret;
   }
   ret = CDE_DRVG_configureScenario( channelHandle, configuration );
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to configure scenario\n");
      return ret;
   }
   else
   {
      channelHandle->intCb = CDE_MNGRG_memoryToPerphIntCallback;
   }

   return ret;

}

/****************************************************************************
*
*  Function Name: CDE_MNGRG_registerOpenedChannel
*
*  Description:   find free place in dmaInfoDB and register onto it
*
*  Inputs:        channelHandle
*
*  Outputs:       channelInfo
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static ERRG_codeE CDE_MNGRP_setupScenarioMemcopy( CDE_MNGRG_channnelInfoT* channelInfo, CDE_DRVG_channelCfgT* configuration, CDE_MNGRG_chanCbInfoT* cbInfo )
{
   ERRG_codeE              ret = CDE_MNGR__RET_SUCCESS;
   CDE_DRVG_channelHandleT    channelH = channelInfo->dmaChannelHandle;

   //real addresses will be supplied when memcpy is called
   configuration->memcpyCfg.srcAddress = 0;
   configuration->memcpyCfg.dstAddress = 0;

   //todo: needs to be placed in cde_drv
   channelH->srcParams.buffSize = configuration->memcpyCfg.buffSize;
   channelH->dstParams.buffSize = configuration->memcpyCfg.buffSize;

   ret = CDE_DRVG_configureScenario( channelH, configuration );
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to configure scenario\n");
      return ret;
   }
   ret = CDE_MNGRG_registerUserCallBacks( channelInfo, cbInfo );
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to register callbacks\n");
      return ret;
   }
   ret = CDE_DRVG_generateChannelProgram( channelInfo->dmaChannelHandle, NULL );
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to configure scenario\n");
      return ret;
   }
   else
   {
      channelH->intCb = CDE_MNGRG_perphToDdrIntCallback;
      channelInfo->channelStatus = CDE_MNGRG_CHAN_STATUS_CONFIGURED_E;
   }

   return ret;

}

/****************************************************************************
*
*  Function Name: CDE_MNGRP_setupScenarioStreamCyclic
*
*  Description:
*
*  Inputs:        channelHandle
*
*  Outputs:       channelInfo
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static ERRG_codeE CDE_MNGRP_setupScenarioStreamCyclic( CDE_MNGRG_channnelInfoT* channelInfo, CDE_DRVG_channelCfgT* configuration, CDE_MNGRG_chanCbInfoT* cbInfo )
{
    ERRG_codeE              ret = CDE_MNGR__RET_SUCCESS;
    CDE_DRVG_PeriphClientIdU   periph = (CDE_DRVG_PeriphClientIdU)configuration->srcCfg.peripheral;
    UINT32                  periphPhyAddress;
    CDE_DRVG_channelHandleT    channelHandle = channelInfo->dmaChannelHandle;


    ret = CDE_MNGRP_allocateAndSetupBuffers( channelInfo, configuration );
    if(ERRG_FAILED(ret))
    {
       LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to allocate memory\n");
       return ret;
    }

    ret = CDE_MNGRP_getPeriphPhysicalAddress( configuration->srcCfg.peripheral, &periphPhyAddress );

    ret = CDE_DRVG_setPeripheralParams( &channelHandle->srcParams, periph.field.periphPort, periphPhyAddress );
    if(ERRG_FAILED(ret))
    {
       LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to set peripheral\n");
       return ret;
    }

    ret = CDE_DRVG_configureScenario( channelHandle, configuration );  //change 2
    if(ERRG_FAILED(ret))
    {
       LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to configure scenario\n");
       return ret;
    }
    ret = CDE_DRVG_generateChannelProgram( channelInfo->dmaChannelHandle, NULL );
    if(ERRG_FAILED(ret))
    {
       LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to configure scenario\n");
       return ret;
    }
    else
    {
       channelHandle->intCb = CDE_MNGRG_perphToDdrIntCallback;
       channelInfo->channelStatus = CDE_MNGRG_CHAN_STATUS_CONFIGURED_E;
    }

    return ret;
}

/****************************************************************************
*
*  Function Name:
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static ERRG_codeE  CDE_MNGRP_getPeriphPhysicalAddress( CDE_DRVG_PeriphClientIdE periphId, UINT32 *address)
{
   MEM_MAPG_addrT             fifoMemAddr;

   switch( periphId )
   {
      case CDE_DRVG_PERIPH_CLIENT_AXI_RD0_TX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_RD0_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_AXI_RD1_TX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_RD1_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_AXI_RD2_TX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_RD2_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_AXI_RD3_TX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_RD3_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_AXI_RD4_TX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_RD4_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_AXI_RD5_TX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_RD5_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_AXI_RD8_TX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_RD8_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_AXI_RD9_TX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_RD9_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_AXI_WB0_RX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_WB0_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_AXI_WB1_RX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_WB1_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_AXI_WB2_RX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_WB2_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_CVA0_IMG_TX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_CVA0_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_CVA1_IMG_TX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_CVA1_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_CVA2_IMG_TX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_CVA2_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_CVA3_KP_DESC_TX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_CVA3_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_CVA4_IMG_TX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_CVA4_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_CVA5_KP_DESC_TX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_CVA5_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_CVA6_CTRL_TX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_CVA6_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_CVJ0_HD_RX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_CVJ0_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_CVJ1_HD_RX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_CVJ1_E,&fifoMemAddr);
      break;
      case CDE_DRVG_PERIPH_CLIENT_CVJ2_KP_RX_E:
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_CVJ2_E,&fifoMemAddr);
      break;

      default:
         LOGG_PRINT(LOG_ERROR_E, NULL, "Could not find phy address for periph 0x%x\n",periphId);
         assert(0);
         MEM_MAPG_getPhyAddr(MEM_MAPG_AXI_RD0_E,&fifoMemAddr);
      break;

   }

   *address = (UINT32)(fifoMemAddr);

   return CDE_MNGR__RET_SUCCESS;
}

/****************************************************************************
***************     G L O B A L       F U N C T I O N S  **************
****************************************************************************/

/****************************************************************************
*
*  Function Name: CDE_MNGRG_init
*
*  Description:   Reset all the manager and driver data and acquire driver address
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
ERRG_codeE CDE_MNGRG_init( void )
{
   ERRG_codeE  ret = CDE_MNGR__RET_SUCCESS;

   UINT32      dmaMngrDbVirtualAddr = 0;
   UINT32      dmaRegistersVirtualAddr = 0;
   UINT32      dmaDriverDBVirtualAddr = 0;
   UINT32      size;

#ifdef __BARE_METAL__
   dmaRegistersVirtualAddr = 0x08140000;
#else

   ret = MEM_MAPG_getVirtAddr(MEM_MAPG_CRAM_CDE_MNGR_DB_E , ((MEM_MAPG_addrT)&dmaMngrDbVirtualAddr));

   dmaInfoDB = (CDE_MNGRG_dmaInfoT *)dmaMngrDbVirtualAddr;

   CDE_MNGRP_reset( );
   ret = MEM_MAPG_getVirtAddr(MEM_MAPG_REG_CDE_E,  ((MEM_MAPG_addrT)&dmaRegistersVirtualAddr));
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to get driver address\n");
   }


   ret = MEM_MAPG_getVirtAddr(MEM_MAPG_CRAM_CDE_PARAMS_E ,  ((MEM_MAPG_addrT)&dmaDriverDBVirtualAddr));
   ret = MEM_MAPG_getSize    (MEM_MAPG_CRAM_CDE_PARAMS_E , &size);
#endif   //__BARE_METAL__
#ifndef CDE_DRVG_VIRTUAL_CHANNELS
   if (sizeof(CDE_MNGRG_dmaInfoT) > size)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "DMA HAS DB IS LARGER THEN CRAM ALLOCATED MEM! %d > %d \n",size,sizeof(CDE_MNGRG_dmaInfoT));
      assert(0);
   }
#endif

   ret = CDE_DRVG_init(dmaRegistersVirtualAddr, dmaDriverDBVirtualAddr, size);

   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "failed to init CDE driver\n");
   }
   else
   {
      dmaInfoDB->drvStatus = CDE_MNGRG_DRV_STATUS_CLOSED_E;
      //LOGG_PRINT(LOG_INFO_E, NULL, "initialized CDE driver\n");
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: CDE_MNGRG_init
*
*  Description:     Reset all the manager and driver data and acquire driver address
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
ERRG_codeE CDE_MNGRG_open( void )
{
   ERRG_codeE  ret = CDE_MNGR__RET_SUCCESS;

   if( dmaInfoDB->drvStatus != CDE_MNGRG_DRV_STATUS_CLOSED_E)
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "failed to open CDE driver\n");
      return CDE__ERR_INIT_FAIL_MNGR_FAIL;
   }
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
   ret = CDE_DRVG_open();
#else
   ret = CDE_DRVG_open(&CDE_MNGRG_channelIntCallback);
#endif
   if(ERRG_FAILED(ret))                                        // if driver failed, return the fail to higher level
   {
      return ret;
   }

   dmaInfoDB->drvStatus = CDE_MNGRG_DRV_STATUS_OPENED_E;

   return ret;
}

/****************************************************************************
*
*  Function Name: CDE_MNGRG_init
*
*  Description:     Reset all the manager and driver data and acquire driver address
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
ERRG_codeE CDE_MNGRG_close( void )
{
   ERRG_codeE  ret = CDE_MNGR__RET_SUCCESS;

   ret = CDE_DRVG_close( );

   dmaInfoDB->drvStatus = CDE_MNGRG_DRV_STATUS_CLOSED_E;


   return ret;
}



/****************************************************************************
*
*  Function Name: CDE_MNGRG_openChannel
*
*  Description:
*
*  Inputs:        source, destination
*
*  Outputs:       channelInfo
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE CDE_MNGRG_openChannel(CDE_MNGRG_channnelInfoT** channelInfo, CDE_DRVG_PeriphClientIdE source, CDE_DRVG_PeriphClientIdE destination)
{
   ERRG_codeE ret;
   CDE_DRVG_channelHandleT channelHandle;

   LOGG_PRINT(LOG_DEBUG_E, NULL, "Opening DMA channel for src 0x%x, dst 0x%x \n", source, destination);
   ret = CDE_DRVG_openChannel( &channelHandle, source, destination );         // open driver according to source and destination
   if(ERRG_FAILED(ret))                                        // if driver failed, return the fail to higher level
   {
      return ret;
   }

   ret = CDE_MNGRP_registerOpenedChannel( channelInfo, channelHandle );    // register channel with manager DB
   
   
   if(ERRG_FAILED(ret))                                        // if driver failed, return the fail to higher level
   {
      return ret;
   }


   return(CDE_MNGR__RET_SUCCESS);

}


/****************************************************************************
*
*  Function Name: CDE_MNGRP_waitOutChBufTblFree
*
*  Description: Wait for out channel buffer descriptors to become free
*     Should be called before closeing/stopping a HW DMA channel.
*
*  Inputs:
*
*  Outputs:
*
*  Returns: success or timeout
*
*  Context:
*
****************************************************************************/
static ERRG_codeE CDE_MNGRP_waitOutChBufTblFree(CDE_MNGRG_channnelInfoT* channelInfo)
{
   ERRG_codeE                 retCode        = CDE_MNGR__RET_SUCCESS;
   UINT32 sleep = 0;

   while(MEM_POOLG_getNumFree(channelInfo->memPoolHandle) < MEM_POOLG_getNumBufs(channelInfo->memPoolHandle))
   {
      OS_LYRG_usleep(CDE_MNGRP_OUT_CHAN_SLEEP_USEC);
      sleep += CDE_MNGRP_OUT_CHAN_SLEEP_USEC;
      if(sleep >= CDE_MNGRP_OUT_CHAN_TIMEOUT_USEC)
      {
         retCode = HW_MNGR__ERR_OUT_CHAN_FREE_TIMEOUT;
         break;
      }
   }

   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "wait DMA channel output buffers fail. dmaChanId=%d, num of free buffers=%d, number of buffers=%d\n", channelInfo->systemChanId, MEM_POOLG_getNumFree(channelInfo->memPoolHandle), MEM_POOLG_getNumBufs(channelInfo->memPoolHandle));
   }
   return retCode;
}



/****************************************************************************
*
*  Function Name: CDE_MNGRG_closeChannel
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
ERRG_codeE CDE_MNGRG_closeChannel(CDE_MNGRG_channnelInfoT* channelInfo)
{

   ERRG_codeE ret;

   if( channelInfo->channelStatus < CDE_MNGRG_CHAN_STATUS_OPENED_E )                // if channel not closed - it can't be open
   {
      ret = CDE__ERR_INIT_FAIL_MNGR_FAIL;
      return ret;
   }
   else
   {
      if (channelInfo->memPoolHandle)
      {
         MEM_POOLG_closePool( channelInfo->memPoolHandle );
      }

      if (channelInfo->metadataDMAChannelHandle)
      {
         CDE_DRVG_channelHandleT m = channelInfo->metadataDMAChannelHandle;

         if(m->metadata_memPoolHandle)
            MEM_POOLG_closePool( m->metadata_memPoolHandle );
      }
      
      if(channelInfo->dmaChannelHandle)
      {
         ret = CDE_DRVG_closeChannel( channelInfo->dmaChannelHandle );
         if(ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to close CDE driver\n");
            return ret;
         }
      }
      if(channelInfo->metadataDMAChannelHandle)
      {
         ret = CDE_DRVG_closeChannel( channelInfo->metadataDMAChannelHandle );
         if(ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "CDE MNGR failed to close CDE driver\n");
            return ret;
         }
      }
      memset((void*)channelInfo, 0x00, sizeof(CDE_MNGRG_channnelInfoT));
      channelInfo->channelStatus    = CDE_MNGRG_CHAN_STATUS_CLOSED_E;
      channelInfo->systemChanId  = 0xFF;
   }
   return(CDE_MNGR__RET_SUCCESS);

}

/****************************************************************************
*
*  Function Name: CDE_MNGRG_setupChannel
*
*  Description:
*
*  Inputs:        configuration, cbInfo
*
*  Outputs:       channelInfo
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE CDE_MNGRG_startChannel(CDE_MNGRG_channnelInfoT* channelInfo)
{
   CDE_DRVG_channelHandleT h = channelInfo->dmaChannelHandle;
   if( channelInfo->channelStatus == CDE_MNGRG_CHAN_STATUS_CONFIGURED_E )
   {
      if (channelInfo->extInfo.useExtIntMode)
      {
         CDE_DRVG_extIntParams extIntParams;

         extIntParams.numSubChannels = channelInfo->extInfo.subChannels;
         for (UINT32 i = 0; i < channelInfo->extInfo.subChannels; i++)
         {
            extIntParams.subChannelHandles[i] = channelInfo->extInfo.subChannelsInfo[i]->dmaChannelHandle;
         }
         CDE_DRVG_startChannelExt(h, &extIntParams);
      }
      else
      {
         CDE_DRVG_startChannel( h );
      }
      channelInfo->channelStatus = CDE_MNGRG_CHAN_STATUS_RUNNING_E;
      
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Started DMA channel on core %d/channel %d \n", h->coreNum, h->hwChannelNum);
      return(CDE_MNGR__RET_SUCCESS);
   }
   else
   {
      if( channelInfo->channelStatus == CDE_MNGRG_CHAN_STATUS_RUNNING_E ) //memcpy does not change state back to configured
      {
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
         LOGG_PRINT(LOG_INFO_E, NULL, "Already started DMA channel on core %d/channel %d \n", h->coreNum, h->hwChannelNum);
#else
         LOGG_PRINT(LOG_INFO_E, NULL, "Already started DMA channel on core %d/channel %d \n", h->coreNum, h->channelNum);
#endif
         return CDE_MNGR__ERR_ILLEGAL_STATE;
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Invalid DMA CHANNEL status on core %d/channel %d \n", h->coreNum, h->hwChannelNum);
         return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                        // TODO: make a new error code
      }
   }
}

/****************************************************************************
*
*  Function Name: CDE_MNGRG_setupChannel
*
*  Description:
*
*  Inputs:        configuration, cbInfo
*
*  Outputs:       channelInfo
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE CDE_MNGRG_stopChannelMemcpy(CDE_MNGRG_channnelInfoT* channelInfo)
{
   ERRG_codeE ret = CDE_MNGR__RET_SUCCESS;
   CDE_DRVG_channelHandleT h;// = channelInfo->dmaChannelHandle;

   if( channelInfo->channelStatus == CDE_MNGRG_CHAN_STATUS_RUNNING_E)
   {
      h = channelInfo->dmaChannelHandle;
      CDE_DRVG_stopChannel( h );
      channelInfo->channelStatus = CDE_MNGRG_CHAN_STATUS_CONFIGURED_E;
      //LOGG_PRINT(LOG_INFO_E, NULL, "Stopped DMA channel on core %d/channel %d \n", h->coreNum, h->channelNum);
   }
   return ret;
}


ERRG_codeE CDE_MNGRG_resumeChannel(CDE_MNGRG_channnelInfoT* channelInfo)
{
   ERRG_codeE ret = CDE_MNGR__RET_SUCCESS;
   CDE_DRVG_channelHandleT h;// = channelInfo->dmaChannelHandle;
   if( channelInfo->channelStatus == CDE_MNGRG_CHAN_STATUS_CONFIGURED_E)
   {
      h = channelInfo->dmaChannelHandle;
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      //printf("resume: %d/%d h %p\n",h->coreNum,h->hwChannelNum,h);
#else
      //printf("resume: %d/%d h %p\n",h->coreNum,h->channelNum,h);
#endif
      channelInfo->channelStatus = CDE_MNGRG_CHAN_STATUS_RUNNING_E;
      CDE_DRVG_startChannel( h );
   }
   else
   {
      //printf("resume: dma not active!! state %d\n",channelInfo->channelStatus);
   }
}


ERRG_codeE CDE_MNGRG_pauseChannel(CDE_MNGRG_channnelInfoT* channelInfo)
{
   ERRG_codeE ret = CDE_MNGR__RET_SUCCESS;
   CDE_DRVG_channelHandleT h;// = channelInfo->dmaChannelHandle;
   if( channelInfo->channelStatus == CDE_MNGRG_CHAN_STATUS_RUNNING_E)
   {
      h = channelInfo->dmaChannelHandle;
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      //printf("pause: %d/%d h %p\n",h->coreNum,h->hwChannelNum,h);
#else
      //printf("pause: %d/%d h %p\n",h->coreNum,h->channelNum,h);
#endif
      h->oldIndex = 0;
      channelInfo->channelStatus = CDE_MNGRG_CHAN_STATUS_CONFIGURED_E;
      CDE_DRVG_stopChannel( h );
   }
   else
   {
      //printf("pause: dma not active!! state %d\n",channelInfo->channelStatus);
   }
}


ERRG_codeE CDE_MNGRG_stopChannel(CDE_MNGRG_channnelInfoT* channelInfo)
{
   ERRG_codeE ret = CDE_MNGR__RET_SUCCESS;
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
   int i = 0;
#endif
   CDE_DRVG_channelHandleT h;// = channelInfo->dmaChannelHandle;
   if( channelInfo->channelStatus == CDE_MNGRG_CHAN_STATUS_RUNNING_E)
   {
      //If Extended DMA Interleaving is used, stop sub channel(s) first
      if (channelInfo->extInfo.useExtIntMode)
      {
         for (UINT32 i = 0; i < channelInfo->extInfo.subChannels; i++)
         {
            h = channelInfo->extInfo.subChannelsInfo[i]->dmaChannelHandle;
            CDE_DRVG_stopChannel(h);
         }
      }
      h = channelInfo->metadataDMAChannelHandle;
      if(h)
         CDE_DRVG_stopChannel( h );
      h = channelInfo->dmaChannelHandle;
      if(h)
         CDE_DRVG_stopChannel( h );
      channelInfo->channelStatus = CDE_MNGRG_CHAN_STATUS_OPENED_E;
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      LOGG_PRINT(LOG_INFO_E, NULL, "Stopped DMA channel on core %d/channel %d \n", h->coreNum, h->hwChannelNum);
#else
      LOGG_PRINT(LOG_INFO_E, NULL, "Stopped DMA channel on core %d/channel %d \n", h->coreNum, h->channelNum);
#endif
      if( (h->srcParams.periphNum == 14) && (h->scenarioType == CDE_DRVG_SCENARIO_TYPE_IIC_STREAM_DDR_LESS) )
      {

      }
      else
      {
         if( channelInfo->memPoolHandle )
         {
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
            //clean up:
            //1. remove drain buffer, and also remove it from the list
            if (channelInfo->dmaChannelHandle->drainingBuffDesc)
            {
               MEM_POOLG_free(channelInfo->dmaChannelHandle->drainingBuffDesc);
               while(i < MAX_NUM_BUFFERS_LOOPS )
               {
                  if (channelInfo->dmaChannelHandle->buffDescListP[i] == channelInfo->dmaChannelHandle->drainingBuffDesc)
                  {
                     channelInfo->dmaChannelHandle->buffDescListP[i] = NULL;
                  }
                  i++;
               }
            }
            channelInfo->dmaChannelHandle->drainingBuffDesc=NULL;

            //2. remove the rest of the buffers
            i = 0;
            while(i < MAX_NUM_BUFFERS_LOOPS )
            {
               if (channelInfo->dmaChannelHandle->buffDescListP[i])
               {
                  MEM_POOLG_free(channelInfo->dmaChannelHandle->buffDescListP[i]);
                  channelInfo->dmaChannelHandle->buffDescListP[i] = NULL;
               }
               i++;
            }
#else
            if ((channelInfo->dmaChannelHandle->curBuffDesc) && (channelInfo->dmaChannelHandle->curBuffDesc != channelInfo->dmaChannelHandle->drainingBuffDesc))
            {
               MEM_POOLG_free(channelInfo->dmaChannelHandle->curBuffDesc);
               channelInfo->dmaChannelHandle->curBuffDesc=NULL;
            }
            if ((channelInfo->dmaChannelHandle->nextBuffDesc) && (channelInfo->dmaChannelHandle->nextBuffDesc != channelInfo->dmaChannelHandle->drainingBuffDesc))
            {
               MEM_POOLG_free(channelInfo->dmaChannelHandle->nextBuffDesc);
               channelInfo->dmaChannelHandle->nextBuffDesc=NULL;
            }
            if (channelInfo->dmaChannelHandle->drainingBuffDesc)
            {
               MEM_POOLG_free(channelInfo->dmaChannelHandle->drainingBuffDesc);
               channelInfo->dmaChannelHandle->drainingBuffDesc=NULL;
            }
#endif
         }
         ret = CDE_MNGRP_waitOutChBufTblFree(channelInfo);
         if(ERRG_FAILED(ret))
         {
            //return ret;
         }
      }
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "FAILED to stop DMA channel. ChannelInfo %p Channel status %d \n", channelInfo, channelInfo->channelStatus);
     ret = CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                        // TODO: make a new error code
   }
   return ret;
}



ERRG_codeE CDE_MNGRG_injectBuffer( CDE_MNGRG_channnelInfoT* channelInfo, UINT8 *injectBuff)
{
   ERRG_codeE ret = CDE_MNGR__RET_SUCCESS;
   CDE_DRVG_channelHandleT h = channelInfo->dmaChannelHandle;

   if( channelInfo->channelStatus == CDE_MNGRG_CHAN_STATUS_CONFIGURED_E )
   {
      //printf("CDE_MNGRG_injectBuffer imgStartY %d buffWidth %d stride %d imgBpp %d addidng %d\n",channelInfo->dmaChannelHandle->srcParams.imgStartY, channelInfo->dmaChannelHandle->srcParams.buffWidth,channelInfo->dmaChannelHandle->srcParams.stride,channelInfo->dmaChannelHandle->srcParams.imgBpp,channelInfo->dmaChannelHandle->srcParams.imgStartY * channelInfo->dmaChannelHandle->srcParams.stride * channelInfo->dmaChannelHandle->srcParams.imgBpp);
      //this line does the vertical crop by DMA while startY is not equal to zero, actually adding offset to inject buffer
      injectBuff += channelInfo->dmaChannelHandle->srcParams.imgStartY * channelInfo->dmaChannelHandle->srcParams.stride * channelInfo->dmaChannelHandle->srcParams.imgBpp;
      if (channelInfo->dmaChannelHandle->srcParams.imgStartX != 0)
      {
         //this line does the jumping to taken data from left/right side of interleaving src image
         injectBuff += (channelInfo->dmaChannelHandle->srcParams.imgStartX * channelInfo->dmaChannelHandle->srcParams.imgBpp);
      }
      CDE_DRVG_updateMemToPeriphsrc(h, injectBuff);
      ret = CDE_DRVG_startChannel( h );
     // LOGG_PRINT(LOG_INFO_E, NULL, "Started DMA channel on core %d/channel %d \n", h->coreNum, h->channelNum);
      return ret;
   }
   else
   {
      if( channelInfo->channelStatus == CDE_MNGRG_CHAN_STATUS_RUNNING_E )
      {
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
         LOGG_PRINT(LOG_INFO_E, NULL, "Already started DMA channel on core %d/channel %d \n", h->coreNum, h->hwChannelNum);
#else
         LOGG_PRINT(LOG_INFO_E, NULL, "Already started DMA channel on core %d/channel %d \n", h->coreNum, h->channelNum);
#endif

        return(CDE_MNGR__RET_SUCCESS);
      }
      else
      {
        return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                   // TODO: make a new error code
      }
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: CDE_MNGRG_setupChannel
*
*  Description:
*
*  Inputs:        configuration, cbInfo
*
*  Outputs:       channelInfo
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE CDE_MNGRG_setupChannel(CDE_MNGRG_channnelInfoT* channelInfo, CDE_DRVG_channelCfgT* configuration, CDE_MNGRG_chanCbInfoT* cbInfo )
{

   ERRG_codeE ret = CDE_MNGR__RET_SUCCESS;
   LOGG_PRINT(LOG_INFO_E,NULL,"Setting up channel for scenario type %u \n",configuration->streamCfg.scenarioType);
   switch( configuration->streamCfg.scenarioType )
   {

      case CDE_DRVG_SCENARIO_REGISTER_TO_MEMORY_E:
         ret = CDE_MNGRP_setupScenarioRegistersToMemory(channelInfo,configuration,cbInfo);
      break;
      case CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_E:
      case CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_EXT_E:
         ret = CDE_MNGRP_setupScenarioStreamFixedSize( channelInfo, configuration, cbInfo );
      break;
      case CDE_DRVG_SCENARIO_TYPE_INJECTION_FIXEDSIZE_E:
         ret = CDE_MNGRP_setupScenarioInjectionFixedSize( channelInfo, configuration, cbInfo );
      break;
      case CDE_DRVG_SCENARIO_TYPE_MEMCOPY_PLAIN2PLAIN_E:
         ret = CDE_MNGRP_setupScenarioMemcopy( channelInfo, configuration, cbInfo );
      break;
      case CDE_DRVG_SCENARIO_TYPE_STREAM_CYCLIC_E:
         ret = CDE_MNGRP_setupScenarioStreamCyclic( channelInfo, configuration, cbInfo );
      break;
      case CDE_DRVG_SCENARIO_TYPE_IIC_STREAM_DDR_LESS:
         ret = CDE_MNGRP_setupScenarioDdrless( channelInfo, configuration, cbInfo );
        break;
      default:
         ret = CDE__ERR_INIT_FAIL_MNGR_FAIL;
      break;
   }

   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "CDE manager failed to setup channel!\n");
      assert(0);
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: CDE_MNGRG_channelIntCallback
*
*  Description:   Dispatch Callbacks registered in INT_CTRL and
*           send messege to the context that invokes user callbacks on this channel
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: INT_CTRLP_thread
*
****************************************************************************/
#ifndef CDE_DRVG_IMPROVED_ISR_HANDLING
ERRG_codeE CDE_MNGRG_channelIntCallback( void *managerInfo, CDE_DRVG_InterruptTypeE intType,  UINT64 timeStamp )
{
   ERRG_codeE                 ret = CDE_MNGR__RET_SUCCESS;
   CDE_MNGRG_channnelInfoT*   chanInfo    = (CDE_MNGRG_channnelInfoT*)managerInfo;
   CDE_DRVG_channelHandleT    chanHandle = chanInfo->dmaChannelHandle;

   // TODO: Complete parameters to INT_MNGRG_intCbDispatch
   //First dispatch registred callbacks

   if (intType == CDE_DRVG_INTERRUPT_TYPE_FRAME_DONE_E)
   {
      CDE_MNGRG_invokeFrameDoneUserCallbacks( managerInfo,
                                              timeStamp,
                                              chanHandle->frameDoneCounter,
                                              chanHandle->doneBuffDesc);
   }
   else
   {
      CDE_MNGRG_invokeChannelDoneUserCallbacks( managerInfo,  timeStamp );
   }

   return ret;
}
#endif

/****************************************************************************
*
*  Function Name: CDE_MNGRG_perphToDdrIntCallback
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: INT_CTRLP_thread
*
****************************************************************************/
static void CDE_MNGRG_perphToDdrIntCallback( void *managerInfo, CDE_DRVG_InterruptTypeE intType, UINT64 timeStamp )
{
   CDE_MNGRG_channnelInfoT*   chanInfo    = (CDE_MNGRG_channnelInfoT*)managerInfo;
   CDE_DRVG_channelHandleT    chanHandle = chanInfo->dmaChannelHandle;
   CDE_DRVG_channelHandleT    metadatachanHandle = chanInfo->metadataDMAChannelHandle;
   CDE_MNGRG_userCbParamsT    cbParams;
   int                        i = 0;
   MEM_POOLG_bufDescT        *bufDescP = NULL;
   MEM_POOLG_bufDescT        *buffDescDoneListP = NULL;
   UINT32                     numIsr = 0;
   CDE_DRVG_extIntParams drvExtIntParams;

   if (chanInfo->extInfo.useExtIntMode)
   {
      drvExtIntParams.numSubChannels = chanInfo->extInfo.subChannels;
      for (UINT32 i = 0; i < chanInfo->extInfo.subChannels; i++)
      {
         drvExtIntParams.subChannelHandles[i] = chanInfo->extInfo.subChannelsInfo[i]->dmaChannelHandle;
      }
      //retreive the list of buffers
      CDE_SCENARIOSG_setNewDest(chanHandle,metadatachanHandle, &buffDescDoneListP, &numIsr, &drvExtIntParams);
   }
   else
   {
      //retreive the list of buffers
      CDE_SCENARIOSG_setNewDest(chanHandle,metadatachanHandle, &buffDescDoneListP, &numIsr, NULL);
   }
   if (intType == CDE_DRVG_INTERRUPT_TYPE_FRAME_DONE_E)
   {
      while(buffDescDoneListP)
      {
         //increment for every buffer DMA copied
         if (((chanHandle->chunkNum) % chanHandle->chunksNumPerFrame) == 0)
         {
            chanHandle->frameDoneCounter++;
            chanHandle->chunkNum = 0;
         }
         bufDescP = buffDescDoneListP;
         buffDescDoneListP = buffDescDoneListP->nextP;
         bufDescP->nextP = NULL; //disconnect the chain. This point we need to optimize, and send a list of buffers into the upper layer

#ifdef DEBUG_IMP_ISR_HANDLE
         printf("core %d, ch %d, chunkNum = %d, chunksNumPerFrame = %d,buffer %p, drain? %d, ts = %llu\n",chanHandle->coreNum,chanHandle->channelNum,
                 chanHandle->chunkNum,chanHandle->chunksNumPerFrame,bufDescP,bufDescP == chanHandle->drainingBuffDesc,timeStamp);
#endif
         //for drain, do not pass any buffer
         if (bufDescP != chanHandle->drainingBuffDesc)
         {
            bufDescP->dataLen = chanHandle->dstParams.buffSize;
            cbParams.buffDescriptorP   = bufDescP;
            cbParams.currVirtPtr       = bufDescP->dataP;
         }
         else
         {
            cbParams.buffDescriptorP   = NULL;
            cbParams.currVirtPtr       = NULL;
         }
         cbParams.dmaChanId         = chanInfo->systemChanId;
         cbParams.systemFrameCntr   = chanHandle->frameDoneCounter;
         cbParams.timeStamp         = timeStamp;
         cbParams.channelInfo       = managerInfo;
         cbParams.chunkNum          = chanHandle->chunkNum;
         for( i = 0; i < CDE_MNGRG_NUM_CHANNEL_CB; i++ )
         {
            if(chanInfo->callBacksInfo[i].frameDoneCb.cb)
            {
               MEM_POOLG_duplicate(bufDescP);
               chanInfo->callBacksInfo[i].frameDoneCb.cb(&cbParams, chanInfo->callBacksInfo[i].frameDoneCb.arg);
            }
         }
         MEM_POOLG_free(bufDescP);
         chanHandle->chunkNum++;
      }
   }
   else
   {
      CDE_MNGRG_invokeChannelDoneUserCallbacks( managerInfo,  timeStamp );
   }
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   HCG_MNGRG_processEvent(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH0_E + (chanHandle->coreNum * 8) + chanHandle->hwChannelNum, timeStamp, numIsr);
#else
   HCG_MNGRG_processEvent(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH0_E + (chanHandle->coreNum * 8) + chanHandle->channelNum, timeStamp, numIsr);
#endif

}


/****************************************************************************
*
*  Function Name: CDE_MNGRG_perphToDdrlessIntCallback
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: INT_CTRLP_thread
*
****************************************************************************/
static void CDE_MNGRG_perphToDdrlessIntCallback( void *managerInfo, CDE_DRVG_InterruptTypeE intType, UINT64 timeStamp )
{
   CDE_MNGRG_channnelInfoT*   chanInfo    = (CDE_MNGRG_channnelInfoT*)managerInfo;
   CDE_DRVG_channelHandleT    chanHandle = chanInfo->dmaChannelHandle;
   CDE_MNGRG_userCbParamsT    cbParams;
   UINT32                     i;

   (void)intType;
   chanHandle->frameDoneCounter++;
   cbParams.systemFrameCntr      = chanHandle->frameDoneCounter;
   cbParams.timeStamp            = timeStamp;
   cbParams.channelInfo          = managerInfo;

   for( i = 0; i < CDE_MNGRG_NUM_CHANNEL_CB; i++ )
   {
      if(chanInfo->callBacksInfo[i].frameDoneCb.cb)
      {
         chanInfo->callBacksInfo[i].frameDoneCb.cb(&cbParams, chanInfo->callBacksInfo[i].frameDoneCb.arg);
      }
   }
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   HCG_MNGRG_processEvent(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH0_E + (chanHandle->coreNum * 8) + chanHandle->hwChannelNum, timeStamp, 1);
#else
   HCG_MNGRG_processEvent(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH0_E + (chanHandle->coreNum * 8) + chanHandle->channelNum, timeStamp, 1);
#endif

}



/****************************************************************************
*
*  Function Name: CDE_MNGRG_memoryToPerphIntCallback
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: INT_CTRLP_thread
*
****************************************************************************/
static void CDE_MNGRG_memoryToPerphIntCallback( void *managerInfo, CDE_DRVG_InterruptTypeE intType, UINT64 timeStamp )
{
   CDE_MNGRG_channnelInfoT*   chanInfo    = (CDE_MNGRG_channnelInfoT*)managerInfo;
   CDE_MNGRG_userCbParamsT    cbParams;
   UINT32                     i;

   (void)intType;
   cbParams.timeStamp = timeStamp;
   for( i = 0; i < CDE_MNGRG_NUM_CHANNEL_CB; i++ )
   {
      if(chanInfo->callBacksInfo[i].frameDoneCb.cb)
      {
         chanInfo->callBacksInfo[i].frameDoneCb.cb(&cbParams, chanInfo->callBacksInfo[i].frameDoneCb.arg);
      }
   }

   CDE_DRVG_stopChannel(chanInfo->dmaChannelHandle);
   //printf("kill command core %d ch %d at %llu timeStamp %llu\n",core,ch,nsec,timeStamp);
}


#ifndef CDE_DRVG_IMPROVED_ISR_HANDLING
/****************************************************************************
*
*  Function Name: CDE_MNGRG_invokeFrameDoneUserCallbacks
*
*  Description:   Dispatch Frame Done User Callbacks registered in CDE_MNGR
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: INT_MNGR_thread
*
****************************************************************************/
ERRG_codeE CDE_MNGRG_invokeFrameDoneUserCallbacks( void *managerInfo,   UINT64 timeStamp ,UINT32 frameDoneCounter, void *bufDescP)
{
   ERRG_codeE                 ret=CDE_MNGR__RET_SUCCESS;

   CDE_MNGRG_channnelInfoT*   chanInfo = (CDE_MNGRG_channnelInfoT*)managerInfo;
   CDE_DRVG_channelHandleT    chanHandle = chanInfo->dmaChannelHandle;
   MEM_POOLG_bufDescT         *buffDescriptorP = (MEM_POOLG_bufDescT*)bufDescP;
   CDE_MNGRG_userCbParamsT    cbParams;
   UINT32                  i = 0;
   UINT32                  phyAddress = 0;

   if( (chanHandle->srcParams.periphNum == 14) && (chanHandle->scenarioType == CDE_DRVG_SCENARIO_TYPE_IIC_STREAM_DDR_LESS) )
   {
      cbParams.systemFrameCntr      = frameDoneCounter;
      cbParams.timeStamp            = timeStamp;
      cbParams.channelInfo          = managerInfo;

      for( i = 0; i < CDE_MNGRG_NUM_CHANNEL_CB; i++ )
      {
         if(chanInfo->callBacksInfo[i].frameDoneCb.cb)
         {
            chanInfo->callBacksInfo[i].frameDoneCb.cb(&cbParams, chanInfo->callBacksInfo[i].frameDoneCb.arg);
         }
      }
   }
   else if(buffDescriptorP)
   {
      buffDescriptorP->dataLen = chanHandle->dstParams.buffSize;

      ret = MEM_POOLG_getDataPhyAddr(buffDescriptorP,&phyAddress);
      if (ERRG_SUCCEEDED(ret))
      {
         cbParams.buffDescriptorP   = buffDescriptorP;
         cbParams.currPhysAddr      = phyAddress;
         cbParams.dmaChanId         = chanInfo->systemChanId;
         cbParams.systemFrameCntr   = frameDoneCounter;
         cbParams.timeStamp         = timeStamp;
         cbParams.currVirtPtr       = buffDescriptorP->dataP;
         cbParams.channelInfo       = managerInfo;
         cbParams.chunkNum          = chanHandle->chunkNum;

         for( i = 0; i < CDE_MNGRG_NUM_CHANNEL_CB; i++ )
         {
            if(chanInfo->callBacksInfo[i].frameDoneCb.cb)
            {
               MEM_POOLG_duplicate(buffDescriptorP);
               chanInfo->callBacksInfo[i].frameDoneCb.cb(&cbParams, chanInfo->callBacksInfo[i].frameDoneCb.arg);
            }
         }
         MEM_POOLG_free(buffDescriptorP);
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, ret, "error getting phyaddress for %p\n",buffDescriptorP->dataP);
         assert(0);
      }
   }
   else
   {
        if (chanInfo->systemChanId == INU_DEFSG_SYS_CH_8_INJECT_ID_E)
        {
            if (chanInfo->callBacksInfo[0].frameDoneCb.cb)
            {
               chanInfo->callBacksInfo[0].frameDoneCb.cb(&cbParams, chanInfo->callBacksInfo[0].frameDoneCb.arg);
            }
        }
     else
     {
         ret = CDE_MNGR__ERR_OUT_OF_RSRCS;
     }
   }
   return ret;

}
#endif

/****************************************************************************
*
*  Function Name: CDE_MNGRG_invokeChannelDoneUserCallbacks
*
*  Description:   Dispatch Channel Done User Callbacks registered in CDE_MNGR
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: INT_CTRLP_thread
*
****************************************************************************/
ERRG_codeE CDE_MNGRG_invokeChannelDoneUserCallbacks( void *managerInfo, UINT64 timeStamp )
{
   CDE_MNGRG_channnelInfoT*   chanInfo = (CDE_MNGRG_channnelInfoT*)managerInfo;
   UINT8                   i = 0;
   CDE_MNGRG_userCbParamsT    cbParams;
   CDE_DRVG_channelHandleT    h = chanInfo->dmaChannelHandle;
   (void)timeStamp;
   cbParams.channelInfo     = chanInfo;
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
   cbParams.buffDescriptorP = h->buffDescListP[0];
#else
   cbParams.buffDescriptorP = h->doneBuffDesc;
#endif
   for( i = 0; i < CDE_MNGRG_NUM_CHANNEL_CB; i++ )
   {
      if(chanInfo->callBacksInfo[i].channelDoneCb.cb)
      {
         chanInfo->callBacksInfo[i].channelDoneCb.cb(&cbParams, chanInfo->callBacksInfo[i].channelDoneCb.arg);
      }
   }
   return CDE_MNGR__RET_SUCCESS;
}
/****************************************************************************
*
*  Function Name: CDE_MNGRG_registerUserMetadataCallBacks
*
*  Description:   Register all types of user interrupts for metadata. 
*
*  Inputs:        channelHandle
*
*  Outputs:       channelInfo
*
*  Returns:    
*
*  Context: 
*
****************************************************************************/

//1 NOTE: The first "frame done" or "channel done" callback have to be registered BEFORE channel state = configured
ERRG_codeE CDE_MNGRG_registerUserMetadataCallBacks( CDE_MNGRG_channnelInfoT* channelInfo, CDE_MNGRG_chanCbInfoT* cbInfo )
{
   UINT8 i = 0;

   CDE_DRVG_channelHandleT channelHandle = channelInfo->metadataDMAChannelHandle;
   
   if( cbInfo->channelDoneCb.cb )
   {
      for( i = 0; i < CDE_MNGRG_NUM_CHANNEL_CB; i++ )
      {
         if( channelInfo->metadataCallBacksInfo[i].channelDoneCb.cb == NULL )
         {
            if( (i == 0) && (channelInfo->metadataChannelStatus >= CDE_MNGRG_CHAN_STATUS_CONFIGURED_E) )   
            {
               return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                                    // TODO: make a new error code
            }
            channelInfo->metadataCallBacksInfo[i].channelDoneCb.cb  = cbInfo->channelDoneCb.cb;      
            channelInfo->metadataCallBacksInfo[i].channelDoneCb.arg = cbInfo->channelDoneCb.arg;

            channelHandle->channelDoneIntEnabled = 1;
            
            break;
         }
      }
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "channelDoneCb not provided \n");
   }

   if( cbInfo->frameDoneCb.cb )
   {
      for( i = 0; i < CDE_MNGRG_NUM_CHANNEL_CB; i++ )
      {
         if( channelInfo->metadataCallBacksInfo[i].frameDoneCb.cb == NULL )
         {
            if( (i == 0) && (channelInfo->metadataChannelStatus >= CDE_MNGRG_CHAN_STATUS_CONFIGURED_E) )   
            {
               return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                                    // TODO: make a new error code
            }
            channelInfo->metadataCallBacksInfo[i].frameDoneCb.cb= cbInfo->frameDoneCb.cb;   
            channelInfo->metadataCallBacksInfo[i].frameDoneCb.arg = cbInfo->frameDoneCb.arg;
            
            channelHandle->frameDoneIntEnabled = 1;            
            
            break;
         }
      }
   }  
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "frameDoneCb not provided \n");
   }

   return CDE_MNGR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: CDE_MNGRG_registerUserCallBacks
*
*  Description:   Register all types of user interrupts.
*
*  Inputs:        channelHandle
*
*  Outputs:       channelInfo
*
*  Returns:
*
*  Context:
*
****************************************************************************/

//1 NOTE: The first "frame done" or "channel done" callback have to be registered BEFORE channel state = configured

ERRG_codeE CDE_MNGRG_registerUserCallBacks( CDE_MNGRG_channnelInfoT* channelInfo, CDE_MNGRG_chanCbInfoT* cbInfo )
{
   UINT8 i = 0;

   CDE_DRVG_channelHandleT channelHandle = channelInfo->dmaChannelHandle;

   if( cbInfo->channelDoneCb.cb )
   {
      for( i = 0; i < CDE_MNGRG_NUM_CHANNEL_CB; i++ )
      {
         if( channelInfo->callBacksInfo[i].channelDoneCb.cb == NULL )
         {
            if( (i == 0) && (channelInfo->channelStatus >= CDE_MNGRG_CHAN_STATUS_CONFIGURED_E) )
            {
               return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                                    // TODO: make a new error code
            }
            channelInfo->callBacksInfo[i].channelDoneCb.cb  = cbInfo->channelDoneCb.cb;
            channelInfo->callBacksInfo[i].channelDoneCb.arg = cbInfo->channelDoneCb.arg;

            channelHandle->channelDoneIntEnabled = 1;

            break;
         }
      }
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "channelDoneCb not provided \n");
   }

   if( cbInfo->frameDoneCb.cb )
   {
      for( i = 0; i < CDE_MNGRG_NUM_CHANNEL_CB; i++ )
      {
         if( channelInfo->callBacksInfo[i].frameDoneCb.cb == NULL )
         {
            if( (i == 0) && (channelInfo->channelStatus >= CDE_MNGRG_CHAN_STATUS_CONFIGURED_E) )
            {
               return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                                    // TODO: make a new error code
            }
            channelInfo->callBacksInfo[i].frameDoneCb.cb= cbInfo->frameDoneCb.cb;
            channelInfo->callBacksInfo[i].frameDoneCb.arg = cbInfo->frameDoneCb.arg;

            channelHandle->frameDoneIntEnabled = 1;

            break;
         }
      }
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "frameDoneCb not provided \n");
   }

   return CDE_MNGR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: CDE_MNGRG_registerUserCallBacks
*
*  Description:   Register all types of user interrupts.
*
*  Inputs:        channelHandle
*
*  Outputs:       channelInfo
*
*  Returns:
*
*  Context:
*
****************************************************************************/


ERRG_codeE CDE_MNGRG_setSysId(CDE_MNGRG_channnelInfoT* channelInfo, INU_DEFSG_sysChannelIdE sysId )
{
   channelInfo->systemChanId = sysId;

   return CDE_MNGR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: CDE_MNGRP_showConfig
*
*  Description:CDE print configuration.
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

ERRG_codeE CDE_MNGRG_showConfig(void)
{
   ERRG_codeE           ret = CDE__RET_SUCCESS;
   CDE_DRVG_channelHandleT h;
   UINT32               i = 0;

   LOGG_PRINT(LOG_INFO_E, NULL, "-------------------\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "----Show CDE congifuration:  \n");

   for( i = 0; i < CDE_MNGRG_MAX_AVAILABLE_CHAN; i++ )
   {
      h = dmaInfoDB->chanInfo[i].dmaChannelHandle;
      if(h)
      {
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
         LOGG_PRINT(LOG_INFO_E, NULL, "\t Info %02d:\t systemChanId = 0x%x, status = %d, coreNum = %d, channelNum = %d, SourcePeriph = %d, Buff Size = %d, drain count = %d\n",
         i, dmaInfoDB->chanInfo[i].systemChanId, dmaInfoDB->chanInfo[i].channelStatus, h->coreNum,  h->hwChannelNum, h->srcParams.periphNum, h->dstParams.buffSize, h->dstParams.drainCtr);

#else
         LOGG_PRINT(LOG_INFO_E, NULL, "\t Info %02d:\t systemChanId = 0x%x, status = %d, coreNum = %d, channelNum = %d, SourcePeriph = %d, Buff Size = %d, drain count = %d\n",
         i, dmaInfoDB->chanInfo[i].systemChanId, dmaInfoDB->chanInfo[i].channelStatus, h->coreNum,  h->channelNum, h->srcParams.periphNum, h->dstParams.buffSize, h->dstParams.drainCtr);

#endif
      }
   }
   LOGG_PRINT(LOG_INFO_E, NULL, "-------------------\n");
   return ret;
}


/****************************************************************************
*
*  Function Name: CDE_MNGRG_isPeriphInUse
*
*  Description: Lookup for peripheral. If already in use, return appropriate ChannelInfo, else return NULL in ChannelInfo
*
*  Inputs: periph, portDir
*
*  Outputs: channelInfo
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE CDE_MNGRG_isPeriphInUse(CDE_MNGRG_channnelInfoT** channelInfo, CDE_DRVG_PeriphClientIdE periph, CDE_MNGRG_portDirE portDir  )
{

   UINT8                   i = 0;
   CDE_DRVG_channelHandleT     h;
   CDE_DRVG_portParamsT    *portP;
   CDE_DRVG_PeriphClientIdU    p;

   p.periphId = periph;
   for( i = 0; i < CDE_MNGRG_MAX_AVAILABLE_CHAN; i++ )
   {
      h = dmaInfoDB->chanInfo[i].dmaChannelHandle;
      if( h )
      {
         portP = (portDir == CDE_MNGRG_PORT_SOURCE_E) ? (&h->srcParams) : (&h->dstParams);
         if( (portP->periphNum == p.field.periphPort) && ((h->coreNum == p.field.core) || (p.field.core == CDE_DRVG_CORE_NUM_ANY_E)))
         {
            *channelInfo = &dmaInfoDB->chanInfo[i];

            LOGG_PRINT(LOG_DEBUG_E, NULL, "--- Found periph %d on chanInfo %p \n", p.field.periphPort, channelInfo );
            return CDE__RET_SUCCESS;
         }
      }
   }

   return CDE__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: CDE_MNGRG_memcpy
*
*  Description:
*
*  Inputs:
*
*  Outputs: None
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE CDE_MNGRG_memcpyOpenChan( CDE_MNGRG_channnelInfoT **channelInfo, UINT32 size, CDE_MNGRG_chCbT doneCb, void *cbArgP)
{
   ERRG_codeE                 ret = HW_MNGR__RET_SUCCESS;
   CDE_DRVG_channelCfgT       copierCfg;
   CDE_MNGRG_chanCbInfoT      copierCbCfg;

   copierCfg.streamCfg.scenarioType = CDE_DRVG_SCENARIO_TYPE_MEMCOPY_PLAIN2PLAIN_E;
   copierCfg.streamCfg.opMode       = CDE_DRVG_OME_SHOT_OP_MODE_E;
   copierCfg.streamCfg.numOfBuffs   = 1;
   copierCfg.srcCfg.peripheral      = CDE_DRVG_PERIPH_CLIENT_MEMORY_TX_E;
   copierCfg.dstCfg.peripheral      = CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E;
   copierCfg.memcpyCfg.buffSize     = size;
   copierCfg.memcpyCfg.srcAddress   = 0x81700000; //place holders, real addresses will be set when performing the copy
   copierCfg.memcpyCfg.dstAddress   = 0x81700000; //place holders, real addresses will be set when performing the copy
   memset(&copierCbCfg,0,sizeof(copierCbCfg));
   copierCbCfg.channelDoneCb.cb     = doneCb;
   copierCbCfg.channelDoneCb.arg    = cbArgP;

   ret = CDE_MNGRG_openChannel( channelInfo, CDE_DRVG_PERIPH_CLIENT_MEMORY_TX_E, CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E );
   if(ERRG_FAILED(ret))
   {
      return ret;
   }

   HCG_MNGRG_voteUnit( HCG_MNGRG_DMA0 + (*channelInfo)->dmaChannelHandle->coreNum );
   ret = CDE_MNGRG_setupChannel( *channelInfo, &copierCfg, &copierCbCfg );
   HCG_MNGRG_devoteUnit( HCG_MNGRG_DMA0 + (*channelInfo)->dmaChannelHandle->coreNum );
   (*channelInfo)->systemChanId = INU_DEFSG_SYS_CH_15_I2S_ID_E; //prevent timestamp overwrite to be zero'ed
   (*channelInfo)->memPoolHandle = NULL; //
   return ret;
}
/****************************************************************************
*
*  Function Name: CDE_MNGRG_metadataOpenChan
*
*  Description: 
*
*  Inputs: 
*
*  Outputs: None
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE CDE_MNGRG_metadataOpenChan( CDE_MNGRG_channnelInfoT **channelInfo,CDE_DRVG_regToMemoryCopyConfigParams *configParams) 
{
   ERRG_codeE                 ret = HW_MNGR__RET_SUCCESS;
   CDE_DRVG_channelCfgT       copierCfg;  
   CDE_MNGRG_chanCbInfoT      copierCbCfg;

   copierCfg.streamCfg.scenarioType = CDE_DRVG_SCENARIO_REGISTER_TO_MEMORY_E;
   copierCfg.streamCfg.opMode       = CDE_DRVG_CONTINIOUS_OP_MODE_E;
   copierCfg.streamCfg.numOfBuffs   = 1;
   copierCfg.srcCfg.peripheral      = CDE_DRVG_PERIPH_CLIENT_MEMORY_TX_E;
   copierCfg.dstCfg.peripheral      = CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E;
   copierCfg.memcpyCfg.buffSize     = 0;
   copierCfg.memcpyCfg.srcAddress   = 0x81700000; //place holders, real addresses will be set when performing the copy
   copierCfg.memcpyCfg.dstAddress   = 0x81700000; //place holders, real addresses will be set when performing the copy
   memcpy(&copierCfg.regToMemoryCopyParams,configParams,sizeof(CDE_DRVG_regToMemoryCopyConfigParams)); /*copy meta copy config */
   memset(&copierCbCfg,0,sizeof(copierCbCfg));
   copierCbCfg.channelDoneCb.cb     = NULL;
   copierCbCfg.channelDoneCb.arg    = NULL;

   const UINT8 desiredCore = (*channelInfo)->dmaChannelHandle->coreNum; /*Use the same channel as the regular DMA channel */
   LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Using core %u for metadata on metadata dma channel handle 0x%x \n", desiredCore,(*channelInfo)->metadataDMAChannelHandle);

   CDE_DRVG_channelHandleT handle;

   ret = CDE_DRVG_openChannelFixedCoreNum(&handle,desiredCore,CDE_DRVG_PERIPH_CLIENT_MEMORY_TX_E,CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E);
   (*channelInfo)->metadataDMAChannelHandle = handle;
   if(ERRG_FAILED(ret))                                     
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to open metadata channel \n", desiredCore);
      return ret;
   }
   ret = CDE_MNGRG_setupChannel( *channelInfo, &copierCfg, &copierCbCfg );
   LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Starting metadata channel on metadata dma channel handle 0x%x \n",handle);
   CDE_DRVG_startChannel(handle);
   LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Registering metadata channel on metadata dma channel handle 0x%x \n", handle);
   CDE_MNGRP_registerOpenedMetadataChannel(channelInfo,handle);
   //(*channelInfo)->metadataDMAChannelHandle = handle;
   LOGG_PRINT(METADATA_DEBUG_LEVEL,NULL,"Metadata dma channel handle assigned to 0x%x \n", (*channelInfo)->metadataDMAChannelHandle);
   return ret;
}
/****************************************************************************
*
*  Function Name: CDE_MNGRG_metadataCloseChan
*
*  Description: 
*
*  Inputs: 
*
*  Outputs: None
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE CDE_MNGRG_metadataCloseChan( CDE_MNGRG_channnelInfoT *channelInfo ) 
{
   ERRG_codeE                 ret = HW_MNGR__RET_SUCCESS;

   ret = CDE_DRVG_stopChannel(channelInfo->metadataDMAChannelHandle);
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "SEQ_MNGRP_meatadataChannelDoneCb failed to stop channel\n");
   }
   else
   {
      ret = CDE_MNGRG_closeChannel(channelInfo);
      if(ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "SEQ_MNGRP_metadataChannelDoneCb failed to close channel\n");
      }
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: CDE_MNGRG_memcpy
*
*  Description:
*
*  Inputs:
*
*  Outputs: None
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE CDE_MNGRG_memcpyCloseChan( CDE_MNGRG_channnelInfoT *channelInfo )
{
   ERRG_codeE                 ret = HW_MNGR__RET_SUCCESS;

   ret = CDE_DRVG_stopChannel(channelInfo->dmaChannelHandle);
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "SEQ_MNGRP_memcpyChannelDoneCb failed to stop channel\n");
   }
   else
   {
      ret = CDE_MNGRG_closeChannel(channelInfo);
      if(ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "SEQ_MNGRP_memcpyChannelDoneCb failed to close channel\n");
      }
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: CDE_MNGRG_memcpy
*
*  Description:
*
*  Inputs:
*
*  Outputs: None
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE CDE_MNGRG_memcpyBlock( CDE_MNGRG_channnelInfoT *channelInfo, UINT32 dst, UINT32 src, UINT32 size )
{
   ERRG_codeE                 ret = HW_MNGR__RET_SUCCESS;
   UINT32                     totalSleep = 0;
   CDE_DRVG_channelHandleT    h;

   HCG_MNGRG_voteUnit( HCG_MNGRG_DMA0 + channelInfo->dmaChannelHandle->coreNum );
   h = channelInfo->dmaChannelHandle;
   if (size != channelInfo->dmaChannelHandle->dstParams.buffSize)
   {
      ret = CDE_DRVG_updateMemToMem(h, dst, src, size);
   }
   else
   {
      CDE_DRVG_updateMemToMemAddress(h, dst, src);
   }
   if (ERRG_SUCCEEDED(ret))
   {
      ret = CDE_MNGRG_startChannel(channelInfo);
      if(ERRG_SUCCEEDED(ret))
      {
         while(CDE_DRVG_getChannelStatus( h ) != CDE_DRVG_STATUS_STOPPED_E )
         {
            //risky, no recovery here
         }
         CDE_DRVG_stopChannel(h);
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Could not start MEMCPY\n");
      }
      HCG_MNGRG_devoteUnit( HCG_MNGRG_DMA0 + channelInfo->dmaChannelHandle->coreNum );
      channelInfo->channelStatus = CDE_MNGRG_CHAN_STATUS_CONFIGURED_E;
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Could not start MEMCPY, retcode=%d\n", ret);
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: CDE_MNGRG_memcpy
*
*  Description: Performes memcpy using DMA. If callback provided - non blocking, else blocking
*
*  Inputs: src, dst, size
*
*  Outputs: None
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE CDE_MNGRG_memcpy( CDE_MNGRG_channnelInfoT *channelInfo, UINT32 dst, UINT32 src, void *buffP )
{
   CDE_DRVG_updateMemToMemAddress(channelInfo->dmaChannelHandle, dst, src);
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
   (void)buffP;
#else
   channelInfo->dmaChannelHandle->doneBuffDesc = buffP;
#endif
   return CDE_MNGRG_startChannel(channelInfo);
}

/****************************************************************************
*
*  Function Name: CDE_MNGRG_colorDataBuffers
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE CDE_MNGRG_colorDataBuffers( CDE_MNGRG_channnelInfoT *channelInfo, UINT8 value )
{
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;

   MEM_POOLG_bufDescT *buffDesc[200];
   UINT16 i = 0;

   for( i = 0; i < MEM_POOLG_getNumBufs(channelInfo->memPoolHandle); i++ )
   {
      ret = MEM_POOLG_alloc(channelInfo->memPoolHandle, MEM_POOLG_getBufSize(channelInfo->memPoolHandle), &buffDesc[i]);
      if(ERRG_SUCCEEDED(ret))
      {
         memset(buffDesc[i]->dataP, value, MEM_POOLG_getBufSize(channelInfo->memPoolHandle));
         CMEM_cacheWb((void*)buffDesc[i]->dataP, MEM_POOLG_getBufSize(channelInfo->memPoolHandle));
         //printf(MAGENTA("Zeroed buffer %d, ptr %p, MEM_POOLG_getBufSize(channelInfo->memPoolHandle) = %d, size - %d\n"), i, buffDesc[i]->dataP, MEM_POOLG_getBufSize(channelInfo->memPoolHandle), size);
      }
      else
      {
         printf("Failed to allocate buffer %d, descriptor %p\n", i, buffDesc[i]);
         return ret;
      }
   }

   for( i = 0; i < MEM_POOLG_getNumBufs(channelInfo->memPoolHandle); i++ )
   {
      MEM_POOLG_free(buffDesc[i]);
   }

   return ret;
}

ERRG_codeE CDE_MNGRG_setupExtIntInfo(CDE_MNGRG_channnelInfoT *mainChannelInfo, UINT32 mainBlkInstance)
{
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
   UINT32 intStrms = 0;

   mainChannelInfo->extInfo.useExtIntMode = true;
   mainChannelInfo->extInfo.mainBlkInstance = mainBlkInstance;
   return ret;
}

ERRG_codeE CDE_MNGRG_fixExtIntAxiRdCfg(CDE_MNGRG_channnelInfoT *mainChannelInfo, CDE_DRVG_channelCfgT *configuration)
{
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
   UINT32 beatLast;
   UINT32 intStrms;
   UINT32 temp;
   UINT32 mainBlkInstance = mainChannelInfo->extInfo.mainBlkInstance;

   /* Read AXI int_strm_en and setup subChannels */
   PPE_MNGRG_getReaderIntStrmEn(mainBlkInstance, &intStrms);
   //printf("<<<<<<<<<<<<<<<<<<<<<<<<AXI_READ%d: int_strm_en=0x%x\n", mainChannelInfo->extInfo.mainBlkInstance, intStrms);
   mainChannelInfo->extInfo.subChannels = 0;
   temp = intStrms;
   while (temp)
   {
      mainChannelInfo->extInfo.subChannels++;
      temp >>= 1;
   }
   mainChannelInfo->extInfo.subChannels--; //Remove main channel
   //printf("AXI_READ%d: IntStrmEn=0x%x, subChannels=%d\n", mainBlkInstance, intStrms, mainChannelInfo->extInfo.subChannels);
   PPE_MNGRG_getReaderBeatLast(mainBlkInstance, &beatLast);
   //printf("AXI_READ%d: original beat last=%d\n", mainBlkInstance, beatLast);
   beatLast =  ((beatLast + 1)/ (mainChannelInfo->extInfo.subChannels + 1)) -1;
   /* Modify AXI int_strm_en & AXI beat_last */
   for (temp = mainBlkInstance; temp < (mainBlkInstance + mainChannelInfo->extInfo.subChannels + 1); temp++)
   {
      PPE_MNGRG_setReaderIntStrmEn(temp, (1 << (temp % 4)));
      PPE_MNGRG_getReaderIntStrmEn(temp, &intStrms);
      PPE_MNGRG_setReaderSingleEnd(temp, 0);
      PPE_MNGRG_setReaderBeatLast(temp, beatLast);
      //printf("AXI_READ%d: int_strm_en=0x%x new beatLast=%d\n", temp, intStrms, beatLast);
   }
   /* Modify readerConfig's scenarioType */
   configuration->streamCfg.scenarioType = CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_EXT_E;

   return ret;
}

ERRG_codeE CDE_MNGRG_allocExtIntSubChannels(CDE_MNGRG_channnelInfoT *mainChannelInfo)
{
   ERRG_codeE ret = HW_MNGR__RET_SUCCESS;
   /* Allocate DMA channels for subchannels*/
   UINT32 blkInstance;
   UINT32 blkOffset;
   UINT32 periphPhyAddress = 0;
   UINT32 totalSubChannels = mainChannelInfo->extInfo.subChannels;
   CDE_DRVG_channelHandleT channelHandle;
   CDE_DRVG_PeriphClientIdU periph;

   //printf("[Leon]:***********MainChannel - 0x%x DMA Handle - 0x%x\n", mainChannelInfo, mainChannelInfo->dmaChannelHandle);

   for (blkOffset = 0; blkOffset < totalSubChannels; blkOffset++)
   {
      blkInstance = mainChannelInfo->extInfo.mainBlkInstance + 1 + blkOffset;
      ret = CDE_MNGRG_openChannel(&mainChannelInfo->extInfo.subChannelsInfo[blkOffset], ppeAxiBuffers[blkInstance], CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E);
      if (ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to open CDE channel(for DMA interleaving) for AXI READ%d\n", blkInstance);
         return ret;
      }

      periph = (CDE_DRVG_PeriphClientIdU)ppeAxiBuffers[blkInstance];
      channelHandle = mainChannelInfo->extInfo.subChannelsInfo[blkOffset]->dmaChannelHandle;
      CDE_MNGRP_getPeriphPhysicalAddress(ppeAxiBuffers[blkInstance], &periphPhyAddress);
      CDE_DRVG_setPeripheralParams(&channelHandle->srcParams, periph.field.periphPort, periphPhyAddress);

      LOGG_PRINT(LOG_INFO_E, NULL, "CDE channel opened(0x%x, DMA Handle - 0x%x) for AXI_READ%d\n"
                      , mainChannelInfo->extInfo.subChannelsInfo[blkOffset]
                      , mainChannelInfo->extInfo.subChannelsInfo[blkOffset]->dmaChannelHandle
                      , blkInstance);
   }

   return ret;
}
#ifdef __cplusplus
}
#endif
