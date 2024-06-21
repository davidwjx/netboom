#include "inu_types.h"
#include "inu2.h"
#include "inu2_internal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "err_defs.h"
#include "internal_cmd.h"
#include "assert.h"
#define DEFAULT_IIR_COEFFICIENT 0.95
typedef struct
{
   inu_imu_data__descriptorT descriptor;
   char name[35];
}inu_imu_data__privData;

typedef struct
{
   inu_imu_data__posSensorChannelTypeE  type;
   char name[15];
} INU_IMU_DATA_SP_typeToNameMap;


static inu_data__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static INU_IMU_DATA_SP_typeToNameMap typeToNameMap[INU_IMU_DATA__SENSOR_CHANNEL_NUM_TYPES_E] = 
         {
               {INU_IMU_DATA__SENSOR_ACCELAROMETER_X_E,"in_accel_x"},
               {INU_IMU_DATA__SENSOR_ACCELAROMETER_Y_E,"in_accel_y"},
               {INU_IMU_DATA__SENSOR_ACCELAROMETER_Z_E,"in_accel_z"},
               {INU_IMU_DATA__SENSOR_GYROSCOPE_X_E,"in_anglvel_x"},
               {INU_IMU_DATA__SENSOR_GYROSCOPE_Y_E,"in_anglvel_y"},
               {INU_IMU_DATA__SENSOR_GYROSCOPE_Z_E,"in_anglvel_z"},
               {INU_IMU_DATA__SENSOR_MAGNETOMETER_X_E,"in_magn_x"},
               {INU_IMU_DATA__SENSOR_MAGNETOMETER_Y_E,"in_magn_y"},
               {INU_IMU_DATA__SENSOR_MAGNETOMETER_Z_E,"in_magn_z"},
               {INU_IMU_DATA__SENSOR_TIMESTAMP_E,"in_timestamp"}
         };


static const char* inu_imu_data__name(inu_ref *me)
{
   inu_imu_data *imu_data = (inu_imu_data*)me;
   inu_imu_data__privData *privP = (inu_imu_data__privData*)imu_data->privP;

   return privP->name;
}

static void inu_imu_data__dtor(inu_ref *me)
{
   inu_imu_data__privData *privP = (inu_imu_data__privData*)((inu_imu_data*)me)->privP;
   inu_data__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
   free((inu_imu_data*)privP);
}

/****************************************************************************
*
*  Function Name: inu_imu_data__size_from_channelarray
*
*  Description: calculate the storage size of a scan
*
*  Inputs:@channels:    the channel info array
*            @num_channels:   number of channels
*
*  Outputs: size in bytes of channel
*
*  Context: 
*
****************************************************************************/ 
int inu_imu_data__size_from_channelarray(inu_imu_data__chDescriptorT *channels, INT32 num_channels)
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


/* Constructor */
ERRG_codeE inu_imu_data__ctor(inu_imu_data *me, inu_imu_data__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   int        sensorIndex, scan_size, max_size=0;
   inu_imu_data__privData *privP;
   inu_imu_data__descriptorT *inu_imu_data__descriptor;
   ret = inu_data__ctor(&me->data, &ctorParamsP->dataCtorParams);
   if(ERRG_SUCCEEDED(ret))
   {
      privP = (inu_imu_data__privData*)malloc(sizeof(inu_imu_data__privData));
      if(privP)
      {
         memset(privP, 0, sizeof(inu_imu_data__privData));
         me->privP = privP;   

         strcpy(privP->name,"INU_IMU_DATA");
         memcpy(&privP->descriptor, &ctorParamsP->descriptor, sizeof(inu_imu_data__descriptorT));

         inu_imu_data__descriptor=(inu_imu_data__descriptorT*)&ctorParamsP->descriptor;

         //get maximum size between all imu channels
         for (sensorIndex=0; sensorIndex<INU_IMU_DATA__SENSOR_NUM_TYPES_E;sensorIndex++)
         {
            scan_size=inu_imu_data__size_from_channelarray((inu_imu_data__chDescriptorT *)inu_imu_data__descriptor->chDescriptors[sensorIndex], inu_imu_data__descriptor->num_channels[sensorIndex]);
            if(max_size<scan_size)
               max_size=scan_size;
         }
         if (max_size == 0)
         {
            printf("invalid size of data (0x%x)\n", ret);
            return INU_IMU_DATA__ERR_INVALID_ARGS;
         }
         inu_data__sizeSet((inu_data*)me,max_size,max_size);
      }
      else
      {
         ret = INU_IMU_DATA__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}


static UINT32 inu_imu_data__hdrMsgCodeGet(inu_data *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return INTERNAL_CMDG_IMU_DATA_SEND_E;
}

static void *inu_imu_data__hdrGet(inu_data *me)
{
   inu_imu_data *imu_data = (inu_imu_data*)me;
   return &imu_data->imuHdr;
}

static void inu_imu_data__hdrSet(inu_data *me, void *hdrP)
{
   inu_imu_data *imu_data = (inu_imu_data*)me;
   memcpy(&imu_data->imuHdr, hdrP, sizeof(inu_imu_data__hdr_t));
}

static void inu_imu_data__free_data(inu_data* me)
{
   inu_imu_data* clone = (inu_imu_data* )me;
   free(clone->privP);
   inu_data__vtable_get()->p_freeData(me);
}

static inu_data *inu_imu_data__duplicate(inu_data *me, UINT8 *bufP)
{
   inu_imu_data *clone = (inu_imu_data*)bufP;
   if (!clone)
   {
      return NULL;
   }

   memcpy(clone,me,sizeof(inu_imu_data));
   inu_data__vtable_get()->p_dataDuplicate(me,bufP);
   clone->privP = (inu_imu_data__privData*)malloc(sizeof(inu_imu_data__privData));
   if (!clone->privP)
   {
      return NULL;
   }
   memcpy(clone->privP, ((inu_imu_data*)me)->privP, sizeof(inu_imu_data__privData));

   return (inu_data*)clone;
}

void inu_imu_data__vtable_init()
{
   if (!_bool_vtable_initialized) 
   {
      inu_data__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_imu_data__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_imu_data__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_imu_data__ctor;

      _vtable.p_hdrMsgCodeGet = inu_imu_data__hdrMsgCodeGet;
      _vtable.p_dataHdrGet    = inu_imu_data__hdrGet;
      _vtable.p_dataHdrSet    = inu_imu_data__hdrSet;
      _vtable.p_dataDuplicate = inu_imu_data__duplicate;
#if DEFSG_IS_HOST
      _vtable.p_freeData      = inu_imu_data__free_data;
#endif
      _bool_vtable_initialized = true;
   }
}

const inu_data__VTable *inu_imu_data__vtable_get(void)
{
   inu_imu_data__vtable_init();
   return (const inu_data__VTable*)&_vtable;
}

inu_imu_data__posSensorChannelTypeE inu_imu_data__typeFromName(char *name)
{
   UINT32 i;

   for (i = 0; i < INU_IMU_DATA__SENSOR_CHANNEL_NUM_TYPES_E; i++)
   {
      if (strcmp(name,typeToNameMap[i].name) == 0)
      {
         return typeToNameMap[i].type;
      }
   }

   printf( "Unknown type of channel - %s\n",name);

   return (inu_imu_data__posSensorChannelTypeE)0;
}


char *inu_imu_data__nameFromType(inu_imu_data__posSensorChannelTypeE type)
{
   UINT32 i;

   for (i = 0; i < INU_IMU_DATA__SENSOR_CHANNEL_NUM_TYPES_E; i++)
   {
      if (type == typeToNameMap[i].type)
      {
         return typeToNameMap[i].name;
      }
   }

   printf( "Unknown type of channel - %d\n",type);

   return 0;
}



/****************************************************************************
*
*  Function Name: inu_imu_data__print2byte
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
static void inu_imu_data__print2byte(INT32 input, inu_imu_data__chDescriptorT *info, float *posSensors)
{
   //todo
   /* First swap if incorrect endian */
   //if (info->be)
   //   input = be16toh((UINT16)input);
   //else
   //   input = le16toh((UINT16)input);

   /*
    * Shift before conversion to avoid sign extension
    * of left aligned data
    */
   input = input >> info->shift;
   if (info->is_signed)
   {
      INT16 val = (UINT16)input;
      val &= (1 << info->bits_used) - 1;
      val = (INT16)(val << (16 - info->bits_used)) >>
      (16 - info->bits_used);
      *posSensors=((float)val + info->offset)*info->scale;
      //printf("%05f ", ((float)val + info->offset)*info->scale);
   }
   else
   {
      UINT16 val = (UINT16)input;
      val &= (1 << info->bits_used) - 1;
      //printf("%05f ", ((float)val + info->offset)*info->scale);
   }
}

/****************************************************************************
*
*  Function Name: inu_imu_data__gpProcess_scan
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
static void inu_imu_data__gpProcess_scan(char *data, inu_imu_data__chDescriptorT *channels, int num_channels, inu_imu_data__readableData *readableDataP)
{
   INT32 k;
   UINT64 mask;
   for (k = 0; k < num_channels; k++)
   {
      if (channels[k].channelType != INU_IMU_DATA__SENSOR_TIMESTAMP_E)
         readableDataP->imuData[k].name = inu_imu_data__nameFromType(channels[k].channelType);
      switch (channels[k].bytes)
      {
         /* only a few cases implemented so far */
         case 2:
            inu_imu_data__print2byte(*(UINT16 *)(data + channels[k].location), &channels[k], &readableDataP->imuData[k].imuVal);
            break;

         case 4:
            if (!channels[k].is_signed)
            {
               UINT32 val = *(UINT32 *)(data + channels[k].location);
               readableDataP->imuData[k].imuVal=((float)val + channels[k].offset)*channels[k].scale;
               //printf("%05f ", ((float)val + channels[k].offset)*channels[k].scale);
            }
            break;

         case 8:
         if(channels[k].channelType == INU_DEFSG_POS_SENSOR_TIMESTAMP_E )
         {
            UINT64 val = *(UINT64 *)(data + channels[k].location);
            readableDataP->timeStamp=val;
         }
         break;

         default:
            break;
      }
   }
   //printf("\n");
}
/**
 * @brief Converts RTC time into nanoseconds
 * @param metadata_time RTC based time reading
 * @return Returns time in nanoseconds
 */
static UINT64 inu_imu_data__convertRTCTimeToNS(UINT64 rtc_time)
{
	UINT64 second = (rtc_time >> 32);
	UINT64 frac_part = (rtc_time & 0xFFFFFFFF);
	double nanoseconds = (double)1E9* (double)frac_part/((double)0xFFFFFFFF);	/* Scale frac_part to be from 0->1*/
	return 1000000000*second + (UINT64) nanoseconds;
}
ERRG_codeE inu_imu_data__convertDataMultipleWithTimestampConversion(inu_imu_dataH dataH, inu_imu_data__readableData *readableDataP, UINT32 readableDataPLength,
 inu_imu_data__hdr_t *hdrP )
{
   int j;
   char *dataP;
   if(readableDataPLength < hdrP->batchCount)
   {
      /* readableDataPLength should be at least the batch count*/
      return INU_IMU__ERR_INVALID_ARGS;
   }
   for(unsigned int i =0; i < hdrP->batchCount; i ++)
   {
      UINT32 batchOffset = hdrP->batchSizeBytes*i;
      inu_imu_data__privData *privP = (inu_imu_data__privData*)((inu_imu_data*)dataH)->privP; 
      inu_imu_data__hdr_t *imuDataHdrP;
      UINT32 dataSize = inu_data__getMemSize(dataH);
      assert(dataSize >= hdrP->batchSizeBytes*hdrP->batchCount); /*Verify that the data size is correct*/
      dataP = (char*)inu_data__getMemPtr(dataH)+batchOffset;
      imuDataHdrP = (inu_imu_data__hdr_t*)inu_data__getHdr(dataH);

      for (j=0;j<(INU_IMU_DATA__SENSOR_CHANNEL_NUM_TYPES_E-2);j++)
         readableDataP[i].imuData[j].name=NULL;
      
      inu_imu_data__gpProcess_scan(dataP,privP->descriptor.chDescriptors[imuDataHdrP->sensorType],privP->descriptor.num_channels[imuDataHdrP->sensorType], &readableDataP[i]);
      /*Converts the timestamp into nanoseconds */
      readableDataP[i].timeStamp = inu_imu_data__convertRTCTimeToNS(readableDataP[i].timeStamp); 
   }
 
   return INU_IMU_DATA__RET_SUCCESS;
}
ERRG_codeE inu_imu_data__convertDataMultipleWithPairedBatchingAndTimestampConversion(inu_imu_dataH dataH, inu_imu_data__readableData *readableDataP, UINT32 readableDataPLength,
 inu_imu_data__hdr_t *hdrP )
{
   int j;
   char *dataP;
   if(readableDataPLength < hdrP->batchCount)
   {
      /* readableDataPLength should be at least the batch count*/
      return INU_IMU__ERR_INVALID_ARGS;
   }
   for(unsigned int i =0; i < hdrP->batchCount; i ++)
   {
      UINT32 batchOffset = hdrP->batchSizeBytes*i;
      inu_imu_data__privData *privP = (inu_imu_data__privData*)((inu_imu_data*)dataH)->privP; 
      inu_imu_data__hdr_t *imuDataHdrP;
      UINT32 dataSize = inu_data__getMemSize(dataH);
      assert(dataSize >= hdrP->batchSizeBytes*hdrP->batchCount); /*Verify that the data size is correct*/
      dataP = (char*)inu_data__getMemPtr(dataH)+batchOffset;
      imuDataHdrP = (inu_imu_data__hdr_t*)inu_data__getHdr(dataH);

      for (j=0;j<(INU_IMU_DATA__SENSOR_CHANNEL_NUM_TYPES_E-2);j++)
         readableDataP[i].imuData[j].name=NULL;
      /*In memory samples are batched like this:
      Accelerometer X,Y,Z,timestamp Gyro X,Y,Z,Timestamp. Where X,Y,Z samples are 2 bytes and the timestamp is 8 bytes. So in total you end up with 16 bytes for the Accelerometer and 16 bytes for the Gyro
      */
      UINT8 channelIndex = i % 2;   /*Even = Accelerometer, Odd = Gyro */
      inu_imu_data__gpProcess_scan(dataP,privP->descriptor.chDescriptors[channelIndex],privP->descriptor.num_channels[imuDataHdrP->sensorType], &readableDataP[i]);
      /*Converts the timestamp into nanoseconds */
      readableDataP[i].timeStamp = inu_imu_data__convertRTCTimeToNS(readableDataP[i].timeStamp); 
   }
 
   return INU_IMU_DATA__RET_SUCCESS;
}

ERRG_codeE inu_imu_data__convertDataMultiple(inu_imu_dataH dataH, inu_imu_data__readableData *readableDataP, UINT32 readableDataPLength, inu_imu_data__hdr_t *hdrP )
{
   int j;
   char *dataP;
   if(readableDataPLength < hdrP->batchCount)
   {
      /* readableDataPLength should be at least the batch count*/
      return INU_IMU__ERR_INVALID_ARGS;
   }
   for(unsigned int i =0; i < hdrP->batchCount; i ++)
   {
      UINT32 batchOffset = hdrP->batchSizeBytes*i;
      inu_imu_data__privData *privP = (inu_imu_data__privData*)((inu_imu_data*)dataH)->privP; 
      inu_imu_data__hdr_t *imuDataHdrP;
      UINT32 dataSize = inu_data__getMemSize(dataH);
      assert(dataSize >= hdrP->batchSizeBytes*hdrP->batchCount); /*Verify that the data size is correct*/
      dataP = (char*)inu_data__getMemPtr(dataH)+batchOffset;
      imuDataHdrP = (inu_imu_data__hdr_t*)inu_data__getHdr(dataH);

      for (j=0;j<(INU_IMU_DATA__SENSOR_CHANNEL_NUM_TYPES_E-2);j++)
         readableDataP[i].imuData[j].name=NULL;
      
      inu_imu_data__gpProcess_scan(dataP,privP->descriptor.chDescriptors[imuDataHdrP->sensorType],privP->descriptor.num_channels[imuDataHdrP->sensorType], &readableDataP[i]);
      // This should be uncommented if the IMU doesn't support timestamps 
      //readableDataP[i].timeStamp +=  (UINT64)(privP->descriptor.shiftTs[imuDataHdrP->sensorType] *1000);
   }
 
   return INU_IMU_DATA__RET_SUCCESS;
}
/****************************************************************************
*
*  Function Name: inu_imu_data__convertData
*
*  Description: utility function, for converting the raw IIO IMU data into a more readable data
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
ERRG_codeE inu_imu_data__convertData(inu_imu_dataH dataH, inu_imu_data__readableData *readableDataP )
{
   int j;
   char *dataP;
   inu_imu_data__privData *privP = (inu_imu_data__privData*)((inu_imu_data*)dataH)->privP;
   inu_imu_data__hdr_t *imuDataHdrP;

   dataP = (char*)inu_data__getMemPtr(dataH);
   imuDataHdrP = (inu_imu_data__hdr_t*)inu_data__getHdr(dataH);

   for (j=0;j<(INU_IMU_DATA__SENSOR_CHANNEL_NUM_TYPES_E-2);j++)
      readableDataP->imuData[j].name=NULL;
	
   inu_imu_data__gpProcess_scan(dataP,privP->descriptor.chDescriptors[imuDataHdrP->sensorType],privP->descriptor.num_channels[imuDataHdrP->sensorType], readableDataP);
   //readableDataP->timeStamp +=  (UINT64)(privP->descriptor.shiftTs[imuDataHdrP->sensorType] *1000);
 
   return INU_IMU_DATA__RET_SUCCESS;
}


