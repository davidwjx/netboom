#include "metadata_sensor_updater.h"
#include "helsinki.h"
#include "metadata_updater.h"
#include "gen_sensor_drv.h"
#include "ppe_mngr.h"
#include "focus_ctrl.h"
#include "inu_sensor_api.h"
#include <signal.h>
#include "isp_mngr.hpp"
#include "helsinki_isp_synchronizer.h"
#include "vsi3a/vsi3a_types.h"
#include "sensorsync_updater.h"
/*This file handles the update of ISP parameters and also the metadata for ISP parameters*/
/**
 * @brief Updates the white balance metadata in frame buffers after NFrames
 *
 * @param entry METADATA_UPDATER_channelHandleT to store the metadata with the new exposure value in
 * @param sensorIndex Sensor Index (0 or 1)
 * @param N  N (when to store the new exposure in)
 * @param wbParameters White balance value
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE METADATA_ISP_UPDATER_updateWBGainsAfterNFrames(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex, const METADATA_SENSOR_UPDATER_frameTimingT N, 
    INU_metadata_ISP_WBPameters *wbParameters)
{
  const INT32 startIndex = METADATA_SENSOR_UPDATER_calculateFramesAfterStartIndex(N);
  if (sensorIndex >= 2)
  {
    return INU_METADATA__ERR_INVALID_ARGS;
  }
  if (entry == NULL)
  {
    return INU_METADATA__ERR_INVALID_ARGS;
  }
  if (startIndex >= 0)
  {
    /* It's safe to update ALL of the protobuf structures at any time because the DMAC only uses the CMEM buffer (which contains serialized metadaata)*/
    const UINT32 nextBuffer = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
    for (int i = startIndex; i < (MAX_NUM_BUFFERS_LOOPS); i++)
    {
      const UINT32 INDEX = (nextBuffer + i) % MAX_NUM_BUFFERS_LOOPS; /*Calculate the buffer index */
      /*Copy the WB Parameters*/
      entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.wb = *wbParameters;
      entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.has_wb = true;
      entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].has_sensor_metadata = true;
    }
  }
  return INU_METADATA__RET_SUCCESS;
}
/**
 * @brief Updates the ISP exposre metadata in frame buffers after N Frames
 *
 * @param entry METADATA_UPDATER_channelHandleT to store the metadata with the new exposure value in
 * @param sensorIndex Sensor Index (0 or 1)
 * @param N  N (when to store the new exposure in)
 * @param expoParameters Exposure value
 * @param updateAGain Update the Analog Gain
 * @param updateExposure Update the Exposure
 * @param updateDGain Update the Digital Gain
 * @param updateOtherParams Update the other parameters
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE METADATA_ISP_UPDATER_updateExpoAfterNFrames(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex, const METADATA_SENSOR_UPDATER_frameTimingT N, 
    INU_metadata_ISP_ExpoParams *expoParameters, const bool updateAGain, const bool updateExposure, const bool updateDGain, const bool updateOtherParams)
{
  const INT32 startIndex = METADATA_SENSOR_UPDATER_calculateFramesAfterStartIndex(N);
  METADATA_UPDATER_markFutureUpdatePending(entry,N);  /*Please read the comments around this function if you want an explanation of what this does*/
  LOGG_PRINT(LOG_DEBUG_E,NULL,"Start index: %lu \n", startIndex);
  UINT32 expoValue = 0;
  if (sensorIndex >= 2)
  {
    return INU_METADATA__ERR_INVALID_ARGS;
  }
  if (entry == NULL)
  {
    return INU_METADATA__ERR_INVALID_ARGS;
  }
  if (startIndex >= 0)
  {
    /* It's safe to update ALL of the protobuf structures at any time because the DMAC only uses the CMEM buffer (which contains serialized metadaata)*/
    uint32_t frameID;
    METADATA_UPDATER_getFullFrameIDWithOverFlowCounter(entry->mapentry,&frameID);
    const UINT32 nextBuffer = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
    if(updateExposure)
    {
      //printf("(%lu) exposure to be %.3f \n",frameID,1E6*expoParameters->integrationTime);
    }
    if(updateAGain)
    {
      //printf("(%lu) gain to be %.3f \n",frameID, expoParameters->analogGain);
    }
    for (int i = startIndex; i < (MAX_NUM_BUFFERS_LOOPS); i++)
    {
      const UINT32 INDEX = (nextBuffer + i) % MAX_NUM_BUFFERS_LOOPS; /*Calculate the buffer index */
      /*Copy the Expo Parameters*/
      expoValue = (UINT32)(expoParameters->integrationTime * 1000000);
      if(updateExposure)
      {
        LOGG_PRINT(LOG_DEBUG_E,NULL,"(%lu) exposure to be %.3f \n",frameID,1E6*expoParameters->integrationTime);
        entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.isp_expo.integrationTime = expoValue;
        entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.has_isp_expo = true;
        entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.exposure.exposure = expoValue;
        entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.has_exposure = true;
      }
      if(updateDGain)
      {
        entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.isp_expo.ispGain = expoParameters->ispGain;
      }
      if(updateAGain)
      {
        LOGG_PRINT(LOG_DEBUG_E,NULL,"(%lu) gain to be %.3f \n",frameID, expoParameters->analogGain);
        entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.gain.analogue_gain = expoParameters->analogGain;
        entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.has_gain = true;    
        entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.isp_expo.analogGain = expoParameters->analogGain;
        entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.has_isp_expo = true;
      }
      if(updateOtherParams)
      {
        entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.isp_expo.has_ispAE = expoParameters->has_ispAE;
        entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.isp_expo.has_ispAEclosedLoop = expoParameters->has_ispAEclosedLoop;
        entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.isp_expo.ispAEclosedLoop = expoParameters->ispAEclosedLoop;
        entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.isp_expo.has_uart_sync = expoParameters->has_uart_sync;
        entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.isp_expo.ispAE = expoParameters->ispAE;
        entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.isp_expo.uart_sync = expoParameters->uart_sync;
      }
   
    
      entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].has_sensor_metadata = true;      
      #ifdef ENABLE_METADATA_DEBUG
      printf("(%lu) W[%lu], expo %.3f \n",frameID,i,entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.isp_expo.integrationTime);
      #endif
    }
  }
  return INU_METADATA__RET_SUCCESS;
}
void METADATA_ISP_UPDATER_copyWBParams( const struct Vsi3AMetaWb *wbParameters, INU_metadata_ISP_WBPameters *ispwbParameters )
{

  for(unsigned int i =0; i<ISP_CHANNEL_CCMATRIX_SIZE; i++ )
  {
    ispwbParameters->colourcorrectinCoefficients.coeff[i] = wbParameters->CCM[i];
  }
  ispwbParameters->colourcorrectinCoefficients.coeff_count = ISP_CHANNEL_CCMATRIX_SIZE;
  ispwbParameters->has_colourcorrectinCoefficients = true;
  /*Copy colour correction offsets*/
  ispwbParameters->colourcorrection.blue = wbParameters->offset[0];
  ispwbParameters->colourcorrection.green  = wbParameters->offset[1];
  ispwbParameters->colourcorrection.red  = wbParameters->offset[2];
  ispwbParameters->has_colourcorrection = true;
  /*Copy WB Gains */
  ispwbParameters->wbGains.gainR = wbParameters->awbGain[0];
  ispwbParameters->wbGains.gainGr = wbParameters->awbGain[1];
  ispwbParameters->wbGains.gainGb = wbParameters->awbGain[2];
  ispwbParameters->wbGains.gainB = wbParameters->awbGain[3];
  ispwbParameters->has_wbGains = true;
  ispwbParameters->has_ispAWB = wbParameters->hasmetaDataEnableAWB;
  ispwbParameters->ispAWB.metadataEnableAWB = wbParameters->metaDataEnableAWB;
  /*Do not copy callback information as this isn't relevant*/
}
void METADATA_ISP_UPDATER_copyExpoParams( const struct Vsi3AMetaExp *expoParameters, INU_metadata_ISP_ExpoParams *ispExpoParameters )
{
  ispExpoParameters->analogGain = expoParameters->expsoureGain;
  ispExpoParameters->has_ispAE = expoParameters->hasmetaDataEnableAE;
  ispExpoParameters->integrationTime = expoParameters->expsoureTime;
  ispExpoParameters->ispAE.metadataEnableAE = expoParameters->hasmetaDataEnableAE;
  ispExpoParameters->ispGain = expoParameters->ispDgain;
  
}

ERRG_codeE METADATA_ISP_UPDATER_updateWBGains(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex,const METADATA_SENSOR_UPDATER_interruptNotification *interruptTiming, 
  INU_metadata_State *incomingSensorState, INU_DEFSG_sensorContextE  context,   METADATA_UPDATE_registeredSensor *sensor)
{
    ERRG_codeE ret = INU_METADATA__RET_SUCCESS;
    LOGG_PRINT(LOG_DEBUG_E,NULL,"ISP white balance updater\n");
    const INU_metadata_ISP_WBPameters *wbParameters = &incomingSensorState->sensor_metadata.wb;
    inu_isp_channel__ispWbParamT ispWbParameters;
    if(wbParameters->colourcorrectinCoefficients.coeff_count != ISP_CHANNEL_CCMATRIX_SIZE)
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"WB Matrix size of %lu!=%lu, ISP values cannot be updated \n",
            wbParameters->colourcorrectinCoefficients.coeff_count,ISP_CHANNEL_CCMATRIX_SIZE );
        return INU_METADATA__ERR_METADATA_INVALID_COLOUR_CORRECTION_MATRIX;
    }
    /*Check that all three sets of values were provided*/
    if(wbParameters->has_colourcorrectinCoefficients && wbParameters->has_colourcorrection && wbParameters->has_wbGains)
    {
        /*Copy CC Matrix*/
        for(unsigned int i =0; i<ISP_CHANNEL_CCMATRIX_SIZE; i++ )
        {
            ispWbParameters.ccMatrix.coeff[i] = wbParameters->colourcorrectinCoefficients.coeff[i];
        }
        /*Copy colour correction offsets*/
        ispWbParameters.ccOffset.red = wbParameters->colourcorrection.red;
        ispWbParameters.ccOffset.green = wbParameters->colourcorrection.green;
        ispWbParameters.ccOffset.blue = wbParameters->colourcorrection.blue;
        /*Copy WB Gains */
        ispWbParameters.wbGain.gainR = wbParameters->wbGains.gainR;
        ispWbParameters.wbGain.gainGr = wbParameters->wbGains.gainGr;
        ispWbParameters.wbGain.gainGb = wbParameters->wbGains.gainGb;
        ispWbParameters.wbGain.gainB = wbParameters->wbGains.gainB;
        ispWbParameters.hasmetadataEnableAWB = wbParameters->has_ispAWB;
        ispWbParameters.metadataEnableAWB = wbParameters->ispAWB.metadataEnableAWB;
        /*Do not write AWB CB*/
        ispWbParameters.has_awb_cb = false;
        ispWbParameters.awb_cb = NULL;
        #if 0  // TODO
        ispWbParameters.has_AWB_closedLoop = wbParameters->has_ispAWBclosedLoop;
        ispWbParameters.AWB_closedLoop = wbParameters->ispAWBclosedLoop.awb_closedLoop;
        #endif
        if(ispWbParameters.hasmetadataEnableAWB)
          /*Enable UART Sync if AWB is enabled, disable UART sync if AWB is disabled*/
          HELSINKI_ISP_SYNCHRONIZER_set_AWBUartSyncState(!ispWbParameters.metadataEnableAWB); /* False = AWB enabled, True = AWB disabled (From 3A JSON logic..)*/
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Updating the White balance gains \n");
        /*Write the White balance parameters*/
        ret = ISP_MNGRG_setWbParams(entry->mapentry->axiReader,&ispWbParameters);


        if(ERRG_FAILED(ret))
            LOGG_PRINT(LOG_ERROR_E,NULL,"Couldn't update the ISP WB gains\n");
        METADATA_ISP_UPDATER_updateWBGainsAfterNFrames(entry,sensorIndex,METADATA_SENSOR_UPDATER_N_1,
            &incomingSensorState->sensor_metadata.wb);
        return ret;
    }
    else
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Missing ISP White balance parameters \n");
        return INU_METADATA__ERR_METADATA_INVALID_WB_VALUES;
    }
}

static void METADATA_ISP_UPDATER_AddToDelayedGainQueue(METADATA_UPDATER_channelHandleT *entry,INU_metadata_ISP_ExpoParams *expoParams, uint32_t sensorIndex )
{
  if(!entry)
    return;

  METADATA_AR2020Gain_Queue_Entry queueEntry;
  queueEntry.sensor_index = sensorIndex;
  queueEntry.expoParams  = *expoParams;
  LOGG_PRINT(LOG_DEBUG_E,NULL,"Adding to the delayed gain queue \n");
  OS_LYRG_sendMsg(&entry->AR2020_gainQ, (UINT8 *)&queueEntry, sizeof(queueEntry));
}
int METADATA_ISP_UPDATER_getDelayedGainQueueSize(METADATA_UPDATER_channelHandleT *entry )
{
  if(!entry)
    return 0;

  if((entry->mapentry->axiReader!= ISP0_READER) && (entry->mapentry->axiReader!= ISP1_READER))
    return 0;
  return OS_LYRG_getCurrNumMsg(&entry->AR2020_gainQ);
}

uint32_t Convert_Time_To_FSG_Pulses(float time_s)
{
    double counts =  time_s * 333333333; /*Work out how long the exposure is in */
   return (uint32_t )counts;
}
ERRG_codeE METADATA_ISP_UPDATER_updateTriggering(METADATA_UPDATER_channelHandleT *entry,float exposure )
{
  SENSOR_SYNC_Pulse_Config trigger = SENSOR_SYNC_Pulse_Config_init_default;
  trigger.has_channelConfiguration = true;
  float max_value = 1.0/91.0; /*The max value is 1/FPS*/
  float trigger_point_f = max_value - exposure;
  uint32_t trigger_point = Convert_Time_To_FSG_Pulses(trigger_point_f);
  LOGG_PRINT(LOG_DEBUG_E,NULL,"Max value: %f, Trigger point: %f \n",max_value,trigger_point_f);
  bool master= HELSINKI_getMaster();
  // Set the trigger point to be (1/FPS) - exposure_time
  if(master)
    trigger.channelConfiguration.counterSource.counter_source = 2;
  else
    trigger.channelConfiguration.counterSource.counter_source = 1;

  trigger.channelConfiguration.has_counterSource = false;
   if(master)
      trigger.channelConfiguration.pulseChannel = SENSOR_SYNC_FSG_Pulse_Channel_FSG_PULSE_2;
   else
      trigger.channelConfiguration.pulseChannel = SENSOR_SYNC_FSG_Pulse_Channel_FSG_PULSE_1;

  trigger.channelConfiguration.pulseEnable = true;
  trigger.channelConfiguration.pulseOffset = trigger_point;
  trigger.channelConfiguration.pulseRepeatPeriod = 1;
  trigger.channelConfiguration.pulsePolarity = SENSOR_SYNC_Pulse_Polarity_Active_High;
  trigger.channelConfiguration.pulseWidth = 255;
  LOGG_PRINT(LOG_DEBUG_E,NULL,"Setting the trigger point to be %lu for exposure %.3f \n",trigger_point,exposure);
  SENSOR_SYNC_updateChannelConfiguration(&trigger);
}

ERRG_codeE METADATA_ISP_UPDATER_updateExpoParameters(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex,const METADATA_SENSOR_UPDATER_interruptNotification *interruptTiming, 
  INU_metadata_State *incomingSensorState, INU_DEFSG_sensorContextE  context,   METADATA_UPDATE_registeredSensor *sensor)
{
    ERRG_codeE ret = INU_METADATA__RET_SUCCESS;
    const INU_metadata_ISP_ExpoParams const *expoParameters = &incomingSensorState->sensor_metadata.isp_expo;
    LOGG_PRINT(LOG_DEBUG_E,NULL,"ISP Expo parameter updater, %lu, %lu \n", expoParameters->ispAE.metadataEnableAE, expoParameters->has_ispAE);
    inu_isp_channel__exposureParamT ispExpoParameters = {
                                                        .analogGain=expoParameters->analogGain,
                                                        .integrationTime=expoParameters->integrationTime,
                                                        .ispGain=expoParameters->ispGain,
                                                        .metadataEnableAE=expoParameters->ispAE.metadataEnableAE,
                                                        .hasmetadataEnableAE = expoParameters->has_ispAE,
                                                        .has_ae_cb = false,   /*Do not set the AE callback*/
                                                        .ae_cb = NULL,
                                                        };
    if(expoParameters->has_ispAE)
      HELSINKI_ISP_SYNCHRONIZER_set_AEUartSyncState(!expoParameters->ispAE.metadataEnableAE);


{
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Updating the exposure parameters to be %.3f,%.3f,%.3f \n",expoParameters->analogGain,expoParameters->integrationTime,expoParameters->ispGain);
    /*Write the expousre parameters*/
    UINT32 isp_Expo_params = 1E6* expoParameters->integrationTime;
    float dummy = 0;
    /*The ISP isn't deterministic about when it performs its I2C writes, we need to do the I2C writes ourselves and disable the I2C exposure/gain writes*/
    ret = ISP_MNGRG_setExposureParams(entry->mapentry->axiReader,&ispExpoParameters);
    if(ERRG_FAILED(ret))
        LOGG_PRINT(LOG_ERROR_E,NULL,"Couldn't update the ISP WB gains\n");
    /*For the AR2020 it takes two frames for the Expo and Gain to be updated*/
    /* But it takes 1 frame for the AGAIN to be updated, so we will do the I2C write on the next frame */
    uint32_t frameID = 0;
    METADATA_UPDATER_getFullFrameIDWithOverFlowCounter(entry->mapentry,&frameID);
    if(entry->mapentry->axiReader == ISP1_READER)
    {
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Expo command \n");
        //printf("Frame ID: %lu, Exposure %lu \n", frameID, (uint32_t)isp_Expo_params);
        #ifdef DELAYED_GAIN_UPDATE
        METADATA_ISP_UPDATER_AddToDelayedGainQueue(entry,(INU_metadata_ISP_ExpoParams *)expoParameters,sensorIndex); 
        entry->exp_counter = 2;
        #else
        float dummy = 0;
        ret = SENSORS_MNGRG_setSensorGain(sensor->sensorInfo,(float *)&expoParameters->analogGain,&dummy,INU_SENSOR__CONTEXT_A);
        #endif
        ret = SENSORS_MNGRG_setSensorExpTime(sensor->sensorInfo,&isp_Expo_params,context);
       
    }

    #ifndef DELAYED_GAIN_UPDATE
        METADATA_ISP_UPDATER_updateExpoAfterNFrames(entry,sensorIndex,METADATA_SENSOR_UPDATER_N_1,
        &incomingSensorState->sensor_metadata.isp_expo,true,false,true,true); /*Only other update expo and other parameters, do not update AGain and DGain*/
    #endif
    if(ERRG_FAILED(ret))
        LOGG_PRINT(LOG_ERROR_E,NULL,"Couldn't write the exposure and gain\n");
    const uint32_t AR2020_EXPO_UPDATE_DELAY  = METADATA_SENSOR_UPDATER_N_2; /*AR2020 takes 2 frames to update exposure*/
    METADATA_ISP_UPDATER_updateExpoAfterNFrames(entry,sensorIndex,AR2020_EXPO_UPDATE_DELAY,
        &incomingSensorState->sensor_metadata.isp_expo,false,true,true,true); /*Only other update expo and other parameters, do not update AGain and DGain*/
    // Update the trigger point
    #ifdef DELAYED_GAIN_UPDATE
    METADATA_ISP_UPDATER_updateTriggering(entry,expoParameters->integrationTime);
    #endif
}
    return ret;
}
/**
 * @brief For the Helsinki project, we have to read Wb/Expo from the slave downscaled ISP( ISP0) and then update the metadata with this value
 * 
 * @param entry METADATA_UPDATER_channelHandleT to store the metadata 
 * @param sensorIndex Sensor Index (0 or 1)
*/
ERRG_codeE METADATA_ISP_UPDATER_live_WB_Expo_metadata(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex)
{
  /*For the Slave ISP downscaled stream, we need to read out the ISP colour correction and exposure values
  Note:
  I looked at the ISP code and the "set" functions just update mutex protected variables, so doing a "get" after a "set" should be fine - William Bright
  */

  METADATA_UPDATE_registeredSensor *sensor = &entry->sensors[sensorIndex];
  if (sensor == NULL)
    return INU_METADATA__ERR_METADATA_INVALID_SENSOR_HANDLE;
  
  if (sensor->registered == false)
    return INU_METADATA__ERR_METADATA_NO_SENSOR_UPDATE_FOR_THIS_SENSOR;
  if (entry->sensors[sensorIndex].sensorInfo == NULL)
    return INU_METADATA__ERR_METADATA_INVALID_SENSOR_HANDLE;
  if(!HELSINKI_getEnableConstantMetadataUpdates(entry->mapentry->axiReader))
      return INU_METADATA__ERR_METADATA_NO_SENSOR_UPDATE_FOR_THIS_SENSOR;
  const int isp_expo_messages = OS_LYRG_getCurrNumMsg(&entry->isp_expoQ);
  const int isp_wb_messages = OS_LYRG_getCurrNumMsg(&entry->isp_wbQ);
  if(entry->softwareInjectCallCount < 100)
    /*TODO: FIX!!
      Reading ISP values before frame ID 100 can cause crashes!*/
      return INU_METADATA__ERR_METADATA_NO_SENSOR_UPDATE_FOR_THIS_SENSOR;
  if(( isp_expo_messages == 0) && (isp_wb_messages== 0))
    return INU_METADATA__ERR_METADATA_NO_SENSOR_UPDATE_FOR_THIS_SENSOR;

  UINT64 startTime =0;
  UINT64 endTime = 0;
  OS_LYRG_getUsecTime(&startTime);
  /*Outgoing values*/
  INU_metadata_ISP_WBPameters outWBParams;
  INU_metadata_ISP_ExpoParams outExpoParams;
  ERRG_codeE ret;
  /*Notification*/
  METADATA_ISP_Wb_Queue_Entry wbNotification;
  METADATA_ISP_Expo_Queue_Entry expoNotification;
  METADATA_UPDATER_LockMutex(entry); 
  int status = 0;
  int size = sizeof(METADATA_ISP_Wb_Queue_Entry);
  if(isp_wb_messages > 0)
  {
  status = OS_LYRG_recvMsg(&entry->isp_wbQ, (UINT8 *)&wbNotification, &size, 0);
  /*Here we will read out the ISP values*/
    while ((status == SUCCESS_E) && (size == sizeof(METADATA_ISP_Wb_Queue_Entry)))
    {
        status = OS_LYRG_recvMsg(&entry->isp_wbQ, (UINT8 *)&wbNotification, &size, 0);
        if(OS_LYRG_getCurrNumMsg(&entry->isp_wbQ) == 0)
            break;
    }
    METADATA_ISP_UPDATER_copyWBParams(&wbNotification.value,&outWBParams);
    ret = METADATA_ISP_UPDATER_updateWBGainsAfterNFrames(entry,sensorIndex,METADATA_SENSOR_UPDATER_N_1,
            &outWBParams);
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to update WB parameters \n");
        return ret;
    }
  }
  size   = sizeof(expoNotification);
  if(isp_expo_messages > 0)
  {
    status = OS_LYRG_recvMsg(&entry->isp_expoQ, (UINT8 *)&expoNotification, &size, 0);
  /*Here we will read out the ISP values*/
    while ((status == SUCCESS_E) && (size == sizeof(METADATA_ISP_Expo_Queue_Entry)))
    {
        status = OS_LYRG_recvMsg(&entry->isp_expoQ, (UINT8 *)&expoNotification, &size, 0);
        if(OS_LYRG_getCurrNumMsg(&entry->isp_expoQ) == 0)
            break;
    }
    METADATA_ISP_UPDATER_copyExpoParams(&expoNotification.value,&outExpoParams);
    ret = METADATA_ISP_UPDATER_updateExpoAfterNFrames(entry,sensorIndex,METADATA_SENSOR_UPDATER_N_1,
            &outExpoParams,true,true,true,true);

    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to update Expo parameters \n");
        return ret;
    }
  }
  METADATA_UPDATER_UnlockMutex(entry); 
  OS_LYRG_getUsecTime(&endTime);
  //printf("%llu \n", endTime-startTime);
  LOGG_PRINT(LOG_DEBUG_E,NULL,"Finished updating the isp_wb and exposure with %lu,%lu messages \n", isp_expo_messages, isp_wb_messages);

  return  INU_METADATA__RET_SUCCESS;;

}
static int METADATA_ISP_UPDATER_AWB_Callback(struct Vsi3AMetaWb * wb_ptr)
{
   LOGG_PRINT(LOG_DEBUG_E,NULL,"AWB has finished with %.3f CC0 \n",wb_ptr->CCM[0]);

  METADATA_UPDATER_channelHandleT * entry = NULL;
  ERRG_codeE ret =  METADATA_getChannelHandleByReaderID(&entry,ISP0_READER);
  if(ERRG_SUCCEEDED(ret))
  {
    METADATA_ISP_Wb_Queue_Entry queueEntry;
    memset(&queueEntry,0,sizeof(queueEntry));
    OS_LYRG_getTimeNsec(&queueEntry.notificationInfo.timestamp);
    queueEntry.value = *wb_ptr;
    #if 0 
    if(OS_LYRG_sendMsg(&entry->isp_wbQ,(UINT8 *)&queueEntry,sizeof(queueEntry))!=0)
      LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to write to AWB Q \n");

    #endif

    return 0;
  }
  return (int) ret;
}

static int METADATA_ISP_UPDATER_AE_Callback(struct Vsi3AMetaExp *expo_ptr)
{
  LOGG_PRINT(LOG_DEBUG_E,NULL,"AE has finished with %.3f integration time \n", expo_ptr->expsoureTime);

  METADATA_UPDATER_channelHandleT * entry = NULL;
  ERRG_codeE ret =  METADATA_getChannelHandleByReaderID(&entry,ISP0_READER);
  if(ERRG_SUCCEEDED(ret))
  {
    METADATA_ISP_Expo_Queue_Entry queueEntry;
    memset(&queueEntry,0,sizeof(queueEntry));
    OS_LYRG_getTimeNsec(&queueEntry.notificationInfo.timestamp);
    queueEntry.value = *expo_ptr;
    #if 0 
    if(OS_LYRG_sendMsg(&entry->isp_expoQ,(UINT8 *)&queueEntry,sizeof(queueEntry))!=0)
      LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to write to expo Q \n");
    #endif
    return 0;
  }
  return (int) ret;
}

int slave_downscaled_AWB_callback(void * ptr)
{
  struct Vsi3AMetaWb * wb_ptr = (struct Vsi3AMetaWb *) ptr;
  if(!wb_ptr)
  {
    LOGG_PRINT(LOG_ERROR_E,NULL,"Null wb pointer detected \n");
    return -1;
  }
  int ret = METADATA_ISP_UPDATER_AWB_Callback(wb_ptr);
  //ret = HELSINKI_ISP_SYNCHRONIZER_AWB_callback(wb_ptr);

 return ret;
}
int slave_downscaled_AE_callback(void * ptr)
{

  struct Vsi3AMetaExp * expo_ptr = (struct Vsi3AMetaExp *) ptr;
  if(!expo_ptr)
  {
    LOGG_PRINT(LOG_ERROR_E,NULL,"Null expo pointer detected \n");
    return -1;
  }
  int ret = METADATA_ISP_UPDATER_AE_Callback(expo_ptr);
  //ret = HELSINKI_ISP_SYNCHRONIZER_AE_callback(expo_ptr);
  return ret;
}
/**
 * @brief For the Helsinki project, we have to set the AE and AWB done callbacks
 * 
 * @param entry METADATA_UPDATER_channelHandleT to store the metadata 
 * @param sensorIndex Sensor Index (0 or 1)
*/
ERRG_codeE METADATA_ISP_UPDATER_set_isp_expo_callback(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex)
{
  /*For the Slave ISP downscaled stream, we need to read out the ISP colour correction and exposure values
  Note:
  I looked at the ISP code and the "set" functions just update mutex protected variables, so doing a "get" after a "set" should be fine - William Bright
  */

  METADATA_UPDATE_registeredSensor *sensor = &entry->sensors[sensorIndex];
  if (sensor == NULL)
    return INU_METADATA__ERR_METADATA_INVALID_SENSOR_HANDLE;
  
  if (sensor->registered == false)
    return INU_METADATA__ERR_METADATA_NO_SENSOR_UPDATE_FOR_THIS_SENSOR;
  if (entry->sensors[sensorIndex].sensorInfo == NULL)
    return INU_METADATA__ERR_METADATA_INVALID_SENSOR_HANDLE;
  if(!HELSINKI_getEnableConstantMetadataUpdates(entry->mapentry->axiReader))
      return INU_METADATA__ERR_METADATA_NO_SENSOR_UPDATE_FOR_THIS_SENSOR;
  if(entry->softwareInjectCallCount < 100)
    /*TODO: FIX!!
      Reading ISP values before frame ID 100 can cause crashes!*/
    return INU_METADATA__ERR_METADATA_NO_SENSOR_UPDATE_FOR_THIS_SENSOR;
  if(entry->has_set_isp_callbacks)
      /* We only need to set the callback once*/
      return INU_METADATA__ERR_METADATA_NO_SENSOR_UPDATE_FOR_THIS_SENSOR;
  inu_isp_channel__ispWbParamT wbParams;
  inu_isp_channel__exposureParamT expoParams;
  ERRG_codeE ret;
  METADATA_UPDATER_LockMutex(entry); 
  /*Set the AWB callback*/
  ret = ISP_MNGRG_getWbParams(ISP0_READER,&wbParams);
  wbParams.awb_cb = slave_downscaled_AWB_callback;
  wbParams.has_awb_cb = true;
  ret = ISP_MNGRG_setWbParams(ISP0_READER,&wbParams);
  /*Set the AE callback*/
  ret = ISP_MNGRG_getExposureParams(ISP0_READER,&expoParams);
  expoParams.ae_cb = slave_downscaled_AE_callback;
  expoParams.has_ae_cb = true;
  ret = ISP_MNGRG_setExposureParams(ISP0_READER,&expoParams);
  /*Callbacks have been set so we set the has_set flag as true*/
  entry->has_set_isp_callbacks = true;
  LOGG_PRINT(LOG_DEBUG_E,NULL,"Finished setting callbacks \n");
  METADATA_UPDATER_UnlockMutex(entry);  
  return  INU_METADATA__RET_SUCCESS;

}