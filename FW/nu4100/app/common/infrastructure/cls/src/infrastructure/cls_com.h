/****************************************************************************
 *
 *   FileName: cls_com.h
 *
 *   Author: Benny V
 *
 *   Date: 22/01/14
 *
 *   Description: CLS (COM Layer Stack) internal functions and prototypes
 *
 ****************************************************************************/
#ifndef CLS_COM_H
#define CLS_COM_H

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define CLS_COMG_CAP_RX              (1<<0)
#define CLS_COMG_CAP_TX              (1<<1)
#define CLS_COMG_CAP_SENDV           (1<<2)
#define CLS_COMG_CAP_PEEK            (1<<3)
#define CLS_COMG_CAP_MODE_STREAM     (1<<4)
#define CLS_COMG_CAP_TIMED_RECV      (1<<5)
#define CLS_COMG_CAP_RX_CONT_RECV    (1<<6)
#define CLS_COMG_CAP_MODE_UVC        (1<<7)
#define CLS_COMG_CAP_MODE_UNBUFFERED_RECV        (1<<8)  /*Added for helsinki, we found that we were receiving three URBs per image frame, so we have added a way of reading the header and the Image frame in one URB*/
#define CLS_COMG_CAP_MODE_UNBUFFERED_SEND_V      (1<<9)  /*Added for helsinki, we found that we were receiving three URBs per image frame, so we have added a way of reading the header and the Image frame in one URB*/

typedef struct
{
   UINT32 flags;
   UINT32 fixedSize;
   UINT32 unbufferedFixedSize;
} CLS_COMG_capsT;

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

typedef struct
{
   ERRG_codeE (*open) (void **handleP, void *cfg);
   ERRG_codeE (*config) (void *handle, void *cfg);
   ERRG_codeE (*close)(void *handle);

   void (*getCaps)(void *handleP, CLS_COMG_capsT *capsP);
   
   ERRG_codeE (*send) (void *handle, UINT8 *bufP, UINT32 len, UINT32 *lenSentP);
   ERRG_codeE (*sendv)(void *handle, void *vecP, UINT32 *lenSentP);
   ERRG_codeE (*recv) (void *handle, UINT8 *bufP, UINT32 len, UINT32 *lenRecvP);
   /* HELSINKI- Added so that we can do a recv that calls inu_usb_read_bulk instead of inu_usb_buffered_read_bulk */
   ERRG_codeE (*recv_unbuffered) (void *handle, UINT8 *bufP, UINT32 len, UINT32 *lenRecvP);
   /* HELSINKI - Added so that we can send a header and frame together in one URB */
   ERRG_codeE (*sendv_unbuffered)(void *handle, void *vecP, UINT32 *lenSentP);
   ERRG_codeE (*recvContBuf) (void *connHandle, UINT8* bufPhyP, UINT8* bufP, UINT32 len, UINT32 *byteLenRecvP);
   ERRG_codeE (*timed_recv) (void *handle, UINT8 *bufP, UINT32 len, UINT32 timeout_usec, UINT32 *lenRecvP);
   ERRG_codeE (*peek) (void *handle, UINT8 *bufP, UINT32 len, UINT32 *lenRecvP);

} CLS_COMG_ifOperations;

typedef void (*CLS_COMG_getIfOps)(CLS_COMG_ifOperations *);


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

#ifdef __cplusplus
}
#endif

#endif //CLS_COM_H
