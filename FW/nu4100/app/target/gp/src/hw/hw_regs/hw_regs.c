/****************************************************************************
 *
 *   FileName: hw_regs.c
 *
 *   Author:  Yaron A. & Eyal A.
 *
 *   Date:
 *
 *   Description: system configuration register list DB
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/


#include "hw_regs.h"
#include "pvt_drv.h"
#include "inu_common.h"
#include "inu_imu.h"
#include "os_lyr.h"
#include "gen_sensor_drv.h"
#include "i2c_hl_drv.h"
#include "gme_mngr.h"
#include "gme_drv.h"
#include "hcg_mngr.h"
#include "inu_charger.h"
#include "mipi_csi2_dw.h"
#ifdef __cplusplus
   extern "C" {
#endif


/****************************************************************************
 ***************     L O C A L          D E F N I T I O N S    **************
 ****************************************************************************/
#define REG_TABLE_SIZE     400
#define SEN_TABLE_SIZE     100
#define I2C_TABLE_SIZE     200
#define WAIT_TABLE_SIZE    30

#define COMMAND_TABLE_SIZE (SEN_TABLE_SIZE + REG_TABLE_SIZE + I2C_TABLE_SIZE + WAIT_TABLE_SIZE)
#define DELAY_USEC  1000

#define HW_REGSP_I2C_NUM_OF_RETRIES                   (10)

typedef enum
{
   HW_REGSP_SEN_TYPE_MASTER_E,
   HW_REGSP_SEN_TYPE_SLAVE_E,
   HW_REGSP_SEN_TYPE_BOTH_E,
   HW_REGSP_NUM_OF_SENSOR_TYPE_E
} HW_REGSP_senTypeE;

typedef enum
{
   HW_REGSP_ENTRY_TYPE_SENSOR_E,
   HW_REGSP_ENTRY_TYPE_REG_E,
   HW_REGSP_ENTRY_TYPE_I2C_E,
   HW_REGSP_ENTRY_TYPE_WAIT_E,
   HW_REGSP_ENTRY_TYPE_NUM_OF_ENTRY_TYPE_E
} HW_REGSP_entryTypeE;

typedef enum
{
   HW_REGSP_FILTER_ALL_E,
   HW_REGSP_FILTER_SENSOR_E,
   HW_REGSP_FILTER_REG_E,
   HW_REGSP_FILTER_I2C_E,
   HW_REGSP_FILTER_WAIT_E,
   HW_REGSP_FILTER_NUM_OF_ENTRY_TYPE_E
} HW_REGSP_tableFilterE;

/****************************************************************************
 ***************      L O C A L          F U N C T I O N S     ***************
 ****************************************************************************/
static ERRG_codeE HW_REGSP_activateHwEntry(void *param, HW_REGSP_tableFilterE filter);
static ERRG_codeE HW_REGSP_activateI2cEntry(void *param, HW_REGSP_tableFilterE filter);
static ERRG_codeE HW_REGSP_activateWaitEntry(void *param, HW_REGSP_tableFilterE filter);
static ERRG_codeE HW_REGSP_activateCommandTable( HW_REGSP_tableFilterE filter, inu_device__dbPhaseE phase);
// I2C module
static ERRG_codeE HW_REGSP_openI2cDrv();
static ERRG_codeE HW_REGSP_closeI2cDrv();

/****************************************************************************
 ***************      L O C A L          T Y P E D E F S     ***************
 ****************************************************************************/
typedef ERRG_codeE(*HW_REGSP_entryOperation)(void *, HW_REGSP_tableFilterE);

typedef struct
{
   HW_REGSP_entryOperation entryFunc;
   UINT32                  regVal;
   UINT16                  regAddress;
   UINT8                   senType;
   UINT8                   regWidth;
} HW_REGSP_senEntry;

typedef struct
{
   HW_REGSP_senEntry   table[SEN_TABLE_SIZE];
   UINT32            tableLen;
} HW_REGSP_senTable;

typedef struct
{
   HW_REGSP_entryOperation entryFunc;
   UINT32                  regAddress;
   UINT32                  regVal;
} HW_REGSP_regEntry;

typedef struct
{
   HW_REGSP_regEntry   table[REG_TABLE_SIZE];
   UINT32            tableLen;
} HW_REGSP_regTable;

typedef struct
{
   HW_REGSP_entryOperation entryFunc;
   inu_device__i2cNumE       i2cNum;
   UINT8                   tarAddress;
   UINT8                   regAddressAccessSize;
   UINT32                  regAddress;
   UINT32                  regWidth;
   UINT32                  i2cData;
} HW_REGSP_i2cEntry;

typedef struct
{
   HW_REGSP_i2cEntry   table[I2C_TABLE_SIZE];
   UINT32            tableLen;
} HW_REGSP_i2cTable;

typedef struct
{
   HW_REGSP_entryOperation entryFunc;
   UINT32                  usecWait;
} HW_REGSP_waitEntry;

typedef struct
{
   HW_REGSP_waitEntry   table[WAIT_TABLE_SIZE];
   UINT32             tableLen;
} HW_REGSP_waitTable;

typedef struct
{
   void* command;
   UINT8 phase;
} HW_REGSP_command_Entry;

typedef struct
{
   HW_REGSP_command_Entry table[COMMAND_TABLE_SIZE];
   UINT32               tableLen;
} HW_REGSP_commandTable;

/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static HW_REGSP_senTable            senTable;
static HW_REGSP_regTable            regTable;
static HW_REGSP_i2cTable            i2cTable;
static HW_REGSP_waitTable           waitTable;
static HW_REGSP_commandTable        commandTable;
static HW_REGSG_customPhaseCbT      customPhaseCb = NULL;
static HW_REGSG_clearRegsPhaseCbT   customClearRegsPhaseCb = NULL;

/****************************************************************************
 ***************       L O C A L       F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: HW_REGSP_activateCommandTable
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
static ERRG_codeE HW_REGSP_activateCommandTable(HW_REGSP_tableFilterE   filter,
                                                inu_device__dbPhaseE      phase)
{
   ERRG_codeE  retVal   = HW_REGS__RET_SUCCESS;
   UINT32      i        = 0;

   for (i=0;i<commandTable.tableLen;++i)
   {
      if (commandTable.table[i].phase == (UINT8)(phase))
      {
         retVal = (*((HW_REGSP_entryOperation *)(commandTable.table[i].command)))(commandTable.table[i].command, filter );
      }
   }
   return retVal;
}

/****************************************************************************
*
*  Function Name: HW_REGSP_activateHwEntry
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
static ERRG_codeE HW_REGSP_activateHwEntry(void *param, HW_REGSP_tableFilterE filter)
{
   ERRG_codeE        retVal   = HW_REGS__RET_SUCCESS;
   HW_REGSP_regEntry *entryP  = NULL;

   if(param)
   {
      if (filter == HW_REGSP_FILTER_ALL_E || filter == HW_REGSP_FILTER_REG_E)
      {
         OS_LYRG_usleep(DELAY_USEC);
         entryP   = (HW_REGSP_regEntry *)param;
         retVal   = HW_REGSG_writeSocReg(entryP->regAddress, entryP->regVal);
      }
   }
   else
   {
      retVal = HW_REGS__ERR_NULL_PTR;
   }
   if (ERRG_FAILED(retVal))
   {
      LOGG_PRINT(LOG_ERROR_E, retVal, "fail: Activate REG periph config command\n");
   }
   return retVal;
}

/****************************************************************************
*
*  Function Name: HW_REGSP_activateI2cEntry
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
static ERRG_codeE HW_REGSP_activateI2cEntry(void *param, HW_REGSP_tableFilterE filter)
{
   ERRG_codeE                       retVal   = HW_REGS__RET_SUCCESS;
   HW_REGSP_i2cEntry                *entryP  = NULL;
   IO_HANDLE                        i2cHandle;
   I2C_HL_DRVG_readWriteParametersT i2cConfig;
   UINT32                           regWidth;
   UINT32                           i2cData;
   inu_device__i2cNumE                i2cNum;
   UINT8                            numOfRetries;

   if(param)
   {
      entryP = (HW_REGSP_i2cEntry *)param;
      if (filter == HW_REGSP_FILTER_ALL_E || filter == HW_REGSP_FILTER_I2C_E)
      {
         i2cNum                         = entryP->i2cNum;
         i2cConfig.regAddress           = entryP->regAddress;
         i2cConfig.tarAddress           = entryP->tarAddress;
         i2cConfig.regAddressAccessSize = entryP->regAddressAccessSize;
         regWidth                       = entryP->regWidth;
         i2cData                        = entryP->i2cData;

         i2cHandle = IO_PALG_getHandle(IO_I2C_0_E + i2cNum);

         if(i2cHandle)
         {
            numOfRetries = 0;
            do
            {
               retVal = IO_PALG_write(i2cHandle, (UINT8*)&i2cData, regWidth, &i2cConfig);
            }
            while((ERRG_FAILED(retVal)) && (numOfRetries++ < HW_REGSP_I2C_NUM_OF_RETRIES));
            if( ERRG_SUCCEEDED(retVal) )
            {
               LOGG_PRINT(LOG_INFO_E, NULL, "I2C#: %d device Addr: 0x%x Addr Len: %d Addr: 0x%x regWidth: 0x%x Data: 0x%x\n", i2cNum, i2cConfig.tarAddress, i2cConfig.regAddressAccessSize, i2cConfig.regAddress, regWidth, i2cData);
            }
            else
            {
               LOGG_PRINT(LOG_ERROR_E, retVal, "Failed I2C write command: I2C#: %d, device Addr: 0x%x, Addr Len: %d, Addr: 0x%x regWidth: 0x%x, Data: 0x%x\n", i2cNum, i2cConfig.tarAddress, i2cConfig.regAddressAccessSize, i2cConfig.regAddress, regWidth, i2cData);
            }
         }
         else
         {
            retVal = HW_REGS__ERR_INVALID_ARGS;
            LOGG_PRINT(LOG_ERROR_E, retVal, "Failed I2C write command (NULL handle): I2C#: %d, device Addr: 0x%x, Addr Len: %d, Addr: 0x%x regWidth: 0x%x, Data: 0x%x\n", i2cNum, i2cConfig.tarAddress, i2cConfig.regAddressAccessSize, i2cConfig.regAddress, regWidth, i2cData);
         }
      }
   }
   else
   {
      retVal = HW_REGS__ERR_NULL_PTR;
      LOGG_PRINT(LOG_ERROR_E, retVal, "error null parameters value\n");
   }
   if (ERRG_FAILED(retVal))
   {
      LOGG_PRINT(LOG_ERROR_E, retVal, "fail: Activate I2C periph config command\n");
   }

   return retVal;
}

/****************************************************************************
*
*  Function Name: HW_REGSP_activateWaitEntry
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
static ERRG_codeE HW_REGSP_activateWaitEntry(void *param, HW_REGSP_tableFilterE filter)
{
   ERRG_codeE        retVal   = HW_REGS__RET_SUCCESS;
   HW_REGSP_waitEntry  *entryP  = NULL;

    if (param)
   {
      if (filter == HW_REGSP_FILTER_ALL_E || filter == HW_REGSP_FILTER_WAIT_E)
      {
         entryP = (HW_REGSP_waitEntry *)param;
         OS_LYRG_usleep(entryP->usecWait);
         LOGG_PRINT(LOG_INFO_E, NULL, "wait = %d microSec \n", entryP->usecWait);
      }
   }
   else
   {
      retVal = HW_REGS__ERR_NULL_PTR;
   }
   if (ERRG_FAILED(retVal))
   {
      LOGG_PRINT(LOG_ERROR_E, retVal, "fail: Activate WAIT periph config command\n");
   }
   return retVal;
}

// I2C functions
/****************************************************************************
*
*  Function Name: HW_REGSP_openI2cDrv
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: pheripherals control
*
****************************************************************************/
static ERRG_codeE HW_REGSP_openI2cDrv()
{
   ERRG_codeE              retCode = HW_REGS__RET_SUCCESS;
   I2C_HL_DRVG_openParamsT i2cDrvOpenParams;
   IO_HANDLE               ioHandle;

   i2cDrvOpenParams.i2cParameters.addressMode = I2C_HL_DRVG_MASTERMODE_7_BIT_ADDRESSING;
   i2cDrvOpenParams.i2cParameters.i2cSpeed = I2C_HL_DRVG_SPEED_STANDARD_E;
   i2cDrvOpenParams.i2cParameters.SlaveAddress = 0x90;

// I2C-0
   MEM_MAPG_getVirtAddr(MEM_MAPG_REG_I2C_0_E, (&i2cDrvOpenParams.i2cRegistersVirtualAddr));

   retCode = IO_PALG_open(&ioHandle, IO_I2C_0_E, &i2cDrvOpenParams);
   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "open I2C-0 driver fail.\n");
   }

// I2C-1
   MEM_MAPG_getVirtAddr(MEM_MAPG_REG_I2C_1_E, (&i2cDrvOpenParams.i2cRegistersVirtualAddr));

   retCode = IO_PALG_open(&ioHandle, IO_I2C_1_E, &i2cDrvOpenParams);
   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "open I2C-1 driver fail.\n");
   }

// I2C-2
   MEM_MAPG_getVirtAddr(MEM_MAPG_REG_I2C_2_E, (&i2cDrvOpenParams.i2cRegistersVirtualAddr));

   retCode = IO_PALG_open(&ioHandle, IO_I2C_2_E, &i2cDrvOpenParams);
   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "open I2C-2 driver fail.\n");
   }

// I2C-3
   MEM_MAPG_getVirtAddr(MEM_MAPG_REG_I2C_3_E, (&i2cDrvOpenParams.i2cRegistersVirtualAddr));

   retCode = IO_PALG_open(&ioHandle, IO_I2C_3_E, &i2cDrvOpenParams);
   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "open I2C-3 driver fail.\n");
   }

// I2C-4
   MEM_MAPG_getVirtAddr(MEM_MAPG_REG_I2C_4_E, (&i2cDrvOpenParams.i2cRegistersVirtualAddr));

   retCode = IO_PALG_open(&ioHandle, IO_I2C_4_E, &i2cDrvOpenParams);
   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "open I2C-4 driver fail.\n");
   }

// I2C-5
   MEM_MAPG_getVirtAddr(MEM_MAPG_REG_I2C_5_E, (&i2cDrvOpenParams.i2cRegistersVirtualAddr));

   retCode = IO_PALG_open(&ioHandle, IO_I2C_5_E, &i2cDrvOpenParams);
   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "open I2C-5 driver fail.\n");
   }

   return(retCode);
}

/****************************************************************************
*
*  Function Name: HW_REGSP_closeI2cDrv
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: pheripherals control
*
****************************************************************************/
static ERRG_codeE HW_REGSP_closeI2cDrv()
{
   ERRG_codeE  retCode  = HW_REGS__RET_SUCCESS;

// I2C-0
   retCode = IO_PALG_close(IO_PALG_getHandle(IO_I2C_0_E));
   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "close I2C 0 fail.\n");
   }

// I2C-1
   retCode = IO_PALG_close(IO_PALG_getHandle(IO_I2C_1_E));
   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "close I2C 1 fail.\n");
   }

// I2C-2
   retCode = IO_PALG_close(IO_PALG_getHandle(IO_I2C_2_E));
   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "close I2C 2 fail.\n");
   }

// I2C-3
   retCode = IO_PALG_close(IO_PALG_getHandle(IO_I2C_3_E));
   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "close I2C 3 fail.\n");
   }

// I2C-4
	retCode = IO_PALG_close(IO_PALG_getHandle(IO_I2C_4_E));
	if(ERRG_FAILED(retCode))
	{
		LOGG_PRINT(LOG_ERROR_E, retCode, "close I2C 4 fail.\n");
	}

// I2C-5
	retCode = IO_PALG_close(IO_PALG_getHandle(IO_I2C_5_E));
	if(ERRG_FAILED(retCode))
	{
		LOGG_PRINT(LOG_ERROR_E, retCode, "close I2C 5 fail.\n");
	}

   return(retCode);
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: HW_REGSG_init
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
ERRG_codeE HW_REGSG_init()
{
   ERRG_codeE  retCode = HW_REGS__RET_SUCCESS;

   senTable.tableLen       = 0;
   regTable.tableLen       = 0;
   i2cTable.tableLen       = 0;
   waitTable.tableLen      = 0;
   commandTable.tableLen   = 0;

   retCode = HW_REGSP_openI2cDrv();
   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "open I2C drivers fail.\n");
   }

   return retCode;
}

/****************************************************************************
*
*  Function Name: HW_REGSG_close
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
ERRG_codeE HW_REGSG_close()
{
   ERRG_codeE  retCode = HW_REGS__RET_SUCCESS;
   retCode = HW_REGSP_closeI2cDrv();
   return(retCode);
}

/****************************************************************************
*
*  Function Name: HW_REGSG_insertParam2Table
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
ERRG_codeE HW_REGSG_insertParam2Table(inu_device__writeRegT *writeRegIoctlParamsP)
{
   ERRG_codeE           retVal   = HW_REGS__RET_SUCCESS;
   inu_device__dbPhaseE   phase    = writeRegIoctlParamsP->phase;
   inu_device__regTypeE   regType  = writeRegIoctlParamsP->regType;

   switch(regType)
   {

      case INU_DEVICE__REG_SOC_E:
      {
         inu_device__socRegT *socRegParamsP = &writeRegIoctlParamsP->socReg;

         if (regTable.tableLen < REG_TABLE_SIZE)
         {
            regTable.table[regTable.tableLen].regAddress       = socRegParamsP->addr;
            regTable.table[regTable.tableLen].regVal           = socRegParamsP->val;
            regTable.table[regTable.tableLen].entryFunc        = HW_REGSP_activateHwEntry;
            commandTable.table[commandTable.tableLen].command  = ((void *)&(regTable.table[regTable.tableLen]));
            commandTable.table[commandTable.tableLen].phase    = (UINT8)phase;
            ++regTable.tableLen;
            ++commandTable.tableLen;
         }
         else
         {
            retVal = HW_REGS__TABLE_FULL;
            LOGG_PRINT(LOG_ERROR_E, retVal, "fail: REG table is full. reg table len is %d\n", regTable.tableLen);
         }
      }
      break;

      case INU_DEVICE__REG_I2C_E:
      {
         inu_device__i2cRegT *i2cRegParamsP = &writeRegIoctlParamsP->i2cReg;

         if (i2cTable.tableLen < I2C_TABLE_SIZE)
         {
            i2cTable.table[i2cTable.tableLen].i2cNum                 = i2cRegParamsP->i2cNum;
            i2cTable.table[i2cTable.tableLen].tarAddress             = i2cRegParamsP->tarAddress;
            i2cTable.table[i2cTable.tableLen].regAddressAccessSize   = i2cRegParamsP->accessSize;
            i2cTable.table[i2cTable.tableLen].regAddress             = i2cRegParamsP->regAddress;
            i2cTable.table[i2cTable.tableLen].regWidth               = i2cRegParamsP->regWidth;
            i2cTable.table[i2cTable.tableLen].i2cData                = i2cRegParamsP->i2cData;
            i2cTable.table[i2cTable.tableLen].entryFunc              = HW_REGSP_activateI2cEntry;
            commandTable.table[commandTable.tableLen].command        = ((void *)&(i2cTable.table[i2cTable.tableLen]));
            commandTable.table[commandTable.tableLen].phase          = (UINT8)phase;
            ++i2cTable.tableLen;
            ++commandTable.tableLen;
         }
         else
         {
            retVal = HW_REGS__TABLE_FULL;
            LOGG_PRINT(LOG_ERROR_E, retVal, "fail: I2C table is full. i2s table len is %d\n", i2cTable.tableLen);
         }
      }
      break;

      case INU_DEVICE__REG_WAIT_E:
      {
         inu_device__waitRegT *waitRegParamsP = &writeRegIoctlParamsP->waitReg;

         if (waitTable.tableLen < WAIT_TABLE_SIZE)
         {
            waitTable.table[waitTable.tableLen].usecWait       = waitRegParamsP->usecWait;
            waitTable.table[waitTable.tableLen].entryFunc      = HW_REGSP_activateWaitEntry;
            commandTable.table[commandTable.tableLen].command  = ((void *)&(waitTable.table[waitTable.tableLen]));
            commandTable.table[commandTable.tableLen].phase    = (UINT8)phase;
            ++waitTable.tableLen;
            ++commandTable.tableLen;
         }
         else
         {
            retVal = HW_REGS__TABLE_FULL;
            LOGG_PRINT(LOG_ERROR_E, retVal, "fail: WAIT table is full. wait table len is %d\n", waitTable.tableLen);
         }
      }
      break;

      default:
      {
         retVal = HW_REGS__INSERT_WRONG_PARAM_TO_TABLE;
         LOGG_PRINT(LOG_ERROR_E, retVal, "trying to write wrong parameter to HW data base table (%d)\n", regType);
      }
      break;

   }

   return retVal;
}

/****************************************************************************
*
*  Function Name: HW_REGSG_ClearSenTable
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
ERRG_codeE HW_REGSG_ClearSenTable()
{
   UINT32 i=0;
   UINT32 currIndex=0;

   memset ((void*)(&senTable),0,sizeof(HW_REGSP_senTable));
   for (i=0;i<COMMAND_TABLE_SIZE;++i)
   {
      if (*(HW_REGSP_entryOperation *)(commandTable.table[i].command))
      {
         commandTable.table[currIndex] = commandTable.table[i];
         ++currIndex;
      }
   }
   commandTable.tableLen = (currIndex);
   return HW_REGS__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: HW_REGSG_ClearHwTable
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
ERRG_codeE HW_REGSG_ClearHwTable()
{
   UINT32 i=0;
   UINT32 currIndex=0;

   memset ((void*)(&regTable),0,sizeof(HW_REGSP_regTable));
   for (i=0;i<COMMAND_TABLE_SIZE;++i)
   {
      if (*(HW_REGSP_entryOperation *)(commandTable.table[i].command))
      {
         commandTable.table[currIndex] = commandTable.table[i];
         ++currIndex;
      }
   }
   commandTable.tableLen = (currIndex);
   return HW_REGS__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: HW_REGSG_clearI2cTable
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
ERRG_codeE HW_REGSG_clearI2cTable()
{
   UINT32 i=0;
   UINT32 currIndex=0;

   memset ((void*)(&i2cTable),0,sizeof(HW_REGSP_i2cTable));
   for (i=0;i<COMMAND_TABLE_SIZE;++i)
   {
      if (*(HW_REGSP_entryOperation *)(commandTable.table[i].command))
      {
         commandTable.table[currIndex] = commandTable.table[i];
         ++currIndex;
      }
   }
   commandTable.tableLen = (currIndex);
   return HW_REGS__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: HW_REGSG_clearWaitTable
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
ERRG_codeE HW_REGSG_clearWaitTable()
{
   UINT32 i=0;
   UINT32 currIndex=0;

   memset ((void*)(&waitTable),0,sizeof(HW_REGSP_waitTable));
   for (i=0;i<COMMAND_TABLE_SIZE;++i)
   {
      if (*(HW_REGSP_entryOperation *)(commandTable.table[i].command))
      {
         commandTable.table[currIndex] = commandTable.table[i];
         ++currIndex;
      }
   }
   commandTable.tableLen = (currIndex);
   return HW_REGS__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: HW_REGSG_clearCommandTable
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
ERRG_codeE HW_REGSG_clearCommandTable()
{
   memset ((void*)(&senTable),      0, sizeof(HW_REGSP_senTable));
   memset ((void*)(&regTable),      0, sizeof(HW_REGSP_regTable));
   memset ((void*)(&i2cTable),      0, sizeof(HW_REGSP_i2cTable));
   memset ((void*)(&waitTable),     0, sizeof(HW_REGSP_waitTable));
   memset ((void*)(&commandTable),  0, sizeof(HW_REGSP_commandTable));
   return HW_REGS__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: HW_REGSG_activateSenTable
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
ERRG_codeE HW_REGSG_activateSenTable(inu_device__dbPhaseE phase)
{
   ERRG_codeE retVal = HW_REGS__RET_SUCCESS;
   retVal = HW_REGSP_activateCommandTable(HW_REGSP_FILTER_SENSOR_E, phase);
   return retVal;
}

/****************************************************************************
*
*  Function Name: HW_REGSG_activateHwTable
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
ERRG_codeE HW_REGSG_activateHwTable(inu_device__dbPhaseE phase)
{
   ERRG_codeE              retVal = HW_REGS__RET_SUCCESS;
   retVal = HW_REGSP_activateCommandTable(HW_REGSP_FILTER_REG_E, phase);
   return retVal;
}

/****************************************************************************
*
*  Function Name: HW_REGSG_activateI2cTable
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
ERRG_codeE HW_REGSG_activateI2cTable(inu_device__dbPhaseE phase)
{
   ERRG_codeE retVal = HW_REGS__RET_SUCCESS;
   retVal = HW_REGSP_activateCommandTable(HW_REGSP_FILTER_I2C_E, phase);
   return retVal;
}

/****************************************************************************
*
*  Function Name: HW_REGSG_activateCommandTable
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
ERRG_codeE HW_REGSG_activateCommandTable(inu_device__dbPhaseE phase)
{
   ERRG_codeE retVal = HW_REGS__RET_SUCCESS;
   retVal = HW_REGSP_activateCommandTable(HW_REGSP_FILTER_ALL_E, phase);
   return retVal;
}

/****************************************************************************
*
*  Function Name: HW_REGSG_writeSocReg
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE HW_REGSG_writeSocReg(UINT32 physicalAddr, UINT32 val)
{
   ERRG_codeE  retCode = HW_REGS__RET_SUCCESS;
   UINT32      virtAddr;
   UINT32      *regP;

   MEM_MAPG_convertPhysicalToVirtual3(physicalAddr, (&virtAddr));
   regP = (UINT32 *)(virtAddr);
   if(regP != NULL)
   {
      HCG_MNGRG_voteUnits(HCG_MNGRG_HW_ALL_UNITS);
      *regP = val;
      HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_ALL_UNITS);
      LOGG_PRINT(LOG_INFO_E, NULL, "write register 0x%X, val=0x%X\n", (UINT32)physicalAddr, val);
   }
   else
   {
      retCode = HW_REGS__ERR_WRITE_SOC_REG_FAIL;
   }
   return(retCode);
}

/****************************************************************************
*
*  Function Name: HW_REGSG_readSocReg
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE HW_REGSG_readSocReg(UINT32 physicalAddr, UINT32 *regValP)
{
   ERRG_codeE  retCode = HW_REGS__RET_SUCCESS;
   UINT32      virtAddr;
   UINT32      *regP;

   MEM_MAPG_convertPhysicalToVirtual3(physicalAddr, (&virtAddr));
   regP = (UINT32 *)(virtAddr);
   if(regP != NULL)
   {
      HCG_MNGRG_voteUnits(HCG_MNGRG_HW_ALL_UNITS);
      *regValP = *regP;
      HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_ALL_UNITS);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "read register fail. invalid address (0x%X)\n", physicalAddr);
      retCode = HW_REGS__ERR_READ_SOC_REG_FAIL;
   }

   return(retCode);
}
/****************************************************************************
*
*  Function Name: HW_REGSG_readSocReg
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE HW_REGSG_readSocRegVolatile(UINT32 physicalAddr,volatile UINT32 *regValP)
{
   ERRG_codeE  retCode = HW_REGS__RET_SUCCESS;
   UINT32      virtAddr;
   volatile UINT32      *regP;

   MEM_MAPG_convertPhysicalToVirtual3(physicalAddr, (&virtAddr));
   regP = (UINT32 *)(virtAddr);
   if(regP != NULL)
   {
      HCG_MNGRG_voteUnits(HCG_MNGRG_HW_ALL_UNITS);
      *regValP = *regP;
      HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_ALL_UNITS);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "read register fail. invalid address (0x%X)\n", physicalAddr);
      retCode = HW_REGS__ERR_READ_SOC_REG_FAIL;
   }

   return(retCode);
}


/****************************************************************************
*
*  Function Name: HW_REGSG_writeI2cReg
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE  HW_REGSG_writeI2cReg(inu_device__i2cNumE i2cNum, UINT32 regAddr, UINT8 tarAddress, UINT8 accessSize, UINT32 regWidth, UINT32 val)
{
   ERRG_codeE                       ret = HW_REGS__RET_SUCCESS;
   I2C_HL_DRVG_readWriteParametersT i2cConfig;

   i2cConfig.regAddress           = regAddr;
   i2cConfig.tarAddress           = tarAddress;
   i2cConfig.regAddressAccessSize = accessSize;

   ret = IO_PALG_write(IO_PALG_getHandle(IO_I2C_0_E + i2cNum), (UINT8*)&val, regWidth, &i2cConfig);
   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "write I2C register ioctl params: address=0x%X (base=0x%X), val=0x%X\n", regAddr, tarAddress, val);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "Failed I2C write command: I2C#: %d device Addr: 0x%x Addr Len: %d Addr: 0x%x Data: 0x%x\n", i2cNum, i2cConfig.tarAddress, i2cConfig.regAddressAccessSize, i2cConfig.regAddress, val);
   }

   return(ret);
}

/****************************************************************************
*
*  Function Name: HW_REGSG_readI2cReg
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
UINT32 HW_REGSG_readI2cReg(inu_device__i2cNumE i2cNum, UINT32 regAddr, UINT8 tarAddress, UINT8 accessSize, UINT32 regWidth)
{
   ERRG_codeE                       ret = HW_REGS__RET_SUCCESS;
   I2C_HL_DRVG_readWriteParametersT i2cConfig;
   UINT32                           i2cData;

   i2cConfig.regAddress           = regAddr;
   i2cConfig.tarAddress           = tarAddress;
   i2cConfig.regAddressAccessSize = accessSize;

   ret = IO_PALG_read(IO_PALG_getHandle(IO_I2C_0_E + i2cNum), (UINT8*)&i2cData, regWidth, NULL, &i2cConfig);
   if(ERRG_SUCCEEDED(ret))
   {
      i2cData = i2cData & ((1 << BITS_IN_BYTE * regWidth) -1);
      LOGG_PRINT(LOG_DEBUG_E, NULL, "read I2C register ioctl params: address=0x%X (base=0x%X), val=0x%X\n", regAddr, tarAddress, i2cData);
   }
   else
   {
      i2cData = 0;
      LOGG_PRINT(LOG_ERROR_E, ret, "Error Reading from I2C: 0x%x  \n", i2cNum);
      LOGG_PRINT(LOG_ERROR_E, ret, "Failed I2C read command: I2C#: %d device Addr: 0x%x Addr Len: %d Addr: 0x%x Data: 0x%x\n", i2cNum, i2cConfig.tarAddress, i2cConfig.regAddressAccessSize, i2cConfig.regAddress, i2cData);
   }
   return(i2cData);
}

/****************************************************************************
*
*  Function Name: HW_REGSG_writeAttReg
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE HW_REGSG_writeAttReg(UINT8 attAddr, UINT8 val)
{
   ERRG_codeE  retCode = HW_REGS__RET_SUCCESS;

   retCode = CHARGERG_writeReg(attAddr, val);
   return(retCode);
}

/****************************************************************************
*
*  Function Name: HW_REGSG_readAttReg
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE HW_REGSG_readAttReg(UINT8 attAddr, UINT8 *regValP)
{
   ERRG_codeE retCode = HW_REGS__RET_SUCCESS;

   retCode = CHARGERG_readReg(attAddr, regValP);
   return(retCode);
}

/****************************************************************************
*
*  Function Name: HW_REGSG_readDphyReg
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE HW_REGSG_readDphyReg(UINT32 mipiInst, UINT32 addr, UINT32* regValP)
{
   ERRG_codeE ret = HW_REGS__RET_SUCCESS;
   *regValP = mipi_csi2_dw_test_code_read(mipiInst, addr);
   return ret;
}

/****************************************************************************
*
*  Function Name: HW_REGSG_readReg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE HW_REGSG_readReg(inu_device__readRegT *readRegIoctlParamsP)
{
   //Read register on GP
   ERRG_codeE           ret      = HW_REGS__RET_SUCCESS;
   inu_device__regTypeE regType  = readRegIoctlParamsP->regType;

   switch(regType)
   {
      case INU_DEVICE__REG_SOC_E:
      {
         inu_device__socRegT *socRegParamsP = &readRegIoctlParamsP->socReg;
         ret = HW_REGSG_readSocReg(socRegParamsP->addr, &socRegParamsP->val);
         LOGG_PRINT(LOG_INFO_E, NULL, "read soc register: address=0x%X, val=0x%X.\n", socRegParamsP->addr, socRegParamsP->val);
      }
      break;

      case INU_DEVICE__REG_I2C_E:
      {
         inu_device__i2cRegT *rdI2cRegIoctlParamsP = &readRegIoctlParamsP->i2cReg;
         rdI2cRegIoctlParamsP->i2cData = HW_REGSG_readI2cReg(rdI2cRegIoctlParamsP->i2cNum, rdI2cRegIoctlParamsP->regAddress, rdI2cRegIoctlParamsP->tarAddress, rdI2cRegIoctlParamsP->accessSize, rdI2cRegIoctlParamsP->regWidth);
      }
      break;

      case INU_DEVICE__REG_ATT_E:
      {
         INU_DEFSG_moduleTypeE  modelType   = GME_MNGRG_getModelType();
         INU_DEFSG_baseVersionE baseVersion = GME_MNGRG_getBaseVersion();

         if ((modelType == INU_DEFSG_BOOT51_E) || (modelType == INU_DEFSG_BOOT501_E) || (baseVersion >= INU_DEFSG_BASE_VERSION_2_E))
         {
            inu_device__attRegT *attRegParamsP = &readRegIoctlParamsP->attReg;
            ret = HW_REGSG_readAttReg(attRegParamsP->addr, &attRegParamsP->val);
            LOGG_PRINT(LOG_INFO_E, NULL, "read Attiny register: address=0x%X, val=0x%X.\n", attRegParamsP->addr, attRegParamsP->val);
         }
         else
         {
            ret = HW_REGS__ERR_NOT_SUPPORTED;
            LOGG_PRINT(LOG_ERROR_E, NULL, "Attiny is not supported for Model Type [%d]\n", (UINT32)modelType);
         }
      }
      break;
      case INU_DEVICE__REG_DPHY_E:
      {
          inu_device__dphyRegT* dphyRegParamsP = &readRegIoctlParamsP->dphyReg;
          ret = HW_REGSG_readDphyReg(dphyRegParamsP->dphyNum, dphyRegParamsP->addr, &dphyRegParamsP->val);
          LOGG_PRINT(LOG_INFO_E, NULL, "read DPHY%d register: address=0x%X, val=0x%X.\n", dphyRegParamsP->dphyNum, dphyRegParamsP->addr, dphyRegParamsP->val);
      }
      break;
      case INU_DEVICE__REG_TEMP_E:
      {
          inu_device__tempRegT* tempRegParamsP = &readRegIoctlParamsP->tempReg;
          inu_imu__temperatureParams    imuTempParams;
          static int init_flags = 0;

          switch(tempRegParamsP->addr)
          {
          case 1:
              if(!init_flags)
              {
                  if (ERRG_FAILED(PVT_DRVG_init()))
                      return INU_TEMPERATURE__ERR_UNEXPECTED;
                  init_flags = 1;
              }

              PVT_DRVG_readTemperatureData(&tempRegParamsP->valf);
              LOGG_PRINT(LOG_DEBUG_E, NULL, "Get chipTemp  val=%f.\n", tempRegParamsP->valf);
              break;
          case 2:

              inu_imu__gpGetTemperatoreIoctl(NULL,&imuTempParams);
              tempRegParamsP->valf   = imuTempParams.temperature;
              LOGG_PRINT(LOG_DEBUG_E, NULL, "Get imuTemp  val=%f.\n", tempRegParamsP->valf);
              break;
          default:
              break;
          }
      }
      break;
      default:
      {
         ret = SYSTEM__ERR_READ_SENSOR_REG_FAIL_WRONG_REG_TYPE;
         LOGG_PRINT(LOG_ERROR_E, NULL, "Fail read from sensor, wrong register type: %d\n", regType);
      }
      break;

   }

   return ret;
}


/****************************************************************************
*
*  Function Name: HW_REGSG_writeReg
*
*  Description: INU_DEFSG_DB_PHASE_0_E: write register, INU_DEFSG_DB_PHASE_1_E: store register into table, INU_DEFSG_DB_PHASE_2_E: store & write register into table,
*  INU_DEFSG_DB_PHASE_A_E, INU_DEFSG_DB_PHASE_B_E: alternate mode A/B.
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE HW_REGSG_writeReg(inu_device__writeRegT *writeRegIoctlParamsP)
{
   ERRG_codeE           ret      = SYSTEM__RET_SUCCESS;
   inu_device__dbPhaseE phase    = writeRegIoctlParamsP->phase;
   inu_device__regTypeE regType  = writeRegIoctlParamsP->regType;

   if(phase == INU_DEVICE__DB_PHASE_1_E || phase == INU_DEVICE__DB_PHASE_2_E)
   {
      inu_device__socRegT    *socRegParamsP = &writeRegIoctlParamsP->socReg;

      switch(socRegParamsP->addr)
      {
         default:
         {
            ret = HW_REGSG_insertParam2Table(writeRegIoctlParamsP);
            break;
         }
      }
   }

   if(phase == INU_DEVICE__DB_PHASE_0_E || phase == INU_DEVICE__DB_PHASE_2_E)
   {
      switch(regType)
      {
         case INU_DEVICE__REG_SOC_E:
         {
            inu_device__socRegT *socRegParamsP = &writeRegIoctlParamsP->socReg;

            ret = HW_REGSG_writeSocReg(socRegParamsP->addr, socRegParamsP->val);
            if(ERRG_FAILED(ret))
            {
               LOGG_PRINT(LOG_ERROR_E, ret, "write register ioctl fail. invalid address (0x%X)\n", socRegParamsP->addr);
            }
         }
         break;

         case INU_DEVICE__REG_I2C_E:
         {
            inu_device__i2cRegT *i2cRegIoctlParamsP = &writeRegIoctlParamsP->i2cReg;
            int i;
            for (i = 0; i < 10; i++)
            {
               ret = HW_REGSG_writeI2cReg(i2cRegIoctlParamsP->i2cNum, i2cRegIoctlParamsP->regAddress, i2cRegIoctlParamsP->tarAddress, i2cRegIoctlParamsP->accessSize, i2cRegIoctlParamsP->regWidth, i2cRegIoctlParamsP->i2cData);
               if(ERRG_SUCCEEDED(ret))
               {
                  break;
               }
            }
         }
         break;

         case INU_DEVICE__REG_ATT_E:
         {
            INU_DEFSG_moduleTypeE  modelType   = GME_MNGRG_getModelType();
            INU_DEFSG_baseVersionE baseVersion = GME_MNGRG_getBaseVersion();

            if ((modelType == INU_DEFSG_BOOT51_E) || (modelType == INU_DEFSG_BOOT501_E) || (baseVersion >= INU_DEFSG_BASE_VERSION_2_E ))
            {
               inu_device__attRegT *attRegParamsP = &writeRegIoctlParamsP->attReg;

               ret = HW_REGSG_writeAttReg(attRegParamsP->addr, attRegParamsP->val);
               if(ERRG_FAILED(ret))
               {
                  LOGG_PRINT(LOG_ERROR_E, ret, "write register ioctl fail. invalid address (0x%X)\n", attRegParamsP->addr);
               }
            }
            else
            {
               ret = HW_REGS__ERR_NOT_SUPPORTED;
               LOGG_PRINT(LOG_ERROR_E, NULL, "Attiny is not supported for Model Type [%d]\n", (UINT32)modelType);
            }
         }
         break;

         case INU_DEVICE__REG_WAIT_E:
         {
            OS_LYRG_usleep(writeRegIoctlParamsP->waitReg.usecWait);
            LOGG_PRINT(LOG_INFO_E, NULL, "wait = %d microSec \n", writeRegIoctlParamsP->waitReg.usecWait);
         }
         break;

         default:
         {
            ret = SYSTEM__ERR_WRITE_SENSOR_REG_FAIL_WRONG_REG_TYPE;
            LOGG_PRINT(LOG_ERROR_E, NULL, "Fail write to sensor, wrong register type: %d\n", regType);
         }
         break;
      }
   }

   // handle phase A/B
   if(phase >= INU_DEVICE__DB_PHASE_A_E && phase <= INU_DEVICE__DB_PHASE_F_E)
   {
      if (customPhaseCb)
         (*customPhaseCb)(writeRegIoctlParamsP);
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: HW_REGSG_customPhaseCb
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: PhaseCb
*
****************************************************************************/
void HW_REGSG_customPhaseCb(HW_REGSG_customPhaseCbT PhaseCb, HW_REGSG_clearRegsPhaseCbT clearRegsPhaseCb)
{
   customPhaseCb              = PhaseCb;
   customClearRegsPhaseCb     = clearRegsPhaseCb;
}

/****************************************************************************
*
*  Function Name: HW_REGSG_dbControl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE HW_REGSG_dbControl(INUG_ioctlDbControlT *dbControlParamsP)
{
   ERRG_codeE retCode  = SYSTEM__RET_SUCCESS;

   if(dbControlParamsP->dbControl == INU_DEFSG_DB_CONTROL_ACTIVATE_E)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Activate HW data base control table\n");
      retCode = HW_REGSG_activateCommandTable(INU_DEVICE__DB_PHASE_1_E);
   }
   else if(dbControlParamsP->dbControl == INU_DEFSG_DB_CONTROL_CLEAR_E)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Clear HW data base control table\n");
      retCode = HW_REGSG_clearCommandTable();
      if (customClearRegsPhaseCb)
         customClearRegsPhaseCb();
   }
   else
   {
      retCode = SYSTEM__ERR_DB_IOCTL_FAIL_WRONG_PARAM;
   }

   return retCode;
}

#ifdef __cplusplus
}
#endif


