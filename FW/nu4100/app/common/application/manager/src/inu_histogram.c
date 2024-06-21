#include "inu_types.h"
#include "err_defs.h"
#include "internal_cmd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inu2_internal.h"
#include "nucfg.h"
#include "log.h"
#include "assert.h"

#if DEFSG_IS_GP
#include "iae_mngr.h"
#include "sequence_mngr.h"
#include "mem_map.h"
#include "iae_drv.h"
#include "cde_mngr_new.h"
#include "data_base.h"
#include "os_lyr.h"
#include "hcg_mngr.h"
#include "cmem.h"
#include "proj_drv.h"
#include "inu_alt.h"
#endif


// workaround  to increase data index while we have not strobe data, like in HP project, to allow sensor control.
// when HW with strobe will came need to roll back this change
#define INU_HISTOGRAM_STROBE_WA

typedef struct inu_histogram__privData
{
   inu_histogram__CtorParams ctorParams;
   MEM_POOLG_handleT memPoolH;
   char name[48];
   inu_sensors_group *sourceSensorGroup;
#if DEFSG_IS_GP
   CDE_MNGRG_channnelInfoT *dmaChanInfoP;
#endif
   MEM_POOLG_bufDescT   *dmaBufDescP;
   inu_histogram_data   *histogramData[4];
   UINT32               numOutputs;
#if DEFSG_IS_GP
   SEQ_MNGRG_channelH   channelH;
#endif
#ifdef INU_HISTOGRAM_STROBE_WA
   UINT32               ctr;
#endif
}inu_histogram__privData;

#if DEFSG_IS_GP
#ifdef NO_HW_WORKAROUND
static void inu_histogram__histDmaDoneCb(CDE_MNGRG_userCbParamsT *cbParam, void *arg);
#endif
#endif
static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;


static const char* inu_histogram__name(inu_ref *me)
{
   inu_histogram *histogram = (inu_histogram*)me;
   inu_histogram__privData *privP = (inu_histogram__privData*)histogram->privP;
   return privP->name;
}

static void inu_histogram__dtor(inu_ref *me)
{
   inu_histogram__privData *privP = (inu_histogram__privData*)((inu_histogram*)me)->privP;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#if DEFSG_IS_GP
   if (privP->ctorParams.engine == INU_HISTOGRAM__IAU_HW)
   {
#ifdef NO_HW_WORKAROUND
      CDE_MNGRG_memcpyCloseChan(privP->dmaChanInfoP);
      //dma channel is stopped and closed, check if it was in the middle of operation and clear the bufDescP
      if (privP->dmaBufDescP)
      {
         MEM_POOLG_free(privP->dmaBufDescP);
      }
#endif
   }

   UINT32                   sleep = 0;
   //wait for all buffers to free
   while(MEM_POOLG_getNumFree(privP->memPoolH) < MEM_POOLG_getNumBufs(privP->memPoolH))
   {
      OS_LYRG_usleep(5000);
      sleep += 5000;
      if(sleep >= (100*5000))
      {
         assert(0);
         break;
      }
   }
   MEM_POOLG_closePool(privP->memPoolH);
#endif

   free(privP);
}

/* Constructor */
ERRG_codeE inu_histogram__ctor(inu_histogram *me, inu_histogram__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_histogram__privData *privP;
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if(ERRG_SUCCEEDED(ret))
   {
      privP = (inu_histogram__privData*)malloc(sizeof(inu_histogram__privData));
      if(privP)
      {
         char nucfgChId[20];
         memset(privP, 0, sizeof(inu_histogram__privData));
         memcpy(&privP->ctorParams, ctorParamsP, sizeof(inu_histogram__CtorParams));
         me->privP = privP;
         strcpy(privP->name,"INU_HISTOGRAM");
         if (privP->ctorParams.engine == INU_HISTOGRAM__IAU_HW)
         {
            sprintf(nucfgChId, " iau: %d", privP->ctorParams.nuCfgChId);
            strcat(privP->name,nucfgChId);
#if DEFSG_IS_GP
#ifdef NO_HW_WORKAROUND
            UINT32 size;
            MEM_MAPG_getSize((MEM_MAPG_IAE_LUT_HIST_0_E + privP->ctorParams.nuCfgChId), &size);
            ret = CDE_MNGRG_memcpyOpenChan(&privP->dmaChanInfoP, size, inu_histogram__histDmaDoneCb, me);
#endif
#endif
         }
      }
      else
      {
         ret = INU_HISTOGRAM__ERR_OUT_OF_MEM;
      }
   }

#if DEFSG_IS_GP
   if(ERRG_SUCCEEDED(ret))
   {
      MEM_POOLG_cfgT poolCfg;

      MEM_MAPG_getSize(MEM_MAPG_IAE_LUT_HIST_0_E,&poolCfg.bufferSize);
      poolCfg.numBuffers         = 8;//todo take from factory
      poolCfg.freeCb             = NULL;
      poolCfg.resetBufPtrInAlloc = FALSE;
      poolCfg.type               = MEM_POOLG_TYPE_ALLOC_CMEM_E;
      poolCfg.memP               = NULL;
      ret = MEM_POOLG_initPool(&(privP->memPoolH), &poolCfg);
   }
#endif
   return ret;
}

static void inu_histogram__newOutput(inu_node *me, inu_node *output)
{
   inu_function__vtable_get()->node_vtable.p_newOutput(me,output);
#if DEFSG_IS_GP
   UINT32 i;
   inu_histogram__privData *privP = (inu_histogram__privData*)((inu_histogram*)me)->privP;
   if (inu_ref__getRefType((inu_ref*)output) == INU_HISTOGRAM_DATA_REF_TYPE)
   {
      //save to ease access
      for( i = 0; i < 4; i++)
      {
         if (!privP->histogramData[i])
         {
            privP->histogramData[i] = (inu_histogram_data*)output;
            privP->numOutputs++;
            break;
         }
      }
   }
#endif
}

static int inu_histogram__rxIoctl(inu_ref *ref, void *msgP, int msgCode)
{
   inu_histogram *me = (inu_histogram*)ref;
   inu_histogram__privData *privP = (inu_histogram__privData*)me->privP;
   int ret = INU_HISTOGRAM__RET_SUCCESS;

   ret = inu_function__vtable_get()->node_vtable.ref_vtable.p_rxSyncCtrl(ref, msgP, msgCode);
#if DEFSG_IS_GP
   inu_histogram_data__hdr_t *histogramHdr;
   inu_image__hdr_t          *imageHdrP;
   UINT32                    i,j;

   switch (msgCode)
   {
      case(INTERNAL_CMDG_HISTOGRAM_ROI_CFG_E):
      {
         IAE_DRVG_histRoiCfgT *histCfgP = (IAE_DRVG_histRoiCfgT*)msgP;
         INU_REF__LOGG_PRINT(me, LOG_WARN_E, ret,"cfg roi! hw %d, %d %d %d %d\n",privP->ctorParams.engine,histCfgP->roi[0].x0,histCfgP->roi[0].x1,histCfgP->roi[0].y0,histCfgP->roi[0].y1);
         if (privP->ctorParams.engine == INU_HISTOGRAM__IAU_HW)
         {
            HCG_MNGRG_voteUnits((1 << HCG_MNGRG_IAE));
            ret = IAE_DRVG_histRoiCfg(privP->ctorParams.nuCfgChId, histCfgP);
            HCG_MNGRG_devoteUnits((1 << HCG_MNGRG_IAE));
            //hw histogram node creates only one histogram data
            histogramHdr = (inu_histogram_data__hdr_t*)inu_data__getHdr(privP->histogramData[0]);
            for (j=0; j < INU_HISTOGRAM__ROI_NUM; j++)
            {
               histogramHdr->roi[j].x0 = histCfgP->roi[j].x0;
               histogramHdr->roi[j].x1 = histCfgP->roi[j].x1;
               histogramHdr->roi[j].y0 = histCfgP->roi[j].y0;
               histogramHdr->roi[j].y1 = histCfgP->roi[j].y1;
            }
         }
         else
         {
            inu_image *imageP = (inu_image*)inu_node__getNextInputNode(ref,NULL);
            if (!inu_ref__instanceOf(imageP, INU_IMAGE_REF_TYPE))
            {
               printf("histogram %d input is not image\n",me->function.node.ref.id);
               assert(0);
            }

            //assumption - only one input
            imageHdrP = (inu_image__hdr_t*)inu_data__getHdr(imageP);
            //update histograms roi per frame (ceva histogram creates multiply histograms data each for interleaved frame)
            for(i=0; i<imageHdrP->imgDescriptor.numInterleaveImages; i++)
            {
                histogramHdr = (inu_histogram_data__hdr_t*)inu_data__getHdr(privP->histogramData[i]);
                for (j=0; j < INU_HISTOGRAM__ROI_NUM; j++)
                {
                   histogramHdr->roi[j].x0 = histCfgP->roi[j].x0;
                   histogramHdr->roi[j].x1 = histCfgP->roi[j].x1;
                   histogramHdr->roi[j].y0 = histCfgP->roi[j].y0;
                   histogramHdr->roi[j].y1 = histCfgP->roi[j].y1;
                }
            }

            ret = INU_HISTOGRAM__ERR_NOT_SUPPORTED;
         }
         break;
      }
   }
#else
   FIX_UNUSED_PARAM_WARN(privP);
#endif
   return ret;
}

#if DEFSG_IS_GP
#ifdef NO_HW_WORKAROUND
//todo - from operate work thread
static void inu_histogram__histDmaDoneCb(CDE_MNGRG_userCbParamsT *cbParam, void *arg)
{
   inu_histogram__privData   *privP = (inu_histogram__privData*)((inu_histogram*)arg)->privP;
   inu_histogram_data        *data = (inu_histogram_data*)cbParam->buffDescriptorP;
   MEM_POOLG_bufDescT        *bufDescP;
   inu_histogram_data__hdr_t *histogramHdrP;
   int i;
   UINT32 *tempP;

   CDE_MNGRG_stopChannelMemcpy(privP->dmaChanInfoP);
   privP->dmaBufDescP = NULL;

   inu_data__bufDescPtrGet((inu_data*)data,&bufDescP);
   histogramHdrP = (inu_histogram_data__hdr_t*)((inu_data__VTable*)data->data.node.ref.p_vtable)->p_dataHdrGet((inu_data*)data);

   histogramHdrP->hwFrameCounter    = bufDescP->dataP[0x2424];
   tempP = (UINT32*)bufDescP->dataP;
   for(i = 0; i < 3; i++)
   {
      histogramHdrP->accumulator[i] = tempP[(0x2400/4)+(i*(0xc/4))+(privP->ctorParams.nuCfgChId*(0x2428/4))];
   }

   inu_function__doneData(arg, (inu_data*)data);
}
#endif

/****************************************************************************
*
*  Function Name: inu_histogram__receiveHistogram
*
*  Description:   invokes DMA memcpy for copying the histogram.
*                     in B0, we would like to use the histogram -> dma -> cpu flow (using last flag)
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
static void inu_histogram__receiveHistogram(void *me, UINT64 timestamp, UINT32 hist)
{
   ERRG_codeE                retCode;
   UINT32                    size;
   MEM_POOLG_bufDescT        *bufDescP = NULL;
#ifdef NO_HW_WORKAROUND
   UINT32                    bufferPhyAddress;
   MEM_MAPG_addrT            histPhyAddressP;
   inu_node                  *node;
   inu_histogram_data        *hist_data;
   inu_histogram_data__hdr_t histogramHdr;
#endif
   SEQ_MNGRG_handleT         handle = inu_graph__getSeqDB(inu_node__getGraph(me));
   inu_histogram__privData   *privP = (inu_histogram__privData*)((inu_histogram*)me)->privP;

   //if input is writer, release grant
   if (inu_ref__getRefType(inu_node__getNextInputNode(me,NULL)) == INU_SOC_WRITER_REF_TYPE)
   {
      void *grantEntry;
      retCode = SEQ_MNGRG_writerSchedulerReleaseGrant(handle, privP->ctorParams.nuCfgChId, SEQ_MNGRG_DONE_BLOCK_CHANNEL_E,&grantEntry);
      if (grantEntry)
      {
         SEQ_MNGRG_pauseChannelPaths(handle, privP->channelH);
         retCode = SEQ_MNGRG_writerSchedulerReleaseGrantComplete(handle, grantEntry);
      }
   }

   //first we create a inu_histogram_data, start to fill the header and pass it as a cookie to cde memcpy
#ifdef NO_HW_WORKAROUND
   node = inu_node__getNextOutputNode(me,NULL);
   if (!node)
   {
      assert(0);
   }

   privP->ctr++;
   histogramHdr.dataHdr.dataIndex = privP->ctr;
   histogramHdr.dataHdr.timestamp = timestamp;
   retCode = IAE_DRVG_histGetRoiCfg(hist, (IAE_DRVG_histRoiCfgT*)&histogramHdr.roi);
   if (ERRG_FAILED(retCode))
   {
      INU_REF__LOGG_PRINT(me, LOG_WARN_E, retCode, "could not get ROI dims\n");
   }
#endif
   retCode = MEM_POOLG_alloc(privP->memPoolH, MEM_POOLG_getBufSize(privP->memPoolH), &bufDescP);
   if(ERRG_SUCCEEDED(retCode) && bufDescP)
   {
      MEM_MAPG_getSize((MEM_MAPG_IAE_LUT_HIST_0_E + hist), &size);
      bufDescP->dataLen = size;

#ifdef NO_HW_WORKAROUND
      retCode = MEM_POOLG_getDataPhyAddr(bufDescP,&bufferPhyAddress);
      if (ERRG_SUCCEEDED(retCode))
      {
         MEM_MAPG_getPhyAddr((MEM_MAPG_IAE_LUT_HIST_0_E + hist),&histPhyAddressP);
         retCode = inu_function__newData((inu_function*)me,(inu_data*)node,bufDescP,&histogramHdr,(inu_data**)&hist_data);
         if (ERRG_FAILED(retCode))
         {
            return;
         }

         retCode = CDE_MNGRG_memcpy(privP->dmaChanInfoP, bufferPhyAddress,(UINT32)histPhyAddressP, hist_data);
#else
         retCode = inu__reuseHistMemcpyMsg(me, timestamp, hist, bufDescP);
#endif

#ifdef NO_HW_WORKAROUND
         if (ERRG_FAILED(retCode))
         {
            inu_data__freeInternal((inu_data*)hist_data);
         }
         else
         {
            //memcpy success. save the bufDescP incase we will want to stop the channel and the DMA will not complete by then.
            privP->dmaBufDescP = bufDescP;
         }
      }
#endif
   }
   else
   {
      INU_REF__LOGG_PRINT(me, LOG_WARN_E, retCode, "alloc buffer failed (pool = %p, free = %d, total = %d)\n",privP->memPoolH,MEM_POOLG_getNumFree(privP->memPoolH),MEM_POOLG_getNumBufs(privP->memPoolH));
   }
}
#endif

static ERRG_codeE inu_histogram__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   inu_sensors_group *sensorGroupP;
   UINT32     i;
   inu_histogram__privData *privP = (inu_histogram__privData*)((inu_histogram*)me)->privP;
   SEQ_MNGRG_handleT handle;

   ret = inu_function__vtable_get()->p_start(me, startParamP);
   if (ERRG_SUCCEEDED(ret))
   {
      //find input sensor group for saving sensor data on output
      ret = inu_graph__getOrigSensGroup(me, (inu_nodeH*)&sensorGroupP);
      if (ERRG_SUCCEEDED(ret))
      {
         privP->sourceSensorGroup = sensorGroupP;
      }
      else
      {
         privP->sourceSensorGroup = NULL;
      }

      if (privP->ctorParams.engine == INU_HISTOGRAM__IAU_HW)
      {
         handle = inu_graph__getSeqDB(inu_node__getGraph(me));
         if (handle)
         {
            ret = SEQ_MNGRG_configChannel(handle, SEQ_MNGRG_XML_BLOCK_HIST_E, privP->ctorParams.nuCfgChId,TRUE, me, &privP->channelH);
            if (ERRG_SUCCEEDED(ret))
            {
               ret = SEQ_MNGRG_startChannel(handle, SEQ_MNGRG_XML_BLOCK_HIST_E, privP->ctorParams.nuCfgChId,(void*)inu_histogram__receiveHistogram,(void *)me, privP->channelH);
            }
            //if input is writer, provide channel
            if ((ERRG_SUCCEEDED(ret)) && (inu_ref__getRefType(inu_node__getNextInputNode(me,NULL)) == INU_SOC_WRITER_REF_TYPE))
            {
               inu_node *node;
               node = inu_node__getNextInputNode(me,NULL);
               while(node)
               {
                  inu_soc_writer__fillOutputChannel((inu_soc_writer*)node,privP->channelH);
                  node = inu_node__getNextInputNode(me,node);
               }
            }
         }
         else
         {
            INU_REF__LOGG_PRINT(me, LOG_ERROR_E, NULL, "Failed to get seq DB!\n");
         }
      }
      else
      {
         inu_nodeH node;
         inu_image *image;
         DATA_BASEG_histogramDataBaseT *histDbP;
         inu_histogram_data__hdr_t     *histogramHdr = NULL;

         node = inu_node__getNextInputNode(me,NULL);
         if (inu_ref__instanceOf(node, INU_IMAGE_REF_TYPE))
         {
            image = (inu_image*)node;
         }
         else
         {
            printf("histogram %d input is not image\n",me->node.ref.id);
            assert(0);
         }

         for ( i = 0; i < 4; i++)
         {
            if (privP->histogramData[i])
            {
               histogramHdr = (inu_histogram_data__hdr_t*)inu_data__getHdr(privP->histogramData[i]);
               //use the input image size as the roi - 1
               histogramHdr->roi[0].x0 = 0;
               histogramHdr->roi[0].x1 = image->imageHdr.imgDescriptor.width - 1;
               histogramHdr->roi[0].y0 = 0;
               histogramHdr->roi[0].y1 = image->imageHdr.imgDescriptor.height - 1;
            }
         }

         DATA_BASEG_accessDataBaseNoMutex((UINT8**)&histDbP,DATA_BASEG_HISTOGRAM);
         memcpy(&histDbP->imgDescriptor, &image->imageHdr.imgDescriptor, sizeof(inu_image__descriptor));
         histDbP->rightShift = 2;
         histDbP->x0 = histogramHdr->roi[0].x0;
         histDbP->x1 = histogramHdr->roi[0].x1;
         histDbP->y0 = histogramHdr->roi[0].y0;
         histDbP->y1 = histogramHdr->roi[0].y1;
         ret = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)me),DATA_BASEG_ALG_HISTOGRAM,1,ICCG_CMD_TARGET_DSPA);
         if (ERRG_FAILED(ret))
         {
            INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
         }
      }
   }
#else
   ret = inu_function__vtable_get()->p_start(me, startParamP);
#endif
   return ret;
}


static ERRG_codeE inu_histogram__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   SEQ_MNGRG_handleT handle;
   inu_histogram__privData *privP = (inu_histogram__privData*)((inu_histogram*)me)->privP;
   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
   if (ERRG_SUCCEEDED(ret))
   {
      if (privP->ctorParams.engine == INU_HISTOGRAM__IAU_HW)
      {
         handle = inu_graph__getSeqDB(inu_node__getGraph(me));
         if (ERRG_SUCCEEDED(ret))
         {
            ret = SEQ_MNGRG_stopChannel(handle, SEQ_MNGRG_XML_BLOCK_HIST_E, privP->ctorParams.nuCfgChId,me, privP->channelH);
         }
         else
         {
            INU_REF__LOGG_PRINT(me, LOG_ERROR_E, NULL, "Failed to get seq DB!\n");
         }
      }
      else
      {
         //call stop to DSP here
      }
   }
#else
   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
#endif
   return ret;
}

#if DEFSG_IS_GP
static void inu_histogram__operate(inu_function *me, inu_function__operateParamsT *inputParamsP)
{
   inu_function__vtable_get()->p_operate(me, inputParamsP);
   ERRG_codeE                  ret;
   MEM_POOLG_bufDescT          *bufDescP = NULL;
   inu_histogram_data__hdr_t   histogramHdr;
   inu_image__hdr_t            *imageHdrP;
   inu_histogram_data          *histogram_data;
   UINT32                      i;
   //sanity
   inu_histogram__privData *privP = (inu_histogram__privData*)((inu_histogram*)me)->privP;
   if (privP->ctorParams.engine == INU_HISTOGRAM__IAU_HW)
   {
      assert(0);
   }

   //assumption - only one input image
   imageHdrP    = (inu_image__hdr_t*)inu_data__getHdr(inputParamsP->dataInputs[0]);

   //create histogram data per image (num interleaved)
   for(i=0; i<imageHdrP->imgDescriptor.numInterleaveImages; i++)
   {
      ret = MEM_POOLG_alloc(privP->memPoolH, MEM_POOLG_getBufSize(privP->memPoolH), &bufDescP);
      if(ERRG_SUCCEEDED(ret) && bufDescP)
      {
         memcpy(&histogramHdr,inu_data__getHdr(privP->histogramData[i]), sizeof(inu_histogram_data__hdr_t));//copy histogram roi size
         histogramHdr.dataHdr = imageHdrP->dataHdr; //copy timestamp and index from image
         bufDescP->dataLen    = bufDescP->size; //histogram size does not change
         INU_REF__LOGG_PRINT(me,LOG_DEBUG_E,NULL,"create new data: len %d, indx %llu, (%d,%d %d,%d)\n",bufDescP->dataLen,histogramHdr.dataHdr.dataIndex,histogramHdr.roi[0].x0,histogramHdr.roi[0].y0,histogramHdr.roi[0].x1,histogramHdr.roi[0].y1);
         ret = inu_function__newData(me,(inu_data*)privP->histogramData[i],bufDescP,&histogramHdr,(inu_data**)&histogram_data);
         if (ERRG_FAILED(ret))
         {
            assert(0);
            return;
         }
         inputParamsP->dataInputs[inputParamsP->dataInputsNum] = histogram_data;
         inputParamsP->dataInputsNum++;
      }
      else
      {
         INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"mem alloc failed\n");
      }
   }

   ret = inu_function__sendDspMsg(me,inputParamsP, DATA_BASEG_ALG_HISTOGRAM, 0, 0, ICCG_CMD_TARGET_DSPA);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
   }

   if(ERRG_FAILED(ret))
   {
      for(i=0;i<inputParamsP->dataInputsNum;i++)
      {
         inu_function__doneData(me, inputParamsP->dataInputs[i]);
      }
      inu_function__complete(me);
   }
}


static void inu_histogram__dspAck(inu_function *me, inu_function__operateParamsT *inputParamsP, inu_function__coreE dspSource)
{
   FIX_UNUSED_PARAM_WARN(dspSource);
   UINT32 i;

   for(i=0;i<inputParamsP->dataInputsNum;i++)
   {
      inu_function__doneData(me, inputParamsP->dataInputs[i]);
   }
   inu_function__complete(me);
}
#endif

void inu_histogram__vtable_init( void )
{
   if (!_bool_vtable_initialized)
   {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_histogram__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_histogram__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_histogram__ctor;
      _vtable.node_vtable.ref_vtable.p_rxSyncCtrl = inu_histogram__rxIoctl;

      _vtable.node_vtable.p_newOutput = inu_histogram__newOutput;

      _vtable.p_start   = inu_histogram__start;
      _vtable.p_stop    = inu_histogram__stop;
#if DEFSG_IS_GP
      _vtable.p_operate = inu_histogram__operate;
      _vtable.p_dspAck  = inu_histogram__dspAck;
#endif
      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_histogram__vtable_get(void)
{
   inu_histogram__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}

ERRG_codeE inu_histogram__cfgRoi(inu_histogramH meH, inu_histogram__roiCfgT *cfgParamsP)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_HISTOGRAM_ROI_CFG_E,cfgParamsP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}


#if DEFSG_IS_GP
#include "cva_mngr.h"
#define REUSE_MSGQ_NAME       "/ReuseMemDataQue"
#define TIME_TO_WAIT_FOR_IIC_READY_U (120)
#define TIME_OUT_WAITING (60 * TIME_TO_WAIT_FOR_IIC_READY_U)
static OS_LYRG_threadHandle inu_memreuse__thrdH = NULL;
static OS_LYRG_msgQueT      inu_memreuse__msgQue;

#define INU_HISTOGRAM__THREAD_WAKEUP_MSEC  (1000*60)

typedef enum
{
   MEM_CPY_HIST            = 0,
   UPDATE_IIC_READY           ,
} inu__reuseMsgTypeT;

typedef struct
{
   inu__reuseMsgTypeT      msgType;
   UINT32                  params[2];
   UINT64                  paramU64;
   MEM_POOLG_bufDescT*     paramsBufDedc;
} inu_reuseMsgT;

#if 0
static void print_hist(UINT8 *dataP)
{
   int i, roi;
   UINT32 *temp_32 = (UINT32*)dataP;
   UINT32 bins[256];
   UINT32 accumulator, testAccumulator;

   for (roi = 0; roi < 3; roi++)
   {
      printf("roi %d:\n", roi);
      testAccumulator = 0;

      for (i = 0; i < (0x100); i++)
      {
         bins[i] = temp_32[i + (roi*(0xc00 / 4))];
         printf("%d ", bins[i]);
      }

      for (i = 0; i < (0x100); i++)
      {
         testAccumulator += i * bins[i];
      }

      accumulator = temp_32[(0x2400 / 4) + (roi*(0xc / 4))];
      printf("\ntestAccumulator = %d accumulator %d, counter %d\n", testAccumulator, accumulator, temp_32[(0x2424 / 4) + (roi*(0xc / 4))]);
   }
   printf("\n\n\n");
}
#endif

/*
   After changing toolchain from 4.7 to 4.9, using memcpy caused missed interrupt from histogram HW.
   It seems the library memcpy implementation has changed. The inu_histMemCpy produces the same
   performance and does not cause the phenomena
*/
void inu_histMemCpy(int *restrict dst, int *restrict src, int len)
{
   int i;
   for(i = 0; i < (len/4); i++)
   {
      dst[i] = src[i];
   }
}

ERRG_codeE inu__reuseMemHandleMsg(inu_reuseMsgT *msgP)
{
   ERRG_codeE retCode;

   switch(msgP->msgType)
   {
      case (MEM_CPY_HIST):
      {
         inu_histogram_data__hdr_t histogramHdr;
         MEM_MAPG_addrT            histVirtAddressP;
         UINT32                    hist = msgP->params[1];
         MEM_POOLG_bufDescT       *bufDescP;
         inu_histogram            *me = (inu_histogram*)msgP->params[0];
         inu_histogram__privData  *privP = (inu_histogram__privData*)me->privP;
         inu_node                 *node;
         inu_histogram_data       *hist_data;
         UINT32                   *tempP, projMode;
         int                       i;
         UINT64                    temp;
         inu_function__stateParam stateParam;

         HCG_MNGRG_voteUnit( HCG_MNGRG_IAE );
         HCG_MNGRG_voteUnit( HCG_MNGRG_PPE );
         HCG_MNGRG_voteUnit( HCG_MNGRG_CVA );

         memset(&histogramHdr,0,sizeof(inu_histogram_data__hdr_t));

         //first we create a inu_histogram_data, start to fill the header
         retCode = inu_sensors_group__getStrobeData(privP->sourceSensorGroup,
                                                    &histogramHdr.dataHdr.timestamp,
                                                    &histogramHdr.dataHdr.dataIndex,
                                                    &projMode);
         if (ERRG_FAILED(retCode))
         {
#ifndef INU_HISTOGRAM_STROBE_WA
            INU_REF__LOGG_PRINT(me, LOG_WARN_E, retCode, "histogram isr without strobe isr data index %d\n", histogramHdr.dataHdr.dataIndex);
#else
            privP->ctr++; //need to increase data index
            histogramHdr.dataHdr.dataIndex = privP->ctr;
#endif
         }

         inu_data__setMode((inu_data__hdr_t*)&histogramHdr,ALTG_getThisFrameMode(inu_sensors_group__getAltHandle(privP->sourceSensorGroup)));
         node = (inu_node*)privP->histogramData[0];
         if (!node)
         {
            assert(0);
         }

         retCode = IAE_DRVG_histGetRoiCfg(hist, (IAE_DRVG_histRoiCfgT*)&histogramHdr.roi);
         if (ERRG_FAILED(retCode))
         {
            INU_REF__LOGG_PRINT(me, LOG_WARN_E, retCode, "could not get ROI dims\n");
         }
         else
         {
            UINT64 startUsec, currentUsec;
            bufDescP = msgP->paramsBufDedc;

            MEM_MAPG_getVirtAddr((MEM_MAPG_IAE_LUT_HIST_0_E + hist),&histVirtAddressP);

            if (CVA_MNGRG_isDdrlessConfigured())
            {
               if(CVA_MNGRG_isLeakyModeConfigured())
               {
                  CVA_MNGRG_setLeakyOpen(0);
                  OS_LYRG_usleep(TIME_TO_WAIT_FOR_IIC_READY_U);
               }
               else
               {
                   OS_LYRG_getUsecTime(&startUsec);
                   while (CVA_MNGRG_getIicReady())
                   {
                      OS_LYRG_usleep(TIME_TO_WAIT_FOR_IIC_READY_U);
                      OS_LYRG_getUsecTime(&currentUsec);
                      if ((currentUsec - startUsec) > TIME_OUT_WAITING)
                      {
                          INU_REF__LOGG_PRINT(me, LOG_WARN_E, NULL, "Time out waiting for freak to finish, continue and hope for the best\n");
                          break;
                      }
                   }
               }
            }
#if 1
            inu_histMemCpy((void*)bufDescP->dataP,(void*)histVirtAddressP,bufDescP->dataLen);
#else
            memcpy((volatile void*)bufDescP->dataP,(volatile void*)histVirtAddressP,bufDescP->dataLen);
#endif
            if (CVA_MNGRG_isDdrlessConfigured())
            {
               if(CVA_MNGRG_isLeakyModeConfigured())
               {
                    CVA_MNGRG_setLeakyOpen(1);
               }
            }

            //if function is stopped, but the HW is not, we ignore the interrupts and release the buffer
            inu_function__getState(me,&stateParam);
            if (stateParam.state == DISABLE)
            {
               MEM_POOLG_free(bufDescP);
               return INU_HISTOGRAM__RET_SUCCESS;
            }

            CMEM_cacheWb((void*)bufDescP->dataP,bufDescP->dataLen);
            histogramHdr.hwFrameCounter = ((UINT32*)bufDescP->dataP)[0x2424/4];
            tempP = (UINT32*)bufDescP->dataP;
            for(i = 0; i < 3; i++)
            {
               histogramHdr.accumulator[i] = tempP[(0x2400/4)+(i*(0xc/4))];
            }

            retCode = inu_function__newData((inu_function*)me,(inu_data*)node,bufDescP,&histogramHdr,(inu_data**)&hist_data);
            //UINT64 usec;
            //OS_LYRG_getUsecTime(&usec);
            //printf("%llu: memcpy done: %d, index %lld hwcntr=%d, accum = %d,%d,%d, bufDescP = %p dataLen %d\n",usec,
            //       privP->ctorParams.nuCfgChId,histogramHdr.dataHdr.dataIndex,histogramHdr.hwFrameCounter,histogramHdr.accumulator[0],histogramHdr.accumulator[1],histogramHdr.accumulator[2],bufDescP->dataP,bufDescP->dataLen);
            //print_hist(bufDescP->dataP);
            if (ERRG_SUCCEEDED(retCode))
            {
               inu_function__doneData((inu_function*)me, (inu_data*)hist_data);
            }
         }
         HCG_MNGRG_devoteUnit( HCG_MNGRG_CVA );
         HCG_MNGRG_devoteUnit( HCG_MNGRG_PPE );
         HCG_MNGRG_devoteUnit( HCG_MNGRG_IAE );
      }
      break;
      case (UPDATE_IIC_READY):
      {
          UINT32 frameNum, address;

          frameNum = msgP->params[0];
          address = msgP->params[1];

          CVA_MNGRG_updateIICReady(frameNum,address);
      }
      break;
   }

   return INU_HISTOGRAM__RET_SUCCESS;
}


static int inu__reuseMemThread(void *argP)
{
   inu_reuseMsgT msg;
   INT32  status;
   UINT32 msgSize = 0;

   FIX_UNUSED_PARAM_WARN(argP);

   while(1)
   {
      msgSize = sizeof(inu_reuseMsgT);
      status = OS_LYRG_recvMsg(&inu_memreuse__msgQue,  (UINT8*)&msg, &msgSize, INU_HISTOGRAM__THREAD_WAKEUP_MSEC);

      if((status == SUCCESS_E) && (msgSize > 0))
      {
         inu__reuseMemHandleMsg(&msg);
      }
   }

   return INU_HISTOGRAM__RET_SUCCESS;
}

ERRG_codeE inu__reuseHistMemcpyMsg(void *me , UINT64 timestamp, UINT32 hist, MEM_POOLG_bufDescT *bufDesc)
{
   ERRG_codeE ret = ICC__RET_SUCCESS;
   inu_reuseMsgT msg;

   msg.msgType = MEM_CPY_HIST;
   msg.params[0] = (UINT32)me;
   msg.params[1] = hist;
   msg.paramU64 = timestamp;
   msg.paramsBufDedc = bufDesc;

   if(OS_LYRG_sendMsg(&inu_memreuse__msgQue,(UINT8 *)&msg, sizeof(inu_reuseMsgT)) != SUCCESS_E)
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "msgP.msgType = %d\n",msg.msgType);
      ret = ICC__ERR_OUT_OF_RSRCS;
   }

   return ret;
}

ERRG_codeE inu__reuseFreakStartMsg(UINT32 systemFrameCtr, UINT32 currentPhyAddress)
{
   ERRG_codeE           ret = ICC__RET_SUCCESS;
   inu_reuseMsgT msg;

   msg.msgType = UPDATE_IIC_READY;
   msg.params[0] = systemFrameCtr;
   msg.params[1] = currentPhyAddress;

   if(OS_LYRG_sendMsg(&inu_memreuse__msgQue,(UINT8 *)&msg, sizeof(inu_reuseMsgT)) != SUCCESS_E)
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "msgP.msgType = %d\n",msg.msgType);
      ret = ICC__ERR_OUT_OF_RSRCS;
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: inu__reuseMemInit
*
*  Description:   The function starts the workaround for HW issue in NU4kB0
*                      The issue occurs when two AXI masters are accessing the same IAE AXI slave.
*                      This happens for example in the following case - Freak in DDR less using DSR
*                      memory reuse + IAE histograms.
*                      To over come the issue, we manage both access from SW by triggering them
*                      at different times (same thread context will invoke).
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
ERRG_codeE inu__reuseMemInit()
{
   ERRG_codeE           ret = ICC__RET_SUCCESS;
   OS_LYRG_threadParams thrParams;

   if (!inu_memreuse__thrdH)
   {
      memcpy(inu_memreuse__msgQue.name, REUSE_MSGQ_NAME, MIN(sizeof(inu_memreuse__msgQue.name), strlen(REUSE_MSGQ_NAME)+1));
      inu_memreuse__msgQue.maxMsgs = 10;
      inu_memreuse__msgQue.msgSize = sizeof(inu_reuseMsgT);

      //Create message queue
      if(OS_LYRG_createMsgQue(&inu_memreuse__msgQue, 0) != SUCCESS_E)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "inu__reuseMemInit msg queue create failed\n");
         assert(0);
      }


      //thread
      thrParams.func =      inu__reuseMemThread;
      thrParams.id =        OS_LYRG_REUSE_MEMORY_ID_E;
      thrParams.event =     NULL;
      thrParams.param =     NULL;
      inu_memreuse__thrdH = OS_LYRG_createThread(&thrParams);
      if(!inu_memreuse__thrdH)
      {
         assert(0);
      }
   }
   return ret;
}
#endif

