/****************************************************************************
 *
 *   FileName: pvt_drv.h
 *
 *   Author: 
 *
 *   Date:
 *
 *   Description: PVT  driver
 *
 ****************************************************************************/
#ifndef PVT_DRV_H
#define PVT_DRV_H

#include "inu_types.h"
#include "err_defs.h"
#include "io_pal.h"

ERRG_codeE PVT_DRVG_init();
ERRG_codeE PVT_DRVG_deinit();
ERRG_codeE PVT_DRVG_readTemperatureData(float *chipTemp);
ERRG_codeE PVT_DRVG_readVoltageData(float *chipVolTage);
ERRG_codeE PVT_DRVG_readProcessData(float *chipProcess);

#endif

