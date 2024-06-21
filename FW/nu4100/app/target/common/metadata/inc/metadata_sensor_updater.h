#pragma once
#include "cde_drv_new.h"
#include "inu_metadata.h"
#include "inu_common.h"
#include "err_defs.h"
#include "mem_map.h"
#include "metadata_updater.h"
#ifdef __cplusplus
extern "C" {
#endif
#define METADATA_SENSOR_UPDATER_N_1 0 // N + 1
#define METADATA_SENSOR_UPDATER_N_2 1 // N + 2
#define METADATA_SENSOR_UPDATER_N_3 2 // N + 2
#define METADATA_SENSOR_UPDATER_SLU_REPEAT_MASK_INDEX 8

/**
 * @brief Updates a sensors parameters [EXPOSURE, etc] if there's data in the queue
 * 
 * @param sensor Pointer to a METADATA Updater entry
 * @param sensorIndex Sensor Index (0 or 1) which is used to access 
 * @param interruptTiming Timing information of the SOF interrupt
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE METADATA_SENSOR_UPDATER_performSensorUpdate(METADATA_UPDATER_channelHandleT *entry, const UINT32 sensorIndex,const METADATA_SENSOR_UPDATER_interruptNotification *interruptTiming);
/**
 * @brief Updates a timing information entry
 *
 * @param mapEntry METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap pointer to read the frame ID from
 * @param timingInformation Pointer to store timing information
 * @param hasFlag Nanopb has flag which will be set to true
 * @return ERRG_codeE Returns an error code
 */
 ERRG_codeE METADATA_SENSOR_UPATER_updateTimingInformation(METADATA_UPDATER_channelHandleT *mapEntry, INU_metadata_Debug_Timing_Information *timingInformation, bool *hasFlag);

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
 ERRG_codeE METADATA_SENSOR_UPDATER_updateROI(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex, UINT8 frameIndex, UINT32 xStart, UINT32 yStart);
 /**
 * @brief Updates the sensor side and sensor ID metadata in frame buffers after NFrames
 *
 * @param entry METADATA_UPDATER_channelHandleT to store the metadata with the new exposure value in
 * @param sensorIndex Sensor Index (0 or 1)
 * @param N  N (How many frames to delay the storing of the sensor ID and side)
 * @param side Sensor side 
 * @param sensorID sensor ID
 * @param hwType HW Type
 * @param writerOperateContext True if called from a writer operate context, false it's called from a SOF/EOF context.
 * @return ERRG_codeE Returns an error code
 */
/**
 * @brief Updates the sensor side and HWType after NFrames
 *
 * @param entry METADATA_UPDATER_channelHandleT to store the metadata with the new exposure value in
 * @param sensorIndex Sensor Index (0 or 1)
 * @param N  N (when to store the new exposure in)
 * @param exposure Exposure value
 * @param sensor Sensor  
 * @param hwType Hardware type
 * @param writerOperateContext True if called from a writer operate context, false it's called from a SOF/EOF context.
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE METADATA_SENSOR_UPDATER_updateSensorInfoAndHWType(METADATA_UPDATER_channelHandleT *entry, UINT32 sensorIndex, const METADATA_SENSOR_UPDATER_frameTimingT N, 
const INU_metadata_Sensor sensor, INU_metadata_HardwareType hwType, const bool writerOperateContext);
/**
 * @brief Converts METADATA_SENSOR_UPDATER_frameTimingT into a frame buffer index
 *
 * @param frameIndex METADATA_SENSOR_UPDATER_frameTimingT to convert
 * @return INT32 Returns an INT32 that is used to calculate which frame buffer to update
 */
INT32 METADATA_SENSOR_UPDATER_calculateFramesAfterStartIndex(METADATA_SENSOR_UPDATER_frameTimingT frameIndex);
/**
 * @brief Updates the sync debug information
 * 
 * @param src - Source to get the command ID from
 * @param dst - Destination to store the command ID 
*/
ERRG_codeE METADATA_SENSOR_UPATER_updateSyncInformation(INU_metadata_Command_Sync *src,INU_metadata_Command_Sync* dst, bool * has_flag);
/**
 * @brief Handles the AR2020 delayed gain update
 * 
*/
ERRG_codeE  METADATA_SENSOR_UPDATE_handleAR2020DelayedGainUpdate(METADATA_UPDATER_channelHandleT *entry);
#ifdef __cplusplus
}
#endif
