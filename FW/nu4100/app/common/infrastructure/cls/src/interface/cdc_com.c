/****************************************************************************
 * 
 *   FileName: cdc_com..c
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: CDC com interface 
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "os_lyr.h"
#include "inu_common.h"
#include "cdc_com.h"
#include "serial.h"

//#deinde CDC_HDLC
/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/

typedef enum
{
   CDC_NONE_E,
   CDC_CLOSE_E,
   CDC_OPEN_E
} CDC_COMP_stateE;

typedef struct
{
#ifdef CDC_HDLC
   UINT32   stuffedBuffLen;
   BYTE     pStuffedBuff[(CDC_COMP_MAX_MSG_LEN*2) + 2];
#endif
   void *   cdcHandle;
} CDC_COMP_infoT;

typedef enum
{
   TX_START_E,
   TX_IN_MSG_E,
   TX_SENDING_7E_E,
   TX_SENDING_7D_E,
   TX_FINISH_E,
   TX_IDLE_E,
} CDC_COMP_txStateMachineE;

typedef enum
{
   CDC_COMP_RX_IDLE_E,
   CDC_COMP_RX_FLAG_FOUND_E,
   CDC_COMP_RX_IN_MSG_E,
   CDC_COMP_RX_ESC_CODE_DETECTED_E,
   CDC_COMP_RX_IGNORE_E,
   CDC_COMP_RX_FINISH
} CDC_COMP_rxStateMachineE;

//#define CDC_HDLC
//#define CDC_PRINT

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define CDC_COMP_TIME_OUT_MS       (2000)

static BOOL cdcRecvStay = TRUE;
static CDC_COMP_stateE cdcState=CDC_NONE_E;
static CDC_COMP_rxStateMachineE rxState = CDC_COMP_RX_IDLE_E;
static CDC_COMP_txStateMachineE txState = TX_IDLE_E;


/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static PUART cdcHandle;
static CDC_COMP_infoT cdcInfo;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/******************************************************************************
 ***************     P R E    D E F I N I T I O N     OF        ***************
 ***************     L O C A L         F U N C T I O N S        ***************
 ******************************************************************************/
#ifdef CDC_HDLC
static void CDC_COMP_buffCommStuffPrepare(UINT8* pBuff, UINT32 buffLen, CDC_COMP_infoT *pCdc);
#endif

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
#ifdef CDC_CRC16
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
  
unsigned short CDC_COMP_crc16_ccitt(const void *buf, int len)
{
   register int counter;
   register unsigned short crc = 0;
   for( counter = 0; counter < len; counter++)
      crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *(char *)buf++)&0x00FF];
   return crc;
}

#endif


#ifdef CDC_HDLC
/****************************************************************************
*
*  Function Name: CDC_COMP_buffCommStuffPrepare
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CDC connection
*
****************************************************************************/
static void CDC_COMP_buffCommStuffPrepare(UINT8* pBuff, UINT32 buffLen, CDC_COMP_infoT *pCdc)
{
   UINT32  ix;
   UINT32  tmpLen = 0;
   pCdc->pStuffedBuff[tmpLen] = 0x7E;
   tmpLen++;
   for (ix=0; ix<buffLen; ix++)
   {
      if (pBuff[ix] == 0x7E)
      {
         pCdc->pStuffedBuff[tmpLen] = 0x7D;
         pCdc->pStuffedBuff[tmpLen+1] = 0x5E;
         tmpLen += 2;
      }
      else if (pBuff[ix] == 0x7D)
      {
         pCdc->pStuffedBuff[tmpLen] = 0x7D;
         pCdc->pStuffedBuff[tmpLen+1] = 0x5D;
         tmpLen += 2;
      }
      else
      {
         pCdc->pStuffedBuff[tmpLen] = pBuff[ix];
         tmpLen++;
      }
   }

   pCdc->pStuffedBuff[tmpLen] = 0x7E;
   tmpLen++;

   pCdc->stuffedBuffLen = tmpLen;
}
#endif

/****************************************************************************
*
*  Function Name: CDC_COMP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CDC connection
*
****************************************************************************/
static ERRG_codeE CDC_COMP_open(void **connHandle, void *openCfgP)
{
   ERRG_codeE retVal = CDC_COM__RET_SUCCESS;
   CDC_COMG_interfaceParamsT *cfgP = (CDC_COMG_interfaceParamsT *)openCfgP;
   CDC_COMP_infoT *comInfoP = &cdcInfo;
   char cdcPortName[MAX_CDC_PORT_NAME_LEN] = {};

   comInfoP->cdcHandle = &cdcHandle;
   cdcState = CDC_OPEN_E;
#if DEFSG_IS_GP
   sprintf(cdcPortName, "%s", "/dev/ttyGS0");
#else
   sprintf(cdcPortName,"%d",cfgP->portNumber);
#endif
   retVal = SERIALG_open((PUART *)comInfoP->cdcHandle, cdcPortName, cfgP->baudrate, SERIALG_TYPE_CDC);
   if(ERRG_SUCCEEDED(retVal))
   {
      *connHandle = comInfoP;
   }
   return retVal;
}

/****************************************************************************
*
*  Function Name: CDC_COMP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CDC connection
*
****************************************************************************/
static ERRG_codeE CDC_COMP_close(void *connHandle)
{
   ERRG_codeE  retVal = CDC_COM__RET_SUCCESS;
   CDC_COMP_infoT *comInfoP = (CDC_COMP_infoT *)connHandle;

   if (comInfoP)
   {
     cdcRecvStay = FALSE;
     cdcState = CDC_CLOSE_E; 
     while ((txState != TX_IDLE_E) ||  (rxState != CDC_COMP_RX_IDLE_E)) {};
     retVal = SERIALG_close((PUART *)comInfoP->cdcHandle);        
   }
   else
   {
      retVal = CDC_COM__ERR_CLOSE_FAIL;
   }

   return retVal;
}

/****************************************************************************
*
*  Function Name: CDC_COMP_send
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CDC connection
*
****************************************************************************/
static ERRG_codeE CDC_COMP_send(void *connHandle,  UINT8* pBuff, UINT32 len, UINT32* byteLenSend)
{
   ERRG_codeE retVal = CDC_COM__RET_SUCCESS;
   CDC_COMP_infoT *comInfoP = (CDC_COMP_infoT *)connHandle;
#ifdef CDC_CRC16
   unsigned short crc;
#endif

#ifdef CDC_PRINT
   UINT32 i;
   UINT8 *temp = pBuff;
#endif

   FIX_UNUSED_PARAM_WARN(byteLenSend);
    // LOGG_PRINT(LOG_INFO_E, NULL , "sendlen %d \n",len);
   if (comInfoP && cdcState == CDC_OPEN_E)
   {
      txState = TX_START_E;
   
#ifdef CDC_HDLC
      CDC_COMP_buffCommStuffPrepare(pBuff,len, comInfoP);
      retVal = SERIALG_send((PUART*)comInfoP->cdcHandle, (INT8 *)comInfoP->pStuffedBuff, comInfoP->stuffedBuffLen, 0xFFFFFFFF);
#else
      retVal = SERIALG_send((PUART*)comInfoP->cdcHandle, (INT8 *)pBuff, len, 0xFFFFFFFF);
  #ifdef CDC_CRC16      
      crc = CDC_COMP_crc16_ccitt(pBuff,len);      
  #endif
#endif

#ifdef CDC_PRINT
      printf("sendlen %d ",len);
      for (i = 0; i < len; i++)
      {
         printf("0x%x ",*temp);
         temp++;
      }
      printf("\n");
#endif

#ifdef CDC_CRC16
      retVal = SERIALG_send((PUART*)comInfoP->cdcHandle, (INT8 *)&crc, sizeof(crc), 0xFFFFFFFF);
#endif

        txState = TX_IDLE_E;     
   }
   return retVal;
}


ERRG_codeE CDC_CTRL_COMP_sendv(void *connHandle,  void *bufP, UINT32 *bytesCntP)
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
      ret = CDC_COMP_send(connHandle, currP->dataP, currP->dataLen, &byteCnt);
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
*  Function Name: CDC_COMP_recv
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CDC connection
*
****************************************************************************/
#ifdef CDC_HDLC
static ERRG_codeE CDC_COMP_recv(void *connHandle, UINT8* pBuff, UINT32 len, UINT32* byteLenRecv)
{
   UINT32 i=0;
   //UINT32 j=0;
   UINT32 bytesRead=0;
   UINT8 val=0;
   ERRG_codeE ret = CDC_COM__RET_SUCCESS;
   CDC_COMP_infoT *comInfoP = (CDC_COMP_infoT *)connHandle;
   *byteLenRecv = 0;
   cdcRecvStay = TRUE;
   rxState = CDC_COMP_RX_IDLE_E;
   if (comInfoP && cdcState == CDC_OPEN_E)
   {
      while (cdcRecvStay)
      {
         ret = SERIALG_recv( (PUART*)comInfoP->cdcHandle, &val,1,&bytesRead, CDC_COMP_TIME_OUT_MS );
#ifdef CDC_PRINT
         printf("0x%x ",val);
#endif      
         switch (rxState)
         {
            case CDC_COMP_RX_IDLE_E:
               if (val == 0x7E)
               {
                  rxState = CDC_COMP_RX_FLAG_FOUND_E;
               }
               break;

            case CDC_COMP_RX_FLAG_FOUND_E:
               if (val == 0x7D)
               {
                  rxState = CDC_COMP_RX_ESC_CODE_DETECTED_E;
               }
               else if (val != 0x7E)
               {
                  if (i >= len)
                  {
                     printf("CDC_COMP_RX_IGNORE_E 1 byteLenRecv = %d len = %d i=%d\n",*byteLenRecv,len,i);
                     rxState = CDC_COMP_RX_IGNORE_E;
                     *byteLenRecv=0;
                     return CDC_COM__ERR_GENERAL_ERROR;
                  }            
                  pBuff[i] = val;
                  i++;
                  rxState = CDC_COMP_RX_IN_MSG_E;
               }
               break;

            case CDC_COMP_RX_IN_MSG_E:
               if (val == 0x7D)
               {
                  rxState = CDC_COMP_RX_ESC_CODE_DETECTED_E;
               }
               else if (val == 0x7E)
               {
                  rxState= CDC_COMP_RX_FINISH;
                  if (*byteLenRecv == len)
                  {
                        cdcRecvStay = FALSE;
                  }
                  else
                  {
                     rxState = CDC_COMP_RX_IDLE_E;
                  }
               }
               else
               {
                  if (i >= len)
                  {
                     printf("CDC_COMP_RX_IGNORE_E 2 byteLenRecv = %d len = %d i=%d\n",*byteLenRecv,len,i);
                     rxState = CDC_COMP_RX_IGNORE_E;
                     *byteLenRecv=0;
                     return CDC_COM__ERR_GENERAL_ERROR;
                  }
                  pBuff[i]= val;
                  i++;
               }
               break;

            case CDC_COMP_RX_ESC_CODE_DETECTED_E:
               if (val == 0x5E)
               {
                  if (i >= len)
                  {
                     printf("CDC_COMP_RX_IGNORE_E 3 byteLenRecv = %d len = %d i=%d\n",*byteLenRecv,len,i);
                     rxState = CDC_COMP_RX_IGNORE_E;
                     *byteLenRecv=0;
                     return CDC_COM__ERR_GENERAL_ERROR;
                    }            
                  pBuff[i] = 0x7E;
                  i++;
                  rxState = CDC_COMP_RX_IN_MSG_E;
               }
               else if (val == 0x5D)
               {
                  if (i >= len)
                  {
                     printf("CDC_COMP_RX_IGNORE_E 4 byteLenRecv = %d len = %d i=%d\n",*byteLenRecv,len,i);
                     rxState = CDC_COMP_RX_IGNORE_E;
                     *byteLenRecv=0;
                     return CDC_COM__ERR_GENERAL_ERROR;
                    }               
                  pBuff[i] = 0x7D;
                  i++;
                  rxState = CDC_COMP_RX_IN_MSG_E;
               }
               else
               {
                  i = 0;
                  rxState = CDC_COMP_RX_IGNORE_E;
                  return CDC_COM__ERR_GENERAL_ERROR;  /* FLAG/other while escaped - must be an error */
               }
               break;

            case CDC_COMP_RX_IGNORE_E:
               /* not supposed to happen, but we'll allow it for now */
               if (val == 0x7E)
               {
                  rxState = CDC_COMP_RX_IDLE_E;
               }
               break;

            case CDC_COMP_RX_FINISH:
               break;

         }

         /* if message is too long, ignore it */
         if (i > len)
         {
#ifdef CDC_PRINT         
            printf("\n");
#endif
            LOGG_PRINT(LOG_ERROR_E, NULL ,"CDC_COMP_RX_IGNORE_E byteLenRecv = %d len = %d i=%d\n",*byteLenRecv,len,i);
            rxState = CDC_COMP_RX_IGNORE_E;
            *byteLenRecv=0;
            return CDC_COM__ERR_GENERAL_ERROR;
         }

         *byteLenRecv=i;

      }
#ifdef CDC_PRINT      
      printf("\n");
#endif
   }
   else
   {
      return CDC_COM__ERR_GENERAL_ERROR;
   }
   rxState = CDC_COMP_RX_IDLE_E;

   return ret;
}
#else
static ERRG_codeE CDC_COMP_recv(void *connHandle, UINT8* pBuff, UINT32 len, UINT32* byteLenRecv)
{
   ERRG_codeE ret = CDC_COM__RET_SUCCESS;
   CDC_COMP_infoT *comInfoP = (CDC_COMP_infoT *)connHandle;
   UINT32 dummy;
   UINT32 bytesRead=0;
#ifdef CDC_CRC16
   unsigned short crc, remoteCrc;
   UINT32 remoteCrcBytesRead;
#endif
#ifdef CDC_PRINT
   UINT32 i;
   UINT8 *temp = pBuff;
#endif

   ret = SERIALG_recv( (PUART*)comInfoP->cdcHandle, pBuff,len,&bytesRead, CDC_COMP_TIME_OUT_MS );
   *byteLenRecv = bytesRead;
 //  printf("CDC_COMP_recv: bytesRead=%d len=%d, ret - 0x%x\n",bytesRead,len, ret);

#ifdef CDC_PRINT
      printf("recvlen %d ",len);
      for (i = 0; i < len; i++)
      {
         printf("0x%x ",*temp);
         temp++;
      }
      printf("\n");
#endif

#ifdef CDC_CRC16
   crc = CDC_COMP_crc16_ccitt(pBuff,len);
   ret = SERIALG_recv( (PUART*)comInfoP->cdcHandle, (UINT8*)&remoteCrc,sizeof(remoteCrc),&remoteCrcBytesRead, CDC_COMP_TIME_OUT_MS );

   if (crc != remoteCrc)
   {
      *byteLenRecv = 0;
   }
#endif      

   if (bytesRead != len)
   {
      printf("error! bytesRead=%d len=%d\n",bytesRead,len);
      ret = CDC_COM__ERR_GENERAL_ERROR;
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

void CDC_COMP_getCaps(void *connHandle, CLS_COMG_capsT *capsP)
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
*  Function Name: CDC_COMG_getOps
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CDC connection
*
****************************************************************************/
void CDC_COMG_getOps(CLS_COMG_ifOperations *ifOpsP)
{
   ifOpsP->open    = &CDC_COMP_open;
   ifOpsP->close   = &CDC_COMP_close;
   ifOpsP->config  = NULL;
   ifOpsP->getCaps = CDC_COMP_getCaps;
   ifOpsP->send    = &CDC_COMP_send;
   ifOpsP->sendv   = &CDC_CTRL_COMP_sendv;
   ifOpsP->recv    = &CDC_COMP_recv;
   ifOpsP->peek    = NULL;
   ifOpsP->recv_unbuffered = NULL;
   ifOpsP->sendv_unbuffered = NULL;
}


