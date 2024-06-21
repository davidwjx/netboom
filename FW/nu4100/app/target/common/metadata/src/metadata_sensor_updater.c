#include "metadata_sensor_updater.h"
#include "helsinki.h"
#include "metadata_updater.h"
#include "gen_sensor_drv.h"
#include "ppe_mngr.h"
#include "focus_ctrl.h"
#include "inu_sensor_api.h"
#include <signal.h>
#include "metadata_isp_updater.h"
#define ENABLE_MULTI_DEQUEUE
// Flag that keeps the status of pending sensor update messages.
// This also holds a repeat flag at the BIT index 8, the Metadata update is required to be done
// twice because one of the four metadata buffers is held active by the DMA to fill the frame buffer
// with prepared metadata. Because one of the buffer is not updated the first time, we need second cycle of 
// metadata buffer updatioin so that all buffers are in sync.
static volatile sig_atomic_t pendingSensorUpdates = 0;
/**
 * @brief Converts a protobuf defined sensor context message into INU_DEFSG_sensorContextE
 *
 * @param context context to convert
 * @return INU_DEFSG_sensorContextE Returns context as an inuitive type
 */
INU_DEFSG_sensorContextE METADATA_SENSOR_UPDATER_convertINU_metadata_Context(INU_metadata_Context context)
{
  if (context == INU_metadata_Context_Context_A)
  {
    return INU_DEFSG_SENSOR_CONTEX_A;
  }
  else
  {
    return INU_DEFSG_SENSOR_CONTEX_B;
  }
}
/**
 * @brief
 *
 * @param channelHandle
 * @param sensorIndex
 * @return ERRG_codeE
 */
static ERRG_codeE METADATA_SENSOR_UPDATER_updateConstFieldsMetadata(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex, METADATA_SENSOR_UPDATER_frameTimingT frameIndex)
{
  /*Prepare the metadata ready for the SOF interrupt */
  const UINT32 index = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
  INU_metadata_State *outgoingState = &entry->outgoingSensorUpdates[index].protobuf_packet.state[sensorIndex];
  /*HELSINKI_getHardwareType() is helsinki specific code */
  outgoingState->sender = HELSINKI_getHardwareType();
  /*Send the SensorID, name and side with this message*/
  outgoingState->sensor = entry->mapentry->sensors[sensorIndex].name;
  outgoingState->has_sensor = true;
  outgoingState->has_snsr_update_timing = true;
  OS_LYRG_getTimeNsec(&outgoingState->timestamp_ns.nanoseconds);
  outgoingState->has_timestamp_ns = true;
  return INU_METADATA__RET_SUCCESS;
}
/**
 * @brief Updates a timing information entry
 *
 * @param mapEntry METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap pointer to read the frame ID from
 * @param timingInformation Pointer to store timing information
 * @param hasFlag Nanopb has flag which will be set to true
 * @return ERRG_codeE Returns an error code
 */
 ERRG_codeE METADATA_SENSOR_UPATER_updateTimingInformation( METADATA_UPDATER_channelHandleT *entry, INU_metadata_Debug_Timing_Information *timingInformation, bool *hasFlag)
{
  if (timingInformation != NULL)
  {
    if((entry->mapentry->axiReader != ISP0_READER) && (entry->mapentry->axiReader != ISP1_READER))
      METADATA_UPDATER_getFrameID(entry->mapentry, &timingInformation->frameID.frameID);
    else
      METADATA_UPDATER_getFullFrameIDWithOverFlowCounter(entry->mapentry,&timingInformation->frameID.frameID);

    OS_LYRG_getTimeNsec(&timingInformation->timestamp.nanoseconds);
    timingInformation->has_timestamp = true;
    timingInformation->has_frameID = true;
    *hasFlag = true;
    return INU_METADATA__RET_SUCCESS;
  }
  return INU_METADATA__ERR_INVALID_ARGS;
}
/**
 * @brief Updates the sync debug information
 * 
 * @param src - Source to get the command ID from
 * @param dst - Destination to store the command ID 
*/
ERRG_codeE METADATA_SENSOR_UPATER_updateSyncInformation(INU_metadata_Command_Sync *src,INU_metadata_Command_Sync * dst, bool * has_flag)
{
  dst->cmd_id = src->cmd_id;
  dst->frameID = src->frameID;
  dst->slu_number = src->slu_number;
  dst->wait_for_slu_SOF_Interrupt = src->wait_for_slu_SOF_Interrupt;
  *has_flag = true;
  return INU_METADATA__RET_SUCCESS;
}

/**
 * @brief Updates the Deque timing information for entry->sensors[sensorIndex]
 *
 * @param entry METADATA_UPDATER_channelHandleT entry that will be updated
 * @param sensorIndex Sensor index for this sensor
 * @param frameIndex Frame buffer index [0->NUM_BUFFERS-1]
 * @return ERRG_codeE Returns an error code
 */
static ERRG_codeE METADATA_SENSOR_UPDATER_updateDequeTimingInfo(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex, METADATA_SENSOR_UPDATER_frameTimingT frameIndex)
{
  if (frameIndex != 0)
  {
    return INU_METADATA__ERR_INVALID_ARGS;
  }

  const UINT32 index = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
  const UINT32 nextBuffer = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
  METADATA_UPDATE_registeredSensor *sensor = &entry->sensors[sensorIndex];
    for (int i = index; i < (MAX_NUM_BUFFERS_LOOPS); i++)
  {
    const UINT32 INDEX = (nextBuffer + i) % MAX_NUM_BUFFERS_LOOPS; /*Calculate the buffer index */
    INU_metadata_State *outgoingState = &entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex];
    ERRG_codeE ret = METADATA_SENSOR_UPATER_updateTimingInformation(entry, &outgoingState->snsr_update_timing.sensor_update_dequed_timing, &outgoingState->snsr_update_timing.has_sensor_update_dequed_timing);
  }

  return INU_METADATA__RET_SUCCESS;
}
/**
 * @brief Copies the message receiving timing information from rxTimingInformation to the metadata for the next frame
 * 
 *
 * @param entry METADATA_UPDATER_channelHandleT entry that will be updated
 * @param sensorIndex Sensor index for this sensor
 * @param rxTimingInformation Timing information to copy from
 * @param frameIndex Frame buffer index [0->NUM_BUFFERS-1]
 * @return ERRG_codeE Returns an error code
 */
static ERRG_codeE METADATA_SENSOR_UPDATER_copyRxTimingInfo(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex,INU_metadata_Debug_Timing_Information *rxTimingInformation, METADATA_SENSOR_UPDATER_frameTimingT frameIndex)
{
  const UINT32 index = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
  const UINT32 nextBuffer = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
    for (int i = index; i < (MAX_NUM_BUFFERS_LOOPS); i++)
  {
    const UINT32 INDEX = (nextBuffer + i) % MAX_NUM_BUFFERS_LOOPS; /*Calculate the buffer index */
    INU_metadata_State *outgoingState = &entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex];
    memcpy(&outgoingState->snsr_update_timing.sensor_update_rx_timing,rxTimingInformation,sizeof(INU_metadata_Debug_Timing_Information));
    outgoingState->snsr_update_timing.has_sensor_update_rx_timing = true;
  }
  return INU_METADATA__RET_SUCCESS;
}
/**
 * @brief Copies the message Sync debug information
 * 
 *
 * @param entry METADATA_UPDATER_channelHandleT entry that will be updated
 * @param sensorIndex Sensor index for this sensor
 * @param synced Debug information for synchronization purposes
 * @param frameIndex Frame buffer index [0->NUM_BUFFERS-1]
 * @return ERRG_codeE Returns an error code
 */
static ERRG_codeE METADATA_SENSOR_UPDATER_copySyncedInfo(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex,INU_metadata_Command_Sync *synced, METADATA_SENSOR_UPDATER_frameTimingT frameIndex)
{
  const UINT32 index = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
  const UINT32 nextBuffer = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
    for (int i = index; i < (MAX_NUM_BUFFERS_LOOPS); i++)
  {
    const UINT32 INDEX = (nextBuffer + i) % MAX_NUM_BUFFERS_LOOPS; /*Calculate the buffer index */
    INU_metadata_State *outgoingState = &entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex];
    memcpy(&outgoingState->sensor_metadata.isp_expo.uart_sync ,synced,sizeof(INU_metadata_Command_Sync));
    outgoingState->sensor_metadata.isp_expo.has_uart_sync = true;
  }
  return INU_METADATA__RET_SUCCESS;
}
/**
 * @brief Updates the SOF interrupt timing for the next frame
 * 
 *
 * @param entry METADATA_UPDATER_channelHandleT entry that will be updated
 * @param sensorIndex Sensor index for this sensor
 * @param interruptTiming Timing information for the interrupt
 * @param frameIndex Frame buffer index [0->NUM_BUFFERS-1]
 * @return ERRG_codeE Returns an error code
 */
static ERRG_codeE METADATA_SENSOR_UPDATER_updateSOFInterruptTimingInfo(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex,const METADATA_SENSOR_UPDATER_interruptNotification *interruptTiming, METADATA_SENSOR_UPDATER_frameTimingT frameIndex)
{
  const UINT32 index = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
  const UINT32 nextBuffer = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
  ERRG_codeE ret;
  for (int i = index; i < (MAX_NUM_BUFFERS_LOOPS); i++)
  {
    const UINT32 INDEX = (nextBuffer + i) % MAX_NUM_BUFFERS_LOOPS; /*Calculate the buffer index */
    INU_metadata_State *outgoingState = &entry->outgoingSensorUpdates[i].protobuf_packet.state[sensorIndex];
    ret = METADATA_SENSOR_UPATER_updateTimingInformation(entry, &outgoingState->snsr_update_timing.sensor_updateSOFInterrupt, &outgoingState->snsr_update_timing.has_sensor_updateSOFInterrupt);

    outgoingState->snsr_update_timing.sensor_updateSOFInterrupt.timestamp.nanoseconds = interruptTiming->timestamp;
    outgoingState->snsr_update_timing.sensor_updateSOFInterrupt.frameID.frameID = interruptTiming->frameID;
  }
   return ret;
}
/**
 * @brief Updates the I2C write complete timing information for entry->sensors[sensorIndex]
 *
 * @param entry METADATA_UPDATER_channelHandleT entry that will be updated
 * @param sensorIndex Sensor index for this sensor
 * @param frameIndex Frame buffer index [0->NUM_BUFFERS-1]
 * @return ERRG_codeE Returns an error code
 */
static ERRG_codeE METADATA_SENSOR_UPDATER_updatei2cWritesCompleteTimingInfo(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex, UINT8 frameIndex)
{
  if (frameIndex != 0)
  {
    return INU_METADATA__ERR_INVALID_ARGS;
  }
  ERRG_codeE ret;
  LOGG_PRINT(LOG_DEBUG_E, NULL, "Frame buffer index is %lu \n", index);
  METADATA_UPDATE_registeredSensor *sensor = &entry->sensors[sensorIndex];
  const INT32 startIndex = METADATA_SENSOR_UPDATER_calculateFramesAfterStartIndex(frameIndex);
  const UINT32 nextBuffer = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
  for (int i = startIndex; i < (MAX_NUM_BUFFERS_LOOPS); i++)
  {
    const UINT32 INDEX = (nextBuffer + i) % MAX_NUM_BUFFERS_LOOPS; /*Calculate the buffer index */
    INU_metadata_State *outgoingState = &entry->outgoingSensorUpdates[i].protobuf_packet.state[sensorIndex];
    ret = METADATA_SENSOR_UPATER_updateTimingInformation(entry, &outgoingState->snsr_update_timing.sensor_update_i2cWritesComplete, &outgoingState->snsr_update_timing.has_sensor_update_i2cWritesComplete);
  }
  return ret;

}
/**
 * @brief Updates the ROI information for entry->sensors[sensorIndex]
 *
 * @param entry METADATA_UPDATER_channelHandleT entry that will be updated
 * @param sensorIndex Sensor index for this sensor
 * @param frameIndex Frame buffer index [0->NUM_BUFFERS-1]
 * @param xStart X Start position
 * @param yStart Y Start position
 * @return ERRG_codeE Returns an error code
 */
 ERRG_codeE METADATA_SENSOR_UPDATER_updateROI(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex, UINT8 frameIndex, UINT32 xStart, UINT32 yStart)
{
  if (frameIndex != 0)
  {
    return INU_METADATA__ERR_INVALID_ARGS;
  }
  const INT32 startIndex = METADATA_SENSOR_UPDATER_calculateFramesAfterStartIndex(frameIndex);
  METADATA_UPDATE_registeredSensor *sensor = &entry->sensors[sensorIndex];

    const UINT32 nextBuffer = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
    for (int i = startIndex; i < (MAX_NUM_BUFFERS_LOOPS); i++)
    {
    const UINT32 INDEX = (nextBuffer + i) % MAX_NUM_BUFFERS_LOOPS; /*Calculate the buffer index */
    INU_metadata_State *outgoingState = &entry->outgoingSensorUpdates[i].protobuf_packet.state[sensorIndex];
    outgoingState->sensor_metadata.has_roi = true;
    outgoingState->sensor_metadata.roi.x_start = xStart;
    outgoingState->sensor_metadata.roi.y_start = yStart;
    outgoingState->has_sensor_metadata = true;
    }
    return INU_METADATA__RET_SUCCESS;
}
/**
 * @brief Updates the ROI information for entry->sensors[sensorIndex]
 *
 * @param entry METADATA_UPDATER_channelHandleT entry that will be updated
 * @param sensorIndex Sensor index for this sensor
 * @param frameIndex Frame buffer index [0->NUM_BUFFERS-1]
 * @param xStart X Start position
 * @param yStart Y Start position
 * @return ERRG_codeE Returns an error code
 */
 ERRG_codeE METADATA_SENSOR_UPDATER_updateROIAfterNFrames(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex, UINT8 frameIndex, UINT32 xStart, UINT32 yStart)
{
  if (frameIndex != 0)
  {
    return INU_METADATA__ERR_INVALID_ARGS;
  }
  const INT32 startIndex = METADATA_SENSOR_UPDATER_calculateFramesAfterStartIndex(frameIndex);
  METADATA_UPDATE_registeredSensor *sensor = &entry->sensors[sensorIndex];

    const UINT32 nextBuffer = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
    for (int i = startIndex; i < (MAX_NUM_BUFFERS_LOOPS); i++)
    {
    const UINT32 INDEX = (nextBuffer + i) % MAX_NUM_BUFFERS_LOOPS; /*Calculate the buffer index */
    INU_metadata_State *outgoingState = &entry->outgoingSensorUpdates[i].protobuf_packet.state[sensorIndex];
    outgoingState->sensor_metadata.has_roi = true;
    outgoingState->sensor_metadata.roi.x_start = xStart;
    outgoingState->sensor_metadata.roi.y_start = yStart;
    outgoingState->has_sensor_metadata = true;
    }
    return INU_METADATA__RET_SUCCESS;
}
/**
 * @brief Updates the Focus information for entry->sensors[sensorIndex]
 *
 * @param entry METADATA_UPDATER_channelHandleT entry that will be updated
 * @param sensorIndex Sensor index for this sensor
 * @param frameIndex Frame buffer index [0->NUM_BUFFERS-1]
 * @param focus Focus position
 * @return ERRG_codeE Returns an error code
 */
 ERRG_codeE METADATA_SENSOR_UPDATER_updateFocusAfterNFrames(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex, UINT8 frameIndex, INT32 focus)
{
  if (frameIndex != 0)
  {
    return INU_METADATA__ERR_INVALID_ARGS;
  }
  const INT32 startIndex = METADATA_SENSOR_UPDATER_calculateFramesAfterStartIndex(frameIndex);
  METADATA_UPDATE_registeredSensor *sensor = &entry->sensors[sensorIndex];

    const UINT32 nextBuffer = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
    for (int i = startIndex; i < (MAX_NUM_BUFFERS_LOOPS); i++)
    {
      const UINT32 INDEX = (nextBuffer + i) % MAX_NUM_BUFFERS_LOOPS; /*Calculate the buffer index */
      INU_metadata_State *outgoingState = &entry->outgoingSensorUpdates[i].protobuf_packet.state[sensorIndex];
      outgoingState->sensor_metadata.has_AFFocusPosition = true;
      outgoingState->sensor_metadata.AFFocusPosition.AFFocusPosition = focus;
      outgoingState->has_sensor_metadata = true;
    }
    return INU_METADATA__RET_SUCCESS;
}

/**
 * @brief Converts METADATA_SENSOR_UPDATER_frameTimingT into a frame buffer index
 *
 * @param frameIndex METADATA_SENSOR_UPDATER_frameTimingT to convert
 * @return INT32 Returns an INT32 that is used to calculate which frame buffer to update
 */
INT32 METADATA_SENSOR_UPDATER_calculateFramesAfterStartIndex(METADATA_SENSOR_UPDATER_frameTimingT frameIndex)
{
  switch(frameIndex)
  {
    case METADATA_SENSOR_UPDATER_N_1:
      return 0;
    case METADATA_SENSOR_UPDATER_N_2:
      return 1;
    case METADATA_SENSOR_UPDATER_N_3:
      return 2;
  }
}
/**
 * @brief Updates the Sensor metadata in N frames, 
 * there's unlikely to be a need to use this function outside of Helsinki project
 *
 * @param entry METADATA_UPDATER_channelHandleT to store the metadata with the new exposure value in
 * @param sensorIndex Sensor Index (0 or 1)
 * @param N  N (when to store the new exposure in)
 * @param exposure Exposure value
 * @param side Sensor side 
 * @param hwType Hardware type
 * @param sensorID sensor ID
 * @param writerOperateContext True if called from a writer operate context, false it's called from a SOF/EOF context.
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE METADATA_SENSOR_UPDATER_updateSensorInfoAndHWType(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex, const METADATA_SENSOR_UPDATER_frameTimingT N, 
const INU_metadata_Sensor sensor, INU_metadata_HardwareType hwType, const bool writerOperateContext)
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
    const UINT32 nextBuffer = METADATA_UPDATER_calculateMetadataBuffer(entry,writerOperateContext);
    for (int i = startIndex; i < (MAX_NUM_BUFFERS_LOOPS); i++)
    {
      const UINT32 INDEX = (nextBuffer + i) % MAX_NUM_BUFFERS_LOOPS; /*Calculate the buffer index */
      /*Special case for the CVA*/
      entry->outgoingSensorUpdates[INDEX].protobuf_packet.state_count = 0;
      entry->outgoingSensorUpdates[INDEX].protobuf_packet.sensors_count = 1;
      entry->outgoingSensorUpdates[INDEX].protobuf_packet.sensors[0] = sensor;
    }
  }
  return INU_METADATA__RET_SUCCESS;
}
/**
 * @brief Updates the exposure metadata in frame buffers after NFrames
 *
 * @param entry METADATA_UPDATER_channelHandleT to store the metadata with the new exposure value in
 * @param sensorIndex Sensor Index (0 or 1)
 * @param N  N (when to store the new exposure in)
 * @param exposure Exposure value
 * @return ERRG_codeE Returns an error code
 */
static ERRG_codeE METADATA_SENSOR_UPDATER_updateExposureAfterNFrames(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex, const METADATA_SENSOR_UPDATER_frameTimingT N, INU_metadata_State *incomingSensorState)
{
  UINT32 exposure = incomingSensorState->sensor_metadata.exposure.exposure;
  INU_metadata_SensorType sensorType = incomingSensorState->sensor.sensorType;
  INU_metadata_Channel_Side side = incomingSensorState->sensor.side;
  UINT32 sensorIndex_2;
  const INT32 startIndex = METADATA_SENSOR_UPDATER_calculateFramesAfterStartIndex(N);
  if (sensorIndex >= 2)
  {
    return INU_METADATA__ERR_INVALID_ARGS;
  }
  if (entry == NULL)
  {
    return INU_METADATA__ERR_INVALID_ARGS;
  }

  if(sensorIndex == 0)
    sensorIndex_2 = 1;
  else if(sensorIndex == 1)
    sensorIndex_2 = 0;
  
  if (startIndex >= 0)
  {
    const UINT32 nextBuffer = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
    for (int i = startIndex; i < (MAX_NUM_BUFFERS_LOOPS); i++)
    {
      const UINT32 INDEX = (nextBuffer + i) % MAX_NUM_BUFFERS_LOOPS; /*Calculate the buffer index */
      entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.has_exposure = true;
      entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.exposure.exposure = exposure;
      entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].has_sensor_metadata = true;

      if(HELSINKI_getGroupedExposureUpdatesRequired(sensorType) && !HELSINKI_ignoreExposureUpdate(sensorType, side))
      {
          entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex_2].sensor_metadata.has_exposure = true;
          entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex_2].sensor_metadata.exposure.exposure = exposure;
          entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex_2].has_sensor_metadata = true;
      }     
    }
  }
  return INU_METADATA__RET_SUCCESS;
}
/**
 * @brief Updates the Gain metadata in frame buffers after NFrames
 *
 * @param entry METADATA_UPDATER_channelHandleT to store the metadata with the new exposure value in
 * @param sensorIndex Sensor Index (0 or 1)
 * @param N  N (when to store the new exposure in)
 * @param analogueGain Analogue gain
 * @param digitalGain  Digital gain
 * @return ERRG_codeE Returns an error code
 */
static ERRG_codeE METADATA_SENSOR_UPDATER_updateGainsAfterNFrames(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex, const METADATA_SENSOR_UPDATER_frameTimingT N, UINT32 analogueGain, UINT32 digitalGain)
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
    const UINT32 nextBuffer = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
    for (int i = startIndex; i < (MAX_NUM_BUFFERS_LOOPS); i++)
    {
      const UINT32 INDEX = (nextBuffer + i) % MAX_NUM_BUFFERS_LOOPS; /*Calculate the buffer index */
      entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.has_gain = true;
      entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.gain.analogue_gain = analogueGain;
      entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].sensor_metadata.gain.digital_gain = digitalGain;
      entry->outgoingSensorUpdates[INDEX].protobuf_packet.state[sensorIndex].has_sensor_metadata = true;
    }
    LOGG_PRINT(LOG_DEBUG_E, NULL, "Written sensor update \n");
  }
  return INU_METADATA__RET_SUCCESS;
}
/****************************************************************************
 *
 *  Function Name: METADATA_SENSOR_UPDATER_i2cWrites
 *
 *  Description: Performs the I2C writes needed to update the exposure if the Strobe ISR isn't available  on a system
 *
 *  Inputs:
 *  sensorInfoP - Handle to perform the I2C writes for
 *  Outputs: none
 *
 *  Returns:
 *
 *  Context: sensors manager
 *
 ****************************************************************************/
static void METADATA_SENSOR_UPDATER_i2cWrites(SENSORS_MNGRG_sensorInfoT *sensorInfoP)
{
  ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
  GEN_SENSOR_DRVG_exposureTimeCfgT setExposure;
  IO_HANDLE sensorHandle = sensorInfoP->sensorHandle;
  PROJ_DRVG_projSetValParamsT projSetValParams;
  PROJ_DRVG_projGetValParamsT projGetValParams;
  GEN_SENSOR_DRVG_specificDeviceDescT *specificDev;

  // UINT64                              currentUsec = 0;
  // OS_LYRG_getUsecTime(&currentUsec);
  // printf("%llu strobe isr. (currentUsec %llu)\n",timestamp/1000,currentUsec);

  // for AS3648 projector in SERES module, we need to set control every strobe.'out_on' and 'mode_setting' AS3648's fields are automatically cleared after a flash pulse.
  if (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_AMS_CGSS130_E)
  {
    system("echo 1 > /sys/bus/i2c/drivers/as3648/0-0030/arm_select");
  }

  OS_LYRG_lockMutex(&sensorInfoP->strobeTsMutex);
  OS_LYRG_unlockMutex(&sensorInfoP->strobeTsMutex);

  // ret = ALTG_strobeHandle(sensorInfoP->altH, sensorInfoP);
  if (ERRG_SUCCEEDED(ret))
  {
    LOGG_PRINT(LOG_DEBUG_E, NULL, "sensorInfoP - %p, pendingChange: %d\n", sensorInfoP, sensorInfoP->pendingChange);
    if (sensorInfoP->pendingChange)
    {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Pending change found \n");
      setExposure.exposureTime = sensorInfoP->pendingExposure[sensorInfoP->pendingContext];
      setExposure.context = sensorInfoP->pendingContext;
      ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_SET_EXPOSURE_TIME_E, &setExposure);
      if (ERRG_SUCCEEDED(ret))
      {
        ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_SET_STROBE_TIME_E, &setExposure);
        if (ERRG_SUCCEEDED(ret))
        {
          SENSORS_MNGRG_updateSensorExpTime((IO_HANDLE)sensorInfoP, setExposure.exposureTime, setExposure.context);
        }
      }
      specificDev = GEN_SENSOR_DRVG_getSensorSpecificHandle(sensorInfoP->sensorHandle);
      LOGG_PRINT(LOG_DEBUG_E, NULL, "sensorSelect: %d sensor model:%d \n", sensorInfoP->sensorCfg.sensorSelect, specificDev->sensorModel);

      if ((specificDev && (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_CGS_132_E)) ||
          (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_OV_4689_E) || (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_AR_0234_E))
      {
        unsigned int savedAddr = specificDev->sensorAddress;
        if (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_CGS_132_E)
          specificDev->sensorAddress = 0x60;
        else if (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_AR_0234_E)
          specificDev->sensorAddress = 0x30;
        else
          specificDev->sensorAddress = 0x6c;
        ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_SET_EXPOSURE_TIME_E, &setExposure);
        if (ERRG_SUCCEEDED(ret))
        {
          ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_SET_STROBE_TIME_E, &setExposure);
          if (ERRG_SUCCEEDED(ret))
          {
            SENSORS_MNGRG_updateSensorExpTime((IO_HANDLE)sensorInfoP, setExposure.exposureTime, setExposure.context);
          }
        }
        specificDev->sensorAddress = savedAddr;
      }
    }
    sensorInfoP->pendingChange = 0;

    // update the projectorState in the sensor according to the current projector state. it was at this state when the strobe was active high
    // to the projector
    projGetValParams.projNum = PROJ_DRVG_PROJ_0_E; // TODO - bind through HW xml the projector to the sensor
    ret = IO_PALG_ioctl(IO_PALG_getHandle(IO_PROJ_0_E), PROJ_DRVG_GET_PROJ_VAL_CMD_E, &projGetValParams);
    if (ERRG_SUCCEEDED(ret))
    {
      sensorInfoP->projectorState = projGetValParams.projState;
    }

    // check for a pending projector change
    if (sensorInfoP->pendingProjectorChange)
    {
      projSetValParams.projNum = PROJ_DRVG_PROJ_0_E; // TODO - bind through HW xml the projector to the sensor
      projSetValParams.projState = sensorInfoP->pendingProjectorState;
      ret = IO_PALG_ioctl(IO_PALG_getHandle(IO_PROJ_0_E), PROJ_DRVG_SET_PROJ_VAL_CMD_E, &projSetValParams);
      sensorInfoP->pendingProjectorChange = 0;
    }
  }

  // GME_MNGRG_processEvent(GME_DRVG_HW_EVENT_STROBE_RISE_ISR_1_E + sensorInfoP->sensorCfg.strobeNum);
}
static ERRG_codeE METADATA_SENSOR_UPDATER_exposureUpdate(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex,const METADATA_SENSOR_UPDATER_interruptNotification *interruptTiming, 
  INU_metadata_State *incomingSensorState, INU_DEFSG_sensorContextE  context,   METADATA_UPDATE_registeredSensor *sensor)
{
  bool exposureUpdated = false;
  ERRG_codeE ret  = INU_METADATA__RET_SUCCESS;
  LOGG_PRINT(LOG_DEBUG_E, NULL, " Sensor Index %lu , SENSORS_MNGRG_setSensorExpTime: exposure:0x%x\n",sensorIndex, &incomingSensorState->sensor_metadata.exposure.exposure);
  if(!HELSINKI_getGroupedExposureUpdatesRequired(incomingSensorState->sensor.sensorType))
  {
  ERRG_codeE ret = SENSORS_MNGRG_setSensorExpTime(sensor->sensorInfo, &incomingSensorState->sensor_metadata.exposure.exposure, context);
  exposureUpdated = true;
  }
  else
  {
    if(!HELSINKI_ignoreExposureUpdate(incomingSensorState->sensor.sensorType, incomingSensorState->sensor.side))
    {
      if(sensorIndex == 0)
      {
          METADATA_UPDATE_registeredSensor *sensor_2 = &entry->sensors[1];
          if(sensor_2 && sensor_2->registered && sensor_2->sensorInfo)
          {
            LOGG_PRINT(LOG_DEBUG_E,NULL," Sensor Index %lu, Performing grouped exposure with exposure %lu \n ",sensorIndex,incomingSensorState->sensor_metadata.exposure.exposure );
            ret = SENSORS_MNGRG_setSensorExpTime(sensor->sensorInfo, &incomingSensorState->sensor_metadata.exposure.exposure, context);
            ret = SENSORS_MNGRG_setSensorExpTime(sensor_2->sensorInfo, &incomingSensorState->sensor_metadata.exposure.exposure, context);
            exposureUpdated = true;
          }
      }
      else
      {
        LOGG_PRINT(LOG_DEBUG_E, NULL, "Ignoring exposure update \n");
      }
    }
  }
  if (ERRG_SUCCEEDED(ret) )
  {
    if(exposureUpdated)
    {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Performing i2c writes for sensor updates \n");
      METADATA_SENSOR_UPDATER_i2cWrites(entry->sensors[sensorIndex].sensorInfo);
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Exposure update complete \n");
      METADATA_SENSOR_UPDATER_updateExposureAfterNFrames(entry, sensorIndex, METADATA_SENSOR_UPDATER_N_1, incomingSensorState);
    }
  }
  else
  {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to write exposure \n");
  }
  return ret;
}
static ERRG_codeE METADATA_SENSOR_UPDATER_gainUpdate(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex,const METADATA_SENSOR_UPDATER_interruptNotification *interruptTiming, 
  INU_metadata_State *incomingSensorState, INU_DEFSG_sensorContextE  context,   METADATA_UPDATE_registeredSensor *sensor)
{
  LOGG_PRINT(LOG_DEBUG_E, NULL, "Writing analogue,digital gain %.3f,%.3f\n",incomingSensorState->sensor_metadata.gain.analogue_gain, incomingSensorState->sensor_metadata.gain.digital_gain);
  ERRG_codeE ret = SENSORS_MNGRG_setSensorGain(sensor->sensorInfo, &incomingSensorState->sensor_metadata.gain.analogue_gain, &incomingSensorState->sensor_metadata.gain.digital_gain, context);
  if (ERRG_SUCCEEDED(ret))
  {
    LOGG_PRINT(LOG_DEBUG_E, NULL, "Gain written to sensor \n");
    METADATA_SENSOR_UPDATER_updateGainsAfterNFrames(entry, sensorIndex, METADATA_SENSOR_UPDATER_N_1, incomingSensorState->sensor_metadata.gain.analogue_gain, incomingSensorState->sensor_metadata.gain.digital_gain);
  }
  else
  {
    LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to write gain \n");
  }
  return ret;
}
/**
 *  @brief Updates the ROI position, originally this was being done in the EOF interrupt context.
 * I have found out from Inuitive that ROI updates can happen at any time be it the SOF interrupt or the EOF callback, 
      so I have removed some complexity and moved the ROI update to the SOF interrupt,
       this also solves the fact that we were sometimes not seeing the N+1 frame being updated
*     
*/
static ERRG_codeE METADATA_SENSOR_UPDATER_ROIUpdate(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex,const METADATA_SENSOR_UPDATER_interruptNotification *interruptTiming, 
  INU_metadata_State *incomingSensorState, INU_DEFSG_sensorContextE  context,   METADATA_UPDATE_registeredSensor *sensor)
{
  LOGG_PRINT(LOG_DEBUG_E, NULL, "Applying cropping update at %lu,%lu \n", incomingSensorState->sensor_metadata.roi.x_start, incomingSensorState->sensor_metadata.roi.y_start);
  const METADATA_TARGET_CONFIG_ppu *ppuPtr = &entry->mapentry->sensors[sensorIndex].ppu;
  if (ppuPtr->valid)
  {
      PPE_MNGRG_cropParamT cropParam = {
          .xStart = incomingSensorState->sensor_metadata.roi.x_start,
          .yStart = incomingSensorState->sensor_metadata.roi.y_start,
          .width = 0,
          .height = 0, /*Height and width aren't used within PPE_MNGRG_updatePPUCrop as this function only allows you to change the x and y cropping start coordinates*/
      };
      /*I have found out from Inuitive that ROI updates can happen at any time be it the SOF interrupt or the EOF callback, 
      so I have removed some complexity and moved the ROI update to the SOF interrupt,
       this also solves the fact that we were sometimes not seeing the N+1 frame being updated
      */
      PPE_MNGRG_updatePPUCrop(&cropParam, ppuPtr->index);
      PPE_MNGRG_setRegistersReady();
      /*Now update the metadata to show that ROI update has taken place*/
      METADATA_SENSOR_UPDATER_updateROIAfterNFrames(entry, sensorIndex, METADATA_SENSOR_UPDATER_N_1, incomingSensorState->sensor_metadata.roi.x_start, incomingSensorState->sensor_metadata.roi.y_start);
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Applied PPU cropping update for reader :%lu, %lu,%lu, PPU %lu, Sensor %lu \n", incomingSensorState->sensor_metadata.roi.x_start, incomingSensorState->sensor_metadata.roi.y_start, ppuPtr->index,sensorIndex);
      return INU_METADATA__RET_SUCCESS;
  }
  return INU_METADATA__ERR_ILLEGAL_STATE;
}
static ERRG_codeE METADATA_SENSOR_UPDATER_SensorROIUpdate(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex, const METADATA_SENSOR_UPDATER_interruptNotification *interruptTiming,
                                                    INU_metadata_State *incomingSensorState, INU_DEFSG_sensorContextE context, METADATA_UPDATE_registeredSensor *sensor)
{
  UINT32 startX = incomingSensorState->sensor_metadata.AFCropPositon.StartX;
  UINT32 startY = incomingSensorState->sensor_metadata.AFCropPositon.StartY;
  UINT32 frameID = 0;
  METADATA_UPDATER_getFullFrameIDWithOverFlowCounter(entry->mapentry,&frameID);
  LOGG_PRINT(LOG_DEBUG_E,NULL, "(%lu) Writing AF VST Crop position %lu,%lu\n", frameID,startX, startY);
  /* We have to use the group handle because when AR2020_DRVP_loadPresetTable is called previously, the group handle is used and not the sensor handle*/
  ERRG_codeE ret = SENSORS_MNGRP_setCropWindow(entry->groupHandle, startX, startY, context);
  if (ERRG_SUCCEEDED(ret))
  {
    LOGG_PRINT(LOG_DEBUG_E, NULL, "Written AF VST Crop position \n");
    METADATA_SENSOR_UPDATER_updateROIAfterNFrames(entry, sensorIndex, METADATA_SENSOR_UPDATER_N_1, startX, startY);
  }
  else
  {
    LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to write AF VST Crop position \n");
  }
  return INU_METADATA__RET_SUCCESS;
}
static ERRG_codeE METADATA_SENSOR_UPDATER_FocusUpdate(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex,const METADATA_SENSOR_UPDATER_interruptNotification *interruptTiming, 
  INU_metadata_State *incomingSensorState, INU_DEFSG_sensorContextE  context,   METADATA_UPDATE_registeredSensor *sensor)
{
  
  LOGG_PRINT(LOG_DEBUG_E, NULL, "Applying focus update at %ld \n", incomingSensorState->sensor_metadata.AFFocusPosition.AFFocusPosition);
  inu_sensor__setFocusParams_t focusParameters;
  focusParameters.dac = incomingSensorState->sensor_metadata.AFFocusPosition.AFFocusPosition;
  focusParameters.context = context;
  focusParameters.mode = INU_SENSOR__FOCUS_MODE_MANUAL;
  FOCUS_CTRLG_setFocusParams(sensor->sensorInfo,&focusParameters);
  METADATA_SENSOR_UPDATER_updateFocusAfterNFrames(entry,sensorIndex,METADATA_SENSOR_UPDATER_N_1,incomingSensorState->sensor_metadata.AFFocusPosition.AFFocusPosition);
  return INU_METADATA__RET_SUCCESS;
}
static ERRG_codeE METADATA_SENSOR_UPDATER_UARTSyncDebugUpdate(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex,const METADATA_SENSOR_UPDATER_interruptNotification *interruptTiming, 
  INU_metadata_State *incomingSensorState, INU_DEFSG_sensorContextE  context,   METADATA_UPDATE_registeredSensor *sensor)
{
 
  const INT32 startIndex = METADATA_SENSOR_UPDATER_calculateFramesAfterStartIndex(METADATA_SENSOR_UPDATER_N_1);
  const UINT32 nextBuffer = METADATA_UPDATER_calculateMetadataBuffer(entry,false);
  for (int i = startIndex; i < (MAX_NUM_BUFFERS_LOOPS); i++)
  {
    const UINT32 INDEX = (nextBuffer + i) % MAX_NUM_BUFFERS_LOOPS; /*Calculate the buffer index */
    INU_metadata_State *outgoingState = &entry->outgoingSensorUpdates[i].protobuf_packet.state[sensorIndex];
    outgoingState->sensor_metadata.isp_expo.uart_sync.cmd_id = incomingSensorState->sensor_metadata.isp_expo.uart_sync.cmd_id;
    outgoingState->sensor_metadata.isp_expo.uart_sync.frameID = incomingSensorState->sensor_metadata.isp_expo.uart_sync.frameID;
    outgoingState->sensor_metadata.isp_expo.uart_sync.slu_number = incomingSensorState->sensor_metadata.isp_expo.uart_sync.slu_number;
    outgoingState->sensor_metadata.isp_expo.uart_sync.wait_for_slu_SOF_Interrupt = incomingSensorState->sensor_metadata.isp_expo.uart_sync.wait_for_slu_SOF_Interrupt;
    outgoingState->sensor_metadata.isp_expo.has_uart_sync = incomingSensorState->sensor_metadata.isp_expo.has_uart_sync;
  }
  return INU_METADATA__RET_SUCCESS;
}
static INT64 METADATA_SENSOR_UPDATE_getInterruptLatency(const METADATA_SENSOR_UPDATER_interruptNotification *interruptTiming)
{
    UINT64 currentTime = 0;
    OS_LYRG_getUsecTime(&currentTime);
    const UINT64 timestamp_us  = interruptTiming->timestamp/1000; /*Convert from ns to us*/
    INT64 timeDiff = currentTime - timestamp_us;
    return timeDiff;
}
ERRG_codeE  METADATA_SENSOR_UPDATE_handleAR2020DelayedGainUpdate(METADATA_UPDATER_channelHandleT *entry)
{

    if((entry->mapentry->axiReader != ISP0_READER) &&  (entry->mapentry->axiReader != ISP1_READER)) // Only supported for ISP downscaled stream
    {
        LOGG_PRINT(LOG_DEBUG_E,NULL,"This function should only be called for the AXI0 reader \n");
        return ERR_NOT_SUPPORTED;
    }
    if(OS_LYRG_getCurrNumMsg(&entry->AR2020_gainQ) <= 0)
    {
        LOGG_PRINT(LOG_DEBUG_E,NULL,"No delayed AR2020 Gain updates \n");
        return INU_METADATA__RET_SUCCESS;
    }
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Handling delayed AR2020 Gain update \n");
    METADATA_AR2020Gain_Queue_Entry qEntry;
    UINT32 messageSize = sizeof(METADATA_AR2020Gain_Queue_Entry);
    INT32 status = OS_LYRG_waitRecvMsg(&entry->AR2020_gainQ,(uint8_t *)&qEntry,&messageSize);
    if(status == SUCCESS_E)
    {
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Handling delayed AR2020 Gain update for Sensor Index  %lu, Gain %f \n",qEntry.sensor_index, qEntry.expoParams.analogGain);
        METADATA_UPDATE_registeredSensor *sensor = &entry->sensors[qEntry.sensor_index];
        float dummy = 0;
        ERRG_codeE ret = SENSORS_MNGRG_setSensorGain(sensor->sensorInfo,&qEntry.expoParams.analogGain,&dummy,INU_SENSOR__CONTEXT_A);
        const uint32_t AR2020_EXPO_UPDATE_DELAY  = METADATA_SENSOR_UPDATER_N_1;
        METADATA_ISP_UPDATER_updateExpoAfterNFrames(entry,qEntry.sensor_index,AR2020_EXPO_UPDATE_DELAY,
            &qEntry.expoParams,true,false,false,false); /*Only other update expo and other parameters, do not update AGain and DGain*/
    }
    return INU_METADATA__RET_SUCCESS;

}
ERRG_codeE METADATA_SENSOR_UPDATER_performSensorUpdate(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex,const METADATA_SENSOR_UPDATER_interruptNotification *interruptTiming)
{
  INT32 status = ( SUCCESS_E);
  METADATA_UPDATE_registeredSensor *sensor = &entry->sensors[sensorIndex];
  /*This structure is populated to  only contain the latest command of each type of command
  For example it will only contain the latest WB command and the latest Crop position command*/
  INU_metadata_State aggregated_command  = INU_metadata_State_init_zero; 
  if (sensor == NULL)
  {
    return INU_METADATA__ERR_METADATA_INVALID_SENSOR_HANDLE;
  }
  if (sensor->registered == false)
  {
    return INU_METADATA__ERR_METADATA_NO_SENSOR_UPDATE_FOR_THIS_SENSOR;
  }
  if (entry->sensors[sensorIndex].sensorInfo == NULL)
  {
    return INU_METADATA__ERR_METADATA_INVALID_SENSOR_HANDLE;
  }

  UINT32 messageSize = sizeof(INU_metadata_State);
  INU_metadata_State *incomingSensorState = &sensor->incomingSensorUpdate;
  unsigned int nummessages = OS_LYRG_getCurrNumMsg(&sensor->sensorQueue);
  if (incomingSensorState == NULL)
  {
    LOGG_PRINT(LOG_ERROR_E, NULL, "Invalid Sensor State \n");
    return INU_METADATA__ERR_METADATA_INVALID_SENSOR_STATE;
  }
  if (&sensor->sensorQueue == NULL)
  {
    LOGG_PRINT(LOG_ERROR_E, NULL, "Invalid Sensor Queue \n");
    return INU_METADATA__ERR_METADATA_INVALID_SENSOR_QUEUE;
  }
  INT64 timeDiff = METADATA_SENSOR_UPDATE_getInterruptLatency(interruptTiming);
  if(timeDiff > MAXIMUM_TIME_DIFF_USEC)
  {
    /*Make sure we catch this high interrupt latency before we start dequeing sensor updates and potentially losing messages in the while loop*/
    return INU_METADATA__RET_SUCCESS;
  }
  /*Check if we have messages to process*/
  if (nummessages ==  0)
  {
    LOGG_PRINT(LOG_DEBUG_E,NULL,"No sensor updates \n");
    INU_metadata_State *incomingSensorState = &sensor->incomingSensorUpdate;
    /*
    Historically we only updated 3/4 structures so we also had to update the last structure when the next frame was received
    However this code isn't need anymore because we now update all four structures during a sensor update but only update 3/4 buffers.
    */
    return INU_METADATA__ERR_METADATA_NO_SENSOR_UPDATE_FOR_THIS_SENSOR;
  }
  LOGG_PRINT(LOG_DEBUG_E,NULL,"Searching for sensor updates for sensorIndex:%lu \n",sensorIndex);
  UINT64 before, after = 0;
  OS_LYRG_getTimeNsec(&before);
/*For the helsinki project, the commands need to be treated in a LIFO manner
  That means that we must use the latest ISP WB command, latest crop position command and discard anything older than that

  Here's what the code ends up doing:
  1. We dequeue all of the sensor updates and then populate the aggregated_command structure
  2. We then perform the I2C writes using the aggregated_command structure sequentially
*/

  while ((status == SUCCESS_E) && (messageSize == sizeof(INU_metadata_State)) && (nummessages > 0))
    {
      /*Read the status*/
      status = OS_LYRG_recvMsg(&sensor->sensorQueue, (uint8_t *)incomingSensorState, &messageSize, 0);
      nummessages = OS_LYRG_getCurrNumMsg(&sensor->sensorQueue);
      /*Dequeue all commands and use the last command of each type, For the helsinki project we just want to use the last of each type of command and discard anything that is older than that*/
      if (incomingSensorState->sender == INU_metadata_HardwareType_HOST)
      {
        if (incomingSensorState->sensor_metadata.has_exposure)
        {
          aggregated_command.sensor_metadata.exposure = incomingSensorState->sensor_metadata.exposure;
          aggregated_command.sensor_metadata.has_exposure = true;
        }
        if (incomingSensorState->sensor_metadata.has_gain)
        {
          aggregated_command.sensor_metadata.gain = incomingSensorState->sensor_metadata.gain;
          aggregated_command.sensor_metadata.has_gain = true;
        }
        if(incomingSensorState->sensor_metadata.has_roi)
        {
          aggregated_command.sensor_metadata.roi = incomingSensorState->sensor_metadata.roi;
          aggregated_command.sensor_metadata.has_roi = true;
        }
        if(incomingSensorState->sensor_metadata.has_wb)
        {
          aggregated_command.sensor_metadata.wb = incomingSensorState->sensor_metadata.wb;
          aggregated_command.sensor_metadata.has_wb = true;
        }
        if(incomingSensorState->sensor_metadata.has_isp_expo)
        {
          aggregated_command.sensor_metadata.isp_expo = incomingSensorState->sensor_metadata.isp_expo;
          aggregated_command.sensor_metadata.has_isp_expo = true;
        }
        if (incomingSensorState->sensor_metadata.has_AFCropPositon)
        {
          aggregated_command.sensor_metadata.AFCropPositon = incomingSensorState->sensor_metadata.AFCropPositon;
          aggregated_command.sensor_metadata.has_AFCropPositon = true;
        }
        if (incomingSensorState->sensor_metadata.has_AFFocusPosition)
        {
          /*Only the AF VST allows the focus to be changed*/
          aggregated_command.sensor_metadata.AFFocusPosition = incomingSensorState->sensor_metadata.AFFocusPosition;
          aggregated_command.sensor_metadata.has_AFFocusPosition = true;
        }
      }
      else
      {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Sender is not the host for timestamp %lu for sensorIndex %lu \n", incomingSensorState->timestamp_ns.nanoseconds, sensorIndex);
      }
      /*Measure interrupt latency to prevent sensor updates when the interrupt latency is too high*/
      timeDiff = METADATA_SENSOR_UPDATE_getInterruptLatency(interruptTiming); 

    
  }
  /*Now we will do all of the I2C writes using the aggregated set of commands*/
  /*For the debug metadata we will use the last sensor update as the reference*/
  const INU_DEFSG_sensorContextE context = METADATA_SENSOR_UPDATER_convertINU_metadata_Context(incomingSensorState->context);
  METADATA_SENSOR_UPDATER_updateConstFieldsMetadata(entry, sensorIndex, METADATA_SENSOR_UPDATER_N_1);
  METADATA_SENSOR_UPDATER_copyRxTimingInfo(entry,sensorIndex,&incomingSensorState->snsr_update_timing.sensor_update_rx_timing,METADATA_SENSOR_UPDATER_N_1);
  METADATA_SENSOR_UPDATER_updateDequeTimingInfo(entry, sensorIndex, METADATA_SENSOR_UPDATER_N_1);
  METADATA_SENSOR_UPDATER_updateSOFInterruptTimingInfo(entry,sensorIndex,interruptTiming,METADATA_SENSOR_UPDATER_N_1);
  METADATA_SENSOR_UPDATER_copySyncedInfo(entry,sensorIndex,&incomingSensorState->sensor_metadata.isp_expo.uart_sync,METADATA_SENSOR_UPDATER_N_1);
  LOGG_PRINT(LOG_DEBUG_E, NULL, "Performing Sensor %lu Updates, %lu,%lu \n", nummessages,incomingSensorState->sensor_metadata.has_isp_expo,incomingSensorState->sensor_metadata.has_wb );
  /*These could be re-ordered depending on if any of them are time sensitive or not*/
  if (aggregated_command.sensor_metadata.has_exposure)
  {
    METADATA_SENSOR_UPDATER_exposureUpdate(entry,sensorIndex,interruptTiming,&aggregated_command,context,sensor);
  }
  if (aggregated_command.sensor_metadata.has_gain)
  {
    METADATA_SENSOR_UPDATER_gainUpdate(entry,sensorIndex,interruptTiming,&aggregated_command,context,sensor);
  }
  if(aggregated_command.sensor_metadata.has_roi)
  {
    METADATA_SENSOR_UPDATER_ROIUpdate(entry,sensorIndex,interruptTiming,&aggregated_command,context,sensor);
  }
  if(aggregated_command.sensor_metadata.has_wb)
  {
    METADATA_ISP_UPDATER_updateWBGains(entry,sensorIndex,interruptTiming,&aggregated_command,context,sensor);
  }
  if(aggregated_command.sensor_metadata.has_isp_expo)
  {
    METADATA_ISP_UPDATER_updateExpoParameters(entry,sensorIndex,interruptTiming,&aggregated_command,context,sensor);
  }
  if (aggregated_command.sensor_metadata.has_AFCropPositon)
  {
    METADATA_SENSOR_UPDATER_SensorROIUpdate(entry,sensorIndex,interruptTiming,&aggregated_command,context,sensor);
  }
  if (aggregated_command.sensor_metadata.has_AFFocusPosition)
  {
    /*Only the AF VST allows the focus to be changed*/
    METADATA_SENSOR_UPDATER_FocusUpdate(entry,sensorIndex,interruptTiming,&aggregated_command,context,sensor);
  }
  METADATA_SENSOR_UPDATER_updatei2cWritesCompleteTimingInfo(entry, sensorIndex, METADATA_SENSOR_UPDATER_N_1);
  
  OS_LYRG_getTimeNsec(&after);
  UINT64 timediff = after - before;
  LOGG_PRINT(LOG_DEBUG_E, NULL, "Sent sensor update to sensor update Queue %llu ns, %lu messages  \n", timediff, nummessages);
  METADATA_UPDATER_setPendingBufferUpdatesForAllBuffers(entry); /* Mark all metadata buffers as needing updating*/
  return INU_METADATA__RET_SUCCESS;


}
