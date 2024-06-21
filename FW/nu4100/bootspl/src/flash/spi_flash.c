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
#ifdef __cplusplus
      extern "C" {
#endif
#include <string.h>
#include "ispi.h"
#include "spi_flash.h"
#include "nu_regs.h"
#include "os_defs.h"
#include "flash_info.h"
#include "inu_storage.h"
#include "storage.h"

 /****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/


#define WAIT_MAX_ITERATION_NUM  10000000
#define BUFF_MAX_LEN            256
#define CHIP_ERASE_WAIT 	    75000000
#define BLOCK_ERASE_WAIT 	    2500000
#define FLASH_CMD_WAIT 	       5000
#define SPI_FLASH_SUCCESS  (0)
#define SPI_FLASH_FAIL     (1)
#define ERRG_SUCCEEDED(err)            ((err) == 0)
UINT32 SPI_DRVG_spiTransaction(BYTE *pTxBuffer, UINT32 txLen, BYTE *pRxBuffer, UINT32 rxLen);

/****************************************************************************
 ***************		L O C A L		  D A T A			   ***************
 ****************************************************************************/

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/
extern FlashInfoT flashInfo;

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

	static UINT32 SPI_FLASHP_writeEnable()
	{
		UINT32 retVal=SPI_FLASH_SUCCESS;
		BYTE val = FLASH_WRITE_ENABLE_OPCODE;
		retVal = SPI_DRVG_spiTransaction(&val, 1,NULL, 0);
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
	static UINT32 SPI_FLASHG_statusGet(BYTE *pStatus)
	{
		UINT32 retVal=SPI_FLASH_SUCCESS;
		BYTE val = FLASH_STATUS_GET_OPCODE;
		if (pStatus)
		{
			// Sent txBuffer:
			if (ERRG_SUCCEEDED(retVal))
			{
				retVal=SPI_DRVG_spiTransaction(&val, 1, pStatus, 1);
			}
		}
		else
		{
			retVal = SPI_FLASH_FAIL;
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

	static UINT32 SPI_FLASHP_waitWIP(UINT32 itrNum)
	{
	   UINT32 cnt=0;
	   UINT32 retVal=SPI_FLASH_SUCCESS;
	   BYTE status=0;

	   cnt=0;
	   do{
		  ++cnt;
		  retVal = SPI_FLASHG_statusGet(&status);
		  if  (cnt>itrNum )
		  {
			 retVal  = SPI_FLASH_FAIL;
		  }
	   }while((status & 0x1) && (ERRG_SUCCEEDED(retVal)));
	   return retVal;
	}



UINT32 SPI_FLASHG_blockErase(UINT32 address)
{
	UINT32 retVal=SPI_FLASH_SUCCESS;
	BYTE tempBuff[5], offset=0;

	if (flashInfo.spi_flags & SPI_NOR_4B_OPCODES)
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

	retVal = SPI_FLASHP_waitWIP(WAIT_MAX_ITERATION_NUM);
	if (ERRG_SUCCEEDED(retVal))
	{
		retVal = SPI_FLASHP_writeEnable();
	}
	if (ERRG_SUCCEEDED(retVal))
	{
		// Sent txBuffer:
		retVal = SPI_DRVG_spiTransaction(tempBuff, offset, NULL, 0);
	}
	if (ERRG_SUCCEEDED(retVal))
	{
		retVal = SPI_FLASHP_waitWIP(WAIT_MAX_ITERATION_NUM);
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

UINT32 SPI_FLASHG_chipErase()
{
    UINT32 retVal=SPI_FLASH_SUCCESS;
	BYTE val;
	val = FLASH_ERASE_CHIP_OPCODE;
	retVal = SPI_FLASHP_waitWIP(WAIT_MAX_ITERATION_NUM);
	if (ERRG_SUCCEEDED(retVal))
	{
		retVal = SPI_FLASHP_writeEnable();
	}
	if (ERRG_SUCCEEDED(retVal))
	{
		// Sent txBuffer:
		retVal = SPI_DRVG_spiTransaction(&val, 1, NULL, 0);
	}
	if (ERRG_SUCCEEDED(retVal))
	{
		retVal = SPI_FLASHP_waitWIP(WAIT_MAX_ITERATION_NUM);
	}

    return retVal;

}

UINT32 SPI_FLASHG_program(UINT32 address, BYTE *pBuff, UINT32 len)
{
	UINT32 retVal=SPI_FLASH_SUCCESS;
	BYTE tempBuff[BUFF_MAX_LEN + 5], offset = 0;
	UINT32 transactionLen = len + 5;

	if (len <= BUFF_MAX_LEN && pBuff)
	{
 		if (flashInfo.spi_flags & SPI_NOR_4B_OPCODES)
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

		memcpy(tempBuff + offset, pBuff, len);

		retVal = SPI_FLASHP_waitWIP(WAIT_MAX_ITERATION_NUM);
		if (ERRG_SUCCEEDED(retVal))
		{
			retVal = SPI_FLASHP_writeEnable();
		}

		if (ERRG_SUCCEEDED(retVal))
		{
			// Sent txBuffer:
			retVal = SPI_DRVG_spiTransaction(tempBuff, transactionLen, NULL, 0);
		}

		if (ERRG_SUCCEEDED(retVal))
		{
			retVal = SPI_FLASHP_waitWIP(WAIT_MAX_ITERATION_NUM);
		}
	}
	else
	{
		retVal = SPI_FLASH_FAIL;
	}
    return retVal;
}

UINT32 SPI_FLASHG_setStatus()
{
    UINT32 retVal=SPI_FLASH_SUCCESS;
   BYTE val[2],status;

   retVal = SPI_FLASHG_statusGet(&status);
   status|=0x40;
   val[0] = 1;val[1]=status;

   retVal = SPI_FLASHP_waitWIP(WAIT_MAX_ITERATION_NUM);
   if (ERRG_SUCCEEDED(retVal))
   {
      retVal = SPI_FLASHP_writeEnable();
   }
   if (ERRG_SUCCEEDED(retVal))
   {
      retVal = SPI_DRVG_spiTransaction(val, 2, NULL, 0);
   }
   if (ERRG_SUCCEEDED(retVal))
   {
      retVal = SPI_FLASHP_waitWIP(WAIT_MAX_ITERATION_NUM);
   }

    return retVal;

}

UINT32 SPI_FLASHG_resetStatus()
{
    UINT32 retVal=SPI_FLASH_SUCCESS;
   BYTE val[2],status;

   retVal = SPI_FLASHG_statusGet(&status);
   status&=0xbf;
   val[0] = 1;val[1]=status;

   retVal = SPI_FLASHP_waitWIP(WAIT_MAX_ITERATION_NUM);
   if (ERRG_SUCCEEDED(retVal))
   {
      retVal = SPI_FLASHP_writeEnable();
   }
   if (ERRG_SUCCEEDED(retVal))
   {
      retVal = SPI_DRVG_spiTransaction(val, 2, NULL, 0);
   }
   if (ERRG_SUCCEEDED(retVal))
   {
      retVal = SPI_FLASHP_waitWIP(WAIT_MAX_ITERATION_NUM);
   }

    return retVal;

}


/////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////

UINT32 SPI_FLASHG_Read(UINT32 address, UINT32 len, BYTE *pRetBuff)
{
   UINT32 retVal=SPI_FLASH_SUCCESS;
   BYTE    tempBuff[6],offset=0,useQuad=0;

   if ((flashInfo.spi_flags & SPI_NOR_QUAD_READ) && (len > MIN_QUAD_LEN))
      useQuad=1;
   if (useQuad)
      SPI_FLASHG_setStatus();
   if (pRetBuff )
   {
      if (flashInfo.spi_flags & SPI_NOR_4B_OPCODES)
      {
          tempBuff[offset++] = FLASH_4B_READ_OPCODE;
           tempBuff[offset++] = (address >> 24) & 0xFF;
      }
      else
      {

          if(useQuad)
            tempBuff[offset++] = FLASH_QUAD_READ_OPCODE;
          else if (flashInfo.spi_flags&SPI_NOR_DUAL_READ)
             tempBuff[offset++] = FLASH_DUAL_READ_OPCODE;
          else tempBuff[offset++] = FLASH_READ_OPCODE;
      }

      tempBuff[offset++] = (address >> 16) & 0xFF;
      tempBuff[offset++] = (address >> 8) & 0xFF;
      tempBuff[offset++] = address & 0xFF;
       if ((flashInfo.spi_flags&SPI_NOR_DUAL_READ)||useQuad)
         tempBuff[offset++] = 0;

      if (ERRG_SUCCEEDED(retVal))
      {
         // Sent txBuffer:
         retVal = SPI_DRVG_spiTransaction(tempBuff, offset, pRetBuff, len);
      }
   }
   else
   {
      retVal = SPI_FLASH_FAIL;
   }
   if (useQuad)
      SPI_FLASHG_resetStatus();
   return retVal;
}


/////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////
UINT32 SPI_FLASHG_read_reg(UINT8 opcode, UINT8 *pRetBuff, UINT32 len)
{

	UINT32 retVal=SPI_FLASH_SUCCESS;
	BYTE    tempBuff[4];

   if (pRetBuff)
	{
		tempBuff[0] = opcode;
			// Sent txBuffer:
		retVal = SPI_DRVG_spiTransaction(tempBuff, 1, pRetBuff, len);
	}
	else
	{
		retVal = SPI_FLASH_FAIL;
	}
	return retVal;
}


UINT32 SPI_FLASHG_sizeGet()
{
	return FLASH_SIZE;
}




