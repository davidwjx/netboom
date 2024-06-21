#include <unistd.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#include "inu_common.h"
#include "tcp_mngr.h"
#include "tcp_com.h"
static PSOCKET serverSocket=-1;
static PSOCKET clientSocket = -1;
struct sockaddr_in serverAddr;
struct sockaddr_in clientAddr;
unsigned char rcvBuff[512];


int TCP_MNGR_rcvBuff(unsigned int len)
{
   INT32 byteCnt = 0;
   INT32 left = len;
   INT32 tmpLen;
   ERRG_codeE ret;

   while(left > 0)
   {
      //Try to read as much as is left each call
      tmpLen = left;
      ret = SOCKETG_recv(clientSocket, (char *)&rcvBuff[byteCnt], &tmpLen, 0);
      if(ERRG_FAILED(ret))
      {
         //Error on read
         LOGG_PRINT(LOG_ERROR_E, NULL, "Error on read sock=%d ret=0x%X\n", clientSocket, ret);
         byteCnt = -1;
         break;
      }
      else
      {
         if(tmpLen == 0)
         {
            //Connection closed
            LOGG_PRINT(LOG_DEBUG_E, NULL, "read 0 (closed) on sock=%d\n", serverSocket);
            byteCnt = -2;
            break;
         }
         //On return tmpLen holds the number of bytes read
         byteCnt += tmpLen;
         left    -= tmpLen;
      }
   }

   return byteCnt;
}

static int TCP_MNGR_Thread(void *argP)
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
   unsigned int sizeRcved;
   char str[100]="hi this is test";
   unsigned int len=sizeof(clientAddr),len1;
   while (1)
   {
      if (ERRG_SUCCEEDED(ret))
      {
         ret = SOCKETG_listen(serverSocket);
         if (ERRG_SUCCEEDED(ret))
         {
            ret = SOCKETG_accept(serverSocket, &clientSocket);
            sizeRcved = TCP_MNGR_rcvBuff(4);
            sizeRcved = TCP_MNGR_rcvBuff(rcvBuff[0]);
            rcvBuff[sizeRcved] =0;
            len1 = 4;
            len = strlen(str);
            SOCKETG_send(clientSocket,(char *)&len,(int *)&len1,0);
            len = strlen(str);
            SOCKETG_send(clientSocket,str,(int *)&len,0);
         }
      }
   }
}

ERRG_codeE TCP_MNGRG_init(unsigned int port)
{

   OS_LYRG_threadParams    thrParams;
   int *tcpMngrThrId;
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
   unsigned int len=sizeof(clientAddr);

   ret = SOCKETG_addressCreate(NULL, port, &serverAddr);

   if (ERRG_SUCCEEDED(ret))
   {
      ret = SOCKETG_create(SOCKETG_TYPE_TCP_REG_E, &serverSocket);
   }
   if (ERRG_SUCCEEDED(ret))
   {
      ret = SOCKETG_bind(serverSocket, &serverAddr);
   }

   if (ERRG_SUCCEEDED(ret))
   {
      thrParams.func = TCP_MNGR_Thread;
      thrParams.id = OS_LYRG_CLS_TCP_SERVER_THREAD_ID_E;
      thrParams.event = NULL;
      thrParams.param = NULL;
      tcpMngrThrId = OS_LYRG_createThread(&thrParams);
   }
   return ret;

}


