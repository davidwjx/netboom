/****************************************************************************
 *
 *   FileName: UVC_com.h
 *
 *   Author:
 *
 *   Date:
 *
 *   Description: UVC com interface
 *
 ****************************************************************************/
#ifndef _UVC_COM_H_
#define _UVC_COM_H_

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/

#if DEFSG_IS_GP
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/usb/ch9.h>
#include <linux/usb/video.h>
#include <sys/ioctl.h>

#include "cls_com.h"
#include "os_lyr.h"
#include "socket.h"
#include "sys_defs.h"
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <inu_streamer_api.h>
//#include "basic_typedef.h"
//#include "uvc-gadget.h"
//#include "uvc_config_define.h"

#ifdef __cplusplus
extern "C" {
#endif




/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define UVC_COMG_FRAGMENT_SIZE     (16*1024)
#define UVC_NUM_OF_DEVICES         1

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

typedef struct
{
   int fd;
}uvc_service;

typedef struct
{
   UINT8 uvcInterfaceIdx;
   UINT32 fixedBuffSize;
   UINT8 fixedSizeCap;
   UINT8 txCap;
   UINT8 rxCap;
   uvc_service *uvc_service;
   void *inu_ref;
} UVC_COMG_paramsT;

typedef struct
{
   UVC_COMG_paramsT param;
   int fd;
} UVC_COMP_interfaceInfT;

typedef struct smybuff
{
   void *stream;
   void *data;
   void *buffer_pointer;
   unsigned int bufSize;
}TuvcRlsParams;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
void UVC_COMG_getOps(CLS_COMG_ifOperations *ifOpsP);
void UVC_MAING_SetStreamTable(void *streamH, inu_streamer__type_e streamType,unsigned int connId);
inu_streamerH *UVC_MAING_GetStreamTable(unsigned int connId);

#ifdef __cplusplus
}
#endif
#endif

#endif/* _UVC_COM_H_ */
