/****************************************************************************
 *
 *   FileName: io_pal.h
 *
 *   Author:  Danny B.
 *
 *   Date: 
 *
 *   Description: Inuitive IO PAL
 *   
 ****************************************************************************/
#ifndef IO_PAL_H
#define IO_PAL_H

#include "inu_types.h"
#include "err_defs.h"

#ifdef __cplusplus
		  extern "C" {
#endif

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
//Define IO connections here
typedef enum {
#ifdef CEVA
#else
   IO_I2C_0_E = 0,
   IO_I2C_1_E,
   IO_I2C_2_E,
   IO_I2C_3_E,
   IO_I2C_4_E,
   IO_I2C_5_E,
   IO_SENSOR_CAM0_E,
   IO_SENSOR_CAM1_E,
   IO_SENSOR_CAM2_E,
   IO_SENSOR_CAM3_E,
   IO_SENSOR_CAM4_E,
   IO_SENSOR_CAM5_E,
   IO_SENSOR_CAM6_E,
   IO_SENSOR_CAM7_E,   
   IO_SENSOR_CAM8_E,     
   IO_SENSOR_CAM9_E,
   IO_SENSOR_CAM10_E,
   IO_SENSOR_CAM11_E,
   IO_SENSOR_CAM12_E,
   IO_SENSOR_CAM13_E,
   IO_SENSOR_CAM14_E,
   IO_SENSOR_CAM15_E,
   IO_SENSOR_CAM16_E,
   IO_SENSOR_CAM17_E,
   IO_SENSOR_CAM18_E,
   IO_SENSOR_CAM19_E,
   IO_SENSOR_CAM20_E,
   IO_SENSOR_CAM21_E,
   IO_SENSOR_CAM22_E,
   IO_SENSOR_CAM23_E,
   IO_GPIO_E,
   IO_PROJ_0_E,
   IO_PROJ_1_E,
   IO_PROJ_2_E,
#endif   
   IO_SPI_FLASH_1_E, 
   IO_SPI_FLASH_2_E,
   IO_MIPI_E,
   IO_GYR_E,
   IO_ACC_E,
   IO_NUM_OF_DEVICE_TYPES_E
} IO_PALG_deviceIdE;

typedef ERRG_codeE (*IO_PALG_openT) (IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
typedef ERRG_codeE (*IO_PALG_closeT)(IO_HANDLE handle);
typedef ERRG_codeE (*IO_PALG_readT) (IO_HANDLE handle, UINT8 *bufP, UINT32 numBytesToRead, void *numBytesReadP, void *params);
typedef ERRG_codeE (*IO_PALG_writeT)(IO_HANDLE handle, UINT8 *bufP, UINT32 numBytes, void *params);
typedef ERRG_codeE (*IO_PALG_ioctlT)(IO_HANDLE handle, UINT32 cmd, void *argP);

typedef struct {
   IO_PALG_openT     open;
   IO_PALG_closeT    close;
   IO_PALG_writeT    write;
   IO_PALG_readT     read;
   IO_PALG_ioctlT    ioctl;
} IO_PALG_apiCommandT;

typedef struct {
   IO_PALG_apiCommandT     IO_PALP_apiCommand;
   IO_HANDLE               handle;
} IO_PALG_deviceDesc;

#define IO_CMD_LEN                     (8)  //up to 256 IOs
#define IO_CMD_MASK(s)                 ((s) << (32-IO_CMD_LEN))
#define IO_CMD_GET_DEVICE_ID(s)        ((UINT32)((s) >> (32-IO_CMD_LEN)))
#define IO_CMD_IS_DEVICE_ID(s,devId)   ( IO_CMD_GET_DEVICE_ID(s)  == devId )
#define IO_CMD_GET_IOCTL(s)            ((UINT16)(s))

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE IO_PALG_init(void);
ERRG_codeE IO_PALG_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
ERRG_codeE IO_PALG_read(IO_HANDLE handle, UINT8 *bufP, UINT32 numBytesToRead, void *numBytesP, void *params);
ERRG_codeE IO_PALG_write(IO_HANDLE handle, UINT8 *bufP, UINT32 numBytes, void *params);
ERRG_codeE IO_PALG_close(IO_HANDLE handle);
ERRG_codeE IO_PALG_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
IO_HANDLE  IO_PALG_getHandle(IO_PALG_deviceIdE deviceId);

#ifdef __cplusplus
}
#endif



#endif // IO_PAL_H
