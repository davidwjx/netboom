/****************************************************************************
 * 
 *   FileName: stub_com.c
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
#include "inu_types.h"
#include "err_defs.h"

#include "log.h"

#include "stub_com.h"
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

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
 
/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/
 
/******************************************************************************
 ***************     P R E    D E F I N I T I O N     OF        ***************
 ***************     L O C A L         F U N C T I O N S        ***************
 ******************************************************************************/
static ERRG_codeE STUB_COMP_open(void **connHandle, void *openCfg);
static ERRG_codeE STUB_COMP_close(void *connHandle);
static ERRG_codeE STUB_COMP_send(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenSendP);
static ERRG_codeE STUB_COMP_recv(void *connHandle, UINT8* pBuff, UINT32 len, UINT32 *byteLenRecvP);

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: STUB_COMP_open
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
static ERRG_codeE STUB_COMP_open(void **connHandle, void *openCfg)
{
   (void)connHandle;
   (void)openCfg;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "open\n");
   return (ERRG_codeE)RET_SUCCESS;

}

/****************************************************************************
*
*  Function Name: STUB_COMP_close
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
static ERRG_codeE STUB_COMP_close(void *connHandle)
{
   (void)connHandle;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "close\n");
   return (ERRG_codeE)RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: STUB_COMP_send
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
static ERRG_codeE STUB_COMP_send(void *connHandle, UINT8* bufP, UINT32 len, UINT32 *byteLenSendP)
{
   (void)connHandle;
   (void)bufP;
   (void)len;
   (void)byteLenSendP;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "send\n");
   return (ERRG_codeE)RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: STUB_COMP_recv
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
static ERRG_codeE STUB_COMP_recv(void *connHandle, UINT8* bufP, UINT32 len, UINT32 *byteLenRecvP)
{
   (void)connHandle;
   (void)bufP;
   (void)len;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "recv\n");
   OS_LYRG_usleep(1000000);//simluate timeout
   *byteLenRecvP = 0;
   return CONN_LYR__ERR_RECV_TIMEOUT;
}

static void STUB_COMP_getCaps(void *connHandle, CLS_COMG_capsT *capsP)
{
   (void)connHandle;
   capsP->flags  =  0;
   capsP->flags |= CLS_COMG_CAP_RX;
   capsP->flags |= CLS_COMG_CAP_TX;
   capsP->flags |= CLS_COMG_CAP_MODE_STREAM;   
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: STUB_COMG_getOps
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
void STUB_COMG_getOps(CLS_COMG_ifOperations *ifOpsP)
{
   ifOpsP->open        = &STUB_COMP_open;
   ifOpsP->close       = &STUB_COMP_close;
   ifOpsP->getCaps     = &STUB_COMP_getCaps;
   ifOpsP->send        = &STUB_COMP_send;
   ifOpsP->sendv       = NULL;
   ifOpsP->recv        = &STUB_COMP_recv;
   ifOpsP->recvContBuf = NULL;
   ifOpsP->peek        = NULL;
   ifOpsP->recv_unbuffered = NULL;
   ifOpsP->sendv_unbuffered = NULL;
}

#ifdef __cplusplus
}
#endif
