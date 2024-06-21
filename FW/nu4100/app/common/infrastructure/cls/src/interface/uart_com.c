/****************************************************************************
 * 
 *   FileName: uart_com..c
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: UART com interface 
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "os_lyr.h"
#include "inu_common.h"
#include "uart_com.h"
#include "serial.h"


/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/

typedef enum
{
   UART_NONE_E,
   UART_CLOSE_E,
   UART_OPEN_E
} UART_COMP_stateE;

typedef struct
{
#ifdef UART_HDLC
   UINT32   stuffedBuffLen;
   BYTE     pStuffedBuff[(UART_COMP_MAX_MSG_LEN*2) + 2];
#endif
   void *   uartHandle;
} UART_COMP_infoT;

typedef enum
{
   TX_START_E,
   TX_IN_MSG_E,
   TX_SENDING_7E_E,
   TX_SENDING_7D_E,
   TX_FINISH_E,
   TX_IDLE_E,
} UART_COMP_txStateMachineE;

typedef enum
{
   UART_COMP_RX_IDLE_E,
   UART_COMP_RX_FLAG_FOUND_E,
   UART_COMP_RX_IN_MSG_E,
   UART_COMP_RX_ESC_CODE_DETECTED_E,
   UART_COMP_RX_IGNORE_E,
   UART_COMP_RX_FINISH
} UART_COMP_rxStateMachineE;

#ifndef __UART_ON_FPGA__   
//#define UART_HDLC
#endif
//#define UART_PRINT

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/

#ifdef __UART_ON_FPGA__   
#define UART_COMP_TIME_OUT_MS       (300000)
#else
#define UART_COMP_TIME_OUT_MS       (2000)
#endif
static BOOL uartRecvStay = TRUE;
static UART_COMP_stateE uartState=UART_NONE_E;
static UART_COMP_rxStateMachineE rxState = UART_COMP_RX_IDLE_E;
static UART_COMP_txStateMachineE txState = TX_IDLE_E;


/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static PUART uartHandle;
static UART_COMP_infoT uartInfo;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/******************************************************************************
 ***************     P R E    D E F I N I T I O N     OF        ***************
 ***************     L O C A L         F U N C T I O N S        ***************
 ******************************************************************************/
#ifdef UART_HDLC
static void UART_COMP_buffCommStuffPrepare(UINT8* pBuff, UINT32 buffLen, UART_COMP_infoT *pUart);
#endif

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
#ifdef UART_CRC16
/* CRC16 implementation acording to CCITT standards */

static const unsigned short crc16tab[256]= {
   0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
   0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
   0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
   0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
   0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
   0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
   0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
   0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
   0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
   0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
   0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
   0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
   0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
   0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
   0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
   0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
   0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
   0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
   0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
   0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
   0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
   0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
   0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
   0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
   0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
   0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
   0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
   0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
   0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
   0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
   0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
   0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};
  
unsigned short UART_COMP_crc16_ccitt(const void *buf, int len)
{
   register int counter;
   register unsigned short crc = 0;
   for( counter = 0; counter < len; counter++)
      crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *(char *)buf++)&0x00FF];
   return crc;
}

#endif


#ifdef UART_HDLC
/****************************************************************************
*
*  Function Name: UART_COMP_buffCommStuffPrepare
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: UART connection
*
****************************************************************************/
static void UART_COMP_buffCommStuffPrepare(UINT8* pBuff, UINT32 buffLen, UART_COMP_infoT *pUart)
{
   UINT32  ix;
   UINT32  tmpLen = 0;
   pUart->pStuffedBuff[tmpLen] = 0x7E;
   tmpLen++;
   for (ix=0; ix<buffLen; ix++)
   {
      if (pBuff[ix] == 0x7E)
      {
         pUart->pStuffedBuff[tmpLen] = 0x7D;
         pUart->pStuffedBuff[tmpLen+1] = 0x5E;
         tmpLen += 2;
      }
      else if (pBuff[ix] == 0x7D)
      {
         pUart->pStuffedBuff[tmpLen] = 0x7D;
         pUart->pStuffedBuff[tmpLen+1] = 0x5D;
         tmpLen += 2;
      }
      else
      {
         pUart->pStuffedBuff[tmpLen] = pBuff[ix];
         tmpLen++;
      }
   }

   pUart->pStuffedBuff[tmpLen] = 0x7E;
   tmpLen++;

   pUart->stuffedBuffLen = tmpLen;
}
#endif

/****************************************************************************
*
*  Function Name: UART_COMP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: UART connection
*
****************************************************************************/
static ERRG_codeE UART_COMP_open(void **connHandle, void *openCfgP)
{
   ERRG_codeE retVal = UART_COM__RET_SUCCESS;
   UART_COMG_interfaceParamsT *cfgP = (UART_COMG_interfaceParamsT *)openCfgP;
   UART_COMP_infoT *comInfoP = &uartInfo;
   char uartPortName[MAX_UART_PORT_NAME_LEN] = {};

   comInfoP->uartHandle = &uartHandle;
   uartState = UART_OPEN_E;

   sprintf(uartPortName,"%d",cfgP->portNumber);

   retVal = SERIALG_open((PUART *)comInfoP->uartHandle, uartPortName, cfgP->baudrate, SERIALG_TYPE_UART);
   if(ERRG_SUCCEEDED(retVal))
   {
      *connHandle = comInfoP;
   }
   return retVal;
}

/****************************************************************************
*
*  Function Name: UART_COMP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: UART connection
*
****************************************************************************/
static ERRG_codeE UART_COMP_close(void *connHandle)
{
   ERRG_codeE  retVal = UART_COM__RET_SUCCESS;
   UART_COMP_infoT *comInfoP = (UART_COMP_infoT *)connHandle;

   if (comInfoP)
   {
     uartRecvStay = FALSE;
     uartState = UART_CLOSE_E; 
     while ((txState != TX_IDLE_E) ||  (rxState != UART_COMP_RX_IDLE_E)) {};
     retVal = SERIALG_close((PUART *)comInfoP->uartHandle);        
   }
   else
   {
      retVal = UART_COM__ERR_CLOSE_FAIL;
   }

   return retVal;
}

/****************************************************************************
*
*  Function Name: UART_COMP_send
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: UART connection
*
****************************************************************************/
static ERRG_codeE UART_COMP_send(void *connHandle,  UINT8* pBuff, UINT32 len, UINT32* byteLenSend)
{
   ERRG_codeE retVal = UART_COM__RET_SUCCESS;
   UART_COMP_infoT *comInfoP = (UART_COMP_infoT *)connHandle;
#ifdef UART_CRC16
   unsigned short crc;
#endif

#ifdef UART_PRINT
   UINT32 i;
   UINT8 *temp = pBuff;
#endif

   FIX_UNUSED_PARAM_WARN(byteLenSend);
    // LOGG_PRINT(LOG_INFO_E, NULL , "sendlen %d \n",len);
   if (comInfoP && uartState == UART_OPEN_E)
   {
      txState = TX_START_E;
   
#ifdef UART_HDLC
      UART_COMP_buffCommStuffPrepare(pBuff,len, comInfoP);
      retVal = SERIALG_send((PUART*)comInfoP->uartHandle, (INT8 *)comInfoP->pStuffedBuff, comInfoP->stuffedBuffLen, 0xFFFFFFFF);
#else
      retVal = SERIALG_send((PUART*)comInfoP->uartHandle, (INT8 *)pBuff, len, 0xFFFFFFFF);
  #ifdef UART_CRC16      
      crc = UART_COMP_crc16_ccitt(pBuff,len);      
  #endif
#endif

#ifdef UART_PRINT
      printf("sendlen %d ",len);
      for (i = 0; i < len; i++)
      {
         printf("0x%x ",*temp);
         temp++;
      }
      printf("\n");
#endif

#ifdef UART_CRC16
      retVal = SERIALG_send((PUART*)comInfoP->uartHandle, (INT8 *)&crc, sizeof(crc), 0xFFFFFFFF);
#endif

        txState = TX_IDLE_E;     
   }
   return retVal;
}


ERRG_codeE UART_CTRL_COMP_sendv(void *connHandle,  void *bufP, UINT32 *bytesCntP)
{
   UINT32 byteCnt = 0;
   UINT32 byteCntTotal = 0;
   ERRG_codeE ret = USB_CTRL_COM__ERR_INVALID_ARGS;
   MEM_POOLG_bufDescT *currP = (MEM_POOLG_bufDescT *)bufP;

   //This is a naive implementation.
   //TODO: use scatter-gather mechanism when supported by driver
   while(currP)
   {
      byteCnt = 0;      
      //Copying to buffer before send due to performance issue in sending on socket from external buffer
      ret = UART_COMP_send(connHandle, currP->dataP, currP->dataLen, &byteCnt);
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
*  Function Name: UART_COMP_recv
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: UART connection
*
****************************************************************************/
#ifdef UART_HDLC
static ERRG_codeE UART_COMP_recv(void *connHandle, UINT8* pBuff, UINT32 len, UINT32* byteLenRecv)
{
   UINT32 i=0;
   //UINT32 j=0;
   UINT32 bytesRead=0;
   UINT8 val=0;
   ERRG_codeE ret = UART_COM__RET_SUCCESS;
   UART_COMP_infoT *comInfoP = (UART_COMP_infoT *)connHandle;
   *byteLenRecv = 0;
   uartRecvStay = TRUE;
   rxState = UART_COMP_RX_IDLE_E;
   if (comInfoP && uartState == UART_OPEN_E)
   {
      while (uartRecvStay)
      {
         ret = SERIALG_recv( (PUART*)comInfoP->uartHandle, &val,1,&bytesRead, UART_COMP_TIME_OUT_MS );
#ifdef UART_PRINT
         printf("0x%x ",val);
#endif      
         switch (rxState)
         {
            case UART_COMP_RX_IDLE_E:
               if (val == 0x7E)
               {
                  rxState = UART_COMP_RX_FLAG_FOUND_E;
               }
               break;

            case UART_COMP_RX_FLAG_FOUND_E:
               if (val == 0x7D)
               {
                  rxState = UART_COMP_RX_ESC_CODE_DETECTED_E;
               }
               else if (val != 0x7E)
               {
                  if (i >= len)
                  {
                     printf("UART_COMP_RX_IGNORE_E 1 byteLenRecv = %d len = %d i=%d\n",*byteLenRecv,len,i);
                     rxState = UART_COMP_RX_IGNORE_E;
                     *byteLenRecv=0;
                     return UART_COM__ERR_GENERAL_ERROR;
                  }            
                  pBuff[i] = val;
                  i++;
                  rxState = UART_COMP_RX_IN_MSG_E;
               }
               break;

            case UART_COMP_RX_IN_MSG_E:
               if (val == 0x7D)
               {
                  rxState = UART_COMP_RX_ESC_CODE_DETECTED_E;
               }
               else if (val == 0x7E)
               {
                  rxState= UART_COMP_RX_FINISH;
                  if (*byteLenRecv == len)
                  {
                        uartRecvStay = FALSE;
                  }
                  else
                  {
                     rxState = UART_COMP_RX_IDLE_E;
                  }
               }
               else
               {
                  if (i >= len)
                  {
                     printf("UART_COMP_RX_IGNORE_E 2 byteLenRecv = %d len = %d i=%d\n",*byteLenRecv,len,i);
                     rxState = UART_COMP_RX_IGNORE_E;
                     *byteLenRecv=0;
                     return UART_COM__ERR_GENERAL_ERROR;
                  }
                  pBuff[i]= val;
                  i++;
               }
               break;

            case UART_COMP_RX_ESC_CODE_DETECTED_E:
               if (val == 0x5E)
               {
                  if (i >= len)
                  {
                     printf("UART_COMP_RX_IGNORE_E 3 byteLenRecv = %d len = %d i=%d\n",*byteLenRecv,len,i);
                     rxState = UART_COMP_RX_IGNORE_E;
                     *byteLenRecv=0;
                     return UART_COM__ERR_GENERAL_ERROR;
                    }            
                  pBuff[i] = 0x7E;
                  i++;
                  rxState = UART_COMP_RX_IN_MSG_E;
               }
               else if (val == 0x5D)
               {
                  if (i >= len)
                  {
                     printf("UART_COMP_RX_IGNORE_E 4 byteLenRecv = %d len = %d i=%d\n",*byteLenRecv,len,i);
                     rxState = UART_COMP_RX_IGNORE_E;
                     *byteLenRecv=0;
                     return UART_COM__ERR_GENERAL_ERROR;
                    }               
                  pBuff[i] = 0x7D;
                  i++;
                  rxState = UART_COMP_RX_IN_MSG_E;
               }
               else
               {
                  i = 0;
                  rxState = UART_COMP_RX_IGNORE_E;
                  return UART_COM__ERR_GENERAL_ERROR;  /* FLAG/other while escaped - must be an error */
               }
               break;

            case UART_COMP_RX_IGNORE_E:
               /* not supposed to happen, but we'll allow it for now */
               if (val == 0x7E)
               {
                  rxState = UART_COMP_RX_IDLE_E;
               }
               break;

            case UART_COMP_RX_FINISH:
               break;

         }

         /* if message is too long, ignore it */
         if (i > len)
         {
#ifdef UART_PRINT         
            printf("\n");
#endif
            LOGG_PRINT(LOG_ERROR_E, NULL ,"UART_COMP_RX_IGNORE_E byteLenRecv = %d len = %d i=%d\n",*byteLenRecv,len,i);
            rxState = UART_COMP_RX_IGNORE_E;
            *byteLenRecv=0;
            return UART_COM__ERR_GENERAL_ERROR;
         }

         *byteLenRecv=i;

      }
#ifdef UART_PRINT      
      printf("\n");
#endif
   }
   else
   {
      return UART_COM__ERR_GENERAL_ERROR;
   }
   rxState = UART_COMP_RX_IDLE_E;

   return ret;
}
#else
static ERRG_codeE UART_COMP_recv(void *connHandle, UINT8* pBuff, UINT32 len, UINT32* byteLenRecv)
{
   ERRG_codeE ret = UART_COM__RET_SUCCESS;
   UART_COMP_infoT *comInfoP = (UART_COMP_infoT *)connHandle;
   UINT32 dummy;
   UINT32 bytesRead=0;
#ifdef UART_CRC16
   unsigned short crc, remoteCrc;
   UINT32 remoteCrcBytesRead;
#endif
#ifdef UART_PRINT
   UINT32 i;
   UINT8 *temp = pBuff;
#endif

   ret = SERIALG_recv( (PUART*)comInfoP->uartHandle, pBuff,len,&bytesRead, UART_COMP_TIME_OUT_MS );
   *byteLenRecv = bytesRead;
 //  printf("UART_COMP_recv: bytesRead=%d len=%d, ret - 0x%x\n",bytesRead,len, ret);

#ifdef UART_PRINT
      printf("recvlen %d ",len);
      for (i = 0; i < len; i++)
      {
         printf("0x%x ",*temp);
         temp++;
      }
      printf("\n");
#endif

#ifdef UART_CRC16
   crc = UART_COMP_crc16_ccitt(pBuff,len);
   ret = SERIALG_recv( (PUART*)comInfoP->uartHandle, (UINT8*)&remoteCrc,sizeof(remoteCrc),&remoteCrcBytesRead, UART_COMP_TIME_OUT_MS );

   if (crc != remoteCrc)
   {
      *byteLenRecv = 0;
   }
#endif      

   if (bytesRead != len)
   {
      printf("error! bytesRead=%d len=%d\n",bytesRead,len);
      ret = UART_COM__ERR_GENERAL_ERROR;
      *byteLenRecv = 0;
   }
#if (DEFSG_PROCESSOR == DEFSG_GP)
   else
   {
   /* - fixme
      MEM_MAPG_convertVirtualToPhysical2((UINT32)pBuff, &dummy, MEM_MAPG_DDR_CDE_BUFFERS_E);
	  if(dummy)
 	  {
 	  	 CMEM_cacheWb((void *)(pBuff), len);
 	  }
*/ 
   }
#endif   

   return ret;
}
#endif

void UART_COMP_getCaps(void *connHandle, CLS_COMG_capsT *capsP)
{
   (void)connHandle;
   capsP->flags = 0;
   capsP->flags |= CLS_COMG_CAP_RX;
   capsP->flags |= CLS_COMG_CAP_TX;
   capsP->flags |= CLS_COMG_CAP_SENDV;
   capsP->flags |= CLS_COMG_CAP_MODE_STREAM;
   capsP->fixedSize = 0;
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: UART_COMG_getOps
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: UART connection
*
****************************************************************************/
void UART_COMG_getOps(CLS_COMG_ifOperations *ifOpsP)
{
   ifOpsP->open    = &UART_COMP_open;
   ifOpsP->close   = &UART_COMP_close;
   ifOpsP->config  = NULL;
   ifOpsP->getCaps  = UART_COMP_getCaps;
   ifOpsP->send    = &UART_COMP_send;
   ifOpsP->sendv   = &UART_CTRL_COMP_sendv;
   ifOpsP->recv    = &UART_COMP_recv;
   ifOpsP->peek    = NULL;
   ifOpsP->recv_unbuffered = NULL;
   ifOpsP->sendv_unbuffered = NULL;
}


