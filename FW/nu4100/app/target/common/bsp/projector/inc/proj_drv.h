/****************************************************************************
 *
 *   FileName: proj_drv.h
 *
 *   Author: Danny B.
 *
 *   Date:
 *
 *   Description: PROJ  driver wrapper
 *
 ****************************************************************************/
#ifndef PROJ_DRV_H
#define PROJ_DRV_H

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "inu_types.h"
#include "i2c_drv.h"

#ifdef __cplusplus
      extern "C" {
#endif

#include "io_pal.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/


/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/


typedef enum
{
   PROJ_DRVG_OPEN_PROJ_IOCTL_E = 0,
   PROJ_DRVG_CLOSE_PROJ_IOCTL_E,
   PROJ_DRVG_SET_PROJ_VAL_IOCTL_E,
   PROJ_DRVG_GET_PROJ_VAL_IOCTL_E,
   PROJ_DRVG_CHECK_SUPPORT_IOCTL_E,
   PROJ_DRVG_NUM_OF_IOCTLS_E
} PROJ_DRVG_ioctlListE;

typedef enum
{
   PROJ_DRVG_OPEN_PROJ_CMD_E                       = PROJ_DRVG_OPEN_PROJ_IOCTL_E                   | IO_CMD_MASK(IO_PROJ_0_E),
   PROJ_DRVG_CLOSE_PROJ_CMD_E                      = PROJ_DRVG_CLOSE_PROJ_IOCTL_E                  | IO_CMD_MASK(IO_PROJ_0_E),
   PROJ_DRVG_SET_PROJ_VAL_CMD_E                    = PROJ_DRVG_SET_PROJ_VAL_IOCTL_E                | IO_CMD_MASK(IO_PROJ_0_E),
   PROJ_DRVG_GET_PROJ_VAL_CMD_E                    = PROJ_DRVG_GET_PROJ_VAL_IOCTL_E                | IO_CMD_MASK(IO_PROJ_0_E),
   PROJ_DRVG_CHECK_SUPPORT_CMD_E                   = PROJ_DRVG_CHECK_SUPPORT_IOCTL_E                | IO_CMD_MASK(IO_PROJ_0_E),   
} PROJ_DRVG_ioctlCmdE;

typedef enum
{
   PROJ_DRVG_PROJ_0_E   = 0,
   PROJ_DRVG_PROJ_1_E   = 1,
   PROJ_DRVG_PROJ_2_E   = 2,
   PROJ_DRVG_NUM_OF_PROJ_E
} PROJ_DRVG_projNumberE;


typedef struct
{
   PROJ_DRVG_projNumberE      projNum;
} PROJ_DRVG_projOpenParamsT;

typedef struct
{
   PROJ_DRVG_projNumberE      projNum;
} PROJ_DRVG_projCloseParamsT;

typedef enum
{
   PROJ_DRVG_PROJ_STATE_CLEAR_E,
   PROJ_DRVG_PROJ_STATE_PATTERN_LOW_E,
   PROJ_DRVG_PROJ_STATE_PATTERN_HIGH_E,
   PROJ_DRVG_PROJ_STATE_FLOOD_LOW_E,
   PROJ_DRVG_PROJ_STATE_FLOOD_HIGH_E
} PROJ_DRVG_projStateE;

typedef struct
{
   PROJ_DRVG_projNumberE   projNum;
   PROJ_DRVG_projStateE    projState;
} PROJ_DRVG_projSetValParamsT;

typedef struct
{
   PROJ_DRVG_projNumberE   projNum;
   PROJ_DRVG_projStateE    projState;
} PROJ_DRVG_projGetValParamsT;

// module open function params
typedef struct
{
   MEM_MAPG_addrT gmeBaseAddress;
} PROJ_DRVG_openParamsT;

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE PROJ_DRVG_init(IO_PALG_apiCommandT *palP);


#ifdef __cplusplus
}
#endif


#endif //I2C_DRV_H




