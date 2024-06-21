/****************************************************************************
 *
 *   FileName: i2c_drv.h
 *
 *   Author: Danny B.
 *
 *   Date: 
 *
 *   Description: I2C driver wrapper
 *   
 ****************************************************************************/
#ifndef I2C_DRV_H
#define I2C_DRV_H

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "inu_types.h"

#ifdef __cplusplus
      extern "C" {
#endif

#include "DW_common.h"
#include "DW_apb_i2c_public.h"
#include "DW_apb_i2c_private.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
typedef enum
{
   I2C_DRVG_I2C_INSTANCE_0_E = 0,
   I2C_DRVG_I2C_INSTANCE_1_E,
   I2C_DRVG_I2C_INSTANCE_2_E,
   I2C_DRVG_I2C_INSTANCE_3_E,
   I2C_DRVG_I2C_INSTANCE_4_E,
   I2C_DRVG_I2C_INSTANCE_5_E,
   I2C_DRVG_NUM_OF_I2C_INSTANCES
} I2C_DRVG_instanceIdE;

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE I2C_DRVG_open(I2C_DRVG_instanceIdE instanceId, UINT16 targetAddress, void* i2cInstanceAddress, enum dw_i2c_speed_mode speed,  enum dw_i2c_address_mode addresingMode);
ERRG_codeE I2C_DRVG_close(I2C_DRVG_instanceIdE instanceId);
ERRG_codeE I2C_DRVG_write(I2C_DRVG_instanceIdE instanceId, UINT32 regAddress, UINT8 regAddressAccessSize, UINT8 dataAccessSize, UINT32 data, UINT16 tarAddress, UINT32 uSdelay);
ERRG_codeE I2C_DRVG_read(I2C_DRVG_instanceIdE instanceId, UINT32 regAddress, UINT8 regAddressAccessSize, UINT8 dataAccessSize, UINT8 *data, UINT16 tarAddress, UINT32 uSdelay);
ERRG_codeE I2C_DRVG_direct_read(I2C_DRVG_instanceIdE instanceId, UINT8 dataAccessSize, UINT8 *data, UINT16 tarAddress, UINT32 uSdelay);
ERRG_codeE I2C_DRVG_direct_write(I2C_DRVG_instanceIdE instanceId, UINT8 dataAccessSize, UINT32 data, UINT16 tarAddress, UINT32 uSdelay);
ERRG_codeE I2C_DRVG_disableI2c(I2C_DRVG_instanceIdE instanceId);
ERRG_codeE I2C_DRVG_enableI2c(I2C_DRVG_instanceIdE instanceId);
ERRG_codeE I2C_DRVG_getI2cStatus(I2C_DRVG_instanceIdE instanceId, UINT32 *data);

#ifdef __cplusplus
}
#endif


#endif //I2C_DRV_H




