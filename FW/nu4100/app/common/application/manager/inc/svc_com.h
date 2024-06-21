/****************************************************************************
 *
 *   FileName: svc_com.h
 *
 *   Author: Ram B. and Sivan C.
 *
 *   Date: 
 *
 *   Description:  
 *   
 ****************************************************************************/
#ifndef _SVC_COM_
#define _SVC_COM_

/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
 ****************************************************************************/
#include "err_defs.h"
#include "conn_lyr.h"

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define SVC_COMG_TXID_IGNORE   (CONN_LYRG_TXID_IGNORE)

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
//SCOM header structure defines 
#define SVC_COMG_CONTROL_IDX        (0)
#define SVC_COMG_ARG_IDX            (1)
#define SVC_COMG_RESP_IDX           (2)

//Masks for control field of SCOM header
#define SVC_COMG_TYPE_NUM_BITS      (2)
#define SVC_COMG_LENGTH_NUM_BITS    (26)
#define SVC_COMG_PROCID_NUM_BITS    (2)
#define SVC_COMG_ARG_NUM_BITS       (16)
#define SVC_COMG_RESP_NUM_BITS      (16)

//type
#define SVC_COMG_TYPE_OFFSET        (0)
#define SVC_COMG_TYPE_MASK          ((1 << SVC_COMG_TYPE_NUM_BITS) - 1)
#define SVC_COMG_TYPE_GET(x)        ((x >> SVC_COMG_TYPE_OFFSET) & SVC_COMG_TYPE_MASK)
#define SVC_COMG_TYPE_SET(x,id)     (x = ((x & ~(SVC_COMG_TYPE_MASK << SVC_COMG_TYPE_OFFSET)) | ((id & SVC_COMG_TYPE_MASK) << SVC_COMG_TYPE_OFFSET)))
//length
#define SVC_COMG_LENGTH_OFFSET      (SVC_COMG_TYPE_NUM_BITS)
#define SVC_COMG_LENGTH_MASK        ((1 << SVC_COMG_LENGTH_NUM_BITS) - 1)
#define SVC_COMG_LENGTH_GET(x)      ((x >> SVC_COMG_LENGTH_OFFSET) & SVC_COMG_LENGTH_MASK)
#define SVC_COMG_LENGTH_SET(x,id)   (x = ((x & ~(SVC_COMG_LENGTH_MASK << SVC_COMG_LENGTH_OFFSET)) | ((id & SVC_COMG_LENGTH_MASK) << SVC_COMG_LENGTH_OFFSET)))
//procId
#define SVC_COMG_PROCID_OFFSET      (SVC_COMG_TYPE_NUM_BITS + SVC_COMG_LENGTH_NUM_BITS)
#define SVC_COMG_PROCID_MASK        ((1 << SVC_COMG_PROCID_NUM_BITS) - 1)
#define SVC_COMG_PROCID_GET(x)      ((x >> SVC_COMG_PROCID_OFFSET) & SVC_COMG_PROCID_MASK)
#define SVC_COMG_PROCID_SET(x,id)   (x = ((x & ~(SVC_COMG_PROCID_MASK << SVC_COMG_PROCID_OFFSET)) | ((id & SVC_COMG_PROCID_MASK) << SVC_COMG_PROCID_OFFSET)))

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef struct
{
   UINT32 control[3];
} SVC_COMG_scomHdrT;

typedef struct
{
   CONN_LYRG_clsHdrT clsHdr;
   SVC_COMG_scomHdrT scomHdr;
} SVC_COMG_fullHdrT;

typedef enum
{
   SVC_COMG_CHAN_DIR_RX_E  = 0,
   SVC_COMG_CHAN_DIR_TX_E  = 1,
   SVC_COMG_CHAN_DIR_TXRX_E= 2
} SVC_COMG_channDirE;

typedef INT32 (*SVC_COMG_recvCb)(void *arg, SVC_COMG_scomHdrT *hdrP, void *bufferP);
typedef CONN_LYRG_txCbT SVC_COMG_sendCbT;


typedef struct
{
   INT32 channelNum;
   SVC_COMG_channDirE direction;
   INT16 priority;
   //Associated connection handle
   void *connHandleP;
   //Associated memory pool for the channel
   void *memPoolP;
   //Receive callback
   SVC_COMG_recvCb recvCb;
   void *argP;
} SVC_COMG_channelCfgT;

typedef void *SVC_COMG_handleT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

ERRG_codeE  SVC_COMG_init(void);
void        SVC_COMG_deInit();
ERRG_codeE  SVC_COMG_channelOpen(SVC_COMG_handleT *handleP, SVC_COMG_channelCfgT *chCfgP);
ERRG_codeE  SVC_COMG_channelClose(SVC_COMG_handleT handle);
ERRG_codeE  SVC_COMG_channelSend(SVC_COMG_handleT handle, SVC_COMG_scomHdrT *hdrP, void *bufP, UINT32 txid);
ERRG_codeE  SVC_COMG_channelSendMsg(SVC_COMG_handleT handle, SVC_COMG_scomHdrT *hdrP, UINT8 *bufP, UINT32 txid);
void        SVC_COMG_setRecvCb(SVC_COMG_handleT handle, SVC_COMG_recvCb recvCb, void *arg);
#ifdef OLD_API
void        SVC_COMG_setSendCb(SVC_COMG_handleT handle, SVC_COMG_sendCbT sendCb);
#endif
INT32       SVC_COMG_getChanNum(SVC_COMG_handleT handle);
void        *SVC_COMG_getMemPool(INT32 channelNum);
void        SVC_COMG_setMemPool(INT32 channelNum, void *memPoolP);
UINT32      SVC_COMG_getFullHdrMarshalSize();
UINT32      SVC_COMG_getHdrMarshalSize();


#ifdef __cplusplus
}
#endif

#endif //_SVC_COM_

