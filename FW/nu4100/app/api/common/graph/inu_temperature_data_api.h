#ifndef _INU_TEMPERATURE_DATA_API_H_
#define _INU_TEMPERATURE_DATA_API_H_

#include "inu2.h"

typedef void* inu_temperature_dataH;

#define PD_SLAVES (9)

typedef struct
{
   inu_data__hdr_t   dataHdr;
   float    imuTemp;
   UINT32   sensorTempCvr;
   UINT32   sensorTempCvl;
   float    chipTemp;
   float    chipVoltage;
   float    chipProcess[PD_SLAVES];
}inu_temperature_data__hdr_t;


#endif
