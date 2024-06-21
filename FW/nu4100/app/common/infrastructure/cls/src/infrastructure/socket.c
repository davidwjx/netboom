/****************************************************************************
 * 
 *   FileName: socket..c
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: socket infrastructure
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"
#include "sys_defs.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#if DEFSG_IS_OS_LINUX
   #include <unistd.h>
   #include <netdb.h>
   #include <netinet/tcp.h>
#endif

#include "socket.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
   #include <winsock2.h>
#endif

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF        *************
 ***************     L O C A L         F U N C T I O N S        *************
 ****************************************************************************/
ERRG_codeE SOCKETP_createX(SOCKETG_socketTypeE socketType, PSOCKET *sockP);
ERRG_codeE SOCKETP_initializeX();
ERRG_codeE SOCKETP_cleanupX();
ERRG_codeE SOCKETP_bindX(PSOCKET sock, PADDRESS *addr);
ERRG_codeE SOCKETP_addressCreateX(const char *ip4, INT32 port, PADDRESS *addrP);
ERRG_codeE SOCKETP_acceptX(PSOCKET sock, PSOCKET *comSockP);
ERRG_codeE SOCKETP_listenX(PSOCKET sock);
ERRG_codeE SOCKETP_connectX(PSOCKET sock, PADDRESS *to);
ERRG_codeE SOCKETP_closeX(PSOCKET sock);
ERRG_codeE SOCKETP_recvFromX(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags,PADDRESS *from);
ERRG_codeE SOCKETP_sendToX(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags,PADDRESS* to);
ERRG_codeE SOCKETP_sendX(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags);
ERRG_codeE SOCKETP_sendX(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags);

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

#ifdef WIN32

/****************************************************************************
*
*  Function Name: SOCKETP_createX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_createX(SOCKETG_socketTypeE socketType, PSOCKET *sockP)
{
   ERRG_codeE     ret = SOCKET_MOD__RET_SUCCESS;
   PSOCKET        sock;
   INT32          sockWinBufRxSize  = 2 * SYS_DEFSG_MAX_FRAME_SIZE;
   INT32          sockWinBufTxSize  = 2 * SYS_DEFSG_MAX_FRAME_SIZE;
   INT32          size              = sizeof(INT32);

   if(socketType == SOCKETG_TYPE_UDP_E)
   {
      sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   }
   else
   {
      sock = socket(AF_INET, SOCK_STREAM, 0);
      //fcntl(sock, F_SETFL, O_NONBLOCK);
      setsockopt(sock, SOL_SOCKET,  SO_SNDBUF,   (char *) &sockWinBufTxSize, size);
      setsockopt(sock, SOL_SOCKET,  SO_RCVBUF,   (char *) &sockWinBufRxSize, size);
      getsockopt(sock, SOL_SOCKET,  SO_SNDBUF,   (char *) &sockWinBufTxSize, &size);
      getsockopt(sock, SOL_SOCKET,  SO_RCVBUF,   (char *) &sockWinBufRxSize, &size);
      LOGG_PRINT(LOG_INFO_E, NULL, "Socket created. Rx window size=%d bytes, Tx window size=%d bytes\n", sockWinBufRxSize, sockWinBufTxSize);
   }

   if(sock == INVALID_SOCKET)
   {
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Socket create: %d\n", sock);
      *sockP = sock;
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_initializeX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_initializeX()
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;
   WSADATA wsaData;

   retval = WSAStartup(MAKEWORD(2,2), &wsaData);
   if(retval != 0)
   {
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_cleanupX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_cleanupX()
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;

   retval = WSACleanup();
   if(retval != 0)
   {
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_bindX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_bindX(PSOCKET sock, PADDRESS *addr)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;

   retval = bind(sock, (SOCKADDR*) addr, sizeof(*addr));
   if(retval == SOCKET_ERROR)
   {
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
      LOGG_PRINT(LOG_ERROR_E, NULL, "bind failed\n");
   }
   else
   {
      char buffer[INET_ADDRSTRLEN];
      inet_ntop( AF_INET, &addr->sin_addr, buffer, sizeof( buffer ));
      LOGG_PRINT(LOG_INFO_E, NULL, "bind success sock = %d addr = %s\n",sock,buffer);
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_addressCreateX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_addressCreateX(const char *ip4, INT32 port, PADDRESS *addrP)
{
   ERRG_codeE ret = SOCKET_MOD__RET_SUCCESS;
   struct hostent *h = NULL;
   
   if(ip4 == NULL)
   {
      memset((void *)addrP, 0, sizeof(PADDRESS));
      addrP->sin_family = AF_INET;
      addrP->sin_port = htons(port);
      addrP->sin_addr.s_addr = htonl(INADDR_ANY);

      LOGG_PRINT(LOG_INFO_E, NULL, "socket initialized for any addr port=%d\n", port);
   }
   else
   {
      //TODO: use of gethostbyname functions is deprecated.
      h = gethostbyname(ip4);  // gethostbyname make compilation warning. remove comment when you need to use TCP
      if(h != NULL)
      {
         memset((void *)addrP, 0, sizeof(PADDRESS));
         addrP->sin_family = AF_INET;
         memcpy((void *)&(addrP->sin_addr), h->h_addr_list[0], h->h_length);
         addrP->sin_port = htons(port);
         LOGG_PRINT(LOG_INFO_E, NULL, "socket initialized for '%s' (IP : %s) port=%d\n", h->h_name, inet_ntoa(*(struct in_addr *)h->h_addr_list[0]),port);
      }
      else
      {
         ret = SOCKET_MOD__ERR_GENERAL_ERR;
         LOGG_PRINT(LOG_ERROR_E, NULL, "CLS TCP err: unknown host '%s' \n", ip4);
      }
   }
   return ret;

}

/****************************************************************************
*
*  Function Name: SOCKETP_acceptX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_acceptX(PSOCKET sock, PSOCKET *comSockP)
{
   PSOCKET comSock;
   ERRG_codeE ret = SOCKET_MOD__RET_SUCCESS;

   comSock = accept(sock, NULL, NULL);
   if(comSock == INVALID_SOCKET)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Error %d\n", WSAGetLastError()); 
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }
   else
   {
      *comSockP = comSock;
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_listenX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_listenX(PSOCKET sock)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;

   retval = listen(sock, 5);
   if(retval == SOCKET_ERROR)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Error %d\n", WSAGetLastError()); 
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_selectX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_selectX(PSOCKET sock, socklen_t *len)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   fd_set rset;
   struct timeval waitTime;
   
   // clear the descriptor set 
   FD_ZERO(&rset); 

   // set listenfd in readset 
   FD_SET(sock, &rset); 
            
   waitTime.tv_sec = 2;
   waitTime.tv_usec = 0;
   // select the ready descriptor 
   select((int)sock + 1, &rset, NULL, NULL, NULL/*waitTime*/);
   // if tcp socket is readable then handle 
   // it by accepting the connection 
   if (FD_ISSET(sock, &rset)) 
   { 
      *len = sizeof(sockaddr_in);
       LOGG_PRINT(LOG_INFO_E, NULL, "select success\n");
	   return SOCKET_MOD__RET_SUCCESS;
    } 
         
   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_connectX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_connectX(PSOCKET sock, PADDRESS *to)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;

   retval = connect(sock, (SOCKADDR*) to, sizeof(PADDRESS));
   if(retval == SOCKET_ERROR)
   {      
      LOGG_PRINT(LOG_ERROR_E, NULL, "Error %d\n", WSAGetLastError());
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_closeX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_closeX(PSOCKET sock)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;

   retval = shutdown(sock, SD_BOTH);
   if(retval == SOCKET_ERROR)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "shutdown error %d sock=%d\n", WSAGetLastError(),sock);
   }

   retval = closesocket(sock);
   if(retval == SOCKET_ERROR)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "close error %d sock=%d\n", WSAGetLastError(),sock);
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_recvX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_recvX(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;

   retval = recv(sock, buf, *lenP, flags);
   if(retval == SOCKET_ERROR)
   {
      retval = WSAGetLastError();
      LOGG_PRINT(LOG_ERROR_E, NULL, "Error in recv ERROR=%d sock =%d\n", retval, sock);
      switch(retval)
      {
         case WSAENETDOWN:
         case WSAENETUNREACH:
         case WSAENETRESET:
         case WSAECONNABORTED:
         case WSAECONNRESET:
         case WSAENOTCONN:
         case WSAESHUTDOWN:
            ret = SOCKET_MOD__ERR_LOST_SOCKET_CONNECTION;
            break;
         default:
            ret = SOCKET_MOD__ERR_GENERAL_ERR;
            break;
      }
   }
   else
   {
      *lenP = retval;
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_recvFromX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_recvFromX(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags, PADDRESS *from)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;
   INT32       alen = sizeof(*from);

   retval = recvfrom(sock, buf, *lenP, flags, (SOCKADDR*)from, &alen);
   if(retval == SOCKET_ERROR)
   {
      retval = WSAGetLastError();
      if(! ((retval == WSAEMSGSIZE) &&  (flags == MSG_PEEK)) )
      {
         //ignoring in case of PEEK and return length
         LOGG_PRINT(LOG_DEBUG_E, NULL, "Error in recvfrom %d\n", retval);
         ret = SOCKET_MOD__ERR_GENERAL_ERR;
      }
   }
   else
   {
      *lenP = retval;
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_sendToX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_sendToX(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags,PADDRESS* to)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;

   retval = sendto(sock, buf, *lenP, flags, (SOCKADDR*)to, sizeof(*to));
   if(retval == SOCKET_ERROR)
   {
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }
   else
   {
      *lenP = retval;
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_sendX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_sendX(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;

   retval = send(sock, buf, *lenP, flags);
   if(retval == SOCKET_ERROR)
   {
      retval = WSAGetLastError();
      LOGG_PRINT(LOG_ERROR_E, NULL, "Error in send ERROR=%d sock =%d\n", retval, sock);
      switch(retval)
      {
         case WSAENETDOWN:
         case WSAENETUNREACH:
         case WSAENETRESET:
         case WSAECONNABORTED:
         case WSAECONNRESET:
         case WSAENOTCONN:
         case WSAESHUTDOWN:
            ret = SOCKET_MOD__ERR_LOST_SOCKET_CONNECTION;
            break;
         default:
            ret = SOCKET_MOD__ERR_GENERAL_ERR;
            break;
      }
   }
   else
   {
      *lenP = retval;
   }

   return ret;
}

#endif /* windows */

/*******************************************__linux__**********************************/

#ifdef __linux__

#include <errno.h>
#include <fcntl.h>

//#define INVALID_SOCKET -1

/****************************************************************************
*
*  Function Name: SOCKETP_createX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_createX(SOCKETG_socketTypeE socketType, PSOCKET *sockP)
{
   ERRG_codeE     ret               = SOCKET_MOD__RET_SUCCESS;
   INT32          sockWinBufRxSize  = SYS_DEFSG_MAX_FRAME_SIZE;
   INT32          sockWinBufTxSize  = SYS_DEFSG_MAX_FRAME_SIZE;
   INT32          size              = sizeof(INT32);
   PSOCKET        sock;

   struct timeval timeout;

   timeout.tv_sec   = 1;  /* 30 Secs Timeout */
   timeout.tv_usec  = 0;  // Not init'ing this can cause strange errors

   if(socketType == SOCKETG_TYPE_UDP_E)
   {
      sock = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   }
   else
   {
      //sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP);
      sock = socket( AF_INET, SOCK_STREAM, 0);
      //fcntl(sock, F_SETFL, O_NONBLOCK);
      if (SOCKETG_TYPE_TCP_E)
      {
         setsockopt(sock, SOL_SOCKET,  SO_SNDBUF,   (char *) &sockWinBufRxSize, size);
         setsockopt(sock, SOL_SOCKET,  SO_RCVBUF,   (char *) &sockWinBufTxSize, size);
         getsockopt(sock, SOL_SOCKET,  SO_SNDBUF,   (char *) &sockWinBufRxSize, (socklen_t  *)&size);
         getsockopt(sock, SOL_SOCKET,  SO_RCVBUF,   (char *) &sockWinBufTxSize, (socklen_t  *)&size);
         LOGG_PRINT(LOG_INFO_E, NULL, "Socket created. Rx window size=%d bytes, Tx window size=%d bytes\n", sockWinBufRxSize, sockWinBufTxSize);
         setsockopt(sock, SOL_SOCKET,  SO_SNDTIMEO, (char *)&timeout, sizeof(timeout));
      }
      else LOGG_PRINT(LOG_INFO_E, NULL, "Socket created\n");
   }

   if(sock == -1)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "create socket %s\n", strerror(errno));
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }
   else
   {
      *sockP = sock;
   }

   LOGG_PRINT(LOG_INFO_E, NULL, "Socket create: %d\n", sock);
   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_initializeX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_initializeX()
{
    return (ERRG_codeE)RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: SOCKETP_cleanupX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_cleanupX()
{
   return (ERRG_codeE)SUCCESS_E;
}

/****************************************************************************
*
*  Function Name: SOCKETP_bindX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_bindX(PSOCKET sock, PADDRESS *addrP)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;

#if 0
   struct linger so_linger;
   so_linger.l_onoff = TRUE;
   so_linger.l_linger = 0;
   setsockopt(sock, SOL_SOCKET, SO_LINGER, &so_linger,sizeof so_linger);
   
   retval = bind(sock, (__CONST_SOCKADDR_ARG)addrP, sizeof(PADDRESS));
   if(retval == -1)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Bind failed, %s\n", strerror(errno));
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Bind success\n");
   }
#else
   retval = bind(sock, (struct sockaddr *)addrP, sizeof(PADDRESS));
   if(retval == -1)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Bind failed, %s\n", strerror(errno));
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Bind success\n");
   }
#endif

   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_addressCreateX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_addressCreateX(const char *ip4, INT32 port, PADDRESS *addrP)
{
   ERRG_codeE ret = SOCKET_MOD__RET_SUCCESS;
   struct hostent *h = NULL;

   if(ip4 == NULL)
   {
      memset((void *)addrP, 0, sizeof(PADDRESS));
      addrP->sin_family = AF_INET;
      addrP->sin_port = htons(port);
      addrP->sin_addr.s_addr = htonl(INADDR_ANY);
      
      LOGG_PRINT(LOG_INFO_E, NULL, "socket initialized for any addr port=%d\n", port);
   }
   else
   {
      //TODO: use of gethostbyname functions is deprecated.
      h = gethostbyname(ip4);  // gethostbyname make compilation warning. remove comment when you need to use TCP
      if(h != NULL)
      {
         memset((void *)addrP, 0, sizeof(PADDRESS));
         addrP->sin_family = AF_INET;
         memcpy((void *)&(addrP->sin_addr), h->h_addr, h->h_length);
         addrP->sin_port = htons(port);
         LOGG_PRINT(LOG_INFO_E, NULL, "socket initialized for '%s' (IP : %s) port=%d\n", h->h_name, inet_ntoa(*(struct in_addr *)h->h_addr_list[0]),port);
      }
      else
      {
         ret = SOCKET_MOD__ERR_GENERAL_ERR;
         LOGG_PRINT(LOG_ERROR_E, NULL, "CLS UDP err: unknown host '%s' \n", ip4);
      }
   }
   return ret;

}

/****************************************************************************
*
*  Function Name: SOCKETP_acceptX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_acceptX(PSOCKET sock, PSOCKET *comSockP)
{
   PSOCKET comSock;
   ERRG_codeE ret = SOCKET_MOD__RET_SUCCESS;

   comSock = accept(sock, NULL, NULL);
   if(comSock == -1)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "accept failed %s\n", strerror(errno));
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "accept success con sock = %d\n",comSock);
      *comSockP = comSock;
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_listenX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_listenX(PSOCKET sock)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;

   retval = listen(sock, 5);
   if(retval == -1)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "listen failed. error description: %s\n", strerror(errno));
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_selectX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_selectX(PSOCKET sock, socklen_t *len)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   fd_set rset;
   struct sockaddr_in cliaddr;
   struct timeval waitTime;
   
   // clear the descriptor set 
   FD_ZERO(&rset); 

   // set listenfd in readset 
   FD_SET(sock, &rset); 
            
   waitTime.tv_sec = 2;
   waitTime.tv_usec = 0;
   // select the ready descriptor 
   select(sock + 1, &rset, NULL, NULL, &waitTime);
   // if tcp socket is readable then handle 
   // it by accepting the connection 
   if (FD_ISSET(sock, &rset)) 
   { 
      *len = sizeof(cliaddr); 
       LOGG_PRINT(LOG_INFO_E, NULL, "select success\n");
       return SOCKET_MOD__RET_SUCCESS;
    } 
         
   return ret;
}


/****************************************************************************
*
*  Function Name: SOCKETP_connectX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_connectX(PSOCKET sock, PADDRESS *to)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
#if 0
   INT32 retval;

   retval = connect(sock, (__CONST_SOCKADDR_ARG) to, sizeof(PADDRESS));
   if(retval == -1)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "connect failed %s\n", strerror(errno));
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }
#else
   INT32 retval;

   retval = connect(sock, (struct sockaddr *) to, sizeof(PADDRESS));
   if(retval == -1)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "connect failed %s\n", strerror(errno));
       ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }
   else
   {
      char buffer[INET_ADDRSTRLEN];
      inet_ntop( AF_INET, &to->sin_addr, buffer, sizeof( buffer ));
      LOGG_PRINT(LOG_INFO_E, NULL, "connect success to %s\n",buffer);
   }
#endif
   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_closeX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_closeX(PSOCKET sock)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;

   retval = shutdown(sock, SHUT_RDWR);
   if(retval == -1)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "shutdown failed %s\n", strerror(errno));
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }
   else
   {
      retval = close(sock);
      if(retval == -1)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "connect failed %s\n", strerror(errno));
         ret = SOCKET_MOD__ERR_GENERAL_ERR;
      }
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_recvFromX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_recvFromX(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags,PADDRESS *from)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;
   UINT32      alen = sizeof(*from);

   retval = recvfrom(sock, buf, *lenP, flags, (struct sockaddr *)from, &alen);
   if(retval == -1)
   {
      if((errno == EAGAIN) || (errno == EWOULDBLOCK))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Receive from socket: EAGAIN\n");
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Receive socket:ret=%d %d len=%d\n", retval, sock, *lenP);
      }
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Receive socket:ret=%d %d len=%d rx=%d\n", retval, sock, *lenP, retval);

      *lenP = retval;
      //UTILSG_dump_hex(buf,retval);
   }

   return ret;

}

/****************************************************************************
*
*  Function Name: SOCKETP_recvX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_recvX(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;

   retval = recv(sock, buf, *lenP, flags);
   if(retval == -1)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "socket receive error. error description: %s\n", strerror(errno));
      ret = SOCKET_MOD__ERR_GENERAL_ERR;
   }
   else
   {
      *lenP = retval;
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_sendToX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_sendToX(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags,PADDRESS *toP)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
#if 0
   INT32 retval;

   retval = sendto(sock, buf, *lenP, flags, (__CONST_SOCKADDR_ARG)toP, sizeof(PADDRESS));
   if(retval == -1)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Send sock=%d len=%d error :%s\n", sock, *lenP, strerror(errno));
   }
   else
   {
      *lenP = retval;
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Send on socket %d len=%d\n", sock, retval);
   }
#else
      FIX_UNUSED_PARAM_WARN(sock);
      FIX_UNUSED_PARAM_WARN(buf);
      FIX_UNUSED_PARAM_WARN(lenP);
      FIX_UNUSED_PARAM_WARN(flags);
      FIX_UNUSED_PARAM_WARN(toP);
#endif
   return ret;
}

/****************************************************************************
*
*  Function Name: SOCKETP_sendX
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETP_sendX(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags)
{
   ERRG_codeE  ret = SOCKET_MOD__RET_SUCCESS;
   INT32       retval;

   //On linux - if a socket is closed by receiver (as for TCP) the send call will cause a SIGPIPE signal
   //to terminate the application. To avoid this we use the MSG_NOSIGNAL flag here. 
   flags |= MSG_NOSIGNAL;
   retval = send(sock, buf, *lenP, flags);
   if(retval == -1)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "socket send error. error description: %s\n", strerror(errno));
      ret = SOCKET_MOD__ERR_SEND_FAIL_ERR;
   }
   else
   {
      *lenP = retval;
   }

   return ret;
}

#endif /* linux */

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: SOCKETG_initialize
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETG_initialize()
{
   return SOCKETP_initializeX();
}

/****************************************************************************
*
*  Function Name: SOCKETG_cleanup
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETG_cleanup()
{
   return SOCKETP_cleanupX();
}

/****************************************************************************
*
*  Function Name: SOCKETG_create
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETG_create(SOCKETG_socketTypeE socketType, PSOCKET *sockP)
{
   return SOCKETP_createX(socketType, sockP);
}

ERRG_codeE  SOCKETG_setCastType(PSOCKET sock, SOCKETG_castTypeE castType, const char* addr)
{
    int ret = 0;
    struct in_addr opt;
    ret = inet_pton(AF_INET, addr, &opt.s_addr);
    if (ret <= 0) 
    {
        if (ret == 0)
            fprintf(stderr, "Not in presentation format");
        else
            perror("inet_pton");
        return SOCKET_MOD__ERR_GENERAL_ERR;
    }

    ret = setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (const char*)&opt, sizeof(opt));
    if (ret < 0)
        return SOCKET_MOD__ERR_GENERAL_ERR;

    return SOCKET_MOD__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: SOCKETG_bind
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETG_bind(PSOCKET sock, PADDRESS *addrP)
{
   return SOCKETP_bindX(sock, addrP);
}

/****************************************************************************
*
*  Function Name: SOCKETG_listen
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETG_listen(PSOCKET sock)
{
   return SOCKETP_listenX(sock);
}

/****************************************************************************
*
*  Function Name: SOCKETG_select
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETG_select(PSOCKET sock, socklen_t *len)
{
   return SOCKETP_selectX(sock, len);
}

/****************************************************************************
*
*  Function Name: SOCKETG_connect
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETG_connect(PSOCKET sock,PADDRESS *toP)
{
   return SOCKETP_connectX(sock,toP);
}

/****************************************************************************
*
*  Function Name: SOCKETG_accept
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETG_accept(PSOCKET sock, PSOCKET *comSockP)
{
   return SOCKETP_acceptX(sock, comSockP);
}

/****************************************************************************
*
*  Function Name: SOCKETG_recvFrom
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETG_recvFrom(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags, PADDRESS *fromP)
{
   return SOCKETP_recvFromX(sock, buf, lenP, flags, fromP);
}

/****************************************************************************
*
*  Function Name: SOCKETG_recv
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETG_recv(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags)
{
   return SOCKETP_recvX(sock, buf, lenP, flags);
}

/****************************************************************************
*
*  Function Name: SOCKETG_sendTo
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETG_sendTo(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags, PADDRESS *toP)
{
   return SOCKETP_sendToX(sock, buf, lenP, flags, toP);
}

/****************************************************************************
*
*  Function Name: SOCKETG_send
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETG_send(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags)
{
   return SOCKETP_sendX(sock, buf, lenP, flags);
}

/****************************************************************************
*
*  Function Name: SOCKETG_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETG_close(PSOCKET sock)
{
   return SOCKETP_closeX(sock);
}

/****************************************************************************
*
*  Function Name: SOCKETG_addressCreate
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: socket infrastructure
*
****************************************************************************/
ERRG_codeE SOCKETG_addressCreate(const char *ip4, INT32 port, PADDRESS *addrP)
{
   return SOCKETP_addressCreateX(ip4, port, addrP);
}


#ifdef __cplusplus
}
#endif

