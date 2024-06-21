#ifndef _INU_IMU_DATA_API_H_
#define _INU_IMU_DATA_API_H_
#include "inu_metadata.h"
#include "inu2.h"

typedef void* inu_imu_dataH;

#define POS_DATA_MAX_NUM_OF_CHANNELS (7)


typedef enum
{
   INU_IMU_DATA__SENSOR_ACCELAROMETER_E = 0,
   INU_IMU_DATA__SENSOR_GYROSCOPE_E = 1,
   INU_IMU_DATA__SENSOR_MAGNETOMETER_E = 2,
   INU_IMU_DATA__SENSOR_NUM_TYPES_E
} inu_imu_data__posSensorTypeE;

typedef enum
{
   INU_IMU_DATA__SENSOR_ACCELAROMETER_X_E = 0,
   INU_IMU_DATA__SENSOR_ACCELAROMETER_Y_E = 1,
   INU_IMU_DATA__SENSOR_ACCELAROMETER_Z_E = 2,
   INU_IMU_DATA__SENSOR_GYROSCOPE_X_E = 3,
   INU_IMU_DATA__SENSOR_GYROSCOPE_Y_E = 4,
   INU_IMU_DATA__SENSOR_GYROSCOPE_Z_E = 5,
   INU_IMU_DATA__SENSOR_MAGNETOMETER_X_E = 6,
   INU_IMU_DATA__SENSOR_MAGNETOMETER_Y_E = 7,
   INU_IMU_DATA__SENSOR_MAGNETOMETER_Z_E = 8,
   INU_IMU_DATA__SENSOR_TIMESTAMP_E = 9,   
   INU_IMU_DATA__SENSOR_CHANNEL_NUM_TYPES_E
} inu_imu_data__posSensorChannelTypeE;

//channel in a sensor
typedef struct
{
   float scale;
   float offset;
   UINT32 bytes;
   UINT32 bits_used;
   UINT32 maskL;
   UINT32 maskH;
   UINT32 is_signed;
   UINT32 location;
   UINT32 be;
   UINT32 shift;
   inu_imu_data__posSensorChannelTypeE channelType;
} inu_imu_data__chDescriptorT;
//IMU
typedef struct
{
   inu_imu_data__chDescriptorT chDescriptors[INU_IMU_DATA__SENSOR_NUM_TYPES_E][INU_IMU_DATA__SENSOR_CHANNEL_NUM_TYPES_E];
   UINT16 num_channels[INU_IMU_DATA__SENSOR_NUM_TYPES_E];
   UINT32  shiftTs[INU_IMU_DATA__SENSOR_NUM_TYPES_E];
} inu_imu_data__descriptorT;
typedef struct 
{
   char buffer[INU_IMU_METADATA_SIZE];
} inu_imu_data__metadataT;
typedef struct
{
   inu_data__hdr_t   dataHdr;
   UINT32            sensorType;
   /*The batch optimization is explained below:
      Instead of sending a single set of X,Y,Z IMU readings we instead
      send batchCount number of X,Y,Z,Timestamp samples all in one USB message. 
      Where each X,Y,Z,Timestamp sample is of size batchSizeBytes.

      This means that in memory, the data looks like this:
      X[0],Y[0],Z[0],Timestamp[0],  X[0],Y[0],Z[0],Timestamp[0]... X[batchCount-1],Y[batchCount-1],Z[batchCount-1],Timestamp[batchCount-1]

      And the total size of the IMU payload is now batchCount*batchSizeBytes
   */
   UINT32            batchCount;       /*Number of IMU samples in this batch     */
   UINT32            batchSizeBytes;  /*Size of each X,Y,Z IMU sample in bytes             */
   inu_imu_data__metadataT metadata;
}inu_imu_data__hdr_t;

typedef struct
{
   inu_data__initParams       dataInitParams;
   inu_imu_data__descriptorT  descriptor;
}inu_imu_data__initParams;

typedef struct
{
   float imuVal;
   char  *name;
} inu_imu_data___sensorReadableData;

typedef struct
{
   inu_imu_data___sensorReadableData   imuData[INU_IMU_DATA__SENSOR_CHANNEL_NUM_TYPES_E-1];
   UINT64                              timeStamp;
} inu_imu_data__readableData;

ERRG_codeE inu_imu_data__convertData(inu_imu_dataH dataH, inu_imu_data__readableData *readableDataP );
/**
 * @brief A version of inu_imu_data__convertData that can accept IMU readings that are batched together
 * 
 *
 * @param dataH Data handle
 * @param readableDataP Output array to store multiple inu_imu_data__readableData items
 * @param readableDataPLength Length of readableDataP array
 * @param hdrP Pointer to the IMU header
 * @return Returns an INU error code
 */
ERRG_codeE inu_imu_data__convertDataMultiple(inu_imu_dataH dataH, inu_imu_data__readableData *readableDataP, UINT32 readableDataPLength, inu_imu_data__hdr_t *hdrP );
/**
 * @brief A version of inu_imu_data__convertData that can accept IMU readings that are batched together. With an added feature of converting RTC timestamps into nanoseconds
 * 
 *
 * @param dataH Data handle
 * @param readableDataP Output array to store multiple inu_imu_data__readableData items
 * @param readableDataPLength Length of readableDataP array
 * @param hdrP Pointer to the IMU header
 * @return Returns an INU error code
 */
ERRG_codeE inu_imu_data__convertDataMultipleWithTimestampConversion(inu_imu_dataH dataH, inu_imu_data__readableData *readableDataP, UINT32 readableDataPLength,inu_imu_data__hdr_t *hdrP );
/**
 * @brief A version of inu_imu_data__convertDataMultipleWithTimestampConversion that can accept IMU readings that are batched together using paired batching. Where samples are arranged like the following:
 *        Acccelerometer X,Y,Z,Timestamp, Gyro X,Y,Z Timestamp
 * 
 *
 * @param dataH Data handle
 * @param readableDataP Output array to store multiple inu_imu_data__readableData items
 * @param readableDataPLength Length of readableDataP array
 * @param hdrP Pointer to the IMU header
 * @return Returns an INU error code
 */
ERRG_codeE inu_imu_data__convertDataMultipleWithPairedBatchingAndTimestampConversion(inu_imu_dataH dataH, inu_imu_data__readableData *readableDataP, UINT32 readableDataPLength,inu_imu_data__hdr_t *hdrP );

#endif
