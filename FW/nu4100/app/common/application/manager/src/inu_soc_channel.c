#include "inu2.h"
#include "inu_soc_channel.h"
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
#include "sensors_mngr.h"
#include "cde_mngr_new.h"
#include "ppe_mngr.h"
#include "inu_alt.h"
#include "helsinki.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
   inu_soc_channel__CtorParams ctorParams;
   char name[35];
   inu_sensors_group *sourceSensorGroup;
#if DEFSG_IS_GP
   SEQ_MNGRG_channelH channelH;
   UINT8 cropUpdateRequest;
   UINT16 cropXStart;
   UINT16 cropYStart;
   UINT32 userParam1;
   UINT32 userParam2;
#endif
}inu_soc_channel__privData;

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static int inu_soc_channel__rxIoctl(inu_ref *ref, void *msgP, int msgCode);

static const char* inu_soc_channel__name(inu_ref *me)
{
   inu_soc_channel *soc_ch = (inu_soc_channel*)me;
   inu_soc_channel__privData *privP = (inu_soc_channel__privData*)soc_ch->privP;
   return privP->name;
}

static void inu_soc_channel__dtor(inu_ref *me)
{
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
   free(((inu_soc_channel*)me)->privP);
}


/* Constructor */
ERRG_codeE inu_soc_channel__ctor(inu_soc_channel *me, inu_soc_channel__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_soc_channel__privData *privP;
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_soc_channel__privData*)malloc(sizeof(inu_soc_channel__privData));
      if (privP)
      {
         memset(privP,0,sizeof(inu_soc_channel__privData));
         memcpy(&privP->ctorParams, ctorParamsP, sizeof(inu_cva_channel__CtorParams));
         me->privP = privP;

         char nucfgChId[20];

         strcpy(privP->name,"INU_SOC_CHANNEL");
         if (privP)
         {
            sprintf(nucfgChId, ", nucfgChId: %d", privP->ctorParams.nuCfgChId);
            strcat(privP->name,nucfgChId);
         }
      }
      else
      {
         ret = INU_SOC_CHANNEL__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}


#if DEFSG_IS_GP
/* This function is defined here instead of using header files because adding the header file (metadata_sensor_updater.h) and its dependencies to all of the CMake files is a huge effort!*/
ERRG_codeE METADATA_UPDATER_updateCroppingWindow(unsigned int axiReader);
static void inu_soc__frameDoneCb(CDE_MNGRG_userCbParamsT *userParams, void *arg)
{
   ERRG_codeE       ret;
   inu_image__hdr_t imageHdr;
   inu_image        *image;
   inu_node         *node;
   inu_soc_channel  *me = (inu_soc_channel*)arg;
   inu_soc_channel__privData *privP = (inu_soc_channel__privData*)me->privP;
   SEQ_MNGRG_handleT handle = inu_graph__getSeqDB(inu_node__getGraph(me));
   UINT32           i;
   UINT32           projMode;
   UINT64           strobeTs,strobeCtr;
   inu_sensor       *sensor = NULL;
   inu_function__stateParam stateParam;

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
   //INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL, "ref %d, %s\n", me->function.node.ref.id, SEQ_MNGRG_getName(handle));

   if(!userParams->buffDescriptorP)
      return;

   //if function is stopped, but the HW is not, we ignore the interrupts
   inu_function__getState(me,&stateParam);
   if (stateParam.state == DISABLE)
   {
      MEM_POOLG_free(userParams->buffDescriptorP);
      return;
   }

   node = inu_node__getNextOutputNode(me,NULL);

   //CHECK #1
   if (!node)
   {
      printf("no output inu_data for inu_soc, release data\n");
      MEM_POOLG_free(userParams->buffDescriptorP);
      return;   
   }

   //fill with image descriptor params
   memcpy(&imageHdr,inu_data__getHdr(node),sizeof(imageHdr));

   //save chunk num
   imageHdr.dataHdr.chunkNum = userParams->chunkNum;

   //insert user params
   imageHdr.userInfo.param1 = privP->userParam1;
   imageHdr.userInfo.param2 = privP->userParam2;

   //if the input are real sensor(s) than:
   if (privP->sourceSensorGroup)
   {
      imageHdr.sensorGroup = inu_sensors_group__getId(privP->sourceSensorGroup);
      inu_data__setMode((inu_data__hdr_t*)&imageHdr,ALTG_getThisFrameMode(inu_sensors_group__getAltHandle(privP->sourceSensorGroup)));
      //check if we have data from strobe
      ret = inu_sensors_group__getStrobeData(privP->sourceSensorGroup,&strobeTs,&strobeCtr,&projMode);
      if (ERRG_SUCCEEDED(ret))
      {
         imageHdr.dataHdr.dataIndex = strobeCtr;
         imageHdr.dataHdr.timestamp = strobeTs;
         imageHdr.projMode = projMode;
         //printf("new. ts %llu, ctr %llu. ",strobeTs,strobeCtr);      
      }
      else
      {
         imageHdr.dataHdr.dataIndex = userParams->systemFrameCntr;
         imageHdr.dataHdr.timestamp = userParams->timeStamp;
      }
      #ifdef ENABLE_ROI_QUEUE
      METADATA_UPDATER_updateCroppingWindow(privP->ctorParams.nuCfgChId);
      #endif
      ret = inu_sensor__getReadoutTs(inu_node__getNextOutputNode(privP->sourceSensorGroup, NULL),&imageHdr.readoutTs);
      if( privP->cropUpdateRequest == 1 )
      {
         PPE_MNGRG_cropParamT crop;
         crop.xStart = privP->cropXStart;
         crop.yStart = privP->cropYStart;
         crop.width =  imageHdr.imgDescriptor.width - 1;
         crop.height = imageHdr.imgDescriptor.height - 1;
//         printf("Cropping update x %d y %d w %d h %d\n", crop.xStart, crop.yStart, crop.width, crop.height);
         if (handle)
         {
            ret = SEQ_MNGRG_updateChannelCrop(handle, SEQ_MNGRG_XML_BLOCK_RDOUT_E, privP->ctorParams.nuCfgChId, &crop );
         }
         if ((!handle) || ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "Cropping update on channel %d failed\n", privP->ctorParams.nuCfgChId);
         }
         else
         {
            //crop success, update the image header of source for next frames
            inu_data *data = (inu_data*)node;
            inu_image__hdr_t imageHdrNextFrames = imageHdr;
            imageHdrNextFrames.imgDescriptor.x = crop.xStart;
            imageHdrNextFrames.imgDescriptor.y = crop.yStart;            
            ((inu_data__VTable*)data->node.ref.p_vtable)->p_dataHdrSet(data, &imageHdrNextFrames);         
         }
         privP->cropUpdateRequest = 0;       
      }
      //traverse over the sensors, and fill the exposure and gain information
      i = 0;
      sensor = inu_node__getNextOutputNode(privP->sourceSensorGroup,NULL);
      while(sensor)
      {
         //todo context
         imageHdr.exposureGain[i].nucfgId = inu_sensor__getId(sensor);
         inu_sensor__getExposureGainInfo(sensor, ALTG_getThisFrameMode(inu_sensors_group__getAltHandle(privP->sourceSensorGroup)), 
                                                    &imageHdr.exposureGain[i].exposure,
                                                    &imageHdr.exposureGain[i].digitalGain,
                                                    &imageHdr.exposureGain[i].analogGain);
         //printf("sensor%d: exp %d, dig gain %d, analog gain %d",
         //       nucfgId,imageHdr.exposureGain[nucfgId].exposure,imageHdr.exposureGain[nucfgId].digitalGain,imageHdr.exposureGain[nucfgId].analogGain);
         sensor = inu_node__getNextOutputNode(privP->sourceSensorGroup,sensor);
         i++;
      }
      //printf("\n");
   }
   else
   {
      //input might be generator or injection. Keep ts and ctr from DMA isr
      imageHdr.dataHdr.dataIndex = userParams->systemFrameCntr;
      imageHdr.dataHdr.timestamp = userParams->timeStamp;
   }

   ret = inu_function__newData((inu_function*)me, (inu_data*)node, userParams->buffDescriptorP, &imageHdr, (inu_data**)&image);
   if (ERRG_SUCCEEDED(ret))
   {
      #ifdef ENABLE_HANDLE_IMMEDIATE_SOC_CHANNELS
      inu_function__doneData_handleImmediately((inu_function*)me, (inu_data *)image);
      #else
      inu_function__doneData((inu_function*)me,(inu_data*)image);
      #endif
   }
}
#endif

static ERRG_codeE inu_soc_channel__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   inu_sensors_group *sensorGroupP;
   inu_soc_channel *socChannelP =(inu_soc_channel *)me;
   inu_soc_channel__privData *privP = (inu_soc_channel__privData*)socChannelP->privP;
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
         char *userName = privP->ctorParams.functionCtorParams.nodeCtorParams.ref_params.userName;

         if (strstr(userName, "EXT")) {
            SEQ_MNGRG_markExtIntChannel(handle, privP->ctorParams.nuCfgChId);
            INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL, "SoC Channel %d use extend DMA interleaving mode\n", privP->ctorParams.nuCfgChId);
         }

         ret = SEQ_MNGRG_configChannel(handle, SEQ_MNGRG_XML_BLOCK_RDOUT_E, privP->ctorParams.nuCfgChId,TRUE,me, &privP->channelH);
         if (ERRG_SUCCEEDED(ret))
         {
            ret = SEQ_MNGRG_startChannel(handle, SEQ_MNGRG_XML_BLOCK_RDOUT_E, privP->ctorParams.nuCfgChId,(void *)inu_soc__frameDoneCb,(void *)me, privP->channelH);
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


static ERRG_codeE inu_soc_channel__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   inu_soc_channel *socChannelP=(inu_soc_channel *)me;
   inu_soc_channel__privData *privP = (inu_soc_channel__privData*)socChannelP->privP;
   SEQ_MNGRG_handleT handle;

   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
   if (ERRG_SUCCEEDED(ret))
   {
      handle = inu_graph__getSeqDB(inu_node__getGraph(me));
      if (handle)
      {
         ret = SEQ_MNGRG_stopChannel(handle, SEQ_MNGRG_XML_BLOCK_RDOUT_E, privP->ctorParams.nuCfgChId,me, privP->channelH);
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


void inu_soc_channel__vtable_init( void )
{
   if (!_bool_vtable_initialized) 
   {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_soc_channel__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_soc_channel__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_soc_channel__ctor;
      _vtable.node_vtable.ref_vtable.p_rxSyncCtrl = inu_soc_channel__rxIoctl;

      _vtable.p_start = inu_soc_channel__start;
      _vtable.p_stop  = inu_soc_channel__stop;

      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_soc_channel__vtable_get(void)
{
   inu_soc_channel__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}

UINT32 inu_soc_channel__getId(inu_soc_channelH meH)
{
   return ((inu_soc_channel__privData*)((inu_soc_channel*)meH)->privP)->ctorParams.nuCfgChId;
}

ERRG_codeE inu_soc_channel__updateCropWindow(inu_soc_channelH meH, inu_soc_channel__cropParamsT *crop)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_CHANGE_CROP_WINDOW_E,crop, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE inu_soc_channel__updateUserParams(inu_soc_channelH meH, inu_soc_channel__userParamsT *user)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_UPDATE_CHANNEL_USER_PARAM_E,user, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE inu_soc_channel__resetFrameCnt(inu_soc_channelH meH)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_RESET_FRAME_CNT_E, NULL, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}


static int inu_soc_channel__rxIoctl(inu_ref *ref, void *msgP, int msgCode)
{
   inu_soc_channel *me = (inu_soc_channel*)ref;
   inu_soc_channel__privData *privP = (inu_soc_channel__privData*)me->privP;
   int ret = INU_SOC_CHANNEL__RET_SUCCESS;
   ret = inu_function__vtable_get()->node_vtable.ref_vtable.p_rxSyncCtrl(ref, msgP, msgCode);
#if DEFSG_IS_GP
   //printf("inu_soc_channel__rxIoctl: msg Code %x\n", msgCode);

   switch (msgCode)
   {
      case(INTERNAL_CMDG_CHANGE_CROP_WINDOW_E):
      {
         inu_soc_channel__cropParamsT *crop = (inu_soc_channel__cropParamsT *)msgP;
         privP->cropUpdateRequest = 1;
         privP->cropXStart = crop->xStart;
         privP->cropYStart = crop->yStart;
         INU_REF__LOGG_PRINT(ref, LOG_DEBUG_E, NULL, "Crop request: x - %d, y - %d\n", privP->cropXStart, privP->cropYStart);
      }
      break;

      case(INTERNAL_CMDG_UPDATE_CHANNEL_USER_PARAM_E):
      {
         // if channel counts strobes
         inu_soc_channel__userParamsT *user = (inu_soc_channel__userParamsT*)msgP;
         privP->userParam1 = user->param1;
         privP->userParam2 = user->param2;
      }
      break;

      case (INTERNAL_CMDG_RESET_FRAME_CNT_E):
      {
         IO_HANDLE sensor_group = NULL;
         ret = inu_sensors_group__getSensorHandle(privP->sourceSensorGroup, &sensor_group);
         if (ERRG_SUCCEEDED(ret))
         {
            SENSORS_MNGRG_resetStrobeCnt(sensor_group);
         }
      }
      break;
      default:
//      ret = INU_SOC_CHANNEL__ERR_NOT_SUPPORTED;
      break;
   }
#else
   FIX_UNUSED_PARAM_WARN(privP);
#endif
   return ret;
}

#ifdef __cplusplus
}
#endif
