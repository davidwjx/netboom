#ifndef _INU_IMU_DATA_H_
#define _INU_IMU_DATA_H_

#include "inu_data.h"
#include "inu2_types.h"


struct inu_imu_data;

typedef struct inu_imu_data
{
   inu_data            data;
   inu_imu_data__hdr_t imuHdr;
   void                *privP;
}inu_imu_data;

typedef struct inu_imu_data_CtorParamsTag
{
   inu_data__CtorParams       dataCtorParams;
   inu_imu_data__descriptorT  descriptor;
}inu_imu_data__CtorParams;

void inu_imu_data__vtable_init(void);
const inu_data__VTable *inu_imu_data__vtable_get(void);
inu_imu_data__posSensorChannelTypeE inu_imu_data__typeFromName(char *name);

#endif

