
/****************************************************************************
*
*   FileName: dma_drv.c
*
*   Author:  Dima S.
*
*   Date:
*
*   Description:  DMA Driver over 3xPL330
*
****************************************************************************/

/****************************************************************************
***************               I N C L U D E   F I L E S        *************
****************************************************************************/


#ifdef __cplusplus
extern "C" {
#endif

#include "inu_common.h"
#include "cde_drv_new.h"
#include "cde_scenarios.h"
#include "PL330_dma_misc.h"
#include "hcg_mngr.h"
#include "assert.h"
/****************************************************************************
***************         L O C A L       D E F N I T I O N S  ***************
****************************************************************************/


/****************************************************************************
***************            L O C A L    T Y P E D E F S      ***************
****************************************************************************/
#ifndef CDE_DRVG_IMPROVED_ISR_HANDLING
typedef struct
{
   CDE_DRVG_driverIntCbT   intCb;
}CDE_DRVP_driverParamsT;
#endif

typedef struct
{
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   CDE_DRVG_channelParamsT    *channelDb[CDE_DRVG_CORE_NUM_MAX_E][CDE_DRVG_MAX_CHANNELS_PER_CORE];
   CDE_DRVG_channelParamsT    virtualChannelDb[CDE_DRVG_CORE_NUM_MAX_E][CDE_DRVG_MAX_VIRTUAL_CHANNELS_PER_CORE];
#else
   CDE_DRVG_channelParamsT    channelDb[CDE_DRVG_CORE_NUM_MAX_E][CDE_DRVG_MAX_CHANNELS_PER_CORE];
#endif
#ifndef CDE_DRVG_IMPROVED_ISR_HANDLING
   CDE_DRVP_driverParamsT     drvParams;
#endif
}CDE_DRVP_driverDbT;


/****************************************************************************
***************       L O C A L         D A T A              ***************
****************************************************************************/
static CDE_DRVP_driverDbT              *drvDbP;
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
static CDE_DRVP_driverDbT              drvDb;
#endif
/****************************************************************************
***************       G L O B A L       D A T A              ***************
****************************************************************************/

/****************************************************************************
***************      E X T E R N A L   F U N C T I O N S     ***************
****************************************************************************/

/****************************************************************************
***************     P R E    D E F I N I T I O N     OF      ***************
***************     L O C A L         F U N C T I O N S      ***************
****************************************************************************/
static   ERRG_codeE CDE_DRVP_setCcr(  CDE_DRVG_channelHandleT  channelHandle );

ERRG_codeE CDE_DRVG_sendEventChannel( CDE_DRVG_channelHandleT channelH );

/****************************************************************************
***************      L O C A L       F U N C T I O N S       ***************
****************************************************************************/

/****************************************************************************
*
*  Function Name: CDE_DRVP_reset
*
*  Description: Reset Data base of driver
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
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
CDE_DRVG_channelHandleT* CDE_DRVP_getChannelParam(UINT8 core, UINT8 channel)
#else
CDE_DRVG_channelHandleT CDE_DRVP_getChannelParam(UINT8 core, UINT8 channel)
#endif
{
   return &drvDbP->channelDb[core][channel];
}

#ifdef CDE_DRVG_VIRTUAL_CHANNELS
static CDE_DRVG_channelHandleT CDE_DRVP_getVirtualChannelParam(UINT8 core, UINT8 channel)
{
   return &drvDbP->virtualChannelDb[core][channel];
}
#endif


/****************************************************************************
*
*  Function Name: CDE_DRVP_reset
*
*  Description: Reset Data base of driver
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

UINT8 CDE_DRVP_getPeriphIdCoreNum(CDE_DRVG_PeriphClientIdU periphId)
{
   return periphId.field.core;
}

/****************************************************************************
*
*  Function Name: CDE_DRVP_reset
*
*  Description: Reset Data base of driver
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

UINT8 CDE_DRVP_getPeriphIdPeriphNum(CDE_DRVG_PeriphClientIdU periphId)
{
   return periphId.field.periphPort;
}

/****************************************************************************
*
*  Function Name: CDE_DRVP_reset
*
*  Description: Reset Data base of driver
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
static ERRG_codeE CDE_DRVP_reset(void)
{
   UINT8 core;
   UINT8 channel;

#ifdef CDE_DRVG_VIRTUAL_CHANNELS

   for( core = 0; core < (UINT8)CDE_DRVG_CORE_NUM_MAX_E; core++ )
   {
      for( channel = 0; channel < (UINT8)CDE_DRVG_MAX_VIRTUAL_CHANNELS_PER_CORE; channel++ )
      {
         drvDbP->virtualChannelDb[core][channel].coreNum = core;
         drvDbP->virtualChannelDb[core][channel].virtualChannelNum       = channel;
         drvDbP->virtualChannelDb[core][channel].srcParams.periphNum     = CDE_DRVG_PERIPH_NUM_INVALID;  
         drvDbP->virtualChannelDb[core][channel].dstParams.periphNum     = CDE_DRVG_PERIPH_NUM_INVALID;  
         drvDbP->virtualChannelDb[core][channel].managerInfo             = NULL;
         drvDbP->virtualChannelDb[core][channel].ccr.word                = 0;
         drvDbP->virtualChannelDb[core][channel].frameDoneIntEnabled     = 0;
         drvDbP->virtualChannelDb[core][channel].channelDoneIntEnabled   = 0;
         drvDbP->virtualChannelDb[core][channel].numSetEventFrameDone    = 0;
      }
   }
#else
   memset( drvDbP, 0x00, sizeof(CDE_DRVP_driverDbT));

#ifndef CDE_DRVG_IMPROVED_ISR_HANDLING
   drvDbP->drvParams.intCb  = NULL;
#endif

   for( core = 0; core < (UINT8)CDE_DRVG_CORE_NUM_MAX_E; core++ )
   {
      for( channel = 0; channel < (UINT8)CDE_DRVG_MAX_CHANNELS_PER_CORE; channel++ )
      {
         drvDbP->channelDb[core][channel].coreNum              = core;
         drvDbP->channelDb[core][channel].channelNum         = channel;
         drvDbP->channelDb[core][channel].srcParams.periphNum  = CDE_DRVG_PERIPH_NUM_INVALID;  
         drvDbP->channelDb[core][channel].dstParams.periphNum  = CDE_DRVG_PERIPH_NUM_INVALID;  
         drvDbP->channelDb[core][channel].managerInfo       = NULL;
         drvDbP->channelDb[core][channel].ccr.word          = 0;
         drvDbP->channelDb[core][channel].frameDoneIntEnabled  = 0;
         drvDbP->channelDb[core][channel].channelDoneIntEnabled   = 0;
       }
   }

#endif
   
   return(CDE__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: CDE_DRVP_setCcr
*
*  Description:      
*
*  Inputs:        channelHandle
*
*  Outputs:       channelHandle
*
*  Returns:
*
*  Context: 
*
****************************************************************************/

ERRG_codeE CDE_DRVP_setCcr(  CDE_DRVG_channelHandleT  channelHandle )
{
   ERRG_codeE ret = CDE__RET_SUCCESS;

   if( channelHandle->srcParams.periphNum == CDE_DRVG_PERIPH_NUM_MEMORY )
   {
      channelHandle->ccr.field.srcInc = 1;                                 // incremental
   }
   else if( channelHandle->srcParams.periphNum == CDE_DRVG_PERIPH_NUM_INVALID )
   {
      return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                              // TODO: define another error
   }
   else
   {
      channelHandle->ccr.field.srcInc = 0;                                 // fixed
   }

   
   if( channelHandle->dstParams.periphNum == CDE_DRVG_PERIPH_NUM_MEMORY )
   {
      channelHandle->ccr.field.dstInc = 1;                                 // incremental
   }
   else if( channelHandle->dstParams.periphNum == CDE_DRVG_PERIPH_NUM_INVALID )
   {
      return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                              // TODO: define another error
   }
   else
   {
      channelHandle->ccr.field.dstInc = 0;                                 // fixed
   }
   if( channelHandle->dstParams.buffSize > 0)
   {
      if( channelHandle->dstParams.buffSize > 0x280)     // assume large buffers aligned to 256 bytes
      {
         // TODO: Write function that calculates size and length according to alignment. Currently fixed to 256 bytes
         channelHandle->ccr.field.srcBurstSize = 4;                                 // 128 bit (1 << size) bytes
         channelHandle->ccr.field.dstBurstSize = 4;                                 // 128 bit (1 << size) bytes

         channelHandle->ccr.field.srcBurstLen = 15;                                 // (len + 1) beats per burst
         channelHandle->ccr.field.dstBurstLen = 15;                                 // (len + 1) beats per burst
      }
      else                                               // assume large buffers aligned to 4 bytes
      {
         channelHandle->ccr.field.srcBurstSize = 4;                                 // 128 bit (1 << size) bytes
         channelHandle->ccr.field.dstBurstSize = 4;                                 // 128 bit (1 << size) bytes
      
         channelHandle->ccr.field.srcBurstLen = 0;                                 // (len + 1) beats per burst
         channelHandle->ccr.field.dstBurstLen = 0;                                 // (len + 1) beats per burst
      }
      channelHandle->ccr.field.endianSwapSize = 0;                            // No swapping
   }

   if( channelHandle->srcParams.buffSize > 0)
   {
      //memcpy
      if ((channelHandle->ccr.field.srcInc == 1) && (channelHandle->ccr.field.dstInc == 1))
      {
         channelHandle->ccr.field.srcBurstSize = 2;                                 // 128 bit (1 << size) bytes
         channelHandle->ccr.field.dstBurstSize = 2;                                 // 128 bit (1 << size) bytes

         if( channelHandle->srcParams.buffSize > 0x100)
         {
            channelHandle->ccr.field.srcBurstLen = 15;                                  // (len + 1) beats per burst
            channelHandle->ccr.field.dstBurstLen = 15;                                 // (len + 1) beats per burst         
         }
         else
         {
            channelHandle->ccr.field.srcBurstLen = 0;                                  // (len + 1) beats per burst
            channelHandle->ccr.field.dstBurstLen = 0;                                 // (len + 1) beats per burst
         }
      }
      // Injection.
      else if( channelHandle->srcParams.buffSize > 0x100)  // assume large buffers aligned to 256 bytes
      {
         // TODO: Write function that calculates size and length according to alignment. Currently fixed to 256 bytes
         channelHandle->ccr.field.srcBurstSize = 2;                                 // 32 bit (1 << size) bytes
         channelHandle->ccr.field.dstBurstSize = INJECTION_BURST_SIZE;                                 // 128 bit (1 << size) bytes

         //burst len value is calculated in 'SEQ_MNGRP_setEnableWriterDma' function
         //channelHandle->ccr.field.srcBurstLen = 15;                                 // (len + 1) beats per burst
         //channelHandle->ccr.field.dstBurstLen = 15;                                 // (len + 1) beats per burst
      }
      else                                               // assume large buffers aligned to 4 bytes
      {
         channelHandle->ccr.field.srcBurstSize = 2;                                 // 128 bit (1 << size) bytes
         channelHandle->ccr.field.dstBurstSize = 2;                                 // 128 bit (1 << size) bytes
      
         channelHandle->ccr.field.srcBurstLen = 0;                                 // (len + 1) beats per burst
         channelHandle->ccr.field.dstBurstLen = 0;                                 // (len + 1) beats per burst
      }
      channelHandle->ccr.field.endianSwapSize = 0;                            // No swapping
   }
   return ret;

   
}

/****************************************************************************
*
*  Function Name: CDE_DRVP_getLaziestCoreNum
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

UINT8 CDE_DRVP_getLaziestCoreNum( void )
{
   UINT8 laziestCore = 0xFF;  
   UINT8 minChanCount = 0xFF;
   UINT8 currentChanCount = 0;
   UINT8 i = 0;
   UINT8 j = 0;
   
   for(j = 0; j < CDE_DRVG_CORE_NUM_MAX_E; j++)
   {
      currentChanCount = 0;
      for(i = 0; i < CDE_DRVG_MAX_CHANNELS_PER_CORE; i++)
      {
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
         if(   CDE_DRVP_getVirtualChannelParam(j, i)->managerInfo != NULL )
#else
         if(   CDE_DRVP_getChannelParam(j, i)->managerInfo != NULL )
#endif
         {
            currentChanCount++;
         }
      }
      if( currentChanCount < minChanCount )
      {  
         minChanCount = currentChanCount;
         laziestCore = j;
      }
   }

   return laziestCore;
}

#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
static void CDE_DRVP_isrCore(UINT64 timeStamp, UINT64 count, void *argP)
{
   CDE_DRVG_channelHandleT channelParam;
   UINT32 interruptStatus[CDE_DRVG_CORE_NUM_MAX_E];
   UINT8  channel = 0;
   UINT32 core = (UINT32)argP;

   (void)count;
   interruptStatus[core] = PL330_DMA_getInterruptStatus(core);                   // snapshot of interrupts
   if( interruptStatus[core] & 0xFF )                                      // quick    check if any frame done interrupt is pending                      
   {
      for( channel = 0; channel < CDE_DRVG_MAX_CHANNELS_PER_CORE; channel++ )       // loop on frame done interrupt
      {
         if( interruptStatus[core] & (PL330_DMA_FRAME_DONE_INT_BIT << channel) )
         {
            PL330_DMA_clearFrameDoneInterrupt( core, channel );
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   channelParam = *(CDE_DRVP_getChannelParam( core, channel ));
#else
   channelParam = CDE_DRVP_getChannelParam( core, channel );
#endif
            if((channelParam) &&(channelParam->intCb))
            {
               //printf("ISR core %d ch %d, interruptStatus 0x%x ts %llu\n",core,channel,interruptStatus[core],timeStamp);
               channelParam->intCb(channelParam->managerInfo, CDE_DRVG_INTERRUPT_TYPE_FRAME_DONE_E, timeStamp);
            }
            //else
            //{
            //   if (!channelParam)
            //   {
            //      printf("ISR core %d ch %d, but no virtual ch assigned!\n",core,channel);
            //   }
            //}
         }
      }
   }

   if( interruptStatus[core] & 0xFF00 )                                    // quick    check if any channel done interrupt is pending                       
   {
      for( channel = 0; channel < CDE_DRVG_MAX_CHANNELS_PER_CORE; channel++ )       // loop on channel done interrupt
      {
         if( interruptStatus[core] & (PL330_DMA_CHANNEL_DONE_INT_BIT << channel) )
         {
            PL330_DMA_clearChannelDoneInterrupt( core, channel );
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
            channelParam = *(CDE_DRVP_getChannelParam( core, channel ));
#else
            channelParam = CDE_DRVP_getChannelParam( core, channel );
#endif
            if((channelParam) && (channelParam->intCb))
            {
               channelParam->intCb(channelParam->managerInfo, CDE_DRVG_INTERRUPT_TYPE_CHANNEL_DONE_E, timeStamp);
            }
            //else
            //{
            //   if (!channelParam)
            //   {
            //      printf("ISR core %d ch %d, but no virtual ch assigned!\n",core,channel);
            //   }
            //}
         }
      }
   }
}

#else
static void CDE_DRVP_isrCore(UINT64 timeStamp, UINT64 count, void *argP)
{
   CDE_DRVG_channelHandleT channelParam;
   UINT32 interruptStatus[CDE_DRVG_CORE_NUM_MAX_E];
   UINT8 channel = 0;
   UINT8 retCode;
   UINT32 core = (UINT32)argP;

   (void)count;
   interruptStatus[core] = PL330_DMA_getInterruptStatus(core);                   // snapshot of interrupts
   if( interruptStatus[core] & 0xFF )                                      // quick    check if any frame done interrupt is pending                      
   {
      for( channel = 0; channel < CDE_DRVG_MAX_CHANNELS_PER_CORE; channel++ )       // loop on frame done interrupt
      {
         if( interruptStatus[core] & (PL330_DMA_FRAME_DONE_INT_BIT << channel) )
         {
            channelParam = CDE_DRVP_getChannelParam( core, channel );
            if (channelParam->srcParams.periphNum != CDE_DRVG_PERIPH_NUM_MEMORY)
            {
				if( (channelParam->srcParams.periphNum == 14) && 
					(channelParam->scenarioType == CDE_DRVG_SCENARIO_TYPE_IIC_STREAM_DDR_LESS) )
				{
					retCode=1;
				}
				else
				{
                	retCode = CDE_SCENARIOSG_setNewDest(channelParam);
				}
            }
            else
            {
               retCode=1;
            }

            channelParam->frameDoneCounter++;                              // Count frame done interrups

            if (retCode != 0)
            {
              if( drvDbP->drvParams.intCb )
              {
                 //LOGG_PRINT(LOG_INFO_E, NULL, "CDE_DRVG_isrCore: Core = %d, channel = %d, mngrInfo = %p, frame cnt = %d, ts = %llu \n", core, channel, channelParam->managerInfo, channelParam->frameDoneCounter , timeStamp);
                 drvDbP->drvParams.intCb(channelParam->managerInfo, CDE_DRVG_INTERRUPT_TYPE_FRAME_DONE_E, timeStamp);
              }
            }
            PL330_DMA_clearFrameDoneInterrupt( core, channel );
            HCG_MNGRG_processEvent(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH0_E + (core * 8) + channel, timeStamp);
         }
      }
   }

   if( interruptStatus[core] & 0xFF00 )                                    // quick    check if any channel done interrupt is pending                       
   {
      for( channel = 0; channel < CDE_DRVG_MAX_CHANNELS_PER_CORE; channel++ )       // loop on channel done interrupt
      {
         if( interruptStatus[core] & (PL330_DMA_CHANNEL_DONE_INT_BIT << channel) )
         {
            channelParam = CDE_DRVP_getChannelParam( core, channel );

            if( drvDbP->drvParams.intCb )
            {
               drvDbP->drvParams.intCb(channelParam->managerInfo, CDE_DRVG_INTERRUPT_TYPE_CHANNEL_DONE_E, timeStamp);
            }

            PL330_DMA_clearChannelDoneInterrupt( core, channel );
         }
      }
   }
}
#endif

static void CDE_DRVP_handleAbortIsr(UINT64 timeStamp, UINT64 count, void *argP)
{
   UINT32 FSRD,FSRC,FTR,DSR,SAR;
   UINT8  channel = 0;
   (void)count;
   UINT32 core = (UINT32)argP;

   FSRD = PL330_DMA_getFaultStatusManager(core);
   FSRC = PL330_DMA_getFaultStatusChannel(core);

   printf("CDE error core %d, ts %llu. FSRD = 0x%x FSRC = 0x%x\n",core,timeStamp,FSRD,FSRC);

   while(FSRC)
   {
      if (FSRC & 0x1)
      {
         FTR = PL330_DMA_getFaultTypeChannel(core,channel);
         DSR = PL330_DMA_getDAR(core,channel);		 
         SAR = PL330_DMA_getSAR(core,channel);
         printf("channel %d, fault type 0x%x. DAR = 0x%x, SAR = 0x%x\n",channel,FTR,DSR,SAR);
         PL330DMAG_SendCommand( (dmaChannelProg)NULL, core, channel, PL330DMA_CMD_DMAKILL, 0 );
      }
      channel++;
      FSRC = FSRC>>1;
   }
}

/****************************************************************************
***************     G L O B A L       F U N C T I O N S  **************
****************************************************************************/

/****************************************************************************
*
*  Function Name: CDE_DRVG_open
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
ERRG_codeE CDE_DRVG_init(UINT32 dmaRegistersVirtualAddr, UINT32 dmaDriverDBVirtualAddr, UINT32 size)
{
   ERRG_codeE ret;
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   int ctr = 0;
   UINT32 allocatedSize = 0;

   drvDbP = &drvDb;
   memset(drvDbP,0,sizeof(CDE_DRVP_driverDbT));
   for(int core = 0; core < CDE_DRVG_CORE_NUM_MAX_E; core++)
   {
      for(int ch = 0; ch < (CDE_DRVG_MAX_VIRTUAL_CHANNELS_PER_CORE); ch++)
      {
         drvDbP->virtualChannelDb[core][ch].program = (UINT8*)(dmaDriverDBVirtualAddr + (CDE_DRVG_CH_PROGRAMM_MAX_SIZE * ctr));
         ret = MEM_MAPG_convertVirtualToPhysical2((UINT32)drvDbP->virtualChannelDb[core][ch].program, &drvDbP->virtualChannelDb[core][ch].phyProgAddress, MEM_MAPG_CRAM_CDE_PARAMS_E);
         if(ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "CDE DRV failed to get programm physical address\n");
            return ret;
         }
         allocatedSize += CDE_DRVG_CH_PROGRAMM_MAX_SIZE;
         ctr++;
      }
   }
   if (allocatedSize > size)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "DMA HAS DB IS LARGER THEN CRAM ALLOCATED MEM! %d > %d \n", allocatedSize, size);
      assert(0);
   }
#else
   drvDbP = (CDE_DRVP_driverDbT*)dmaDriverDBVirtualAddr;
#endif
   
   ret = CDE_DRVP_reset( );
   
   PL330_DMA_initBaseAddress(dmaRegistersVirtualAddr);

   return ret;
}

#ifdef CDE_DRVG_VIRTUAL_CHANNELS
ERRG_codeE CDE_DRVG_assignPhysicalChannel( CDE_DRVG_channelHandleT channelH )
{
   ERRG_codeE ret = CDE__RET_SUCCESS;
   UINT32 i, foundCh = 0;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "Assigning channel 0x%x",channelH);
   
   //check if this channelH already assigned
      for(i = 0; i < CDE_DRVG_MAX_CHANNELS_PER_CORE; i++)
      {
         if(drvDbP->channelDb[channelH->coreNum][i] == channelH )
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "channelH 0x%x already assigned\n",channelH);
            assert(0);
         }
      }
   
      // find unoccupied physical channel 
      for(i = 0; i < CDE_DRVG_MAX_CHANNELS_PER_CORE; i++)
      {
         if(drvDbP->channelDb[channelH->coreNum][i] == NULL )
         {
            //LOGG_PRINT(LOG_INFO_E, NULL, "found physical free channel %d for core %d point to virtual %p\n",i,channelH->coreNum,&drvDbP->virtualChannelDb[channelH->coreNum][channelH->virtualChannelNum]);
            drvDbP->channelDb[channelH->coreNum][i] = &drvDbP->virtualChannelDb[channelH->coreNum][channelH->virtualChannelNum];// if found point to physical-virtual
            channelH->hwChannelNum = i;
            foundCh = 1;
            break;
         }
      }
      
      if (!foundCh)
      {
         return CDE__ERR_OUT_OF_RSRCS;
      }
         
      //update programa with given phisical channel
      CDE_SCENARIOSG_setEvent(channelH);

   return ret;
}
#endif
/****************************************************************************
*
*  Function Name: CDE_DRVG_openChannelFixedCoreNum
*
*  Description:
*   This is used to create the metadata thread (if metadata is used) which has to run on the same core as 
*   the main frame buffer thread. 
*   CDE_DRVG_openChannel cannot be used because it can assign any DMA thread to any core depending on what's free.
*   And due to the two threads communicating over the Event register (Using the DMASEV and DMAWFE event instructions), 
*   both threads have to run on the same core so therefore the metadata thread must be pinned to the same 
*   core as the frame buffer thread.
*
*  Outputs: none
*
*  Returns:
*  Error code
*  Context: 
*
****************************************************************************/
ERRG_codeE CDE_DRVG_openChannelFixedCoreNum( CDE_DRVG_channelHandleT* channelH,CDE_DRVG_coreNumE requestedCore, 
   CDE_DRVG_PeriphClientIdE source,  CDE_DRVG_PeriphClientIdE destination )
{
   CDE_DRVG_channelHandleT handle;
   UINT8 i = 0;
   /*This function assigns a DMA thread to a virtual channel on the core described by the requestedCore argument
     It was created with inspiration from CDE_DRVG_openChannel  
   */
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   for(i = 0; i < CDE_DRVG_MAX_VIRTUAL_CHANNELS_PER_CORE; i++)
   {
      if( CDE_DRVP_getVirtualChannelParam(requestedCore, i)->managerInfo == NULL )                  // find unoccupied virtual channel 
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "found free virtual channel %d for core %d virtual handle pointer %p\n",i, requestedCore,&drvDbP->virtualChannelDb[requestedCore][i]);
         handle = CDE_DRVP_getVirtualChannelParam(requestedCore, i);                       // if found return
         handle->virtualChannelNum = i;
        *channelH = handle;

         handle->srcParams.periphNum = CDE_DRVP_getPeriphIdPeriphNum((CDE_DRVG_PeriphClientIdU) source);
         handle->dstParams.periphNum = CDE_DRVP_getPeriphIdPeriphNum((CDE_DRVG_PeriphClientIdU) destination);
         
         return(CDE__RET_SUCCESS);
      }
   }
#else
   for(i = 0; i < CDE_DRVG_MAX_CHANNELS_PER_CORE; i++)
   {
      if( CDE_DRVP_getChannelParam(requestedCore, i)->managerInfo == NULL )                  // find unoccupied channel 
      {
         handle = CDE_DRVP_getChannelParam(requestedCore, i);                       // if found return
         *channelH = handle;

         handle->srcParams.periphNum = CDE_DRVP_getPeriphIdPeriphNum((CDE_DRVG_PeriphClientIdU) source);
         handle->dstParams.periphNum = CDE_DRVP_getPeriphIdPeriphNum((CDE_DRVG_PeriphClientIdU) destination);
         
         return(CDE__RET_SUCCESS);
      }
   }
#endif

    
                                                                  // if we are here - there was no free channel on this core
   return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                                 // TODO: define another error
}

/****************************************************************************
*
*  Function Name: CDE_DRVG_openChannel
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
ERRG_codeE CDE_DRVG_openChannel( CDE_DRVG_channelHandleT* channelH, CDE_DRVG_PeriphClientIdE source, CDE_DRVG_PeriphClientIdE destination )
{
   CDE_DRVG_coreNumE srcCore = (CDE_DRVG_coreNumE)CDE_DRVP_getPeriphIdCoreNum( (CDE_DRVG_PeriphClientIdU)source );
   CDE_DRVG_coreNumE dstCore = (CDE_DRVG_coreNumE)CDE_DRVP_getPeriphIdCoreNum( (CDE_DRVG_PeriphClientIdU)destination );
   CDE_DRVG_coreNumE requestedCore = CDE_DRVG_CORE_NUM_INVALID_E;
   CDE_DRVG_channelHandleT handle;
   UINT8 i = 0;

   if( (srcCore < CDE_DRVG_CORE_NUM_MAX_E) && (dstCore < CDE_DRVG_CORE_NUM_MAX_E) )          // src and dst are peripherals
   {
      if( srcCore == dstCore )
      {
         requestedCore = srcCore;
      }  
   }
   else if( (srcCore < CDE_DRVG_CORE_NUM_MAX_E) && (dstCore == CDE_DRVG_CORE_NUM_ANY_E) )       // src is peripheral, dst is memory
   {
      requestedCore = srcCore;
   }
   else if( (srcCore == CDE_DRVG_CORE_NUM_ANY_E) && (dstCore < CDE_DRVG_CORE_NUM_MAX_E) )       // src is memory, dst is peripheral
   {
      requestedCore = dstCore;      
   }
   else if( (srcCore== CDE_DRVG_CORE_NUM_ANY_E) && (dstCore == CDE_DRVG_CORE_NUM_ANY_E) )       // src is memory, dst is memory
   {
      requestedCore = CDE_DRVP_getLaziestCoreNum();
   }
   else
   {
      return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                              // TODO: define another error
   }

#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   for(i = 0; i < CDE_DRVG_MAX_VIRTUAL_CHANNELS_PER_CORE; i++)
   {
      if( CDE_DRVP_getVirtualChannelParam(requestedCore, i)->managerInfo == NULL )                  // find unoccupied virtual channel 
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "found free virtual channel %d for core %d virtual handle pointer %p\n",i, requestedCore,&drvDbP->virtualChannelDb[requestedCore][i]);
         handle = CDE_DRVP_getVirtualChannelParam(requestedCore, i);                       // if found return
         handle->virtualChannelNum = i;
        *channelH = handle;

         handle->srcParams.periphNum = CDE_DRVP_getPeriphIdPeriphNum((CDE_DRVG_PeriphClientIdU) source);
         handle->dstParams.periphNum = CDE_DRVP_getPeriphIdPeriphNum((CDE_DRVG_PeriphClientIdU) destination);
         
         return(CDE__RET_SUCCESS);
      }
   }
#else
   for(i = 0; i < CDE_DRVG_MAX_CHANNELS_PER_CORE; i++)
   {
      if( CDE_DRVP_getChannelParam(requestedCore, i)->managerInfo == NULL )                  // find unoccupied channel 
      {
         handle = CDE_DRVP_getChannelParam(requestedCore, i);                       // if found return
         *channelH = handle;

         handle->srcParams.periphNum = CDE_DRVP_getPeriphIdPeriphNum((CDE_DRVG_PeriphClientIdU) source);
         handle->dstParams.periphNum = CDE_DRVP_getPeriphIdPeriphNum((CDE_DRVG_PeriphClientIdU) destination);
         
         return(CDE__RET_SUCCESS);
      }
   }
#endif

    
                                                                  // if we are here - there was no free channel on this core
   return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                                 // TODO: define another error
}


/****************************************************************************
*
*  Function Name: CDE_DRVG_openChannel
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
ERRG_codeE CDE_DRVG_closeChannel( CDE_DRVG_channelHandleT channelH )
{
#ifdef CDE_DRVG_VIRTUAL_CHANNELS

   memset(channelH->program, 0x00, CDE_DRVG_CH_PROGRAMM_MAX_SIZE);

   channelH->srcParams.periphNum  = CDE_DRVG_PERIPH_NUM_INVALID;
   channelH->dstParams.periphNum  = CDE_DRVG_PERIPH_NUM_INVALID;
   channelH->managerInfo          = NULL;
   channelH->ccr.word             = 0;
   channelH->frameDoneIntEnabled  = 0;
   channelH->channelDoneIntEnabled= 0;
   channelH->frameDoneCounter     = 0;
   channelH->chunkNum             = 0;
   channelH->numSetEventFrameDone = 0;
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
   channelH->oldIndex             = 0;
#endif


#else
   UINT8 core     = channelH->coreNum;
   UINT8 channel  = channelH->channelNum;

   memset(&channelH->program, 0x00, CDE_DRVG_CH_PROGRAMM_MAX_SIZE);
   
   drvDbP->channelDb[core][channel].srcParams.periphNum  = CDE_DRVG_PERIPH_NUM_INVALID;   
   drvDbP->channelDb[core][channel].dstParams.periphNum  = CDE_DRVG_PERIPH_NUM_INVALID;   
   drvDbP->channelDb[core][channel].managerInfo          = NULL;
   drvDbP->channelDb[core][channel].ccr.word             = 0;
   drvDbP->channelDb[core][channel].frameDoneIntEnabled  = 0;
   drvDbP->channelDb[core][channel].channelDoneIntEnabled= 0;
   drvDbP->channelDb[core][channel].frameDoneCounter     = 0;
   drvDbP->channelDb[core][channel].chunkNum             = 0;
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
   drvDbP->channelDb[core][channel].oldIndex             = 0;
#endif

#endif

   return(CDE__RET_SUCCESS);
}


/****************************************************************************
*
*  Function Name: CDE_DRVG_openChannel
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
ERRG_codeE CDE_DRVG_startChannel( CDE_DRVG_channelHandleT channelH )
{
   ERRG_codeE ret = CDE__RET_SUCCESS;
   UINT32   phyProgAddress;
   CDE_DRVG_channelHandleT handle;

   //find free physical channel
#ifdef CDE_DRVG_VIRTUAL_CHANNELS

   ret = CDE_DRVG_assignPhysicalChannel(channelH);
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "there is no free virtual channel for core %d\n",channelH->coreNum);
      return ret;
   }

#endif

   if( channelH->frameDoneIntEnabled )
   {
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      if (channelH->hwChannelNum== PL330DMA_LOOP_EVNT_NUM)
#else
      if (channelH->channelNum == PL330DMA_LOOP_EVNT_NUM)
#endif
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "No more available interrupts!\n");
         assert(0);
      }
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      PL330_DMA_enableFrameDoneInterrupt( channelH->coreNum, channelH->hwChannelNum);//before start, in setup procces, we
#else
      PL330_DMA_enableFrameDoneInterrupt( channelH->coreNum, channelH->channelNum);//before start, in setup procces, we
#endif
      
   }
   if( channelH->channelDoneIntEnabled )
   {
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      PL330_DMA_enableChannelDoneInterrupt( channelH->coreNum, channelH->hwChannelNum );
#else
      PL330_DMA_enableChannelDoneInterrupt( channelH->coreNum, channelH->channelNum );
#endif
      
   }
   
   //here needs to activate with given phisical channel
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   if( 0xFF == PL330DMAG_SendCommand( (dmaChannelProg)channelH->phyProgAddress, channelH->coreNum, channelH->hwChannelNum, PL330DMA_CMD_DMAGO, 0 ))
   {
      LOGG_PRINT(LOG_DEBUG_E, ret, "CDE DRV Send command failed on core/channel %d/%d\n",channelH->coreNum, channelH->hwChannelNum);
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, ret, "CDE DRV PL330DMA_CMD_DMAGO command DONE on core/channel %d/%d\n",channelH->coreNum, channelH->hwChannelNum);
   }
#else
   ret = MEM_MAPG_convertVirtualToPhysical2((UINT32)channelH->program, &phyProgAddress, MEM_MAPG_CRAM_CDE_PARAMS_E);
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE DRV failed to get programm physical address\n");
      return ret;
   }
   if( 0xFF == PL330DMAG_SendCommand( (dmaChannelProg)phyProgAddress, channelH->coreNum, channelH->channelNum, PL330DMA_CMD_DMAGO, 0 ))
   {
      LOGG_PRINT(LOG_INFO_E, ret, "CDE DRV Send command failed on core/channel %d/%d\n",channelH->coreNum, channelH->channelNum);
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, ret, "CDE DRV PL330DMA_CMD_DMAGO command DONE on core/channel %d/%d\n",channelH->coreNum, channelH->channelNum);
   }

#endif
   channelH->dstParams.loopCount=0;// we receive first interrupt after first loop
   return(CDE__RET_SUCCESS);
}

static void CDE_DRVP_startExtIntSubchannels(CDE_DRVG_extIntParams *extIntParams)
{
   ERRG_codeE ret = CDE__RET_SUCCESS;
   UINT32 i;
   CDE_DRVG_channelHandleT channelH;

   for (i = 0; i < extIntParams->numSubChannels; i++)
   {
      channelH = extIntParams->subChannelHandles[i];
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      if( 0xFF == PL330DMAG_SendCommand( (dmaChannelProg)channelH->phyProgAddress, channelH->coreNum, channelH->hwChannelNum, PL330DMA_CMD_DMAGO, 0 ))
      {
         LOGG_PRINT(LOG_DEBUG_E, ret, "CDE DRV Send command failed on core/channel %d/%d\n",channelH->coreNum, channelH->hwChannelNum);
      }
      else
      {
         LOGG_PRINT(LOG_DEBUG_E, ret, "CDE DRV PL330DMA_CMD_DMAGO command DONE on core/channel %d/%d\n",channelH->coreNum, channelH->hwChannelNum);
      }
#else
      ret = MEM_MAPG_convertVirtualToPhysical2((UINT32)channelH->program, &phyProgAddress, MEM_MAPG_CRAM_CDE_PARAMS_E);
      if(ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, ret, "CDE DRV failed to get programm physical address\n");
         return;
      }
      if( 0xFF == PL330DMAG_SendCommand( (dmaChannelProg)phyProgAddress, channelH->coreNum, channelH->channelNum, PL330DMA_CMD_DMAGO, 0 ))
      {
         LOGG_PRINT(LOG_INFO_E, ret, "CDE DRV Send command failed on core/channel %d/%d\n",channelH->coreNum, channelH->channelNum);
      }
      else
      {
         LOGG_PRINT(LOG_DEBUG_E, ret, "CDE DRV PL330DMA_CMD_DMAGO command DONE on core/channel %d/%d\n",channelH->coreNum, channelH->channelNum);
      }
#endif
      channelH->dstParams.loopCount=0;// we receive first interrupt after first loop
   }
}
/****************************************************************************
*
*  Function Name: CDE_DRVG_startChannelExt
*
*  Description: Extended DMA Interleaving mode channel start
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
ERRG_codeE CDE_DRVG_startChannelExt( CDE_DRVG_channelHandleT channelH, CDE_DRVG_extIntParams *extIntParams)
{
   ERRG_codeE ret = CDE__RET_SUCCESS;
   UINT32   phyProgAddress;
   CDE_DRVG_channelHandleT handle;
   UINT32   i;

   //find free physical channel
#ifdef CDE_DRVG_VIRTUAL_CHANNELS

   ret = CDE_DRVG_assignPhysicalChannel(channelH);
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "there is no free virtual channel for core %d\n",channelH->coreNum);
      return ret;
   }

   for (i = 0; i < extIntParams->numSubChannels; i++)
   {
       ret = CDE_DRVG_assignPhysicalChannel(extIntParams->subChannelHandles[i]);
       if(ERRG_FAILED(ret))
       {
          LOGG_PRINT(LOG_ERROR_E, ret, "there is no free virtual channel for core %d subchannel %d\n", extIntParams->subChannelHandles[i]->coreNum, i);
          return ret;
       }
       /* Update the WFE instruction for main channel since the sub channel numbers are assigned*/
       for (UINT32 loopNum = 0; loopNum < channelH->numLoops; loopNum++)
       {
          //printf("*******************Main Channel WFE loopNum=%d subChannel-%d hwChannelNum=%d\n", loopNum, i, extIntParams->subChannelHandles[i]->hwChannelNum);
          CDE_SCENARIOSG_waitEvent(channelH, loopNum, i, extIntParams->subChannelHandles[i]->hwChannelNum);
       }
   }

#endif

   if( channelH->frameDoneIntEnabled )
   {
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      if (channelH->hwChannelNum== PL330DMA_LOOP_EVNT_NUM)
#else
      if (channelH->channelNum == PL330DMA_LOOP_EVNT_NUM)
#endif
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "No more available interrupts!\n");
         assert(0);
      }
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      PL330_DMA_enableFrameDoneInterrupt( channelH->coreNum, channelH->hwChannelNum);//before start, in setup procces, we
#else
      PL330_DMA_enableFrameDoneInterrupt( channelH->coreNum, channelH->channelNum);//before start, in setup procces, we
#endif
   }
   if( channelH->channelDoneIntEnabled )
   {
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      PL330_DMA_enableChannelDoneInterrupt( channelH->coreNum, channelH->hwChannelNum );
#else
      PL330_DMA_enableChannelDoneInterrupt( channelH->coreNum, channelH->channelNum );
#endif
   }

   //here needs to activate with given phisical channel
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   if( 0xFF == PL330DMAG_SendCommand( (dmaChannelProg)channelH->phyProgAddress, channelH->coreNum, channelH->hwChannelNum, PL330DMA_CMD_DMAGO, 0 ))
   {
      LOGG_PRINT(LOG_DEBUG_E, ret, "CDE DRV Send command failed on core/channel %d/%d\n",channelH->coreNum, channelH->hwChannelNum);
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, ret, "CDE DRV PL330DMA_CMD_DMAGO command DONE on core/channel %d/%d\n",channelH->coreNum, channelH->hwChannelNum);
   }
#else
   ret = MEM_MAPG_convertVirtualToPhysical2((UINT32)channelH->program, &phyProgAddress, MEM_MAPG_CRAM_CDE_PARAMS_E);
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE DRV failed to get programm physical address\n");
      return ret;
   }
   if( 0xFF == PL330DMAG_SendCommand( (dmaChannelProg)phyProgAddress, channelH->coreNum, channelH->channelNum, PL330DMA_CMD_DMAGO, 0 ))
   {
      LOGG_PRINT(LOG_INFO_E, ret, "CDE DRV Send command failed on core/channel %d/%d\n",channelH->coreNum, channelH->channelNum);
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, ret, "CDE DRV PL330DMA_CMD_DMAGO command DONE on core/channel %d/%d\n",channelH->coreNum, channelH->channelNum);
   }

#endif
   channelH->dstParams.loopCount=0;// we receive first interrupt after first loop
#if 1
   CDE_DRVP_startExtIntSubchannels(extIntParams);
#endif
   return(CDE__RET_SUCCESS);
}




/****************************************************************************
*
*  Function Name: CDE_DRVG_openChannel
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
ERRG_codeE CDE_DRVG_stopChannel( CDE_DRVG_channelHandleT channelH )
{
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   PL330DMAG_SendCommand( (dmaChannelProg)NULL, channelH->coreNum, channelH->hwChannelNum, PL330DMA_CMD_DMAKILL, 0 );
   
   //LOGG_PRINT(LOG_INFO_E, NULL, "put null on channelDb (physical list) core %d channel %d handle 0x%x\n",channelH->coreNum, channelH->hwChannelNum,drvDbP->channelDb[channelH->coreNum][channelH->hwChannelNum]);
   drvDbP->channelDb[channelH->coreNum][channelH->hwChannelNum] = NULL;

#else
   PL330DMAG_SendCommand( (dmaChannelProg)NULL, channelH->coreNum, channelH->channelNum, PL330DMA_CMD_DMAKILL, 0 );
#endif

   return(CDE__RET_SUCCESS);
}


/****************************************************************************
*
*  Function Name: CDE_DRVG_openChannel
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
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING  
ERRG_codeE CDE_DRVG_open( )
#else
ERRG_codeE CDE_DRVG_open(CDE_DRVG_driverIntCbT intCb)
#endif
{
#ifndef CDE_DRVG_IMPROVED_ISR_HANDLING  
   if( (intCb) && (drvDbP->drvParams.intCb == NULL) )
   {
      drvDbP->drvParams.intCb = intCb;
   }
   else
   {
      return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                              // TODO: define another error
   }
#endif
#if 0
   OS_LYRG_intCtrlRegister( OS_LYRG_INT_CDE_1_E, CDE_DRVG_isr );
   OS_LYRG_intCtrlEnable( OS_LYRG_INT_CDE_1_E ); 
   OS_LYRG_intCtrlRegister( OS_LYRG_INT_CDE_2_E, CDE_DRVG_isr );
   OS_LYRG_intCtrlEnable( OS_LYRG_INT_CDE_2_E ); 
#else
   OS_LYRG_intCtrlRegister( OS_LYRG_INT_CDE_1_E, CDE_DRVP_isrCore , (void*)0);
   OS_LYRG_intCtrlEnable( OS_LYRG_INT_CDE_1_E ); 
   OS_LYRG_intCtrlRegister( OS_LYRG_INT_CDE_2_E, CDE_DRVP_isrCore , (void*)1);
   OS_LYRG_intCtrlEnable( OS_LYRG_INT_CDE_2_E ); 
   OS_LYRG_intCtrlRegister( OS_LYRG_INT_CDE_3_E, CDE_DRVP_isrCore , (void*)2);
   OS_LYRG_intCtrlEnable( OS_LYRG_INT_CDE_3_E ); 

   OS_LYRG_intCtrlRegister( OS_LYRG_INT_CDE_4_E, CDE_DRVP_isrCore , (void*)0);
   OS_LYRG_intCtrlEnable( OS_LYRG_INT_CDE_4_E ); 
   OS_LYRG_intCtrlRegister( OS_LYRG_INT_CDE_5_E, CDE_DRVP_isrCore , (void*)1);
   OS_LYRG_intCtrlEnable( OS_LYRG_INT_CDE_5_E ); 
   OS_LYRG_intCtrlRegister( OS_LYRG_INT_CDE_6_E, CDE_DRVP_isrCore , (void*)2);
   OS_LYRG_intCtrlEnable( OS_LYRG_INT_CDE_6_E ); 

   OS_LYRG_intCtrlRegister( OS_LYRG_INT_CDE_ABORT_0_E, CDE_DRVP_handleAbortIsr , (void*)0);
   OS_LYRG_intCtrlEnable( OS_LYRG_INT_CDE_ABORT_0_E ); 
   OS_LYRG_intCtrlRegister( OS_LYRG_INT_CDE_ABORT_1_E, CDE_DRVP_handleAbortIsr , (void*)1);
   OS_LYRG_intCtrlEnable( OS_LYRG_INT_CDE_ABORT_1_E ); 
   OS_LYRG_intCtrlRegister( OS_LYRG_INT_CDE_ABORT_2_E, CDE_DRVP_handleAbortIsr , (void*)2);
   OS_LYRG_intCtrlEnable( OS_LYRG_INT_CDE_ABORT_2_E ); 
#endif
   return(CDE__RET_SUCCESS);
}


/****************************************************************************
*
*  Function Name: CDE_DRVG_openChannel
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
ERRG_codeE CDE_DRVG_close(void)
{
   // TODO: Not implemented
   // TODO: remove CB
      return(CDE__RET_SUCCESS);
   
}


/****************************************************************************
*
*  Function Name: CDE_DRVG_updateMemToPeriphsrc
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
void CDE_DRVG_updateMemToPeriphsrc( CDE_DRVG_channelHandleT h, UINT8 *injectBuff )
{
   CDE_SCENARIOSG_setMemToPeriphSrc(h, (UINT32)injectBuff);
}


/****************************************************************************
*
*  Function Name: CDE_DRVG_updateMemToPeriphsrc
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
void CDE_DRVG_updateMemToMemAddress( CDE_DRVG_channelHandleT h, UINT32 dst, UINT32 src )
{
   CDE_SCENARIOSG_setDstSrc(h, dst, src);
}

/****************************************************************************
*
*  Function Name: CDE_DRVG_updateMemToMem
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
ERRG_codeE CDE_DRVG_updateMemToMem( CDE_DRVG_channelHandleT h, UINT32 dst, UINT32 src, UINT32 size )
{
   ERRG_codeE ret = CDE__RET_SUCCESS;

   h->dstParams.address[0] = dst;
   h->srcParams.address[0] = src;
   h->dstParams.buffSize = size;
   if( h->scenario )
   {
      ret = h->scenario( h, NULL );
      if(ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, ret, "CDE DRV failed to update scenario\n");
         return ret;
      }
   }
   else
   {
   

      return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;
   }
   return ret;
}
ERRG_codeE CDE_DRVG_calculateCcrForTransferSize(PL330DMA_CcrU * ccr, UINT8 transferSize, CDE_Addr_IncrementT SrcIncrement, CDE_Addr_IncrementT DestIncrement)
{
   /*For now we can only support a few transfers */
   switch(transferSize)
   {
      case 1: /* 1 Byte */
         ccr->field.srcBurstSize = 0;
         ccr->field.dstBurstSize = 0;
         ccr->field.srcBurstLen = 0;
         ccr->field.dstBurstLen = 0;
         break;
      case 2: /*2 byte transfer */
         ccr->field.srcBurstSize = 1;
         ccr->field.dstBurstSize = 1;
         ccr->field.srcBurstLen = 0;
         ccr->field.dstBurstLen = 0;
         break;
      case 4:/*4 byte transfer */
         ccr->field.srcBurstSize = 2;
         ccr->field.dstBurstSize = 2;
         ccr->field.srcBurstLen = 0;
         ccr->field.dstBurstLen = 0;
         break;
      case 8:/*8 byte transfer */
         ccr->field.srcBurstSize = 2;
         ccr->field.dstBurstSize = 2;
         ccr->field.srcBurstLen = 1;
         ccr->field.dstBurstLen = 1;
         break;
      case 12:
         ccr->field.srcBurstSize = 2;
         ccr->field.dstBurstSize = 2;   
         ccr->field.srcBurstLen = 2;
         ccr->field.dstBurstLen = 2;
         break; 
      case 16 /*16 byte transfer */: 
         ccr->field.srcBurstSize = 2;
         ccr->field.dstBurstSize = 2;   
         ccr->field.srcBurstLen = 3;
         ccr->field.dstBurstLen = 3;
         break;  
      default:
         return (CDE__ERR_SET_CCR_FAIL);
   }
   /*Perform 1 length transfers */
   ccr->field.endianSwapSize =0;
   ccr->field.dstCacheCtrl = 0 ;
   ccr->field.srcCacheCtrl = 0;
   /* We can use dest increment and src increment to avoid extra Mov instructions */
   if(DestIncrement == Address_Increment)
   {
      ccr->field.dstInc = 1;
   }
   else
   {
      ccr->field.dstInc = 0;
   }
   if(SrcIncrement == Address_Increment)
   {
      ccr->field.srcInc = 1;
   }
   else
   {
      ccr->field.srcInc = 0;
   }
   return (CDE__RET_SUCCESS);
}
/****************************************************************************
*
*  Function Name: CDE_DRVG_configureregToMemoryCopyParams
*
*  Description:      configures the register to memory parameters
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
ERRG_codeE CDE_DRVG_configureregToMemoryCopyParams(CDE_DRVG_channelHandleT  channelHandle, CDE_DRVG_regToMemoryCopyConfigParams configParams)
{
   for(int i = 0 ; i < MAX_NUM_REGISTER_ADDRESS && i<configParams.registerAddressCopiesToDo; i ++)
   {
      channelHandle->regToMemoryParams.registerAddresses[i] = configParams.registerAddresses[i];
      for(int j = 0; j < MAX_NUM_BUFFERS_LOOPS; j ++)
      {
         channelHandle->regToMemoryParams.memoryAddresses[j][i] = configParams.memoryAddresses[j][i];
         channelHandle->regToMemoryParams.protobuf_metadata_dstAddress[j] = configParams.protobuf_metadata_dstAddress[j];
      }
      channelHandle->regToMemoryParams.numLoops[i] = configParams.numLoops[i];

      ERRG_codeE ret = CDE_DRVG_calculateCcrForTransferSize(&channelHandle->regToMemoryParams.ccrValuesToUse[i],configParams.transferSizes[i],configParams.srcIncrement[i],configParams.dstIncrement[i]);
      if(ret != CDE__RET_SUCCESS)
      {
         LOGG_PRINT(LOG_ERROR_E,NULL,"Error calculating CCR for register copy:%d",i);
         return ret;
      }
      channelHandle->regToMemoryParams.numLoops[i] = configParams.numLoops[i];   
   }
   channelHandle->regToMemoryParams.waitEventID = configParams.waitEventID;
   channelHandle->regToMemoryParams.giveEventID = configParams.giveEventID;
   channelHandle->regToMemoryParams.waitforEvent = configParams.waitforEvent;
   channelHandle->regToMemoryParams.giveEvent = configParams.giveEvent;
   channelHandle->regToMemoryParams.registerAddressCopiesToDo = configParams.registerAddressCopiesToDo;
   channelHandle->regToMemoryParams.numBuffers = configParams.numBuffers;
   channelHandle->numberMetadataLines = configParams.numberMetadataLines;
   channelHandle->regToMemoryParams.frameBufferOffsetBytes = configParams.frameBufferOffsetBytes;
   channelHandle->regToMemoryParams.entry = configParams.entry;
   LOGG_PRINT(LOG_INFO_E,NULL,"Configured reg->memory copy parameters \n"); 
   return CDE__RET_SUCCESS;
}
/****************************************************************************
*
*  Function Name: CDE_DRVG_configureScenario
*
*  Description:      configure scenario of the channel
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
ERRG_codeE CDE_DRVG_configureScenario( CDE_DRVG_channelHandleT  channelHandle, CDE_DRVG_channelCfgT* channelCfg )
{
   CDE_DRVG_chOpModeE         opMode  =   channelCfg->streamCfg.opMode;
   CDE_DRVG_scenarioTypeE     scenarioType = channelCfg->streamCfg.scenarioType;
   CDE_DRVG_PeriphClientIdU   srcPeriph = (CDE_DRVG_PeriphClientIdU)channelCfg->srcCfg.peripheral;
   CDE_DRVG_PeriphClientIdU   dstPeriph = (CDE_DRVG_PeriphClientIdU)channelCfg->dstCfg.peripheral;
    
   if( ((srcPeriph.field.core < CDE_DRVG_CORE_NUM_MAX_E) && (dstPeriph.field.core < CDE_DRVG_CORE_NUM_MAX_E)) &&
      (srcPeriph.field.core != dstPeriph.field.core) )
   {
      return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                              // TODO: define another error
   }
   LOGG_PRINT(LOG_INFO_E,NULL,"Scenariotype is %u \n", scenarioType);
   switch( scenarioType )
   {
      case CDE_DRVG_SCENARIO_REGISTER_TO_MEMORY_E:
         if(opMode == CDE_DRVG_CONTINIOUS_OP_MODE_E)
         {
            channelHandle->scenario = CDE_SCENARIOSG_registerToMemorywithExtraMetadataInfinite;
         }
         else
         {
            channelHandle->scenario = NULL;  
         }
         break;
      case CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_E:
         
         if( opMode == CDE_DRVG_OME_SHOT_OP_MODE_E )
         {
            channelHandle->scenario = NULL;
         }
         else if( opMode == CDE_DRVG_CONTINIOUS_OP_MODE_E )
         {
            channelHandle->scenario = CDE_SCENARIOSG_periphToMemoryInfinite;
         }
      break;


      case CDE_DRVG_SCENARIO_TYPE_INJECTION_FIXEDSIZE_E:

         if( opMode == CDE_DRVG_OME_SHOT_OP_MODE_E )
         {
            channelHandle->scenario = CDE_SCENARIOSG_memoryToPeriph;
         }
         else if( opMode == CDE_DRVG_CONTINIOUS_OP_MODE_E )
         {
            channelHandle->scenario = NULL;
         }
         
      break;
      case  CDE_DRVG_SCENARIO_TYPE_MEMCOPY_PLAIN2PLAIN_E:      
         if( opMode == CDE_DRVG_OME_SHOT_OP_MODE_E )
         {
            channelHandle->scenario = CDE_SCENARIOSG_memoryToMemoryOnce;
         }
         else if( opMode == CDE_DRVG_CONTINIOUS_OP_MODE_E )
         {
            channelHandle->scenario = NULL;
         }

      break;
      case CDE_DRVG_SCENARIO_TYPE_STREAM_CYCLIC_E:
	  	
 		if( opMode == CDE_DRVG_OME_SHOT_OP_MODE_E )
 		{
 		   channelHandle->scenario = NULL;
 		}
 		else if( opMode == CDE_DRVG_CONTINIOUS_OP_MODE_E )
 		{
 		   channelHandle->scenario = CDE_SCENARIOSG_periphToMemoryCyclic;
 		}
	  break;
      case CDE_DRVG_SCENARIO_TYPE_IIC_STREAM_DDR_LESS:
         if( opMode == CDE_DRVG_OME_SHOT_OP_MODE_E )
			{
				channelHandle->scenario = NULL;
			}
			else if( opMode == CDE_DRVG_CONTINIOUS_OP_MODE_E )
			{
				channelHandle->scenario = CDE_SCENARIOSG_periphToMemoryDdrLess;
				channelHandle->scenarioType = CDE_DRVG_SCENARIO_TYPE_IIC_STREAM_DDR_LESS;
			}
      break;
      case CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_EXT_E:
         if( opMode == CDE_DRVG_OME_SHOT_OP_MODE_E )
         {
            channelHandle->scenario = NULL;
         }
         else if( opMode == CDE_DRVG_CONTINIOUS_OP_MODE_E )
         {
            channelHandle->scenario = CDE_SCENARIOSG_periphToMemoryInfiniteExt;
         }
      break;


      case  CDE_DRVG_SCENARIO_TYPE_INVALID_E:
      case  CDE_DRVG_SCENARIO_TYPE_STREAM_NON_FIXEDSIZE_E:
      case  CDE_DRVG_SCENARIO_TYPE_INJECTION_NON_FIXEDSIZE_E:     
      case  CDE_DRVG_SCENARIO_TYPE_MEMCOPY_CYCLIC2PLAIN_E:     
      case  CDE_DRVG_SCENARIO_TYPE_LAST_E:
      default:
      break;

   }
   
   if( channelHandle->scenario == NULL )
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "CDE_DRVG_confgureScenario NO SCENARIO\n");
      return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                              // TODO: define another error
   }
   else
   {
      return(CDE__RET_SUCCESS);
   }
   
}
/****************************************************************************
*
*  Function Name: 
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

ERRG_codeE CDE_DRVG_setRegisterAddresses( CDE_DRVG_portParamsT* portParams, UINT32 *registerAddresses, UINT8 size )
{
   portParams->buffSize       = 0;
   portParams->numberOfBuffers = 0;
   portParams->loopCount = 0;
   portParams->periphNum      = 0;
   portParams->address[0]     = 0;
      
   return(CDE__RET_SUCCESS);
}
/****************************************************************************
*
*  Function Name: 
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

ERRG_codeE CDE_DRVG_setPeripheralParams( CDE_DRVG_portParamsT* portParams, UINT8 periphNumber, UINT32 periphAddress )
{
   portParams->buffSize       = 0;
   portParams->numberOfBuffers = 0;
   portParams->loopCount = 0;
   portParams->periphNum      = periphNumber;
   portParams->address[0]     = periphAddress;
      
   return(CDE__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: 
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

ERRG_codeE CDE_DRVG_generateChannelProgram(  CDE_DRVG_channelHandleT channelHandle, void* arg )
{
   ERRG_codeE ret = CDE__RET_SUCCESS;

   ret = CDE_DRVP_setCcr( channelHandle );   
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "CDE DRV failed to set CCR\n");
      return ret;
   }

   if( channelHandle->scenario )
   {
      ret = channelHandle->scenario( channelHandle, arg );
      if(ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, ret, "CDE DRV failed to generate scenario\n");
         return ret;
      }
   }
   else
   {
      return CDE__ERR_DRV_CH_CFG_NULL_HANDLE;                              // TODO: define another error
   }
   
   return ret;
}


/****************************************************************************
*
*  Function Name: CDE_DRVG_increaseUnhadledFrameCnt
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
CDE_DRVG_channelStatusE CDE_DRVG_getChannelStatus( CDE_DRVG_channelHandleT channelHandle )
{
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   UINT32 channelStatus =  PL330_DMA_getChannelStatus(channelHandle->coreNum, channelHandle->hwChannelNum);
#else
   UINT32 channelStatus =  PL330_DMA_getChannelStatus(channelHandle->coreNum, channelHandle->channelNum);
#endif

   return (channelStatus & 0x0F);
}


/****************************************************************************
*
*  Function Name: CDE_DRVG_getChannelDestAddress
*
*  Description:      Get destination pointer in cyclic buffer scenario
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
UINT32 CDE_DRVG_getChannelDestAddress( CDE_DRVG_channelHandleT channelHandle )
{
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   UINT32 phyDestAddr = PL330_DMA_getDAR(channelHandle->coreNum, channelHandle->hwChannelNum);
#else
   UINT32 phyDestAddr = PL330_DMA_getDAR(channelHandle->coreNum, channelHandle->channelNum);
#endif
   UINT32 phyBaseAddr = channelHandle->dstParams.address[0];
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
   UINT8 *virtBaseAddr = (channelHandle->buffDescListP[0])->dataP;
#else
   UINT8 *virtBaseAddr = (channelHandle->curBuffDesc)->dataP;
#endif
   UINT32 phyOffset  = phyDestAddr - phyBaseAddr;

   //printf("CDE_DRVG_getChannelDestAddress virt base 0x%08x phy base 0x%08x  phy dest 0x%08x \n",(unsigned int)virtBaseAddr, phyBaseAddr, phyDestAddr );
   if( phyOffset == 0 )
   	{
   		phyOffset = channelHandle->dstParams.buffSize;
   	}
   return (((UINT32)virtBaseAddr) + (phyOffset));
}

ERRG_codeE CDE_DRVG_sendEventChannel( CDE_DRVG_channelHandleT channelH )
{
   ERRG_codeE ret = CDE__RET_SUCCESS;

#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   if( 0xFF == PL330DMAG_SendCommand( NULL, channelH->coreNum, channelH->hwChannelNum, PL330DMA_CMD_DMASEV, PL330DMA_LOOP_EVNT_NUM ))
   {
      LOGG_PRINT(LOG_INFO_E, ret, "CDE DRV Send command failed on core/channel %d/%d\n",channelH->coreNum, channelH->hwChannelNum);
   }
//   else LOGG_PRINT(LOG_INFO_E, ret, "CDE DRV PL330DMA_CMD_DMAGO command DONE on core/channel %d/%d\n",channelH->coreNum, channelH->hwChannelNum);

#else
   if( 0xFF == PL330DMAG_SendCommand( NULL, channelH->coreNum, channelH->channelNum, PL330DMA_CMD_DMASEV, PL330DMA_LOOP_EVNT_NUM ))
   {
      LOGG_PRINT(LOG_INFO_E, ret, "CDE DRV Send command failed on core/channel %d/%d\n",channelH->coreNum, channelH->channelNum);
   }
//   else LOGG_PRINT(LOG_INFO_E, ret, "CDE DRV PL330DMA_CMD_DMAGO command DONE on core/channel %d/%d\n",channelH->coreNum, channelH->channelNum);
#endif

   return(CDE__RET_SUCCESS);
}


/****************************************************************************
*
*  Function Name: CDE_DRVG_isr
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

// TODO: In order to make ISR more effective, pass HW IRQ number to ISR as parameter and use it here

//3   DMA            IRQ_num     USAGE
//3   DMA_0[7:0]           23       frameDone core 0
//3   DMA_1[7:0]           26       frameDone core 1
//3   DMA_2[7:0]           29       frameDone core 2
//3   DMA_0[15:8]       24       channelDone core 0
//3   DMA_1[15:8]       27       channelDone core 1
//3   DMA_2[15:8]       30       channelDone core 2

//3   DMA_0_abort       25       TODO
//3   DMA_1_abort       28       TODO
//3   DMA_2_abort       31       TODO

#if 0
void CDE_DRVG_isr(UINT64 timeStamp)
{
   CDE_DRVG_channelHandleT channelParam;
   UINT32 interruptStatus[CDE_DRVG_CORE_NUM_MAX_E];
   UINT8 core = 0;   
   UINT8 channel = 0;
   UINT32 dummy;
   UINT8 i = 0;


   for( core = 0; core < CDE_DRVG_CORE_NUM_MAX_E; core++ )
   {
      interruptStatus[core] = PL330_DMA_getInterruptStatus(core);                   // snapshot of interrupts
   }

   
   for( core = 0; core < CDE_DRVG_CORE_NUM_MAX_E; core++ )
   {
      if( interruptStatus[core] & 0xFF )                                      // quick    check if any frame done interrupt is pending                      
      {
         for( channel = 0; channel < CDE_DRVG_MAX_CHANNELS_PER_CORE; channel++ )       // loop on frame done interrupt
         {
            if( interruptStatus[core] & (PL330_DMA_FRAME_DONE_INT_BIT << channel) )
            {
               channelParam = CDE_DRVP_getChannelParam( core, channel );

               OS_LYRG_lockMutex(&drvDbP->drvParams.frameCntMutex[core][channel]);

               channelParam->frameDoneCounter++;                              // Count frame done interrups

               OS_LYRG_unlockMutex(&drvDbP->drvParams.frameCntMutex[core][channel]);

               CDE_DRVG_getIncreasedUnhadledFrameCnt( channelParam, &dummy );


               if( drvDbP->drvParams.intCb )
               {
                  LOGG_PRINT(LOG_INFO_E, NULL, "CDE_DRVG_isr: Core = %d, channel = %d, mngrInfo = %p, frame cnt = %d \n", core, channel, channelParam->managerInfo, channelParam->frameDoneCounter );
                  drvDbP->drvParams.intCb(channelParam->managerInfo, CDE_DRVG_INTERRUPT_TYPE_FRAME_DONE_E, timeStamp);
               }

               PL330_DMA_clearFrameDoneInterrupt( core, channel );
            }
         }
      }
   }

   
   for( core = 0; core < CDE_DRVG_CORE_NUM_MAX_E; core++ )
   {
      if( interruptStatus[core] & 0xFF00 )                                    // quick    check if any channel done interrupt is pending                       
      {
         for( channel = 0; channel < CDE_DRVG_MAX_CHANNELS_PER_CORE; channel++ )       // loop on channel done interrupt
         {
            if( interruptStatus[core] & (PL330_DMA_CHANNEL_DONE_INT_BIT << channel) )
            {
               channelParam = CDE_DRVP_getChannelParam( core, channel );

               if( drvDbP->drvParams.intCb )
               {
                  drvDbP->drvParams.intCb(channelParam->managerInfo, CDE_DRVG_INTERRUPT_TYPE_CHANNEL_DONE_E, timeStamp);
               }

               PL330_DMA_clearChannelDoneInterrupt( core, channel );
            }
         }
      }
   }

}

#endif

#ifdef __cplusplus
 }
#endif


