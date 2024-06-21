/****************************************************************************
 * 
 *   FileName: usb_ctrl_com.h
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: USB control com interface 
 *
 ****************************************************************************/
#ifndef _USB_CTRL_COM_H_
#define _USB_CTRL_COM_H_

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "cls_com.h"
#include "sys_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

#define USB_CTRL_COMG_PEEK_BUF_SIZE  (128)
/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef struct
{
   UINT8 usbInterfaceIdx;
   UINT32 fixedBuffSize;
   UINT8 fixedSizeCap;
   UINT8 txCap;
   UINT8 rxCap;
   UINT8 unBufferedReadCap ;     /*Added for helsinki, we found that we were receiving three URBs per image frame, so we have added a way of reading the header and the Image frame in one URB
                                 This is different than fixedBuffSize which also changes how transmission works*/
   UINT32 unBufferedReadSize;     /*How large the unbuffered read should be in bytes, this should be sizeof(frame) + sizeof(header), the header for an image is 172 bytes. 
                                 I recommend using wireshark to workout what this value is*/                            
   
#if DEFSG_IS_HOST
   UINT8 peekBuf[USB_CTRL_COMG_PEEK_BUF_SIZE];
   UINT16 peekRdIdx;
   UINT16 peekWrIdx;
#endif  
 
} USB_CTRL_COMG_paramsT;


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
void USB_CTRL_COMG_getOps(CLS_COMG_ifOperations *ifOpsP);

#ifdef __cplusplus
}
#endif

#endif /* _USB_CTRL_COM_H_ */
