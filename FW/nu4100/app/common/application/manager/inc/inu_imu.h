#pragma once
#include "inu2.h"
#include "inu_function.h"
#include "inu_imu_data.h"
#define ACCELEROMETER_TIMESTAMP_INDEX 0
#define GYRO_TIMESTAMP_INDEX 1
#define ENABLE_WATERMARK
/*These structs were unhudden from within inu_imu.c*/
typedef struct
{
	inu_function                 function;

	void                        *privP;
}inu_imu;

typedef struct
{
	inu_function__CtorParams	functionCtorParams;
	float			 			      scale[INU_IMU_DATA__SENSOR_NUM_TYPES_E];
	float						      odr[INU_IMU_DATA__SENSOR_NUM_TYPES_E];
   unsigned int				   accelBw[INU_IMU_DATA__SENSOR_NUM_TYPES_E];
   unsigned int				   gyroBw[INU_IMU_DATA__SENSOR_NUM_TYPES_E];
   unsigned int				   batchSize[INU_IMU_DATA__SENSOR_NUM_TYPES_E];
	inu_imu_data__descriptorT	descriptor;
   unsigned int  IMU_paired_batching_batchSize; /*0 = Disabled Paired batching 
                                          >0  = Number of sample pairs in batch
                                          As an example setting this to 4 would result in the following sequence of samples in a batch a,g,a,g */
}inu_imu__CtorParams;
typedef struct inu_imu__privData
{
   inu_imu_data__descriptorT   descriptor;
   BOOLEAN                     active;
   BOOLEAN                     thrActive;
   inu_imu_data               *imu_data;
}inu_imu__privData;
#define PAIRED_BATCH_BUFFER_SIZE 16
typedef struct 
{
   char buffer[PAIRED_BATCH_BUFFER_SIZE];
   UINT64 timestamp;
   
} inu_IMU_pairedBatchEntry;
#define IMU_NUM_BUFFERS 40
typedef struct 
{
   inu_IMU_pairedBatchEntry buffers[IMU_NUM_BUFFERS];
   UINT32 head;            /*Where to write too*/
   UINT32 tail;            /*Where to read from*/
   OS_LYRG_mutexT mutex;   /*Mutex to lock when reading from the buffers*/
   size_t max_size;        /*Circular buffer size*/
   size_t current_size;    /*Number of entries that are valid*/
} inu_IMU_circularBuffer;
typedef struct 
{                     
   UINT64 lastIMUTimestamps[2];                                /*Index 0 = Last accelerometer sample in nanoseconds */
                                                               /*Index 1 = Lat gyro sample timestamp in nanoseconds */
                                                               /*By looking for the case when Index 0==Index 1  we can find out when a pair of IMU samples is ready for sending, otherwise we have to wait until we get both samples in sync*/
   MEM_POOLG_bufDescT         *bufP;                           /*This is where the batch is assembled into*/
   UINT32                     currentSizeOfBatchBytes;         /*This the current location of where to copy in the next set of samples into*/
   UINT32                     currentSizeOfBatch;              /*Number of IMU samples in the batch
                                                               As an example, if you had the following set of samples in BufP then you would have 4 IMU samples:
                                                               a,g,a,g*/   
   UINT32                     completeBatchSize;               /*How many IMU samples will be in a batch*/
   UINT32                     completeBatchSizeBytes;          /*How large the batch size will be in bytes*/
   UINT64                     batchDoneCounter;              /*Counter describing how many batches have been completed in total*/ 
   MEM_POOLG_handleT          dataPool;                
   inu_IMU_circularBuffer     gyroCircularBuffer;           /*Gyro circular buffer*/
   inu_IMU_circularBuffer     accelCircularBuffer;          /*Accelerometer circular buffer*/
   inu_imu_data__hdr_t        imuHdr;                       /*Holds the IMU data header*/
   bool                       serializationDone;            /*Flag to say if the Metadata has been prepared*/


} inu_IMU_pairedBatch;
typedef struct
{
   char                       device_name[10];
   char*                      trigger_name;
   INU_DEFSG_posSensorTypeE   sensorType;
   int                        buf_len;
   char*                      buf_dir_name;
   char*                      dev_dir_name;
   struct iio_channel_info    *channels;
   int                        num_channels;
   int                        scan_size;
   int                        fp;
   char                       *data;
   MEM_POOLG_handleT          dataPool;
   char                       *buffer_access;
   char                       *sampling_access;
   char                       *scaling_access;
   float                      tempScale;
   int                        tempOffset;
   UINT64                     frameCntr;
   UINT64                     numberBatches;        /*Numer of batches before sending data to the host*/
   UINT64                     currentSizeOfBatch;   /*Current size of batch*/
   MEM_POOLG_bufDescT         *bufP;
   UINT64                     startIMURTCBasedTimestamp; /*Start time of the IMU in nano seconds*/
   inu_IMU_pairedBatch        *batchGroup;                /*NULL = Not part of a batch group
                                                         Otherwise this struct is used to batch multiple IMU channels together into a single message
                                                         */
   inu_imu_data__hdr_t        *imuHdr;                     /*IMU header which is prepared only once*/
   bool                       imuHdrPrepared;             /*True if the IMU header has been prepared*/
} inu_imu__dbT;

typedef struct
{
   unsigned short scale;
   unsigned char  accelBw;
   unsigned char  gyroBw;
}inu_imu__scaleAndBwT;
#define CLIENT_SENSORG_BUF_LEN (100)
void inu_imu__vtable_init(void);
const inu_function__VTable *inu_imu__vtable_get(void);
void inu_imu__setDescriptor(inu_imuH imuH, inu_imu_data__descriptorT *desc);
void inu_imu__getDescriptor(inu_imuH imuH, inu_imu_data__descriptorT *desc);
ERRG_codeE inu_imu__gpGetTemperatoreIoctl(inu_imuH me, inu_imu__temperatureParams *getTempParamsP);
/**
 * @brief Serializes the Metadata for the IMU
 * 
 *
 * @param metadata Metadata pointer
 * @param rtcTime  RTC time
 * @param pairedBatching - True if using paired batching, False = Paired batching not used
 * @return Returns an error code
 */
ERRG_codeE inu_imu__serializeMetadata(char *buffer, size_t size, UINT64 startTimestamp_ns, bool pairedBatching);
