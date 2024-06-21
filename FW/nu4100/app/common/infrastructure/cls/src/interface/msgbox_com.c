/****************************************************************************
 * 
 *   FileName: msgbox_com.c
 *
 *   Author: Ram B.
 *
 *   Date: 
 *
 *   Description: INU message box communication connection
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"
#include "msgbox_com.h"

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

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: MSGBOX_COMP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: message box connection
*
****************************************************************************/
ERRG_codeE MSGBOX_COMP_open(void **connHandle, void *openCfgP)
{
   ERRG_codeE ret = MSGBOX_COM__RET_SUCCESS;
   MSGBOX_COMG_interfaceParamsT *msgBoxP = (MSGBOX_COMG_interfaceParamsT *)openCfgP;
   MSG_BOXG_handleT handle;
   ret = MSG_BOXG_init();
   
   if(ERRG_SUCCEEDED(ret))
   {
      ret = MSG_BOXG_open(&handle, msgBoxP->inst);
      if(handle)
         *connHandle = handle;
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: MSGBOX_COMP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: message box connection
*
****************************************************************************/
static ERRG_codeE MSGBOX_COMP_close(void *connHandle)
{
   FIX_UNUSED_PARAM_WARN(connHandle);
   return MSGBOX_COM__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: MSGBOX_COMP_add
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: message box connection
*
****************************************************************************/
static ERRG_codeE MSGBOX_COMP_add(void *connHandle, UINT8* bufP, UINT32 len, UINT32 *bytesSentP)
{
   ERRG_codeE ret = MSGBOX_COM__RET_SUCCESS;
   MSG_BOXG_handleT handle = (MSG_BOXG_handleT)connHandle;

   if(len > MSG_BOXP_getMsgSize(handle))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "msg box error. len too large. len=%d, msg size=%d\n", len ,MSG_BOXP_getMsgSize(handle));
      //fprintf(stderr, "%s() - len too large %u %u\n", __func__, len, MSG_BOXP_getMsgSize(msgBoxP->handle));
      ret = MSGBOX_COM__ERR_FAIL_ADD_MSG_LEN_TOO_LARGE; //Error msg len too large
   }
   else
   {
      ret = MSG_BOXG_add(handle, bufP, len);
      if(ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL,  "msg box Overflow. message box is full.\n");
         //Overflow on msg box - return 0 bytes and OK status.
         *bytesSentP = 0;
      }
      else
      {
         *bytesSentP = len ;
      }
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: MSGBOX_COMP_rem
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: message box connection
*
****************************************************************************/
static ERRG_codeE MSGBOX_COMP_rem(void *connHandle, UINT8* bufP, UINT32 len, UINT32 *bytesReadP)
{
   ERRG_codeE ret = MSGBOX_COM__RET_SUCCESS;
   MSG_BOXG_handleT handle = (MSG_BOXG_handleT)connHandle;

   if(len > MSG_BOXP_getMsgSize(handle))
   {
      ret = MSGBOX_COM__ERR_FAIL_REMOVE_MSG_LEN_TOO_LARGE; //Error msg len too large
   }
   else
   {
      ret = MSG_BOXG_rem(handle, bufP, len);
      if(ERRG_FAILED(ret))
      {
         //Empty msg box - return 0 bytes and ERROR status.
         *bytesReadP = 0;
      }
      else
      {
         *bytesReadP = len;
      }
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: MSGBOX_COMP_peek
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: message box connection
*
****************************************************************************/
static ERRG_codeE MSGBOX_COMP_peek(void *connHandle, UINT8* bufP, UINT32 len, UINT32 *bytesToReadP)
{
   ERRG_codeE ret = MSGBOX_COM__RET_SUCCESS;
   MSG_BOXG_handleT handle = (MSG_BOXG_handleT)connHandle;

   if(len > MSG_BOXP_getMsgSize(handle))
   {
      ret = MSGBOX_COM__ERR_FAIL_PEEK_MSG_LEN_TOO_LARGE; //Error msg len too large
   }
   else
   {
      //Peek - check avaialbe data without removing from msgbox
      ret = MSG_BOXG_peek(handle, bufP, len);
      if(ERRG_FAILED(ret))
      {
         //Empty msg box - return 0 bytes and OK status.
         *bytesToReadP = 0;
      }
      else
      {
         *bytesToReadP = len;
      }
   }
   return ret;
}


void MSGBOX_COMG_getCaps(void *connHandle,CLS_COMG_capsT *capsP)
{
   (void)connHandle;
   capsP->flags = 0;
   capsP->flags |= CLS_COMG_CAP_RX;
   capsP->flags |= CLS_COMG_CAP_TX;
   capsP->flags |= CLS_COMG_CAP_MODE_STREAM;
   capsP->fixedSize = 0;
}

/****************************************************************************
 ***************       G L O B A L     F U N C T I O N        ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: MSGBOX_COMG_getOps
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: message box connection
*
****************************************************************************/
void MSGBOX_COMG_getOps(CLS_COMG_ifOperations *ifOpsP)
{
   ifOpsP->open    = &MSGBOX_COMP_open;
   ifOpsP->close   = &MSGBOX_COMP_close;
   ifOpsP->send    = &MSGBOX_COMP_add;
   ifOpsP->recv    = &MSGBOX_COMP_rem;
   ifOpsP->peek    = &MSGBOX_COMP_peek;
   ifOpsP->recv_unbuffered = NULL;
   ifOpsP->sendv_unbuffered = NULL;
}

