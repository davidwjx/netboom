
#include "inu2.h"
#include "inu_isp_channel.h"
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
#include "isp_mngr.hpp"
#include "helsinki.h"
#include "helsinki_isp_synchronizer.h"

#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
   inu_isp_channel__CtorParams ctorParams;
   char name[35];
   inu_sensors_group *sourceSensorGroup;
#if DEFSG_IS_GP
   CDE_MNGRG_channnelInfoT *dmaChanInfoP;
   SEQ_MNGRG_channelH channelH;
   //MEM_POOLG_handleT memPoolH;
   UINT8 cropUpdateRequest;
   UINT16 cropXStart;
   UINT16 cropYStart;
   UINT32 userParam1;
   UINT32 userParam2;
   UINT32 width;
   UINT32 height;
   UINT32 bpp;
   UINT32 size;
   UINT32 frameId;
#endif
}inu_isp_channel__privData;

#if DEFSG_IS_GP
static bool dmaDoneCopiedFlag = FALSE;

static void inu_isp_channel__ispDmaDoneCb(CDE_MNGRG_userCbParamsT *cbParam, void *arg)
{
   inu_isp_channel__privData   *privP = (inu_isp_channel__privData*)((inu_isp_channel*)arg)->privP;

   CDE_MNGRG_stopChannelMemcpy(privP->dmaChanInfoP);
   dmaDoneCopiedFlag = TRUE;

   LOGG_PRINT(LOG_INFO_E, NULL, "Copy DMA Done\n");
}
#endif

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static int inu_isp_channel__rxIoctl(inu_ref *ref, void *msgP, int msgCode);

static const char* inu_isp_channel__name(inu_ref *me)
{
   inu_isp_channel *isp_ch = (inu_isp_channel*)me;
   inu_isp_channel__privData *privP = (inu_isp_channel__privData*)isp_ch->privP;
   return privP->name;
}

ERRG_codeE inu_isp_channel__sendIspCommand(inu_isp_channelH meH, inu_isp_channel__commandE cmd, inu_isp_channel__ispCmdParamU *param)
{
   ERRG_codeE ret;

   inu_isp_channel__commandT command;

   command.cmd = cmd;
   memcpy((void*)&command.cmdParams,(void*)param, sizeof(inu_isp_channel__ispCmdParamU));

   ret = inu_ref__sendCtrlSync((inu_ref*)meH, INTERNAL_CMDG_SEND_ISP_COMMAND_E, &command, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
   //printf("!!!! %s:: command %d, param_0 %d  INPUT ( %d %f) \n", __func__, command.cmd, command.cmdParams[0], cmd, param->exposureParam.integrationTime);
   //copy an answer
   memcpy((void*)param, (void*)&command.cmdParams, sizeof(inu_isp_channel__ispCmdParamU));

   return ret;
}

static void inu_isp_channel__dtor(inu_ref *me)
{
   inu_isp_channel *isp_ch = (inu_isp_channel*)me;
   inu_isp_channel__privData *privP = (inu_isp_channel__privData*)isp_ch->privP;
#if DEFSG_IS_GP
#endif
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
   free(((inu_isp_channel*)me)->privP);
}


/* Constructor */
ERRG_codeE inu_isp_channel__ctor(inu_isp_channel *me, inu_isp_channel__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_isp_channel__privData *privP;
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_isp_channel__privData*)malloc(sizeof(inu_isp_channel__privData));
      if (privP)
      {
         memset(privP,0,sizeof(inu_isp_channel__privData));
         memcpy(&privP->ctorParams, ctorParamsP, sizeof(inu_cva_channel__CtorParams));
         me->privP = privP;

         char nucfgChId[20];

         strcpy(privP->name,"INU_ISP_CHANNEL");
         if (privP)
         {
            sprintf(nucfgChId, ", nucfgChId: %d", privP->ctorParams.nuCfgChId);
            strcat(privP->name,nucfgChId);
         }
      }
      else
      {
         ret = INU_ISP_CHANNEL__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}


#if DEFSG_IS_GP
static void inu_isp_channel__frameDoneCb(void *pPhyBuff, void *arg)
{
   ERRG_codeE       ret;
   inu_image__hdr_t imageHdr;
   inu_image        *image;
   inu_node         *node;
   inu_isp_channel  *me = (inu_isp_channel*)arg;
   inu_isp_channel__privData *privP = (inu_isp_channel__privData*)me->privP;
   SEQ_MNGRG_handleT handle = inu_graph__getSeqDB(inu_node__getGraph(me));
   UINT32           i;
   UINT32           projMode;
   UINT64           strobeTs,strobeCtr;
   inu_sensor                 *sensor = NULL;
   inu_function__stateParam   stateParam;
   MEM_POOLG_bufDescT         *bufDescP = NULL;
   UINT32                     bufferPhyAddress;

#if 1
//without memcpy between ISP buffer to our pool buffer
   bufDescP = (MEM_POOLG_bufDescT*)pPhyBuff;
   bufDescP->dataLen = privP->size;
#else
//with memcpy between ISP buffer to our pool buffer
   ret = MEM_POOLG_alloc(privP->memPoolH, privP->size, &bufDescP);
   //if (MEM_POOLG_getNumFree(privP->memPoolH)<10)
   //   printf("inu_isp_channel__frameDoneCb alloc buf %p isp %d num free %d\n",bufDescP,privP->ctorParams.nuCfgChId,MEM_POOLG_getNumFree(privP->memPoolH));
   if (ERRG_SUCCEEDED(ret))
   {
      ret = MEM_POOLG_getDataPhyAddr(bufDescP,&bufferPhyAddress);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to alloc buf\n");
      return;
   }

   //ret = CDE_MNGRG_memcpy(privP->dmaChanInfoP, bufferPhyAddress,(UINT32)pPhyBuff, NULL);
   ret = CDE_MNGRG_memcpyBlock(privP->dmaChanInfoP, bufferPhyAddress,(UINT32)pPhyBuff, privP->size);
   bufDescP->dataLen = privP->size;

   //while (!dmaDoneCopiedFlag);

   dmaDoneCopiedFlag = FALSE;

#endif

   //if function is stopped, but the HW is not, we ignore the interrupts
   inu_function__getState(me,&stateParam);
   if (stateParam.state == DISABLE)
   {
      return;
   }

   node = inu_node__getNextOutputNode(me,NULL);

   //CHECK #1
   if (!node)
   {
      printf("no output inu_data for inu_isp_channel, release data\n");
      return;
   }

   //fill with image descriptor params
   memcpy(&imageHdr,inu_data__getHdr(node),sizeof(imageHdr));

   //save chunk num
   // imageHdr.dataHdr.chunkNum = userParams->chunkNum;

   //if the input are real sensor(s) than:
   if (privP->sourceSensorGroup)
   {
      imageHdr.sensorGroup = inu_sensors_group__getId(privP->sourceSensorGroup);
      inu_data__setMode((inu_data__hdr_t*)&imageHdr,ALTG_getThisFrameMode(inu_sensors_group__getAltHandle(privP->sourceSensorGroup)));
      //TBD: temp solution, taking timestamp and frame id here
      UINT64 time;
      OS_LYRG_getTimeNsec(&time);

      imageHdr.dataHdr.dataIndex = ++privP->frameId;
      imageHdr.dataHdr.timestamp = time;

      /*HELSINKI specfic change, we need to update the ISP parameters across all ISPs*/
      #ifdef HELSINKI_ENABLE_ISP_SYNCHRONIZER
      if((imageHdr.dataHdr.dataIndex > 100) && (privP->ctorParams.nuCfgChId==23)) 
      {
         if((imageHdr.dataHdr.dataIndex % HELSINKI_SYNC_PERIOD) == 0) /*Slow down how often this runs to save CPU and UART time!*/
         {
            HELSINKI_notifyISPSyncThread(imageHdr.dataHdr.dataIndex,imageHdr.dataHdr.timestamp);
         }
      }
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
      //input might be generator or injection. Keep ts and ctr from DMA ISR
   }

   ret = inu_function__newData((inu_function*)me, (inu_data*)node, bufDescP /*userParams->buffDescriptorP*/, &imageHdr, (inu_data**)&image);
   if (ERRG_SUCCEEDED(ret))
   {
      ret = inu_function__doneData((inu_function*)me,(inu_data*)image);
   }
}

static ERRG_codeE inu_isp_channel__InitThread(inu_function *me, UINT32 ispRdNum)
{
   ERRG_codeE ret;

   ret = ISP_MNGRG_ThreadInit((void*)me, ispRdNum, (void*)inu_isp_channel__frameDoneCb);

   return ret;
}

static void inu_Isp_channel__getSize(inu_isp_channel *me)
{
   inu_isp_channel__privData *privP = (inu_isp_channel__privData*)me->privP;
   inu_image__hdr_t imageHdr;
   inu_image        *image;
   inu_node         *node;

   node = inu_node__getNextOutputNode(me,NULL);

   //fill with image descriptor params
   memcpy(&imageHdr,inu_data__getHdr(node),sizeof(imageHdr));

   privP->width = imageHdr.imgDescriptor.width;
   privP->height = imageHdr.imgDescriptor.height;
   privP->bpp = NUCFG_format2Bpp((NUCFG_formatE)imageHdr.imgDescriptor.format);
#ifdef ISP_BUFFER_ALIGN_SIZE
   UINT32 metadata_size = inu_metadata__getNumberMetadataRows() * privP->width *  ((privP->bpp+7)/8);
   metadata_size = METADATA_SIZE_ALIGN(metadata_size);
   privP->size = (privP->width * privP->height * privP->bpp) / 8;
   privP->size += metadata_size;
   printf("inu_Isp_channel__getSize size:%u\n", privP->size);
#else
   privP->size = (privP->width * privP->height * privP->bpp) / 8;
#endif
   //printf("width = %d, height = %d, bpp = %d, size = %d\n", privP->width, privP->height, privP->bpp, privP->size);


}
#endif

static ERRG_codeE inu_isp_channel__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   inu_sensors_group *sensorGroupP;
   inu_isp_channel *ispChannelP =(inu_isp_channel *)me;
   inu_isp_channel__privData *privP = (inu_isp_channel__privData*)ispChannelP->privP;
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
         MEM_POOLG_cfgT poolCfg;
         printf("inu_isp_channel__start chId %d\n",privP->ctorParams.nuCfgChId);

         inu_Isp_channel__getSize(ispChannelP);

         poolCfg.numBuffers = 15;//TBD: need to measurment and define
         poolCfg.type = MEM_POOLG_TYPE_ALLOC_CMEM_E;
         poolCfg.bufferSize = (privP->width * privP->height * privP->bpp) / 8;
         poolCfg.freeCb = NULL;
         poolCfg.memP = NULL;
         poolCfg.freeArg = NULL;
         poolCfg.resetBufPtrInAlloc = 0;
         //ret = MEM_POOLG_initPool(&privP->memPoolH, &poolCfg);
         ret = SEQ_MNGRG_configChannel(handle, SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E, privP->ctorParams.nuCfgChId,TRUE,me, &privP->channelH);

         if (ERRG_SUCCEEDED(ret))
         {
            ret = CDE_MNGRG_memcpyOpenChan(&privP->dmaChanInfoP, privP->size, /*inu_isp_channel__ispDmaDoneCb*/NULL, me);
         }

         if (ERRG_SUCCEEDED(ret))
         {
            ret = inu_isp_channel__InitThread(me, privP->ctorParams.nuCfgChId);
         }

         if (ERRG_SUCCEEDED(ret))
         {
            ret = SEQ_MNGRG_startChannel(handle, SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E, privP->ctorParams.nuCfgChId,(void *)inu_isp_channel__frameDoneCb,(void *)me, privP->channelH);
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


static ERRG_codeE inu_isp_channel__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   inu_isp_channel *ispChannelP=(inu_isp_channel *)me;
   inu_isp_channel__privData *privP = (inu_isp_channel__privData*)ispChannelP->privP;
   SEQ_MNGRG_handleT handle;

   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
   if (ERRG_SUCCEEDED(ret))
   {
      handle = inu_graph__getSeqDB(inu_node__getGraph(me));
      if (handle)
      {
         ret = SEQ_MNGRG_stopChannel(handle, SEQ_MNGRG_XML_BLOCK_ISP_RDOUT_E, privP->ctorParams.nuCfgChId,me, privP->channelH);
         if (ERRG_SUCCEEDED(ret))
         {
            //printf(MAGENTA("%s:: SEQ_MNGRG_stopChannel finished\n"), __func__);
         }

         //MEM_POOLG_waitBuffReleaseClosePool(privP->memPoolH);
         CDE_MNGRG_memcpyCloseChan(privP->dmaChanInfoP);
         //printf(MAGENTA("%s:: CDE_MNGRG_memcpyCloseChan finished\n"), __func__);
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


void inu_isp_channel__vtable_init( void )
{
   if (!_bool_vtable_initialized)
   {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_isp_channel__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_isp_channel__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_isp_channel__ctor;
      _vtable.node_vtable.ref_vtable.p_rxSyncCtrl = inu_isp_channel__rxIoctl;

      _vtable.p_start = inu_isp_channel__start;
      _vtable.p_stop  = inu_isp_channel__stop;

      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_isp_channel__vtable_get(void)
{
   inu_isp_channel__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}
/*INT32 inu_isp_channel__getId(inu_isp_channelH meH)
{
   return ((inu_isp_channel__privData*)((inu_isp_channel*)meH)->privP)->ctorParams.nuCfgChId;
}

ERRG_codeE inu_isp_channel__updateCropWindow(inu_isp_channelH meH, inu_isp_channel__cropParamsT *crop)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_CHANGE_CROP_WINDOW_E,crop, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE inu_isp_channel__updateUserParams(inu_isp_channelH meH, inu_isp_channel__userParamsT *user)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_UPDATE_CHANNEL_USER_PARAM_E,user, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE inu_isp_channel__resetFrameCnt(inu_isp_channelH meH)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_RESET_FRAME_CNT_E, NULL, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}
*/

static int inu_isp_channel__rxIoctl(inu_ref *ref, void *msgP, int msgCode)
{
   inu_isp_channel *me = (inu_isp_channel*)ref;
   inu_isp_channel__privData *privP = (inu_isp_channel__privData*)me->privP;
   ERRG_codeE ret = INU_ISP_CHANNEL__RET_SUCCESS;
   ret = (ERRG_codeE)inu_function__vtable_get()->node_vtable.ref_vtable.p_rxSyncCtrl(ref, msgP, msgCode);
#if DEFSG_IS_GP
   //printf("!!!!!!!!inu_isp_channel__rxIoctl: msg Code %x\n", msgCode);

   switch (msgCode)
   {
      case(INTERNAL_CMDG_SEND_ISP_COMMAND_E):
      {
         inu_isp_channel__commandT *command = (inu_isp_channel__commandT *)msgP;
         UINT32 ispRdNum = privP->ctorParams.nuCfgChId;

         switch (command->cmd)
         {
            case(INU_ISP_COMMAND_SET_EXPOSURE_E):
            {
               inu_isp_channel__exposureParamT *exp = (inu_isp_channel__exposureParamT *)command->cmdParams;

               LOGG_PRINT(LOG_INFO_E, NULL, "SET PARAMS: ispRdNum = %d Command %d  integration-time %f, analog-gain %f isp-gain %f\n", ispRdNum, command->cmd, exp->integrationTime, exp->analogGain, exp->ispGain);
               ret = ISP_MNGRG_setExposureParams(ispRdNum, exp);
            }
            break;
            case(INU_ISP_COMMAND_GET_EXPOSURE_E):
            {
               inu_isp_channel__exposureParamT *exp = (inu_isp_channel__exposureParamT *)command->cmdParams;

               ret = ISP_MNGRG_getExposureParams(ispRdNum, exp);
               LOGG_PRINT(LOG_INFO_E, NULL, "GET PARAMS: ispRdNum = %d Command %d  integration-time %f, analog-gain %f isp-gain %f\n", ispRdNum, command->cmd, exp->integrationTime, exp->analogGain, exp->ispGain);
            }
            break;
            case(INU_ISP_COMMAND_SET_WB_E):
            {
               inu_isp_channel__ispWbParamT *wb = (inu_isp_channel__ispWbParamT *)command->cmdParams;
               ret = ISP_MNGRG_setWbParams(ispRdNum, wb);
               //LOGG_PRINT(LOG_INFO_E, NULL, "GET PARAMS: ispRdNum = %d Command %d  integration-time %f, analog-gain %f isp-gain %f\n", ispRdNum, command->cmd, exp->integrationTime, exp->analogGain, exp->ispGain);
            }
            break;

            case(INU_ISP_COMMAND_GET_WB_E):
            {
               inu_isp_channel__ispWbParamT *wb = (inu_isp_channel__ispWbParamT *)command->cmdParams;
               ret = ISP_MNGRG_getWbParams(ispRdNum, wb);
               //LOGG_PRINT(LOG_INFO_E, NULL, "GET PARAMS: ispRdNum = %d Command %d  integration-time %f, analog-gain %f isp-gain %f\n", ispRdNum, command->cmd, exp->integrationTime, exp->analogGain, exp->ispGain);
            }
            break;

            case (INU_ISP_COMMAND_SET_LSC_E):
            {
               inu_isp_channel__ispLscParamT *lsc = (inu_isp_channel__ispLscParamT *)command->cmdParams;
               ret = ISP_MNGRG_setLscParams(ispRdNum, lsc);
               LOGG_PRINT(LOG_INFO_E, NULL, "INU_ISP_COMMAND_SET_LSC_E done\n");
            }
            break;

            case (INU_ISP_COMMAND_GET_LSC_E):
            {
               inu_isp_channel__ispLscParamT *lsc = (inu_isp_channel__ispLscParamT *)command->cmdParams;
               ret = ISP_MNGRG_getLscParams(ispRdNum, lsc);
               LOGG_PRINT(LOG_INFO_E, NULL, "INU_ISP_COMMAND_GET_LSC_E done");
            }
            break;

            default:
            {
               ret = INU_ISP_CHANNEL__ERR_NOT_SUPPORTED;
               LOGG_PRINT(LOG_INFO_E, ret, "Invalid ISP_COMMAND [%d]. Valid commands are between [%d] and [%d]\n", command->cmd, INU_ISP_COMMAND_FIRST_E, INU_ISP_COMMAND_LAST_E);
            }
            break;
         }

         if (ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "ISP_COMMAND [%d] Failed.\n", command->cmd);
         }
      }
      break;

   /*
      case(INTERNAL_CMDG_CHANGE_CROP_WINDOW_E):
      {
         inu_isp_channel__cropParamsT *crop = (inu_isp_channel__cropParamsT *)msgP;
         privP->cropUpdateRequest = 1;
         privP->cropXStart = crop->xStart;
         privP->cropYStart = crop->yStart;
         INU_REF__LOGG_PRINT(ref, LOG_DEBUG_E, NULL, "Crop request: x - %d, y - %d\n", privP->cropXStart, privP->cropYStart);
      }
      break;

      case(INTERNAL_CMDG_UPDATE_CHANNEL_USER_PARAM_E):
      {
         // if channel counts strobes
         inu_isp_channel__userParamsT *user = (inu_isp_channel__userParamsT*)msgP;
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
      break;*/
      default:
//      ret = INU_ISP_CHANNEL__ERR_NOT_SUPPORTED;
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
