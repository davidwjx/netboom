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
   #include "inu_imu_paired_batching.h"
#if DEFSG_IS_OS_LINUX
   #include "os_lyr.h"
   #include <unistd.h>
   #include <inttypes.h>
   #include "iio_utils.h"
   #include <poll.h>
   #include "cmem.h"
#endif
#endif


static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_IMU";


#if DEFSG_IS_GP
static OS_LYRG_threadHandle   inu_imu__listenerThrdH = NULL;

#define ACCELEROMETER_TIMESTAMP_INDEX 0
#define GYRO_TIMESTAMP_INDEX 1


static inu_IMU_pairedBatch Helsinki_Gyro_Accelerometer_Paired_Batching; /*Group batching handle for the Gyro and accelerometer*/
/* As a target CPU usage optimization we now have pre-prepared IMU headers that contain the metadata needed for the IMU 
where the metadata is only serialzied once after the first sample has been measured*/
inu_imu_data__hdr_t accelerometerIMUHeader;
inu_imu_data__hdr_t gyroIMUHeader;
inu_imu_data__hdr_t magnetometerHeader;

inu_imu__dbT      inu_imu__db[INU_IMU_DATA__SENSOR_NUM_TYPES_E] =
                  {
                     {"lsm6dsr_a",NULL,INU_IMU_DATA__SENSOR_ACCELAROMETER_E,CLIENT_SENSORG_BUF_LEN,NULL,NULL,NULL,0,0,-1,NULL,0,NULL,"sampling_frequency","in_accel_x_scale",0,0,0,HELSINKI_DEFAULT_IMU_BATCH_SIZE,0,NULL,0,&Helsinki_Gyro_Accelerometer_Paired_Batching,&accelerometerIMUHeader,FALSE},
                     {"lsm6dsr_g",NULL,INU_IMU_DATA__SENSOR_GYROSCOPE_E,CLIENT_SENSORG_BUF_LEN,NULL,NULL,NULL,0,0,-1,NULL,0,NULL,"sampling_frequency","in_anglvel_x_scale",0,0,0,HELSINKI_DEFAULT_IMU_BATCH_SIZE,0,NULL,0,&Helsinki_Gyro_Accelerometer_Paired_Batching,&gyroIMUHeader,FALSE},
                     {"lis2mdl",NULL,INU_IMU_DATA__SENSOR_MAGNETOMETER_E,CLIENT_SENSORG_BUF_LEN,NULL,NULL,NULL,0,0,-1,NULL,0,NULL,"sampling_frequency","in_magn_x_scale",0,0,0,1,0,NULL,0,NULL,&magnetometerHeader,FALSE},
                  };

struct pollfd fds[INU_IMU_DATA__SENSOR_NUM_TYPES_E];

bool inu_mag_init = false;

/****************************************************************************
*
*  Function Name: inu_imu__initialize
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static void inu_imu__initialize()
{
   int i;
   INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();

   if (modelType == INU_DEFSG_BOOT310_E)
   {
      strncpy(inu_imu__db[INU_IMU_DATA__SENSOR_ACCELAROMETER_E].device_name, "bma2x2", sizeof("bma2x2"));
   }

   for (i = 0; i < INU_IMU_DATA__SENSOR_NUM_TYPES_E; i++)
   {
      inu_imu__db[i].fp = -1;
   }
}

#if 0
TODO
/****************************************************************************
*
*  Function Name: inu_imu__gpGetTemperature
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Pos_sensors service
*
****************************************************************************/
static void inu_imu__gpGetTemperature(INU_CMDG_posTemperatureT *posTemperatureP)
{
   UINT32                              sensorIndex;
   int                                 temperatureRaw;

   for (sensorIndex = 0; sensorIndex < INU_IMU_DATA__SENSOR_NUM_TYPES_E; sensorIndex++)
   {
      if (inu_imu__db[sensorIndex].dev_dir_name)
      {
         temperatureRaw = read_sysfs_posint("in_temp_raw",inu_imu__db[sensorIndex].dev_dir_name);
         posTemperatureP->temperature = ((float)temperatureRaw * inu_imu__db[sensorIndex].tempScale) + inu_imu__db[sensorIndex].tempOffset;
         //printf("posTemperatureP->temperature = %f ((%f * %f) + %d\n",posTemperatureP->temperature,(float)temperatureRaw,inu_imu__db[sensorIndex].tempScale,inu_imu__db[sensorIndex].tempOffset);
         return;
      }
   }

   return;
}
#endif

/****************************************************************************
*
*  Function Name: inu_imu__size_from_channelarray
*
*  Description: calculate the storage size of a scan
*
*  Inputs:@channels:    the channel info array
*            @num_channels:   number of channels
*
*  Outputs:
*
*  Returns:
*
*  Context: Pos_sensors service
*
****************************************************************************/
int inu_imu__size_from_channelarray(struct iio_channel_info *channels, INT32 num_channels)
{
   INT32 bytes = 0;
   INT32 i = 0;
   while (i < num_channels)
   {
      if (bytes % channels[i].bytes == 0)
         channels[i].location = bytes;
      else
         channels[i].location = bytes - bytes%channels[i].bytes + channels[i].bytes;
      bytes = channels[i].location + channels[i].bytes;
      i++;
   }
   return bytes;
}


/****************************************************************************
*
*  Function Name: inu_imu__setAttributeSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Pos_sensors service
*
****************************************************************************/
int inu_imu__setAttributeSensor(char *access, char *dev_dir_name, float *input)
{
   int ret = 0;
   float result = 0;
   char str[20];

   if (access)
   {
      if (sprintf(str, "%f", *input) < 0)
      {
         printf( "Failed to write %f to string\n",*input);
      }
      else
      {
         ret = write_sysfs_string(access, dev_dir_name, str);
         read_sysfs_float(access, dev_dir_name,&result);

         //printf("input:%f. read result of %s/%s: %f\n",*input,dev_dir_name,access,result);
         *input = result;
      }
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: inu_imu__setBwAttributeSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Pos_sensors service
*
****************************************************************************/
int inu_imu__setBwAttributeSensor(char *access, char *dev_dir_name, float *input,unsigned int accelBw,unsigned int gyroBw)
{
   int ret = -1;
   float result = 0;
   char str[20];
   inu_imu__scaleAndBwT scaleAndBw;

   scaleAndBw.scale = (unsigned short )*input;
   scaleAndBw.accelBw = accelBw;
   scaleAndBw.gyroBw =  gyroBw;

   if (access)
   {
      if (sprintf(str, "%d",*((unsigned int *)&scaleAndBw)) < 0)
      {
        printf( "Failed to write %d to string\n",*((unsigned int *)&scaleAndBw));
      }
      else
      {
        ret = write_sysfs_string(access, dev_dir_name, str);
        read_sysfs_float(access, dev_dir_name,&result);

        //printf("input:%f. read result of %s/%s: %f\n",*input,dev_dir_name,access,result);
       *input = result;
      }
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: inu_imu__initSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Pos_sensors service
*
****************************************************************************/
int inu_imu__initSensor(int sensor, struct pollfd *fds, inu_imu__CtorParams *posStartParams)
{
   int dev_num, trig_num, channel;
   int ret = 0,searchNextDev=0;

   if (inu_imu__db[sensor].device_name== NULL)
      return -1;

   /* Find the device requested */
   dev_num = find_type_by_name(inu_imu__db[sensor].device_name, "iio:device");
   if (dev_num < 0)
   {
      //printf( "Failed to find the %s\n", inu_imu__db[sensor].device_name);
      ret = -ENODEV;
      return ret;
   }

   printf("=============== Found device %s, device id = %d \n", inu_imu__db[sensor].device_name, dev_num);

   //printf("iio device number being used is %d\n", dev_num);

   asprintf(&inu_imu__db[sensor].dev_dir_name, "%siio:device%d", iio_dir, dev_num);

   /* Enable all sensor channels */
   ret = enable_disable_all_channels(inu_imu__db[sensor].dev_dir_name,0, 1);
   if (ret) {
      printf( "Problem enabling channel %d\n", sensor);
      return ret;
   }

   // read_sysfs_float("in_temp_scale", inu_imu__db[sensor].dev_dir_name,&inu_imu__db[sensor].tempScale);
   // inu_imu__db[sensor].tempOffset = read_sysfs_posint("in_temp_offset", inu_imu__db[sensor].dev_dir_name);

   // Set the tempScale and tempOffset attributes
   printf("=============== Setting tempScale and tempOffset attributes %d %s \n", sensor, inu_imu__db[sensor].dev_dir_name);
   read_sysfs_float("in_temp_scale", "/sys/bus/iio/devices/iio:device2/", &inu_imu__db[sensor].tempScale);
   inu_imu__db[sensor].tempOffset = read_sysfs_posint("in_temp_offset", "/sys/bus/iio/devices/iio:device2/");

   printf("=============== temp_scale  = %f, temp_offset = %d \n", inu_imu__db[sensor].tempScale, inu_imu__db[sensor].tempOffset);
 
   if (inu_imu__db[sensor].trigger_name == NULL)
   {
      searchNextDev=1;
      /*
       * Build the trigger name. If it is device associated its
       * name is <device_name>_dev[n] where n matches the device
       * number found above
       */
      // temporary change until upgrading to new kernel 5.x

      ret = asprintf(&inu_imu__db[sensor].trigger_name, "%s-dev%d", inu_imu__db[sensor].device_name, dev_num);
      if (ret < 0)
      {
         ret = -ENOMEM;
         return ret;
      }
   }

   // /* Verify the trigger exists */
   // trig_num = find_type_by_name(inu_imu__db[sensor].trigger_name, "trigger");
   // if (trig_num < 0)
   // {
   //    if (searchNextDev)
   //    {
   //       sprintf(inu_imu__db[sensor].trigger_name, "%s-dev%d", inu_imu__db[sensor].device_name, dev_num + 1);
   //       trig_num = find_type_by_name(inu_imu__db[sensor].trigger_name, "trigger");
   //     }
   // }
   // if (trig_num < 0)
   // {
   //    printf( "Failed to find the trigger %s\n", inu_imu__db[sensor].trigger_name);
   //    ret = -ENODEV;
   //    goto error_free_triggername;
   // }


   //printf("iio trigger number being used is %d\n", trig_num);
   /* set odr and scaling */

   printf("=============== Setting odr and scaling attributes %d %s %d %d \n", sensor, inu_imu__db[sensor].dev_dir_name, posStartParams->accelBw[sensor], posStartParams->gyroBw[sensor]);

   if(sensor == INU_IMU_DATA__SENSOR_ACCELAROMETER_E || sensor == INU_IMU_DATA__SENSOR_GYROSCOPE_E )
   {
      inu_imu__setBwAttributeSensor(inu_imu__db[sensor].sampling_access,
                                    inu_imu__db[sensor].dev_dir_name,
                                    &posStartParams->odr[sensor],
                                    posStartParams->accelBw[sensor],
                                    posStartParams->gyroBw[sensor]);

      inu_imu__setAttributeSensor(inu_imu__db[sensor].scaling_access,
                                 inu_imu__db[sensor].dev_dir_name,
                                 &posStartParams->scale[sensor]);
   }
   

   if(sensor == INU_IMU_DATA__SENSOR_MAGNETOMETER_E && inu_mag_init == false)
   {
      inu_imu__setBwAttributeSensor(inu_imu__db[sensor].sampling_access,
                                    inu_imu__db[sensor].dev_dir_name,
                                    &posStartParams->odr[sensor],
                                    posStartParams->accelBw[sensor],
                                    posStartParams->gyroBw[sensor]);

      inu_imu__setAttributeSensor(inu_imu__db[sensor].scaling_access,
                                 inu_imu__db[sensor].dev_dir_name,
                                 &posStartParams->scale[sensor]);
      inu_mag_init = true;
   }

   if( posStartParams->batchSize[sensor] > 0 && posStartParams->batchSize[sensor] < 100 )
   {
      LOGG_PRINT(LOG_INFO_E,NULL,"Setting batch size for sensor %lu to be %lu \n",sensor,posStartParams->batchSize[sensor] );
      inu_imu__db[sensor].numberBatches = posStartParams->batchSize[sensor];
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"ERROR:Invalid batch size, Setting batch size for sensor %lu to be %lu \n",sensor,1 );
      inu_imu__db[sensor].numberBatches = 1;
   }
  

   /*
    * Parse the files in scan_elements to identify what channels are
    * present
    */
   ret = build_channel_array(inu_imu__db[sensor].dev_dir_name,0, &inu_imu__db[sensor].channels, &inu_imu__db[sensor].num_channels);
   if (ret) {
      printf( "Problem reading scan element information\n");
      printf( "diag %s\n", inu_imu__db[sensor].dev_dir_name);
      goto error_free_triggername;
   }

   /*
    * Construct the directory name for the associated buffer.
    * As we know that the lis3l02dq has only one buffer this may
    * be built rather than found.
    */
   ret = asprintf(&inu_imu__db[sensor].buf_dir_name, "%siio:device%d/buffer", iio_dir, dev_num);
   if (ret < 0)
   {
      ret = -ENOMEM;
      goto error_free_triggername;
   }
   //printf( "%s %s\n", inu_imu__db[sensor].dev_dir_name, inu_imu__db[sensor].trigger_name);
   /* Set the device trigger to be the data ready trigger found above */

   // ret = write_sysfs_string_and_verify("trigger/current_trigger",
   //                                     inu_imu__db[sensor].dev_dir_name,
   //                                     inu_imu__db[sensor].trigger_name);
   // if (ret < 0)
   // {
   //    printf( "Failed to write current_trigger file\n");
   //    goto error_free_buf_dir_name;
   // }

   /* Setup ring buffer parameters */
   #ifdef ENABLE_WATERMARK
   if( inu_imu__db[0].batchGroup && posStartParams->IMU_paired_batching_batchSize > 0)
   {
      if (ret < 0)
      {
         ret = -ENOMEM;
         goto error_free_triggername;
      }
      LOGG_PRINT(LOG_INFO_E,NULL,"Enabling the watermark for the IMU to be a value of %lu \n", posStartParams->IMU_paired_batching_batchSize);
      ret = write_sysfs_int("watermark", "/sys/bus/iio/devices/iio:device0/buffer0", posStartParams->IMU_paired_batching_batchSize);

   }
   #endif
   ret = write_sysfs_int("length", inu_imu__db[sensor].buf_dir_name, inu_imu__db[sensor].buf_len);
   if (ret < 0)
      goto error_free_buf_dir_name;

   inu_imu__db[sensor].scan_size = inu_imu__size_from_channelarray(inu_imu__db[sensor].channels, inu_imu__db[sensor].num_channels);
   inu_imu__db[sensor].data = malloc(inu_imu__db[sensor].scan_size*inu_imu__db[sensor].buf_len);
   if (!inu_imu__db[sensor].data)
   {
      ret = -ENOMEM;
      goto error_free_buf_dir_name;
   }

   MEM_POOLG_handleT poolH;
   MEM_POOLG_cfgT cfg;
   UINT32 bufferSize =  inu_imu__db[sensor].scan_size*inu_imu__db[sensor].numberBatches;
   cfg.bufferSize = MAGNETOMETER_PADDING+inu_imu__db[sensor].scan_size*inu_imu__db[sensor].numberBatches;
   cfg.numBuffers = 40;
   cfg.resetBufPtrInAlloc = 0;
   cfg.freeCb = NULL;
   cfg.freeArg = NULL;
   cfg.type = MEM_POOLG_TYPE_ALLOC_CMEM_E;
   cfg.memP = NULL;

   MEM_POOLG_initPool(&poolH, &cfg);
   inu_imu__db[sensor].dataPool = poolH;

   if (!inu_imu__db[sensor].dataPool)
   {
      ret = -ENOMEM;
      goto error_free_data;
   }
   LOGG_PRINT(LOG_DEBUG_E,NULL,"Preallocating the first IMU batch buffer to be size %lu- WRB \n",bufferSize );
   MEM_POOLG_alloc(inu_imu__db[sensor].dataPool,MAGNETOMETER_PADDING+inu_imu__db[sensor].scan_size*inu_imu__db[sensor].numberBatches,&inu_imu__db[sensor].bufP);

   ret = asprintf(&inu_imu__db[sensor].buffer_access, "/dev/iio:device%d", dev_num);
   if (ret < 0)
   {
      ret = -ENOMEM;
      goto error_free_data;
   }
   /* Attempt to open non blocking the access dev */
   inu_imu__db[sensor].fp = open(inu_imu__db[sensor].buffer_access, O_RDONLY | O_NONBLOCK);
   if (inu_imu__db[sensor].fp == -1)
   { /* If it isn't there make the node */
      printf( "Failed to open %s, dev_num = %d\n", inu_imu__db[sensor].buffer_access,dev_num);
      ret = -errno;
      goto error_free_buffer_access;
   }

   //printf( "%s, fp = %d\n",inu_imu__db[sensor].buffer_access,inu_imu__db[sensor].fp);

   fds->fd = inu_imu__db[sensor].fp;
   fds->events = POLLIN;

   posStartParams->descriptor.num_channels[sensor] = inu_imu__db[sensor].num_channels;

   for (channel = 0; channel < inu_imu__db[sensor].num_channels; channel++)
   {
      posStartParams->descriptor.chDescriptors[sensor][channel].bits_used = inu_imu__db[sensor].channels[channel].bits_used;
      posStartParams->descriptor.chDescriptors[sensor][channel].bytes     = inu_imu__db[sensor].channels[channel].bytes;
      posStartParams->descriptor.chDescriptors[sensor][channel].is_signed = inu_imu__db[sensor].channels[channel].is_signed;
      posStartParams->descriptor.chDescriptors[sensor][channel].location  = inu_imu__db[sensor].channels[channel].location;
      posStartParams->descriptor.chDescriptors[sensor][channel].maskL     = inu_imu__db[sensor].channels[channel].mask & 0xFFFFFFFF;
      posStartParams->descriptor.chDescriptors[sensor][channel].maskH     = (inu_imu__db[sensor].channels[channel].mask >> 32) & 0xFFFFFFFF;
      posStartParams->descriptor.chDescriptors[sensor][channel].offset    = inu_imu__db[sensor].channels[channel].offset;
      posStartParams->descriptor.chDescriptors[sensor][channel].scale     = inu_imu__db[sensor].channels[channel].scale;
      posStartParams->descriptor.chDescriptors[sensor][channel].be        = inu_imu__db[sensor].channels[channel].be;
      posStartParams->descriptor.chDescriptors[sensor][channel].shift     = inu_imu__db[sensor].channels[channel].shift;
      posStartParams->descriptor.chDescriptors[sensor][channel].channelType = inu_imu_data__typeFromName(inu_imu__db[sensor].channels[channel].name);

      /*printf( "sensor=%d channel=%d name=%s generic_name=%s bits_used=%d bytes=%d signed=%d location=%d maskL=0x%x maskH=0x%x offset=%f scale=%f be=%d shift=%d type=%d\n",
                                    sensor,channel,
                                    inu_imu__db[sensor].channels[channel].name,
                                    inu_imu__db[sensor].channels[channel].generic_name,
                                    posStartParams->descriptor.chDescriptors[sensor][channel].bits_used,
                                    posStartParams->descriptor.chDescriptors[sensor][channel].bytes,
                                    posStartParams->descriptor.chDescriptors[sensor][channel].is_signed,
                                    posStartParams->descriptor.chDescriptors[sensor][channel].location,
                                    posStartParams->descriptor.chDescriptors[sensor][channel].maskL,
                                    posStartParams->descriptor.chDescriptors[sensor][channel].maskH,
                                    posStartParams->descriptor.chDescriptors[sensor][channel].offset,
                                    posStartParams->descriptor.chDescriptors[sensor][channel].scale,
                                    posStartParams->descriptor.chDescriptors[sensor][channel].be,
                                    posStartParams->descriptor.chDescriptors[sensor][channel].shift,
                                    posStartParams->descriptor.chDescriptors[sensor][channel].channelType);*/
   }
   return 0;

   error_free_data:
      free(inu_imu__db[sensor].data);
   error_free_buffer_access:
      free(inu_imu__db[sensor].buffer_access);
   error_free_buf_dir_name:
      free(inu_imu__db[sensor].buf_dir_name);
   error_free_triggername:
      if (0)//(datardytrigger)
         free(inu_imu__db[sensor].trigger_name);

   if (inu_imu__db[sensor].dataPool)
   {
      MEM_POOLG_closePool(inu_imu__db[sensor].dataPool);
      inu_imu__db[sensor].dataPool = NULL;
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: inu_imu__deInitSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Pos_sensors service, system reset
*
****************************************************************************/
int inu_imu__deInitSensor()
{
   int ret = 0, sensor;

   for (sensor = 0; sensor < INU_DEFSG_POS_SENSOR_NUM_TYPES_E; sensor++)
   {
      if (inu_imu__db[sensor].fp > -1)
      {
         /* Stop the buffer */
         ret = write_sysfs_int("enable", inu_imu__db[sensor].buf_dir_name, 0);
         if (ret < 0)
            goto error_close_buffer_access;

         /* Disconnect the trigger - just write a dummy name. */
        // write_sysfs_string("trigger/current_trigger", inu_imu__db[sensor].dev_dir_name, "NULL");

         error_close_buffer_access:
            close(inu_imu__db[sensor].fp);
            free(inu_imu__db[sensor].data);
            MEM_POOLG_closePool(inu_imu__db[sensor].dataPool);
            free(inu_imu__db[sensor].buffer_access);
            free(inu_imu__db[sensor].buf_dir_name);

         inu_imu__db[sensor].currentSizeOfBatch = 0;

         inu_imu__db[sensor].fp = -1;
      }
   }

   return ret;
}



/****************************************************************************
*
*  Function Name: POS_SENSORSP_print2byte_debug
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Pos_sensors service
*
****************************************************************************/
void POS_SENSORSP_print2byte_debug(INT32 input, struct iio_channel_info *info)
{
   /* First swap if incorrect endian */
   if (info->be)
      input = be16toh((UINT16)input);
   else
      input = le16toh((UINT16)input);

   /*
    * Shift before conversion to avoid sign extension
    * of left aligned data
    */
   input = input >> info->shift;
   if (info->is_signed)
   {
      INT16 val = input;
      val &= (1 << info->bits_used) - 1;
      val = (INT16)(val << (16 - info->bits_used)) >>
      (16 - info->bits_used);
      printf("%05f ", ((float)val + info->offset)*info->scale);
   }
   else
   {
      UINT16 val = input;
      val &= (1 << info->bits_used) - 1;
      printf("%05f ", ((float)val + info->offset)*info->scale);
   }
}



/****************************************************************************
*
*  Function Name: POS_SENSORSP_gpProcess_scan_debug
*
*  Description: print out the values in SI units
*
*  Inputs:
*           @data:      pointer to the start of the scan
*           @channels:     information about the channels. Note
*                                   size_from_channelarray must have been called first to fill the
*                                   location offsets.
*           @num_channels: number of channels
*  Outputs:
*
*  Returns:
*
*  Context: Pos_sensors service
*
****************************************************************************/
void POS_SENSORSP_gpProcess_scan_debug(char *data, struct iio_channel_info *channels, int num_channels)
{
   INT32 k;

   for (k = 0; k < num_channels; k++)
   switch (channels[k].bytes)
   {
      /* only a few cases implemented so far */
      case 2:
         POS_SENSORSP_print2byte_debug(*(UINT16 *)(data + channels[k].location), &channels[k]);
         break;

      case 4:
         if (!channels[k].is_signed)
         {
            UINT32 val = *(UINT32 *)(data + channels[k].location);
            printf("%05f ", ((float)val + channels[k].offset)*channels[k].scale);
         }
         break;

      case 8:
      if (channels[k].is_signed)
      {
         int64_t val = *(int64_t *)(data + channels[k].location);
         if ((val >> channels[k].bits_used) & 1)
            val = (val & channels[k].mask) | ~channels[k].mask;
         /* special case for timestamp */
         if (channels[k].scale == 1.0f && channels[k].offset == 0.0f)
            printf("%" PRId64 " ", val);
         else
            printf("%05f ", ((float)val + channels[k].offset)*channels[k].scale);
      }
      break;

      default:
         break;
   }
   printf("\n");
}
/**
 * @brief Serializes the Metadata for the IMU
 * 
 *
 * @param metadata Metadata pointer
 * @param rtcTime  RTC time
 * @param pairedBatching - True for Paired batching, False for unpaired batching
 * @return Returns an error code
 */
ERRG_codeE inu_imu__serializeMetadata(char *buffer, size_t size, UINT64 startTimestamp_ns, bool pairedBatching)
{
   /*We only need to store the RTC timestamp in the metadata*/
   static int IMU_Metadata_Counter = 0;
   INU_Metadata_T metadata = ZEROED_OUT_METADATA; /*Create an empty metadata object*/
   inu_imu_data__hdr_t imuHdr;
   /*Read the RTC Time*/
   RTC_DRVG_RtcTimeT rtcTime;
   RTC_DRVG_getTime(&rtcTime);
   /*Prepare the IMU metadata*/
   UINT64 extended_seconds  = rtcTime.seconds;
   metadata.timestamp = ((extended_seconds & 0xFFFFFFFF) << 32) | rtcTime.fracSeconds;
   metadata.protobuf_packet.state_count = 0; 
   /*Get the current timestamp */
   UINT64 currentTs;
   OS_LYRG_getTimeNsec(&currentTs);
   metadata.protobuf_packet.metadatacounter = IMU_Metadata_Counter;
   metadata.protobuf_packet.protocol_version = inu_metadata__getProtocol_Version();
   metadata.protobuf_packet.timestamp_ns.nanoseconds = currentTs;
   metadata.protobuf_packet.has_timestamp_ns = true;
   metadata.protobuf_packet.has_imuTimestamp = true;
   metadata.protobuf_packet.imuTimestamp.userspace_timestamp.seconds = rtcTime.seconds;
   metadata.protobuf_packet.imuTimestamp.userspace_timestamp.fracSeconds = rtcTime.fracSeconds;
   metadata.protobuf_packet.imuTimestamp.start_imu_timestamp_ns = startTimestamp_ns;
   metadata.protobuf_packet.imuTimestamp.has_userspace_timestamp = true;
   metadata.protobuf_packet.has_imuBatchInfo = true;
   if(pairedBatching)
   {
      /*Paired batching is being used*/
      metadata.protobuf_packet.imuBatchInfo.batchingMethod = INU_metadata_IMU_Batching_Method_Paired_Gyro_Accelerometer_Channel_Batching;
   }
   else
   {
      /*Paired batch isn't being used*/
      metadata.protobuf_packet.imuBatchInfo.batchingMethod = INU_metadata_IMU_Batching_Method_Individual_Channel_Batching;
   }

   /*Let the host know if we're the master or slave sending this message*/
   if(HELSINKI_getMaster())
   {
      metadata.protobuf_packet.sender = INU_metadata_HardwareType_MASTER;
   }
   else
   {
      metadata.protobuf_packet.sender = INU_metadata_HardwareType_SLAVE;
   }
   ERRG_codeE ret = inu_metadata__serialize(buffer,size, &metadata);
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to serialize Metadata \n");
   }
   IMU_Metadata_Counter++;
   return ret;
}
static INU_Metadata_T IMU_Metadata = ZEROED_OUT_METADATA; /*Create an empty metadata object*/
/****************************************************************************
*
*  Function Name: inu_imu__gpSensorListener
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Pos_sensors service
*
****************************************************************************/
int inu_imu__gpSensorListener(void *argP)
{
   ERRG_codeE          ret;
   inu_imu             *me = (inu_imu*)argP;
   inu_imu__privData   *privP = (inu_imu__privData*)me->privP;
   int                 i, sensorIndex;
   ssize_t             read_size;
   int                 numSet = 1;
   inu_imu_data        *imu_data;

   privP->thrActive = TRUE;
   
   while(privP->active)
   {
      numSet = poll(fds, INU_IMU_DATA__SENSOR_NUM_TYPES_E, 1000);// 1 sec timeout

      if (numSet < 0)
      {
         printf( "pselect error = %s, numSet = %d\n",strerror(errno),numSet);
      }
      else if (numSet > 0)
      {
         for (sensorIndex = 0; sensorIndex < INU_IMU_DATA__SENSOR_NUM_TYPES_E; sensorIndex++)
         {
            if(fds[sensorIndex].revents & POLLIN)
            {
               read_size = read(inu_imu__db[sensorIndex].fp, inu_imu__db[sensorIndex].data, inu_imu__db[sensorIndex].buf_len*inu_imu__db[sensorIndex].scan_size);

               if (read_size == -EAGAIN)
               {
                  continue;
               }

               for (i = 0; i < read_size/inu_imu__db[sensorIndex].scan_size; i++)
               {
                  if(inu_imu__db[sensorIndex].batchGroup==NULL)
                  {
                  inu_imu_data__hdr_t * imuHdr = inu_imu__db[sensorIndex].imuHdr;

                  imuHdr->dataHdr.dataIndex = inu_imu__db[sensorIndex].frameCntr;
                  imuHdr->sensorType        = sensorIndex;
                  imuHdr->dataHdr.flags     = 64;//signal imu type
                  imuHdr->batchCount = inu_imu__db[sensorIndex].numberBatches;  /*Update how many sets of readings we have in this message */
                  imuHdr->batchSizeBytes = inu_imu__db[sensorIndex].scan_size;   /*Size of each set of X,Y,Z,Timestamp samples in bytes*/
                  inu_imu__db[sensorIndex].frameCntr++;
                  MEM_POOLG_bufDescT         *bufP = inu_imu__db[sensorIndex].bufP;
                  /*Allocate a buffer which is sized to be scan_size * number of batches */
                  if (bufP == NULL)
                  {
                     printf("IMU DRAIN!\n");
                     continue;
                  }
                  //POS_SENSORSP_gpProcess_scan_debug(inu_imu__db[sensorIndex].data + inu_imu__db[sensorIndex].scan_size*i,
                  //       inu_imu__db[sensorIndex].channels,
                  //      inu_imu__db[sensorIndex].num_channels);
                  /*Copy the reading into the next batch location*/
                  UINT32 batch_size_offset = inu_imu__db[sensorIndex].scan_size*inu_imu__db[sensorIndex].currentSizeOfBatch;
                  LOGG_PRINT(LOG_DEBUG_E,NULL, "Batch size offset %lu, batch number %llu,Scan size %d \n", batch_size_offset, inu_imu__db[sensorIndex].currentSizeOfBatch,inu_imu__db[sensorIndex].scan_size );
                  /*Assert that the offset+ scan size is less than the size of the buffer allocated to prevent nasty bugs from buffer overflows */
                  assert(batch_size_offset +inu_imu__db[sensorIndex].scan_size <= inu_imu__db[sensorIndex].scan_size*inu_imu__db[sensorIndex].numberBatches );
                  /*Copy the new IMU data into dataP plus the batch_size_offset*/
                  memcpy(&bufP->dataP[batch_size_offset], inu_imu__db[sensorIndex].data + inu_imu__db[sensorIndex].scan_size*i, inu_imu__db[sensorIndex].scan_size);
                  bufP->dataLen += inu_imu__db[sensorIndex].scan_size; /*Increase data length of batch buffer by 1x inu_imu_data___sensorReadableData*/
                  inu_imu__db[sensorIndex].currentSizeOfBatch++;       /*Increase the current number of entries in our batch*/
                  if( inu_imu__db[sensorIndex].currentSizeOfBatch  >= inu_imu__db[sensorIndex].numberBatches) /* Wait until we have enough samples for a "batch "*/
                  {
                     /*We now have a complete batch that we can send*/
                     CMEM_cacheWbInv(bufP->dataP,inu_imu__db[sensorIndex].scan_size);
                     UINT64 startIMUTimestamp = 0;
                     /*Serialize the metadata buffer*/
                     if(inu_imu__db[0].startIMURTCBasedTimestamp!=0) /* Use the Acceleromter start time as the start time */
                     {
                        LOGG_PRINT(LOG_DEBUG_E,NULL,"IMU Start Timestamp is %llu \n",inu_imu__db[0].startIMURTCBasedTimestamp );
                        startIMUTimestamp = inu_imu__db[0].startIMURTCBasedTimestamp; /*We must always use the starting time of the acceleromter as the IMU start timestamp*/
                     }
                     else
                     {  LOGG_PRINT(LOG_ERROR_E,NULL,"IMU Start Timestamp is %llu \n",inu_imu__db[0].startIMURTCBasedTimestamp );
                        startIMUTimestamp = 0;  /*This shouldn't happen, there must be an issue so we should send the start timestamp as 0*/
                     }
                     if(!inu_imu__db[sensorIndex].imuHdrPrepared)
                     {
                        /*Only prepare the IMU metadata once to reduce the CPU usage caused by the IMU stream*/
                        ERRG_codeE metadata_ret = inu_imu__serializeMetadata(imuHdr->metadata.buffer,sizeof(imuHdr->metadata.buffer),startIMUTimestamp,false);
                        if(ERRG_FAILED(metadata_ret))
                        {
                           LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to serialize IMU metadata \n");
                        }
                        inu_imu__db[sensorIndex].imuHdrPrepared = true;
                     }
                     LOGG_PRINT(LOG_DEBUG_E,NULL,"Batch complete for IMU with size %lu  \n", inu_imu__db[sensorIndex].bufP->dataLen);
                     inu_imu__db[sensorIndex].bufP->dataLen += MAGNETOMETER_PADDING;
                     ret = inu_function__newData((inu_function*)me,(inu_data*)privP->imu_data, bufP, imuHdr, (inu_data**)&imu_data);
                     if (ERRG_SUCCEEDED(ret))
                     {
                        #ifdef ENABLE_HANDLE_IMMEDIATE
                        inu_function__doneData_handleImmediately((inu_function*)me, (inu_data *)imu_data);
                        #else
                        inu_function__doneData((inu_function*)me, (inu_data *)imu_data);
                        #endif

                     }
                     /*Allocate the next buffer*/
                     /*We need to pad out the magnetometer bufers so that its the same size as Gyro/Accelerometer samples*/
                     MEM_POOLG_alloc(inu_imu__db[sensorIndex].dataPool,MAGNETOMETER_PADDING+inu_imu__db[sensorIndex].scan_size*inu_imu__db[sensorIndex].numberBatches,&inu_imu__db[sensorIndex].bufP);
                     bufP = inu_imu__db[sensorIndex].bufP;
                     bufP->dataLen = 0;
                     if (!privP->active)
                     {
                        privP->thrActive = FALSE;
                        return 0;
                     }
                     inu_imu__db[sensorIndex].currentSizeOfBatch = 0;
                  }
                  }
                     else
                     {
                        inu_imu___pairedBatching_processPairedSamples(&Helsinki_Gyro_Accelerometer_Paired_Batching,sensorIndex,read_size,i,me,privP,inu_imu__db);
                     }
                  }
            }
         }
      }
   }

   privP->thrActive = FALSE;

   return 0;
}

ERRG_codeE inu_imu__gpGetTemperatoreIoctl(inu_imuH me, inu_imu__temperatureParams *getTempParamsP)
{
#if 0
   UINT32                              sensorIndex;
   int                                 temperatureRaw;
   FIX_UNUSED_PARAM_WARN(me);

   for (sensorIndex = 0; sensorIndex < INU_IMU_DATA__SENSOR_NUM_TYPES_E; sensorIndex++)
   {
      if (inu_imu__db[sensorIndex].dev_dir_name)
      {
         //temperatureRaw = read_sysfs_posint("in_temp_raw",inu_imu__db[sensorIndex].dev_dir_name);
         //getTempParamsP->temperature = ((float)temperatureRaw * inu_imu__db[sensorIndex].tempScale) + inu_imu__db[sensorIndex].tempOffset;
         //printf("imuTemperatureP->temperature = %f ((%f * %f) + %d\n",getTempParamsP->temperature,(float)temperatureRaw,inu_imu__db[sensorIndex].tempScale,inu_imu__db[sensorIndex].tempOffset);
         read_sysfs_float("in_temp_raw", "/sys/bus/iio/devices/iio:device2/", &inu_imu__db[sensorIndex].tempScale);
         getTempParamsP->temperature = inu_imu__db[sensorIndex].tempScale;
         return INU_IMU__RET_SUCCESS;
      }
   }
   return INU_IMU__ERR_NOT_SUPPORTED;
#endif   

   FIX_UNUSED_PARAM_WARN(me);

   float temp_raw,temp_scale,temp_offset;
   
   read_sysfs_float("in_temp_raw", "/sys/bus/iio/devices/iio:device2/", &temp_raw);
   read_sysfs_float("in_temp_scale", "/sys/bus/iio/devices/iio:device2/", &temp_scale);
   read_sysfs_float("in_temp_offset", "/sys/bus/iio/devices/iio:device2/", &temp_offset);
   
   getTempParamsP->temperature = (temp_raw + temp_offset) * temp_scale;

   return INU_IMU__RET_SUCCESS;

}

#endif

static const char* inu_imu__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_imu__dtor(inu_ref *me)
{
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#if DEFSG_IS_GP
   inu_imu__deInitSensor();
#endif
   free(((inu_imu*)me)->privP);
}

/* Constructor */
ERRG_codeE inu_imu__ctor(inu_imu *me, inu_imu__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_imu__privData *privP;
#if DEFSG_IS_GP
   UINT32             sensorIndex, failsCnt=0;
   int                initRet = 0;
#endif

   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_imu__privData*)malloc(sizeof(inu_imu__privData));
      if(privP)
      {
         memset(privP, 0, sizeof(inu_imu__privData));
         me->privP = privP;
#if DEFSG_IS_GP
         inu_imu__initialize();
         for (sensorIndex = 0; sensorIndex < INU_DEFSG_POS_SENSOR_NUM_TYPES_E; sensorIndex++)
         {
            ctorParamsP->descriptor.num_channels[sensorIndex] = 0;
            /* count failures, to allow working even if some of the IMU is not loaded, but if all of them failed it means there is proble, to communicate with the sensor */
            initRet = inu_imu__initSensor(sensorIndex,&(fds[sensorIndex]),ctorParamsP);
            if (initRet < 0)
            {
               failsCnt++;
            }
            //("inu_imu__ctor imu init ret %d\n",ret);
         }
         /*Initialize the paired batching memory, we are taking the number of paired IMU samples  from the accelerometer batch size*/
         inu_imu__pairedBatching_init(&Helsinki_Gyro_Accelerometer_Paired_Batching,ctorParamsP->IMU_paired_batching_batchSize, inu_imu__db[0].scan_size,inu_imu__db[1].scan_size);
         if (failsCnt == INU_DEFSG_POS_SENSOR_NUM_TYPES_E)
         {
            printf("no IMU communication\n");
            return INU_IMU__ERR_UNEXPECTED;
         }
#endif
      }
      else
      {
         ret = INU_IMU__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}

static int inu_imu__rxIoctl(inu_ref *ref, void *msgP, int msgCode)
{
      int ret = INU_IMU__RET_SUCCESS;

      ret = inu_function__vtable_get()->node_vtable.ref_vtable.p_rxSyncCtrl(ref, msgP, msgCode);
#if DEFSG_IS_GP
      switch (msgCode)
      {
         case(INTERNAL_CMDG_GET_IMU_TEMPERATURE_E):
         {
            ret = inu_imu__gpGetTemperatoreIoctl((inu_imu *)ref,(inu_imu__temperatureParams*)msgP);
            break;
         }
         default:
         {
            break;
         }
      }
#endif
return ret;
}

static void inu_imu__newOutput(inu_node *me, inu_node *output)
{
   inu_imu *imu = (inu_imu*)me;
   inu_imu__privData *privP = (inu_imu__privData*)imu->privP;

   inu_function__vtable_get()->node_vtable.p_newOutput(me, output);
   //save data to ease access
   privP->imu_data = (inu_imu_data*)output;
}

static ERRG_codeE inu_imu__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
   ret = inu_function__vtable_get()->p_start(me, startParamP);
#if DEFSG_IS_GP
   OS_LYRG_threadParams     thrParams;
   inu_imu *imu = (inu_imu*)me;
   inu_imu__privData *privP = (inu_imu__privData*)imu->privP;
   int sensorIndex;

   for (sensorIndex = 0; sensorIndex < INU_IMU_DATA__SENSOR_NUM_TYPES_E; sensorIndex++)
   {
      inu_imu__db[sensorIndex].frameCntr = 0;

      if (inu_imu__db[sensorIndex].fp > -1)
      {
         /*Record when the IMU was started in nanoseconds based off the RTC time*/
         RTC_DRVG_getTimeNs(&inu_imu__db[sensorIndex].startIMURTCBasedTimestamp );
         if ( write_sysfs_int("enable", inu_imu__db[sensorIndex].buf_dir_name, 1) < 0)
         {
            ret = INU_IMU__ERR_WRITE_FILE;
         }
      }
   }

   privP->active = TRUE;
   thrParams.func = inu_imu__gpSensorListener;
   thrParams.id = OS_LYRG_POS_LISTENER_THREAD_ID_E;
   thrParams.event = NULL;
   thrParams.param = (void*)me;
   inu_imu__listenerThrdH = OS_LYRG_createThread(&thrParams);
   if(!inu_imu__listenerThrdH)
   {
      ret = INU_IMU__ERR_UNEXPECTED;
   }
#endif
   return ret;
}

static ERRG_codeE inu_imu__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
#if DEFSG_IS_GP
   inu_imu *imu = (inu_imu*)me;
   inu_imu__privData *privP = (inu_imu__privData*)imu->privP;
   int safetyCtr = 0;
   int sensorIndex;


   privP->active = FALSE;
   while((privP->thrActive) && (safetyCtr < 100))
   {
      OS_LYRG_usleep(10000);
      safetyCtr++;
   }
   for (sensorIndex = 0; sensorIndex < INU_IMU_DATA__SENSOR_NUM_TYPES_E; sensorIndex++)
   {

       if (inu_imu__db[sensorIndex].fp > -1)
       {
          if ( write_sysfs_int("enable", inu_imu__db[sensorIndex].buf_dir_name, 0) < 0)
          {
              ret = INU_IMU__ERR_WRITE_FILE;
          }
       }
   }

   if (privP->thrActive)
   {
      ret = INU_IMU__ERR_UNEXPECTED;
   }
#endif
   return ret;
}


void inu_imu__vtable_init(void)
{
   if (!_bool_vtable_initialized) {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_imu__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_imu__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_imu__ctor;
     _vtable.node_vtable.ref_vtable.p_rxSyncCtrl= inu_imu__rxIoctl;

      _vtable.node_vtable.p_newOutput = inu_imu__newOutput;

      _vtable.p_start = inu_imu__start;
      _vtable.p_stop  = inu_imu__stop;

      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_imu__vtable_get(void)
{
   inu_imu__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}

void inu_imu__setDescriptor(inu_imuH imuH, inu_imu_data__descriptorT *desc)
{
   inu_imu *imu = (inu_imu*)imuH;
   inu_imu__privData *privP = (inu_imu__privData*)imu->privP;
   memcpy(&privP->descriptor, desc, sizeof(inu_imu_data__descriptorT));
}

void inu_imu__getDescriptor(inu_imuH imuH, inu_imu_data__descriptorT *desc)
{
   inu_imu *imu = (inu_imu*)imuH;
   inu_imu__privData *privP = (inu_imu__privData*)imu->privP;
   memcpy(desc, &(privP->descriptor), sizeof(inu_imu_data__descriptorT));
}

ERRG_codeE inu_imu__getTemperature(inu_imuH meH, inu_imu__temperatureParams *imuTemperatureP)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INTERNAL_CMDG_GET_IMU_TEMPERATURE_E, imuTemperatureP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}


