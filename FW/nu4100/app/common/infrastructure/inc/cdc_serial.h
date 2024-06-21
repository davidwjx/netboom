/****************************************************************************
 *
 *   FileName: cdc_serial.h
 *
 *   Author: Oshri A.
 *
 *   Date:
 *
 *   Description: serial module
 *
 ****************************************************************************/
#ifndef _CDC_SERIAL_H_
#define _CDC_SERIAL_H_

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "serial.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE CDC_SERIALG_open(PUART *pUartHandle, char *pPortName, UINT32 baudRate, SERIALG_serialTypeE serialType);
ERRG_codeE CDC_SERIALG_close(PUART *uartHandle);
ERRG_codeE CDC_SERIALG_recv(PUART *uartHandle, UINT8 *inBuf, UINT32 nBytesToRead, UINT32 *pBytesRead, UINT32 timeout);
ERRG_codeE CDC_SERIALG_send(PUART *uartHandle, INT8 *lpByte, UINT32 dwBytesToWrite, UINT32 timeout);

#ifdef __cplusplus
}
#endif

#endif //_CDC_SERIAL_H_
