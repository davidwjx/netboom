/****************************************************************************
 *
 *   FileName: gen_projector_drv.h
 *
 *   Author: Elad R.
 *
 *   Date: 
 *
 *   Description: Generic projector layer
 *   
 ****************************************************************************/
#ifndef GEN_PROJECTOR_DRV_H
#define GEN_PROJECTOR_DRV_H


#ifdef __cplusplus
      extern "C" {
#endif

#include "err_defs.h"
#include "io_pal.h"
#include "i2c_hl_drv.h"
#include "i2c_drv.h"
#include "gme_drv.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define GEN_PROJECTOR_DRVG_ERROR_TEST(X)           if(ERRG_FAILED(X)){return X;}
#define GEN_PROJECTOR_DRVG_REGISTER_ACCESS_SIZE    (2)
#define GEN_PROJECTOR_DRVG_PROJECTOR_TABLE_DELAY      (5*1000) // 5 msec delay

#define GEN_PROJECTOR_DRVG_WRITE_SENSOR_REG(paramsStruct, handle, Address, dataVal, accessSizeVal, retVal)\
   {  paramsStruct.accessRegParams.rdWr         = WRITE_ACCESS_E;                                        \
      paramsStruct.accessRegParams.regAdd       = Address;                                               \
      paramsStruct.accessRegParams.data         = dataVal;                                               \
      paramsStruct.accessRegParams.accessSize   = accessSizeVal;                                         \
      retVal = GEN_PROJECTOR_DRVG_accessProjectorReg(handle,&paramsStruct);}

#define GEN_PROJECTOR_DRVG_READ_SENSOR_REG(paramsStruct, handle, Address, accessSizeVal, retVal)\
   {  paramsStruct.accessRegParams.rdWr         = READ_ACCESS_E;                                         \
      paramsStruct.accessRegParams.regAdd       = Address;                                               \
      paramsStruct.accessRegParams.accessSize   = accessSizeVal;                                         \
      retVal = GEN_PROJECTOR_DRVG_accessProjectorReg(handle,&paramsStruct);}

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

typedef struct
{
   UINT16   regAdd;
   UINT16   accessSize;
   UINT32   data;
} GEN_PROJECTOR_DRVG_regTblParamsT;

typedef enum
{
   GEN_SENSOR_DRVG_INSTANCE_STATUS_DUMMY     = 0,
   GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E   = 1,
   GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN      = 2
} GEN_PROJECTOR_DRVG_instanceStatusE;

typedef enum
{
   GEN_PROJECTOR_DRVG_PROJECTOR_CMD_CTRL_SELF_E     = 0,
   GEN_PROJECTOR_DRVG_PROJECTOR_CMD_CTRL_PARTNER_E  = 1,
   GEN_PROJECTOR_DRVG_PROJECTOR_CMD_CTRL_BOTH_E     = 2,
   GEN_PROJECTOR_DRVG_PROJECTOR_CMD_CTRL_NUM_CMDS_E   
} GEN_PROJECTOR_DRVG_projectorCmdCtrlE;


//connection IOCTL commands
typedef enum
{
   GEN_PROJECTOR_DRVG_RESET_PROJECTOR_IOCTL_E  = 0,
   GEN_PROJECTOR_DRVG_INIT_PROJECTOR_IOCTL_E   = 1,
   GEN_PROJECTOR_DRVG_CONFIG_PROJECTOR_IOCTL_E = 2,
   GEN_PROJECTOR_DRVG_START_PROJECTOR_IOCTL_E  = 3,
   GEN_PROJECTOR_DRVG_STOP_PROJECTOR_IOCTL_E   = 4,
   GEN_PROJECTOR_DRVG_NUM_OF_IOCTLS_E
} GEN_PROJECTOR_DRVG_ioctlListE;

typedef enum
{
   GEN_PROJECTOR_DRVG_RESET_PROJECTOR_E            = GEN_PROJECTOR_DRVG_RESET_PROJECTOR_IOCTL_E          | IO_CMD_MASK(IO_PROJ_0_E),
   GEN_PROJECTOR_DRVG_PROJECTOR_INIT_E             = GEN_PROJECTOR_DRVG_INIT_PROJECTOR_IOCTL_E           | IO_CMD_MASK(IO_PROJ_0_E),
   GEN_PROJECTOR_DRVG_CONFIG_PROJECTOR_E           = GEN_PROJECTOR_DRVG_CONFIG_PROJECTOR_IOCTL_E         | IO_CMD_MASK(IO_PROJ_0_E),
   GEN_PROJECTOR_DRVG_START_PROJECTOR_E            = GEN_PROJECTOR_DRVG_START_PROJECTOR_IOCTL_E          | IO_CMD_MASK(IO_PROJ_0_E),
   GEN_PROJECTOR_DRVG_STOP_PROJECTOR_E             = GEN_PROJECTOR_DRVG_STOP_PROJECTOR_IOCTL_E           | IO_CMD_MASK(IO_PROJ_0_E),
   GEN_PROJECTOR_DRVG_DUMMY_E
} GEN_PROJECTOR_DRVG_CmdE;


typedef struct
{
   UINT32                           data;
   UINT16                           regAdd;
   UINT8                            rdWr;
   UINT8                            accessSize;
   GEN_PROJECTOR_DRVG_projectorCmdCtrlE   cmdCtrl;
} GEN_PROJECTOR_DRVG_accessRegParamsT;

typedef struct
{
   UINT16                           projectorId[INU_DEFSG_IOCTL_PROJECTOR_ID_LENGTH];
   INT8                             projectorIdStr[17];
   GEN_PROJECTOR_DRVG_projectorCmdCtrlE   cmdCtrl;
} GEN_PROJECTOR_DRVG_getDeviceIdParamsT;


typedef union
{
   GEN_PROJECTOR_DRVG_accessRegParamsT       accessRegParams;
   GEN_PROJECTOR_DRVG_getDeviceIdParamsT     getDeviceIdParams;
} GEN_PROJECTOR_DRVG_projectorParametersT;

typedef ERRG_codeE (*PROJECTOR_init)(IO_PALG_apiCommandT *palP);

typedef struct
{
   void*                      i2cControllerAddress;
   I2C_HL_DRVG_instanceIdE    i2cInstanceId;
   PROJECTOR_init                projectorInitFunc;
   I2C_HL_DRVG_iscSpeedE      i2cSpeed;
   INU_DEFSG_projectorModelE  projectorModel;
   INU_DEFSG_projectorTypeE   projectorType;
   UINT16                     projectorId;
}  GEN_PROJECTOR_DRVG_openParametersT;

typedef ERRG_codeE (*GEN_PROJECTOR_DRVG_ioctlFuncListT)(IO_HANDLE handle, void *argP);

typedef struct
{
   GEN_PROJECTOR_DRVG_instanceStatusE  deviceStatus;
   GEN_PROJECTOR_DRVG_ioctlFuncListT   *ioctlFuncList;
   UINT16                              projectorAddress; 
   I2C_DRVG_instanceIdE                i2cInstanceId;
   I2C_HL_DRVG_iscSpeedE               i2cSpeed;
   INU_DEFSG_projectorModelE           projectorModel;
   INU_DEFSG_projectorTypeE            projectorType;
   void                               *projectorHandle; 
} GEN_PROJECTOR_DRVG_specificDeviceDescT;

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
ERRG_codeE GEN_PROJECTOR_DRVG_init(IO_PALG_apiCommandT *palP);
ERRG_codeE GEN_PROJECTOR_DRVG_ioctlAccessReg(IO_HANDLE handle, void *pParams);
ERRG_codeE GEN_PROJECTOR_DRVG_accessProjectorReg(IO_HANDLE handle, void *pParams);
ERRG_codeE GEN_PROJECTOR_DRVP_gpioInit(UINT32 gpioNum);

#ifdef __cplusplus
}
#endif


#endif //GEN_SENSOR_DRV_H


