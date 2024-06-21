#pragma once
#include "err_defs.h"
#ifdef __cplusplus
extern "C" {
#endif


#define HELSINKI_USB_OPTIMIZATION
#define HELSINKI_CVA_SPECIAL_WRITER_ID 1
/*Strictly speaking we only need to add 1024 bytes but I am padding this out a bit more just incase it goes wrong*/
#define URB_TRANSFER_SIZE                        1024
#define HELSINKI_USB3_EXTRA_BUFFER_PADDING_SIZE 0 /*We don't need this anymore, its possible to send a single URB */
#define MAGNETOMETER_PADDING 32
#define HELSINKI_DISABLE_MULTI_INPUT_SUPPORT /*The Writer doesn't support multiple inputs so we need to return early, we're not using any software graph nodes that support multiple inputs anyway.*/
UINT32 HELSINKI_getURBSize(UINT8 endpointID);
#if DEFSG_IS_TARGET
#include "metadata_target_config.h"
#include "inu_defs.h"
#include "metadata_sensor_updater.h"
#define HELSINKI_DEFAULT_IMU_BATCH_SIZE 1
#define HELSINKI_USING_FSG_FOR_FTRIG_OUTPUTS
#define METADATA_DEBUG_LEVEL LOG_DEBUG_E
/*The SDK will use three different main graph names, main, main_slave and main_master, we need to be able to handle these names and not just "main" */
#define HELSINKI_MAIN_GRAPH_NAME_1 "main"
#define HELSINKI_MAIN_GRAPH_NAME_2 "main_slave"
#define HELSINKI_MAIN_GRAPH_NAME_3 "main_master"
#define HELSINKI_IIO_TIMESTAMP_LOCATON 8 /*For both the accelerometer and gyro, the timestamp is always located in bytes 8 ->15*/
#define ISP_MAIN_GRAPH_NAMES HELSINKI_MAIN_GRAPH_NAME_1,HELSINKI_MAIN_GRAPH_NAME_2,HELSINKI_MAIN_GRAPH_NAME_3
#define ENABLE_HANDLE_IMMEDIATE        /*Graph events are handled within the IMU thread*/
#define ENABLE_HANDLE_IMMEDIATE_SOC_CHANNELS   /*Graph events are handled within the interrupt thread*/
#define FRAME_PERIOD_MICROSECONDS_AF_VST (10989) /*AF VST frame rate is 91 Hz*/
#define HELSINKI_MAXIMUM_ISP_LATENCY FRAME_PERIOD_MICROSECONDS_AF_VST /*According to DIMA, the ISP shouldn't take longer than 6ms to process an image*/
#define HELSINKI_ENABLE_ISP_SYNCHRONIZER
typedef enum
{
   FrameCounter = 1,
   ISO = 2,
   Exposure = 3,
   Whitebalance_Gain = 4,
   WhiteBalance_Temperature = 5,
   EVCompensation = 6,
   FlickerCompensation = 7,
   Sharpness = 8,
   EdgeEnhancement = 9,
   NoiseReduction = 10,
   FrameRate = 11,
   CCM_In_use = 12,
   Timestamp = 13,
   AFCropPositon = 14,
   AFFocusPosition = 15,
   RGBHistogram = 16,
   ContrastValue = 17,
   ROI = 18,
   HELSINKI_NumSensorUpdateDataTypes
} HELSINKI_sensorUpdateDataTypes;
#define MAX_NUMBER_FRAME_TIMINGS HELSINKI_NumSensorUpdateDataTypes
typedef enum
{
   HELSINKI_MASTER_E = 0, /*UART0_SIN at startup */
   HELSINKI_SLAVE_E = 1,
   INU_DEFSG_NUM_OF_HARWDARE_TYPES
} HELSINKI_masterOrSlaveE;
typedef struct 
{
   METADATA_SENSOR_UPDATER_frameTimingT frameTime;                    /*How long it takes this data type to update */
   HELSINKI_sensorUpdateDataTypes dataT;                              /*Data type */
} HELSINKI_datatypeFrameTimingT;
typedef struct 
{
   INU_DEFSG_sensorModelE modelNumber;                                /*Model number of the sensor */
   HELSINKI_datatypeFrameTimingT frameTimings[MAX_NUMBER_FRAME_TIMINGS];/*Frame timings for each of the data types*/
}  HELSINKI_frameTimingEntryT;

/**
 * @brief Reads if we are a master or slave target and modifies I/O muxing
 * 
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE HELSINKI_init();
/**
 * @brief Returns if the hardware is a master
 * 
 * @return bool Returns true if the hardware is a master and false if the hardware is a slave
 */
bool HELSINKI_getMaster();
/**
 * @brief Gets the channel map for this system
 * 
 * @param map Double pointer to store the map pointer into
 * @param mapEntries Pointer to store the number of map entries in
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE HELSINKI_getChannelMap(const METADATA_TARGET_CONFIG_channelIDToSLUAndAXIReaderMap **map, UINT32 *mapEntries);
/**
 * @brief Gets the hardware type based upon if we're a master or slave
 * 
 * @return INU_metadata_HardwareType 
 */
INU_metadata_HardwareType HELSINKI_getHardwareType();
/**
 * @brief This deals with the special case of the VST AF sensors for the helsinki project.
 *
 *   1. The fps of VST AF sensors 832x608 is 75
 *
 */
void HELSINKI_changeVSTAFFsgCnt(int fps);
/**
 * @brief This deals with the special case where we need to append metadata to the CVA stream based upon the metadata contained with images sent into writer1
 * 
 * @param writerID Writer ID
 * @param frameIdValue Pointer which is updated with the frame ID value to use
 * @param timestampValue Pointer which is updated with the timestamp value to use
 * @param data Inu_Data pointer containing a frame with metadata
 * 
 * @return Returns an error code
 */
ERRG_codeE HELSINKI_prepareCVAMetadataFromWriterOperateContext(UINT32 writerID,UINT32 * frameIDValue,UINT64 *timestampValue, inu_data* data);
/**
 * @brief Checks if we need to use the writer in a particular way to meet the requirements of Helsinki
 * @param writerID Writer ID
 * @return Returns true if we need to treat the writer differently for the CVA used on Helsinki
 */
bool HELSINKI_checkForSpecialCVAScenario(UINT32 writerID);
/**
 * @brief Invalidates the cache for metadata within a frame buffer that's going to the CVA
 * @param buf Buffer to Buffer to invalidate the cache for
 * @param physicalAddress Set to a physical address of where to start the writer injection from
 * @return Returns an error code
 */
ERRG_codeE HELSINKI_specialCaseOffsetFrameBufferAndInvalidateCache(MEM_POOLG_bufDescT * buf, UINT32 *physicalAddress);
/**
 * @brief Returns the batch size for the IMU, this value is in terms of scans/batch. So if this value is set to 50 then you will have 50 sets of accelerometer readings per USB3 transfer
   @return Returns IMU batch size
*/
UINT32 HELSINKI_specialGetIMUBatchSize();
/**
 * @brief For the tracking and gaze sensors we need to make sure that we perform both the left and right exposure updates at the same time
 * @param sensorType Sensor type
 * @return Return true
 */
bool HELSINKI_getGroupedExposureUpdatesRequired(INU_metadata_SensorType sensorType);
/**
 * @brief For the Tracking and Gaze we will ignore the right exposure updates and only process left exposure updates
 * @param sensorType Sensor type
 * @param side Sensor side
 * @return Returns true if we should ignore the sensor update
 */
bool HELSINKI_ignoreExposureUpdate(INU_metadata_SensorType sensorType, INU_metadata_Channel_Side side);
/**
 * @brief If this limit is exceeded, its highly likely that the ISP has dropped a frame
 * 
 * @return UINT32 Returns the maximum ISP latency
 */
bool HELSINKI_getMaximumISPLatency(UINT32 latency_us);
void HELSINKI_enableToFTrigger(void);
  /*For the slave ISP0 stream, we will have per frame metadata updates so that we get WB and exposure values for each frame into the metadata!*/
bool HELSINKI_getEnableConstantMetadataUpdates(int READER);


#endif
#ifdef __cplusplus
}
#endif
