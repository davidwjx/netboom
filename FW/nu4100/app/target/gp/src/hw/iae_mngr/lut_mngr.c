
#include "assert.h"

#include "inu_device.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inu2.h"
#include "lut_mngr.h"
#include "inu_defs.h"
#include "cde_mngr_new.h"
#include "hw_mngr.h"
#include "hcg_mngr.h"
#include "svc_cfg.h"
#include "internal_cmd.h"
#include "sequence_mngr.h"
#include "inu_lm.h"
#include "inu2_internal.h"
#include "inu_common.h"

typedef struct lut_mngr_ptrs
{
   void *phys;
   void *virt;
   UINT32 size;
}lut_mngr_ptrs_T;

#define LUT_MNGRP_ONE_DMA_CH

// first half IB LUTS then DSR loot
static lut_mngr_ptrs_T lut_mngr_memPtr[LUT_MNGR_NUM_TOTAL_SIZE][LUT_MNGR_NUM_OF_BUFF] = {0};
static UINT32          lut_mngr_doubleBufferIdx = 0;
static UINT8           firstTime=1;

#ifdef LUT_MNGRP_ONE_DMA_CH
static CDE_MNGRG_channnelInfoT *channelInfo;
#endif

int inu_sensors_group__getSensNumAndRes(inu_ref *me,unsigned int *sensNum,unsigned int numSens,unsigned int *resolution);
void lut_mngr_gpActivateSensorGroupLutMsg(inu_ref *ref);
BOOLEAN inu_device__readTestFile(CALIB_sensorLutModeE sensorMode,char filebin[],
        inu_device_iaeLutIdE lutId,unsigned int sensorInd);


/****************************************************************************
*
*  Function Name: lut_mngr_gpGetLutDdrBufAddr
*
*  Description: Get the LUT addr in ddr & IAE
*
*  Inputs: receives lut id and mode
*
*  Outputs: physical & virtual LUTs addr in DDR & IAE
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
void lut_mngr_gpGetLutDdrBufAddr(CALIB_sensorLutModeE calibMode,
                                 CALIB_sensorTemperModeE temperInd,
                                 UINT32 ddrLutIdOffset,
                                 UINT16 mode, // A or B
                                 UINT32 dbIdx,
                                 void **ddrLutVirtAddr,
                                 void **ddrLutPhyAddr,
                                 int *isShared,
                                 UINT32 *sizeP,
                                 lut_mngr_ptrs_T **lutEntryP)
{
   UINT32 ddrSize,ddrInd,ddrSectionSize; // DDR map for each lut is devided to binning full and vertical binning
   *sizeP = 0;
   ddrInd = calibMode+temperInd*CALIB_NUM_LUT_MODES_E;
   if (ddrLutIdOffset < LUT_MNGR_NUM_LUT_SIZE)
   {
      //These need to be saved in DDR (for recovery or alternating mode)
      //The LUT enum must be aligned to the LUT in mem_map
      if  (mode)
          ddrLutIdOffset += LUT_MNGR_NUM_LUT_SIZE;
      if (lut_mngr_memPtr[ddrLutIdOffset][dbIdx].size)
      {
         *ddrLutVirtAddr = lut_mngr_memPtr[ddrLutIdOffset][dbIdx].virt;
         *ddrLutPhyAddr = lut_mngr_memPtr[ddrLutIdOffset][dbIdx].phys;
         ddrSize= lut_mngr_memPtr[ddrLutIdOffset][dbIdx].size;
         ddrSectionSize = ddrSize/(CALIB_NUM_LUT_MODES_E*CALIB_NUM_TEMPER_LUT_MODES_E);
         *ddrLutPhyAddr += ddrInd * ddrSectionSize;
         *ddrLutVirtAddr += ddrInd * ddrSectionSize;
         *isShared = 0;
         *sizeP = ddrSize -4;
         *lutEntryP = &lut_mngr_memPtr[ddrLutIdOffset][dbIdx];
      }
   }
}

ERRG_codeE lut_mngr_allocBuff(UINT32 lutId, UINT32 dbuffId)
{
   UINT32 lutSize;
   UINT32 lutIBSize[4]={0x4F04,0x4F04,0x9204,0x9204};
   UINT32 lutDsrSize[4]={0x2804,0x2804,0x12404,0x12404};
   UINT32 lutIDsrSize = 0x8274;
   UINT32 lutLscSize = 0x400;
   ERRG_codeE retCode;

   if (lutId < LUT_MNGR_DSR_START_ID) // low half for IB
      lutSize = lutIBSize[lutId%LUT_MNGR_NUM_IAE_SUB_LUT];
   else if (lutId < (LUT_MNGR_IDSR_START_ID))
      lutSize = lutDsrSize[lutId%LUT_MNGR_NUM_IAE_SUB_LUT];
   else if ( lutId < LUT_MNGR_LSC_START_ID)
      lutSize = lutIDsrSize;
   else
      lutSize = lutLscSize;

   lutSize *= CALIB_NUM_LUT_MODES_E*CALIB_NUM_TEMPER_LUT_MODES_E;
   retCode = MEM_MAPG_alloc(lutSize,(UINT32 *)&lut_mngr_memPtr[lutId][dbuffId].phys,(UINT32 *)&lut_mngr_memPtr[lutId][dbuffId].virt,1);
   if  (ERRG_SUCCEEDED(retCode))
        lut_mngr_memPtr[lutId][dbuffId].size=lutSize;
   return retCode;
}



ERRG_codeE lut_mngr_gpActivateSensorLut(unsigned int iauInd,unsigned int sensorId,CALIB_sensorLutModeE calibMode)
{
   ERRG_codeE             retCode     = SYSTEM__RET_SUCCESS;
   int                    shared;
   UINT32                 ddrStartInd,totalSize;
   MEM_MAPG_addrT         iaeLutVirtsAddr;
   MEM_MAPG_addrT         iaeLutPhyAddr;
#ifndef LUT_MNGRP_ONE_DMA_CH
    CDE_MNGRG_channnelInfoT   *channelInfo;
#endif

    lut_mngr_ddrLut     *ddrLutVirtBuff=NULL, *ddrLutPhyBuff=NULL;
    UINT16 mode=0,lutStartInd,subLutInd,sensorDdrLutInd,dsrOrIb;
    lut_mngr_ptrs_T        *lutEntryP;

    if (calibMode < CALIB_NUM_LUT_MODES_E)
    {
      for (dsrOrIb=0;dsrOrIb<2;dsrOrIb++)
      {
         if (dsrOrIb == 0)
         {
            lutStartInd = MEM_MAPG_IAE_LUT_DSR_0_0_E + iauInd*LUT_MNGR_NUM_IAE_SUB_LUT;
            ddrStartInd = LUT_MNGR_NUM_IAE_SUB_LUT*LUT_MNGR_NUM_SENSORS + sensorId*LUT_MNGR_NUM_IAE_SUB_LUT;
         }
         else
         {
            lutStartInd = MEM_MAPG_IAE_LUT_IB_0_0_E + iauInd*LUT_MNGR_NUM_IAE_SUB_LUT;
            ddrStartInd = sensorId*LUT_MNGR_NUM_IAE_SUB_LUT;
         }

         for (subLutInd=0;subLutInd<LUT_MNGR_NUM_IAE_SUB_LUT;subLutInd++)
         {
            MEM_MAPG_getPhyAddr(lutStartInd+subLutInd, &iaeLutPhyAddr);
            lut_mngr_gpGetLutDdrBufAddr(  calibMode,
                                          CALIB_LUT_TEMPER_MODE_0_E,
                                          ddrStartInd+subLutInd,
                                          mode,
                                          lut_mngr_doubleBufferIdx,
                                          (void **)&ddrLutVirtBuff,
                                          (void **)&ddrLutPhyBuff,
                                          &shared,
                                          &totalSize,&lutEntryP);
            if (totalSize)
            {
                if (ddrLutVirtBuff->lutSize != 0)
                {
#ifndef LUT_MNGRP_ONE_DMA_CH
                   //DMA memcpy from ddr to iae
                   retCode = CDE_MNGRG_memcpyOpenChan(&channelInfo, ddrLutVirtBuff->lutSize, NULL, NULL);
#endif
                   if(ERRG_SUCCEEDED(retCode))
                   {
                       //LOGG_PRINT(LOG_DEBUG_E, NULL, "DMA mem copy from ddrLutPhyAddr 0x%x to iaeLutPhyAddr 0x%x size %x calibMode %d iauInd %d DDR lutInd %d lut IAU %d\n",(UINT32)ddrLutPhyBuff->data, (UINT32)iaeLutPhyAddr,ddrLutVirtBuff->lutSize,calibMode,iauInd,ddrStartInd + subLutInd,lutStartInd + subLutInd);
                       //printf("DMA mem copy from ddrLutPhyAddr 0x%x to iaeLutPhyAddr 0x%x size %x calibMode %d iauInd %d DDR lutInd %d lut IAU %d\n",(UINT32)ddrLutPhyBuff->data, (UINT32)iaeLutPhyAddr,ddrLutVirtBuff->lutSize,calibMode,iauInd,ddrStartInd + subLutInd,lutStartInd + subLutInd);

                       HCG_MNGRG_voteUnits(1 << HCG_MNGRG_IAE);
                       retCode = CDE_MNGRG_memcpyBlock(channelInfo,(UINT32)iaeLutPhyAddr,(UINT32)ddrLutPhyBuff->data,ddrLutVirtBuff->lutSize);
                       /*Return early if this transfer fails*/
                       if(ERRG_FAILED(retCode))
                       {
                           LOGG_PRINT(LOG_ERROR_E, NULL, "WARNING Calibration may not work! FAILED TO DMA mem copy from ddrLutPhyAddr 0x%x to iaeLutPhyAddr 0x%x size %x calibMode %d iauInd %d DDR lutInd %d lut IAU %d\n",(UINT32)ddrLutPhyBuff->data, (UINT32)iaeLutPhyAddr,ddrLutVirtBuff->lutSize,calibMode,iauInd,ddrStartInd + subLutInd,lutStartInd + subLutInd);
                           return retCode;
                       }
                
                       HCG_MNGRG_devoteUnits(1 << HCG_MNGRG_IAE);
#ifndef LUT_MNGRP_ONE_DMA_CH
                       CDE_MNGRG_memcpyCloseChan(channelInfo);
#endif
                   }
                   else
                   {
                       LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to open dma channel for memcpy lut from ddr\n");
                       return retCode;
                   }
                }
                else
                {
                   LOGG_PRINT(LOG_DEBUG_E, NULL, "zero lut size!\n");
                   retCode = HW_MNGR__ERR_INVALID_ARGS;
                }
             }
             else
             {
                LOGG_PRINT(LOG_DEBUG_E, NULL, "zero total size! lut %d\n",ddrStartInd+subLutInd);
                retCode = HW_MNGR__ERR_INVALID_ARGS;
             }
         }
      }
    }
    else
    {
       LOGG_PRINT(LOG_ERROR_E, NULL, "wrong calib mode! (%d)\n",calibMode);
       retCode = HW_MNGR__ERR_INVALID_ARGS;
    }

    return retCode;
}

#if 0
/****************************************************************************
*
*  Function Name: lut_mngr_gpActivateLutMsg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE lut_mngr_gpActivateLutMsg(void *handle)
{
   ERRG_codeE             retCode     = SYSTEM__RET_SUCCESS;
   int                    shared;
   UINT32                 iauInd,ddrStartInd,totalSize;
   MEM_MAPG_addrT         iaeLutVirtsAddr;
   MEM_MAPG_addrT         iaeLutPhyAddr;
   CDE_MNGRG_channnelInfoT   *channelInfo;
   lut_mngr_ddrLut     *ddrLutVirtBuff, *ddrLutPhyBuff;
   UINT16 mode=0,lutStartInd,subLutInd,sensorDdrLutInd,dsrOrIb;
   CALIB_sensorLutModeE calibMode;
   CALIB_iauTosensor iauToSensor[LUT_MNGR_NUM_IAE_LUT];

//   lut_mngr_testLutFiles("/media/inuitive/Rev001/");
   memset(iauToSensor,0xff,sizeof(iauToSensor));
   SEQ_MNGRG_getSensorNumAndCalibMode(handle, iauToSensor);
   for (iauInd =0; iauInd<LUT_MNGR_NUM_IAE_LUT; iauInd++)
   {
      if ((iauToSensor[iauInd].sensorId < LUT_MNGR_NUM_SENSORS))
      {
         switch (iauToSensor[iauInd].calib)
         {
            case 1:
            calibMode=CALIB_LUT_MODE_BIN_E;
            break;
            case 2:
            calibMode=CALIB_LUT_MODE_VERTICAL_BINNING_E;
            break;
            case 3:
            calibMode=CALIB_LUT_MODE_FULL_E;
            break;
            default:
            calibMode=CALIB_NUM_LUT_MODES_E;
            break;
         }
         lut_mngr_gpActivateSensorLut(iauInd,iauToSensor[iauInd].sensorId,calibMode);
      }
   }

   return(retCode);
}
#endif

void lut_mngr_gpClearLutDb()
{
   UINT32        lutId;
   UINT32        dbInd;
#ifdef LUT_MNGRP_ONE_DMA_CH
   ERRG_codeE    retCode;
#endif

   if (firstTime==1)
   {
      firstTime=0;
      memset(lut_mngr_memPtr,0,sizeof(lut_mngr_memPtr));
#ifndef LUT_MNGRP_ONE_DMA_CH //NEED TO FIX WITH PROPER INIT/DEINIT FUNCTIONS FOR LUT_MNGR
      //DMA memcpy from ddr to iae, use dummy size
      retCode = CDE_MNGRG_memcpyOpenChan(&channelInfo, 0x2000, NULL, NULL);
      if (ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_ERROR_E, retCode, "Error opening DMA ch for lut usage\n");
         assert(0);
      }
#endif
   }
   else
   {
      for (lutId=0;lutId<LUT_MNGR_NUM_LUT_SIZE;lutId++)
      {
         for (dbInd = 0; dbInd < LUT_MNGR_NUM_OF_BUFF; dbInd++)
         {
            if (lut_mngr_memPtr[lutId][dbInd].virt)
               MEM_MAPG_free(lut_mngr_memPtr[lutId][dbInd].virt);
            lut_mngr_memPtr[lutId][dbInd].phys = NULL;
            lut_mngr_memPtr[lutId][dbInd].virt = NULL;
            lut_mngr_memPtr[lutId][dbInd].size = 0;
         }
      }
   }
}


ERRG_codeE lut_mngr_init()
{
   ERRG_codeE    retCode = RET_SUCCESS;
#ifdef LUT_MNGRP_ONE_DMA_CH
   //DMA memcpy from ddr to iae, use dummy size
   retCode = CDE_MNGRG_memcpyOpenChan(&channelInfo, 0x2000, NULL, NULL);
   if (ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "Error opening DMA ch for lut usage\n");
   }
#endif

   firstTime = 1;

   return retCode;
}


ERRG_codeE lut_mngr_deinit()
{
   ERRG_codeE    retCode = RET_SUCCESS;
#ifdef LUT_MNGRP_ONE_DMA_CH
   retCode = CDE_MNGRG_closeChannel(channelInfo);
#endif
   return retCode;
}

void lut_mngr_updateDblBuffId(inu_deviceH me, UINT32 dblBuffId)
{
   lut_mngr_doubleBufferIdx = dblBuffId;

   printf("Set new lut_mngr_doubleBufferIdx to %d\n", lut_mngr_doubleBufferIdx);
}

/****************************************************************************
*
*  Function Name: lut_mngr_gpHandleLutChunkMsg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE lut_mngr_gpHandleLutChunkMsg(inu_deviceH me, void *lutChunkHdrPtr, UINT8 *chunkP)
{
   ERRG_codeE             retCode     = SYSTEM__RET_SUCCESS;
   UINT32                 ddrLutOffset;
   UINT32                 ddrOffset;
   int                    shared;
   UINT32                 totalSize=0;
   MEM_MAPG_addrT         iaeLutVirtsAddr;
   lut_mngr_ddrLut        *ddrLutVirtBuff=NULL, *ddrLutPhyBuff=NULL;
   INTERNAL_CMDG_lutChunkHdrT *lutChunkHdrP=(INTERNAL_CMDG_lutChunkHdrT *)lutChunkHdrPtr;
   lut_mngr_ptrs_T        *lutEntryP;

   if (lutChunkHdrP->lutId>=LUT_MNGR_NUM_LUT_SIZE)
   {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Lut sensor Ind limits! lutId %d max LUt id %d\n",lutChunkHdrP->lutId,LUT_MNGR_NUM_LUT_SIZE);
         return SYSTEM__ERR_UNEXPECTED;
   }

   if (lut_mngr_memPtr[lutChunkHdrP->lutId][lutChunkHdrP->dblBuffId].virt == NULL)
   {
      retCode=lut_mngr_allocBuff(lutChunkHdrP->lutId, lutChunkHdrP->dblBuffId);
      if (ERRG_FAILED(retCode))
      {
            return retCode;
      }
   }

   if (lutChunkHdrP->lutId < LUT_MNGR_NUM_LUT_SIZE)
   {
      lut_mngr_gpGetLutDdrBufAddr(lutChunkHdrP->calibMode,
              CALIB_LUT_TEMPER_MODE_0_E,
              lutChunkHdrP->lutId,
              lutChunkHdrP->mode,
              lutChunkHdrP->dblBuffId,
              (void **)&ddrLutVirtBuff,
              (void **)&ddrLutPhyBuff,
              &shared,
              &totalSize, &lutEntryP);

      ddrLutOffset = (lutChunkHdrP->chunkId << LUT_MNGR_CHUNK_SIZE_FACTOR);
      ddrOffset = (shared) ? 0 : ddrLutOffset;

      ddrLutVirtBuff->lutSize = 0; // update size only on last chunk
      totalSize -= sizeof(ddrLutVirtBuff->lutSize); // at start of buffer 4 bytes is saved for lut size

      if((ddrLutOffset + lutChunkHdrP->chunkLen) > totalSize)
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "Lut size exceeds limits! lutId %d chunk=%d size=%d saved to %p offset 0x%x, totalSize = %d, so far = %d\n",
           lutChunkHdrP->lutId, lutChunkHdrP->chunkId, lutChunkHdrP->chunkLen,ddrLutVirtBuff->data +ddrOffset, ddrLutOffset, totalSize,(ddrLutOffset + lutChunkHdrP->chunkLen));

         return SYSTEM__ERR_UNEXPECTED;
      }

      LOGG_PRINT(LOG_DEBUG_E, NULL, "lut = %d mem copy from 0x%x to 0x%x size %d\n",
              lutChunkHdrP->lutId,(void *)chunkP,(UINT32)ddrLutVirtBuff->data + ddrOffset,lutChunkHdrP->chunkLen);
      memcpy((void *)((UINT32)ddrLutVirtBuff->data + ddrOffset),      (void *)chunkP, lutChunkHdrP->chunkLen);
      LOGG_PRINT(LOG_DEBUG_E, NULL, "lutId %d chunk=%d size=%d saved to phys-%p offset 0x%x calibMode %d data %x rcv %x\n",
        lutChunkHdrP->lutId, lutChunkHdrP->chunkId, lutChunkHdrP->chunkLen,ddrLutPhyBuff->data+ddrOffset,
        ddrLutOffset,lutChunkHdrP->calibMode,*((unsigned int *)&ddrLutVirtBuff->data[ddrOffset]), *((UINT32 *)chunkP));
      if(lutChunkHdrP->chunkId == ( lutChunkHdrP->numOfChunks - 1 ))
      {
         if((ddrLutOffset + lutChunkHdrP->chunkLen) != lutChunkHdrP->lutSize)
         {
           LOGG_PRINT(LOG_ERROR_E, NULL, "Fail: iaeTransferByte is not equal to lutSize. iaeTransferByte = %d, lutSize = %d\n", ddrLutOffset + lutChunkHdrP->chunkLen, lutChunkHdrP->lutSize);
         }
         ddrLutVirtBuff->lutSize = lutChunkHdrP->lutSize;

         LOGG_PRINT(LOG_DEBUG_E, NULL, "Received LUT sensor %d lutId = %d ( mode %d calibMode %d) lut size %d\n", lutChunkHdrP->sensorInd, lutChunkHdrP->lutId,lutChunkHdrP->mode,lutChunkHdrP->calibMode,ddrLutVirtBuff->lutSize);
         //Send receive chunk lut Ack to host
         INTERNAL_CMDG_lutChunkHdrT lutTblAck;

         lutTblAck.lutId = lutChunkHdrP->lutId;
         inu_ref__copyAndSendDataAsync((inu_ref*)me,INTERNAL_CMDG_SEND_LUT_HDR_E,&lutTblAck, NULL, 0);
      }
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Received bad lutId = %d\n", lutChunkHdrP->lutId);
   }

   return(retCode);

}

ERRG_codeE lut_mngr_getIdsrLutAddress(void **idsrLutVirtAddress, void **idsrLutPhysAddress)

{
   //ASSUME: we have only one idsr lut for one sensor only in our system (it can be exist on vertical or full folder but same content). we don't know in runtime for which sensor we got the idsr lut
   //and we don't want to ask user to pass resolution mode,.
   // so, we search for the first data of idsr and return it.
   //this function does not support multi graph
   ERRG_codeE    retCode = RET_SUCCESS;
   UINT32 sensorId, ddrOffset, ddrSectionSize,readSize;

   for (sensorId = 0; sensorId<LUT_MNGR_NUM_SENSORS; sensorId++)
   {
         //printf("lut_mngr_getIdsrLutAddress sen %d virt 0x%x size %d\n",sensorId,lut_mngr_memPtr[LUT_MNGR_IDSR_LUT_ID_OFFSET + sensorId][0].virt,lut_mngr_memPtr[LUT_MNGR_IDSR_LUT_ID_OFFSET + sensorId][0].size);

         if (lut_mngr_memPtr[LUT_MNGR_IDSR_LUT_ID_OFFSET + sensorId][0].virt)
         {
            ddrSectionSize = (lut_mngr_memPtr[LUT_MNGR_IDSR_LUT_ID_OFFSET + sensorId][0].size) /(CALIB_NUM_LUT_MODES_E*CALIB_NUM_TEMPER_LUT_MODES_E);
            for (UINT32 calib = 0; calib < CALIB_NUM_LUT_MODES_E; calib++)
            {
               //printf("lut_mngr_getIdsrLutAddress calib %d add 0x%x val 0x%x\n",calib, lut_mngr_memPtr[LUT_MNGR_IDSR_LUT_ID_OFFSET + sensorId][0].virt+calib * ddrSectionSize,*((INT32*)(lut_mngr_memPtr[LUT_MNGR_IDSR_LUT_ID_OFFSET + sensorId][0].virt+calib * ddrSectionSize)));
               readSize = *(INT32*)(lut_mngr_memPtr[LUT_MNGR_IDSR_LUT_ID_OFFSET + sensorId][0].virt + calib * ddrSectionSize);
               //if we put here size of luts, so we fill content
               if (readSize != 0)
               {
                  *idsrLutVirtAddress = lut_mngr_memPtr[LUT_MNGR_IDSR_LUT_ID_OFFSET + sensorId][0].virt+ calib * ddrSectionSize;
                  *idsrLutPhysAddress = lut_mngr_memPtr[LUT_MNGR_IDSR_LUT_ID_OFFSET + sensorId][0].phys+ calib * ddrSectionSize;
                  LOGG_PRINT(LOG_INFO_E, NULL, "found idsr lut for sensor id %d calib mode %d, virt address: 0x%x  phys address 0x%x\n",sensorId, calib, *idsrLutVirtAddress,*idsrLutPhysAddress);
                  return retCode;
               }
            }
         }

   }

   LOGG_PRINT(LOG_ERROR_E, NULL, "IDSR lut did not load\n" );

   return HW_MNGR__ERR_INVALID_ARGS;
}


#if 0
static ERRG_codeE lut_mngr_gpStandaloneSendLut(inu_ref *ref,CALIB_sensorOperatingModeE calibMode,UINT8 *bufP, UINT32 bufSize)
{
   ERRG_codeE ret = SYSTEM__RET_SUCCESS;
   inu_device *me = (inu_device*)ref;
   UINT16                     chunkId;
   UINT32                     chunkLen    = LUT_MNGR_CHUNK_DATA_LEN;
   UINT32                     numOfChunks;
   UINT32                     lastchunkLen;
   UINT32                     lutByteSend = 0;
   INU_DEFSG_lutHeaderT        *lutHeader = ( INU_DEFSG_lutHeaderT *)(bufP);
   INTERNAL_CMDG_lutChunkHdrT lutChunkHdr;

   bufP     += INU_DEFSG_IAE_LUT_TABLE_HDR_LEN;
   bufSize  -= INU_DEFSG_IAE_LUT_TABLE_HDR_LEN;

   numOfChunks    = (bufSize + chunkLen -1)/chunkLen;
   lastchunkLen   = bufSize % chunkLen;
   if ( lastchunkLen == 0 )
      lastchunkLen= LUT_MNGR_CHUNK_DATA_LEN;

   for(chunkId = 0; chunkId < numOfChunks; )
   {
      if (chunkId == (numOfChunks-1))
      {
         chunkLen = lastchunkLen; //last lut chunk can be shorter
      }

      lutChunkHdr.lutId       = getLutIdAB(lutHeader->lutId);
      lutChunkHdr.mode        = getLutModeAB(lutHeader->lutId);
      lutChunkHdr.chunkId     = chunkId;
      lutChunkHdr.chunkLen    = chunkLen;
      lutChunkHdr.numOfChunks = numOfChunks;
      lutChunkHdr.lutSize     = bufSize;
      lutChunkHdr.calibMode  = calibMode;
      ret = lut_mngr_gpHandleLutChunkMsg(me, &lutChunkHdr, &bufP[lutByteSend]);

      if(ERRG_SUCCEEDED(ret))
      {
         chunkId++;
         lutByteSend += chunkLen;
      }
      else
      {
         if(ERRG_GET_MID(ret) == MEM_POOL)
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "failed alloc lut buf for lutId = %d chunkId = %d retry\n", lutHeader->lutId, chunkId);
         }
         else
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "failed channel send lut data -retry\n");
         }
         OS_LYRG_usleep(1000);
      }

   }
   return ret;

}


static int lut_mngr_gpStandaloneLoadLut(inu_ref *ref,CALIB_sensorOperatingModeE calibMode,UINT32 lutId, char lutFilePath[])
{
  unsigned char *LutPtr = NULL;
  inu_device_memory_t lutfile;
  inu_device_lut_hdr_t lutHdr;
  UINT32 size2malloc;

  if ( inu_device__readBinFile(lutFilePath, &lutfile))
  {
     size2malloc = sizeof(inu_device_lut_hdr_t) + lutfile.bufsize;
     LutPtr = malloc(size2malloc);
     lutHdr.lutId = lutId;
     memcpy(LutPtr, &lutHdr, sizeof(lutHdr));
     memcpy(LutPtr + sizeof(lutHdr), lutfile.bufP, lutfile.bufsize);
     lut_mngr_gpStandaloneSendLut(ref, calibMode,LutPtr, size2malloc);
     free(LutPtr);
     free(lutfile.bufP);
  }
  return 0;
}

#endif


#define GP_LUT_TEST 0
#if GP_LUT_TEST
#define PATH_SEPARATOR "/"
static BOOLEAN lut_mngr_readBinFile(const char *fileName, inu_device_memory_t *buffer)
{
    FILE *file;
    file = fopen(fileName, "rb");  // r for read, b for binary
    if (file)
    {
        fseek(file, 0, SEEK_END);
        buffer->bufsize = ftell(file);
        fseek(file, 0, SEEK_SET);
        buffer->bufP = (char *)malloc((buffer->bufsize) + 1);
        fread(buffer->bufP, buffer->bufsize, 1, file);
        fclose(file);
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

ERRG_codeE lut_mngr_gpTestLuts(CALIB_sensorLutModeE calibMode,
                              inu_device_memory_t *lutfile ,
                               inu_device_lut_hdr_t *lutHdr)
{
   ERRG_codeE             retCode     = SYSTEM__RET_SUCCESS;
   int                    shared,calibModeXml;
   UINT32                 ddrStartInd,totalSize,bypassBitMap;
   MEM_MAPG_addrT         iaeLutVirtsAddr;
   MEM_MAPG_addrT         iaeLutPhyAddr;
   CDE_MNGRG_channnelInfoT   *channelInfo;
   lut_mngr_ddrLut        *ddrLutVirtBuff=NULL, *ddrLutPhyBuff=NULL;
   UINT16 mode=0,lutStartInd,subLutInd,dsrOrIb,ind;
   lut_mngr_ptrs_T        *lutEntryP;

  if (calibMode < CALIB_NUM_LUT_MODES_E)
  {
      lut_mngr_gpGetLutDdrBufAddr(calibMode,
              CALIB_LUT_TEMPER_MODE_0_E,
              lutHdr->lutId,
              0,
              lut_mngr_doubleBufferIdx,
              (MEM_MAPG_addrT *)&ddrLutVirtBuff,
              (MEM_MAPG_addrT *)&ddrLutPhyBuff,
              &shared,
              &totalSize, &lutEntryP);


      if (ddrLutVirtBuff->lutSize != 0)
      {

        printf("++++++++++++++lutfile->bufsize %x data  %x %x %x %x\n",
              lutfile->bufsize,
              ((UINT32 *)lutfile->bufP)[0],
              ((UINT32 *)lutfile->bufP)[1],
              ((UINT32 *)lutfile->bufP)[2],
              ((UINT32 *)lutfile->bufP)[3]);
          //DMA memcpy from ddr to iae
  //       retCode = CDE_MNGRG_memcpyOpenChan(&channelInfo, ddrLutVirtBuff->lutSize, NULL, NULL);
         if(ERRG_SUCCEEDED(retCode))
         {
             printf( "Testcopy lut %d from ddrLutPhyAddr 0x%x size %x calibMode %d sensorInd %d lut %d\n",
                     lutHdr->lutId,
                     (UINT32)ddrLutPhyBuff->data,
                     ddrLutVirtBuff->lutSize,
                     calibMode,
                     lutHdr->sensorInd,
                     lutHdr->lutId);
//             retCode = CDE_MNGRG_memcpyBlock(channelInfo,(UINT32)iaeLutPhyAddr,(UINT32)ddrLutPhyBuff->data,NULL);
             for (ind=0;ind<lutfile->bufsize;ind++)
             {
                if (ddrLutVirtBuff->data[ind] != lutfile->bufP[ind])
                {
                  printf("-------------ERRR %x %x\n",ddrLutVirtBuff->data[ind], lutfile->bufP[ind]);
                 }
              }
         }
      }
   }

   return(retCode);

}

static BOOLEAN lut_mngr_readTestFile(CALIB_sensorLutModeE sensorMode,char filebin[], UINT32 lutId,unsigned int sensorInd)
{
   ERRG_codeE status;
   char *LutPtr = NULL;
   inu_device_memory_t lutfile;
   inu_device_lut_hdr_t lutHdr;
   UINT32 size2malloc;

   if (lut_mngr_readBinFile(filebin, &lutfile) == TRUE)
   {
      size2malloc = sizeof(inu_device_lut_hdr_t) + lutfile.bufsize;
      LutPtr = (char*)malloc(size2malloc);
      lutHdr.lutId = lutId;
      lutHdr.sensorInd = sensorInd;
      memcpy(LutPtr, &lutHdr, sizeof(lutHdr));
      memcpy(LutPtr + sizeof(lutHdr), lutfile.bufP, lutfile.bufsize);


      status = lut_mngr_gpTestLuts(sensorMode, &lutfile, &lutHdr);
      free(LutPtr);
      free(lutfile.bufP);
      return TRUE;
   }
   return FALSE;
}

void lut_mngr_testLutFiles(char *calibPath)
{
   char filebin[500];
   unsigned int lutInd, sensorInd, dsrIbInd;
   unsigned int startLutOffset[2] = { 32 ,0 };
   CALIB_sensorLutModeE sensorMode;
   char *sensTypes[3]={"Binning","Full","VerticalBinning"};
   char *dsrIbStr[2]={"dsr","ib"};
   char *rightLeft[2]={"right","left"};
   BOOLEAN fileExists=TRUE;
   printf("++++++++++++++++++++++START TEST\n");
 //  NUCFG_getSensorsFromIau(iauIndexes);
  // calibPath[strlen(calibPath)-4]=0; //*************************************$$$$$$$$$$$ remove only for debug
   for (sensorInd =0; sensorInd<8; sensorInd++)
   {
     for (sensorMode = CALIB_LUT_MODE_BIN_E;sensorMode<CALIB_NUM_LUT_MODES_E;sensorMode = (CALIB_sensorLutModeE)(sensorMode + 1))
     {
       for (dsrIbInd =0; dsrIbInd<2; dsrIbInd++)
        {
            for (lutInd =0; lutInd<LUT_MNGR_NUM_IAE_SUB_LUT; lutInd++)
            {
              sprintf(filebin,"%s%s%sNU4K%ssensor_%d_%d_%s.bin",calibPath,sensTypes[sensorMode%CALIB_NUM_LUT_MODES_E], PATH_SEPARATOR, PATH_SEPARATOR, sensorInd, lutInd,dsrIbStr[dsrIbInd]);
              printf("----------%s\n",filebin);
              if ( lut_mngr_readTestFile(sensorMode,filebin, startLutOffset[dsrIbInd] +sensorInd*4+ lutInd,sensorInd) == FALSE)
                 fileExists=FALSE;
            }
        }
      }
   }
   printf("++++++++++++++++++++++END TEST\n");
}

#endif

