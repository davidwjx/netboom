/****************************************************************************
 *
 *   FileName: ispi.c
 *
 *   Author:  Yaron A..
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "common.h"

#ifdef __cplusplus
      extern "C" {
#endif
#include "ispi.h"
#include "nu_regs.h"
#include "spi_flash.h"


/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#define MAX_SPI_CLK_ITR_NUM     10000                           // wait spi clock enable iteration loop
#define RX_FIFO_DEPTH           64                              // RX fifo depth in ispi version 2.0
#define TX_FIFO_DEPTH           64                              // TX fifo depth in ispi version 2.0
#define RX_FIFO_THR             64                              // define RX fifo fullness threshold
#define RX_TX_DIS                0                              // disable spi rx & tx 
#define TX_EN_RX_DIS             1                              // enable spi tx enable, rx disable
#define RX_EN_TX_DIS             2                              // enable spi rx
#define RX_TX_EN                 3                              // enable spi rx & tx
#define MASTER                   1                              // spi master mode
#define SLAVE                    0                              // spi slave mode
#define CPHASE_LOW               0                              // spi sclk phase low
#define CPHASE_HIGH              1                              // spi sclk phase high
#define CPOL_LOW                 0                              // spi sclk polarity low (motorola mode)
#define CPOL_HIGH                1                                 // spi sclk polarity high (TI mode)
#define SAMPLE_8_BITS            0                              // 8 bits frame
#define SAMPLE_16_BITS           1                              // 16 bits frame
#define SAMPLE_32_BITS           2                              // 32 bits frame
#define SPI_TX_MAX_DELAY        10000000
#define SPI_SUCCESS        (0)
#define SPI_FAIL           (1)
#define ERRG_SUCCEEDED(err)            ((err) == 0)

#ifdef NU3K_TEST
#define SPI_N(name)  ISPI_ ## name
#else
#define SPI_N(name)  SPI0_ ## name
#endif


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
*  Function Name: SPI_DRVG_spiTransaction
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: SPI driver
*
****************************************************************************/
UINT32 SPI_DRVG_spiTransaction(BYTE *pTxBuffer, UINT32 txLen, BYTE *pRxBuffer, UINT32 rxLen)
{
   UINT32   retVal = SPI_SUCCESS;
   UINT32 xmt_count = 0,rxEntries;
   UINT32 itrNum = 0,readSize=1,temp;
   BYTE *psavedTxBuff=pTxBuffer;
   // sending tx fifo to serial port
   SPI_N(ISPI_CS_CS_EN_W(1));
   SPI_N(ISPI_ACTIVATE_CS_ACTIVATE_CS_W(1));
   SPI_N(ISPI_CONFIGURATION_SPI_EN_W(TX_EN_RX_DIS));                     // enable TX

   //copy tx buffer to tx fifo
   while (xmt_count < txLen )
   {
   
      if (SPI_N(ISPI_STATUS_TX_ENTRIES_R) > 0)     //was: if (ISPI_ISPI_STATUS_TX_ENTRIES_R < TX_FIFO_DEPTH)
      {
         SPI_N(ISPI_FIFO_VAL) = *pTxBuffer;   
         pTxBuffer++;
         xmt_count++;
      }
   }
   // check spi busy flag
   // this while shall be replaced by a timeout of > 100us!!!!
   itrNum =0;
   while ((SPI_N(ISPI_STATUS_SPI_BUSY_R) == 1) && (itrNum < MAX_SPI_CLK_ITR_NUM))
   {
      ++itrNum;
   }
   // check failure: wait on busy
   if ((itrNum == MAX_SPI_CLK_ITR_NUM) || (SPI_N(ISPI_STATUS_TX_ENTRIES_R) < TX_FIFO_DEPTH))
   {
      rel_log("\nspi error 1\n");
      retVal = SPI_FAIL;                                  // spi busy timeout or data have been lefted in tx fifo 
   }
   // tx complete detected
   SPI_N(ISPI_CONFIGURATION_SPI_EN_W(RX_TX_DIS));               // disable tx to prevent tx fifo underrun

   if (pRxBuffer && (ERRG_SUCCEEDED(retVal)))
   {
      if ((rxLen%4)==0)
      {
         SPI_N(ISPI_CONFIGURATION_SAMPLE_BITS_W(SAMPLE_32_BITS));            // Number of bits in frame set to 8
         readSize = 4;
      }

      if(psavedTxBuff[0] == FLASH_DUAL_READ_OPCODE)
      {
            SPI0_ISPI_RX_BIT_ORDER_VAL=0x3201;
            SPI_N(ISPI_CONFIGURATION_RX_MODE_W(1));                        // single data lane for RX
      }
      SPI_N(ISPI_CONFIGURATION_SPI_EN_W(RX_EN_TX_DIS));               // enable rx   
      xmt_count = 0;
      itrNum = 0;
      // read rx data while not end of data and rx fifo not empty and no timeout 
      while ((xmt_count < rxLen) && (itrNum < MAX_SPI_CLK_ITR_NUM))
      {
         rxEntries = SPI_N(ISPI_STATUS_RX_ENTRIES_R);
         while  ((rxEntries > 0) && (xmt_count < rxLen))
         {
            if (readSize == 1)
            *(pRxBuffer) =  (BYTE)SPI_N(ISPI_FIFO_VAL);
            else
            {
               temp = (UINT32)SPI_N(ISPI_FIFO_VAL);
               *((UINT32 *)pRxBuffer) = ((temp>>24)&0xff) | // move byte 3 to byte 0
                                   ((temp<<8)&0xff0000) | // move byte 1 to byte 2
                                   ((temp>>8)&0xff00) | // move byte 2 to byte 1
                                   ((temp<<24)&0xff000000); // byte 0 to byte 3
            }

            pRxBuffer+=readSize;
            xmt_count+=readSize;
            rxEntries--;
            itrNum = 0;
         }
         itrNum++;
      }   
      // tx complete or error detected
      SPI_N(ISPI_CONFIGURATION_SPI_EN_W(RX_TX_DIS));                  // close spi rx & tx
      if ((rxLen%4)==0)
         SPI_N(ISPI_CONFIGURATION_SAMPLE_BITS_W(SAMPLE_8_BITS));            // Number of bits in frame set to 8
      if(psavedTxBuff[0] == FLASH_DUAL_READ_OPCODE)
      {
        SPI_N(ISPI_CONFIGURATION_RX_MODE_W(0));
      }// single data lane for RX

      // check if error: timeout or short rx length
      if ((itrNum == MAX_SPI_CLK_ITR_NUM) || (xmt_count != rxLen))
      {
         rel_log("\nspi error 2: itrNum: %d xmt_count:%d rxLen:%d\n", itrNum, xmt_count, rxLen);
         retVal = SPI_FAIL;
      }
   }
   SPI_N(ISPI_COMMAND_RX_RESET_W(1));                            // clear RX fifo (self cleared reset)
   SPI_N(ISPI_COMMAND_TX_RESET_W(1));                            // clear TX fifo (self cleared reset)
   SPI_N(ISPI_ACTIVATE_CS_ACTIVATE_CS_W(0));
   return retVal;
}


#ifdef __cplusplus
}
#endif

