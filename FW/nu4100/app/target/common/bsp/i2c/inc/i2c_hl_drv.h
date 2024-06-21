/****************************************************************************
 *
 *   FileName: I2C_HL_drv.h
 *
 *   Author: Danny B.
 *
 *   Date: 
 *
 *   Description: Inuitive IAE Driver
 *   
 ****************************************************************************/
#ifndef I2C_HL_DRV_H
#define I2C_HL_DRV_H

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "io_pal.h"
#include "err_defs.h"
#include "DW_common.h"
#include "DW_apb_i2c_public.h"
#include "DW_apb_i2c_private.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum {
   I2C_HL_DRVG_I2C_INSTANCE_0_E = 0,
   I2C_HL_DRVG_I2C_INSTANCE_1_E,
   I2C_HL_DRVG_I2C_INSTANCE_2_E,
   I2C_HL_DRVG_I2C_INSTANCE_3_E,
   I2C_HL_DRVG_I2C_INSTANCE_4_E,
   I2C_HL_DRVG_I2C_INSTANCE_5_E,
   I2C_HL_DRVG_NUM_OF_I2C_INSTANCES
} I2C_HL_DRVG_instanceIdE;


//////////////////////////////////////////////////////////////////
//connection IOCTL commands
/////////////////////////////////////////////////////////////////
typedef enum
{
   I2C_HL_DRVG_I2C_START_IOCTL_E = 0,
   I2C_HL_DRVG_I2C_STOP_IOCTL_E,
   I2C_HL_DRVG_I2C_GET_STATUS_IOCTL_E,
   I2C_HL_DRVG_I2C_RECONFIGURE_IOCTL_E,
   I2C_HL_DRVG_NUM_OF_IOCTLS_E
} I2C_HL_DRVG_ioctlListE;

typedef enum
{
   I2C_HL_DRVG_I2C_START_CMD_E                        = I2C_HL_DRVG_I2C_START_IOCTL_E                    | IO_CMD_MASK(IO_I2C_0_E),
   I2C_HL_DRVG_I2C_STOP_CMD_E                         = I2C_HL_DRVG_I2C_STOP_IOCTL_E                     | IO_CMD_MASK(IO_I2C_0_E),
   I2C_HL_DRVG_I2C_GET_STATUS_CMD_E                   = I2C_HL_DRVG_I2C_GET_STATUS_IOCTL_E               | IO_CMD_MASK(IO_I2C_0_E),
   I2C_HL_DRVG_I2C_RECONFIGURE_CMD_E                  = I2C_HL_DRVG_I2C_RECONFIGURE_IOCTL_E              | IO_CMD_MASK(IO_I2C_0_E)
} I2C_HL_DRVG_ioctlCmdE;

typedef enum
{
  I2C_HL_DRVG_SPEED_STANDARD_E   = I2c_speed_standard,
  I2C_HL_DRVG_SPEED_FAST_E       = I2c_speed_fast,
  I2C_HL_DRVG_SPEED_HIGHT_E      = I2c_speed_high
} I2C_HL_DRVG_iscSpeedE;

typedef enum
{
   I2C_HL_DRVG_MASTERMODE_7_BIT_ADDRESSING = I2c_7bit_address,
   I2C_HL_DRVG_MASTERMODE_10_BIT_ADDRESSING = I2c_10bit_address
} I2C_HL_DRVG_masterAddressModeE;

typedef struct
{
   UINT32      regAddress;
   UINT8       tarAddress;
   UINT8       regAddressAccessSize;
} I2C_HL_DRVG_readWriteParametersT;

typedef struct
{
   UINT32                           *data;
   I2C_HL_DRVG_iscSpeedE            i2cSpeed;
   I2C_HL_DRVG_masterAddressModeE   addressMode;
   UINT8                            SlaveAddress;
} I2C_HL_DRVG_i2cParametersT;

// module open function params
typedef struct
{
   void                          *i2cRegistersVirtualAddr;
   I2C_HL_DRVG_i2cParametersT    i2cParameters;
} I2C_HL_DRVG_openParamsT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE I2C_HL_DRVG_init(IO_PALG_apiCommandT *palP);
void I2C_HL_DRVP_printIaeRegs(void);
void I2C_HL_DRVP_writeIaeRegsDebug(UINT32 offset, UINT32 data);

#endif //I2C_HL_DRV_H


