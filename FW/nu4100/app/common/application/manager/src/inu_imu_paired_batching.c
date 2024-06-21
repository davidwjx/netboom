#include "inu_metadata.h"
#include "inu_metadata_serializer.h"
#include "inu2_types.h"
#include "inu2_internal.h"
#include "inu_types.h"
#include "err_defs.h"
#include "internal_cmd.h"
#include "mem_pool.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inu_imu.h"
#include "nucfg.h"

#if DEFSG_IS_GP
   #include "gme_mngr.h"
   #include <errno.h>
   #include <ctype.h>
   #include <sys/utsname.h>
   #include "log.h"
   #include <assert.h>
   #include "helsinki.h"
   #include "inu_device_api.h"
   #include "metadata.pb.h"
   #include "rtc.h"
   #include "marshal.h"
#if DEFSG_IS_OS_LINUX
   #include "os_lyr.h"
   #include <unistd.h>
   #include <inttypes.h>
   #include "iio_utils.h"
   #include <poll.h>
   #include "cmem.h"
#endif
#endif
#define IMU_BATCHING_DEBUG_LEVEL LOG_DEBUG_E
/**
 * @brief Dequeues an item from the paired batching circular buffer
 * 
 *
 * @param circularBuffer Circular buffer containing Gyro or Accelerometer readings
 * @param item Value is stored in item
 * @return Returns 0 on success, -1 when the circular buffer is empty.
 */
static int inu_imu_paired_batching_circular_buffer_dequeue(inu_IMU_circularBuffer *circularBuffer,inu_IMU_pairedBatchEntry *item)
{
   int ret = 0;
   OS_LYRG_lockMutex(&circularBuffer->mutex);
   /*Removes an entry from the circular buffer if this is possible*/
   if(circularBuffer->head ==  circularBuffer->tail )
   {
      ret = -1;
   }
   else
   {
      unsigned int next_index = circularBuffer->tail + 1;
      if(next_index >=  circularBuffer->max_size)
      {
         next_index =0;
      }
      if(item)
         /*User can provide a NULL pointer and the Circular buffer entry will not be copied into item*/
         *item = circularBuffer->buffers[circularBuffer->tail];
      circularBuffer->tail = next_index;
      circularBuffer->current_size--;
   }
   
   OS_LYRG_unlockMutex(&circularBuffer->mutex);
   return ret;
}
/**
 * @brief Gets the size of a circular bufer
 * 
 *
 * @param circularBuffer Circular buffer containing Gyro or Accelerometer readings
 * @return Returns the size of the circular buffer
 */
static int inu_imu_paired_batching_circular_buffer_getSize(inu_IMU_circularBuffer *circularBuffer)
{

   OS_LYRG_lockMutex(&circularBuffer->mutex);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"Current size of the circular buffer is %lu\n",circularBuffer->current_size);
   int size = circularBuffer->current_size;
   OS_LYRG_unlockMutex(&circularBuffer->mutex);
   return size;
}

/**
 * @brief Enqueues an item from the paired batching circular buffer
 * 
 *
 * @param circularBuffer Circular buffer containing Gyro or Accelerometer readings
 * @param item Value is stored in item
 * @return Returns 0 on success, -1 when the circular buffer is empty.
 */
static int inu_imu_paired_batching_circular_buffer_enqueue(inu_IMU_circularBuffer *circularBuffer, inu_IMU_pairedBatchEntry *item)
{

   OS_LYRG_lockMutex(&circularBuffer->mutex);
   int ret = 0;
   /*Calculate the next index*/
   unsigned int next_index = circularBuffer->head + 1;
   /*Removes an entry from the circular buffer*/
   if(next_index >= circularBuffer->max_size)
   {
      next_index = 0;
   }
   if(next_index == circularBuffer->tail)
   {
      /*Circular buffer is full*/
      ret = -1;
   }
   else
   {
      /*Store item at the HEAD index*/
      circularBuffer->buffers[circularBuffer->head] = *item;
      circularBuffer->head = next_index;
      circularBuffer->current_size++;
   }
   OS_LYRG_unlockMutex(&circularBuffer->mutex);
   return ret;
}
/**
 * @brief Peeks an item from the paired batching circular buffer
 * 
 *
 * @param circularBuffer Circular buffer containing Gyro or Accelerometer readings
 * @param item Value is stored in item
 * @return Returns 0 on success, -1 when the circular buffer is empty.
 */
static int inu_imu_paired_batching_circular_buffer_peek(inu_IMU_circularBuffer *circularBuffer, inu_IMU_pairedBatchEntry *item)
{
   int size = inu_imu_paired_batching_circular_buffer_getSize(circularBuffer);
   OS_LYRG_lockMutex(&circularBuffer->mutex);
   int ret = 0;
   /*Calculate the next index*/
   if(size)
   {
      *item = circularBuffer->buffers[circularBuffer->tail];
   }
   else
   {
      ret = -1;
   }
   OS_LYRG_unlockMutex(&circularBuffer->mutex);
   return ret;
}
/**
 * @brief Please see  CInuDualSensors::RightToLeftIndexMatch(std::shared_ptr<const InuDev::CImageFrame> iFrame) for the inspiration to this
 * method of finding matches, it's what was used for the GAZE ROI interleaving
 * 
 *
 * @param circularBuffer Circular buffer
 * @param item Item to look for matching pairs with
 * @return Return an error code
 */
static int inu_imu__pairedBatching_lookForMatchingPairs( inu_IMU_circularBuffer *circularBuffer, inu_IMU_pairedBatchEntry *item,inu_IMU_pairedBatch *pairedBatch,
   inu_imu *me,  inu_imu__privData   *privP, inu_imu__dbT * inu_imu__db, inu_IMU_pairedBatchEntry *matchedOutput)
{
   int ret = -1;
   int size = inu_imu_paired_batching_circular_buffer_getSize(circularBuffer);
   int i =0;
   while(i < size)
   {
      LOGG_PRINT(LOG_DEBUG_E,NULL,"Looking for matches in our circular buffer with size  %lu, at position %lu\n",size,i);

      inu_imu_paired_batching_circular_buffer_peek(circularBuffer,matchedOutput );
      LOGG_PRINT(LOG_DEBUG_E,NULL,"input ts : %llu, circular buffer: %llu \n",item->timestamp, matchedOutput->timestamp );
      if(matchedOutput->timestamp < item->timestamp)
      {
         LOGG_PRINT(LOG_DEBUG_E,NULL,"Discarding entry from circular buffer %p, %llu, %llu \n", circularBuffer, matchedOutput->timestamp, item->timestamp);
         inu_imu_paired_batching_circular_buffer_dequeue(circularBuffer,matchedOutput);
      }
      else if(matchedOutput->timestamp == item->timestamp)
      {
         LOGG_PRINT(LOG_DEBUG_E,NULL,"Found match: %p with timestamp: %llu \n", circularBuffer, item->timestamp);
         inu_imu_paired_batching_circular_buffer_dequeue(circularBuffer,NULL);
         ret = 0;
         break;
      }
      else{
         LOGG_PRINT(LOG_DEBUG_E,NULL,"No match found \n");
         ret = -1;
         break;
      }
      i++;
   }
   return ret;
}


 /**
  * @brief Initializes the circular buffer structure
  * @param circularBuffer Circular buffer pointer
  */
 static void inu_imu__pairedBatching_initCircularBuffer(inu_IMU_circularBuffer *circularBuffer)
 {

   /*We have our own circular buffer because we need to be able to look at a queue entry and not remove it from the queue
   I couldn't find a way of */
   circularBuffer->head =0;
   circularBuffer->tail =0;
   OS_LYRG_aquireMutex(&circularBuffer->mutex);
   memset(circularBuffer->buffers, 0, sizeof(circularBuffer->buffers));
   circularBuffer->max_size = IMU_NUM_BUFFERS;
   circularBuffer->current_size = 0 ;
 }

 ERRG_codeE inu_imu__pairedBatching_init(inu_IMU_pairedBatch * pairedBatch,UINT32 numberOfPairedSamples, UINT32 accelerometer_buffer_size, UINT32 gyro_buffer_size)
{

    UINT32 bufferSize = numberOfPairedSamples*(accelerometer_buffer_size+gyro_buffer_size) + 16; /* Leave last 16 bytes free so that we can have some space left in the buffers if buffer overflows happen!*/
     /*Initializes a group batching structure*/
   pairedBatch->lastIMUTimestamps[ACCELEROMETER_TIMESTAMP_INDEX] = 0;
   pairedBatch->lastIMUTimestamps[GYRO_TIMESTAMP_INDEX] = 0;
   pairedBatch->currentSizeOfBatchBytes = 0;
   pairedBatch->batchDoneCounter = 0;
   pairedBatch->currentSizeOfBatch = 0;
   pairedBatch->completeBatchSize = numberOfPairedSamples; /*Set how many IMU samples will be in a complete batch*/
   pairedBatch->completeBatchSizeBytes = bufferSize;
   pairedBatch->serializationDone = false;
   MEM_POOLG_handleT poolH;
   MEM_POOLG_cfgT cfg;
  
   cfg.bufferSize = bufferSize;
   cfg.numBuffers = 40;
   cfg.resetBufPtrInAlloc = 0;
   cfg.freeCb = NULL;
   cfg.freeArg = NULL;
   cfg.type = MEM_POOLG_TYPE_ALLOC_CMEM_E;
   cfg.memP = NULL;

   ERRG_codeE ret = MEM_POOLG_initPool(&poolH, &cfg);

   inu_imu__pairedBatching_initCircularBuffer(&pairedBatch->gyroCircularBuffer);
   inu_imu__pairedBatching_initCircularBuffer(&pairedBatch->accelCircularBuffer);

   if (ERRG_FAILED(ret))
      {

         LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to allocated pair IMU batching data pool \n");
         return ret;
      }
   pairedBatch->dataPool = poolH;

   LOGG_PRINT(IMU_BATCHING_DEBUG_LEVEL,NULL,"Preallocating the first IMU paired batch buffer to be size %lu  \n",bufferSize );
   MEM_POOLG_alloc(pairedBatch->dataPool,bufferSize,&pairedBatch->bufP);
   
   LOGG_PRINT(IMU_BATCHING_DEBUG_LEVEL,NULL,"Paired batching enabled with a complete batch size of %lu and %lu bytes in total \n",numberOfPairedSamples, pairedBatch->completeBatchSizeBytes );
    return INU_IMU__RET_SUCCESS;
}

static ERRG_codeE inu_imu_pairedBatchComplete(inu_IMU_pairedBatch *pairedBatch,inu_imu *me,  inu_imu__privData   *privP, inu_imu__dbT * inu_imu__db )
{
   inu_imu_data        *imu_data;

   MEM_POOLG_bufDescT *bufP = pairedBatch->bufP;
   inu_imu_data__hdr_t *imuHdr = &pairedBatch->imuHdr;
   imuHdr->dataHdr.dataIndex = pairedBatch->batchDoneCounter;
   imuHdr->sensorType        = 0;
   imuHdr->dataHdr.flags     = 64;//signal imu type
   imuHdr->batchCount =     pairedBatch->currentSizeOfBatch;  /*Update how many sets of readings we have in this message */
   imuHdr->batchSizeBytes = inu_imu__db[0].scan_size;         /*Size of each set of X,Y,Z,Timestamp samples in bytes*/                      
   pairedBatch->batchDoneCounter++;


   LOGG_PRINT(IMU_BATCHING_DEBUG_LEVEL, NULL, "Paired batch complete \n");
   /*We now have a complete batch that we can send*/
   CMEM_cacheWbInv(bufP->dataP, bufP->dataLen);
   UINT64 startIMUTimestamp = 0;
   /*Serialize the metadata buffer*/
   if (inu_imu__db[0].startIMURTCBasedTimestamp != 0) /* Use the Accelerometer start time as the start time */
   {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "IMU Start Timestamp is %llu \n", inu_imu__db[0].startIMURTCBasedTimestamp);
         startIMUTimestamp = inu_imu__db[0].startIMURTCBasedTimestamp; /*We must always use the starting time of the acceleromter as the IMU start timestamp*/
   }
   else
   {
         LOGG_PRINT(LOG_ERROR_E, NULL, "IMU Start Timestamp is %llu \n", inu_imu__db[0].startIMURTCBasedTimestamp);
         startIMUTimestamp = 0; /*This shouldn't happen, there must be an issue so we should send the start timestamp as 0*/
   }
   if(!pairedBatch->serializationDone)
   {
      /**We only need to serialize the header metadata once*/
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Paired batch header is now prepared  \n");
      ERRG_codeE metadata_ret = inu_imu__serializeMetadata(imuHdr->metadata.buffer, sizeof(imuHdr->metadata.buffer), startIMUTimestamp,true);
      if (ERRG_FAILED(metadata_ret))
      {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to serialize IMU metadata \n");
      }
      pairedBatch->serializationDone = true;
   }
   LOGG_PRINT(IMU_BATCHING_DEBUG_LEVEL, NULL, "Paired batch complete for IMU with size %lu  \n", bufP->dataLen);
   ERRG_codeE ret = inu_function__newData((inu_function *)me, (inu_data *)privP->imu_data, bufP, imuHdr, (inu_data **)&imu_data);
   if (ERRG_SUCCEEDED(ret))
   {

      #ifdef ENABLE_HANDLE_IMMEDIATE
      inu_function__doneData_handleImmediately((inu_function*)me, (inu_data *)imu_data);
      #else
      inu_function__doneData((inu_function*)me, (inu_data *)imu_data);
      #endif
   }

    /*Allocate the next buffer*/

    LOGG_PRINT(IMU_BATCHING_DEBUG_LEVEL, NULL, "Allocating next buffer of size %lu  \n",pairedBatch->completeBatchSizeBytes);
   MEM_POOLG_alloc(pairedBatch->dataPool,pairedBatch->completeBatchSizeBytes,&pairedBatch->bufP);
   bufP = pairedBatch->bufP;
   bufP->dataLen = 0;
   if (!privP->active)
   {
      privP->thrActive = FALSE;
      return 0;
   }
   pairedBatch->currentSizeOfBatch = 0;
   return INU_IMU__RET_SUCCESS;

}

static void inu_imu_pairedBatchMatch(inu_IMU_pairedBatch *pairedBatch,inu_IMU_pairedBatchEntry *accelerometer, inu_IMU_pairedBatchEntry *gyro,
 inu_imu *me,  inu_imu__privData   *privP, inu_imu__dbT * inu_imu__db)
{
   MEM_POOLG_bufDescT *bufP = pairedBatch->bufP;
   if(accelerometer->timestamp == gyro->timestamp)
   {
      
      LOGG_PRINT(IMU_BATCHING_DEBUG_LEVEL,NULL,"buffer 0 length %lu, buffer 1 length %lu \n",inu_imu__db[0].bufP->dataLen,inu_imu__db[1].bufP->dataLen );
      
      memcpy(&bufP->dataP[bufP->dataLen], accelerometer->buffer , sizeof(accelerometer->buffer));
      bufP->dataLen+=sizeof(accelerometer->buffer);
      memcpy(&bufP->dataP[bufP->dataLen], gyro->buffer , sizeof(gyro->buffer));
      bufP->dataLen+=sizeof(gyro->buffer);
      pairedBatch->currentSizeOfBatch = pairedBatch->currentSizeOfBatch + 2;

      LOGG_PRINT(IMU_BATCHING_DEBUG_LEVEL,NULL,"Current batching size %lu/%lu \n", pairedBatch->currentSizeOfBatch, pairedBatch->completeBatchSize );
      if(pairedBatch->currentSizeOfBatch >= pairedBatch->completeBatchSize   )
      {
         inu_imu_pairedBatchComplete(pairedBatch,me,privP,inu_imu__db);
      }

      LOGG_PRINT(LOG_DEBUG_E,NULL,"Timestamp match found %llu \n", accelerometer->timestamp   );
   }
}

static ERRG_codeE inu_imu_addIMUSampleToPairedBatch( inu_IMU_pairedBatch *pairedBatch, UINT32 sensorIndex, UINT32 readSize, inu_imu *me,  inu_imu__privData   *privP, inu_imu__dbT * inu_imu__db)
{

   /*Paired batch shared buffer */
   inu_IMU_pairedBatchEntry matchedOutput;
   MEM_POOLG_bufDescT *bufP = pairedBatch->bufP;

   MARSHALG_u64(MARSHALG_UNDO,&pairedBatch->lastIMUTimestamps[ACCELEROMETER_TIMESTAMP_INDEX],&inu_imu__db[ACCELEROMETER_TIMESTAMP_INDEX].bufP->dataP[HELSINKI_IIO_TIMESTAMP_LOCATON]);
   MARSHALG_u64(MARSHALG_UNDO,&pairedBatch->lastIMUTimestamps[GYRO_TIMESTAMP_INDEX],&inu_imu__db[GYRO_TIMESTAMP_INDEX].bufP->dataP[HELSINKI_IIO_TIMESTAMP_LOCATON]);
  
   LOGG_PRINT(LOG_DEBUG_E,NULL,"Accelerometer timestamp %llu, Gyro timestamp %llu \n",pairedBatch->lastIMUTimestamps[ACCELEROMETER_TIMESTAMP_INDEX],pairedBatch->lastIMUTimestamps[GYRO_TIMESTAMP_INDEX]);

   assert(inu_imu__db[sensorIndex].bufP->dataLen == 16);
   /*This memcpy can probably be removed as an optimization*/

   /*Look for matching pairs in the OTHER circular buffers */
   if(sensorIndex == ACCELEROMETER_TIMESTAMP_INDEX)
  {
      inu_IMU_pairedBatchEntry accelReading;
      memcpy(accelReading.buffer,inu_imu__db[sensorIndex].bufP->dataP,inu_imu__db[sensorIndex].bufP->dataLen);
      accelReading.timestamp = pairedBatch->lastIMUTimestamps[ACCELEROMETER_TIMESTAMP_INDEX];
      int ret = inu_imu__pairedBatching_lookForMatchingPairs(&pairedBatch->gyroCircularBuffer,&accelReading,pairedBatch,me,privP,inu_imu__db, &matchedOutput);
      if(ret)
         inu_imu_paired_batching_circular_buffer_enqueue(&pairedBatch->accelCircularBuffer,&accelReading);
      else
         inu_imu_pairedBatchMatch(pairedBatch,&accelReading,&matchedOutput,me,privP,inu_imu__db);
  }
  else if(sensorIndex == GYRO_TIMESTAMP_INDEX)
  {
      inu_IMU_pairedBatchEntry gyroReadng;
      memcpy(gyroReadng.buffer,inu_imu__db[sensorIndex].bufP->dataP,inu_imu__db[sensorIndex].bufP->dataLen);
      gyroReadng.timestamp = pairedBatch->lastIMUTimestamps[GYRO_TIMESTAMP_INDEX];
      int ret = inu_imu__pairedBatching_lookForMatchingPairs(&pairedBatch->accelCircularBuffer,&gyroReadng,pairedBatch,me,privP,inu_imu__db,&matchedOutput);
      if(ret)
         inu_imu_paired_batching_circular_buffer_enqueue(&pairedBatch->gyroCircularBuffer,&gyroReadng);
      else
         inu_imu_pairedBatchMatch(pairedBatch,&matchedOutput,&gyroReadng,me,privP,inu_imu__db);
  }
  return INU_IMU__RET_SUCCESS;
}
 ERRG_codeE inu_imu___pairedBatching_processPairedSamples(inu_IMU_pairedBatch *pairedBatch, UINT32 sensorIndex, UINT32 readSize, 
 unsigned int i, inu_imu *me,  inu_imu__privData   *privP, inu_imu__dbT * inu_imu__db)
{
   /*Treat as a normal buffer up until the point where the batch has been completed*/
   if(sensorIndex > 1)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Processing paired samples is only supported for the Gyro and accelerometer \n");
      return INU_IMU__ERR_INVALID_ARGS;
   }

   MEM_POOLG_bufDescT *bufP = inu_imu__db[sensorIndex].bufP;
   /*Allocate a buffer which is sized to be scan_size * number of batches */
   if (bufP == NULL)
   {
         printf("IMU DRAIN!\n");
         return INU_IMU__ERR_INVALID_ARGS;
   }
   UINT32 batch_size_offset = 0;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "Paired Batch size offset %lu, batch number %llu,Scan size %d \n", batch_size_offset, inu_imu__db[sensorIndex].currentSizeOfBatch, inu_imu__db[sensorIndex].scan_size);
   /*Assert that the offset+ scan size is less than the size of the buffer allocated to prevent nasty bugs from buffer overflows */
   /*Copy the new IMU data into dataP */
   memcpy(&bufP->dataP[batch_size_offset], inu_imu__db[sensorIndex].data + inu_imu__db[sensorIndex].scan_size * i, inu_imu__db[sensorIndex].scan_size);
   bufP->dataLen = inu_imu__db[sensorIndex].scan_size;                                       /*Increase data length of batch buffer by 1x inu_imu_data___sensorReadableData*/
   inu_imu_addIMUSampleToPairedBatch(pairedBatch, sensorIndex, readSize,me,privP,inu_imu__db);
   return INU_IMU__RET_SUCCESS;
}