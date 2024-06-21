/****************************************************************************
 *
 *   FileName: SPI_flash.c
 *
 *   Author:  Yaron A..
 *
 *   Date:
 *
 *   Description:  Macronix MX25U6435F is 64Mb bits serial Flash memory, which is configured as 8,388,608 x 8
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/

#include "inu_types.h"

#ifdef __cplusplus
      extern "C" {
#endif

#include "log.h"
#include "err_defs.h"
#include "ispi.h"
#include "spi_flash.h"
#include "spi_flash_info.h"
#include "os_lyr.h"
#include "nu4100_spi0_regs.h"

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/

#define WAIT_MAX_ITERATION_NUM   10000000
#define BUFF_MAX_LEN             256
#define CHIP_ERASE_WAIT          75000000
#define BLOCK_ERASE_WAIT         2500000
#define FLASH_CMD_WAIT           5000

/****************************************************************************
 ***************     L O C A L        D A T A                  **************
 ****************************************************************************/
static unsigned int spi_erase_size[NUM_OF_FLASH]= {1024*64,1024*64};
static unsigned int spi_num_sectors[NUM_OF_FLASH] ={ 128,32};
static unsigned int spi_flags[NUM_OF_FLASH];

extern UINT32 spi_nor_ids_arr_size;
extern struct flash_info spi_nor_ids[1];

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/
/****************************************************************************
*
*  Function Name: SPI_FLASHP_writeEnable
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: SPI flash
*
****************************************************************************/

static ERRG_codeE SPI_FLASHP_writeEnable(UINT8 flashNum)
{
   ERRG_codeE retVal=SPI_FLASH__RET_SUCCESS;
   BYTE val = FLASH_WRITE_ENABLE_OPCODE;
   retVal = SPI_DRVG_spiTransaction(flashNum,&val, 1,NULL, 0);
   return retVal;
}


/****************************************************************************
*
*  Function Name: SPI_FLASHG_statusGet
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: SPI flash
*
****************************************************************************/
static ERRG_codeE SPI_FLASHG_statusGet(UINT8 flashNum, BYTE *pStatus)
{
   ERRG_codeE retVal=SPI_FLASH__RET_SUCCESS;
   BYTE val = FLASH_STATUS_GET_OPCODE;
   if (pStatus)
   {
      // Sent txBuffer:
      if (ERRG_SUCCEEDED(retVal))
      {
         retVal=SPI_DRVG_spiTransaction(flashNum, &val, 1, pStatus, 1);
      }
   }
   else
   {
      retVal = SPI_FLASH__ERR_FLASH_STATUS_READ_FAIL;
   }
   return retVal;
}

/****************************************************************************
*
*  Function Name: SPI_FLASHP_waitWIP
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: SPI flash
*
****************************************************************************/

static ERRG_codeE SPI_FLASHP_waitWIP(UINT8 flashNum, UINT32 itrNum)
{
   UINT32 cnt=0;
   ERRG_codeE retVal=SPI_FLASH__RET_SUCCESS;
   BYTE status=0;

   cnt=0;
   do{
      ++cnt;
      retVal = SPI_FLASHG_statusGet(flashNum, &status);
      if  (cnt>itrNum )
      {
         retVal  = SPI_FLASH__ERR_FLASH_WIP_ITR_FAIL;
      }
   }while((status & 0x1) && (ERRG_SUCCEEDED(retVal)));
   return retVal;
}


/****************************************************************************
*
*  Function Name: SPI_FLASHG_blockErase
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: SPI flash
*
****************************************************************************/

ERRG_codeE SPI_FLASHG_blockErase(UINT8 flashNum, UINT32 address)
{
   ERRG_codeE retVal=SPI_FLASH__RET_SUCCESS;
   BYTE tempBuff[5],offset=0;

   if (spi_flags[flashNum] & SPI_NOR_4B_OPCODES)
   {
      tempBuff[offset++] = FLASH_4B_ERASE_BLOCK_OPCODE;
      tempBuff[offset++] = (address >> 24) & 0xFF;
   }
   else
   {
      tempBuff[offset++] = FLASH_ERASE_BLOCK_OPCODE;
   }

   tempBuff[offset++] = (address >> 16) & 0xFF;
   tempBuff[offset++] = (address >> 8) & 0xFF;
   tempBuff[offset++] = address & 0xFF;

   retVal = SPI_FLASHP_waitWIP(flashNum, WAIT_MAX_ITERATION_NUM);
   if (ERRG_SUCCEEDED(retVal))
   {
      retVal = SPI_FLASHP_writeEnable(flashNum);
   }
   if (ERRG_SUCCEEDED(retVal))
   {
      // Sent txBuffer:
      retVal = SPI_DRVG_spiTransaction(flashNum, tempBuff, offset, NULL, 0);
   }
   if (ERRG_SUCCEEDED(retVal))
   {
      retVal = SPI_FLASHP_waitWIP(flashNum, WAIT_MAX_ITERATION_NUM);
   }

    return retVal;

}



/****************************************************************************
*
*  Function Name: SPI_FLASHG_chipErase
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: SPI flash
*
****************************************************************************/

ERRG_codeE SPI_FLASHG_chipErase(UINT8 flashNum)
{
    ERRG_codeE retVal=SPI_FLASH__RET_SUCCESS;
   BYTE val;
   val = FLASH_ERASE_CHIP_OPCODE;
   retVal = SPI_FLASHP_waitWIP(flashNum, WAIT_MAX_ITERATION_NUM);
   if (ERRG_SUCCEEDED(retVal))
   {
      retVal = SPI_FLASHP_writeEnable(flashNum);
   }
   if (ERRG_SUCCEEDED(retVal))
   {
      // Sent txBuffer:
      retVal = SPI_DRVG_spiTransaction(flashNum, &val, 1, NULL, 0);
   }
   if (ERRG_SUCCEEDED(retVal))
   {
      retVal = SPI_FLASHP_waitWIP(flashNum, WAIT_MAX_ITERATION_NUM);
   }

    return retVal;

}

ERRG_codeE SPI_FLASHG_program(UINT8 flashNum, UINT32 address, BYTE *pBuff, UINT32 len)
{
   ERRG_codeE retVal=SPI_FLASH__RET_SUCCESS;
   BYTE tempBuff[BUFF_MAX_LEN + 5];
   UINT32 transactionLen, offset = 0;
   UINT32 flashLastAddr =   (spi_erase_size[flashNum]*spi_num_sectors[flashNum]);

   if (len <= BUFF_MAX_LEN && pBuff && address <= flashLastAddr)
   {
      if (spi_flags[flashNum] & SPI_NOR_4B_OPCODES)
      {
         tempBuff[offset++] = FLASH_4B_PAGE_PROGRAM_OPCODE;
         tempBuff[offset++] = (address >> 24) & 0xFF;
      }
      else
      {
         tempBuff[offset++] = FLASH_PAGE_PROGRAM_OPCODE;
      }
      tempBuff[offset++] = (address >> 16) & 0xFF;
      tempBuff[offset++] = (address >> 8) & 0xFF;
      tempBuff[offset++] = address & 0xFF;
      transactionLen = len + offset;
      memcpy(tempBuff+offset, pBuff, len);
      retVal = SPI_FLASHP_waitWIP(flashNum, WAIT_MAX_ITERATION_NUM);
      if (ERRG_SUCCEEDED(retVal))
      {
         retVal = SPI_FLASHP_writeEnable(flashNum);
      }
      if (ERRG_SUCCEEDED(retVal))
      {
         // Sent txBuffer:
         retVal = SPI_DRVG_spiTransaction(flashNum, tempBuff, transactionLen, NULL, 0);

      }
      if (ERRG_SUCCEEDED(retVal))

      {
         retVal = SPI_FLASHP_waitWIP(flashNum, WAIT_MAX_ITERATION_NUM);
      }
   }
   else
   {
      retVal = SPI_FLASH__ERR_FLASH_INVALID_PARAMS;
   }
    return retVal;
}


/////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////

// ERRG_codeE SPI_FLASHG_Read(UINT8 flashNum, UINT32 address, UINT32 len, BYTE *pRetBuff)
// {
//    ERRG_codeE retVal=SPI_FLASH__RET_SUCCESS;
//    BYTE tempBuff[5];
//    UINT32 offset = 0;
//    UINT32 flashLastAddr =   (spi_erase_size[flashNum]*spi_num_sectors[flashNum]);

//    if (pRetBuff && address<flashLastAddr)
//    {
//       if (spi_flags[flashNum] & SPI_NOR_4B_OPCODES)
//       {
//          tempBuff[offset++] = FLASH_4B_READ_OPCODE;
//          tempBuff[offset++] = (address >> 24) & 0xFF;
//       }
//       else
//       {
//          if (spi_flags[flashNum] & SPI_NOR_DUAL_READ)
//          {
//             tempBuff[offset++] = FLASH_DUAL_READ_OPCODE;
//          }
//          else
//          {
//             tempBuff[offset++] = FLASH_READ_OPCODE;
//          }
//       }
//       tempBuff[offset++] = (address >> 16) & 0xFF;
//       tempBuff[offset++] = (address >> 8) & 0xFF;
//       tempBuff[offset++] = address & 0xFF;

//       if (spi_flags[flashNum] & SPI_NOR_DUAL_READ)
//          tempBuff[offset++] = 0;


//       if (ERRG_SUCCEEDED(retVal))
//       {
//          // Sent txBuffer:
//          retVal = SPI_DRVG_spiTransaction(flashNum, tempBuff, offset, pRetBuff, len);
//       }
//    }
//    else
//    {
//       retVal = SPI_FLASH__ERR_FLASH_INVALID_PARAMS;
//    }
//    return retVal;
// }

ERRG_codeE SPI_FLASHG_Read(UINT8 flashNum, UINT32 address, UINT32 len, BYTE *pRetBuff)
{
   ERRG_codeE retVal=SPI_FLASH__RET_SUCCESS;
   BYTE    tempBuff[6],offset=0;

   if (pRetBuff )
   {
      if (spi_flags[flashNum]&SPI_NOR_4B_OPCODES)
      {
          tempBuff[offset++] = FLASH_4B_READ_OPCODE;
          tempBuff[offset++] = (address >> 24) & 0xFF;
      }
      else
      {
          if (spi_flags[flashNum]&SPI_NOR_DUAL_READ)
             tempBuff[offset++] = FLASH_DUAL_READ_OPCODE;
          else tempBuff[offset++] = FLASH_READ_OPCODE;
      }

      tempBuff[offset++] = (address >> 16) & 0xFF;
      tempBuff[offset++] = (address >> 8) & 0xFF;
      tempBuff[offset++] = address & 0xFF;
      if (spi_flags[flashNum]&SPI_NOR_DUAL_READ)
         tempBuff[offset++] = 0;

      if (ERRG_SUCCEEDED(retVal))
      {
         // Sent txBuffer:
         retVal = SPI_DRVG_spiTransaction(flashNum, tempBuff, offset, pRetBuff, len);
      }
   }
   else
   {
      retVal = SPI_FLASH__ERR_FLASH_INVALID_PARAMS;
   }
   return retVal;
}

/////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////
static UINT32 SPI_FLASHG_read_reg(UINT8 flashNum,UINT8 opcode, UINT8 *pRetBuff, UINT32 len)
{

   UINT32 retVal=SPI_FLASH__RET_SUCCESS;
   BYTE    tempBuff[4];

   if (pRetBuff)
   {
      tempBuff[0] = opcode;
         // Sent txBuffer:
      retVal = SPI_DRVG_spiTransaction(flashNum,tempBuff, 1, pRetBuff, len);
   }
   else
   {
      retVal = SPI_FLASH__ERR_FLASH_INVALID_PARAMS;
   }
   return retVal;
}

UINT32 SPI_FLASHG_sizeGet(UINT8 flashNum)
{
   return (spi_erase_size[flashNum] * spi_num_sectors[flashNum]);
}

UINT32 SPI_FLASHG_getBlockSize(UINT8 flashNum)
{
   return (spi_erase_size[flashNum]);
}

UINT32 SPI_FLASHG_getNumBlocks(UINT8 flashNum)
{
   return (spi_num_sectors[flashNum]);
}

ERRG_codeE SPI_FLASHG_init(UINT8 flashNum, char *deviceName)
{
   ERRG_codeE retVal;
   UINT32 tmp;
   UINT32 ind;
   UINT8 id[6];
   struct flash_info *info;

   retVal = SPI_DRVG_open(flashNum, deviceName);
   if (retVal == SPI__RET_SUCCESS)
   {
      retVal = SPI_FLASHG_read_reg(flashNum,FLASH_OP_RDID, id, SPI_NOR_MAX_ID_LEN);
      LOGG_PRINT(LOG_INFO_E, NULL,"FLASH version %x %x %x %x %x %x\n", id[0], id[1], id[2],
            id[3], id[4], id[5]);

      for (tmp = 0; tmp < spi_nor_ids_arr_size - 1; tmp++)
      {
         info = &spi_nor_ids[tmp];
         if (info->id_len)
         {
            for (ind = 0; ind < info->id_len; ind++)
            {
               if (info->id[ind] != id[ind])
                  break;

            }
            if (ind == info->id_len)
            {
               spi_erase_size[flashNum]    = info->sector_size;
               spi_num_sectors[flashNum]   = info->n_sectors;
               spi_flags[flashNum]         = (info->flags & ~SPI_NOR_DUAL_READ);
               LOGG_PRINT(LOG_INFO_E, NULL,
                     "flash found name %s erase size 0x%x numb of sectors %d, spi_flags = 0x%x\n",
                     info->name, spi_erase_size[flashNum], spi_num_sectors[flashNum], spi_flags[flashNum]);

               break;
            }

         }
      }
   }
   return retVal;

}

ERRG_codeE SPI_FLASHG_deInit(UINT8 flashNum)
{
   return SPI_DRVG_close(flashNum);
}





