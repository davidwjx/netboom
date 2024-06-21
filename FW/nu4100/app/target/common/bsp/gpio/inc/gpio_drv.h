/****************************************************************************
 *
 *   FileName: gpio_drv.h
 *
 *   Author: Danny B.
 *
 *   Date: 
 *
 *   Description: GPIO  driver wrapper
 *   
 ****************************************************************************/
#ifndef GPIO_DRV_H
#define GPIO_DRV_H

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "inu_types.h"

#ifdef __cplusplus
      extern "C" {
#endif

#include "DW_common.h"
#include "DW_apb_gpio_public.h"
#include "DW_apb_gpio_private.h"
#include "io_pal.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/


/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/


typedef enum
{
   GPIO_DRVG_OPEN_GPIO_IOCTL_E = 0,
   GPIO_DRVG_CLOSE_GPIO_IOCTL_E,
   GPIO_DRVG_SET_GPIO_DIR_IOCTL_E,
   GPIO_DRVG_SET_GPIO_VAL_IOCTL_E,
   GPIO_DRVG_GET_GPIO_VAL_IOCTL_E,
   GPIO_DRVG_SET_GPIO_PORT_DIR_IOCTL_E,
   GPIO_DRVG_SET_GPIO_PORT_VAL_IOCTL_E,
   GPIO_DRVG_GET_GPIO_PORT_VAL_IOCTL_E,
   GPIO_DRVG_NUM_OF_IOCTLS_E
} GPIO_DRVG_ioctlListE;

typedef enum
{
	GPIO_DRVG_INST0_E,
	GPIO_DRVG_INST1_E,
	GPIO_DRVG_NUM_INST

} GPIO_DRVG_numInstE;

typedef enum
{
   GPIO_DRVG_OPEN_GPIO_CMD_E                       = GPIO_DRVG_OPEN_GPIO_IOCTL_E                   | IO_CMD_MASK(IO_GPIO_E),
   GPIO_DRVG_CLOSE_GPIO_CMD_E                      = GPIO_DRVG_CLOSE_GPIO_IOCTL_E                  | IO_CMD_MASK(IO_GPIO_E),
   GPIO_DRVG_SET_GPIO_DIR_CMD_E                    = GPIO_DRVG_SET_GPIO_DIR_IOCTL_E                | IO_CMD_MASK(IO_GPIO_E),
   GPIO_DRVG_SET_GPIO_VAL_CMD_E                    = GPIO_DRVG_SET_GPIO_VAL_IOCTL_E                | IO_CMD_MASK(IO_GPIO_E),
   GPIO_DRVG_GET_GPIO_VAL_CMD_E                    = GPIO_DRVG_GET_GPIO_VAL_IOCTL_E                | IO_CMD_MASK(IO_GPIO_E),
   GPIO_DRVG_SET_GPIO_PORT_DIR_CMD_E               = GPIO_DRVG_SET_GPIO_PORT_DIR_IOCTL_E           | IO_CMD_MASK(IO_GPIO_E),
   GPIO_DRVG_SET_GPIO_PORT_VAL_CMD_E               = GPIO_DRVG_SET_GPIO_PORT_VAL_IOCTL_E           | IO_CMD_MASK(IO_GPIO_E),
   GPIO_DRVG_GET_GPIO_PORT_VAL_CMD_E               = GPIO_DRVG_GET_GPIO_PORT_VAL_IOCTL_E           | IO_CMD_MASK(IO_GPIO_E)
} GPIO_DRVG_ioctlCmdE;

typedef enum
{
   GPIO_DRVG_GPIO_0_E   = 0,
   GPIO_DRVG_GPIO_1_E   = 1,
   GPIO_DRVG_GPIO_2_E   = 2,
   GPIO_DRVG_GPIO_3_E   = 3,
   GPIO_DRVG_GPIO_4_E   = 4,
   GPIO_DRVG_GPIO_5_E   = 5,
   GPIO_DRVG_GPIO_6_E   = 6,
   GPIO_DRVG_GPIO_7_E   = 7,
   GPIO_DRVG_GPIO_8_E   = 8,
   GPIO_DRVG_GPIO_9_E   = 9,
   GPIO_DRVG_GPIO_10_E   = 10,
   GPIO_DRVG_GPIO_11_E   = 11,
   GPIO_DRVG_GPIO_12_E   = 12,
   GPIO_DRVG_GPIO_13_E   = 13,
   GPIO_DRVG_GPIO_14_E   = 14,
   GPIO_DRVG_GPIO_15_E   = 15,
   GPIO_DRVG_GPIO_16_E   = 16,
   GPIO_DRVG_GPIO_17_E   = 17,
   GPIO_DRVG_GPIO_18_E   = 18,
   GPIO_DRVG_GPIO_19_E   = 19,
   GPIO_DRVG_GPIO_20_E   = 20,
   GPIO_DRVG_GPIO_21_E   = 21,
   GPIO_DRVG_GPIO_22_E   = 22,
   GPIO_DRVG_GPIO_23_E   = 23,
   GPIO_DRVG_GPIO_24_E   = 24,
   GPIO_DRVG_GPIO_25_E   = 25,
   GPIO_DRVG_GPIO_26_E   = 26,
   GPIO_DRVG_GPIO_27_E   = 27,
   GPIO_DRVG_GPIO_28_E   = 28,
   GPIO_DRVG_GPIO_29_E   = 29,
   GPIO_DRVG_GPIO_30_E   = 30,
   GPIO_DRVG_GPIO_31_E   = 31,
   GPIO_DRVG_GPIO_32_E   = 32,
   GPIO_DRVG_GPIO_33_E   = 33,
   GPIO_DRVG_GPIO_34_E   = 34,
   GPIO_DRVG_GPIO_35_E   = 35,
   GPIO_DRVG_GPIO_36_E   = 36,
   GPIO_DRVG_GPIO_37_E   = 37,
   GPIO_DRVG_GPIO_38_E   = 38,
   GPIO_DRVG_GPIO_39_E   = 39,
   GPIO_DRVG_GPIO_40_E   = 40,
   GPIO_DRVG_GPIO_41_E   = 41,
   GPIO_DRVG_GPIO_42_E   = 42,
   GPIO_DRVG_GPIO_43_E   = 43,
   GPIO_DRVG_GPIO_44_E   = 44,
   GPIO_DRVG_GPIO_45_E   = 45,
   GPIO_DRVG_GPIO_46_E   = 46,
   GPIO_DRVG_GPIO_47_E   = 47,
   GPIO_DRVG_GPIO_48_E   = 48,
   GPIO_DRVG_GPIO_49_E   = 49,
   GPIO_DRVG_GPIO_50_E   = 50,
   GPIO_DRVG_GPIO_51_E   = 51,
   GPIO_DRVG_GPIO_52_E   = 52,
   GPIO_DRVG_GPIO_53_E   = 53,
   GPIO_DRVG_GPIO_54_E   = 54,
   GPIO_DRVG_GPIO_55_E   = 55,
   GPIO_DRVG_GPIO_56_E   = 56,
   GPIO_DRVG_GPIO_57_E   = 57,
   GPIO_DRVG_GPIO_58_E   = 58,
   GPIO_DRVG_GPIO_59_E   = 59,
   GPIO_DRVG_GPIO_60_E   = 60,
   GPIO_DRVG_GPIO_61_E   = 61,
   GPIO_DRVG_GPIO_62_E   = 62,
   GPIO_DRVG_GPIO_63_E   = 63,
   GPIO_DRVG_NUM_OF_GPIO_E
} GPIO_DRVG_gpioNumberE;

typedef enum
{
   GPIO_DRVG_IN_DIRECTION_E   = Gpio_input,
   GPIO_DRVG_OUT_DIRECTION_E  = Gpio_output
} GPIO_DRVG_gpioDirectionE;

typedef struct
{
   GPIO_DRVG_gpioNumberE      gpioNum;
} GPIO_DRVG_gpioOpenParamsT;

typedef struct
{
   GPIO_DRVG_gpioNumberE      gpioNum;
} GPIO_DRVG_gpioCloseParamsT;

typedef struct
{
   GPIO_DRVG_gpioNumberE      gpioNum;
   GPIO_DRVG_gpioDirectionE   direction;
} GPIO_DRVG_gpioSetDirParamsT;

typedef enum
{
   GPIO_DRVG_GPIO_STATE_SET_E   = Dw_set,
   GPIO_DRVG_GPIO_STATE_CLEAR_E = Dw_clear
} GPIO_DRVG_gpioStateE;

typedef struct
{
   GPIO_DRVG_gpioNumberE   gpioNum;
   GPIO_DRVG_gpioStateE    val;
} GPIO_DRVG_gpioSetValParamsT;

typedef struct
{
   GPIO_DRVG_gpioNumberE   gpioNum;
   GPIO_DRVG_gpioStateE    val;
} GPIO_DRVG_gpioGetValParamsT;

typedef struct
{
   GPIO_DRVG_gpioDirectionE direction;
} GPIO_DRVG_gpioSetPortDirParamsT;

typedef struct
{
   UINT64   portVal;
} GPIO_DRVG_gpioSetPortValParamsT;

typedef struct
{
   UINT64   portVal;
} GPIO_DRVG_gpioGetPortValParamsT;

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE GPIO_DRVG_init(IO_PALG_apiCommandT *palP);


#ifdef __cplusplus
}
#endif


#endif //I2C_DRV_H




