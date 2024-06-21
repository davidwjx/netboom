/****************************************************************************
 *
 *   FileName: socket.h
 *
 *   Author: 
 *
 *   Date: 
 *
 *   Description: socket module
 *   
 ****************************************************************************/

#ifndef _SOCKET_H_
#define _SOCKET_H_

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#if DEFSG_IS_OS_LINUX
   #include <sys/socket.h>
   #include <netinet/in.h>
   #include <arpa/inet.h>
#else
   #include <winsock2.h>
#endif 

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

#if (DEFSG_OS == DEFSG_WINDOWS)
   typedef SOCKET PSOCKET;
#else
   typedef INT32 PSOCKET;
#endif

typedef enum
{
   SOCKETG_TYPE_UDP_E      = 0,
   SOCKETG_TYPE_TCP_E      = 1,
   SOCKETG_TYPE_TCP_REG_E  = 2,
   SOCKETG_NUM_OF_TYPES_E  = 3
} SOCKETG_socketTypeE;

typedef enum
{
	CAST_TYPE_UNIQUE_E = 0,
	CAST_TYPE_MULTIPLE_E = 1,
	CAST_TYPE_BROAD_REG_E = 2,
	CAST_NUM_OF_TYPES_E = 3
} SOCKETG_castTypeE;

typedef struct sockaddr_in PADDRESS;


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

ERRG_codeE  SOCKETG_initialize(void);
ERRG_codeE  SOCKETG_create(SOCKETG_socketTypeE socketType, PSOCKET *sockP);
ERRG_codeE  SOCKETG_setCastType(PSOCKET sock, SOCKETG_castTypeE castType, const char* addr);
ERRG_codeE  SOCKETG_listen(PSOCKET sock);
ERRG_codeE  SOCKETG_select(PSOCKET sock, socklen_t *len);
ERRG_codeE  SOCKETG_bind(PSOCKET sock, PADDRESS *addrP);
ERRG_codeE  SOCKETG_accept(PSOCKET sock, PSOCKET *comSock);
ERRG_codeE  SOCKETG_recv(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags);
ERRG_codeE  SOCKETG_send(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags);
ERRG_codeE  SOCKETG_recvFrom(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags, PADDRESS *fromP);
ERRG_codeE  SOCKETG_sendTo(PSOCKET sock, char *buf, INT32 *lenP, INT32 flags, PADDRESS *toP);
ERRG_codeE  SOCKETG_close(PSOCKET sock);
ERRG_codeE  SOCKETG_cleanup(void);
ERRG_codeE  SOCKETG_connect(PSOCKET sock, PADDRESS *toP);
ERRG_codeE  SOCKETG_addressCreate(const char *ip4, INT32 port, PADDRESS *addrP);

#ifdef __cplusplus
}
#endif

#endif//_SOCKET_H_
