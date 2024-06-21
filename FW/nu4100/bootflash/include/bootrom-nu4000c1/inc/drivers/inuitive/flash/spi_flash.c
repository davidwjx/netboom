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

/****************************************************************************
 ***************		L O C A L		  D A T A			   ***************
 ****************************************************************************/
 
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

UINT32 SPI_FLASHG_blockErase(UINT32 address)
{
   UINT32 retVal=SPI_FLASH_SUCCESS;
   BYTE tempBuff[4];
   
   tempBuff[0] = FLASH_ERASE_BLOCK_OPCODE;
   tempBuff[1] = (address >> 16) & 0xFF;
   tempBuff[2] = (address >> 8) & 0xFF;
   tempBuff[3] = address & 0xFF;
   retVal = SPI_FLASHP_waitWIP(WAIT_MAX_ITERATION_NUM);
   if (ERRG_SUCCEEDED(retVal))
   {
      retVal = SPI_FLASHP_writeEnable();
   }
   if (ERRG_SUCCEEDED(retVal))
   {
      // Sent txBuffer:
      retVal = SPI_DRVG_spiTransaction(tempBuff, 4, NULL, 0);
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
    BYTE tempBuff[BUFF_MAX_LEN + 4];
	UINT32 transactionLen = len + 4;
	if (len <= BUFF_MAX_LEN && pBuff && address <= FLASH_LAST_ADDRESS)
	{	
		tempBuff[0] = FLASH_PAGE_PROGRAM_OPCODE;
		tempBuff[1] = (address >> 16) & 0xFF;
		tempBuff[2] = (address >> 8) & 0xFF;
		tempBuff[3] = address & 0xFF;
		memcpy(tempBuff+4, pBuff, len);
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


/////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////

UINT32 SPI_FLASHG_Read(UINT32 address, UINT32 len, BYTE *pRetBuff)
{
	UINT32 retVal=SPI_FLASH_SUCCESS;
	BYTE    tempBuff[4];
   if (pRetBuff && address<FLASH_LAST_ADDRESS)
	{	
		tempBuff[0] = FLASH_READ_OPCODE;
		tempBuff[1] = (address >> 16) & 0xFF;
		tempBuff[2] = (address >> 8) & 0xFF;
		tempBuff[3] = address & 0xFF;

		if (ERRG_SUCCEEDED(retVal))
		{			
			// Sent txBuffer:
			retVal = SPI_DRVG_spiTransaction(tempBuff, 4, pRetBuff, len);
		}
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


UINT32 SPI_FLASHG_init(UINT8 pol, UINT8 phase, unsigned int apb_spi_freq_hz)
{
   return SPI_DRVG_open(pol,  phase, apb_spi_freq_hz);
}

UINT32 SPI_FLASHG_deInit()
{
   return SPI_DRVG_close();
}


