/****************************************************************************
 * 
 *   FileName: tcp_com.c
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: TCP com interface 
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"
#include "mem_pool.h"
#include "tcp_com.h"

#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef struct
{
   int                        isServer;
   PSOCKET                    ipSocket;
   PSOCKET                    ipSocketSvrCom;
   PADDRESS                   ipAddrPeer;
   PADDRESS                   ipAddrMe;
   OS_LYRG_threadParams       serverThrParams;
   volatile int               serverCloseFlag;
   UINT8                      txBuffer[TCP_COMG_FRAGMENT_SIZE];
} TCP_COMP_infoT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static TCP_COMP_infoT comInfo;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/******************************************************************************
 ***************     P R E    D E F I N I T I O N     OF        ***************
 ***************     L O C A L         F U N C T I O N S        ***************
 ******************************************************************************/
static INT32      TCP_COMP_serverThread(void *argP);
ERRG_codeE TCP_COMP_init(TCP_COMG_interfaceParamsT *openCfgP, TCP_COMP_infoT *comInfoP);

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: TCP_COMP_read
*
*  Description:
*  Read len bytes into buffer. Returns length read on success (or if connection
*  was closed during read), and -1 on any error during read.
*  This functions attempts to read requested length retrying read if needed.
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: TCP connection
*
****************************************************************************/
static INT32 TCP_COMP_read(PSOCKET sock, UINT8 *buffP, INT32 len, INT32 *isEofP)
{
   INT32 byteCnt = 0;
   INT32 left = len;
   INT32 tmpLen;
   ERRG_codeE ret;

   while(left > 0)
   {
      //Try to read as much as is left each call 
      tmpLen = left;
      ret = SOCKETG_recv(sock, (char *)&buffP[byteCnt], &tmpLen, 0);
      if(ERRG_FAILED(ret))
      {
         //Error on read
         LOGG_PRINT(LOG_ERROR_E, NULL, "Error on read sock=%d ret=0x%X\n", sock, ret);
         byteCnt = -1;
         break;
      }
      else
      {
         if(tmpLen == 0)
         {
            //Connection closed
            LOGG_PRINT(LOG_DEBUG_E, NULL, "read 0 (closed) on sock=%d\n", sock);
            *isEofP = TRUE;
            break;
         }
         //On return tmpLen holds the number of bytes read
         byteCnt += tmpLen;
         left    -= tmpLen;
      }
   }

   return byteCnt;
}

/****************************************************************************
*
*  Function Name: TCP_COMP_socketPeek
*
*  Description:
*  This function is similar to TCP_COMP_read, except that the data
*  is copied but not removed from the socket. 
*  Essentially we peek into the socket, several times if needed, until at 
*  least len bytes exist in the socket.
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: TCP connection
*
****************************************************************************/
static ERRG_codeE TCP_COMP_socketPeek(PSOCKET sock, UINT8 *buffP, UINT32 len, INT32 *isEofP, UINT32 *byteCntP)
{
   ERRG_codeE  ret = TCP_COM__RET_SUCCESS;
   INT32       tmpLen;

   *byteCntP = 0;

   while(*byteCntP < len)
   {
      //Peek does not remove data from socket - so we need to keep
      //trying to peek the entire length into the start of the buffer
      tmpLen = len;
      ret = SOCKETG_recv(sock, (char *)&buffP[0], &tmpLen, MSG_PEEK);
      if(ERRG_FAILED(ret))
      {         
         //Error on peek
         LOGG_PRINT(LOG_DEBUG_E, ret, "Error on read sock=%d\n", sock);
         *byteCntP = 0;
         break;
      }
      else
      {
         if(tmpLen == 0)
         {
            //Connection closed
            LOGG_PRINT(LOG_DEBUG_E, NULL, "read 0 (closed) on sock=%d\n", sock);
            *isEofP = TRUE;
            break;
         }
         //On return tmpLen holds the number of bytes peeked
         *byteCntP = tmpLen;
      }
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: TCP_COMP_serverThread
*
*  Description:
*  Simple server to accept incoming connection requests
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: TCP connection
*
****************************************************************************/
static INT32 TCP_COMP_serverThread(void *argP)
{
   enum {BIND_RETRY_MSECS = 5000};
   TCP_COMP_infoT *comInfoP = (TCP_COMP_infoT *)argP;
   ERRG_codeE ret = TCP_COM__RET_SUCCESS;
   //socklen_t len;

   LOGG_PRINT(LOG_DEBUG_E, NULL, "up\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "Binding local addr on sock=%d\n", comInfoP->ipSocket);

   //First bind address - retry as needed
   while(!comInfoP->serverCloseFlag)
   {
      ret = SOCKETG_bind(comInfoP->ipSocket, &comInfoP->ipAddrMe);
      if(ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Binding failed - waiting to retry...%d msecs\n", BIND_RETRY_MSECS);
         OS_LYRG_usleep(BIND_RETRY_MSECS * 1000);
         continue; //retry after sleep to allow for OS to timeout
      }
      else
      {
         break; //bound
      }
   }

   if(ERRG_SUCCEEDED(ret))
   {
      //Bind successful - now accept connection fro client
      while(!comInfoP->serverCloseFlag)
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "listening on sock=%d...\n", comInfoP->ipSocket);

         //Wait for and accept incoming connection
         ret = SOCKETG_listen(comInfoP->ipSocket);

         //ret = SOCKETG_select(comInfoP->ipSocket, &len);//TODO:check for multiple connection
         if(ERRG_FAILED(ret))
         {
            continue; //retry if server not closed
         }
         LOGG_PRINT(LOG_INFO_E, NULL, "listen done. accepting...\n");

         ret = SOCKETG_accept(comInfoP->ipSocket, &comInfoP->ipSocketSvrCom);
         if(ERRG_FAILED(ret))
         {
            continue; //retry if server not closed
         }

         LOGG_PRINT(LOG_INFO_E, NULL, "accepted on sock=%d, com on sock=%d\n", comInfoP->ipSocket, comInfoP->ipSocketSvrCom);
      }
   }
   LOGG_PRINT(LOG_INFO_E, NULL, "exiting ret=0x%X\n", ret);

   return ret;
}

/****************************************************************************
*
*  Function Name: TCP_COMP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: TCP connection
*
****************************************************************************/
ERRG_codeE TCP_COMP_open(void **connHandle, void *openCfgP)
{
   ERRG_codeE                 retVal   = TCP_COM__RET_SUCCESS;
   TCP_COMP_infoT *comInfoP = &comInfo;
   LOGG_PRINT(LOG_INFO_E, NULL, "Open TCP params: server %d port %d ip address peer %s\n",((TCP_COMG_interfaceParamsT *)openCfgP)->isServer,((TCP_COMG_interfaceParamsT *)openCfgP)->port,((TCP_COMG_interfaceParamsT *)openCfgP)->ipAddrPeerP);
   
   retVal = TCP_COMP_init((TCP_COMG_interfaceParamsT *)openCfgP, comInfoP);

   if(ERRG_SUCCEEDED(retVal))
   {
      retVal = SOCKETG_create(SOCKETG_TYPE_TCP_E, &comInfoP->ipSocket);
   }
   if(ERRG_FAILED(retVal))
   {
      return retVal;
   }

   comInfoP->isServer = ((TCP_COMG_interfaceParamsT *)openCfgP)->isServer;

   if(((TCP_COMG_interfaceParamsT *)openCfgP)->isServer == TRUE)
   {
      //Launch server thread to accept incoming connections
      comInfoP->serverCloseFlag              = FALSE;
      comInfoP->serverThrParams.func         = TCP_COMP_serverThread;
      comInfoP->serverThrParams.param        = (void *)comInfoP;
      comInfoP->serverThrParams.id           = OS_LYRG_CLS_TCP_SERVER_THREAD_ID_E;
      comInfoP->serverThrParams.threadHandle  = OS_LYRG_createThread(&comInfoP->serverThrParams);
      if(!(comInfoP->serverThrParams.threadHandle))
         retVal = TCP_COM__ERR_GENERAL_ERROR;
      LOGG_PRINT(LOG_INFO_E, NULL, "Created TCP server thread\n");
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Connecting to server sock=%d\n", comInfoP->ipSocket);
      //Connect with peer as client
      retVal = SOCKETG_connect(comInfoP->ipSocket, &comInfoP->ipAddrPeer);
   }

   if(ERRG_SUCCEEDED(retVal))
   {
      *connHandle = comInfoP;
      LOGG_PRINT(LOG_INFO_E, NULL, "open TCP Success\n");
   }

   return retVal;
}

/****************************************************************************
*
*  Function Name: TCP_COMP_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: TCP connection
*
****************************************************************************/
ERRG_codeE TCP_COMP_init(TCP_COMG_interfaceParamsT *openCfgP, TCP_COMP_infoT *comInfoP)
{
   ERRG_codeE                 retVal      = TCP_COM__RET_SUCCESS;

   retVal = SOCKETG_initialize();

   if(ERRG_FAILED(retVal))
   {
      retVal = TCP_COM__ERR_SOCKET_INIT_FAIL;
   }
   else
   {
      if(openCfgP->isServer == TRUE)
      {
         retVal = SOCKETG_addressCreate(NULL, openCfgP->port, &comInfoP->ipAddrMe);
         if(ERRG_FAILED(retVal))
         {
            retVal = TCP_COM__ERR_SOCKET_ADDR_CREATE_FAIL;
         }
         else
         {
            LOGG_PRINT(LOG_INFO_E, NULL, "TCP_COMP_init server success\n");
         }
      }
      else
      {
         retVal = SOCKETG_addressCreate((const char *)openCfgP->ipAddrPeerP, openCfgP->port, &comInfoP->ipAddrPeer);
         if(ERRG_FAILED(retVal))
         {
            retVal = TCP_COM__ERR_SOCKET_ADDR_CREATE_FAIL;
         }
         else
         {
            LOGG_PRINT(LOG_INFO_E, NULL, "TCP_COMP_init client success\n");
         }
      }
   }

   return retVal;
}

/****************************************************************************
*
*  Function Name: TCP_COMP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: TCP connection
*
****************************************************************************/
ERRG_codeE TCP_COMP_close(void *connHandle)
{
   TCP_COMP_infoT *comInfoP = (TCP_COMP_infoT *)connHandle;

   if(comInfoP->isServer == TRUE)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Waiting for TCP server thread close\n");

      //Close communication socket so client does an orderly shutdown and to unblock local recv calls.
      SOCKETG_close(comInfoP->ipSocketSvrCom);

      //Close socket to unblock accept in server thread
      SOCKETG_close(comInfoP->ipSocket);

      comInfoP->serverCloseFlag = TRUE;

      OS_LYRG_waitForThread(comInfoP->serverThrParams.threadHandle, OS_LYRG_INFINITE);

      LOGG_PRINT(LOG_INFO_E, NULL, "Closing server sockets\n");
   }
   else
   {
      SOCKETG_close(comInfoP->ipSocket);
   }

   SOCKETG_cleanup();

   return TCP_COM__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: TCP_COMP_send
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: TCP connection
*
****************************************************************************/
ERRG_codeE TCP_COMP_send(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenSendP)
{
   ERRG_codeE retVal = TCP_COM__RET_SUCCESS;
   PSOCKET sock;
   TCP_COMP_infoT *comInfoP = (TCP_COMP_infoT *)connHandle;
   INT32 left;
   UINT32 toSend;

   //Wait for socket to come up in case it isn't yet.
   sock = (comInfoP->isServer == TRUE) ? comInfoP->ipSocketSvrCom : comInfoP->ipSocket;
   while(sock == 0)
   {
      OS_LYRG_usleep(1000000);
      sock = (comInfoP->isServer == TRUE) ? comInfoP->ipSocketSvrCom : comInfoP->ipSocket;
   }

   //Send in chunks - this is to improve TCP perfomance
   left = len;
   while(left > 0)
   {
      toSend = (UINT32)MIN((INT32)sizeof(comInfoP->txBuffer), left);
      
      //Copying to buffer before send due to performance issue in sending on socket from external buffer
      memcpy(comInfoP->txBuffer, &pBuff[len-left], toSend);
      retVal = SOCKETG_send(sock, (char *)comInfoP->txBuffer, (INT32 *)&toSend, 0);
      if(ERRG_FAILED(retVal))
      {
         break;
      }
      else
      {
         left -= toSend;
      }
   }
   *byteLenSendP = len-left;
   
   return retVal;
}



ERRG_codeE TCP_COMP_sendv(void *connHandle,  void *bufP, UINT32 *bytesCntP)
{
   UINT32 byteCnt = 0;
   UINT32 byteCntTotal = 0;
   ERRG_codeE ret = TCP_COM__ERR_INVALID_ARGS;
   MEM_POOLG_bufDescT *currP = (MEM_POOLG_bufDescT *)bufP;
 
   while(currP)
   {
      byteCnt = 0;
      
      ret = TCP_COMP_send(connHandle, currP->dataP, currP->dataLen, &byteCnt);
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
*  Function Name: TCP_COMP_recv
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: TCP connection
*
****************************************************************************/
ERRG_codeE TCP_COMP_recv(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenRecvP)
{
   INT32 ret;
   INT32 isEof = FALSE;
   ERRG_codeE retVal= TCP_COM__RET_SUCCESS;
   PSOCKET sock;
   TCP_COMP_infoT *comInfoP = (TCP_COMP_infoT *)connHandle;

   sock = (comInfoP->isServer == TRUE) ? comInfoP->ipSocketSvrCom : comInfoP->ipSocket;

   while(sock == 0)
   {
      OS_LYRG_usleep(1000000);
      sock = (comInfoP->isServer == TRUE) ? comInfoP->ipSocketSvrCom : comInfoP->ipSocket;
   }

   //Read all the requested data
   ret = TCP_COMP_read(sock, pBuff, len, &isEof);
   if(ret < 0)
   {
      *byteLenRecvP = 0;
      retVal = TCP_COM__ERR_GENERAL_ERROR;
   }
   else
   {
      if(isEof)
      {
         //Shutdown and close communication socket so peer also closes
         SOCKETG_close(sock);
      }
      *byteLenRecvP = ret;
   }

   return retVal;
}

/****************************************************************************
*
*  Function Name: TCP_COMP_peek
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: TCP connection
*
****************************************************************************/
ERRG_codeE TCP_COMP_peek(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenRecvP)
{
   ERRG_codeE retVal= TCP_COM__RET_SUCCESS;
   PSOCKET sock;
   INT32 isEof = FALSE;
   TCP_COMP_infoT *comInfoP = (TCP_COMP_infoT *)connHandle;

   sock = (comInfoP->isServer == TRUE) ? comInfoP->ipSocketSvrCom : comInfoP->ipSocket;

   while(sock == 0)
   {
      OS_LYRG_usleep(1000000);
      sock = (comInfoP->isServer == TRUE) ? comInfoP->ipSocketSvrCom : comInfoP->ipSocket;
   }

   //Read all the requested data with peek
   retVal = TCP_COMP_socketPeek(sock, pBuff, len, &isEof, byteLenRecvP);

   if(ERRG_SUCCEEDED(retVal))
   {
      if(isEof)
      {
         comInfoP->ipSocketSvrCom = 0;
         //Shutdown and close communication socket so peer also closes 
         LOGG_PRINT(LOG_INFO_E, NULL, "Shutdown detected in recv - closing\n");
         SOCKETG_close(sock);
      }
   }

   return retVal;
}


void TCP_COMP_getCaps(void *connHandle, CLS_COMG_capsT *capsP)
{
   (void)connHandle;

   capsP->flags = 0;
   capsP->flags |= CLS_COMG_CAP_RX;
   capsP->flags |= CLS_COMG_CAP_TX;
   capsP->flags |= CLS_COMG_CAP_SENDV;
   capsP->flags |= CLS_COMG_CAP_PEEK;
   capsP->flags |= CLS_COMG_CAP_MODE_STREAM;
   capsP->fixedSize = 0;
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: TCP_COMG_getOps
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: TCP connection
*
****************************************************************************/
void TCP_COMG_getOps(CLS_COMG_ifOperations *ifOpsP)
{
   ifOpsP->open    = &TCP_COMP_open;
   ifOpsP->config  = NULL;
   ifOpsP->close   = &TCP_COMP_close;
   ifOpsP->getCaps = &TCP_COMP_getCaps;
   ifOpsP->send    = &TCP_COMP_send;
   ifOpsP->sendv   = &TCP_COMP_sendv;
   ifOpsP->recv    = &TCP_COMP_recv;
   ifOpsP->peek    = &TCP_COMP_peek;
   ifOpsP->recv_unbuffered = NULL;
   ifOpsP->sendv_unbuffered = NULL;
}

#ifdef __cplusplus
}
#endif
