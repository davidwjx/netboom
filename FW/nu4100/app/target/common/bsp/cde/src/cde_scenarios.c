
/****************************************************************************
*
*   FileName: dma_scenarios.c
*
*   Author:  Dima S.
*
*   Date:
*
*   Description:  PL330DMA programms, that implement various scenarios
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
#include "mem_map.h"
#include "log.h"

#include "ppe_mngr.h"
#include "ppe_drv.h"

#include "xml_path.h"
#include "xml_db.h"
#include "assert.h"
#include "cmem.h"
#ifdef CDE_DRVG_METADATA
#define CDE_DRVG_METADATA_AFTER_LOOP
#include "metadata_target_config.h"
#include "metadata_updater.h"
#include "metadata_target_serializer.h"
#include "gme_mngr.h"
#endif

/****************************************************************************
***************         L O C A L       D E F N I T I O N S  ***************
****************************************************************************/
//#define DEBUG_IMP_ISR_HANDLE

/****************************************************************************
***************            L O C A L    T Y P E D E F S      ***************
****************************************************************************/
typedef struct
{
   UINT8 periphNum;
   UINT8 coreNum;
}CDE_SCENARIOS_readInstFieldsT;

typedef union
{
   CDE_SCENARIOS_readInstFieldsT    field;
   UINT16                     word;
}CDE_SCENARIOS_readInstU;


/****************************************************************************
***************       L O C A L         D A T A              ***************
****************************************************************************/
static CDE_SCENARIOS_readInstU reader[PPE_MAX_AXI_READER] =
{
   {.field.coreNum = 0, .field.periphNum = 10},
   {.field.coreNum = 0, .field.periphNum = 11},
   {.field.coreNum = 0, .field.periphNum = 12},
   {.field.coreNum = 0, .field.periphNum = 13},
   {.field.coreNum = 1, .field.periphNum = 10},
   {.field.coreNum = 1, .field.periphNum = 11},
   {.field.coreNum = 1, .field.periphNum = 12},
   {.field.coreNum = 1, .field.periphNum = 13},
   {.field.coreNum = 2, .field.periphNum = 10},
   {.field.coreNum = 2, .field.periphNum = 11},
   {.field.coreNum = 2, .field.periphNum = 12},
   {.field.coreNum = 2, .field.periphNum = 13},
};

static CDE_SCENARIOS_readInstU writer[PPE_MAX_AXI_WRITER] =
{
   {.field.coreNum = 0, .field.periphNum = 17},
   {.field.coreNum = 1, .field.periphNum = 17},
   {.field.coreNum = 2, .field.periphNum = 17},
};



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


/****************************************************************************
***************      L O C A L       F U N C T I O N S       ***************
****************************************************************************/

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

UINT32 CDE_SCENARIOSG_CalculateMetadataSize(CDE_DRVG_channelParamsT* params);



/****************************************************************************
***************   G L O B A L      F U N C T I O N S  **************
****************************************************************************/


/****************************************************************************
*
*  Function Name: CDE_SCENARIOSG_optimizeLoop
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
UINT32 CDE_SCENARIOSG_optimizeLoop( CDE_DRVG_channelParamsT* params )
{
   UINT32 numOfLoopsDiv = 0, numOfLoopsMod = 0;
   UINT32 tempBurstLen = 0, i = 0;

   UINT16 periphId = (params->coreNum << 8) | params->srcParams.periphNum;

   for( i = 0; i < PPE_MAX_AXI_READER; i++ )
   {
        if(periphId == reader[i].word)
        {
           break;
        }
   }

   params->ccr.field.dstBurstLen = 0;

   for( tempBurstLen = 0; tempBurstLen < 16; tempBurstLen++ )
   {
      numOfLoopsDiv = ((params->dstParams.imgWidth) * params->dstParams.imgBpp) / ((tempBurstLen + 1) * (1 << params->ccr.field.dstBurstSize));
      numOfLoopsMod = (((params->dstParams.imgWidth) * params->dstParams.imgBpp) % ((tempBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) / (1 << params->ccr.field.dstBurstSize));
      if(numOfLoopsMod == 0)
      {
         params->ccr.field.dstBurstLen = tempBurstLen;
         params->ccr.field.srcBurstLen = tempBurstLen;
      }
   }


   numOfLoopsDiv = ((params->dstParams.imgWidth) * params->dstParams.imgBpp) / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize));
   numOfLoopsMod = (((params->dstParams.imgWidth) * params->dstParams.imgBpp) % ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) / (1 << params->ccr.field.dstBurstSize));
   PPE_MNGRG_setReaderBurstLen( i, params->ccr.field.srcBurstLen );

   if(numOfLoopsMod != 0)
      printf("\t\t\tCDE_SCENARIOSG_optimizeCcr: OI, GEVALT!!!!!!\n");

   //printf("\t\t\tCDE_SCENARIOSG_optimizeCcr: Found dstBurstLen %d - loops div %d mod %d ccr  0x%x\n", params->ccr.field.dstBurstLen, numOfLoopsDiv, numOfLoopsMod, params->ccr.word);

   return numOfLoopsDiv;
}

/****************************************************************************
*
*  Function Name: CDE_SCENARIOSG_optimizeLoopExt
*
*  Description: Optimize loop for Extended DMA Interleaving
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
UINT32 CDE_SCENARIOSG_optimizeLoopExt( CDE_DRVG_channelParamsT* params, UINT32 numInterleaved)
{
   UINT32 numOfLoopsDiv = 0, numOfLoopsMod = 0;
   UINT32 tempBurstLen = 0, i = 0;

   UINT16 periphId = (params->coreNum << 8) | params->srcParams.periphNum;
   UINT32 imgWidth = params->dstParams.imgWidth / numInterleaved;

   for( i = 0; i < PPE_MAX_AXI_READER; i++ )
   {
        if(periphId == reader[i].word)
        {
           break;
        }
   }

   params->ccr.field.dstBurstLen = 0;

   for( tempBurstLen = 15; tempBurstLen <= 15; tempBurstLen-- )
   {
      numOfLoopsDiv = (imgWidth * params->dstParams.imgBpp) / ((tempBurstLen + 1) * (1 << params->ccr.field.dstBurstSize));
      numOfLoopsMod = ((imgWidth * params->dstParams.imgBpp) % ((tempBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) / (1 << params->ccr.field.dstBurstSize));
      if(numOfLoopsMod == 0)
      {
         params->ccr.field.dstBurstLen = tempBurstLen;
         params->ccr.field.srcBurstLen = tempBurstLen;
         break;
      }
   }

   numOfLoopsDiv = (imgWidth * params->dstParams.imgBpp) / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize));
   numOfLoopsMod = ((imgWidth * params->dstParams.imgBpp) % ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) / (1 << params->ccr.field.dstBurstSize));
   PPE_MNGRG_setReaderBurstLen(i, params->ccr.field.srcBurstLen);

   if(numOfLoopsMod != 0)
      printf("\t\t\tCDE_SCENARIOSG_optimizeCcr: OI, GEVALT!!!!!!\n");

   //printf("\t\t\tCDE_SCENARIOSG_optimizeCcr: Found dstBurstLen %d - loops div %d mod %d ccr  0x%x\n", params->ccr.field.dstBurstLen, numOfLoopsDiv, numOfLoopsMod, params->ccr.word);

   return numOfLoopsDiv;
}

#ifdef CDE_DRVG_VIRTUAL_CHANNELS
static void CDE_SCENARIOSP_saveSetEvent(CDE_DRVG_channelParamsT* params)
{
   if(params->numSetEventFrameDone < MAX_NUM_BUFFERS_LOOPS)
   {
      params->setEventFrameDoneProgramP[params->numSetEventFrameDone] = params->programPtr;
      //LOGG_PRINT(LOG_DEBUG_E, NULL, "save programPtr %p num= %d\n",params->setEventFrameDoneProgramP[params->numSetEventFrameDone],params->numSetEventFrameDone);
      params->numSetEventFrameDone++;
      PL330DMAP_DMASEV( params->programPtr, 0 + PL330_DMA_FRAME_DONE_INT_OFFSET);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "max events reached, core %d channel %d\n",params->coreNum, params->virtualChannelNum);
      assert(0);
   }
}
#endif
static void CDE_SCENARIOSP_saveWaitEvent(CDE_DRVG_channelParamsT* params, UINT8 loopNum, UINT8 index)
{
   if (params->numWaitEvents < MAX_NUM_BUFFERS_LOOPS * CDE_DRVG_MAX_WAIT_EVENTS)
   {
      if (loopNum >= MAX_NUM_BUFFERS_LOOPS || index >= CDE_DRVG_MAX_WAIT_EVENTS)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Invalid loopNum=%d or subChannel index=%d\n", loopNum, index);
         assert(0);
      }
      params->waitEventProgramP[loopNum][index] = params->programPtr;
      params->numWaitEvents++;
      //printf("################Saved WFE instruction at 0x%x for loopNum=%d subChannel=%d\n", params->programPtr, loopNum, index);
      PL330DMAP_DMAWFE(params->programPtr, 0 + PL330_DMA_FRAME_DONE_INT_OFFSET, 1);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "max wait events reached, core %d channel %d\n",params->coreNum, params->virtualChannelNum);
      assert(0);
   }
}

UINT8 CDE_SCENARIOSG_setEvent(CDE_DRVG_channelHandleT channelH)
{
   UINT8 retVal=0;
   UINT8 *tempP;
   UINT32 i = 0;
#ifdef CDE_DRVG_VIRTUAL_CHANNELS

   if( channelH->frameDoneIntEnabled )
   {
      for(i = 0;i < channelH->numSetEventFrameDone;i++)
      {
         //LOGG_PRINT(LOG_DEBUG_E, NULL, "frameDoneIntEnabled %d setEventFrameDoneProgramP[%d] %p channelDoneIntEnabled %d setEventChannelDoneProgramP %p hw core %d ch %d\n",channelH->frameDoneIntEnabled,channelH->setEventFrameDoneProgramP[i],i,channelH->channelDoneIntEnabled,channelH->setEventChannelDoneProgramP, channelH->coreNum, channelH->hwChannelNum);
         tempP = channelH->setEventFrameDoneProgramP[i];
         PL330DMAP_DMASEV( tempP, channelH->hwChannelNum + PL330_DMA_FRAME_DONE_INT_OFFSET);
      }
   }

   if( channelH->channelDoneIntEnabled)
   {
      tempP = channelH->setEventChannelDoneProgramP;
      PL330DMAP_DMASEV( tempP, channelH->hwChannelNum + PL330_DMA_CHANNEL_DONE_INT_OFFSET);
   }

#endif

   return retVal;

}

UINT8 CDE_SCENARIOSG_waitEvent(CDE_DRVG_channelHandleT channelH, UINT8 loopNum, UINT8 index, UINT8 channelNum)
{
    UINT8 retVal = 0;
    UINT8 *instructionAddr;

    if (loopNum < MAX_NUM_BUFFERS_LOOPS && index < CDE_DRVG_MAX_WAIT_EVENTS)
    {
        instructionAddr = channelH->waitEventProgramP[loopNum][index];
        //printf("################Complete WFE instruction at 0x%x for loopNum=%d subChannel=%d\n", instructionAddr, loopNum, index);
        PL330DMAP_DMAWFE(instructionAddr, channelNum + PL330_DMA_FRAME_DONE_INT_OFFSET, 1);
    }

    return retVal;
}

static inline void CDE_SCENARIOSP_updateDARInstruction(CDE_DRVG_extIntParams *extIntParams, UINT8 loopIndex, UINT32 mainChannelPhyAddress)
{
    UINT32 i;
    CDE_DRVG_channelHandleT channelH;
    UINT8 *instructionAddr;

    if (extIntParams == NULL)
       return;
    //Debug
    //printf("Main Channel Buffer Phy Address: 0x%x numSubChannels: %d\n", mainChannelPhyAddress, extIntParams->numSubChannels);
    for (i = 0; i < extIntParams->numSubChannels; i++)
    {
       channelH = extIntParams->subChannelHandles[i];
       if (channelH == NULL)
               break;
       instructionAddr = channelH->dstAddressProgramP[loopIndex];
       //printf("subChannel%d: set DAR instruction at 0x%x\n", i, instructionAddr);
       PL330DMAP_DMAMOV(instructionAddr, PL330DMA_REG_DAR, mainChannelPhyAddress + channelH->dstParams.imgWidth * channelH->dstParams.imgBpp * (i + 1));
       //printf("SubChannel%d: buffer address=0x%x\n", i, mainChannelPhyAddress + channelH->dstParams.imgWidth * channelH->dstParams.imgBpp * (i + 1));
    }
}

#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
#ifdef CDE_DRVG_METADATA
UINT8 CDE_SCENARIOSG_setNewDest(CDE_DRVG_channelHandleT channelH, CDE_DRVG_channelHandleT metadataChannelHandle, MEM_POOLG_bufDescT **buffDescDoneListP, UINT32 *numP, CDE_DRVG_extIntParams *extIntParams)
#else
UINT8 CDE_SCENARIOSG_setNewDest(CDE_DRVG_channelHandleT channelH, MEM_POOLG_bufDescT **buffDescDoneListP, UINT32 *numP, CDE_DRVG_extIntParams *extIntParams)
#endif
#else
UINT8 CDE_SCENARIOSG_setNewDest(CDE_DRVG_channelHandleT channelH, CDE_DRVG_extIntParams *extIntParams)
#endif
{
   UINT8 retVal=0;
   ERRG_codeE                 ret;
   UINT32 phyAddress;
   UINT32 offset = 0;
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
   MEM_POOLG_bufDescT *runnerBuffDescP = NULL;

   //find out where is the DMA program counter in it's code
   UINT8 numBufsComplete, i, j, currentIndex;
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   volatile UINT32 pc = PL330_DMA_getPC(channelH->coreNum, channelH->hwChannelNum);
#else
   volatile UINT32 pc = PL330_DMA_getPC(channelH->coreNum, channelH->channelNum);
#endif
   UINT8 *temp;

   //calculate current index (round down)
   currentIndex = (pc - channelH->dmaDstOffset) / channelH->dmaLoopSize;

   //find out how many buffers the DMA has finished copying - no treatment for wraparound overwrite over the entire program
   if (currentIndex <= channelH->oldIndex)
   {
      numBufsComplete = (channelH->numLoops - channelH->oldIndex) + currentIndex;
   }
   else
   {
      numBufsComplete = currentIndex - channelH->oldIndex;
   }

#ifdef DEBUG_IMP_ISR_HANDLE
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   printf("(%d/%d) numBufsComplete = %d, pc = 0x%x, dmaDstOffset = 0x%x, currentIndex %d, oldIndex = %d dmaLoopSize = %d \n",channelH->coreNum, channelH->hwChannelNum,
            numBufsComplete,pc,channelH->dmaDstOffset,currentIndex,channelH->oldIndex,channelH->dmaLoopSize);
#else
printf("(%d/%d) numBufsComplete = %d, pc = 0x%x, dmaDstOffset = 0x%x, currentIndex %d, oldIndex = %d dmaLoopSize = %d \n",channelH->coreNum, channelH->channelNum,
         numBufsComplete,pc,channelH->dmaDstOffset,currentIndex,channelH->oldIndex,channelH->dmaLoopSize);

#endif
#endif

   for(i = channelH->oldIndex, j = 0; j < numBufsComplete; j++)
   {
      //save the list of buffers for handling
      if (runnerBuffDescP)
      {
         runnerBuffDescP->nextP = channelH->buffDescListP[i];
         runnerBuffDescP = runnerBuffDescP->nextP;
      }
      else
      {
         runnerBuffDescP = channelH->buffDescListP[i];
         *buffDescDoneListP = runnerBuffDescP;
      }

      //update the list with new buffers
      ret = MEM_POOLG_alloc(channelH->memPoolHandle, MEM_POOLG_getBufSize(channelH->memPoolHandle), &channelH->buffDescListP[i]);
      if(ERRG_SUCCEEDED(ret))
      {
         MEM_POOLG_getDataPhyAddr(channelH->buffDescListP[i],&phyAddress);
         offset = channelH->dstParams.offset;
#ifdef MEM_POOLG_BUFF_TRACE
         static char* dmaName = "dma";
         MEM_POOLG_buffTraceUpdateState(dmaName, channelH->buffDescListP[i], __func__);
#endif
      }
      else
      {
         channelH->buffDescListP[i] = channelH->drainingBuffDesc;
         phyAddress = channelH->drainingBuffAddr;
#ifdef MEM_POOLG_BUFF_TRACE
         MEM_POOLG_buffTracePrint(channelH->memPoolHandle, "No free buffers (used drain)");
#endif
      }
      //update the DAR in the program
#ifdef DEBUG_IMP_ISR_HANDLE
      printf("(%d/%d) handle buffers %d: into index %d, bufDesc for upper layers %p, new bufDec %p, address = 0x%x, offset %d\n",channelH->coreNum, channelH->hwChannelNum, j, i,runnerBuffDescP,channelH->buffDescListP[i],phyAddress,offset);
#endif
      temp = channelH->dstAddressProgramP[i];
#ifdef CDE_DRVG_METADATA
      const INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();
      PL330DMAP_DMAMOV( temp, PL330DMA_REG_DAR,(phyAddress + offset+channelH->framebuffer_offset));
      CDE_SCENARIOSP_updateDARInstruction(extIntParams, i, (phyAddress + offset + channelH->framebuffer_offset));
      METADATA_UPDATER_updateMOVInstructions(metadataChannelHandle,modelType,i,phyAddress,offset);
#else
      PL330DMAP_DMAMOV( temp, PL330DMA_REG_DAR,(phyAddress + offset));
      CDE_SCENARIOSP_updateDARInstruction(extIntParams, i, (phyAddress + offset);
#endif
      //wraparound the index if needed
      i = (i + 1) % channelH->numLoops;
   }

   //mark last one as null to mark the end of the list
   if (runnerBuffDescP)
   {
      runnerBuffDescP->nextP = NULL;
   }

   //update oldIndex to the current DMA working index (this buffer was not given to upper layers)
   channelH->oldIndex = currentIndex;
   *numP = numBufsComplete;
#else
   channelH->programPtr    = &channelH->program[channelH->dmaDstOffset + channelH->dmaLoopSize*(channelH->dstParams.loopCount)];
   channelH->doneBuffDesc  = channelH->curBuffDesc;
   channelH->curBuffDesc   = channelH->nextBuffDesc;

   ret = MEM_POOLG_alloc(channelH->memPoolHandle, MEM_POOLG_getBufSize(channelH->memPoolHandle), &channelH->nextBuffDesc);

   if(ERRG_SUCCEEDED(ret))
   {
      MEM_POOLG_getDataPhyAddr(channelH->nextBuffDesc,&phyAddress);
      offset = channelH->dstParams.offset;
      //printf("\t\t\tCDE_SCENARIOSG_setNewDest: Found offset of %d bytes. XY(%d/%d) width (%d)\n", offset, channelH->dstParams.imgStartX, channelH->dstParams.imgStartY, channelH->dstParams.imgWidth);
      //printf ("CDE_SCENARIOSG_setNewDest success to allocate buffer (free = %d, total = %d)\n",MEM_POOLG_getNumFree(channelH->memPoolHandle),MEM_POOLG_getNumBufs(channelH->memPoolHandle));
   }
   else
   {
      if (channelH->drainingBuffDesc)
      {
         channelH->nextBuffDesc = channelH->drainingBuffDesc;
         phyAddress = channelH->drainingBuffAddr;
         //printf ("CDE_SCENARIOSG_setNewDest--------- failed to allocate buffer (free = %d, total = %d)\n",MEM_POOLG_getNumFree(channelH->memPoolHandle),MEM_POOLG_getNumBufs(channelH->memPoolHandle));
      }
   }

   PL330DMAP_DMAMOV( channelH->programPtr, PL330DMA_REG_DAR,(phyAddress + offset));
   channelH->dstParams.loopCount = (channelH->dstParams.loopCount + 1) % MAX_SCENARIO_LOOPS;

   if (channelH->doneBuffDesc != channelH->drainingBuffDesc)
   {
      retVal = 1;
   }
   else
   {
      channelH->drainCtr++;
   }
#endif
   return retVal;
}


/*
This function is called when creating the DMA program.
Its called before writing the mov to destination register instruction.
The program pointer points to where the instruction will be written, and we use it to save this location.
We will need this location inorder to update the DMA program every interrupt handling
*/
static void CDE_SCENARIOSP_setDestAndOffsets(CDE_DRVG_channelParamsT* params,unsigned int address,int loopNum, UINT32 metadatasize)
{
    ERRG_codeE                 ret;
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
    if (loopNum == 0)
    {
        ret = MEM_MAPG_convertVirtualToPhysical((UINT32)params->programPtr,&params->dmaDstOffset);
        if(ERRG_FAILED(ret))
            LOGG_PRINT(LOG_ERROR_E, NULL, "unable to get physical address for %p\n", params->programPtr);
    }
    if (loopNum == 1)
    {
        UINT32 temp = 0;
        ret = MEM_MAPG_convertVirtualToPhysical((UINT32)params->programPtr, &temp);
        if(ERRG_FAILED(ret))
            LOGG_PRINT(LOG_ERROR_E, NULL, "unable to get physical address for %p\n", params->programPtr);
        params->dmaLoopSize = temp - params->dmaDstOffset;
    }

   /*
         Save the location of the instruction, we will need it when we update new buffers for the DMA.
         Keep the physical address, to ease the calculation in the ISR handling
      */
   params->dstAddressProgramP[loopNum] = params->programPtr;
   //printf("CDE_SCENARIOSP_setDestAndOffsets (setup): loop %d address of mov to DAR instruction %p, offset = %d\n",loopNum,params->dstAddressProgramP[loopNum],params->dstParams.offset);

   /* Set the start of the destination address according to the x,y for 2d copy */
   PL330DMAP_DMAMOV( params->programPtr, PL330DMA_REG_DAR, (address + params->dstParams.offset));
#else
   if (params->dmaDstOffset == 0)
   {
      params->dmaDstOffset =  (UINT32)params->programPtr - (UINT32)params->program;
   }
   else if (params->dmaLoopSize == 0)
   {
      params->dmaLoopSize = (UINT32)params->programPtr -(UINT32)params->program - (UINT32)params->dmaDstOffset;
   }

   /* Set the start of the destination address according to the x,y for 2d copy */
   PL330DMAP_DMAMOV( params->programPtr, PL330DMA_REG_DAR, (address + params->dstParams.offset));
#endif
}


void CDE_SCENARIOSG_setDstSrc(CDE_DRVG_channelParamsT* params,unsigned int dstAddress, unsigned int srcAddress)
{
//printf("set dst: prog = 0x%x, offset = 0x%x, final = 0x%x. dstAddress = 0x%x\n",params->program,params->dstParams.addressOffset,&params->program[params->dstParams.addressOffset],dstAddress);
   params->dstParams.address[0] = dstAddress;
   params->programPtr = &params->program[params->dstParams.addressOffset];
   PL330DMAP_DMAMOV( params->programPtr, PL330DMA_REG_DAR, dstAddress);
//printf("set src: prog = 0x%x, offset = 0x%x, final = 0x%x. srcAddress = 0x%x\n",params->program,params->srcParams.addressOffset,&params->program[params->srcParams.addressOffset],srcAddress);
   params->srcParams.address[0] = srcAddress;
   params->programPtr = &params->program[params->srcParams.addressOffset];
   PL330DMAP_DMAMOV( params->programPtr, PL330DMA_REG_SAR, srcAddress);
}
/****************************************************************************
*
*  Function Name: CDE_SCENARIOSG_CalculateMetadataSize
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*  Metadatasize in bytes
*  Context: 
*
****************************************************************************/

UINT32 CDE_SCENARIOSG_CalculateMetadataSize(CDE_DRVG_channelParamsT* params)
{
   #ifdef CDE_DRVG_METADATA
   return MEM_POOLG_getBufSize(params->metadata_memPoolHandle);
   #endif
   return 0;   /*No metadata */
}

/****************************************************************************
 *
 *  Function Name: CDE_SCENARIOSG_PerformMetaDataCopy
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
ERRG_codeE CDE_SCENARIOSG_PerformMetaDataCopy(CDE_DRVG_channelParamsT *params, int loopNum, UINT32 DestinationAddress)
{
#ifdef CDE_DRVG_METADATA

   /*
      WARNING: A big trap with this DMA controller is that backwards_jump is only 8 bits even though the PC is 16 bits,
      this means we cannot make loops longer than 256 instructions.
      The loops are currently unrolled so that the GP can track where it is in the DMA loops and workout where which transfers have completed.
      So my idea of add a LP to reduce the number of instructions won't work without a heavy refactor of code.

      We need to do the following if we have metadata:
      1. Calculate the number of loops we need for metadata copy
      2. Change the DAR register
      3. Change the CCR configuration so the src/destination transfer sizes are the same
      4. Change the SAR register
      5. DMA into the DAR
      */
   UINT32 metadata_src_address; /*Src address for the metadata */
   UINT32 ret = MEM_POOLG_getDataPhyAddr(params->metadata_buffDescListP[loopNum], &metadata_src_address);
   PL330DMA_CcrU old_ccr = params->ccr;
   PL330DMA_CcrU new_ccr = params->ccr;
   new_ccr.field.srcInc = 1; /*Turn on source increment */
   new_ccr.field.dstInc = 1; /*Turn on source increment */
   new_ccr.field.srcBurstSize = 4;
   new_ccr.field.dstBurstSize = 4;
   new_ccr.field.srcBurstLen = 0; /*16*1 byte transfers, we aren't attempting to rely on burst mode transfers and are instead using single AXI transfers*/
   new_ccr.field.dstBurstLen = 0;
   new_ccr.field.dstCacheCtrl = 0;
   new_ccr.field.srcCacheCtrl = 0;

   const UINT32 metadatasize = MEM_POOLG_getBufSize(params->metadata_memPoolHandle);
   const UINT32 burst_bytes = (1 << new_ccr.field.srcBurstSize);        /* burstSize bytes =  1<< ccr.srcBurstSize */
   const UINT32 burst_length = new_ccr.field.srcBurstLen + 1;           /* Burst size length is = ccr.srcBurstSize */
   const UINT32 total_burst_size_bytes = burst_bytes * burst_length;    /* Total burst size is product of burstSize_Bytes and burst_length */
   const UINT32 Number_Loops = (metadatasize / total_burst_size_bytes); /* Number of loops needed for DMA copy */
   const UINT32 remainder = metadatasize % total_burst_size_bytes;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "DMA burst size is %lu bytes repeated %lu times.   \n", burst_bytes, burst_length);
   LOGG_PRINT(LOG_DEBUG_E, NULL, "DMA burst size product is : %lu bytes and metadatasize is %lu, so we need to perform %lu loops  \n", total_burst_size_bytes, metadatasize, Number_Loops);
   /*Task 1 */
   if (Number_Loops > 255)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Number_Loops > 255. Metadata cannot be copied into the width of a frame");
      return 1;
   }
   if (remainder != 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Metadatasize modulo total_burst_size_bytes !=0 so we cannot allow prepending of metadata \n");
      return 1;
   }
   if (ERRG_SUCCEEDED(ret))
   {
      if (DestinationAddress > 0)
      {
         /* Task 2 */
         LOGG_PRINT(LOG_DEBUG_E, NULL, "Memory destination for index %d is physical address :%X \n", loopNum, DestinationAddress);
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
         if (loopNum == 0)
         {
            MEM_MAPG_convertVirtualToPhysical((UINT32)params->programPtr,&params->dmaDstOffset);
         }
         if (loopNum == 1)
         {
            UINT32 temp = 0;
            MEM_MAPG_convertVirtualToPhysical((UINT32)params->programPtr, &temp);
            params->dmaLoopSize = temp - params->dmaDstOffset;
         }
#endif
         params->dstAddressProgramP[loopNum] = params->programPtr;
         PL330DMAP_DMAMOV(params->programPtr, PL330DMA_REG_DAR, DestinationAddress);
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Memory address not provided for protobuf metadata location \n");
      }
      /*Task 3 */
      PL330DMAP_DMAMOV(params->programPtr, PL330DMA_REG_CCR, new_ccr.word);
      params->meatadata_srcAddressProgramP[loopNum] = params->programPtr;
      /*Task 4 */
      PL330DMAP_DMAMOV(params->programPtr, PL330DMA_REG_SAR, metadata_src_address);
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Metadata source for index %d is physical address 0x%X \n", loopNum, metadata_src_address);
      /*Task 5 Copy metadasize_bytes into the location specified within DAR */
      PL330DMAP_DMALP(params->programPtr, Number_Loops - 1);
         PL330DMAP_DMALD(params->programPtr);
         PL330DMAP_DMAST(params->programPtr);
      PL330DMAP_DMALPEND(params->programPtr);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Could not get physical address for metadata \n");
   }
#endif
   return CDE__RET_SUCCESS;
}
#ifdef CDE_DRVG_METADATA
/****************************************************************************
*
*  Function Name: CDE_SCENARIOSG_registerToMemorywithExtraMetadataInfinite
*
*  Description:   Copies from MAX_NUM_REGISTER_ADDRESS amount of registers into the DAR and adds offsets if required
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
ERRG_codeE CDE_SCENARIOSG_registerToMemorywithExtraMetadataInfinite(CDE_DRVG_channelParamsT *params, void *arg)
{
   dmaChannelProg dmaProg = params->program;
   ERRG_codeE ret;
   UINT32 ccr_word =  params->regToMemoryParams.ccrValuesToUse[0].word;

   (void)arg;

   params->dmaDstOffset = 0;
   params->dmaLoopSize = 0;
   params->programPtr = PL330DMAP_ConstrInit(dmaProg);

   /*Start an infinite loop */
   PL330DMAP_DMALPFE(params->programPtr);
   LOGG_PRINT(LOG_DEBUG_E, NULL, "Register to memory addreesses, numbers %u \n", params->regToMemoryParams.numBuffers);
   for (int j = 0; j < params->regToMemoryParams.numBuffers; j++)
   {
      /*Each Register address (value loaded from registerAddresses) is loaded into the SAR and then copied into memory (value loaded from memoryAddresses)
         Task 1: Wait for an event ID to happen
         Task 2: Copy the protobuf metadata over
         Task 3: Setup the destination address
         Task 4: Setup the CCR
         Task 5: Setup the source address
         Task 6: Perform the store and load required
         Task 7: Signal that the event ID has finished
      */
      UINT32 metadatasize = MEM_POOLG_getBufSize(params->metadata_memPoolHandle);
      params->regToMemoryParams.dstAddressProgramP_MetaData[j] = NULL;
      params->dstAddressProgramP[j] = NULL;
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Allocating (%lu bytes) memory  for metadata \n", metadatasize);
      ret = MEM_POOLG_alloc(params->metadata_memPoolHandle, metadatasize, &params->metadata_buffDescListP[j]);
      if (ERRG_SUCCEEDED(ret))
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "Allocated memory pool for metadata, setting up metapool buffers \n");
         memset(params->metadata_buffDescListP[j]->dataP, 0, metadatasize);    /*Clear the buffer */
         INU_Metadata_T metadata = ZEROED_OUT_METADATA; /*Clear the metadata structure */
                                                                               // UINT32 * bufferNumber = (UINT32 * )&params->metadata_buffDescListP[j]->dataP[4];
         UINT64 currentTs;
         OS_LYRG_getTimeNsec(&currentTs);
         metadata.protobuf_packet.metadatacounter = j;
         metadata.protobuf_packet.protocol_version = inu_metadata__getProtocol_Version();
         metadata.protobuf_packet.state_count = 0;
         metadata.protobuf_packet.timestamp_ns.nanoseconds = currentTs;
         metadata.protobuf_packet.has_timestamp_ns = true;
         /*These two values are overwritten by the DMAC */
         metadata.timestamp = 0xFF;
         metadata.frameID = j;
         /*Serialization counter which increments everytime METADATA_target_serialize is called */
         metadata.serialization_counter=  0;
         /*Send the frame buffer offset */
         metadata.framebuffer_offset = params->regToMemoryParams.frameBufferOffsetBytes;
         /* Set the source sensor - only need to update state[0] */
         if(params->regToMemoryParams.entry)
         {
            metadata.protobuf_packet.sensors_count = 0;
            if(params->regToMemoryParams.entry->sensors[0].valid)
            {
               LOGG_PRINT(LOG_DEBUG_E,NULL,"Initialized Sensor 0 Metadata \n");
               metadata.protobuf_packet.sensors[0] = params->regToMemoryParams.entry->sensors[0].name;
               metadata.protobuf_packet.sensors_count++;
            }
            if(params->regToMemoryParams.entry->sensors[1].valid)
            {
               LOGG_PRINT(LOG_DEBUG_E,NULL,"Initialized Sensor 1 Metadata \n");
               metadata.protobuf_packet.sensors[1] = params->regToMemoryParams.entry->sensors[1].name;
               metadata.protobuf_packet.sensors_count++;
            }
         }
         else
         {
            LOGG_PRINT(LOG_WARN_E,NULL,"No metadata entry has been found whilst trying to initialize the Sensor Metadata");
         }
         METADATA_target_serialize(params->metadata_buffDescListP[j],&metadata,metadatasize);
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to allocate memory pool for metadata \n");
      }
      if (params->regToMemoryParams.waitforEvent == WAIT)
      {
         /*Task 1 */
         LOGG_PRINT(LOG_DEBUG_E, NULL, "Wait for event ID:%u \n", params->regToMemoryParams.waitEventID);
         PL330DMAP_DMAWFE(params->programPtr, params->regToMemoryParams.waitEventID, 1); 
      }
      /*Task 2 */
      CDE_SCENARIOSG_PerformMetaDataCopy(params, j, params->regToMemoryParams.protobuf_metadata_dstAddress[j]);
      /*Task 3 - We only allow memory address to be set once per loop and not per register*/
       if (params->regToMemoryParams.memoryAddresses[j][0] > 0)
      {
            LOGG_PRINT(LOG_DEBUG_E, NULL, "Reg->Memory copy: Memory address:%X \n", params->regToMemoryParams.memoryAddresses[j][0]);
            params->regToMemoryParams.dstAddressProgramP_MetaData[j] = params->programPtr;                           /*Store PC for updating destination */
            PL330DMAP_DMAMOV(params->programPtr, PL330DMA_REG_DAR, params->regToMemoryParams.memoryAddresses[j][0]); 
      }
      for (int i = 0; i < params->regToMemoryParams.registerAddressCopiesToDo; i++)
      {
         /*Task 4 */
         if(i > 0)
         {
            /*Only update the CCR if it needs to change, this will reduce the program size*/
            if(ccr_word != params->regToMemoryParams.ccrValuesToUse[i].word)
            {
               LOGG_PRINT(LOG_DEBUG_E, NULL, "CCR being loaded for %lu, previous CCR %lu \n", params->regToMemoryParams.ccrValuesToUse[i].word,ccr_word);
               PL330DMAP_DMAMOV(params->programPtr, PL330DMA_REG_CCR, params->regToMemoryParams.ccrValuesToUse[i].word); 
            }
            else
            {
               LOGG_PRINT(LOG_DEBUG_E, NULL, "Optimization enabled for CCR MOV for register address 0x%X",params->regToMemoryParams.registerAddresses[i]);
            }
         }
         else
         {
            /*Always update the CCR for the first register copy */
            PL330DMAP_DMAMOV(params->programPtr, PL330DMA_REG_CCR, params->regToMemoryParams.ccrValuesToUse[i].word); 
         }
         /*Store the CCR so we can check if we need to update the CCR */
         ccr_word = params->regToMemoryParams.ccrValuesToUse[i].word; 
         if (params->regToMemoryParams.registerAddresses[i] > 0)
         {
            /*Task 5 */
            PL330DMAP_DMAMOV(params->programPtr, PL330DMA_REG_SAR, params->regToMemoryParams.registerAddresses[i]); 
         }
         /* Task 6:
               (TODO : We could add a loop here, but I expect we will only have a couple of words to copy
               and in which case we can just add more register copies)
         */
         PL330DMAP_DMALD(params->programPtr);
         PL330DMAP_DMAST(params->programPtr);
      }
      /*Task 7 */
      if (params->regToMemoryParams.giveEvent == GIVE)
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "Give event ID:%u \n", params->regToMemoryParams.giveEventID);
         PL330DMAP_DMASEV(params->programPtr, params->regToMemoryParams.giveEventID); /*Task 6*/
      }
   }
   PL330DMAP_DMALPEND(params->programPtr);
   const UINT32 programSize = params->programPtr - dmaProg;
   while ((UINT32)params->programPtr % 4)
   {
      PL330DMAP_DMANOP(params->programPtr);
   }

   if (programSize > 255)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Loop size too big (%lu bytes) for CDE Register to memory metadata scenario \n", programSize);
      return 1; // TODO: return program buffer overflow
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Program size for CDE Register to memory Metadata scenario is %d \n", programSize);
   }
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   PL330DMAP_PrintProgToRegister(params->coreNum, params->hwChannelNum, params->programPtr, dmaProg, params->dstParams.buffSize / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)));
#else
   PL330DMAP_PrintProgToRegister(params->coreNum, params->channelNum, params->programPtr, dmaProg, params->dstParams.buffSize / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)));
#endif
   return CDE__RET_SUCCESS;
}
ERRG_codeE CDE_SCENARIOSG_sendTransferCompleteEvent( CDE_DRVG_channelParamsT* params )
{
   if(params->regToMemoryParams.giveEvent == GIVE)
   {
      LOGG_PRINT(LOG_DEBUG_E,NULL,"Transfer complete event ID is being given with ID: %u \n", params->regToMemoryParams.giveEventID);
      PL330DMAP_DMASEV(params->programPtr,params->regToMemoryParams.giveEventID);
   }
   return CDE__RET_SUCCESS;
}
ERRG_codeE CDE_SCENARIOSG_waitTransferCompleteEvent( CDE_DRVG_channelParamsT* params )
{
   if(params->regToMemoryParams.waitforEvent == WAIT)
   {
      LOGG_PRINT(LOG_DEBUG_E,NULL,"Transfer complete wait is using wait ID: %u \n", params->regToMemoryParams.waitEventID);
      PL330DMAP_DMAWFE(params->programPtr,params->regToMemoryParams.waitEventID,1);
   }
   return CDE__RET_SUCCESS;
}
#endif

ERRG_codeE CDE_SCENARIOSG_initiateMetadataCopy(CDE_DRVG_channelParamsT* params)
{
#ifdef CDE_DRVG_METADATA_AFTER_LOOP
/*I have found out that the Metadata copy can cause an SLU overflow.
so as a presentative measure I now start the metadata transfer as soon as some CiiF data has been received and then the metadata copy can happen
whilst the frame is being received. The DMAC is happy serving each Metadata thread in a round-robin fashion 
and we will now have a lot more time for the metadata copy to happen without affecting the main DMA thread 
*/
   PL330DMAP_DMAWFP( params->programPtr ,params->srcParams.periphNum, PL330DMA_REQ_PERIPH );
   CDE_SCENARIOSG_sendTransferCompleteEvent(params);
   /*We need to do at least one load/store otherwise the next PL330DMAP_DMAWFP will stall the DMAC thread*/
   PL330DMAP_DMALD( params->programPtr); 
   PL330DMAP_DMAST( params->programPtr );

#endif
return CDE__RET_SUCCESS;
}

ERRG_codeE CDE_SCENARIOSG_waitForMetadataCopy (CDE_DRVG_channelParamsT* params)
{
#ifdef CDE_DRVG_METADATA
#ifdef CDE_DRVG_METADATA_AFTER_LOOP
   /*Wait for second thread to finish transfers */
   CDE_SCENARIOSG_waitTransferCompleteEvent(params);
#else
   PL330DMAP_DMAWFP( params->programPtr ,params->srcParams.periphNum, PL330DMA_REQ_PERIPH );
   CDE_SCENARIOSG_sendTransferCompleteEvent(params);
   CDE_SCENARIOSG_waitTransferCompleteEvent(params);
   PL330DMAP_DMALDB( params->programPtr); 
   PL330DMAP_DMASTB( params->programPtr );
   PL330DMAP_DMALDS( params->programPtr );
   PL330DMAP_DMASTS( params->programPtr );
#endif
#endif
return CDE__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: CDE_SCENARIOSG_periphToMemoryInfinite
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
ERRG_codeE CDE_SCENARIOSG_periphToMemoryInfinite( CDE_DRVG_channelParamsT* params, void* arg )
{
   dmaChannelProg       dmaProg = params->program;
   ERRG_codeE           ret;
   UINT32            phyAddress,  i = 0;
#ifndef CDE_DRVG_IMPROVED_ISR_HANDLING
   MEM_POOLG_bufDescT*  bufDescP = NULL;
#endif
   UINT32            numOfLoopsDiv = 1;
   UINT32            metadataDestination = 0;
   UINT32            strideRequired = ( (params->dstParams.stride > params->dstParams.imgWidth) ? (1) : (0) ); // indication if stride is required
   (void)arg;

   params->dmaDstOffset = 0;
   params->dmaLoopSize = 0;
   params->programPtr = PL330DMAP_ConstrInit(dmaProg);

   if(strideRequired)
   {
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   printf("Found stride requirement for core %d / channel %d \n", params->coreNum, params->hwChannelNum);
#else
   printf("Found stride requirement for core %d / channel %d \n", params->coreNum, params->channelNum);
#endif
      numOfLoopsDiv = CDE_SCENARIOSG_optimizeLoop( params );
   }

   PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_CCR, params->ccr.word);
   PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_SAR, params->srcParams.address[0]);

   PL330DMAP_DMALPFE( params->programPtr );

   for( i = 0; i < params->numLoops; i++ )
   {
      PL330DMAP_DMAFLUSHP( params->programPtr ,params->srcParams.periphNum );
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
      ret = MEM_POOLG_alloc(params->memPoolHandle, MEM_POOLG_getBufSize(params->memPoolHandle), &params->buffDescListP[i]);
#ifdef DEBUG_IMP_ISR_HANDLE
      printf("CDE_SCENARIOSP_setDestAndOffsets (setup2): loop %d bufDesc %p\n",i,params->buffDescListP[i]);
#endif
#else
      ret = MEM_POOLG_alloc(params->memPoolHandle, MEM_POOLG_getBufSize(params->memPoolHandle), &bufDescP);
#endif
      if(ERRG_SUCCEEDED(ret))
      {
#ifndef CDE_DRVG_IMPROVED_ISR_HANDLING
         if ( i == 0)
         {
            params->curBuffDesc  = bufDescP;
         }
         else
         {
            params->nextBuffDesc = bufDescP;
         }
         MEM_POOLG_getDataPhyAddr(bufDescP,&phyAddress);
#else
         MEM_POOLG_getDataPhyAddr(params->buffDescListP[i],&phyAddress);
#endif
#ifdef CDE_DRVG_METADATA
         CDE_SCENARIOSP_setDestAndOffsets(params, phyAddress+params->framebuffer_offset, i,0);
#else
         CDE_SCENARIOSP_setDestAndOffsets(params, phyAddress, i,0);
#endif

      }
      else
      {
         return 1;
      }

      metadataDestination = phyAddress; /*Set the metadata destination to the start of the frame buffer */
      if(strideRequired)
      {
            /*Perform the metadata copy if it's enabled */
            //CDE_SCENARIOSG_PerformMetaDataCopy(params,i,phyAddress);
#ifdef CDE_DRVG_METADATA
#ifndef CDE_DRVG_METADATA_AFTER_LOOP
      CDE_SCENARIOSG_waitForMetadataCopy(params);
#else
      CDE_SCENARIOSG_initiateMetadataCopy(params);
#endif
#endif
     PL330DMAP_DMALPFE( params->programPtr );
            PL330DMAP_DMALP( params->programPtr, (numOfLoopsDiv - 1));
               PL330DMAP_DMAWFP( params->programPtr ,params->srcParams.periphNum, PL330DMA_REQ_PERIPH);

               PL330DMAP_DMALDB( params->programPtr);
               PL330DMAP_DMASTB( params->programPtr );

            PL330DMAP_DMALPEND( params->programPtr );

            PL330DMAP_DMAADDH( params->programPtr, PL330DMA_REG_DAR, ((params->dstParams.stride - params->dstParams.imgWidth) * params->dstParams.imgBpp) );
         PL330DMAP_DMALPEND( params->programPtr );
#ifdef CDE_DRVG_METADATA
#ifdef CDE_DRVG_METADATA_AFTER_LOOP
         CDE_SCENARIOSG_waitForMetadataCopy(params);
#endif
#endif
      }
      else  // no strride is required - standard channel
      {
#ifdef CDE_DRVG_METADATA
#ifndef CDE_DRVG_METADATA_AFTER_LOOP
         CDE_SCENARIOSG_waitForMetadataCopy(params);
#else
         CDE_SCENARIOSG_initiateMetadataCopy(params);
#endif
#endif
         PL330DMAP_DMALPFE( params->programPtr );
            PL330DMAP_DMAWFP( params->programPtr ,params->srcParams.periphNum, PL330DMA_REQ_PERIPH );

            PL330DMAP_DMALDB( params->programPtr);
            PL330DMAP_DMASTB( params->programPtr );
            PL330DMAP_DMALDS( params->programPtr );
            PL330DMAP_DMASTS( params->programPtr );

         PL330DMAP_DMALPEND( params->programPtr );
#ifdef CDE_DRVG_METADATA
#ifdef CDE_DRVG_METADATA_AFTER_LOOP
         CDE_SCENARIOSG_waitForMetadataCopy(params);
#endif
#endif
      }

      //1. clean instruction pipeline
      PL330DMAP_DMARMB( params->programPtr );
      PL330DMAP_DMAWMB( params->programPtr );
      //2. invalidate the instruction cache so new dst address is loaded.Must use unique channel number to not affet other channels.
      PL330DMAP_DMASEV( params->programPtr ,PL330DMA_LOOP_EVNT_NUM + params->virtualChannelNum);
      PL330DMAP_DMAWFE( params->programPtr ,PL330DMA_LOOP_EVNT_NUM + params->virtualChannelNum,1);

      if( params->frameDoneIntEnabled )
      {
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
         CDE_SCENARIOSP_saveSetEvent(params);
#else
         PL330DMAP_DMASEV( params->programPtr, params->channelNum + PL330_DMA_FRAME_DONE_INT_OFFSET);
#endif
      }
   }
   PL330DMAP_DMALPEND( params->programPtr );

   if( params->channelDoneIntEnabled )
   {
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      params->setEventChannelDoneProgramP = params->programPtr;
      PL330DMAP_DMASEV( params->programPtr ,params->hwChannelNum + PL330_DMA_CHANNEL_DONE_INT_OFFSET );
#else
      PL330DMAP_DMASEV( params->programPtr ,params->channelNum + PL330_DMA_CHANNEL_DONE_INT_OFFSET );
#endif
   }

   PL330DMAP_DMAEND( params->programPtr );

   while( (UINT32)params->programPtr % 4 )
   {
      PL330DMAP_DMANOP( params->programPtr );
   }
   const UINT32 programSize = params->programPtr - dmaProg;
   if( programSize > CDE_DRVG_CH_PROGRAMM_MAX_SIZE )
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Program size too big \n");
      return 1;                                                // TODO: return program buffer overflow
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E,NULL,"Program size for Fixed Infinite scenario is %d \n", programSize);
   }
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   PL330DMAP_PrintProgToRegister(params->coreNum, params->hwChannelNum, params->programPtr, dmaProg, params->dstParams.buffSize / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) );
#else
   PL330DMAP_PrintProgToRegister(params->coreNum, params->channelNum, params->programPtr, dmaProg, params->dstParams.buffSize / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) );
#endif

   return CDE__RET_SUCCESS;

}

/****************************************************************************
*
*  Function Name: CDE_SCENARIOSG_periphToMemoryInfiniteExtSubChannel
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
ERRG_codeE CDE_SCENARIOSG_periphToMemoryInfiniteExtSubChannel(CDE_DRVG_channelParamsT* params, UINT32 numOfLoopsDiv, UINT32 *dstPhyAddress,UINT32 frameOffset, UINT32 skipBytesPerLine, bool isLastChannel)
{
   ERRG_codeE    ret;
   UINT32        phyAddress;
   UINT32        i = 0;
   UINT32            strideRequired = ( (params->dstParams.stride > params->dstParams.imgWidth) ? (1) : (0) ); // indication if stride is required
   dmaChannelProg       dmaProg = params->program;
   UINT16 periphId = (params->coreNum << 8) | params->srcParams.periphNum;

   //Modify AXI READER burst len
   for( i = 0; i < PPE_MAX_AXI_READER; i++ )
   {
        if(periphId == reader[i].word)
        {
           PPE_MNGRG_setReaderBurstLen(i, params->ccr.field.srcBurstLen);
           break;
        }
   }

   params->dmaDstOffset = 0;
   params->dmaLoopSize = 0;
   params->programPtr = PL330DMAP_ConstrInit(dmaProg);

   PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_CCR, params->ccr.word);
   PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_SAR, params->srcParams.address[0]);

   PL330DMAP_DMALPFE( params->programPtr );

   for( i = 0; i < params->numLoops; i++ )
   {
      PL330DMAP_DMAFLUSHP( params->programPtr ,params->srcParams.periphNum );
      phyAddress = dstPhyAddress[i] + frameOffset;
      CDE_SCENARIOSP_setDestAndOffsets(params, phyAddress, i,0);

      if(isLastChannel && strideRequired)
      {
         PL330DMAP_DMALPFE( params->programPtr );
            PL330DMAP_DMALP( params->programPtr, (numOfLoopsDiv - 1));
               PL330DMAP_DMAWFP( params->programPtr ,params->srcParams.periphNum, PL330DMA_REQ_PERIPH);
               PL330DMAP_DMALDB( params->programPtr);
               PL330DMAP_DMASTB( params->programPtr );
            PL330DMAP_DMALPEND( params->programPtr );
            PL330DMAP_DMAADDH( params->programPtr, PL330DMA_REG_DAR, ((params->dstParams.stride - params->dstParams.imgWidth) * params->dstParams.imgBpp) + skipBytesPerLine);
         PL330DMAP_DMALPEND( params->programPtr );
       }
       else  // no strride is required - standard channel
       {
         PL330DMAP_DMALPFE( params->programPtr );
            PL330DMAP_DMALP( params->programPtr, (numOfLoopsDiv - 1));
               PL330DMAP_DMAWFP( params->programPtr ,params->srcParams.periphNum, PL330DMA_REQ_PERIPH );
               PL330DMAP_DMALDB( params->programPtr);
               PL330DMAP_DMASTB( params->programPtr );
            PL330DMAP_DMALPEND( params->programPtr );
            PL330DMAP_DMAADDH( params->programPtr, PL330DMA_REG_DAR, skipBytesPerLine);
         PL330DMAP_DMALPEND( params->programPtr );
      }
      //1. clean instruction pipeline
      PL330DMAP_DMARMB( params->programPtr );
      PL330DMAP_DMAWMB( params->programPtr );
      //2. invalidate the instruction cache so new dst address is loaded.Must use unique channel number to not affet other channels.
      PL330DMAP_DMASEV( params->programPtr ,PL330DMA_LOOP_EVNT_NUM + params->virtualChannelNum);
      PL330DMAP_DMAWFE( params->programPtr ,PL330DMA_LOOP_EVNT_NUM + params->virtualChannelNum,1);

#ifdef CDE_DRVG_VIRTUAL_CHANNELS
         //Need to save the programPtr to make CDE_DRVG_startChannel() to modify the hwChannelNum
         //Even if frameDoneIntEnabled is false
         CDE_SCENARIOSP_saveSetEvent(params);
         //Debug, set event for main channel
         //PL330DMAP_DMASEV(params->programPtr, 1 + PL330_DMA_FRAME_DONE_INT_OFFSET);
#else
         PL330DMAP_DMASEV( params->programPtr, params->channelNum + PL330_DMA_FRAME_DONE_INT_OFFSET);
#endif
   }
   PL330DMAP_DMALPEND( params->programPtr );
   PL330DMAP_DMAEND( params->programPtr );

   //Debug setevent
   //CDE_SCENARIOSP_saveSetEvent(params);

   while( (UINT32)params->programPtr % 4 )
   {
      PL330DMAP_DMANOP( params->programPtr );
   }
   const UINT32 programSize = params->programPtr - dmaProg;
   if( programSize > CDE_DRVG_CH_PROGRAMM_MAX_SIZE )
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Program size too big \n");
      return 1;                                                // TODO: return program buffer overflow
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E,NULL,"Program size for Fixed Infinite scenario is %d \n", programSize);
   }
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   PL330DMAP_PrintProgToRegister(params->coreNum, params->hwChannelNum, params->programPtr, dmaProg, params->dstParams.buffSize / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) );
#else
   PL330DMAP_PrintProgToRegister(params->coreNum, params->channelNum, params->programPtr, dmaProg, params->dstParams.buffSize / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) );
#endif

   return CDE__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: CDE_SCENARIOSG_periphToMemoryInfiniteExt
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
static void DMA_DumpProgram(UINT8 *programPtr, UINT32 phyAddr, UINT32 size, char *tips)
{
   printf("================%s(%d bytes)==============\n", tips, size);
   for (UINT32 i = 0; i < size; i++) {
           if (i % 8 == 0)
           {
              printf("\n[%08x]:\t", phyAddr);
              phyAddr += 8;
           }
           printf("%02x\t", programPtr[i]);
   }
   printf("\n");
}

ERRG_codeE CDE_SCENARIOSG_periphToMemoryInfiniteExt( CDE_DRVG_channelParamsT* params, void* arg )
{
   dmaChannelProg       dmaProg = params->program;
   ERRG_codeE           ret;
   UINT32            phyAddress,  i = 0;
#ifndef CDE_DRVG_IMPROVED_ISR_HANDLING
   MEM_POOLG_bufDescT*  bufDescP = NULL;
#endif
   UINT32            numOfLoopsDiv = 1;
   UINT32            metadataDestination = 0;
   CDE_DRVG_extIntParams *extIntParams = (CDE_DRVG_extIntParams *)arg;
   UINT32            imgWidth = params->dstParams.imgWidth / (extIntParams->numSubChannels + 1);
   UINT32            skipBytesPerLine = imgWidth * params->dstParams.imgBpp * extIntParams->numSubChannels;
   UINT32            phyAddresses[MAX_NUM_BUFFERS_LOOPS] = {0};

   params->dmaDstOffset = 0;
   params->dmaLoopSize = 0;
   params->programPtr = PL330DMAP_ConstrInit(dmaProg);
   UINT32 programPhyAddr;
   MEM_MAPG_convertVirtualToPhysical2((UINT32)params->program, &programPhyAddr, MEM_MAPG_CRAM_CDE_PARAMS_E);

#ifdef DEBUG_IMP_ISR_HANDLE
   printf("++++++++++++++extIntParams: NumOfSubChannes - %d PC begin at 0x%x(phy:0x%x)\n", extIntParams->numSubChannels, params->programPtr, programPhyAddr);
   for (UINT32 i = 0; i < extIntParams->numSubChannels; i++)
   {
      printf("+++++++++SubChannel DMA Handle: 0x%x\n", extIntParams->subChannelHandles[i]);
   }

   printf("DMA Interleaving for core %d / channel %d \n", params->coreNum, params->hwChannelNum);
#endif
   numOfLoopsDiv = CDE_SCENARIOSG_optimizeLoopExt( params, extIntParams->numSubChannels + 1);

   PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_CCR, params->ccr.word);
   PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_SAR, params->srcParams.address[0]);

   PL330DMAP_DMALPFE( params->programPtr );

   for( i = 0; i < params->numLoops; i++ )
   {
      PL330DMAP_DMAFLUSHP( params->programPtr ,params->srcParams.periphNum );
#ifdef CDE_DRVG_IMPROVED_ISR_HANDLING
      ret = MEM_POOLG_alloc(params->memPoolHandle, MEM_POOLG_getBufSize(params->memPoolHandle), &params->buffDescListP[i]);
#ifdef DEBUG_IMP_ISR_HANDLE
      printf("CDE_SCENARIOSP_setDestAndOffsets (setup2): loop %d bufDesc %p\n",i,params->buffDescListP[i]);
#endif
#else
      ret = MEM_POOLG_alloc(params->memPoolHandle, MEM_POOLG_getBufSize(params->memPoolHandle), &bufDescP);
#endif
      if(ERRG_SUCCEEDED(ret))
      {
#ifndef CDE_DRVG_IMPROVED_ISR_HANDLING
         if ( i == 0)
         {
            params->curBuffDesc  = bufDescP;
         }
         else
         {
            params->nextBuffDesc = bufDescP;
         }
         MEM_POOLG_getDataPhyAddr(bufDescP,&phyAddress);
#else
         MEM_POOLG_getDataPhyAddr(params->buffDescListP[i],&phyAddress);
#endif
#ifdef CDE_DRVG_METADATA
         CDE_SCENARIOSP_setDestAndOffsets(params, phyAddress+params->framebuffer_offset, i,0);
         //Save phyAddress for sub channel(s)
         phyAddresses[i] = phyAddress + params->framebuffer_offset;
#else
         CDE_SCENARIOSP_setDestAndOffsets(params, phyAddress, i,0);
         phyAddresses[i] = phyAddress;
#endif
      }
      else
      {
         return 1;
      }

      metadataDestination = phyAddress; /*Set the metadata destination to the start of the frame buffer */
      {
         //The Last sub channel should handle the stride if stride is required
         //Main channel does not care the stride setting
#ifdef CDE_DRVG_METADATA
#ifndef CDE_DRVG_METADATA_AFTER_LOOP
         CDE_SCENARIOSG_waitForMetadataCopy(params);
#else
         CDE_SCENARIOSG_initiateMetadataCopy(params);
#endif
#endif
         //1 burst is done in CDE_SCENARIOSG_initiateMetadatCopy()
         //At first we finish the first line
         PL330DMAP_DMALP(params->programPtr, (numOfLoopsDiv - 2));
            PL330DMAP_DMAWFP( params->programPtr ,params->srcParams.periphNum, PL330DMA_REQ_PERIPH );
            PL330DMAP_DMALDB( params->programPtr);
            PL330DMAP_DMASTB( params->programPtr );
         PL330DMAP_DMALPEND( params->programPtr );
         PL330DMAP_DMAADDH(params->programPtr, PL330DMA_REG_DAR, skipBytesPerLine);
         PL330DMAP_DMALPFE( params->programPtr );
            PL330DMAP_DMALP(params->programPtr, (numOfLoopsDiv - 1));
               PL330DMAP_DMAWFP( params->programPtr ,params->srcParams.periphNum, PL330DMA_REQ_PERIPH );
               PL330DMAP_DMALDB( params->programPtr);
               PL330DMAP_DMASTB( params->programPtr );
            PL330DMAP_DMALPEND( params->programPtr );
            PL330DMAP_DMAADDH(params->programPtr, PL330DMA_REG_DAR, skipBytesPerLine);
         PL330DMAP_DMALPEND( params->programPtr );
#ifdef CDE_DRVG_METADATA
#ifdef CDE_DRVG_METADATA_AFTER_LOOP
         CDE_SCENARIOSG_waitForMetadataCopy(params);
#endif
#endif
      }
      //1. clean instruction pipeline
      PL330DMAP_DMARMB( params->programPtr );
      PL330DMAP_DMAWMB( params->programPtr );
      //2. invalidate the instruction cache so new dst address is loaded.Must use unique channel number to not affet other channels.
      PL330DMAP_DMASEV( params->programPtr ,PL330DMA_LOOP_EVNT_NUM + params->virtualChannelNum);
      PL330DMAP_DMAWFE( params->programPtr ,PL330DMA_LOOP_EVNT_NUM + params->virtualChannelNum,1);

      if( params->frameDoneIntEnabled )
      {
         //Wait for sub-channel(s)
         for (UINT8 subChId = 0; subChId < extIntParams->numSubChannels; subChId++)
         {
            CDE_SCENARIOSP_saveWaitEvent(params, i, subChId);
         }
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
         CDE_SCENARIOSP_saveSetEvent(params);
#else
         PL330DMAP_DMASEV( params->programPtr, params->channelNum + PL330_DMA_FRAME_DONE_INT_OFFSET);
#endif
      }
   }
   PL330DMAP_DMALPEND( params->programPtr );

   if( params->channelDoneIntEnabled )
   {
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      params->setEventChannelDoneProgramP = params->programPtr;
      PL330DMAP_DMASEV( params->programPtr ,params->hwChannelNum + PL330_DMA_CHANNEL_DONE_INT_OFFSET );
#else
      PL330DMAP_DMASEV( params->programPtr ,params->channelNum + PL330_DMA_CHANNEL_DONE_INT_OFFSET );
#endif
   }

   PL330DMAP_DMAEND( params->programPtr );

   while( (UINT32)params->programPtr % 4 )
   {
      PL330DMAP_DMANOP( params->programPtr );
   }
   const UINT32 programSize = params->programPtr - dmaProg;
   if( programSize > CDE_DRVG_CH_PROGRAMM_MAX_SIZE )
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Program size too big \n");
      return 1;                                                // TODO: return program buffer overflow
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E,NULL,"Program size for Fixed Infinite scenario is %d \n", programSize);
   }

#ifdef DEBUG_IMP_ISR_HANDLE
   DMA_DumpProgram(params->program, programPhyAddr, programSize, "DMA Channel Program Dump");
#endif
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   PL330DMAP_PrintProgToRegister(params->coreNum, params->hwChannelNum, params->programPtr, dmaProg, params->dstParams.buffSize / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) );
#else
   PL330DMAP_PrintProgToRegister(params->coreNum, params->channelNum, params->programPtr, dmaProg, params->dstParams.buffSize / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) );
#endif

   for (UINT32 i = 0; i < extIntParams->numSubChannels; i++)
   {
      CDE_DRVG_channelParamsT *subChannelParams = extIntParams->subChannelHandles[i];
      bool isLastChannel = (i == (extIntParams->numSubChannels - 1));

      subChannelParams->numLoops = params->numLoops;
      //Note: the sub channel interrupt will not be enabled
      //      But hw channel num is assiged in CDE_DRVG_startChannelExt()
      //      So here set frameDoneIntEnabled to 1 to let the CDE_SCENARIOSG_setEvent(channelH) modify the SEV instruction
      subChannelParams->frameDoneIntEnabled = 1;
      subChannelParams->channelDoneIntEnabled = 0;
      subChannelParams->ccr.word = params->ccr.word;
      subChannelParams->dstParams.imgWidth = imgWidth;
      subChannelParams->dstParams.imgBpp = params->dstParams.imgBpp;
      //The original imgWidth in params->dstParams is twice the size of the imgWidth, stride will be used for the last subchannel
      //e.g: The interleaving stream is 1600*800, 2 channels, 1600 is used to allocate the buffer in mainChannel
      //     If stride is needed, then the second channel will need to deal with the stride
      if (isLastChannel)
      {
         subChannelParams->dstParams.stride = (params->dstParams.stride > params->dstParams.imgWidth) ? 0 : params->dstParams.imgWidth - imgWidth * (i + 1);
      }

      //printf("SubChannel[%d](at 0x%x): srcParams(address-0x%x periphNum-%d) dstParams(imgWidth-%d) ccr(0x%x)\n", i, (UINT32)subChannelParams
      //              , subChannelParams->srcParams.address[0], subChannelParams->srcParams.periphNum
      //              , subChannelParams->dstParams.imgWidth, subChannelParams->ccr.word);
      CDE_SCENARIOSG_periphToMemoryInfiniteExtSubChannel(subChannelParams, numOfLoopsDiv, phyAddresses, imgWidth * subChannelParams->dstParams.imgBpp * (i + 1),skipBytesPerLine, isLastChannel);
   }

   return CDE__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: CDE_SCENARIOSG_periphToMemoryInfinite
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
ERRG_codeE CDE_SCENARIOSG_memoryToMemoryOnce( CDE_DRVG_channelParamsT* params, void* arg )
{
   dmaChannelProg dmaProg = params->program;
   UINT32 numOfLoops = params->dstParams.buffSize / ((params->ccr.field.srcBurstLen + 1) * (1 << params->ccr.field.srcBurstSize));
   (void)arg;
   arg = (void *)0;
   params->programPtr = PL330DMAP_ConstrInit(dmaProg);

   //for drain mode, we set the next next destination address each isr:
   //1. clean instruction pipeline
   PL330DMAP_DMARMB( params->programPtr );
   PL330DMAP_DMAWMB( params->programPtr );
   //2. invalidate the instruction cache so new dst address is loaded
   PL330DMAP_DMASEV( params->programPtr, PL330DMA_LOOP_EVNT_NUM);
   PL330DMAP_DMAWFE( params->programPtr, PL330DMA_LOOP_EVNT_NUM,1);

   PL330DMAP_DMAMOV( params->programPtr, PL330DMA_REG_CCR, params->ccr.word );
   params->srcParams.addressOffset = (UINT32)(params->programPtr - params->program);
   PL330DMAP_DMAMOV( params->programPtr, PL330DMA_REG_SAR, params->srcParams.address[0]);
   params->dstParams.addressOffset = (UINT32)(params->programPtr - params->program);
   PL330DMAP_DMAMOV( params->programPtr, PL330DMA_REG_DAR, params->dstParams.address[0]);

   params->dstParams.buffSizeOffset = (UINT32)(params->programPtr - params->program);

   PL330DMAP_DMALP( params->programPtr, numOfLoops - 1 );
      PL330DMAP_DMALD( params->programPtr );
      PL330DMAP_DMAST( params->programPtr );
   PL330DMAP_DMALPEND( params->programPtr );

   PL330DMA_CcrU newCcr;
   UINT32 rem;
   newCcr.word = params->ccr.word;
   newCcr.field.srcBurstLen = 0;
   newCcr.field.dstBurstLen = 0;
   rem = params->dstParams.buffSize - (numOfLoops * ((params->ccr.field.srcBurstLen + 1) * (1 << params->ccr.field.srcBurstSize)));
   numOfLoops = rem / ((newCcr.field.srcBurstLen + 1) * (1 << newCcr.field.srcBurstSize));
   if (numOfLoops)
   {
      PL330DMAP_DMAMOV( params->programPtr, PL330DMA_REG_CCR, newCcr.word );
      PL330DMAP_DMALP( params->programPtr, numOfLoops - 1 );
         PL330DMAP_DMALD( params->programPtr );
         PL330DMAP_DMAST( params->programPtr );
      PL330DMAP_DMALPEND( params->programPtr );
   }

   if( params->channelDoneIntEnabled )
   {
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      params->setEventChannelDoneProgramP = params->programPtr;
      PL330DMAP_DMASEV( params->programPtr,params->hwChannelNum+ PL330_DMA_CHANNEL_DONE_INT_OFFSET );
#else
      PL330DMAP_DMASEV( params->programPtr,params->channelNum + PL330_DMA_CHANNEL_DONE_INT_OFFSET );
#endif
   }

   PL330DMAP_DMAEND( params->programPtr );

   while( (UINT32)params->programPtr % 4 )
   {
      PL330DMAP_DMANOP( params->programPtr );
   }

   if( (params->programPtr - dmaProg) > CDE_DRVG_CH_PROGRAMM_MAX_SIZE )
   {
      return 1;                                                // TODO: return program buffer overflow
   }
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   PL330DMAP_PrintProgToRegister(params->coreNum, params->hwChannelNum, params->programPtr, dmaProg, params->dstParams.buffSize / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) );
#else
   PL330DMAP_PrintProgToRegister(params->coreNum, params->channelNum, params->programPtr, dmaProg, params->dstParams.buffSize / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) );
#endif


   return CDE__RET_SUCCESS;

}




void CDE_SCENARIOSG_setMemToPeriphSrc(CDE_DRVG_channelHandleT channelH, UINT32 srcAddress)
{
 //  printf ("CDE_SCENARIOSG_setMemToPeriphSrc channelH->dmaDstOffset %d srcAddress %p\n",channelH->dmaDstOffset,srcAddress);
   channelH->programPtr = &channelH->program[channelH->dmaDstOffset];
   PL330DMAP_DMAMOV( channelH->programPtr, PL330DMA_REG_SAR,srcAddress);
}


ERRG_codeE CDE_SCENARIOSG_memoryToPeriph( CDE_DRVG_channelParamsT* params, void *arg )
{
   (void)arg;
    dmaChannelProg dmaProg = params->program;

   //printf ("CDE_SCENARIOSG_memoryToPeriph buffsize=%d params->ccr.field.srcBurstLen =%d params->ccr.field.srcBurstSize=%d\n", params->srcParams.buffSize,params->ccr.field.srcBurstLen,params->ccr.field.srcBurstSize);

   params->programPtr = PL330DMAP_ConstrInit(dmaProg);

   PL330DMAP_DMAMOV( params->programPtr, PL330DMA_REG_CCR, params->ccr.word );
   PL330DMAP_DMAMOV( params->programPtr, PL330DMA_REG_DAR, params->dstParams.address[0] );
   params->dmaDstOffset =  (UINT32)params->programPtr - (UINT32)params->program;

   //printf ("CDE_SCENARIOSG_memoryToPeriph channelH->dmaDstOffset %d\n",params->dmaDstOffset);
   PL330DMAP_DMAMOV( params->programPtr, PL330DMA_REG_SAR, 0);

   //PL330DMAP_DMAMOV( PL330DMA_REG_SAR, params->srcParams.address[0] );

   PL330DMAP_DMAFLUSHP(params->programPtr, params->dstParams.periphNum);
//option 1: loop forever with calculation beat last and single end =1
//option 2: find loops per line by numOfLoopsDiv (when numOfLoopsMod =0). and bigger loop run on nun of lines.

   PL330DMAP_DMALP( params->programPtr, params->srcParams.imgHeight - 1);
      PL330DMAP_DMALP( params->programPtr, params->srcParams.numLoopsPerLine - 1 );
         PL330DMAP_DMAWFP( params->programPtr, params->dstParams.periphNum, PL330DMA_REQ_BURST );
         PL330DMAP_DMALDB( params->programPtr );
         PL330DMAP_DMALDB( params->programPtr );
         PL330DMAP_DMALDB( params->programPtr );
         PL330DMAP_DMALDB( params->programPtr );
         PL330DMAP_DMASTB( params->programPtr );
        //PL330DMAP_DMASTP( params->programPtr, params->dstParams.periphNum, PL330DMA_REQ_BURST );
      PL330DMAP_DMALPEND( params->programPtr );

      //in multigraph injection we have to jump at the end of line (stride-buffWidth).
      //at the other cases (regular injection for example), stride-buffwidth can be not equal to zero but we do not need to jump
      //thus, ew will jump only when stride > buffWidth, when we need to jump in interleave image in left/right side
      if (params->srcParams.stride > params->srcParams.buffWidth)
      {
          PL330DMAP_DMAADDH( params->programPtr, PL330DMA_REG_SAR, ((params->srcParams.stride - params->srcParams.buffWidth) * params->srcParams.imgBpp));//add jumping in case of 2 writers & 1 image for each, other case will add 0
      }
   PL330DMAP_DMALPEND( params->programPtr );

   if( params->frameDoneIntEnabled)
   {
      //printf ("CDE_SCENARIOSG_memoryToPeriph frame done params->channelNum =%d corenum= %d\n", params->channelNum,params->coreNum);
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      CDE_SCENARIOSP_saveSetEvent(params);
#else
      PL330DMAP_DMASEV( params->programPtr, params->channelNum+ PL330_DMA_FRAME_DONE_INT_OFFSET);
#endif
   }

   PL330DMAP_DMAEND( params->programPtr );

   if( (params->programPtr - dmaProg) > CDE_DRVG_CH_PROGRAMM_MAX_SIZE )
   {
      //printf ("CDE_SCENARIOSG_memoryToPeriph actual buff size = %d \n",(params->programPtr - dmaProg));
      return 1;                                                // TODO: return program buffer overflow
   }
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   PL330DMAP_PrintProgToRegister(params->coreNum, params->hwChannelNum, params->programPtr, dmaProg ,params->srcParams.numLoopsPerLine * params->srcParams.imgHeight);
#else
   PL330DMAP_PrintProgToRegister(params->coreNum, params->channelNum, params->programPtr, dmaProg ,params->srcParams.numLoopsPerLine * params->srcParams.imgHeight);
#endif

   return CDE__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: CDE_SCENARIOSG_periphToMemoryCyclic
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
ERRG_codeE CDE_SCENARIOSG_periphToMemoryCyclic( CDE_DRVG_channelParamsT* params, void* arg )
{
   dmaChannelProg dmaProg = params->program;
   UINT32 numOfLoops = params->dstParams.buffSize / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize));
   (void)arg;

// printf("buffSize %d, dstBurstLen %d dstBurstSize %d src 0x%x, dst 0x%x, numOfLoops = %d\n", params->dstParams.buffSize, params->ccr.field.dstBurstLen, params->ccr.field.dstBurstSize,
//      params->srcParams.address[0],params->dstParams.address[0],numOfLoops);

   params->programPtr = PL330DMAP_ConstrInit(dmaProg);

   PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_CCR, params->ccr.word );
   PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_SAR, params->srcParams.address[0]);
   PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_DAR, params->dstParams.address[0]);

   numOfLoops--;
   PL330DMAP_DMALPFE( params->programPtr );

      PL330DMAP_DMALP( params->programPtr ,numOfLoops );
      PL330DMAP_DMAFLUSHP( params->programPtr ,params->srcParams.periphNum );

      PL330DMAP_DMAWFP( params->programPtr ,params->srcParams.periphNum, PL330DMA_REQ_SINGLE );

      PL330DMAP_DMALDS( params->programPtr );
      PL330DMAP_DMASTS( params->programPtr );

      PL330DMAP_DMALPEND( params->programPtr );
      //PL330DMAP_DMAADNH( params->programPtr, PL330DMA_REG_DAR, TWOS_COMPLIMENT_16BIT(params->dstParams.buffSize) );

      PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_DAR, params->dstParams.address[0]);

   PL330DMAP_DMALPEND( params->programPtr );


   PL330DMAP_DMAEND( params->programPtr );

   while( (UINT32)params->programPtr % 4 )
   {
      PL330DMAP_DMANOP( params->programPtr );
   }

   if( (params->programPtr - dmaProg) > CDE_DRVG_CH_PROGRAMM_MAX_SIZE )
   {
      return 1;                                    // TODO: return program buffer overflow
   }

#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   PL330DMAP_PrintProgToRegister(params->coreNum, params->hwChannelNum, params->programPtr, dmaProg, params->dstParams.buffSize / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) );
#else
   PL330DMAP_PrintProgToRegister(params->coreNum, params->channelNum, params->programPtr, dmaProg, params->dstParams.buffSize / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) );
#endif

   return CDE__RET_SUCCESS;

}

/****************************************************************************
*
*  Function Name: CDE_SCENARIOSG_periphToMemoryCyclic
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
//#define _FULL_DSR_
typedef struct
{
   UINT32 baseAddress;
   INT32 size;
   UINT32 loopsNum;
}CDE_SCENARIOS_ddrless_buff_T;

#define DSR_LINE_IN_BYTES     (2880)
extern void CVA_MNGRG_getDdrlessInfo(UINT32 num, UINT32 *addressP, UINT32 *sizeP);
ERRG_codeE CDE_SCENARIOSG_periphToMemoryDdrLess( CDE_DRVG_channelParamsT* params, void* arg )
{
   UINT32 i = 0;
   INT32 remainedSize = 0;
   (void)arg;
   dmaChannelProg dmaProg = params->program;
#if 1
   CDE_SCENARIOS_ddrless_buff_T ddrless_buff[6];

   memset(ddrless_buff,0,sizeof(ddrless_buff));
   //assmption - the registers are already populated
   for (i = 0; i < 6; i++)
   {
      CVA_MNGRG_getDdrlessInfo(i,&ddrless_buff[i].baseAddress, &ddrless_buff[i].size);
   }
#else
   CDE_SCENARIOS_ddrless_buff_T ddrless_buff[6] =
   {
#ifdef   _FULL_DSR_
      {0x04500000, (1755*1024), 0 },
#else
      {0x04500000, (272 * DSR_LINE_IN_BYTES), 0 }, // option 13 in ram_manager
//	      {0x04500000, (448 * DSR_LINE_IN_BYTES), 0 }, // option 4 in ram_manager   = 0x13B000
#endif
      {0x01004000, (0x7c000), 0 },// for option 13 in ram_manager we use CSM for CVA
      {0x02002000, (0x8000), 0 },
      {0x00, 0x00, 0},
      {0x00, 0x00, 0},
      {0x00, 0x00, 0},
      {0x00, 0x00, 0},
   };
#endif

   //printf("buff 0x%x, dstBurstLen %d dstBurstSize %d \n", params->dstParams.buffSize, params->ccr.field.dstBurstLen, params->ccr.field.dstBurstSize);
   remainedSize = params->dstParams.buffSize;

   for(i = 0; i < 6; i++)
   {
//	  printf("CDE_SCENARIOSG_periphToMemoryDdrLess (%d,%d): Address 0x%x, size 0x%x\n",params->coreNum, params->channelNum, ddrless_buff[i].baseAddress, ddrless_buff[i].size);

      if( (ddrless_buff[i].baseAddress == 0) || (remainedSize <= 0) )
         continue;

      ddrless_buff[i].loopsNum = ((remainedSize <= ddrless_buff[i].size)?(remainedSize):(ddrless_buff[i].size)) / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize));
      remainedSize = remainedSize - ((remainedSize <= ddrless_buff[i].size)?(remainedSize):(ddrless_buff[i].size));
//      printf("i = %d, loop size %d, address 0x%08x size 0x%08x\n", i, ddrless_buff[i].loopsNum, ddrless_buff[i].baseAddress, ddrless_buff[i].size);
   }

   params->programPtr = PL330DMAP_ConstrInit(dmaProg);

// PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_CCR, params->ccr.word );

   PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_CCR, 0x003D40F8 );

   PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_SAR, params->srcParams.address[0]);

   PL330DMAP_DMALPFE( params->programPtr );  // outer infinite loop

   PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_DAR, (ddrless_buff[0].baseAddress));
   PL330DMAP_DMAFLUSHP( params->programPtr ,params->srcParams.periphNum );
   if(ddrless_buff[1].loopsNum == 0)
   {
      PL330DMAP_DMALPFE( params->programPtr );
          PL330DMAP_DMAWFP( params->programPtr ,params->srcParams.periphNum, PL330DMA_REQ_PERIPH );

         PL330DMAP_DMALDB( params->programPtr);

         PL330DMAP_DMASTB( params->programPtr );
         PL330DMAP_DMASTB( params->programPtr );
         PL330DMAP_DMASTB( params->programPtr );
         PL330DMAP_DMASTB( params->programPtr );

         PL330DMAP_DMALDS( params->programPtr );

         PL330DMAP_DMASTS( params->programPtr );
         PL330DMAP_DMASTS( params->programPtr );
         PL330DMAP_DMASTS( params->programPtr );
         PL330DMAP_DMASTS( params->programPtr );

      PL330DMAP_DMALPEND( params->programPtr );
   }
   else
   {
      PL330DMAP_DMALP( params->programPtr, (ddrless_buff[0].loopsNum - 1) );

         PL330DMAP_DMAWFP( params->programPtr ,params->srcParams.periphNum, PL330DMA_REQ_PERIPH );

         PL330DMAP_DMALDB( params->programPtr);

         PL330DMAP_DMASTB( params->programPtr );
         PL330DMAP_DMASTB( params->programPtr );
         PL330DMAP_DMASTB( params->programPtr );
         PL330DMAP_DMASTB( params->programPtr );

      PL330DMAP_DMALPEND( params->programPtr );

      PL330DMAP_DMARMB( params->programPtr );
      PL330DMAP_DMAWMB( params->programPtr );

//    PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_CCR, 0x003E40F8 );
      PL330DMAP_DMAMOV( params->programPtr ,PL330DMA_REG_DAR, (ddrless_buff[1].baseAddress));

      PL330DMAP_DMALPFE( params->programPtr );
         PL330DMAP_DMAWFP( params->programPtr ,params->srcParams.periphNum, PL330DMA_REQ_PERIPH );

         PL330DMAP_DMALDB( params->programPtr);

         PL330DMAP_DMASTB( params->programPtr );
         PL330DMAP_DMASTB( params->programPtr );
         PL330DMAP_DMASTB( params->programPtr );
         PL330DMAP_DMASTB( params->programPtr );


         PL330DMAP_DMALDS( params->programPtr );

         PL330DMAP_DMASTS( params->programPtr );
         PL330DMAP_DMASTS( params->programPtr );
         PL330DMAP_DMASTS( params->programPtr );
         PL330DMAP_DMASTS( params->programPtr );

      PL330DMAP_DMALPEND( params->programPtr );
   }

   if( params->frameDoneIntEnabled )
   {
#ifdef CDE_DRVG_VIRTUAL_CHANNELS
      CDE_SCENARIOSP_saveSetEvent(params);
#else
      PL330DMAP_DMASEV( params->programPtr ,params->channelNum + PL330_DMA_FRAME_DONE_INT_OFFSET );
#endif
   }

   PL330DMAP_DMALPEND( params->programPtr );


   PL330DMAP_DMAEND( params->programPtr );

   while( (UINT32)params->programPtr % 4 )
   {
      PL330DMAP_DMANOP( params->programPtr );
   }

   if( (params->programPtr - dmaProg) > CDE_DRVG_CH_PROGRAMM_MAX_SIZE )
   {
      return 1;                                    // TODO: return program buffer overflow
   }

#ifdef CDE_DRVG_VIRTUAL_CHANNELS
   PL330DMAP_PrintProgToRegister(params->coreNum, params->hwChannelNum, params->programPtr, dmaProg, params->dstParams.buffSize / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) );
#else
   PL330DMAP_PrintProgToRegister(params->coreNum, params->channelNum, params->programPtr, dmaProg, params->dstParams.buffSize / ((params->ccr.field.dstBurstLen + 1) * (1 << params->ccr.field.dstBurstSize)) );
#endif

   return CDE__RET_SUCCESS;



}


