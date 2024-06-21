#ifndef _INU_SENSORS_GROUP_API_H_
#define _INU_SENSORS_GROUP_API_H_

#include "inu2.h"

typedef void* inu_sensors_groupH;

UINT32      inu_sensors_group__getId(inu_sensors_groupH meH);
ERRG_codeE  inu_sensors_group__setRuntimeCfg(inu_sensors_groupH meH, inu_sensor__runtimeCfg_t *cfgP );
ERRG_codeE  inu_sensors_group__getRuntimeCfg(inu_sensors_groupH meH, inu_sensor__runtimeCfg_t *cfgP );
#endif