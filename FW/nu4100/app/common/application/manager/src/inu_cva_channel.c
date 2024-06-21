#include "inu2.h"
#include "inu_cva_channel.h"
#include "inu2_types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "err_defs.h"
#include "mem_pool.h"
#include "internal_cmd.h"

#include "inu_graph.h"

#if DEFSG_IS_GP
#include "inu2_internal.h"
#include "sequence_mngr.h"
#include "cde_mngr_new.h"
#include "cva_mngr.h"
#include "inu_alt.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
   inu_cva_channel__CtorParams ctorParams;
   char name[35];   
   inu_sensors_group *sourceSensorGroup;
#if DEFSG_IS_GP
   SEQ_MNGRG_channelH channelH;
#endif
}inu_cva_channel__privData;

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;

static const char* inu_cva_channel__name(inu_ref *me)
{
   inu_cva_channel *cva_ch = (inu_cva_channel*)me;
   inu_cva_channel__privData *privP = (inu_cva_channel__privData*)cva_ch->privP;
   return privP->name;
}

static void inu_cva_channel__dtor(inu_ref *me)
{
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
   free(((inu_cva_channel*)me)->privP);
}


/* Constructor */
ERRG_codeE inu_cva_channel__ctor(inu_cva_channel *me, inu_cva_channel__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_cva_channel__privData *privP;
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_cva_channel__privData*)malloc(sizeof(inu_cva_channel__privData));
      if (privP)
      {
         memset(privP,0,sizeof(inu_cva_channel__privData));
         memcpy(&privP->ctorParams, ctorParamsP, sizeof(inu_cva_channel__CtorParams));
         me->privP = privP;

         char nucfgChId[20];

         strcpy(privP->name,"INU_CVA_CHANNEL");
         if (privP)
         {
            sprintf(nucfgChId, ", nucfgChId: %d", privP->ctorParams.nuCfgChId);
            strcat(privP->name,nucfgChId);
         }
      }
      else
      {
         ret = INU_CVA_CHANNEL__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}

static void inu_cva_channel__newOutput(inu_node *me, inu_node *output)
{
   inu_function__vtable_get()->node_vtable.p_newOutput(me,output);
}

#if DEFSG_IS_GP
void inu_cva__iicFrameDoneCb(void *userParams, void *arg)
{
   ERRG_codeE       ret;
   inu_image__hdr_t imageHdr;
   inu_image        *image;
   inu_node         *node = NULL;
   inu_cva_channel  *me = (inu_cva_channel*)arg;
   CDE_MNGRG_userCbParamsT *userCb = (CDE_MNGRG_userCbParamsT *)userParams;
   CDE_MNGRG_channnelInfoT* channelInfo = userCb->channelInfo;
   CDE_DRVG_channelHandleT    h = channelInfo->dmaChannelHandle;
   UINT8  *imageData;   

    int notFound = 1;
    while (notFound)
    {
        node = inu_node__getNextOutputNode(me,node);
        if(INU_IMAGE_REF_TYPE == inu_ref__getRefType((inu_ref*)node))
        {
            //printf("Found INU_IMAGE_REF_TYPE node\n");
            notFound = 0;
        }
    }


   //CHECK #1
   if (!node)
   {
      printf("no output inu_data for inu_soc, release data\n");
      MEM_POOLG_free(userCb->buffDescriptorP);   
      return;
   }


   memcpy(&imageHdr,inu_data__getHdr(node),sizeof(imageHdr));
   imageHdr.dataHdr.dataIndex = userCb->systemFrameCntr;
   imageHdr.dataHdr.timestamp = userCb->timeStamp;


   if(h->scenarioType ==  CDE_DRVG_SCENARIO_TYPE_IIC_STREAM_DDR_LESS)
   {
       ret = MEM_POOLG_alloc(channelInfo->memPoolHandle, MEM_POOLG_getBufSize(channelInfo->memPoolHandle), &userCb->buffDescriptorP);
       if(ERRG_SUCCEEDED(ret))
       {
          ret = MEM_MAPG_getVirtAddr(/*MEM_MAPG_REG_CRAM_E*/MEM_MAPG_IAE_LUT_MRS_E , ((MEM_MAPG_addrT)&imageData));
          if(ERRG_SUCCEEDED(ret))
          {
              (userCb->buffDescriptorP)->dataLen     = /*0x000A0000*/0x0013B000;
              (userCb->buffDescriptorP)->dataP      = imageData;
              imageHdr.imgDescriptor.bufferHeight   = /*0x000A0000*/0x0013B000; //inform host on actual size

              printf(GREEN("inu_cva__iicFrameDoneCb ddrless - Dataptr = %p, size 0x%08x, CTRL FrNum %llu, timestamp %llu\n"),
                           imageData, /*0x000A0000*/0x0013B000, imageHdr.dataHdr.dataIndex, imageHdr.dataHdr.timestamp);
              ret = inu_function__newData((inu_function*)me, (inu_data*)node, userCb->buffDescriptorP, &imageHdr, (inu_data**)&image);
              if (ERRG_SUCCEEDED(ret))
              {
                 ret = inu_function__doneData((inu_function*)me,(inu_data*)image);
              }

          }
       }
       else
        {
            printf(GREEN("inu_cva__iicFrameDoneCb allocation failed\n"));
            
            return;
        }
    }
    else
    {
        imageHdr.imgDescriptor.bufferHeight = (userCb->buffDescriptorP)->dataLen;
        printf(GREEN("inu_cva__iicFrameDoneCb ddr - Dataptr = %p, size 0x%08x, CTRL FrNum %llu, timestamp %llu\n"),
                      (userCb->buffDescriptorP)->dataP,  (userCb->buffDescriptorP)->dataLen, imageHdr.dataHdr.dataIndex, imageHdr.dataHdr.timestamp);
        ret = inu_function__newData((inu_function*)me, (inu_data*)node, userCb->buffDescriptorP, &imageHdr, (inu_data**)&image);
        if (ERRG_SUCCEEDED(ret))
        {
           ret = inu_function__doneData((inu_function*)me,(inu_data*)image);
        }
    }
}

static void inu_cva__frameDoneCb(CDE_MNGRG_userCbParamsT *userParams, void *arg)
{
   ERRG_codeE       ret;
   inu_cva_data__hdr_t cvaHdr;
   inu_image        *image;
   inu_node         *node = NULL;
   inu_cva_channel  *me = (inu_cva_channel*)arg;
   inu_cva_channel__privData *privP = (inu_cva_channel__privData*)me->privP;
   UINT64           strobeTs,strobeCtr;
   CVA_MNGRG_freakFrameParamsT frameParam;
   UINT32           projMode;//dummy
   SEQ_MNGRG_handleT         handle = inu_graph__getSeqDB(inu_node__getGraph(me));

   LOGG_PRINT(LOG_DEBUG_E,NULL,"Freak frame done cb \n");
   //if input is writer, disable the axi reader config. (will be reconfigured at writer operate)
   if (inu_ref__getRefType(inu_node__getNextInputNode(me,NULL)) == INU_SOC_WRITER_REF_TYPE)
   {
      void *grantEntry;
      ret = SEQ_MNGRG_writerSchedulerReleaseGrant(handle, privP->ctorParams.nuCfgChId, SEQ_MNGRG_DONE_BLOCK_CHANNEL_E,&grantEntry);
      if (grantEntry)
      {
         SEQ_MNGRG_pauseChannelPaths(handle, privP->channelH);
         ret = SEQ_MNGRG_writerSchedulerReleaseGrantComplete(handle, grantEntry);
      }
   }

   if(!userParams->buffDescriptorP)
      return;

   int notFound = 1;
   while (notFound)
   {
      node = inu_node__getNextOutputNode(me,node);
      if(INU_CVA_DATA_REF_TYPE == inu_ref__getRefType((inu_ref*)node))
      {
         //printf("Found INU_CVA_DATA_REF_TYPE node\n");
         notFound = 0;
      }
   }

   //CHECK #1
   if (!node)
   {
      printf("no output inu_data for inu_soc, release data\n");
      MEM_POOLG_free(userParams->buffDescriptorP);   
      return;
   }

   
   CVA_MNGRG_getFreakFrameParams(&frameParam);
   if( (frameParam.descrNum == 0xFFFF) || (frameParam.descrNum > 1600) )
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Freak Control is wrong, frame dropped\n");
      MEM_POOLG_free(userParams->buffDescriptorP);   
      return;
   }

   memcpy(&cvaHdr,inu_data__getHdr(node),sizeof(cvaHdr));
   if(privP->sourceSensorGroup != NULL)   // take the strobe time and count   
   {
      inu_data__setMode((inu_data__hdr_t*)&cvaHdr,ALTG_getThisFrameMode(inu_sensors_group__getAltHandle(privP->sourceSensorGroup)));
      ret = inu_sensors_group__getStrobeData(privP->sourceSensorGroup,&strobeTs,&strobeCtr,&projMode);
      if (ERRG_SUCCEEDED(ret))
      {
         cvaHdr.dataHdr.dataIndex = strobeCtr;
         cvaHdr.dataHdr.timestamp = strobeTs;
         //printf("new. ts %llu, ctr %llu. ",strobeTs,strobeCtr);     
      }
      else
      {
         cvaHdr.dataHdr.dataIndex = frameParam.frameCnt;
         cvaHdr.dataHdr.timestamp = userParams->timeStamp;
      }

      ret = inu_sensor__getReadoutTs(inu_node__getNextOutputNode(privP->sourceSensorGroup, NULL),&cvaHdr.readoutTs);
   }
   else
   {
      //injection case, take frame num from freak control
      cvaHdr.dataHdr.dataIndex = (UINT64)frameParam.frameCnt;
      OS_LYRG_getTimeNsec(&cvaHdr.dataHdr.timestamp);
   }
   cvaHdr.cvaDescriptor.kpNum = frameParam.descrNum;
   //printf(CYAN("inu_cva__frameDoneCb - CVA frame Done: CTRL FrNum %d, KpNum %d\n"), frameParam.frameCnt, frameParam.descrNum);

   ret = inu_function__newData((inu_function*)me, (inu_data*)node, userParams->buffDescriptorP, &cvaHdr, (inu_data**)&image);
   if (ERRG_SUCCEEDED(ret))
   {
      ret = inu_function__doneData((inu_function*)me,(inu_data*)image);
   }
}

#endif

static ERRG_codeE inu_cva_channel__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   inu_sensors_group *sensorGroupP;
   inu_cva_channel *cvaChannelP=(inu_cva_channel*)me;
   inu_cva_channel__privData *privP = (inu_cva_channel__privData*)cvaChannelP->privP;
   SEQ_MNGRG_handleT handle;

   ret = inu_function__vtable_get()->p_start(me, startParamP);
   if (ERRG_SUCCEEDED(ret))
   {
      ret = inu_graph__getOrigSensGroup(me, (inu_nodeH*)&sensorGroupP);
      if (ERRG_SUCCEEDED(ret))
      {
         privP->sourceSensorGroup = sensorGroupP;
      }
      else
      {
         privP->sourceSensorGroup = NULL;
      }

      handle = inu_graph__getSeqDB(inu_node__getGraph(me));
      if (handle)
      {
         ret = SEQ_MNGRG_configChannel(handle, SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E, ((inu_cva_channel__privData*)cvaChannelP->privP)->ctorParams.nuCfgChId,TRUE,me, &privP->channelH);
         if (ERRG_SUCCEEDED(ret))
         {
            ret = SEQ_MNGRG_startChannel(handle, SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E, ((inu_cva_channel__privData*)cvaChannelP->privP)->ctorParams.nuCfgChId,(void *)inu_cva__frameDoneCb,(void *)me, privP->channelH);
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
#else
   ret = inu_function__vtable_get()->p_start(me, startParamP);
#endif
   return ret;
}


static ERRG_codeE inu_cva_channel__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   inu_cva_channel *cvaChannelP=(inu_cva_channel *)me;
   inu_cva_channel__privData *privP = (inu_cva_channel__privData*)cvaChannelP->privP;
   SEQ_MNGRG_handleT handle;

   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
   if (ERRG_SUCCEEDED(ret))
   {
      handle = inu_graph__getSeqDB(inu_node__getGraph(me));
      if (handle)
      {   
         ret = SEQ_MNGRG_stopChannel(handle, SEQ_MNGRG_XML_BLOCK_CVA_RDOUT_E, ((inu_cva_channel__privData*)cvaChannelP->privP)->ctorParams.nuCfgChId,me, privP->channelH);
      }
      else
      {
         INU_REF__LOGG_PRINT(me, LOG_ERROR_E, NULL, "Failed to get seq DB!\n");
      }
   }
#else
   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
#endif
   return ret;
}


void inu_cva_channel__vtable_init( void )
{
   if (!_bool_vtable_initialized) 
   {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_cva_channel__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_cva_channel__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_cva_channel__ctor;

      _vtable.node_vtable.p_newOutput = inu_cva_channel__newOutput;

      _vtable.p_start = inu_cva_channel__start;
      _vtable.p_stop  = inu_cva_channel__stop;

      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_cva_channel__vtable_get(void)
{
   inu_cva_channel__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}

UINT32 inu_cva_channel__getId(inu_cva_channelH meH)
{
   return ((inu_cva_channel__privData*)((inu_cva_channel*)meH)->privP)->ctorParams.nuCfgChId;
}

#ifdef __cplusplus
}
#endif
