/****************************************************************************
 *
 *   FileName: serial.h
 *
 *   Author: 
 *
 *   Date: 
 *
 *   Description: serial module
 *   
 ****************************************************************************/
#ifndef PSERIAL_H
#define PSERIAL_H
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/

#ifdef WIN32
#include <windows.h>
#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
//#define UART_CRC16

#define MAX_UART_PORT_NAME_LEN 64
#define MAX_CDC_PORT_NAME_LEN  MAX_UART_PORT_NAME_LEN

typedef enum
{
    SERIALG_TYPE_UART,
    SERIALG_TYPE_CDC,
    SERIALG_TYPE_BLOCKING
} SERIALG_serialTypeE;

#ifdef WIN32

typedef struct
{
    OVERLAPPED  osRead;
    OVERLAPPED  osWrite;
    HANDLE      uartHandle;
} PUART;
#else

typedef struct
{
   SERIALG_serialTypeE  serialType;
   INT32                uartHandle;
} PUART;

#endif

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

ERRG_codeE SERIALG_open(PUART *pUartHandle, char *pPortName, UINT32 baudRate, SERIALG_serialTypeE serialType);
ERRG_codeE SERIALG_close(PUART *uartHandle);
ERRG_codeE SERIALG_recv(PUART *uartHandle, UINT8 *inBuf, UINT32 nBytesToRead, UINT32 *pBytesRead, UINT32 timeout);
ERRG_codeE SERIALG_send(PUART *uartHandle, INT8 *lpByte, UINT32 dwBytesToWrite, UINT32 timeout);

#endif//PSERIAL_H
#ifdef __cplusplus
}
#endif
