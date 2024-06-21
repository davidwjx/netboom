#include "sensorsync_updater.h"
#include "inu_sensorsync_serializer.h"
#include "iae_drv.h"
#include "helsinki.h"
#include "gme_drv.h"
#include "helsinki.h"
#include <unistd.h>
#define SENSORSYNC_QUEUE_SIZE 10
#define EOF_QUEUE_SIZE 10
#define SENSORSYNC_UPDATE_DEBUG_LEVEL (LOG_DEBUG_E)
OS_LYRG_msgQueT                 sensorSyncQueue;                                 /*Queue of FSG updates to deal with on each EOF (Added to by connection layer, read by the sensor sync manager) */
//#define SENSORSYNC_UPDATER_TOGGLE_ENABLE_BIT    /*Not needed due to workaround found by toggling all of the software triggers high after updating the FSG registers */
//#define SENSOR_SYNC_RESET_IAE_BEFORE_UPDATES    /*Not needed due to workaround found by toggling all of the software triggers high after updating the FSG registers  */
static OS_LYRG_msgQueT eofInterruptQueue; /*Queue for EOF interrupts, there's one thread for the target*/
static OS_LYRG_threadHandle SENSORSYNC_UPDATER_threadHandle = NULL;

static ERRG_codeE SENSOR_SYNC_updateCounterConfiguration(SENSOR_SYNC_FSG_Counter_Configuration *counterConfig)
{
  ERRG_codeE ret = 0;
    LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL,NULL,"Count Mode %lu, Trigger source %lu, Value %lu \n", counterConfig->countMode.countMode, 
      counterConfig->triggerSource.triggerSource,counterConfig->counter,counterConfig->value.counter_value);
  if(counterConfig->has_countMode)
  {
     ret = IAE_DRVG_setFSGCntMode(counterConfig->counter,counterConfig->countMode.countMode);
    if(ERRG_FAILED(ret))
    {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to update FSG Count mode for ftrig[%lu] \n",counterConfig->counter);
      return ret;
    }
  }
  if(counterConfig->has_triggerSource)
  {
    ret = IAE_DRVG_setFSGTrigSrc(counterConfig->counter,counterConfig->triggerSource.triggerSource);
    if(ERRG_SUCCEEDED(ret))
    {
      LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL,NULL,"FSG Trigger Source updated to be %lu \n", counterConfig->triggerSource.triggerSource);
    }
  }
  if(counterConfig->has_value)
  {
    LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL,NULL,"(%lu) Writing counter value to be %lu \n",counterConfig->counter,counterConfig->value.counter_value);
    ret = IAE_DRVG_setFSGCntPeriod(counterConfig->counter,counterConfig->value.counter_value);
    if(ERRG_FAILED(ret))
    {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to update Frame Rate for FSG Counter [%lu] \n",counterConfig->counter);
      return ret;
    }
  }
  return ret;
}

ERRG_codeE SENSOR_SYNC_updateChannelConfiguration(SENSOR_SYNC_Pulse_Config *pulseConfig)
{
    IAE_DRVG_snsrCtrlFSGFtrigPulseCtrl cfg;
    /*Create IAE_DRVG_snsrCtrlFSGFtrigPulseCtrl structure based off the pulseConfig*/
    cfg.width = pulseConfig->channelConfiguration.pulseWidth; 
    cfg.pulseEnable = pulseConfig->channelConfiguration.pulseEnable;
    cfg.counterSrc = pulseConfig->channelConfiguration.counterSource.counter_source;
    /*Convert the pulse polarity enum*/
    if (pulseConfig->channelConfiguration.pulsePolarity == SENSOR_SYNC_Pulse_Polarity_Active_High)
      cfg.pulsePolarity = IAE_DRVG_FSG_PULSE_ACTIVE_HIGH;
    else
      cfg.pulsePolarity = IAE_DRVG_FSG_PULSE_ACTIVE_LOW;
    ERRG_codeE ret = IAE_DRVG_FSGPulseCtrlCfg(pulseConfig->channelConfiguration.pulseChannel,&cfg);
      if(ERRG_SUCCEEDED(ret))
      {
        LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL,NULL,"Pulse configuration updated \n");
      }
      else
      {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to update pulse configuration for ftrig[%lu] \n",pulseConfig->channelConfiguration.pulseChannel);
      }  
      ret = IAE_DRVG_setPulseOffset(pulseConfig->channelConfiguration.pulseChannel,pulseConfig->channelConfiguration.pulseOffset);
      if(ERRG_FAILED(ret))
      {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to update FSG Pulse offset for ftrig[%lu] \n",pulseConfig->channelConfiguration.pulseChannel);
        return ret;
      }
      ret = IAE_DRVG_setFSGPulseRepeatPeriod(pulseConfig->channelConfiguration.pulseChannel,pulseConfig->channelConfiguration.pulseRepeatPeriod);
      if(ERRG_FAILED(ret))
      {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to update FSG Pulse repeat period for ftrig[%lu] \n",pulseConfig->channelConfiguration.pulseChannel);
        return ret;
      }
      LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL,NULL,"Channel configuration done \n");
  return ret;
}

/**
 * @brief High level function to update IAE FSG regisers based on the configuration found in pulseConfig
 * 
 *
 * @param entry Metadata channel entry
 * @param pulseConfig Pulse configuration to apply
 * @return Returns an error code
 */
static ERRG_codeE SENSORSYNC_UPDATER_updateFSGRegisters(SENSOR_SYNC_Pulse_Config *pulseConfig)
{
  /*Check that FSG updates are enabled*/

#ifdef SENSORSYNC_UPDATER_TOGGLE_ENABLE_BIT
    IAE_DRVG_FSG_sensDisable();
#endif
    LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL,NULL,"Writing FSG registers for counter source %lu \n", pulseConfig->channelConfiguration.counterSource);
#ifdef SENSOR_SYNC_RESET_IAE_BEFORE_UPDATES
    GME_DRVG_iaeCoreReset();
    XMLDB_restoreIAERegisters();
#endif
  ERRG_codeE ret = 0;
    if(pulseConfig->has_counterConfiguration)
      ret = SENSOR_SYNC_updateCounterConfiguration(&pulseConfig->counterConfiguration);
    else
      LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL,NULL,"No Counter configuration found \n");

    if(pulseConfig->has_channelConfiguration)
      ret = SENSOR_SYNC_updateChannelConfiguration(pulseConfig);
    else
      LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL,NULL,"No channel configuration found \n");

    /*Issue a software trigger after updating all of the FSG registers,
      this is to deal with quirk of the FSG. 
      If you don't perform a software trigger then updates are ignored
    */

    /*Switch to GPIO mode to try and mask out the pulse when the SW trigger happens*/
    if(pulseConfig->has_counterConfiguration )
    {
      IAE_DRVG_fsgCounterNumE GMERegister = pulseConfig->counterConfiguration.counter;
      GME_DRVG_modifyFTRIGMode(GMERegister,TRIGGER_MANAGER);
      IAE_DRVG_FSG_swTrigger(pulseConfig->counterConfiguration.counter);
      usleep(100); /*100 microseconds should mask out the pulse*/
      GME_DRVG_modifyFTRIGMode(GMERegister,FSG_BLOCK);
    }

    IAE_DRVG_iaeReady();
  

#ifdef SENSORSYNC_UPDATER_TOGGLE_ENABLE_BIT
    IAE_DRVG_FSG_sensEnable();
#endif

    return INU_SENSORSYNC__RET_SUCCESS;

}

static ERRG_codeE SENSORSYNC_UPDATER_performSensorSyncCnfgUpdate(OS_LYRG_msgQueT *queue)
{
  INT32 status = -1;
  UINT32 messageSize = sizeof(SENSOR_SYNC_Pulse_Config);
  SENSOR_SYNC_Pulse_Config incomingPulseConfig;

  /*Check if we have messages to process*/
  status = OS_LYRG_recvMsg(queue, (UINT8 *)&incomingPulseConfig, &messageSize, 10000);

  UINT64 before, after = 0;
  OS_LYRG_getTimeNsec(&before);
  while ((status == SUCCESS_E) && (messageSize == sizeof(SENSOR_SYNC_Pulse_Config)))
  {

    ERRG_codeE ret = SENSORSYNC_UPDATER_updateFSGRegisters(&incomingPulseConfig);
    if(ERRG_SUCCEEDED(ret))
    {
      LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL,NULL,"Sensor Sync updates have completed \n");
    }
    else
    {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Sensor Sync updates have failed \n");
    }
 
    messageSize == sizeof(SENSOR_SYNC_Pulse_Config);
    UINT32 nummessages = OS_LYRG_getCurrNumMsg(queue);
    if(nummessages)
      status = OS_LYRG_recvMsg(queue, (uint8_t *)&incomingPulseConfig, &messageSize, 0);
    else
      break;
  }
  OS_LYRG_getTimeNsec(&after);
  UINT64 timediff = after - before;
  LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL, NULL, "Sent sensor update to sensor update Queue %llu ns \n", timediff);

  return INU_SENSORSYNC__RET_SUCCESS;
}
/**
 * @brief Sensor Synchronization updater thread 
 *        This thread updates the IAE_FSG registers based on requests sent via the Host API
 *
 * @param arg Argument (UNUSED)
 * @return Returns an error code
 */
static int SENSORSYNC_UPDATER_updateSensorSynchronizationThread(void *arg)
{
  /*This thread will update the FSG registers (which are used to trigger sensors) after an EOF interrupt*/

  while (1)
  {
    /* We should still handle sensor sync commands even if we don't have an EOF interrupt*/
    SENSORSYNC_UPDATER_performSensorSyncCnfgUpdate(&sensorSyncQueue);
    LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL, NULL, "Found DMA channel to update FSG \n");
  }
  return 0;
}
ERRG_codeE SENSORSYNC_UPDATER_processIncomingSensorSyncCfgUpdate(char *buffer, size_t size)
{
  inu_sensorSync__updateT tempSensorSync;
  LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL, NULL, "Received sensor update with size %lu bytes \n", size);
  ERRG_codeE ret = inu_sensorsync__deserialize(buffer, size, &tempSensorSync);
  const int registeredDMAChannels = METADATA_UPDATER_numberRegisteredDMAChannels();
  METADATA_UPDATER_channelHandleT *metadatachannels = METADATA_UPDATER_getChannelHandleArray();
  if (ERRG_FAILED(ret))
  {
    LOGG_PRINT(LOG_ERROR_E, NULL, "Decode failed (errorcode:%lu) of sensor sync with size %lu bytes \n", ret, size);
    return ret;
  }
  else
  {
    LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL, NULL, "Decoded sensor sync configuration update request, now looking in our map for matching channels in %lu registered channels,configCount =%lu \n", registeredDMAChannels, tempSensorSync.protobuf_packet.configs_count);
    for (int i = 0; i < tempSensorSync.protobuf_packet.configs_count; i++)
    {
     
      /*If the target sync group is the same as the sensor type then we have found a match and can send the Pulse config to the sensor sync queue*/
      if (1)
      {
        {
          LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL,NULL,"Added sensor sync to the sensor sync queue for index %lu with counter %lu \n",i,tempSensorSync.protobuf_packet.configs[i].counterConfiguration.counter );
          UINT64 numMessages = OS_LYRG_getCurrNumMsg(&sensorSyncQueue);
          if (numMessages < EOF_QUEUE_SIZE)
          {
            LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL, NULL,"Sending message to the Sensor Sync Queue \n");
            OS_LYRG_sendMsg(&sensorSyncQueue, (UINT8 *)&tempSensorSync.protobuf_packet.configs[i], sizeof(SENSOR_SYNC_Pulse_Config));
          }
          else
          {
            LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL, NULL, "Dropped messages due to lack of space in sensorSyncQueue \n");
          }
        }
    
      }
      
    }
  }
  return ret;
}

ERRG_codeE SENSORSYNC_UPDATER_init()
{

  SENSORSYNC_UPDATER_initSensorSyncQueue(); /*Sensor Sync queue*/
  OS_LYRG_threadParams threadParams;
  /*Create the end of frame interrupt queue */
  eofInterruptQueue.maxMsgs = EOF_QUEUE_SIZE;
  eofInterruptQueue.msgSize = sizeof(METADATA_SENSOR_UPDATER_interruptNotification);
  snprintf(eofInterruptQueue.name, OS_LYRG_MAX_MQUE_NAME_LEN, "/EOFInterruptNotification");
  ERRG_codeE ret = OS_LYRG_createMsgQue(&eofInterruptQueue, OS_LYRG_MULTUPLE_EVENTS_NUM_E);
  if (ret != SUCCESS_E)
  {
    LOGG_PRINT(LOG_ERROR_E, NULL, "Could not create EOF interrupt queue \n");
  }
  /*Thread parameters*/
  threadParams.func =   SENSORSYNC_UPDATER_updateSensorSynchronizationThread;
  threadParams.id =     OS_LYR_SENSOR_SYNC_THREAD_ID_E;
  threadParams.event =  NULL;
  threadParams.param =  NULL;

  LOGG_PRINT(LOG_INFO_E, NULL, "Starting sensor synchronization updater thread\n");

  // create thread
#ifdef CDE_DRVG_ENABLE_SENSOR_SYNC_AND_UPDATE
  SENSORSYNC_UPDATER_threadHandle = OS_LYRG_createThread(&threadParams);
#endif
  return INU_METADATA__RET_SUCCESS;
}

ERRG_codeE SENSORSYNC_UPDATER_initSensorSyncQueue()
{
  sensorSyncQueue.maxMsgs = SENSORSYNC_QUEUE_SIZE;
  sensorSyncQueue.msgSize = sizeof(SENSOR_SYNC_Pulse_Config);
  snprintf(sensorSyncQueue.name, OS_LYRG_MAX_MQUE_NAME_LEN, "/SnsrSyncQueue_%lu", 0);
  ERRG_codeE ret = OS_LYRG_createMsgQue(&sensorSyncQueue, OS_LYRG_MULTUPLE_EVENTS_NUM_E);
  if (ret != SUCCESS_E)
  {
    LOGG_PRINT(LOG_ERROR_E, NULL, "Could not create Sensor updater Queue \n");
  }
  return ret;
}

void SENSORSYNC_UPDATER_eofInterruptCb(UINT64 timestamp, UINT32 slu, void *argP)
{
  #if 0 
    LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL,NULL,"Timestamp,%llu, SLU,%lu \n", timestamp,slu);
    METADATA_SENSOR_UPDATER_interruptNotification interruptNotification = {.timestamp = timestamp, .slu = slu};
    if(&eofInterruptQueue!=NULL && &interruptNotification!=NULL )
    {
        UINT32 numMessages = OS_LYRG_getCurrNumMsg(&eofInterruptQueue);
        if(numMessages < EOF_QUEUE_SIZE) /*Check that there's space in the queue*/
        {
            OS_LYRG_sendMsg(&eofInterruptQueue, (UINT8 *)&interruptNotification, sizeof(METADATA_SENSOR_UPDATER_interruptNotification));
        }
        else
        {
            /*No space to write message*/
            LOGG_PRINT(SENSORSYNC_UPDATE_DEBUG_LEVEL,NULL,"Cannot write EOF interrupt \n");
        }
    }
    else
    {
        LOGG_PRINT(LOG_INFO_E,NULL,"Nullptr detected \n");
    }
    #endif
}
ERRG_codeE SENSORSYNC_UPDATER_startFSG(const SENSORS_MNGRG_sensorInfoT *handle)
{
  const int registeredDMAChannels = METADATA_UPDATER_numberRegisteredDMAChannels();
  const METADATA_UPDATER_channelHandleT *metadatachannels = METADATA_UPDATER_getChannelHandleArray();
  LOGG_PRINT(LOG_INFO_E,NULL,"Searching for group handle %p so that we can enable the FTRIG output \n", handle);
  if(metadatachannels== NULL)
  {
    return INU_SENSORSYNC__ERR_NOT_SUPPORTED;
  }
  for (int i = 0; i < registeredDMAChannels; i++)
  {
    if (metadatachannels[i].inUse == true && metadatachannels[i].mapentry != NULL)
    {
      if(metadatachannels[i].groupHandle == handle)
      {
        for(int j =0; j <1;j++ )
        {
          if (metadatachannels[i].sensors[j].registered)
          {
            IAE_DRVG_enableFTRIGOutput(metadatachannels[i].mapentry->counterSrc);
          }
        }
      }
    }
  }
  return  INU_SENSORSYNC__RET_SUCCESS;
}
ERRG_codeE SENSORSYNC_UPDATER_stopFSG(const SENSORS_MNGRG_sensorInfoT *handle)
{
  const int registeredDMAChannels = METADATA_UPDATER_numberRegisteredDMAChannels();
  const METADATA_UPDATER_channelHandleT *metadatachannels = METADATA_UPDATER_getChannelHandleArray();
  LOGG_PRINT(LOG_INFO_E,NULL,"Searching for group handle %p so that we can disable the FTRIG output \n", handle);
  if(metadatachannels== NULL)
  {
    return INU_SENSORSYNC__ERR_NOT_SUPPORTED;
  }
  for (int i = 0; i < registeredDMAChannels; i++)
  {
    if (metadatachannels[i].inUse == true && metadatachannels[i].mapentry != NULL)
    {
      if(metadatachannels[i].groupHandle == handle)
      {
        for(int j =0; j <1;j++ )
        {
          if (metadatachannels[i].sensors[j].registered)
          {
            const IAE_DRVG_fsgCounterNumE GMERegister = metadatachannels->mapentry->fsgIndex;
            /*Switch the Pin back to a GPIO pin and disable the Ftrig output*/
            GME_DRVG_modifyFTRIGMode(GMERegister,TRIGGER_MANAGER);
            IAE_DRVG_disableFTRIGOutput(metadatachannels[i].mapentry->counterSrc);
            LOGG_PRINT(LOG_INFO_E,NULL,"Disabled FSG %lu by switching GME mux and disabling Ftrig outputs \n", GMERegister);
          }
        }
      }
    }
  }
  return  INU_SENSORSYNC__RET_SUCCESS;
}