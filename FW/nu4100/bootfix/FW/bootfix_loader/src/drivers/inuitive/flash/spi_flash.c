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






/////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////

UINT32 SPI_FLASHG_Read(UINT32 address, UINT32 len, BYTE *pRetBuff)
{
   UINT32 retVal=SPI_FLASH_SUCCESS;
   BYTE    tempBuff[5],offset=0;

   if (pRetBuff )
   {
      tempBuff[offset++] = FLASH_DUAL_READ_OPCODE;
      tempBuff[offset++] = (address >> 16) & 0xFF;
      tempBuff[offset++] = (address >> 8) & 0xFF;
      tempBuff[offset++] = address & 0xFF;
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
   return retVal;
}

