#pragma once
#include "metadata_sensor_updater.h"
#include "helsinki.h"
#include "metadata_updater.h"
#include "gen_sensor_drv.h"
#include "ppe_mngr.h"
#include "focus_ctrl.h"
#include "inu_sensor_api.h"
#include <signal.h>
#ifdef __cplusplus
extern "C" {
#endif
/* @brief Updates the white balance gains for an ISP stream and then updates the metadata
   @param entry Entry pointer
   @param Sensor index (0 or 1)
   @param interruptTiming - Interrupt timing 
   @param incomingSensorState - Pointer to a structure containing sensor update commands
   @param Context - Sensor context
   @param sensor - Registered sensor handle
 * 
*/
ERRG_codeE METADATA_ISP_UPDATER_updateWBGains(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex,const METADATA_SENSOR_UPDATER_interruptNotification *interruptTiming, 
  INU_metadata_State *incomingSensorState, INU_DEFSG_sensorContextE  context,   METADATA_UPDATE_registeredSensor *sensor);
/* @brief Updates the exposure for an ISP stream and then updates the metadata
  @param entry Entry pointer
  @param Sensor index (0 or 1)
  @param interruptTiming - Interrupt timing 
  @param incomingSensorState - Pointer to a structure containing sensor update commands
  @param Context - Sensor context
  @param sensor - Registered sensor handle
* 
*/
ERRG_codeE METADATA_ISP_UPDATER_updateExpoParameters(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex,const METADATA_SENSOR_UPDATER_interruptNotification *interruptTiming, 
  INU_metadata_State *incomingSensorState, INU_DEFSG_sensorContextE  context,   METADATA_UPDATE_registeredSensor *sensor);
/**
 * @brief Updates the ISP exposre metadata in frame buffers after NFrames
 *
 * @param entry METADATA_UPDATER_channelHandleT to store the metadata with the new exposure value in
 * @param sensorIndex Sensor Index (0 or 1)
 * @param N  N (when to store the new exposure in)
 * @param expoParameters Exposure value
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE METADATA_ISP_UPDATER_updateExpoAfterNFrames(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex, const METADATA_SENSOR_UPDATER_frameTimingT N, 
    INU_metadata_ISP_ExpoParams *expoParameters, const bool updateAGain, const bool updateExposure, const bool updateDGain, const bool updateOtherParams);
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
    INU_metadata_ISP_WBPameters *wbParameters);
/**
 * @brief For the Helsinki project, we have to read Wb/Expo from the slave downscaled ISP( ISP0) and then update the metadata with this value
 * 
 * @param entry METADATA_UPDATER_channelHandleT to store the metadata 
 * @param sensorIndex Sensor Index (0 or 1)
*/
ERRG_codeE METADATA_ISP_UPDATER_live_WB_Expo_metadata(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex);

/*The copies for the ISP wb and exposure structs are expensive!! We should do it once instead of four times per frame!!!
Therefore we will only use buffer index 0 for ISP metadata, we have no need to prepare metadata for more than 1 frames at the moment

William Bright
*/
/**
 * @brief For the Helsinki project, we have to set the AE and AWB done callbacks
 * 
 * @param entry METADATA_UPDATER_channelHandleT to store the metadata 
 * @param sensorIndex Sensor Index (0 or 1)
*/
ERRG_codeE METADATA_ISP_UPDATER_set_isp_expo_callback(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex);
/**
 * @brief Returns how many delayed gain updates are in the queue 
 * @return Returns the size of the queue
 * */
int METADATA_ISP_UPDATER_getDelayedGainQueueSize(METADATA_UPDATER_channelHandleT *entry );
#ifdef __cplusplus
}
#endif