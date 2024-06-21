#ifndef _INU_IMU_API_H_
#define _INU_IMU_API_H_

#include "inu2.h"

typedef void* inu_imuH;

typedef struct
{
   inu_graphH  graphH;
   inu_deviceH deviceH;
   float       scale[INU_IMU_DATA__SENSOR_NUM_TYPES_E];
   float       odr[INU_IMU_DATA__SENSOR_NUM_TYPES_E];
}inu_imu__initParams;

typedef struct
{
   float       temperature;
}inu_imu__temperatureParams;

ERRG_codeE inu_imu__getTemperature(inu_imuH meH, inu_imu__temperatureParams *imuTemperatureP);

#endif