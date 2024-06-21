/****************************************************************************
 * 
 *   FileName: udp_com..c
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: UDP com interface 
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_types.h"
#include "err_defs.h"


#include "udp_com.h"

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
   PSOCKET                    ipSocketTx;
   PSOCKET                    ipSocketRx;
   PADDRESS                   ipAddrPeer;
   PADDRESS                   ipAddrMe;
} UDP_COMP_infoT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static UDP_COMP_infoT udpInfo;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/******************************************************************************
 ***************     P R E    D E F I N I T I O N     OF        ***************
 ***************     L O C A L         F U N C T I O N S        ***************
 ******************************************************************************/
static ERRG_codeE UDP_COMP_open(void **connHandle, void *openCfgP);
static ERRG_codeE UDP_COMP_init(UDP_COMG_interfaceParamsT *openCfgP, UDP_COMP_infoT *comInfoP);
static ERRG_codeE UDP_COMP_close(void *connHandle);
static ERRG_codeE UDP_COMP_send(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenSend);
static ERRG_codeE UDP_COMP_recv(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenRecv);
static ERRG_codeE UDP_COMP_peek(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenRecv);

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: UDP_COMP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: UDP connection
*
****************************************************************************/
static ERRG_codeE UDP_COMP_open(void **connHandle, void *openCfgP)
{
   ERRG_codeE                 retVal = UDP_COM__RET_SUCCESS;
   UDP_COMP_infoT *comInfoP = &udpInfo;

   UDP_COMP_init((UDP_COMG_interfaceParamsT *)openCfgP, comInfoP);

   retVal = SOCKETG_create(SOCKETG_TYPE_UDP_E, &(comInfoP->ipSocketTx));

   if(ERRG_SUCCEEDED(retVal))
   {
      retVal = SOCKETG_create(SOCKETG_TYPE_UDP_E, &(comInfoP->ipSocketRx));
   }

   if(ERRG_SUCCEEDED(retVal))
   {
      retVal = SOCKETG_bind(comInfoP->ipSocketRx, &(comInfoP->ipAddrMe));
   }

   if(ERRG_SUCCEEDED(retVal))
   {
      *connHandle = comInfoP;
   }
   return retVal;
}

/****************************************************************************
*
*  Function Name: UDP_COMP_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: UDP connection
*
****************************************************************************/
static ERRG_codeE UDP_COMP_init(UDP_COMG_interfaceParamsT *openCfgP, UDP_COMP_infoT *comInfoP)
{
   ERRG_codeE                 retVal = UDP_COM__RET_SUCCESS;

   retVal = SOCKETG_initialize();

   if(ERRG_SUCCEEDED(retVal))
   {
      retVal = SOCKETG_addressCreate((const char *)openCfgP->ipAddrPeerP, openCfgP->portTx, &comInfoP->ipAddrPeer);
   }

   if(ERRG_SUCCEEDED(retVal))
   {
      retVal = SOCKETG_addressCreate(NULL, openCfgP->portRx, &comInfoP->ipAddrMe);
   }

   return retVal;
}

/****************************************************************************
*
*  Function Name: UDP_COMP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: UDP connection
*
****************************************************************************/
static ERRG_codeE UDP_COMP_close(void *connHandle)
{
   UDP_COMP_infoT *comInfoP= (UDP_COMP_infoT *)connHandle;

   SOCKETG_close(comInfoP->ipSocketTx);
   SOCKETG_close(comInfoP->ipSocketRx);
   SOCKETG_cleanup();

   return UDP_COM__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: UDP_COMP_send
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: UDP connection
*
****************************************************************************/
static ERRG_codeE UDP_COMP_send(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenSend)
{
   ERRG_codeE                 retVal = UDP_COM__RET_SUCCESS;
   int                        tmpLen = (int)len;
   UDP_COMP_infoT *comInfoP= (UDP_COMP_infoT *)connHandle;

   retVal = SOCKETG_sendTo(comInfoP->ipSocketTx,(char *)pBuff, &tmpLen, 0, &comInfoP->ipAddrPeer);
   if(ERRG_FAILED(retVal))
   {
      *byteLenSend = 0;
      retVal = UDP_COM__ERR_GENERAL_ERROR;
   }
   else
   {
      *byteLenSend = tmpLen;
   }

   return retVal;
}

/****************************************************************************
*
*  Function Name: UDP_COMP_recv
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: UDP connection
*
****************************************************************************/
static ERRG_codeE UDP_COMP_recv(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenRecv)
{
   ERRG_codeE                 retVal = UDP_COM__RET_SUCCESS;
   PADDRESS                   ipOther;
   int                        tmpLen = (int)len;
   UDP_COMP_infoT *comInfoP= (UDP_COMP_infoT *)connHandle;

   retVal = SOCKETG_recvFrom(comInfoP->ipSocketRx, (char *)pBuff, &tmpLen, 0, &ipOther);
   if(ERRG_FAILED(retVal))
   {
      *byteLenRecv=0;
      retVal = UDP_COM__ERR_GENERAL_ERROR;
   }
   else
   {
      *byteLenRecv = tmpLen;
   }
   return retVal;
}

/****************************************************************************
*
*  Function Name: UDP_COMP_peek
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: UDP connection
*
****************************************************************************/
static ERRG_codeE UDP_COMP_peek(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenRecv)
{
   ERRG_codeE                 retVal= UDP_COM__RET_SUCCESS;
   PADDRESS                   ipOther;
   int                        tmpLen = (int)len;
   UDP_COMP_infoT *comInfoP= (UDP_COMP_infoT *)connHandle;

   retVal = SOCKETG_recvFrom(comInfoP->ipSocketRx, (char *)pBuff, &tmpLen, MSG_PEEK, &ipOther);
   if(ERRG_FAILED(retVal))
   {
      *byteLenRecv=0;
      retVal = UDP_COM__ERR_GENERAL_ERROR;
   }
   else
   {
      *byteLenRecv = tmpLen;
   }

   return retVal;
}

void UDP_COMP_getCaps(void *connHandle, CLS_COMG_capsT *capsP)
{
   (void)connHandle;
   capsP->flags = 0;
   capsP->flags |= CLS_COMG_CAP_RX;
   capsP->flags |= CLS_COMG_CAP_TX;
   capsP->flags |= CLS_COMG_CAP_PEEK;
   capsP->flags |= CLS_COMG_CAP_MODE_STREAM;
   capsP->fixedSize = 0;
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: UDP_COMG_getOps
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: UDP connection
*
****************************************************************************/
void UDP_COMG_getOps(CLS_COMG_ifOperations *ifOpsP)
{
   ifOpsP->open    = &UDP_COMP_open;
   ifOpsP->close   = &UDP_COMP_close;
   ifOpsP->getCaps  = &UDP_COMP_getCaps;
   ifOpsP->send    = &UDP_COMP_send;
   ifOpsP->recv    = &UDP_COMP_recv;
   ifOpsP->peek    = &UDP_COMP_peek;
   ifOpsP->recv_unbuffered = NULL;
   ifOpsP->sendv_unbuffered = NULL;
}

#ifdef __cplusplus
}
#endif
