/****************************************************************************
 * 
 *   FileName: inu_charger.c
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: INU charger driver
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/

#include "inu_common.h"
#include "inu2.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "inu_charger.h"
#include "serial.h"
#if DEFSG_IS_GP
#include <unistd.h>
#include "gpio_drv.h"
#include <errno.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <stdint.h>
#endif


/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
typedef struct
{
   UINT64               timestamp;
} CHARGERP_msgQParamsT;


#define CHARGERP_MSGQ_NAME      "/ChargerMsgQue"
#define CHARGERP_MSGQ_SIZE      (sizeof(CHARGERP_msgQParamsT))
#define CHARGERP_MSGQ_MAX_MSG   (20)
#define CHARGERP_MSGQ_TIMEOUT   (5000)


typedef enum
{
   READ_CMD,
   WRITE_CMD,
   NUM_CMDS,
}CHARGERP_readWriteCmd;

typedef enum
{
   LED_REGISTER_ADDR              = (0x0),
   POWER_CONTROL_REGISTER_ADDR    = (0x1),
   SOC_REGISTER_ADDR              = (0x2),
   POWER_TIMER_REGISTER_ADDR      = (0x3),
   CHARGER_STATUS_REGISTER_ADDR   = (0x4),
   INTERRUPT_STATUS_REGISTER_ADDR = (0x5),
   INTERRUPT_MASK_REGISTER_ADDR   = (0x6),
   VERSION_REGISTER_ADDR          = (0x7),
   CHIP_ID_REGISTER_ADDR          = (0x8),
   NUM_REGS       
}CHARGERP_regs_address;

#define AT828_IIS_CHIP_ID  (0x77)
#define AT828_V2_CHIP_ID   (0x88)
#define AT828_V3_CHIP_ID   (0x89)
#define AT828_M45S_CHIP_ID (0x8A)

#define CHARGERP_CHIP_ID_READ_MAX_TRY (3)
#ifndef MIN
    #define MIN(A,B) ((A)<(B) ? (A):(B))
#endif

#define CHARGERP_MAX_ADDR_LENGTH (2)
//r
#define READ_ASCII_VALUE  (114)
//w
#define WRITE_ASCII_VALUE (119)
//\r
#define EOL_ASCII_VALUE   (13)
//" "
#define SPACE_ASCII_VALUE (32)

#define READ_REPLY_LEN    (2)
#define WRITE_REPLY_LEN   (11)


#define SET_READ_OPCODE(buf,addr)   buf[0] = READ_ASCII_VALUE;\
                                    buf[1] = SPACE_ASCII_VALUE;\
                                    buf[2] = addr[0];\
                                    buf[3] = addr[1];\
                                    buf[4] = EOL_ASCII_VALUE;\

#define READ_CMD_LEN                (5)

#define SET_WRITE_ADDR(buf,addr,valueH,valueL) buf[0] = WRITE_ASCII_VALUE;\
                                               buf[1] = SPACE_ASCII_VALUE;\
                                               buf[2] = addr[0];\
                                               buf[3] = addr[1];\
                                               buf[4] = SPACE_ASCII_VALUE;\
                                               buf[5] = valueH;\
                                               buf[6] = valueL;\
                                               buf[7] = EOL_ASCII_VALUE;\

#define WRITE_CMD_LEN                          (8)

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
typedef struct
{
   inu_charger__init_charger_t initChargerParams;
   BOOL                        initialized;
   PUART                       uartH;
   UINT8                       regsAddress[NUM_REGS][CHARGERP_MAX_ADDR_LENGTH];
   OS_LYRG_threadHandle        thrdH;
   BOOL                        threadIsActive;
   OS_LYRG_msgQueT             msgQue;
   UINT8                       version;
} CHARGERP_infoT;

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static CHARGERP_infoT CHARGERP_params;

 /****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static ERRG_codeE CHARGERP_write( CHARGERP_infoT *paramsP, UINT8 *txBuf, UINT32 len, UINT32 timeout )
{
   if (paramsP->initChargerParams.debugEnable)
   {
      printf("write (len=%d): %s\n",len,txBuf);
   }
    
   return SERIALG_send(&paramsP->uartH,(INT8*)txBuf,len,timeout);
}


static ERRG_codeE CHARGERP_read( CHARGERP_infoT *paramsP, UINT8 *rxBuf, UINT32 len, UINT32 *bytesRead, UINT32 timeout )
{
   ERRG_codeE ret;
   ret = SERIALG_recv(&paramsP->uartH,rxBuf,len,bytesRead,timeout);
   if (paramsP->initChargerParams.debugEnable)
   {
      printf("read: %s\n",rxBuf);
   }
   return ret;
}



static UINT8 CHARGERP_extractData( UINT8 *rxBuf )
{
   UINT8 ret;
   //printf("extract: %d, %d -> ",rxBuf[0],rxBuf[1]);
   //Two actions here:
   //Convert from ASCII
   //Convert from Hex to Dec
   ret = (UINT8)strtol((const char*)rxBuf, NULL, 16);
   //printf("%d\n",ret);
   return ret;
}

/****************************************************************************
*
*  Function Name: CHARGERP_uartModeStart
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/ 
static ERRG_codeE CHARGERP_uartModeStart( CHARGERP_infoT *paramsP )
{
   char uartPortName[MAX_UART_PORT_NAME_LEN] = {};

   sprintf(uartPortName,"%d",paramsP->initChargerParams.uartPortNum);

   return SERIALG_open(&paramsP->uartH, uartPortName, paramsP->initChargerParams.uartBaudRate, SERIALG_TYPE_UART);
}
  
/****************************************************************************
*
*  Function Name: CHARGERP_uartModeStop
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/ 
static ERRG_codeE CHARGERP_uartModeStop( CHARGERP_infoT *paramsP )
{
   return SERIALG_close(&paramsP->uartH);
}

/****************************************************************************
*
*  Function Name: CHARGERP_initCmdsBuffer
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/ 
static void CHARGERP_initCmdsBuffer( CHARGERP_infoT *paramsP )
{
   UINT32 i;

   for(i = 0; i < NUM_REGS; i++)
   {
      paramsP->regsAddress[i][0] = 49; //ascii value for 1
      paramsP->regsAddress[i][1] = 48 + i; //ascii value for 0
   }   
}

/****************************************************************************
*
*  Function Name: CHARGERG_writeReg
*
*  Description: External function for LibFW's runtime_commands
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/ 
ERRG_codeE CHARGERG_writeReg(UINT8 reg, UINT8 regVal)
{
   UINT8 txBuf[20] = {0};
   //need to convert value to 2 digits hex:
   char hex[5] = {0};
   char reg_str[5] = {0};
   CHARGERP_infoT *paramsP = (CHARGERP_infoT*)&CHARGERP_params;
   
   sprintf(hex, "%02x", regVal);
   sprintf(reg_str, "%02x", reg);
   printf("hex = %s\n",hex);
   SET_WRITE_ADDR(txBuf,reg_str,hex[0],hex[1]);
   return CHARGERP_write(paramsP,txBuf,WRITE_CMD_LEN,1000);
}

/****************************************************************************
*
*  Function Name: CHARGERG_readReg
*
*  Description: External function for LibFW's runtime_commands
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/ 
ERRG_codeE CHARGERG_readReg(UINT8 reg, UINT8 *regValP )
{
   ERRG_codeE ret;
   UINT8 txBuf[20] = {0};
   UINT8 rxBuf[20] = {0};
   UINT8 reg_str[5] = {0};
   CHARGERP_infoT *paramsP = (CHARGERP_infoT*)&CHARGERP_params;
   UINT32 cnt;

   sprintf(reg_str, "%02x", reg);
   SET_READ_OPCODE(txBuf, reg_str);
   ret = CHARGERP_write(paramsP,txBuf,READ_CMD_LEN,1000);
   ret = CHARGERP_read(paramsP,rxBuf,READ_REPLY_LEN,&cnt,2000);
   *regValP = CHARGERP_extractData(rxBuf);
   return ret;
}

/****************************************************************************
*
*  Function Name: CHARGERP_writeReg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/ 
static ERRG_codeE CHARGERP_writeReg( CHARGERP_infoT *paramsP, CHARGERP_regs_address reg, UINT8 regVal )
{
   UINT8 txBuf[20] = {0};
   //need to convert value to 2 digits hex:
   char hex[5] = {0};

   sprintf(hex, "%02x", regVal);
   printf("hex = %s\n",hex);
   SET_WRITE_ADDR(txBuf,paramsP->regsAddress[reg],hex[0],hex[1]);
   return CHARGERP_write(paramsP,txBuf,WRITE_CMD_LEN,1000);
}

/****************************************************************************
*
*  Function Name: CHARGERP_readReg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/
static ERRG_codeE CHARGERP_readReg( CHARGERP_infoT *paramsP, CHARGERP_regs_address reg, UINT8 *regValP )
{
   ERRG_codeE ret;
   UINT8 txBuf[20] = {0};
   UINT8 rxBuf[20] = {0};
   UINT32 cnt;
   UINT8 addr[2];

   if (reg < 10)
   {
      SET_READ_OPCODE(txBuf, paramsP->regsAddress[reg]);
   }
   else
   {
      addr[0] = 0x30 + reg / 10;
      addr[1] = 0x30 + reg % 10;
      SET_READ_OPCODE(txBuf, addr);
   }

   ret = CHARGERP_write(paramsP, txBuf, READ_CMD_LEN, 1000);
   ret = CHARGERP_read(paramsP, rxBuf, READ_REPLY_LEN, &cnt, 2000);
   *regValP = CHARGERP_extractData(rxBuf);

   return ret;
}

/****************************************************************************
*
*  Function Name: CHARGERP_readVersionId
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/ 
static ERRG_codeE CHARGERP_readVersionId( CHARGERP_infoT *paramsP )
{
   UINT8 version = 0;
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   ret = CHARGERP_readReg(paramsP, VERSION_REGISTER_ADDR, &version);
   LOGG_PRINT(LOG_INFO_E, NULL, "version = 0x%x\n",version);
   paramsP->version = version;
     
   return ret;
}

/****************************************************************************
*
*  Function Name: CHARGERP_validateChipId
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
static INT32 CHARGERP_validateChipId(UINT8 chipID)
{
   if (chipID == AT828_IIS_CHIP_ID)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "AT828 chipID = 0x%x - IIS Found!\n",chipID);
   }
   else if (chipID == AT828_V2_CHIP_ID)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "AT828 chipID = 0x%x - V2 Found!\n",chipID);
   }
   else if (chipID == AT828_V3_CHIP_ID)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "AT828 chipID = 0x%x - V3 Found!\n",chipID);
   }
   else if (chipID == AT828_M45S_CHIP_ID)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "AT828 chipID = 0x%x - M4.5S Found!\n",chipID);
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "AT828 chipID = 0x%x - Unsupported Chip Id.\n",chipID);
      return 0;
   }

   return 1;
}

/****************************************************************************
*
*  Function Name: CHARGERP_readChipId
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/ 
static ERRG_codeE CHARGERP_readChipId( CHARGERP_infoT *paramsP )
{
   UINT32 numTry = 0;
   UINT8 chipID = 0;
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   for(numTry = 0; numTry < CHARGERP_CHIP_ID_READ_MAX_TRY; numTry++)
   {
      OS_LYRG_usleep(10000); //sleep for 10ms
      ret = CHARGERP_readReg(paramsP, CHIP_ID_REGISTER_ADDR, &chipID);

      if (CHARGERP_validateChipId(chipID))
      {
         break;
      }
   }
   
   if (numTry == CHARGERP_CHIP_ID_READ_MAX_TRY)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to read chipId for charger, exit\n");
      ret = (ERRG_codeE)ERR_IO_ERROR;
   }
   
   return ret;
}


static void CHARGERP_isr(UINT64 timestamp, UINT64 count, void *argP)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   CHARGERP_infoT *paramsP = (CHARGERP_infoT*)argP;
   CHARGERP_msgQParamsT msg;

   (void)count;
   msg.timestamp = timestamp;
   ret = OS_LYRG_sendMsg(&paramsP->msgQue, (UINT8*)&msg, sizeof(CHARGERP_msgQParamsT));
   if (ERRG_FAILED(ret))
   {
      ret = CDE_MNGR__ERR_FAIL_TO_SEND_INT_MSG;
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to send msg to thread!\n");
   }
}


static ERRG_codeE CHARGERP_registerIsr( CHARGERP_infoT *paramsP )
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   OS_LYRG_intNumE intNum = 15; //TODO - parameter

   ret = OS_LYRG_intCtrlRegister(intNum, CHARGERP_isr, paramsP);
   if (ERRG_SUCCEEDED(ret))
   {
      ret = OS_LYRG_intCtrlEnable(intNum);
   }

   if (ERRG_SUCCEEDED(ret))
   {
      UINT8 status;
      //one time read
      ret = CHARGERP_readReg(paramsP, INTERRUPT_STATUS_REGISTER_ADDR, &status);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to enable isr\n");
   }

   
   return ret;   
}

static ERRG_codeE CHARGERP_unregisterIsr( )
{
   return OS_LYRG_intCtrlUnregister(15);   
}

static void CHARGERP_handleMsg(CHARGERP_infoT *paramsP, CHARGERP_msgQParamsT *msgQParamsP)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   UINT8      status;
   
   ret = CHARGERP_readReg(paramsP, INTERRUPT_STATUS_REGISTER_ADDR, &status);
   if (ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "isr status = 0x%x, (%llu)\n",status,msgQParamsP->timestamp);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to read isr status\n");
   }   
}


static int CHARGERP_thread(void *argP)
{
   CHARGERP_msgQParamsT    msg;
   INT32                   status   = 0;
   UINT32                  msgSize  = 0;
   CHARGERP_infoT          *paramsP = (CHARGERP_infoT*)argP;

   LOGG_PRINT(LOG_INFO_E, NULL, "thread started\n");
   paramsP->threadIsActive = 1;
   while(paramsP->threadIsActive)
   {
      msgSize  = sizeof(msg);
      status   = OS_LYRG_recvMsg(&paramsP->msgQue, (UINT8*)&msg, &msgSize, CHARGERP_MSGQ_TIMEOUT);
      if((status == SUCCESS_E) && (msgSize == sizeof(msg)))
      {
         CHARGERP_handleMsg(paramsP, &msg);
      }
   }

   LOGG_PRINT(LOG_INFO_E, NULL, "thread exited\n");

   OS_LYRG_exitThread();

   return status;
}


static ERRG_codeE CHARGERP_createThread( CHARGERP_infoT *paramsP )
{
   //Early initializations on GP before service open . Assumed called once 
   ERRG_codeE           retCode = (ERRG_codeE)RET_SUCCESS;
   OS_LYRG_threadParams thrParams;

   memcpy(paramsP->msgQue.name, CHARGERP_MSGQ_NAME, sizeof(CHARGERP_MSGQ_NAME));
   paramsP->msgQue.maxMsgs = CHARGERP_MSGQ_MAX_MSG;
   paramsP->msgQue.msgSize = CHARGERP_MSGQ_SIZE;

   //Create msg queue
   if(OS_LYRG_createMsgQue(&paramsP->msgQue, OS_LYRG_MULTUPLE_EVENTS_NUM_E) != SUCCESS_E)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "msg queue create failed\n");
      return ERR_UNEXPECTED;
   }

   //Invoke thread
   thrParams.func    = CHARGERP_thread;
   thrParams.id      = OS_LYRG_CHARGER_THREAD_ID_E;
   thrParams.event   = NULL;
   thrParams.param   = paramsP;
   paramsP->thrdH    = OS_LYRG_createThread(&thrParams);
   if(!paramsP->thrdH)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "thread create failed\n");
      retCode = HW_MNGR__ERR_CREATE_THREAD_FAIL;
   }
   return retCode;
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
/****************************************************************************
*
*  Function Name: CHARGERG_enterPowerSave
*
*  Description: write to reg 0x11 bit 1
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE CHARGERG_enterPowerSave( UINT8 wake_up_sec )
{
   ERRG_codeE      ret = INU_DEVICE__RET_SUCCESS;
   CHARGERP_infoT *paramsP = &CHARGERP_params;
   UINT8           regVal;

   if (!CHARGERP_params.initialized)
      return INU_DEVICE__ERR_ILLEGAL_STATE;
   
   ret = CHARGERP_writeReg( paramsP, POWER_TIMER_REGISTER_ADDR, wake_up_sec );
   ret = CHARGERP_readReg ( paramsP, POWER_CONTROL_REGISTER_ADDR, &regVal );
   regVal |= 0x2; //todo define
   ret = CHARGERP_writeReg( paramsP, POWER_CONTROL_REGISTER_ADDR, regVal ); 
   return ret;
}


/****************************************************************************
*
*  Function Name: CHARGERG_enterPowerDown
*
*  Description: write to reg 0x11 bit 0
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE CHARGERG_enterPowerDown( UINT8 wake_up_sec )
{
   ERRG_codeE      ret = INU_DEVICE__RET_SUCCESS;
   CHARGERP_infoT *paramsP = &CHARGERP_params;
   UINT8           regVal;

   if (!CHARGERP_params.initialized)
      return INU_DEVICE__ERR_ILLEGAL_STATE;
   
   ret = CHARGERP_writeReg( paramsP, POWER_TIMER_REGISTER_ADDR, wake_up_sec );
   ret = CHARGERP_readReg ( paramsP, POWER_CONTROL_REGISTER_ADDR, &regVal );
   regVal |= 0x1; //todo define
   ret = CHARGERP_writeReg( paramsP, POWER_CONTROL_REGISTER_ADDR, regVal );   
   return ret;
}

/****************************************************************************
*
*  Function Name: CHARGERG_batteryStatus
*
*  Description: read reg 0x12
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE CHARGERG_batteryStatus( UINT8 *levelP )
{
   ERRG_codeE      ret = INU_DEVICE__RET_SUCCESS;
   CHARGERP_infoT *paramsP = &CHARGERP_params;

   if (!CHARGERP_params.initialized)
      return INU_DEVICE__ERR_ILLEGAL_STATE;
   
   if (levelP)
   {
      ret = CHARGERP_readReg( paramsP, SOC_REGISTER_ADDR, levelP );
      if (paramsP->initChargerParams.debugEnable)
      {
         printf("%s: ret %x, value %d\n",__FUNCTION__,ret,*levelP);
      }
   }
   else
   {
      ret = ERR_NULL_PTR;
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: CHARGERG_batteryStatus
*
*  Description: read reg 0x12
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE CHARGERG_chargerStatus( UINT8 *levelP )
{
   ERRG_codeE      ret = INU_DEVICE__RET_SUCCESS;
   CHARGERP_infoT *paramsP = &CHARGERP_params;

   if (!CHARGERP_params.initialized)
      return INU_DEVICE__ERR_ILLEGAL_STATE;
   
   if (levelP)
   {
      ret = CHARGERP_readReg( paramsP, CHARGER_STATUS_REGISTER_ADDR, levelP );
      if (paramsP->initChargerParams.debugEnable)
      {
         printf("%s: ret %x, value %d\n",__FUNCTION__,ret,*levelP);
      }
   }
   else
   {
      ret = ERR_NULL_PTR;
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: CHARGERG_start
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/ 
ERRG_codeE CHARGERG_start( inu_charger__init_charger_t *initChargerParamsP )
{
   ERRG_codeE              ret = INU_DEVICE__RET_SUCCESS;
   UINT8                   batteryLevel;

   memset(&CHARGERP_params,0,sizeof(CHARGERP_params));
   memcpy(&CHARGERP_params.initChargerParams, initChargerParamsP, sizeof(inu_charger__init_charger_t));

   CHARGERP_initCmdsBuffer(&CHARGERP_params);

   //open UART
   ret = CHARGERP_uartModeStart(&CHARGERP_params);

   //try to read chip ID
   if(ERRG_SUCCEEDED(ret))
   {
      ret = CHARGERP_readChipId(&CHARGERP_params);
   }

   //get SW version
   if(ERRG_SUCCEEDED(ret))
   {
      ret = CHARGERP_readVersionId(&CHARGERP_params);
   }

   //create thread for handling the interrupts
   if(ERRG_SUCCEEDED(ret))
   {
      ret = CHARGERP_createThread(&CHARGERP_params);
   }

   //register ISR
   if(ERRG_SUCCEEDED(ret))
   {
      ret = CHARGERP_registerIsr(&CHARGERP_params);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      CHARGERP_params.initialized = TRUE;
      ret = CHARGERG_batteryStatus(&batteryLevel);
      //Read battery status
      if(ERRG_SUCCEEDED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL,"CHARGER init success. Battery level = %d\n",batteryLevel);
      }
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: CHARGERG_stop
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/ 
ERRG_codeE CHARGERG_stop( )
{
   CHARGERP_infoT *paramsP = (CHARGERP_infoT*)&CHARGERP_params;
   CHARGERP_unregisterIsr();
   paramsP->threadIsActive = 0; //exit thread
   CHARGERP_uartModeStop(paramsP);

   memset(paramsP,0,sizeof(CHARGERP_infoT));
   LOGG_PRINT(LOG_INFO_E, NULL,"CHARGERG_stop succesful\n");
   return INU_DEVICE__RET_SUCCESS;
}

