#pragma once
#include "inu_sensorsync.h"
#include "inu_device.h"
#include "inu_types.h"
#include "metadata_updater.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "sensorsync_updater.h"
#include "sensorsync_updater.h"
typedef struct 
{
  UINT32 slu;
  UINT64 timestamp;
}SENSORSYNC_UPDATER_EOFInterruptNotificationT;
/**
 * @brief Deals with a new Sensor synchronization Request from the host
 * 
 *
 * @param buffer Buffer containing a sensor Synchronization request
 * @param size   Size of the buffer
 * @return Returns an error code
 */
ERRG_codeE SENSORSYNC_UPDATER_processIncomingSensorSyncCfgUpdate(char *buffer, const size_t size);
/**
 * @brief Callback for when an EOF interrupt occurs
 * 
 *
 * @param timestamp Timestamp of when the EOF interrupt happens
 * @param slu       SLU that the EOF interrupt happened
 * @param argP      UNUSED
 */
void SENSORSYNC_UPDATER_eofInterruptCb(UINT64 timestamp, UINT32 slu, void *argP);
/**
 * @brief Initializes the Sensor Synchronizaition Config Updater, the following tasks will be done:
 *  Relevant Queues are created
 *  The Sensor Sync thread is created
 * @return Returns an error code
 */
ERRG_codeE  SENSORSYNC_UPDATER_init();
/**
 * @brief Initializes the queue used to handle sensor sync updates
 * 
 *
 * @param entry Entry to initialize the sensor sync queue for
 * @return Returns an error code
 */
ERRG_codeE  SENSORSYNC_UPDATER_initSensorSyncQueue();
/**
 * @brief Switches between FSG managed sensor synchronization and Trigger manager based sensor synchronization.
 * 
 *
 * @param handle Handle of the sensor to switch the trigger source mode
 * @param triggerSrc Trigger Source mode
 * @return Returns an error code
 */
ERRG_codeE SENSORSYNC_UPDATER_switchTriggerMngrSrc(const SENSORS_MNGRG_sensorInfoT *handle,const INU_DEFSG_sensorTriggerSrcE triggerSrc);
/**
 * @brief This function can be used to start Pulse outputs from the FSG
 * @param handle Sensor to start the FSG for 
 * @return Returns an error code
 */
ERRG_codeE SENSORSYNC_UPDATER_startFSG(const SENSORS_MNGRG_sensorInfoT *handle);
/**
 * @brief Stops the FSG Trigger
 * 
 *
 * @param handle Sensor to stop the FSG for 
 * @return Returns an error code.
 */
ERRG_codeE SENSORSYNC_UPDATER_stopFSG(const SENSORS_MNGRG_sensorInfoT *handle);
/**
 * @brief This function can be used to  change the triggering setup for a channel
*/
ERRG_codeE SENSOR_SYNC_updateChannelConfiguration(SENSOR_SYNC_Pulse_Config *pulseConfig);
#ifdef __cplusplus
}
#endif




