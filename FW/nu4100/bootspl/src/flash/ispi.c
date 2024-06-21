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
#include "spl.h"


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

typedef struct {
   UINT8 pol;
   UINT8 phase;   
} SPI_DRVP_DeviceDescT;

/****************************************************************************
 ***************      L O C A L        D A T A            ***************
 ****************************************************************************/
static SPI_DRVP_DeviceDescT    SPI_DRVP_dviceDesc;

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
*  Function Name: SPI_DRVP_clockSetup
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
static UINT32 SPI_DRVP_clockSetup()
{
   UINT32     retVal = SPI_SUCCESS;
#if 0   

   UINT32 clk_stat = 0;
   UINT32 itrNum = 0;   
   // GME SW reset
   GME_SW_RESET_SW_FCU_RESET_N_W(0);                              // assert spi sw. reset
   GME_SW_RESET_SW_FCU_RESET_N_W(1);                              // de-assert spi sw. reset

   // GME clock enable
   GME_CLOCK_ENABLE_FCU_CLK_EN_W(1);                              // enable spi clock
   
   // GME IO MUX Control 
   GME_IO_MUX_CTRL_2_IO_MUX_CTRL_3_W(0)                           // set IO pad spi_sdi_0 as input with enabled pull-down
   GME_IO_MUX_CTRL_2_IO_MUX_CTRL_4_W(0)                           // set IO pad spi_sdo_0 as bi-dir with enabled pull-down
   GME_IO_MUX_CTRL_2_IO_MUX_CTRL_5_W(0)                           // set IO pad spi_scs_n_0 as bi-dir with enabled pull-down
   GME_IO_MUX_CTRL_2_IO_MUX_CTRL_6_W(0)                           // set IO pad spi_sclk as bi-dir with enabled pull-down
   
   //**** wait loop on verifying spi clock is enabled
   // this while shall be replaced by a timeout of 1ms!!!!
   while ((clk_stat==0) && (itrNum < MAX_SPI_CLK_ITR_NUM))
   {
      clk_stat = GME_CLOCK_ENABLE_STATUS_FCU_CLK_EN_R;               // read enabled clock status
      ++itrNum;
   }
   if (itrNum == MAX_SPI_CLK_ITR_NUM)
   {
      retVal = SPI_FAIL;                        // clock enable wait loop failure
   }
#endif   
   return retVal;
   
}

/****************************************************************************
*
*  Function Name: SPI_DRVP_init
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
static UINT32 SPI_DRVP_init_(unsigned int apb_spi_freq_hz)
{
   UINT32   retVal = SPI_SUCCESS;
#ifdef SLOW_SPI
   UINT32	div = 0x8;
#else
   UINT32	div = 0x0;
#endif
   retVal = SPI_DRVP_clockSetup();

   if (ERRG_SUCCEEDED(retVal))
   {
      //spi interrupt enable register
     SPI_N(ISPI_INT_EN_INT_EN_RX_OVER_W(0));                           // disalbe RX fifo overflow interrupt
     SPI_N(ISPI_INT_EN_INT_EN_TX_UNDER_W(0));                        // disable TX fifo underflow interrupt
     SPI_N(ISPI_INT_EN_INT_EN_RX_THR_W(0));                            // disable interrupt of RX FIFO fullness over threshold
     SPI_N(ISPI_INT_EN_INT_EN_TX_THR_W(0));                            // disable interrupt of TX FIFO available space over threshold
     SPI_N(ISPI_INT_EN_INT_EN_RX_TIME_W(0));                           // disable interrupt of RX timeout in slave mode
      
      //spi command register
     SPI_N(ISPI_COMMAND_RX_RESET_W(1));                               // clear RX fifo (self cleared reset)
     SPI_N(ISPI_COMMAND_TX_RESET_W(1));                               // clear TX fifo (self cleared reset)
      
     SPI_N(ISPI_DMA_RX_THRESHOLD_W(RX_FIFO_THR));                     // set RX fifo threshold to half of its depth
      
     //ISPI_RX_TIMER - POR value used: not applicable for master mode.
     //ISPI_CS -  POR value used: cs_0 is enabled out of 4.
     //ISPI_FILTER - POR value used: currently not applicable.
     //ISPI_FILTER_PATTERN -  POR value used: currently not applicable.
     //ISPI_TX_SETUP -    POR value used currently: TBD.
     //ISPI_RX_BIT_ORDER -  POR value used. defines bit order for spi dual/quad lanes. currently single lane is used.
     //ISPI_ACTIVATE_CS -    POR value used.

      
     //ISPI_CONFIGURATION
     SPI_N(ISPI_CONFIGURATION_SPI_EN_W(RX_TX_DIS));                   // disable RX & TX
     SPI_N(ISPI_CONFIGURATION_TI_MODE_W(CPOL_LOW));                      // set spi to motorola mode
     SPI_N(ISPI_CONFIGURATION_MASTER_W(MASTER));                     // set spi as master
     SPI_N(ISPI_CONFIGURATION_CLK_PHASE_W(CPHASE_LOW));                // serial clk phase set to 0                      
     SPI_N(ISPI_CONFIGURATION_CLK_POL_W(CPOL_LOW));                   // serial clk idle polarity set to 0 (motorola mode) 
     SPI_N(ISPI_CONFIGURATION_TX_PHASE_W(0));                        // TX data is sent on the edge defined by the protocol
     SPI_N(ISPI_CONFIGURATION_RX_PHASE_W(1));                        // RX data is sampled on the edge defined by the protocol
     SPI_N(ISPI_CONFIGURATION_SAMPLE_BITS_W(SAMPLE_8_BITS));            // Number of bits in frame set to 8
     SPI_N(ISPI_CONFIGURATION_CONST_OEN_W(0));                        // Data out pin is in tri-state when not in active data transfer
     SPI_N(ISPI_CONFIGURATION_FIFO_PACK_W(0));                        // Every APB access to fifo transfer 8 bits of data
     SPI_N(ISPI_CONFIGURATION_LSB_FIRST_W(0));                        // MSB is the first bit in serial transfer
     SPI_N(ISPI_CONFIGURATION_RX_MODE_W(0));                        // single data lane for RX
     //Max input clock is 50MHz. External dividers (in gme) are 2(fixed) and 4. Internal divider is 2.
     //For 24MHz src: 24/2/2=6/2=3Mbps
     //For 400MHz src: 400/2/4=50/2=25Mbps
     SPI_N(ISPI_CONFIGURATION_CLK_DIV_W(div));							   // set sclk to ~696/8/(div+1)/2
     spl_log("spi rate: %dbps\n",apb_spi_freq_hz/((div+1)*2));
     
   }
   return retVal;
}





/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: SPI_DRVG_open
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
UINT32 SPI_DRVG_open( UINT8 pol, UINT8 phase, unsigned int apb_spi_freq_hz)
{
   UINT32  retVal = SPI_SUCCESS;
   SPI_DRVP_dviceDesc.pol = pol;
   SPI_DRVP_dviceDesc.phase = phase;
   retVal = SPI_DRVP_init_(apb_spi_freq_hz);
   return retVal;
}


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

   psavedTxBuff=pTxBuffer;
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
      spl_log("\nspi error 1\n");
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

     if(psavedTxBuff[0] == FLASH_QUAD_READ_OPCODE)
      {
         SPI0_ISPI_RX_BIT_ORDER_VAL=0x3201;
         SPI_N(ISPI_CONFIGURATION_RX_MODE_W(2));                        // single data lane for RX
      }
      else if(psavedTxBuff[0] == FLASH_DUAL_READ_OPCODE)
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
      if((psavedTxBuff[0] == FLASH_DUAL_READ_OPCODE)||(psavedTxBuff[0] == FLASH_QUAD_READ_OPCODE))
      {
        SPI_N(ISPI_CONFIGURATION_RX_MODE_W(0));
      }// single data lane for RX

      // check if error: timeout or short rx length
      if ((itrNum == MAX_SPI_CLK_ITR_NUM) || (xmt_count != rxLen))
      {
         spl_log("\nspi error 2: itrNum: %d xmt_count:%d rxLen:%d\n", itrNum, xmt_count, rxLen);
         retVal = SPI_FAIL;
      }
   }
   SPI_N(ISPI_COMMAND_RX_RESET_W(1));                            // clear RX fifo (self cleared reset)
   SPI_N(ISPI_COMMAND_TX_RESET_W(1));                            // clear TX fifo (self cleared reset)
   SPI_N(ISPI_ACTIVATE_CS_ACTIVATE_CS_W(0));
   return retVal;
}


/****************************************************************************
*
*  Function Name: SPI_DRVG_close
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
UINT32 SPI_DRVG_close()
{
   UINT32  retVal = SPI_SUCCESS;


   return(retVal);
}


#ifdef __cplusplus
}
#endif

