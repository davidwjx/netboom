#include "inu_ref.h"
#include "inu_comm.h"
#include "inu2.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inu_factory.h"
#include "svc_com.h"
#include "mem_pool.h"
#include "marshal.h"
#include <assert.h>

/****************************************************************************
***************      L O C A L            T Y P E D E F S     ***************
****************************************************************************/
//SCOM header structure defines 
#define INU_COMM__CONTROL_IDX        (0)
#define INU_COMM__ARG_IDX            (1)
#define INU_COMM__RESP_IDX           (2)

//Masks for control field of SCOM header
#define INU_COMM__TYPE_NUM_BITS      (2)
#define INU_COMM__LENGTH_NUM_BITS    (26)
#define INU_COMM__PROCID_NUM_BITS    (2)
#define INU_COMM__ARG_NUM_BITS       (16)
#define INU_COMM__RESP_NUM_BITS      (16)

//type
#define INU_COMM__TYPE_OFFSET        (0)
#define INU_COMM__TYPE_MASK          ((1 << INU_COMM__TYPE_NUM_BITS) - 1)
#define INU_COMM__TYPE_GET(x)        ((x >> INU_COMM__TYPE_OFFSET) & INU_COMM__TYPE_MASK)
#define INU_COMM__TYPE_SET(x,id)     (x = ((x & ~(INU_COMM__TYPE_MASK << INU_COMM__TYPE_OFFSET)) | ((id & INU_COMM__TYPE_MASK) << INU_COMM__TYPE_OFFSET)))
//length
#define INU_COMM__LENGTH_OFFSET      (INU_COMM__TYPE_NUM_BITS)
#define INU_COMM__LENGTH_MASK        ((1 << INU_COMM__LENGTH_NUM_BITS) - 1)
#define INU_COMM__LENGTH_GET(x)      ((x >> INU_COMM__LENGTH_OFFSET) & INU_COMM__LENGTH_MASK)
#define INU_COMM__LENGTH_SET(x,id)   (x = ((x & ~(INU_COMM__LENGTH_MASK << INU_COMM__LENGTH_OFFSET)) | ((id & INU_COMM__LENGTH_MASK) << INU_COMM__LENGTH_OFFSET)))
//procId
#define INU_COMM__PROCID_OFFSET      (INU_COMM__TYPE_NUM_BITS + INU_COMM__LENGTH_NUM_BITS)
#define INU_COMM__PROCID_MASK        ((1 << INU_COMM__PROCID_NUM_BITS) - 1)
#define INU_COMM__PROCID_GET(x)      ((x >> INU_COMM__PROCID_OFFSET) & INU_COMM__PROCID_MASK)
#define INU_COMM__PROCID_SET(x,id)   (x = ((x & ~(INU_COMM__PROCID_MASK << INU_COMM__PROCID_OFFSET)) | ((id & INU_COMM__PROCID_MASK) << INU_COMM__PROCID_OFFSET)))

#define INU_COMM__HOST_ID_E (0)
#define INU_COMM__GP_ID_E (1)
#define INU_COMM__CEVA_ID_E (2)

#if (DEFSG_PROCESSOR == DEFSG_HOST)
#define  INU_COMM__PROC_ID    (INU_COMM__HOST_ID_E)
#elif (DEFSG_PROCESSOR == DEFSG_GP)
#define  INU_COMM__PROC_ID    (INU_COMM__GP_ID_E)
#else
#define  INU_COMM__PROC_ID    (INU_COMM__CEVA_ID_E)
#endif

typedef enum
{
   INU_COMM__MSG_IOCTL_E,
   INU_COMM__MSG_RESP_E,
   INU_COMM__MSG_DATA_E,
   INU_COMM__MSG_PING_E,
} inu_comm__msgE;

typedef struct
{
   UINT32 control[3];
} inu_comm__hdrT;

typedef struct
{
   inu_comm__hdrT hdr;
   MEM_POOLG_bufDescT *bufDesc;
   inu_ref *ref;
} inu_comm__msgT;

#define INU_COMM__THR_MSG_Q_NAME "/comm_q"
#define INU_COMM__THR_MSG_Q_BUFS (40)
#define INU_COMM__THR_SLEEP_MSEC (2000)


typedef struct
{
   OS_LYRG_threadHandle thrHandle;
   OS_LYRG_msgQueT      thrMsgQue;
   UINT32               refCtr;
   inu_comm            *comm;
}inu_comm__threadData;



#ifdef __cplusplus
extern "C" {
#endif


static inu_comm__threadData threadData = {0};
static UINT32 inu_comm__hdrMarshalSize = 0;
static ERRG_codeE inu_comm__sendMsg(inu_comm *me, inu_ref *ref, inu_comm__msgT *msgP);


static UINT32 inu_comm__marshalHdr(INT16 dir, UINT8 *bufP, inu_comm__hdrT *hdrP)
{
   //Parse header into local structure
   UINT32 idx = 0;
   BOOL dryRun = (bufP && hdrP) ? FALSE : TRUE;

   if (!dryRun)
      MARSHALG_u32(dir, &(hdrP->control[0]), &bufP[idx]);
   idx += sizeof(UINT32);

   if (!dryRun)
      MARSHALG_u32(dir, &(hdrP->control[1]), &bufP[idx]);
   idx += sizeof(UINT32);

   if (!dryRun)
      MARSHALG_u32(dir, &(hdrP->control[2]), &bufP[idx]);
   idx += sizeof(UINT32);

   return idx;
}

static void inu_comm__parseHeader(UINT8 *bufP, inu_comm__hdrT *hdrP)
{
   inu_comm__marshalHdr(MARSHALG_UNDO, bufP, hdrP);
}

static void inu_comm__writeHeader(UINT8 *bufP, inu_comm__hdrT *hdrP)
{
   inu_comm__marshalHdr(MARSHALG_DO, bufP, hdrP);
}

static void inu_comm__handleMsg(inu_comm *me, inu_comm__msgT *msgP)
{
   inu_comm__msgE    msgType;
   INT32             msgCode;
   int               respCode;
   inu_ref           *ref = msgP->ref;

   msgType = (inu_comm__msgE)INU_COMM__TYPE_GET(msgP->hdr.control[0]);
   msgCode = msgP->hdr.control[INU_COMM__ARG_IDX];

   switch (msgType)
   {
      case (INU_COMM__MSG_RESP_E):
      {
         //Response message - forward to next peer or handle locally depending on the src ID.
         if (SVC_COMG_PROCID_GET(msgP->hdr.control[SVC_COMG_CONTROL_IDX]) == INU_COMM__PROC_ID)
         {
            inu_ref__ctrlSyncDone(ref, msgP->hdr.control[SVC_COMG_RESP_IDX], msgP->bufDesc);
         }
         else
         {
            //Forward response to prev peer
            assert(0);
            //inu_comm__sendMsg(me, ref, msgP);
         }
      }
      break;

      case (INU_COMM__MSG_IOCTL_E):
      {
         if (SVC_COMG_PROCID_GET(msgP->hdr.control[SVC_COMG_CONTROL_IDX]) == INU_COMM__PROC_ID)
         {
            respCode = ref->p_vtable->p_rxSyncCtrl(ref, msgP->bufDesc->dataP, msgCode);
            //Action complete - return response
            INU_COMM__TYPE_SET(msgP->hdr.control[SVC_COMG_CONTROL_IDX], INU_COMM__MSG_RESP_E);
            msgP->hdr.control[SVC_COMG_RESP_IDX] = respCode;
            inu_ref__ctrlSyncDone(ref, msgP->hdr.control[SVC_COMG_RESP_IDX],msgP->bufDesc);
         }
         else
         {
            respCode = ref->p_vtable->p_rxSyncCtrl(ref, msgP->bufDesc->dataP, msgCode);

            //Action complete - return response
            INU_COMM__TYPE_SET(msgP->hdr.control[SVC_COMG_CONTROL_IDX], INU_COMM__MSG_RESP_E);
            msgP->hdr.control[SVC_COMG_RESP_IDX] = respCode;

            //Return response to sender
            inu_comm__sendMsg(me,ref, msgP);
            msgP->bufDesc->nextP = NULL;
            MEM_POOLG_free(msgP->bufDesc);
         }
      }
      break;

      case (INU_COMM__MSG_DATA_E):
      {
         MEM_POOLG_bufDescT *msgDescP = msgP->bufDesc;
         MEM_POOLG_bufDescT *dataDescP = msgP->bufDesc->nextP;

         if (dataDescP)
            ref->p_vtable->p_rxAsyncData(ref, msgCode, msgDescP->dataP, dataDescP->dataP, dataDescP->dataLen, (void**)&dataDescP);
         else
            ref->p_vtable->p_rxAsyncData(ref, msgCode, msgDescP->dataP, NULL, 0, NULL);

         msgDescP->nextP = NULL;
         MEM_POOLG_free(msgDescP);
      }
      break;

      default:
      break;
   }
}


static ERRG_codeE inu_comm__parseMsg(inu_comm *me, inu_ref *ref, inu_comm__hdrT *hdrP, MEM_POOLG_bufDescT *chanBufDescP)
{
   ERRG_codeE ret;
   MEM_POOLG_bufDescT *mBufDescP;
   inu_comm__msgT msg;
   INT32 status;
   INUG_ioctlCmdE msgCode;

   memcpy(&msg.hdr, hdrP, sizeof(SVC_COMG_scomHdrT));
   msgCode = (INUG_ioctlCmdE)msg.hdr.control[SVC_COMG_ARG_IDX];

   //Allocate buffer for unmarshalling from interface
   ret = inu_ref__allocBuf(ref, MARSHALG_getSize(msgCode), &mBufDescP);
   if (ERRG_SUCCEEDED(ret))
   {
      MARSHALG_convertIoctl(MARSHALG_UNDO, msgCode, mBufDescP->dataP, chanBufDescP->dataP, &mBufDescP->dataLen);

      if (chanBufDescP->dataLen > MARSHALG_getMarshalSize(msgCode))
      {
         //If there is additional data in the buffer besides the message -chain with the unmarshalled buffer
         chanBufDescP->dataLen -= MARSHALG_getMarshalSize(msgCode);
         chanBufDescP->dataP += MARSHALG_getMarshalSize(msgCode);
         MEM_POOLG_chain(mBufDescP, chanBufDescP);
      }
      else
      {
         MEM_POOLG_free(chanBufDescP);
      }

      SVC_COMG_LENGTH_SET(msg.hdr.control[SVC_COMG_CONTROL_IDX], MEM_POOLG_getChainLen(mBufDescP)); //adjust legnth to account for marshalling 
      msg.bufDesc = mBufDescP;
      msg.ref = ref;
      if (inu_ref__dataChanDirect(ref))
      {
            inu_comm__handleMsg(me, &msg);
      }
      else
      {
         status = OS_LYRG_sendMsg(&threadData.thrMsgQue, (UINT8 *)&msg, sizeof(msg));
         if(status == FAIL_E)
         {
            ret = INU_COMM__ERR_OUT_OF_RSRCS; 
            MEM_POOLG_free(mBufDescP); 
         }
      }
   }
   else
   {
      //LOGG_PRINT(LOG_INFO_E, ret, "out of mem msgcode=0x%x size=%d pool size=%d num bufs=%d\n", msgCode, MARSHALG_getSize(msgCode),
      // MEM_POOLG_getBufSize(svcP->resc.localPoolH), MEM_POOLG_getNumBufs(svcP->resc.localPoolH));
      ret = INU_COMM__ERR_OUT_OF_MEM;
      MEM_POOLG_free(chanBufDescP);
   }

   return ret;
}

inline static void inu_comm__notifyAllNodes(inu_comm* me)
{
    for (int i = 0; i < INU_COMM__MAX_NUM_REGISTERED; i++)
    {
        if (me->registeredRef[i].assigned && me->registeredRef[i].ref)
        {
            inu_ref * ref = (inu_ref*)me->registeredRef[i].ref;
            //iinfo("gonna notify me(%p)->regRefList[%d].ref: %p %s", me, i, ref, inu_ref__getUserName(ref));
            inu_ref__ctrlSyncDone(ref, USB_CTRL_COM__ERR_LOST_CONNECTION, NULL);
        }
    }
}

static void inu_comm__connLyrRxCb(void *argP, INT32 refId, void *bufferP)
{
   inu_comm               *me = (inu_comm*)argP;
   inu_comm__hdrT          hdr;
   MEM_POOLG_bufDescT     *bufDescP = (MEM_POOLG_bufDescT *)bufferP;
   ERRG_codeE              ret;
   inu_ref                *ref = NULL;

   if (refId == -1 && bufferP == NULL)
   {
       inu_comm__notifyAllNodes(me);
       return;
   }

   OS_LYRG_lockMutex(&me->registeredRefListMutex);
   if (me->registeredRef[refId].assigned)
   {
      ref = (inu_ref*)me->registeredRef[refId].ref;
   }
   OS_LYRG_unlockMutex(&me->registeredRefListMutex);
   
   if (ref)
   {
      //Parse header
      inu_comm__parseHeader(bufDescP->dataP, &hdr);

      //Advance data pointer in buffer
      bufDescP->dataP += inu_comm__hdrMarshalSize;
      bufDescP->dataLen -= inu_comm__hdrMarshalSize;

      ref->stats.chCnt++;
      ret = inu_comm__parseMsg(me, ref, &hdr, (MEM_POOLG_bufDescT*)bufferP);
      if (ERRG_FAILED(ret))
      {
         ref->stats.chDropCnt++;
      }
   }
   else
   {
      MEM_POOLG_free((MEM_POOLG_bufDescT*)bufDescP);
   }
}

static void inu_comm__connLyrTxCb(void *argP, INT32 refId, void *bufferP)
{
   inu_comm *me = (inu_comm*)argP;
   inu_ref *ref;

   OS_LYRG_lockMutex(&me->registeredRefListMutex);
   if (me->registeredRef[refId].assigned)
   {
      ref = (inu_ref*)me->registeredRef[refId].ref;
      ref->p_vtable->p_txDone(ref, bufferP);
      MEM_POOLG_free((MEM_POOLG_bufDescT*)bufferP);    //always free tx buffer
   }
   OS_LYRG_unlockMutex(&me->registeredRefListMutex);

}


ERRG_codeE inu_comm__register(inu_comm *me, inu_ref *ref)
{
   ERRG_codeE ret = INU_COMM__RET_SUCCESS;

   //mark entry
   OS_LYRG_lockMutex(&me->registeredRefListMutex);
   if (!me->registeredRef[ref->id].assigned)
   {
      me->registeredRef[ref->id].assigned = 1;
      me->registeredRef[ref->id].ref = ref;
   }
   OS_LYRG_unlockMutex(&me->registeredRefListMutex);

   return ret;
}

void inu_comm__unregister(inu_comm *me, inu_ref *ref)
{
   OS_LYRG_lockMutex(&me->registeredRefListMutex);
   me->registeredRef[ref->id].assigned = 0;
   me->registeredRef[ref->id].ref = NULL;
   OS_LYRG_unlockMutex(&me->registeredRefListMutex);
}


ERRG_codeE inu_comm__sendMsg(inu_comm *me, inu_ref *ref, inu_comm__msgT *msgP)
{
   ERRG_codeE     ret = SVC_MNGR__ERR_OUT_OF_MEM;
   MEM_POOLG_bufDescT *chanBufDescP;
   UINT32 chanBufSize;
   INUG_ioctlCmdE msgCode = (INUG_ioctlCmdE)msgP->hdr.control[INU_COMM__ARG_IDX];

   chanBufSize = MARSHALG_getMarshalSize(msgCode) + SVC_COMG_getFullHdrMarshalSize();
   ret = inu_ref__timedAllocBuf(ref, chanBufSize, 15, &chanBufDescP);
   if (ERRG_SUCCEEDED(ret))
   {
      chanBufDescP->dataP += SVC_COMG_getFullHdrMarshalSize();
      if (MARSHALG_getMarshalSize(msgCode))
      {
         MARSHALG_convertIoctl(MARSHALG_DO, msgCode, msgP->bufDesc->dataP, chanBufDescP->dataP, &chanBufDescP->dataLen);
      }

      if ((msgP->bufDesc) && (msgP->bufDesc->nextP))
         MEM_POOLG_chain(chanBufDescP, msgP->bufDesc->nextP);

      SVC_COMG_LENGTH_SET(msgP->hdr.control[SVC_COMG_CONTROL_IDX], MEM_POOLG_getChainLen(chanBufDescP)); //adjust legnth to account for marshalling 

      //Move back data pointer in buffer for header
      chanBufDescP->dataP -= inu_comm__hdrMarshalSize;
      chanBufDescP->dataLen += inu_comm__hdrMarshalSize;

      //Add header to buffer assuming pre-allocated header room
      inu_comm__writeHeader(chanBufDescP->dataP, &msgP->hdr);

      ret = CONN_LYRG_send(me->conn, ref->id, chanBufDescP, (UINT32)ref->id,msgP->hdr.control[INU_COMM__ARG_IDX]); //remove txid if not needed
      if (ERRG_FAILED(ret))
      {
         MEM_POOLG_free(chanBufDescP);
         ref->stats.chDropCnt++;
      }
   }
   else
   {
      //printf("inu_comm__sendMsg no buffers free.\n");
   }
   return ret;
}

ERRG_codeE inu_comm__sendIoctl(inu_comm *me, inu_ref *ref, int cmd, void *argP)
{
   inu_comm__msgT msg;
   (void)me;
   UINT32              len;
   MEM_POOLG_bufDescT *bufDescP;
   ERRG_codeE     ret = SVC_MNGR__RET_SUCCESS;
#if DEFSG_IS_GP
   INT32 status;
#endif

   len = MARSHALG_getSize((INUG_ioctlCmdE)cmd);
   ret = inu_ref__allocBuf(ref, len, &bufDescP);

   if (ERRG_SUCCEEDED(ret) && argP)
   {
      memcpy(bufDescP->dataP,argP,len);
      bufDescP->dataLen += len;
      msg.bufDesc = bufDescP;
   }
   else
   {
      msg.bufDesc = NULL;
   }

   INU_COMM__TYPE_SET(msg.hdr.control[INU_COMM__CONTROL_IDX], INU_COMM__MSG_IOCTL_E);
   INU_COMM__LENGTH_SET(msg.hdr.control[INU_COMM__CONTROL_IDX], len);
   INU_COMM__PROCID_SET(msg.hdr.control[INU_COMM__CONTROL_IDX], INU_COMM__PROC_ID);
   msg.hdr.control[INU_COMM__ARG_IDX] = cmd;
   msg.hdr.control[INU_COMM__RESP_IDX] = 0; //used in response type
#if DEFSG_IS_GP
   // There are only loopback IOCTL in gp. Ioctl won't be send to host
   msg.ref=ref;
   status = OS_LYRG_sendMsg(&threadData.thrMsgQue, (UINT8 *)&msg, sizeof(msg));
   if(status == FAIL_E)
   {
       msg.hdr.control[SVC_COMG_RESP_IDX] = INU_COMM__ERR_SEND_QUEUE;
   }
   // Loop back message behavior need to be like we received from host
   // ctrlSyncDone send event and releases the message. Need duplicate here.
   MEM_POOLG_duplicate(msg.bufDesc);
#else
   ret = inu_comm__sendMsg(me, ref, &msg);
#endif
   bufDescP->nextP = NULL; //unchain before free
   MEM_POOLG_free(bufDescP);
   return ret;
}


/*
   Chain the msg buffer descriptor to the mbuf or the buffer.
   if mbuf is null, allocate and copy from buffer and then chain.
*/
static ERRG_codeE inu_comm__chainMsgAndBuffer(inu_ref *ref, MEM_POOLG_bufDescT *msg, MEM_POOLG_bufDescT *mbuf, UINT8 *buf, UINT32 bufLen)
{
   ERRG_codeE ret = INU_COMM__RET_SUCCESS;
   MEM_POOLG_bufDescT *databuf = NULL;

   if((!mbuf) && (buf && (bufLen > 0)))
   {
    /*  //find pool with minimum size for this msg
      ret = inu_ref__getMinMempool(ref, bufLen, &poolH);
      if(ERRG_FAILED(ret))
      {
         printf("inu_comm__chainMsgAndBuffer: get mempool failed\n");
         return ret;
      }

      //Need to allocate mem pool buffer and copy
      ret = inu_comm__timedAlloc(poolH, 0, bufLen, &databuf);*/
      ret = inu_ref__timedAllocBuf(ref,bufLen,0,&databuf);

      if(ERRG_SUCCEEDED(ret))
      {
         memcpy(databuf->dataP, buf, bufLen);
         databuf->dataLen += bufLen;
      }
      else
      {
         printf("inu_comm__chainMsgAndBuffer: alloc failed\n");
      }
      mbuf = databuf;
   }

   if(ERRG_SUCCEEDED(ret))
   {
      MEM_POOLG_chain(msg, mbuf);
   }
   
   return ret;
}

UINT32 inu_comm__isUvcType(inu_ref *ref)
{
    inu_comm *comm;
    comm = (inu_comm *)inu_ref__getDataComm(ref);
    if (comm != NULL)
        if ((comm->id >= INU_FACTORY__CONN_UVC1_E) && (comm->id <= INU_FACTORY__CONN_UVC_MAX_E))
        {
            return 1;
        }
    return 0;
}

ERRG_codeE inu_comm__sendData(inu_comm *me, inu_ref *ref, int cmd, void *argP, MEM_POOLG_bufDescT *dataBufDescP, void *bufferP, int bufferLen)
{
   inu_comm__msgT msg;
   UINT32              len;
   MEM_POOLG_bufDescT *bufDescP;
   ERRG_codeE     ret = SVC_MNGR__RET_SUCCESS;
#if DEFSG_IS_GP
   INT32 status;
#endif

   if (inu_ref__standAloneModeCodeIgnr(ref,cmd))
      return INU_COMM__ERR_NOT_SUPPORTED;
   len = MARSHALG_getSize((INUG_ioctlCmdE)cmd);
   ret = inu_ref__timedAllocBuf(ref,len,0,&bufDescP);
   if (ERRG_SUCCEEDED(ret) && argP)
   {
      memcpy(bufDescP->dataP, argP, len);
      bufDescP->dataLen += len;
      msg.bufDesc = bufDescP;
   }
   else
   {
      printf("inu_comm__sendData: alloc failed! len = %d cmd %x\n",len,cmd);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      ret = inu_comm__chainMsgAndBuffer(ref,bufDescP, dataBufDescP, (UINT8*)bufferP,bufferLen);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      INU_COMM__TYPE_SET(msg.hdr.control[INU_COMM__CONTROL_IDX], INU_COMM__MSG_DATA_E);
      INU_COMM__LENGTH_SET(msg.hdr.control[INU_COMM__CONTROL_IDX], MEM_POOLG_getChainLen(bufDescP));
      INU_COMM__PROCID_SET(msg.hdr.control[INU_COMM__CONTROL_IDX], INU_COMM__PROC_ID);
      msg.hdr.control[INU_COMM__ARG_IDX] = cmd;
      msg.hdr.control[INU_COMM__RESP_IDX] = 0; //used in response type
#if DEFSG_IS_GP
      // standalone mode, dont send to host. TODO - using binding functions
      if (inu_ref__standAloneMode(ref) && (cmd != INTERNAL_CMDG_IMAGE_SEND_E))
      {
         msg.ref=ref;
         status = OS_LYRG_sendMsg(&threadData.thrMsgQue, (UINT8 *)&msg, sizeof(msg));
         if(status == FAIL_E)
         {
             msg.hdr.control[SVC_COMG_RESP_IDX] = INU_COMM__ERR_SEND_QUEUE;
         }
      }
      else
      {
         ret = inu_comm__sendMsg(me, ref, &msg);
         bufDescP->nextP = NULL; //unchain before free
         MEM_POOLG_free(bufDescP);
      }
#else   
      ret = inu_comm__sendMsg(me, ref, &msg);
      bufDescP->nextP = NULL; //unchain before free
      MEM_POOLG_free(bufDescP);
#endif
   }
   else
   {
      printf("error sending data for %s\n",inu_ref__getUserName(ref));
   }
   return ret;
}

void inu_comm__thread(void *argP)
{
   inu_comm__threadData *thrDataP = (inu_comm__threadData*)argP;
   INT32 status;
   inu_comm__msgT msg;
   UINT32 msgSize = 0;

   while(thrDataP->refCtr)
   {
      msgSize = sizeof(msg);
      status = OS_LYRG_recvMsg(&threadData.thrMsgQue, (UINT8*)&msg, &msgSize, INU_COMM__THR_SLEEP_MSEC);
      if((status == SUCCESS_E)&& (msgSize > 0))
      {
         inu_comm__handleMsg(threadData.comm, &msg);
      }
   }
}

/* Constructor */
ERRG_codeE inu_comm__ctor(inu_comm *me, const inu_comm__CtorParams *ctorParamsP)
{
   ERRG_codeE ret = INU_COMM__RET_SUCCESS;
   int status;
   CONN_LYRG_openCfgT *connCfgP;

   memset(me, 0x00, sizeof(inu_comm));
   inu_comm__hdrMarshalSize = inu_comm__marshalHdr(MARSHALG_DO, NULL, NULL);
   connCfgP = (CONN_LYRG_openCfgT*)inu_factory__getConnConfig((inu_factory__connIdE)ctorParamsP->id);
   connCfgP->commP = me;
   connCfgP->timedAllocBufCb = inu_comm__timedAllocBuffByRefId;

   //Mutex for accessing mempool list
   status = OS_LYRG_aquireMutex(&me->registeredRefListMutex);
   if (status != SUCCESS_E)
      assert(0);


   ret = CONN_LYRG_open((CON_LYRG_handleT*)&(me->conn), connCfgP, ctorParamsP->id);
   if (ERRG_SUCCEEDED(ret))
   {
      me->id = ctorParamsP->id;
      CONN_LYRG_rxRegister(me->conn, inu_comm__connLyrRxCb, me);
      CONN_LYRG_txRegister(me->conn, inu_comm__connLyrTxCb, me);

      //create thread for invoking messages callbacks
      if (!threadData.refCtr)
      {
         OS_LYRG_threadParams      thrdParams;
         memcpy(threadData.thrMsgQue.name, INU_COMM__THR_MSG_Q_NAME, MIN(sizeof(threadData.thrMsgQue.name), strlen(INU_COMM__THR_MSG_Q_NAME)+1));
         threadData.thrMsgQue.maxMsgs = INU_COMM__THR_MSG_Q_BUFS;
         threadData.thrMsgQue.msgSize = sizeof(inu_comm__msgT);

         status = OS_LYRG_createMsgQue(&threadData.thrMsgQue, OS_LYRG_MULTUPLE_EVENTS_NUM_E);
         if (status)
         {
            return INU_COMM__ERR_OUT_OF_RSRCS;
         }

         thrdParams.func  = (OS_LYRG_threadFunction)inu_comm__thread;
         thrdParams.param = &threadData;
         thrdParams.id    = OS_LYRG_COMM_THREAD_ID_E;
         thrdParams.event = NULL;

         threadData.refCtr++;
         threadData.comm   = me;
         threadData.thrHandle = OS_LYRG_createThread(&thrdParams);
      }
      else
      {
         threadData.refCtr++;
      }
   }
   return ret;
}

void inu_comm__delete(inu_comm *me)
{
   threadData.refCtr--;

   CONN_LYRG_close((CON_LYRG_handleT*)me->conn);

   OS_LYRG_releaseMutex(&me->registeredRefListMutex);
   if (me)
      free(me);
}

ERRG_codeE inu_comm__new(inu_comm **comm, const inu_comm__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   *comm = (inu_comm*)malloc(sizeof(inu_comm));
   if (*comm)
   {
      ret = inu_comm__ctor(*comm, ctorParamsP);
   }
   else
   {
      ret = INU_COMM__ERR_OUT_OF_MEM;
   }

   return ret;
}

UINT32 inu_comm__getFullHdrMarshalSize(void)
{
   return inu_comm__hdrMarshalSize + CONN_LYRG_getHdrMarshalSize();
}

ERRG_codeE inu_comm__timedAllocBuffByRefId(void *me, UINT32 refId, UINT32 len, UINT32 timeoutMsec, void **bufDescP)
{
   ERRG_codeE retCode = INU_COMM__RET_SUCCESS;
   inu_comm *commP = (inu_comm*)me;

   if (!commP)
      return INU_COMM__ERR_NULL_PTR;

   if(refId<INU_COMM__MAX_NUM_REGISTERED)
   {
      if (commP->registeredRef[refId].assigned)
      {
         retCode = inu_ref__timedAllocBuf((inu_ref*)commP->registeredRef[refId].ref, len, timeoutMsec, (MEM_POOLG_bufDescT**)bufDescP);
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "refId is not assigned. refId = %d len = %d\n",refId, len);
         retCode = INU_COMM__ERR_UNEXPECTED;
      }
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "illegal refId. refId = %d len = %d\n",refId, len);
      retCode = INU_COMM__ERR_UNEXPECTED;
   }
   return retCode;
}


#ifdef __cplusplus
}
#endif
