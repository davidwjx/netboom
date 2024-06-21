/****************************************************************************
 *
 *   FileName: svc_com.c
 *
 *   Author: Ram B. and Sivan C.
 *
 *   Date: 
 *
 *   Description:  
 *   
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/

#include "inu_types.h"
#include "err_defs.h"
#include "log.h"

#include <assert.h>
#include <string.h>

#include "sys_defs.h"
#include "svc_com.h"
#include "mem_pool.h"
#include "conn_lyr.h"
#include "marshal.h"


#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define SVC_COMP_MAX_NUM_OF_CHANNELS   (SYS_DEFSG_MAX_NUM_CHANNELS_E)

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/

typedef struct
{
   INT32 channelId;
   //Associated connection handle
   void *connHandleP;
   //Associated memory pool for the channel
   void *memPoolP;
   SVC_COMG_recvCb recvCb;
   void *arg;
   OS_LYRG_mutexT cbMutex;
   unsigned int cbMutexValid;
} SVC_COMP_channelParams;

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
#ifdef OLD_API
static void SVC_COMP_recvCb(void *argP, void *bufP);
static void SVC_COMP_parseHeader(UINT8 *bufP, SVC_COMG_scomHdrT *hdrP);
#endif

static void SVC_COMP_clearEntry(UINT32 idx);
static UINT32 SVC_COMP_marshalHdr(INT16 dir, UINT8 *bufP, SVC_COMG_scomHdrT *hdrP);
static void SVC_COMP_writeHeader(UINT8 *bufP, SVC_COMG_scomHdrT *hdrP);

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static SVC_COMP_channelParams SVC_COMP_channelTable[SVC_COMP_MAX_NUM_OF_CHANNELS];

static UINT32 SVC_COMP_hdrMarshalSize = 0;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static void SVC_COMP_clearEntry(UINT32 idx)
{
   SVC_COMP_channelParams *entryP;

   if(idx < SVC_COMP_MAX_NUM_OF_CHANNELS)
   {
      entryP = &SVC_COMP_channelTable[idx];
      entryP->channelId = -1;
      entryP->connHandleP = 0;
      entryP->recvCb = NULL;
      entryP->arg = NULL;
      entryP->memPoolP = NULL;
      entryP->cbMutexValid = 0;
   }
}

static UINT32 SVC_COMP_marshalHdr(INT16 dir, UINT8 *bufP, SVC_COMG_scomHdrT *hdrP)
{
   //Parse header into local structure
   UINT32 idx = 0;
   BOOL dryRun =  (bufP && hdrP)  ? FALSE : TRUE;

   if(!dryRun) 
      MARSHALG_u32(dir, &(hdrP->control[0]), &bufP[idx]);
   idx += sizeof(UINT32);

   if(!dryRun) 
      MARSHALG_u32(dir, &(hdrP->control[1]), &bufP[idx]);
   idx += sizeof(UINT32);

   if(!dryRun) 
      MARSHALG_u32(dir, &(hdrP->control[2]), &bufP[idx]);
   idx += sizeof(UINT32);

   return idx;
}

#ifdef OLD_API
static void SVC_COMP_parseHeader(UINT8 *bufP, SVC_COMG_scomHdrT *hdrP)
{
   SVC_COMP_marshalHdr(MARSHALG_UNDO, bufP, hdrP);
}
#endif

static void SVC_COMP_writeHeader(UINT8 *bufP, SVC_COMG_scomHdrT *hdrP)
{
   SVC_COMP_marshalHdr(MARSHALG_DO, bufP, hdrP);
}

#ifdef OLD_API
static void SVC_COMP_recvCb(void *argP, void *bufP)
{
   SVC_COMP_channelParams  *chanParamsP = (SVC_COMP_channelParams *)argP;
   SVC_COMG_scomHdrT       hdr;
   MEM_POOLG_bufDescT      *bufDescP = (MEM_POOLG_bufDescT *)bufP;
   int dofree = TRUE;

   //Parse header
   SVC_COMP_parseHeader(bufDescP->dataP, &hdr);

   //Advance data pointer in buffer
   bufDescP->dataP += SVC_COMP_hdrMarshalSize;
   bufDescP->dataLen -= SVC_COMP_hdrMarshalSize; 
   
   //LOGG_PRINT(LOG_DEBUG_E, NULL, "SVC COM: rx channel:%d, len=%d\n", chanParamsP->channelId, bufDescP->dataLen);

   OS_LYRG_lockMutex(&chanParamsP->cbMutex);
   //Invoke user's channel callback if exists
   if(chanParamsP->recvCb)
   {
      chanParamsP->recvCb(chanParamsP->arg, &hdr, bufDescP);
      dofree = FALSE;
   }
   OS_LYRG_unlockMutex(&chanParamsP->cbMutex);

   if(dofree)
   {
      MEM_POOLG_free(bufDescP);
   }

}
#endif
/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

ERRG_codeE SVC_COMG_init(void)
{
   ERRG_codeE ret = SVC_COM__RET_SUCCESS;
   int i;
   SVC_COMP_channelParams *entryP;

   for (i = 0; i < SVC_COMP_MAX_NUM_OF_CHANNELS; i++)
   {
      entryP = &SVC_COMP_channelTable[i];

      SVC_COMP_clearEntry(i);
      
      //mutex for cb protection
      if(OS_LYRG_aquireMutex(&entryP->cbMutex) == SUCCESS_E)
      {
         entryP->cbMutexValid= 1;
      }
      else
      {
         ret = SVC_COM__ERR_OUT_OF_RSRCS;
         break;
      }
   }

   SVC_COMP_hdrMarshalSize = SVC_COMP_marshalHdr(MARSHALG_DO, NULL, NULL);

   return ret;
}


void SVC_COMG_deInit()
{
   unsigned int i;
   SVC_COMP_channelParams *entryP;
   
   for (i = 0; i < SVC_COMP_MAX_NUM_OF_CHANNELS; i++)
   {
      entryP = &SVC_COMP_channelTable[i];
      if(entryP->cbMutexValid)
      {
         OS_LYRG_releaseMutex(&entryP->cbMutex);
         entryP->cbMutexValid = 0;
      }

      SVC_COMP_clearEntry(i);
   }

}

/****************************************************************************
*
*  Function Name: SVC_COMG_channelOpen
*
*  Description: Description Open a peer-to-peer communication channel for the specified service.
*
*  Inputs: Service ID – Identifies the service ID
           connId - ID of interface to receive/send on.
*          priority – Channel priority (not supported)
*          recvCb - A CB function which will be invoked upon a message receive
*
*  Outputs: handle - handle to be passed to API on subsequent calls.
*
*  Returns: Success or specific error code.
*
*  Context: Multiple, blockable.
****************************************************************************/
ERRG_codeE  SVC_COMG_channelOpen(SVC_COMG_handleT *handleP, SVC_COMG_channelCfgT *chCfgP)
{
   ERRG_codeE ret = SVC_COM__RET_SUCCESS;
   SVC_COMP_channelParams *chanParamsP;

   //Opening a channel involves registering the channel's callbacks with the given connection.
   //On RX register an internal callback - which peels off the svc com header and invokes the user's channel callback.
   if(chCfgP->channelNum < SYS_DEFSG_MAX_NUM_CHANNELS_E)
   {
      //For quick access map channel numbers 1-1 with channel table.
      //Assuming channel number range is small.  
      chanParamsP = &SVC_COMP_channelTable[chCfgP->channelNum];

      //Save configuration to configuration table
      chanParamsP->channelId = chCfgP->channelNum;
      //Maintain connection handle for transmission on connection layer
      chanParamsP->connHandleP = chCfgP->connHandleP;   
      //Maintain mempool for this channel for use by connection layer on reception.
      chanParamsP->memPoolP= chCfgP->memPoolP;
      //User's receive callback optionally registered here
      chanParamsP->recvCb = chCfgP->recvCb;
      chanParamsP->arg = chCfgP->argP;
#ifdef OLD_API
      //Register receive callback with connection layer - pass channel params structure as an argument
      CONN_LYRG_rxRegister(chanParamsP->connHandleP, chanParamsP->channelId, SVC_COMP_recvCb, (void *)chanParamsP);
#endif
      //Return handle
      *handleP = chanParamsP;
   }
   else
   {
      ret = SVC_COM__ERR_INVALID_CHANEL_NUM;
   }
   return ret;

}
#ifdef OLD_API
/****************************************************************************
*
*  Function Name: SVC_COMG_channelClose
*
*  Description: Close a peer-to-peer communication channel.   
*     
*  Inputs: handle - Handle returned in open call.
*            
*  Outputs: None.
*  
*  Returns: Success or specific error code.
*
*  Context: Multiple, blockable.
****************************************************************************/
ERRG_codeE  SVC_COMG_channelClose(SVC_COMG_handleT handle)
{
   SVC_COMP_channelParams *chanParamsP = (SVC_COMP_channelParams *) handle;

   //Register NULL callbacks with connection layer
   CONN_LYRG_rxRegister(chanParamsP->connHandleP, chanParamsP->channelId, NULL, NULL);
   CONN_LYRG_txRegister(chanParamsP->connHandleP, chanParamsP->channelId, NULL);

   OS_LYRG_lockMutex(&chanParamsP->cbMutex);
   chanParamsP->recvCb = NULL;
   OS_LYRG_unlockMutex(&chanParamsP->cbMutex);

   SVC_COMP_clearEntry(chanParamsP->channelId);
   return SVC_COM__RET_SUCCESS;   
}
#endif
/****************************************************************************
*
*  Function Name: SVC_COMG_channelSend
*
*  Description: Send a message to a peer on an open channel (no copy).
*     Buffer is not copied. Buffer will be freed internally after 
*     transmission.
*
*  Inputs: handle - Handle returned in open call.
*          message ID – ID of message to un-register.
*          buffer pointer – pointer to buffer structure with data.
*
*  Outputs: None.
*
*  Returns: Success or specific error code.
*
*  Context: Multiple, blockable.
****************************************************************************/
ERRG_codeE  SVC_COMG_channelSend(SVC_COMG_handleT handle, SVC_COMG_scomHdrT *hdrP, void *bufP, UINT32 txid)
{
   ERRG_codeE              retval      = SVC_COM__RET_SUCCESS;
   SVC_COMP_channelParams  *chanParamsP= (SVC_COMP_channelParams *) handle;
   MEM_POOLG_bufDescT      *bufDescP   = (MEM_POOLG_bufDescT *)bufP;
   INT32                   ret;

   //Move back data pointer in buffer for header
   bufDescP->dataP -= SVC_COMP_hdrMarshalSize;
   bufDescP->dataLen += SVC_COMP_hdrMarshalSize;

   //Add header to buffer assuming pre-allocated header room
   SVC_COMP_writeHeader(bufDescP->dataP, hdrP);

   //Call connection layer send function
   ret = CONN_LYRG_send(chanParamsP->connHandleP, chanParamsP->channelId, bufP, txid, 0);

   if(ERRG_FAILED(ret))
   {
      // error handling
      LOGG_PRINT(LOG_ERROR_E, ret, "SVC COM: failed to send packet.\n");
      retval = SVC_COM__ERR_SEND;
   }

   return retval;
}

/****************************************************************************
*
*  Function Name: SVC_COMG_channelSendMsg
*
*  Description: Send a message to a peer on an open channel (with copy)
*     Note: buffer is copied internally.   
*     
*  Inputs: handle - Handle returned in open call. 
*          message ID – ID of message to un-register.
*          buffer pointer – pointer to buffer with data
*          len - message length
*
*  Outputs: None.
*
*  Returns: Success or specific error code.
*
*  Context: Multiple, blockable.
****************************************************************************/
ERRG_codeE  SVC_COMG_channelSendMsg(SVC_COMG_handleT handle, SVC_COMG_scomHdrT *hdrP, UINT8 *bufP, UINT32 txid)
{
   ERRG_codeE              retval         = SVC_COM__RET_SUCCESS;
   SVC_COMP_channelParams  *chanParamsP   = (SVC_COMP_channelParams *) handle;
   MEM_POOLG_bufDescT      *bufDescP      = NULL;

   //Allocate buffer from channel's mempool with extra header room for lower layers
   retval = MEM_POOLG_alloc(chanParamsP->memPoolP, SVC_COMG_LENGTH_GET(hdrP->control[SVC_COMG_CONTROL_IDX]) + SVC_COMG_getFullHdrMarshalSize(), &bufDescP);
   if(bufDescP != NULL)
   {
      bufDescP->dataP += SVC_COMG_getFullHdrMarshalSize();
      memcpy(bufDescP->dataP, bufP, SVC_COMG_LENGTH_GET(hdrP->control[SVC_COMG_CONTROL_IDX]));
      bufDescP->dataLen =  SVC_COMG_LENGTH_GET(hdrP->control[SVC_COMG_CONTROL_IDX]);
      retval = SVC_COMG_channelSend(chanParamsP, hdrP, bufDescP, txid);
   }

   return retval;
}

void *SVC_COMG_getMemPool(INT32 channelNum)
{
   //assert(channelNum <= SYS_DEFSG_MAX_NUM_CHANNELS_E);     
   SVC_COMP_channelParams *chanParamsP = &SVC_COMP_channelTable[channelNum];
   return chanParamsP->memPoolP;
}

void SVC_COMG_setMemPool(INT32 channelNum, void *memPoolP)
{
   if(channelNum < SYS_DEFSG_MAX_NUM_CHANNELS_E)
   {
      SVC_COMP_channelTable[channelNum].memPoolP = memPoolP;
   }
}

INT32  SVC_COMG_getChanNum(SVC_COMG_handleT handle)
{
   SVC_COMP_channelParams *chanParamsP = (SVC_COMP_channelParams *) handle;
   return chanParamsP->channelId;
}

void SVC_COMG_setRecvCb(SVC_COMG_handleT handle, SVC_COMG_recvCb recvCb, void *arg)
{
   SVC_COMP_channelParams *chanParamsP = (SVC_COMP_channelParams *)handle;

   OS_LYRG_lockMutex(&chanParamsP->cbMutex);
   chanParamsP->recvCb = recvCb;
   chanParamsP->arg = arg;
   OS_LYRG_unlockMutex(&chanParamsP->cbMutex);
}
#ifdef OLD_API
void SVC_COMG_setSendCb(SVC_COMG_handleT handle, SVC_COMG_sendCbT sendCb)
{
   SVC_COMP_channelParams *chanParamsP = (SVC_COMP_channelParams *)handle;

   CONN_LYRG_txRegister(chanParamsP->connHandleP, chanParamsP->channelId, sendCb);
}
#endif

UINT32 SVC_COMG_getFullHdrMarshalSize(void)
{
   return SVC_COMG_getHdrMarshalSize() + CONN_LYRG_getHdrMarshalSize();
}

UINT32  SVC_COMG_getHdrMarshalSize(void)
{
   return SVC_COMP_hdrMarshalSize;
}

#ifdef __cplusplus
}
#endif

