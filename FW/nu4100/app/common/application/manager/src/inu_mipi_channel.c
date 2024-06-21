#include "inu2.h"
#include "inu_mipi_channel.h"
#include "inu2_types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "err_defs.h"
#include "mem_pool.h"
#include "internal_cmd.h"

#include "inu_graph.h"

#if DEFSG_IS_GP
#include "log.h"
#include "sequence_mngr.h"
#include "ppe_mngr.h"
#include "inu_alt.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
   inu_mipi_channel__CtorParams ctorParams;
   char name[35];
   inu_sensors_group *sourceSensorGroup;
#if DEFSG_IS_GP
   SEQ_MNGRG_channelH channelH;
   UINT8 cropUpdateRequest;
   UINT16 cropXStart;
   UINT16 cropYStart;
#endif
}inu_mipi_channel__privData;

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
//static int inu_mipi_channel__rxIoctl(inu_ref *ref, void *msgP, int msgCode);

static const char* inu_mipi_channel__name(inu_ref *me)
{
   inu_mipi_channel *mipi_ch = (inu_mipi_channel*)me;
   inu_mipi_channel__privData *privP = (inu_mipi_channel__privData*)mipi_ch->privP;
   return privP->name;
}

static void inu_mipi_channel__dtor(inu_ref *me)
{
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
   free(((inu_mipi_channel*)me)->privP);
}


/* Constructor */
ERRG_codeE inu_mipi_channel__ctor(inu_mipi_channel *me, inu_mipi_channel__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_mipi_channel__privData *privP;
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_mipi_channel__privData*)malloc(sizeof(inu_mipi_channel__privData));
      if (privP)
      {
         memset(privP,0,sizeof(inu_mipi_channel__privData));
         memcpy(&privP->ctorParams, ctorParamsP, sizeof(inu_cva_channel__CtorParams));
         me->privP = privP;

         char nucfgChId[20];

         strcpy(privP->name,"INU_MIPI_CHANNEL");
         if (privP)
         {
            sprintf(nucfgChId, ", nucfgChId: %d", privP->ctorParams.nuCfgChId);
            strcat(privP->name,nucfgChId);
         }
      }
      else
      {
         ret = INU_MIPI_CHANNEL__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}

#if DEFSG_IS_GP
static void inu_mipi__frameDoneCb(UINT64 timestamp, UINT32 num, void *arg)
{
   ERRG_codeE       ret;
   inu_node         *node;
   inu_mipi_channel  *me = (inu_mipi_channel*)arg;
   inu_mipi_channel__privData *privP = (inu_mipi_channel__privData*)me->privP;   
   SEQ_MNGRG_handleT handle = inu_graph__getSeqDB(inu_node__getGraph(me));

   //if input is writer, disable the mipi. (will be reconfigured at writer operate)
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
   //INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL, "ref %d, %s\n", me->function.node.ref.id, SEQ_MNGRG_getName(handle));
}
#endif


static ERRG_codeE inu_mipi_channel__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   inu_sensors_group *sensorGroupP;
   inu_mipi_channel *mipiChannelP =(inu_mipi_channel *)me;
   inu_mipi_channel__privData *privP = (inu_mipi_channel__privData*)mipiChannelP->privP;
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

      handle = inu_graph__getSeqDB(inu_node__getGraph(me));
      if (handle)
      {
         ret = SEQ_MNGRG_configChannel(handle, SEQ_MNGRG_XML_BLOCK_DPHY_TX_E, privP->ctorParams.nuCfgChId,TRUE,me, &privP->channelH);
         if (ERRG_SUCCEEDED(ret))
         {
            ret = SEQ_MNGRG_startChannel(handle, SEQ_MNGRG_XML_BLOCK_DPHY_TX_E, privP->ctorParams.nuCfgChId,inu_mipi__frameDoneCb,(void *)me, privP->channelH);
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


static ERRG_codeE inu_mipi_channel__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   inu_mipi_channel *mipiChannelP=(inu_mipi_channel *)me;
   inu_mipi_channel__privData *privP = (inu_mipi_channel__privData*)mipiChannelP->privP;
   SEQ_MNGRG_handleT handle;

   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
   if (ERRG_SUCCEEDED(ret))
   {
      handle = inu_graph__getSeqDB(inu_node__getGraph(me));
      if (handle)
      {
         ret = SEQ_MNGRG_stopChannel(handle, SEQ_MNGRG_XML_BLOCK_DPHY_TX_E, privP->ctorParams.nuCfgChId,me, privP->channelH);
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


void inu_mipi_channel__vtable_init( void )
{
   if (!_bool_vtable_initialized) 
   {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_mipi_channel__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_mipi_channel__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_mipi_channel__ctor;
//      _vtable.node_vtable.ref_vtable.p_rxSyncCtrl = inu_mipi_channel__rxIoctl;

      _vtable.p_start = inu_mipi_channel__start;
      _vtable.p_stop  = inu_mipi_channel__stop;

      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_mipi_channel__vtable_get(void)
{
   inu_mipi_channel__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}

UINT32 inu_mipi_channel__getId(inu_mipi_channelH meH)
{
   return ((inu_mipi_channel__privData*)((inu_mipi_channel*)meH)->privP)->ctorParams.nuCfgChId;
}

#if 0
ERRG_codeE inu_mipi_channel__updateCropWindow(inu_mipi_channelH meH, inu_mipi_channel__cropParamsT *crop)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_CHANGE_CROP_WINDOW_E,crop, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

static int inu_mipi_channel__rxIoctl(inu_ref *ref, void *msgP, int msgCode)
{
   inu_mipi_channel *me = (inu_mipi_channel*)ref;
   inu_mipi_channel__privData *privP = (inu_mipi_channel__privData*)me->privP;
   int ret = INU_MIPI_CHANNEL__RET_SUCCESS;

   ret = inu_function__vtable_get()->node_vtable.ref_vtable.p_rxSyncCtrl(ref, msgP, msgCode);
#if DEFSG_IS_GP
   //printf("inu_mipi_channel__rxIoctl: msg Code %x\n", msgCode);

   switch (msgCode)
   {
      case(INTERNAL_CMDG_CHANGE_CROP_WINDOW_E):
      {
         inu_mipi_channel__cropParamsT *crop = (inu_mipi_channel__cropParamsT *)msgP;
         privP->cropUpdateRequest = 1;
         privP->cropXStart = crop->xStart;
         privP->cropYStart = crop->yStart;
         INU_REF__LOGG_PRINT(ref, LOG_DEBUG_E, NULL, "Crop request: x - %d, y - %d\n", privP->cropXStart, privP->cropYStart);
      }
      break;

      default:
//      ret = INU_MIPI_CHANNEL__ERR_NOT_SUPPORTED;
      break;
   }
#else
   FIX_UNUSED_PARAM_WARN(privP);
#endif
   return ret;
}
#endif

#ifdef __cplusplus
}
#endif
