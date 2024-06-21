#ifdef ENABLE_UVC
/****************************************************************************
 *
 *   FileName: uvc_ctrl_com.c
 *
 *   Author:
 *
 *   Date:
 *
 *   Description: UVC control com interface
 *
 ****************************************************************************/
/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_types.h"
#include "err_defs.h"
#if DEFSG_IS_GP
#include "log.h"

#include "uvc_com.h"
#include "mem_map.h"
#include "cmem.h"
#include "inu_factory.h"
#include "inu_function.h"

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <errno.h>
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define UVC_NUM_INTERFACE 16

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef struct
{
  UINT8 buf[1024];
  UINT32 cnt;
  UINT32 idx;
} UVC_COMP_bufT;

/*typedef struct
{
   UVC_COMG_paramsT param;
   int fd;
} UVC_COMP_interfaceInfT;
*/
#define PG_CMD_WRITE _IOW('P', 1, int)
#define PG_CMD_READ  _IOR('P', 2, int)
#define PG_CMD_INTERRUPT  _IOR('P', 3, int)


typedef struct {
   UINT32 physAddr;
   UINT32 size;
} UVC_COMP_ioctlArgT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static UVC_COMP_bufT remBuf;
UVC_COMP_interfaceInfT interfaceInfTbl[UVC_NUM_INTERFACE];
typedef struct uvcRlsParams
{
    UINT32 mycount;
    void *buffer_pointer;
    void *bufP;

}TuvcRlsParams1;

#define MAX_BUFF_FREE 20
TuvcRlsParams buffersUsed[UVC_NUM_OF_DEVICES][MAX_BUFF_FREE];

static TuvcRlsParams1   buffersUsed3[6];

extern UINT32 UVC_MAIN_freeCount[];
extern UINT32 UVC_MAIN_shutterBuff;
extern unsigned int startCapturing[];
extern unsigned int forcePic[];

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/
/******************************************************************************
 ***************     P R E    D E F I N I T I O N     OF        ***************
 ***************     L O C A L         F U N C T I O N S        ***************
 ******************************************************************************/
static ERRG_codeE UVC_COMP_open(void **connHandle, void *openCfg);
static ERRG_codeE UVC_COMP_close(void *connHandle);
static ERRG_codeE UVC_COMP_send(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenSendP);
static ERRG_codeE UVC_COMP_sendv(void *connHandle,  void *bufP, UINT32 *bytesCntP);
static ERRG_codeE UVC_COMP_recv(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenRecvP);
//static ERRG_codeE UVC_COMP_peek(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenRecvP);
void *inu_factory__getConnIfCfg(inu_factory__connIdE id);
void CONN_LYRG_txBuf(void *entry, void *bufP, INT32 chanNum, UINT32 txid);

void *getUvcService(UVC_COMP_interfaceInfT *connHandle)
{
}

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/


/****************************************************************************
*
*  Function Name: UVC_COMP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE UVC_COMP_open(void **connHandle, void *openCfg)
{
   int totalSleep = 0;
   char str[80];

  //  return USB_CTRL_COM__RET_SUCCESS;
    UVC_COMG_paramsT *cfgP = (UVC_COMG_paramsT *)openCfg;
    LOGG_PRINT(LOG_DEBUG_E, NULL, "\n");


    cfgP->uvc_service = interfaceInfTbl[cfgP->uvcInterfaceIdx].param.uvc_service;

    if (cfgP->uvc_service)
    	LOGG_PRINT(LOG_INFO_E, NULL, "device opened  fd=%d\n",cfgP->uvc_service->fd);

    memset(&remBuf,0,sizeof(remBuf));
    interfaceInfTbl[cfgP->uvcInterfaceIdx].param = *cfgP;
    if (cfgP->uvc_service)
        interfaceInfTbl[cfgP->uvcInterfaceIdx].fd = cfgP->uvc_service->fd;
    *connHandle = (void *)(&interfaceInfTbl[cfgP->uvcInterfaceIdx]);
    return USB_CTRL_COM__RET_SUCCESS;

}

/****************************************************************************
*
*  Function Name: UVC_COMP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
#include <stropts.h>

ERRG_codeE UVC_COMP_close(void *connHandle)
{
   int fd = ((UVC_COMP_interfaceInfT *)connHandle)->fd;
   UVC_COMG_paramsT *cfgP = &((UVC_COMP_interfaceInfT *)connHandle)->param;

   LOGG_PRINT(LOG_DEBUG_E, NULL, "\n");

  return UVC_COM__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: UVC_COMP_send
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE UVC_COMP_sendPhy(void *connHandle, UINT8* bufP, UINT32 len, UINT32 *byteLenSendP)
{
   UVC_COMP_ioctlArgT arg;
   ERRG_codeE  retVal = UVC_COM__RET_SUCCESS;
   int result = 0;
   int fd = (int)((UVC_COMP_interfaceInfT *)connHandle)->fd;
#if 0
   //LOGG_PRINT(LOG_DEBUG_E, NULL, "\n");
   //LOGG_PRINT(LOG_DEBUG_E, NULL, "device send fd=%d bufP=%x len=%d\n", fd, bufP,len);
   if(fd < 0)
   {
      return UVC_COM__ERR_DEV_NOT_INIT;
   }

   if (bufP)
   {
      arg.physAddr = (UINT32)bufP;
      arg.size = len;
      if((result = ioctl (fd, PG_CMD_WRITE, &arg))  != 0)
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "device send failed result=%d fd=%d bufP=0x%x len=%d\n", result, fd, bufP, len);
         retVal = UVC_COM__ERR_WRITE_FAILED;
      }
      *byteLenSendP = len;
      return retVal;
   }
#endif
//   LOGG_PRINT(LOG_DEBUG_E, NULL, "-->done send %d\n", *byteLenSendP );
   return retVal;
}


/****************************************************************************
*
*  Function Name: UVC_COMP_send
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE UVC_COMP_send(void *connHandle, UINT8* bufP, UINT32 len, UINT32 *byteLenSendP)
{
   ERRG_codeE  retVal = UVC_COM__RET_SUCCESS;
   int result = 0;
   int fd = (int)((UVC_COMP_interfaceInfT *)connHandle)->fd;

   return UVC_COM__RET_SUCCESS;
}

void UVC_MAING_freeStream(void *ptr, int uvcDevice)
{
   unsigned int ind,currCount = 0;

   for (ind = 0; ind < MAX_BUFF_FREE; ind++)
   {
      if (buffersUsed[uvcDevice][ind].buffer_pointer == ptr)
      {
         inu_streamer__free(buffersUsed[uvcDevice][ind].stream, buffersUsed[uvcDevice][ind].data);
         buffersUsed[uvcDevice][ind].buffer_pointer = 0;
         break;
      }
   }
   if (UVC_MAIN_shutterBuff == (UINT32)ptr)
   {
      UVC_MAIN_freeCount[uvcDevice]++;
   }
   if (ind == 6)
   {
        return;
   }
/*   for (ind=0;ind<6;ind++)
   {
      if (buffersUsed[ind].buffer_pointer)
      {
         printf("----------ERROR buffer not freed\n");
         inu_streamer__free(buffersUsed[ind].stream, buffersUsed[ind].data);
         buffersUsed[ind].buffer_pointer = 0;
      }

   }*/
}

ERRG_codeE UVC_COMP_sendv(void *connHandle,  void *params, UINT32 *bytesCntP)
{
   UINT32 byteCnt = 0;
   UINT32 byteCntTotal = 0;
   ERRG_codeE ret = 0;
   UVC_COMP_interfaceInfT *conn = (UVC_COMP_interfaceInfT *)connHandle;
   TuvcRlsParams *uvcRlsParms = (TuvcRlsParams *)params;
   unsigned int buffer_data,buf_size=uvcRlsParms->bufSize,bytes_used;
   void *buffer_pointer = NULL;
   static int ind = -1;

   if (ind == -1)
   {
      memset(buffersUsed, 0, sizeof(buffersUsed));
      ind = 0;
   }

   buffer_data=(unsigned int)uvcRlsParms->buffer_pointer;
   buffer_pointer = (void *) buffer_data;

   buffersUsed[conn->param.uvcInterfaceIdx][ind].stream = uvcRlsParms->stream;
   buffersUsed[conn->param.uvcInterfaceIdx][ind].data = uvcRlsParms->data;
   buffersUsed[conn->param.uvcInterfaceIdx][ind].buffer_pointer = uvcRlsParms->buffer_pointer;
   bytes_used = uvcRlsParms->bufSize;
   // printf("TEST   ===== %x %x %x\n",buffersUsed[ind].stream,buffersUsed[ind].data,buffersUsed[ind].buffer_pointer);

   forcePic[conn->param.uvcInterfaceIdx] = 0;
   UVC_MAING_callPushDataToUvc(UVC_MAING_getUvcDevice(conn->param.uvcInterfaceIdx), (unsigned long *)&buffer_data, (int *)&buf_size, (int *)&bytes_used, (void **)&buffer_pointer);

   inu_function__complete(uvcRlsParms->stream);
   ind = (ind + 1) % MAX_BUFF_FREE;

   return ret;
}

/****************************************************************************
*
*  Function Name: UVC_COMP_recv
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE UVC_COMP_recv(void *connHandle, UINT8* bufP, UINT32 len, UINT32 *byteLenRecvP)
{
   enum {CACHE_INV_SIZE = 256};
   ERRG_codeE  retVal = UVC_COM__RET_SUCCESS;
   int fd =(int)((UVC_COMP_interfaceInfT *)connHandle)->fd;
   int result = 0;
   UINT8 *readP;
   UINT32 rem;
   UINT32 lenToRead;
   UINT32 remReadCnt = 0;
   return retVal;
}



/****************************************************************************
*
*  Function Name: UVC_COMP_recvPhy
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE UVC_COMP_recvDma(void *connHandle, UINT8* bufPhyP, UINT8* bufP, UINT32 len, UINT32 *byteLenRecvP)
{
   UVC_COMP_ioctlArgT arg;
   enum {CACHE_INV_SIZE = 256};
   ERRG_codeE  retVal = UVC_COM__RET_SUCCESS;
   int fd =(int)((UVC_COMP_interfaceInfT *)connHandle)->fd;
   int result = 0;
   UINT8 *readP;
   UINT32 physAddr = 0;
   UINT32 rem;
   UINT32 lenToRead;
   UINT32 remReadCnt = 0;

   return UVC_COM__ERR_DEV_NOT_INIT;
}

void UVC_COMP_getCaps(void *connHandle, CLS_COMG_capsT *capsP)
{
   UVC_COMP_interfaceInfT *pInf = (UVC_COMP_interfaceInfT *)connHandle;
   capsP->flags  =  0;
   capsP->fixedSize = 0;
   capsP->flags |= CLS_COMG_CAP_MODE_UVC;
   if (pInf->param.rxCap)
   {
      capsP->flags |= CLS_COMG_CAP_RX;
   }
   if (pInf->param.txCap)
   {
      capsP->flags |= CLS_COMG_CAP_TX;
   }
   if (!pInf->param.fixedSizeCap)
   {
      capsP->flags |= CLS_COMG_CAP_MODE_STREAM;
   }
   else
   {
      capsP->fixedSize = pInf->param.fixedBuffSize;
   }
   capsP->flags |= (CLS_COMG_CAP_SENDV | CLS_COMG_CAP_RX_CONT_RECV);
}


char *uvc_com_getSrcStream(unsigned int id)
{
   UVC_COMG_paramsT *cfgP;
   if (id > (INU_FACTORY__CONN_UVC_MAX_E - INU_FACTORY__CONN_UVC1_E))
       return NULL;
   cfgP = (UVC_COMG_paramsT *)inu_factory__getConnIfCfg(id+INU_FACTORY__CONN_UVC1_E);
   if (!cfgP)
       return NULL;
   else
   {
       return (cfgP->inu_ref);
   }
}

void uvc_com_Setsrc(void *inu_ref,void *ifaceCfg)
{
    UVC_COMG_paramsT *cfgP = (UVC_COMG_paramsT *)ifaceCfg;
    cfgP->inu_ref = inu_ref;
}


unsigned int UVC_VIDEO_findStreamNum(char *name)

{
    FILE *fp = NULL;
    unsigned int ind,rc;
    char cmd[128],str[128];

     for (ind = 0; ind < 8; ind++)
     {
         snprintf(cmd, sizeof(cmd), "cat /sys/kernel/config/usb_gadget/g1/functions/uvc.usb%d/uvc_video_name", ind);
         fp = popen(cmd, "r");
         if (fp)
         {
             if (fgets(str, sizeof(str), fp))
              {
                 if (rc=strncmp(name,str,strlen(name))==0)
                 {
                     pclose(fp);
                     return ind;
                 }

              }
             pclose(fp);
         }
     }
     return 0;

}

/****************************************************************************
*
*  Function Name: UVC_COMG_getOps
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void UVC_COMG_getOps(CLS_COMG_ifOperations *ifOpsP)
{
   //OpsP->init     = &UVC_COMP_init;
   ifOpsP->open        = &UVC_COMP_open;
   ifOpsP->close       = &UVC_COMP_close;
   ifOpsP->getCaps     = &UVC_COMP_getCaps;
   ifOpsP->send        = &UVC_COMP_send;
   ifOpsP->sendv       = &UVC_COMP_sendv;
   ifOpsP->recv        = &UVC_COMP_recv;
   ifOpsP->recvContBuf = &UVC_COMP_recvDma;
   ifOpsP->peek        = NULL;
    ifOpsP->recv_unbuffered = NULL;
   ifOpsP->sendv_unbuffered = NULL;
}

#ifdef __cplusplus
}
#endif
#endif
#endif