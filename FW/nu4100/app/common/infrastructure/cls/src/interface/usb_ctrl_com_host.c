/****************************************************************************
 * 
 *   FileName: usb_ctrl_com.c
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: USB control com interface 
 *
 ****************************************************************************/
/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"
#include "usb_ctrl_com.h"
#include "internal_cmd.h"

#if DEFSG_IS_HOST
#include "inu_usb.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define  USB_CTRL_COMP_TARGET_KERNEL_BUF_SIZE   (128*1024)
#define USB_NUM_INTERFACE 7

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef struct
{
   USB_CTRL_COMG_paramsT param;
   void* handle;
} USB_CTRL_COMP_interfaceInfT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
USB_CTRL_COMP_interfaceInfT interfaceInfTbl[USB_NUM_INTERFACE];

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/******************************************************************************
 ***************     P R E    D E F I N I T I O N     OF        ***************
 ***************     L O C A L         F U N C T I O N S        ***************
 ******************************************************************************/
static ERRG_codeE USB_CTRL_COMP_open(void **connHandle, void *openCfg);
static ERRG_codeE USB_CTRL_COMP_close(void *connHandle);
static ERRG_codeE USB_CTRL_COMP_send(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenSendP);
static ERRG_codeE USB_CTRL_COMP_sendv(void *connHandle,  void *bufP, UINT32 *bytesCntP);
static ERRG_codeE USB_CTRL_COMP_recv(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenRecvP);

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: USB_CTRL_COMP_open
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
ERRG_codeE USB_CTRL_COMP_open(void **connHandle, void *openCfg)
{
   USB_CTRL_COMG_paramsT *cfgP = (USB_CTRL_COMG_paramsT *)openCfg;
    interfaceInfTbl[cfgP->usbInterfaceIdx].param = *cfgP;
   *connHandle = (void *)(&interfaceInfTbl[cfgP->usbInterfaceIdx]);

   if (inu_usb_open(cfgP->usbInterfaceIdx) != INU_USB_ERR_SUCCESS)
      return USB_CTRL_COM__ERR_IO_ERROR;

   return USB_CTRL_COM__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: USB_CTRL_COMP_close
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
ERRG_codeE USB_CTRL_COMP_close(void *connHandle)
{
   USB_CTRL_COMP_interfaceInfT *pInf = (USB_CTRL_COMP_interfaceInfT *)connHandle;

   if (inu_usb_close(pInf->param.usbInterfaceIdx) != INU_USB_ERR_SUCCESS)
      return USB_CTRL_COM__ERR_IO_ERROR;

   return USB_CTRL_COM__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: USB_CTRL_COMP_send
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
ERRG_codeE USB_CTRL_COMP_send(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenSendP)
{
   InuUsbErrorE err;
   USB_CTRL_COMP_interfaceInfT *pInf = (USB_CTRL_COMP_interfaceInfT *)connHandle;

#ifdef INU_PROFILER_USB_HOST
   INUG_profiler_get_timestamp();
#endif   

   //printf ("sending: len = %d \n",len);
 //  err = inu_usb_write_bulk((unsigned char *)pBuff, len, byteLenSendP);
   err = inu_usb_write_bulk(pInf->param.usbInterfaceIdx,(unsigned char *)pBuff, len, byteLenSendP);
   //printf ("sending: len = %d *byteLenRecvP = %d (err=%d)\n",len, *byteLenSendP,err);
   if (err == INU_USB_ERR_SUCCESS) {
#ifdef INU_PROFILER_USB_HOST
     INUG_profiler_get_timestamp();
#endif
      return USB_CTRL_COM__RET_SUCCESS;
   }
   else if (err == INU_USB_ERR_DISCONNECTED) {
#ifdef INU_PROFILER_USB_HOST
       INUG_profiler_get_timestamp();
#endif      
      return CONN_LYR__ERR_CONNECTION_IS_DOWN;
   } else {
#ifdef INU_PROFILER_USB_HOST
      INUG_profiler_get_timestamp();
#endif     
      return USB_CTRL_COM__ERR_IO_ERROR;
   }
}

/****************************************************************************
*
*  Function Name: USB_CTRL_COMP_sendv
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
ERRG_codeE USB_CTRL_COMP_sendv(void *connHandle,  void *bufP, UINT32 *bytesCntP)
{
   ERRG_codeE           ret            = USB_CTRL_COM__ERR_IO_ERROR;
   UINT32               byteCnt        = 0;
   UINT32               byteCntTotal   = 0;
   MEM_POOLG_bufDescT   *currP         = (MEM_POOLG_bufDescT *)bufP;

   //This is a naive implementation.
   //TODO: use scatter-gather mechanism when supported by driver
   if(currP)
   {
      if(currP->dataLen > USB_CTRL_COMP_TARGET_KERNEL_BUF_SIZE)
      {
         UINT32 firstPcktSize  = 64;//SVC_COMG_getFullHdrMarshalSize() + sizeof(INTERNAL_CMDG_injectFrameHdrT); // bennyv - should be investigated why cannot send any packet size

         byteCnt = 0;
         ret = USB_CTRL_COMP_send(connHandle, currP->dataP, firstPcktSize, &byteCnt);
         byteCntTotal += byteCnt;
         byteCnt = 0;
         ret = USB_CTRL_COMP_send(connHandle, currP->dataP+firstPcktSize, currP->dataLen-firstPcktSize, &byteCnt);
         byteCntTotal += byteCnt;
         currP = currP->nextP;
      }
   }

   while(currP)
   {
      byteCnt = 0;
      //Copying to buffer before send due to performance issue in sending on socket from external buffer
      ret = USB_CTRL_COMP_send(connHandle, currP->dataP, currP->dataLen, &byteCnt);
      if(ERRG_FAILED(ret))
      {
         break;
      }
      byteCntTotal += byteCnt;

      currP = currP->nextP;
   }

   *bytesCntP = byteCntTotal;

   return ret;
}

/****************************************************************************
*
*  Function Name: USB_CTRL_COMP_recv
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
ERRG_codeE USB_CTRL_COMP_recv(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenRecvP)
{
   ERRG_codeE     retVal   = USB_CTRL_COM__RET_SUCCESS;
   unsigned int   readLen  = 0;
   InuUsbErrorE   err;
   USB_CTRL_COMP_interfaceInfT *pInf = (USB_CTRL_COMP_interfaceInfT *)connHandle;

#ifdef INU_PROFILER_USB_HOST
   INUG_profiler_get_timestamp();
#endif   
   if(len)
   {
      *byteLenRecvP = 0;
      while(*byteLenRecvP < len)
      {
      // printf ("USB_CTRL_COMP_recv: len = %d, *byteLenRecvP = %d\n",len, readLen, *byteLenRecvP);
#ifdef INU_PROFILER_USB_HOST     
      INUG_profiler_get_timestamp();
#endif     
         err = inu_usb_buffered_read_bulk(pInf->param.usbInterfaceIdx, pBuff + *byteLenRecvP, len-*byteLenRecvP, &readLen);
#ifdef INU_PROFILER_USB_HOST
      INUG_profiler_get_timestamp();
#endif     
   //gettimeofday(&time_end, NULL);
         if (err == INU_USB_ERR_SUCCESS)
         {
            *byteLenRecvP+= readLen;
            // printf ("len = %d readLen =%d *byteLenRecvP = %d usbRetVal = %d\n",len, readLen, *byteLenRecvP, err);
            readLen=0;
         }
         else if (err == INU_USB_ERR_DISCONNECTED)
         {
            //printf ("usb is disconnected ... \n");
#ifdef INU_PROFILER_USB_HOST
            INUG_profiler_get_timestamp();
#endif         
            return USB_CTRL_COM__ERR_LOST_CONNECTION;
         }
         else
         {            
            LOGG_PRINT(LOG_ERROR_E, NULL, "inu_usb_lib: readpipe failed (rc=%d)\n",err);
#ifdef INU_PROFILER_USB_HOST
            INUG_profiler_get_timestamp();
#endif          
            return USB_CTRL_COM__ERR_READ_FAILED;
         }
      }
   }
#ifdef INU_PROFILER_USB_HOST   
   INUG_profiler_get_timestamp();
#endif   
   return retVal;
}
ERRG_codeE USB_CTRL_COMP_recvUnBuffered(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenRecvP)
{
   ERRG_codeE     retVal   = USB_CTRL_COM__RET_SUCCESS;
   unsigned int   readLen  = 0;
   InuUsbErrorE   err;
   USB_CTRL_COMP_interfaceInfT *pInf = (USB_CTRL_COMP_interfaceInfT *)connHandle;

#ifdef INU_PROFILER_USB_HOST
   INUG_profiler_get_timestamp();
#endif   
   if(len)
   {
      *byteLenRecvP = 0;
      while(*byteLenRecvP < len)
      {
      LOGG_PRINT (LOG_DEBUG_E,NULL,"USB_CTRL_COMP_recvUnBuffered: len = %d, *byteLenRecvP = %d\n",len, readLen, *byteLenRecvP);
#ifdef INU_PROFILER_USB_HOST     
      INUG_profiler_get_timestamp();
#endif     
         err = inu_usb_read_bulk(pInf->param.usbInterfaceIdx, pBuff + *byteLenRecvP, len-*byteLenRecvP, &readLen);
#ifdef INU_PROFILER_USB_HOST
      INUG_profiler_get_timestamp();
#endif     
   //gettimeofday(&time_end, NULL);
         if (err == INU_USB_ERR_SUCCESS)
         {
            *byteLenRecvP+= readLen;
            LOGG_PRINT(LOG_DEBUG_E,NULL,"len = %d readLen =%d *byteLenRecvP = %d usbRetVal = %d\n",len, readLen, *byteLenRecvP, err);
            readLen=0;
         }
         else if (err == INU_USB_ERR_DISCONNECTED)
         {
            //printf ("usb is disconnected ... \n");
#ifdef INU_PROFILER_USB_HOST
            INUG_profiler_get_timestamp();
#endif         
            return USB_CTRL_COM__ERR_LOST_CONNECTION;
         }
         else
         {            
            LOGG_PRINT(LOG_ERROR_E, NULL, "inu_usb_lib: readpipe failed (rc=%d)\n",err);
#ifdef INU_PROFILER_USB_HOST
            INUG_profiler_get_timestamp();
#endif          
            return USB_CTRL_COM__ERR_READ_FAILED;
         }
      }
   }
#ifdef INU_PROFILER_USB_HOST   
   INUG_profiler_get_timestamp();
#endif   
   return retVal;
}

void USB_CTRL_COMP_getCaps(void *connHandle, CLS_COMG_capsT *capsP)
{
   USB_CTRL_COMP_interfaceInfT *pInf = (USB_CTRL_COMP_interfaceInfT *)connHandle;
   capsP->flags  =  0;
   capsP->fixedSize = 0;  
   if (pInf->param.rxCap)
   {
      capsP->flags |= CLS_COMG_CAP_RX;
   }
   if (pInf->param.txCap)
   {
      capsP->flags |= CLS_COMG_CAP_TX;
   }
   if (!pInf->param.fixedSizeCap &&  !pInf->param.unBufferedReadCap)
   {
      capsP->flags |= CLS_COMG_CAP_MODE_STREAM;
   }
    else if(pInf->param.unBufferedReadCap && !pInf->param.fixedSizeCap )
   {
      capsP->flags  |= CLS_COMG_CAP_MODE_UNBUFFERED_RECV;
      capsP->unbufferedFixedSize = pInf->param.unBufferedReadSize ;
   }
   else
   {
      capsP->fixedSize = pInf->param.fixedBuffSize; 
   }
   capsP->flags |= CLS_COMG_CAP_SENDV;
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: USB_CTRL_COMG_getOps
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
void USB_CTRL_COMG_getOps(CLS_COMG_ifOperations *ifOpsP)
{
   ifOpsP->open     = &USB_CTRL_COMP_open;
   ifOpsP->close    = &USB_CTRL_COMP_close;
   ifOpsP->getCaps  = &USB_CTRL_COMP_getCaps;
   ifOpsP->send     = &USB_CTRL_COMP_send;
   ifOpsP->sendv    = &USB_CTRL_COMP_sendv;
   ifOpsP->recv     = &USB_CTRL_COMP_recv;
   ifOpsP->peek     = NULL;
   ifOpsP->recv_unbuffered = &USB_CTRL_COMP_recvUnBuffered;
   ifOpsP->sendv_unbuffered = NULL;
}


#ifdef __cplusplus
}
#endif
#endif

