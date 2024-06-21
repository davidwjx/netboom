#pragma once
#include "inu_imu.h"
/*Paired batching explanation is below:
In the standard batching mode, Gyro and Accelerometer samples are sent individually over USB3 like below where X is the accelerometer batch size and Y is the gyro batch size :
USB_Packet 0: Accelerometer sample 0, accelerometer sample 1.. Accelerometer sample X
USB_Packet 1: Gyro sample 0, Gyro sample 1 ... Gyro sample Y

This means that when the host unbatches the samples, the timestamps received in the IMU callback will not be consecutive because Accelerometer sample X will have a newer timestamp the Gyro sample 0

In the paired batching mode, Gyro and Accelerometer samples are paired together before sending, so you always end up with consecutive timestamps. Z is stored internally within inu_IMU_pairedBatch->completeBatchSize and is derived from the SW XML

USB_Packet 0: Accelerometer sample 0, Gyro sample 0, Accelerometer sample 1, Gyro Sample 1 , Accelerometer sample Z, Gyro sample Z
*/
#if DEFSG_IS_GP
/**
 * @brief Initializes the paired IMU batching data pool
 * 
 *
 * @param pairedBatch Pointer to the paired batch structured
 * @param numberOfPairedSamples Number of samples to acquire before a paired batch is completed
 * @param accelerometer_buffer_size Buffer size of accelerometer IIO data
 * @param gyro_buffer_size Buffer size of gyro IIO data
 * @return Returns an error code
 */
 ERRG_codeE inu_imu__pairedBatching_init(inu_IMU_pairedBatch * pairedBatch,UINT32 numberOfPairedSamples, UINT32 accelerometer_buffer_size, UINT32 gyro_buffer_size);
 /**
  * @brief Processes a new set of X,Y,Z,Timestamp samples from the IMU (This can either be the gyro or accelerometer)
  * @param pairedBatch Paired batching handle
  * @param sensorIndex (0 = Accelerometer ,1 = Gyro)
  * @param readSize Size of data read from the IIO buffer
  * @param i Buffer Index 
  * @param me inu_imu Handle
  * @param privP inu_imu__privData structure pointer
  * @param inu_imu__db Database of INU IMU entries
  * @return Returns an error code
  */
 ERRG_codeE inu_imu___pairedBatching_processPairedSamples(inu_IMU_pairedBatch *pairedBatch, UINT32 sensorIndex, UINT32 readSize, 
 unsigned int i, inu_imu *me,  inu_imu__privData   *privP, inu_imu__dbT * inu_imu__db);

#endif