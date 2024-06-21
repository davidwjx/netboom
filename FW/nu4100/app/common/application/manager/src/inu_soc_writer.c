#include "inu2.h"
#include "inu_soc_writer.h"
#include "inu_soc_writer_api.h"

#include "inu2_types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "err_defs.h"
#include "mem_pool.h"
#include "internal_cmd.h"
#include "log.h"
#include "inu_graph.h"

#if DEFSG_IS_GP
#include "hcg_mngr.h"
#include "sequence_mngr.h"
#include "cde_mngr_new.h"
#include "assert.h"
#include "ppe_drv.h"
#include "helsinki.h"
#include "cde_drv_new.h"
#include "metadata_updater.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
//#define MAX_THROUGHPUT_INJECTION

typedef struct
{
   inu_soc_writer__CtorParams ctorParams;
#if DEFSG_IS_GP
   SEQ_MNGRG_handleT handle;
   SEQ_MNGRG_channelH chH;
#endif
   void *writerChannelP;
   inu_data *currDdata;
   char name[35];
}inu_soc_writer__privData;
#ifdef MAX_THROUGHPUT_INJECTION   
bool first_injection_done = 0;

#define BUFFER_ARRAY_TOTAL_SIZE 99

inu_dataH bufferArray[BUFFER_ARRAY_TOTAL_SIZE];
UINT32 bufferArraySize;
UINT32 currentIndex;
UINT32 totalInjected = 0;

void Injection_Buffer_Push(const inu_dataH img);
void Injection_Buffer_Pop(inu_dataH * img);
void Injection_Buffer_Init(UINT8 capacity, UINT8 size);
#endif

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;

static const char* inu_soc_writer__name(inu_ref *me)
{
   inu_soc_writer *soc_ch = (inu_soc_writer*)me;
   inu_soc_writer__privData *privP = (inu_soc_writer__privData*)soc_ch->privP;

   return privP->name;
}

static void inu_soc_writer__dtor(inu_ref *me)
{
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
   free(((inu_soc_writer*)me)->privP);
}

/* Constructor */
ERRG_codeE inu_soc_writer__ctor(inu_soc_writer *me, inu_soc_writer__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_soc_writer__privData *privP;
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_soc_writer__privData*)malloc(sizeof(inu_soc_writer__privData));
      if (privP)
      {
         memset(privP,0,sizeof(inu_soc_writer__privData));
         memcpy(&privP->ctorParams, ctorParamsP, sizeof(inu_soc_writer__CtorParams));
         me->privP = privP;

         char nucfgChId[20];

         strcpy(privP->name,"INU_SOC_WRITER");
         if (privP)
         {
            sprintf(nucfgChId, ", nucfgChId: %d", privP->ctorParams.nuCfgChId);
            strcat(privP->name,nucfgChId);
         }
#if DEFSG_IS_GP
         HCG_MNGRG_voteUnits(HCG_MNGRG_HW_ALL_UNITS);
#endif
      }
      else
      {
         ret = INU_SOC_WRITER__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}

static void inu_soc_writer__newOutput(inu_node *me, inu_node *output)
{
   inu_function__vtable_get()->node_vtable.p_newOutput(me,output);
}

static void inu_soc_writer__newInput(inu_node *me, inu_node *input)
{
   inu_function__vtable_get()->node_vtable.p_newInput(me, input);
#if 1
#if DEFSG_IS_GP
   UINT32 numInter;
   SEQ_MNGRG_handleT seqH;
   XMLDB_dbH xml_db;
   inu_soc_writer__privData *privP = (inu_soc_writer__privData*)((inu_soc_writer*)me)->privP;
   
   if (inu_ref__getRefType(input) == INU_IMAGE_REF_TYPE)
   {
      inu_image__hdr_t *hdrP = (inu_image__hdr_t*)inu_data__getHdr(input);
      //hdrP->imgDescriptor.numInterleaveImages
      seqH = inu_graph__getSeqDB(inu_node__getGraph(me));
      xml_db = SEQ_MNGRG_getXmlDb( seqH );
      //..update DB 
      XMLDB_setValue(xml_db,NUFLD_calcPath(NUFLD_META_WRITERS_E, privP->ctorParams.nuCfgChId, META_WRITERS_WT_0_INPUT_RES_NUM_INTERLEAVES_E),hdrP->imgDescriptor.numInterleaveImages);
      XMLDB_setValue(xml_db,NUFLD_calcPath(NUFLD_META_WRITERS_E, privP->ctorParams.nuCfgChId, META_WRITERS_WT_0_INPUT_RES_STRIDE_E),hdrP->imgDescriptor.stride);
      //printf("inu_soc_writer__newInput set num interleaved to %d field db %d stride %d width %d\n",hdrP->imgDescriptor.numInterleaveImages,NUFLD_calcPath(NUFLD_META_WRITERS_E, privP->ctorParams.nuCfgChId, META_WRITERS_WT_0_INPUT_RES_NUM_INTERLEAVES_E),hdrP->imgDescriptor.stride,hdrP->imgDescriptor.width);
   }
#endif
#else
inu_soc_writer__privData *privP = (inu_soc_writer__privData*)((inu_soc_writer*)me)->privP;
//XMLDB_setValue(xml_db, NUFLD_calcPath(NUFLD_META_WRITERS_E, privP->ctorParams.nuCfgChId, META_WRITERS_WT_0_INPUT_RES_NUM_INTERLEAVES_E), privP->ctorParams.imageDescriptor.numInterleaveImages);
#endif
}

#if DEFSG_IS_GP
static void inu_soc_writer__frameDoneCb(CDE_MNGRG_userCbParamsT *userParams, void *arg)
{
   FIX_UNUSED_PARAM_WARN(userParams);
   inu_data *data;
   inu_soc_writer *socWriterP=(inu_soc_writer *)arg;
   inu_soc_writer__privData *privP = (inu_soc_writer__privData*)socWriterP->privP;
   void *grantEntryP;
   ERRG_codeE ret = 0;
#ifdef MAX_THROUGHPUT_INJECTION
   inu_dataH FramePtr = NULL;
   UINT32 srcPhyAddress;  
   MEM_POOLG_bufDescT *bufP;
#endif
   data = privP->currDdata;
   privP->currDdata = NULL;
#ifdef MAX_THROUGHPUT_INJECTION
   Injection_Buffer_Pop(&FramePtr);
   inu_data__bufDescPtrGet(FramePtr,&bufP);

   if(privP->writerChannelP && bufP->dataP)
   {
      inu_data__hdr_t *hdrP = inu_data__getHdr(FramePtr);
      MEM_POOLG_getDataPhyAddr(bufP, &srcPhyAddress); 
      privP->currDdata = FramePtr;
      UINT64 nsec;
      OS_LYRG_getTimeNsec(&nsec);
      HCG_MNGRG_processEvent(HCG_MNGRG_HW_EVENT_WRITER_0_E + privP->ctorParams.nuCfgChId, nsec, 1);
      PPE_DRVG_updateWriterFrameId(privP->ctorParams.nuCfgChId, (UINT32)hdrP->dataIndex); //--TODO has two issues: the value does not update the CVA index, and HCG does not vote for ppe?
      CDE_MNGRG_injectBuffer(privP->writerChannelP,(UINT8*)srcPhyAddress);
      totalInjected++;
    }
    else
    {
       LOGG_PRINT(LOG_ERROR_E, ret, "SoC writer operate failure: ChannelID=%d\n",privP->ctorParams.nuCfgChId);
    }
    
#else
   //INU_REF__LOGG_PRINT(socWriterP,LOG_INFO_E, NULL, "0: inject done\n");
   void *grantEntry;
      ret = SEQ_MNGRG_writerSchedulerReleaseGrant(privP->handle, privP->ctorParams.nuCfgChId, SEQ_MNGRG_DONE_BLOCK_WRITER_E,&grantEntry);
      if (grantEntry)
      {
         SEQ_MNGRG_pauseChannelPaths(privP->handle, privP->chH);
         ret = SEQ_MNGRG_writerSchedulerReleaseGrantComplete(privP->handle, grantEntry);
      }
#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
                  char tmp2[256]="";
                  UINT64 time;
                  OS_LYRG_getTimeNsec(&time);
                  sprintf(tmp2," writer__frameDoneCb %d %llu\n",privP->ctorParams.nuCfgChId,time);
                  SEQ_MNGRG_addSchedulerDebugPrint(tmp2);
#endif

   inu_function__doneData((inu_function*)socWriterP, data);
   inu_function__complete((inu_function*)arg);
#endif

   
}
#endif

static ERRG_codeE inu_soc_writer__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
   ret = inu_function__vtable_get()->p_start(me, startParamP);
#ifdef MAX_THROUGHPUT_INJECTION   
   bufferArraySize = 0;
   currentIndex = 0;
#endif
#if DEFSG_IS_GP
   inu_soc_writer__privData *privP = (inu_soc_writer__privData*)((inu_soc_writer*)me)->privP;
   privP->handle = inu_graph__getSeqDB(inu_node__getGraph(me));
   if (!privP->handle)
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "Failed to find seq db for %s\n",privP->ctorParams.configName);
   }
#endif
   return ret;
}


static ERRG_codeE inu_soc_writer__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   inu_soc_writer__privData *privP = (inu_soc_writer__privData*)((inu_soc_writer*)me)->privP;
   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
   if(ERRG_SUCCEEDED(ret))
   {
      privP->writerChannelP=NULL;
   }
#else    
   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
#endif
   return ret;
}

#if DEFSG_IS_GP
ERRG_codeE inu_soc_writer__fillOutputChannel(inu_soc_writer *me, void *seqChH)
{
   inu_soc_writer__privData *privP = (inu_soc_writer__privData*)me->privP;
   privP->chH = (SEQ_MNGRG_channelH*)seqChH;
   return INU_SOC_WRITER__RET_SUCCESS;
}

static void inu_soc_writer__operate(inu_function *me, inu_function__operateParamsT *paramsP)
{
   ERRG_codeE ret = INU_SOC_WRITER__ERR_UNEXPECTED;
   inu_soc_writer__privData *privP = (inu_soc_writer__privData*)((inu_soc_writer*)me)->privP;
   inu_function__vtable_get()->p_operate(me, paramsP);
   MEM_POOLG_bufDescT *bufP;
   UINT32 srcPhyAddress=0;
   inu_data *data;
   void *writerChannelP = NULL;
   #ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
            char tmp[256]="";
            UINT64 time;
            OS_LYRG_getTimeNsec(&time);
            sprintf(tmp,"%s: operate %llu\n",SEQ_MNGRG_getName(privP->handle),time);
            SEQ_MNGRG_addSchedulerDebugPrint(tmp);
#endif

   data = paramsP->dataInputs[0];
   //INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL, "refid = %d, %s\n",me->node.ref.id, SEQ_MNGRG_getName(privP->handle));

   ret = SEQ_MNGRG_getWriterChannel(privP->handle, &writerChannelP, privP->ctorParams.nuCfgChId);
   if(ERRG_SUCCEEDED(ret) && writerChannelP != privP->writerChannelP)
   {
      CDE_MNGRG_chanCbInfoT cbInfo;

      memset(&cbInfo,0,sizeof(cbInfo));
      cbInfo.frameDoneCb.cb = inu_soc_writer__frameDoneCb;
      cbInfo.frameDoneCb.arg = me;
      privP->currDdata = NULL;
      HCG_MNGRG_voteUnits((1 << HCG_MNGRG_DMA0) | (1 << HCG_MNGRG_DMA1) | (1 << HCG_MNGRG_DMA2));
      ret = CDE_MNGRG_registerUserCallBacks(writerChannelP,&cbInfo);
      HCG_MNGRG_devoteUnits((1 << HCG_MNGRG_DMA0) | (1 << HCG_MNGRG_DMA1) | (1 << HCG_MNGRG_DMA2));
      if (ERRG_FAILED(ret))
      {
         privP->writerChannelP = NULL;
      }
   }
   privP->writerChannelP = writerChannelP;
#ifdef MAX_THROUGHPUT_INJECTION
   // Fill Injection buffer with frames until it is full
   if (bufferArraySize < BUFFER_ARRAY_TOTAL_SIZE)
   {
      Injection_Buffer_Push(paramsP->dataInputs[0]);
      inu_function__complete(me);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "buffer is full\n");
      if (!first_injection_done)
      {
         inu_dataH FramePtr = 0;
         Injection_Buffer_Pop(&FramePtr);

         inu_data__bufDescPtrGet(FramePtr,&bufP);
         if(privP->writerChannelP && bufP->dataP)
         {
            inu_data__hdr_t *hdrP = inu_data__getHdr(FramePtr);
            MEM_POOLG_getDataPhyAddr(bufP, &srcPhyAddress); 
            privP->currDdata = FramePtr;
            UINT64 nsec;
            OS_LYRG_getTimeNsec(&nsec);
            HCG_MNGRG_processEvent(HCG_MNGRG_HW_EVENT_WRITER_0_E + privP->ctorParams.nuCfgChId, nsec, 1);
            first_injection_done = true;

            PPE_DRVG_updateWriterFrameId(privP->ctorParams.nuCfgChId, (UINT32)hdrP->dataIndex); //--TODO has two issues: the value does not update the CVA index, and HCG does not vote for ppe?
            CDE_MNGRG_injectBuffer(privP->writerChannelP,(UINT8*)srcPhyAddress);
            totalInjected++;
         }
         else
         {
            inu_function__complete(me);
            LOGG_PRINT(LOG_DEBUG_E, ret, "SoC writer operate failure: ChannelID=%d\n", privP->ctorParams.nuCfgChId);
         }
      }
   }
#else
   inu_data__bufDescPtrGet(data,&bufP);
   if(privP->writerChannelP && bufP->dataP)
   {
      inu_data__hdr_t *hdrP = inu_data__getHdr(data);
      //MEM_MAPG_convertVirtualToPhysical( (UINT32)bufP->dataP, &srcPhyAddress );
      MEM_POOLG_getDataPhyAddr(bufP,&srcPhyAddress);
      //INU_REF__LOGG_PRINT(me,LOG_INFO_E, NULL, "0: %s Writer %d operate inject buff=(virt=%p,phy=0x%x), index=%llu\n",SEQ_MNGRG_getName(privP->handle),
      //                                             privP->ctorParams.nuCfgChId,bufP->dataP, srcPhyAddress, hdrP->dataIndex);
      privP->currDdata = data;
      //update writer frame index

      PPE_DRVG_updateWriterFrameId(privP->ctorParams.nuCfgChId, (UINT32)hdrP->dataIndex); //--TODO has two issues: the value does not update the CVA index, and HCG does not vote for ppe?

      ret = SEQ_MNGRG_reconfigWriterPaths(inu_graph__getSeqDB(inu_node__getGraph(me)), privP->ctorParams.nuCfgChId, privP->writerChannelP,bufP,data);
   }
   else
   {
      ret = INU_SOC_WRITER__ERR_UNEXPECTED;
   }

   if (ERRG_FAILED(ret))
   {
      inu_function__doneData(me, data);
      inu_function__complete(me);
      LOGG_PRINT(LOG_ERROR_E, ret, "SoC writer operate failure: ChannelID=%d\n",privP->ctorParams.nuCfgChId);  
   }
#endif
}
#endif


void inu_soc_writer__vtable_init( void )
{
   if (!_bool_vtable_initialized) 
   {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_soc_writer__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_soc_writer__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_soc_writer__ctor;

      _vtable.node_vtable.p_newOutput = inu_soc_writer__newOutput;
      _vtable.node_vtable.p_newInput = inu_soc_writer__newInput;

#if DEFSG_IS_GP
      _vtable.p_operate = inu_soc_writer__operate;
#endif
      _vtable.p_start   = inu_soc_writer__start;
      _vtable.p_stop    = inu_soc_writer__stop;
      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_soc_writer__vtable_get(void)
{
   inu_soc_writer__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}

#ifdef MAX_THROUGHPUT_INJECTION  

void Injection_Buffer_Push(const inu_dataH img)
{

   bufferArray[bufferArraySize] = img;     
   bufferArraySize++;
}

void Injection_Buffer_Pop(inu_dataH *img)
{
   *img = bufferArray[currentIndex];
   currentIndex++;
   if (currentIndex == bufferArraySize)
      currentIndex = 0;
}
#endif

#ifdef __cplusplus
}
#endif
