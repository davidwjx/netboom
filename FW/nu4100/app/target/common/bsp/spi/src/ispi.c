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
#include "inu_common.h"

#ifdef __cplusplus
      extern "C" {
#endif

#include "ispi.h"
#include "os_lyr.h"
#include "nu4100_spi0_regs.h"

#ifdef __KERNEL_SPI__
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <errno.h>

// temporary definition till update linux/spi/spidev.h with new struct spi_ioc_transfer
       struct spi_ioc_transfer_n {
      	__u64		tx_buf;
      	__u64		rx_buf;

      	__u32		len;
      	__u32		speed_hz;

      	__u16		delay_usecs;
      	__u8		bits_per_word;
      	__u8		cs_change;
      	__u8		tx_nbits;
      	__u8		rx_nbits;
      	__u16		pad;

      	/* If the contents of 'struct spi_ioc_transfer' ever change
      	 * incompatibly, then the ioctl number (currently 0) must change;
      	 * ioctls with constant size fields get a bit more in the way of
      	 * error checking than ones (like this) where that field varies.
      	 *
      	 * NOTE: struct layout is the same in 64bit and 32bit userspace.
      	 */
      };
#define NUM_FLASH 2
static INT32 fd[NUM_FLASH]={-1,-1};

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
ERRG_codeE SPI_DRVG_open(UINT8 flashNum,char *deviceName)
{
   ERRG_codeE  retVal = SPI__RET_SUCCESS;


   if(fd[flashNum] < 0)
   {
      fd[flashNum] = open(deviceName, O_RDWR);
      LOGG_PRINT(LOG_INFO_E, NULL, "open deviceName %s fd = %d \n",deviceName, fd[flashNum]);
      if (fd[flashNum] < 0)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "spi driver open %s failed fd=%d\n",deviceName,fd[flashNum]);
         retVal = SPI__ERR_INVALID_ARGS;
      }
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "spi driver already open \n");
      retVal = SPI__ERR_ILLEGAL_STATE;
   }
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
ERRG_codeE SPI_DRVG_spiTransaction(UINT8 flashNum, BYTE *pTxBuffer, UINT32 txLen, BYTE *pRxBuffer, UINT32 rxLen)
{
  ERRG_codeE   retVal = SPI__RET_SUCCESS;
  INT32        ret;
  struct       spi_ioc_transfer_n xfer[2];

   if(fd[flashNum] >= 0)
   {
      if(rxLen == 0)
      {
         struct spi_ioc_transfer_n tr = {
            .tx_buf = (unsigned long)pTxBuffer,
            .rx_buf = (unsigned long)0,
            .len = txLen,
            .delay_usecs = 0,
            .speed_hz = 0,
            .bits_per_word = 8,
            .cs_change = 0,
            .tx_nbits = 0,
            .rx_nbits = 0,
        };
         ret = ioctl(fd[flashNum], SPI_IOC_MESSAGE(1), &tr);
         if (ret < 0)
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "spi driver transaction error ret = %d \n",ret);
            retVal = SPI__ERR_UNEXPECTED;
            }
      }
      else
      {

    	 memset(xfer, 0, sizeof xfer);
         xfer[0].tx_buf = (unsigned long)pTxBuffer;
         xfer[0].rx_buf = (unsigned long)0;
         xfer[0].len = txLen;
         xfer[0].bits_per_word = 8;
         xfer[0].cs_change=0;

         xfer[1].rx_buf = (unsigned long) pRxBuffer;
         xfer[1].tx_buf = (unsigned long)0;
         xfer[1].len = rxLen;
         xfer[1].bits_per_word = 8;
         xfer[1].cs_change=0;

         ret = ioctl(fd[flashNum], SPI_IOC_MESSAGE(2), xfer);
         if (ret < 0) 
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "spi driver transaction error ret = %d \n ",ret);
            retVal = SPI__ERR_UNEXPECTED;
         }
      }
   }
   else
   {
     LOGG_PRINT(LOG_ERROR_E, NULL, "spi driver already open \n"); 
      retVal = SPI__ERR_ILLEGAL_STATE;
   }
   return retVal;
}

ERRG_codeE SPI_DRVG_fullDuplexspiTrans(BYTE *pTxBuffer, BYTE *pRxBuffer, UINT32 len)
{
   ERRG_codeE  retVal = SPI__RET_SUCCESS;
   INT32 ret;
   struct spi_ioc_transfer_n tr = {
      .tx_buf = (unsigned long)pTxBuffer,
      .rx_buf = (unsigned long)pRxBuffer,
      .len = len,
      .delay_usecs = 0,
      .speed_hz = 0,
      .bits_per_word = 8,
   };

   ret = ioctl(fd[0], SPI_IOC_MESSAGE(1), &tr);
   if (ret < 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "spi driver transaction error \n");
      retVal = SPI__ERR_UNEXPECTED;
   }
   return retVal;
}

ERRG_codeE SPI_DRVG_close(UINT8 flashNum)
{
   ERRG_codeE  retVal = SPI__RET_SUCCESS;   
   if (fd[flashNum]>=0)
   {
      close(fd[flashNum]);
      fd[flashNum]=-1;
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "spi driver already open \n"); 
      retVal = SPI__ERR_ILLEGAL_STATE;   
   }
   return(retVal);
}

int SPI_DRVG_read( BYTE *pRxBuffer, UINT32 *len)
{
   int  retVal = -1;

   retVal = read(fd[0],pRxBuffer,*len);
               
   if (retVal < 0) 
   {
      printf("spi driver read error ret = %d \n ",retVal);
      return -1;
   }

   *len = retVal;

   return retVal;
}

ERRG_codeE SPI_DRVG_setupMode(int mode)
{
   int     status=0;

   status = ioctl(fd[0], SPI_IOC_WR_MODE, &mode);

   if (status < 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "error in SPI_IOC_WR_MODE ioctl 0x%x, %s\n",mode,strerror(errno));
      return SPI__ERR_UNEXPECTED;      
   }
   
   LOGG_PRINT(LOG_INFO_E, NULL, "SPI_DRVG_setup mode completed. mode = 0x%x\n",mode);

   return SPI__RET_SUCCESS;

}

ERRG_codeE SPI_DRVG_setupSpeed(int speed)
{
   int     status=0;

   status = ioctl(fd[0], SPI_IOC_WR_MAX_SPEED_HZ, &speed);

   if (status < 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "error in SPI_IOC_WR_MAX_SPEED_HZ ioctl 0x%x, %s\n",speed,strerror(errno));
      return SPI__ERR_UNEXPECTED;      
   }
  
   LOGG_PRINT(LOG_INFO_E, NULL, "SPI_DRVG_setup setup completed. speed = %d\n",speed);

   return SPI__RET_SUCCESS;

}

ERRG_codeE SPI_DRVG_setupLsb(int lsbFirst)
{
   int     status=0;

   status = ioctl(fd[0], SPI_IOC_WR_LSB_FIRST, &lsbFirst);
   
   if (status < 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "error in SPI_IOC_WR_LSB_FIRST ioctl 0x%x, %s\n",lsbFirst,strerror(errno));
      return SPI__ERR_UNEXPECTED;       
   }
   
   LOGG_PRINT(LOG_INFO_E, NULL, "SPI_DRVG_setup setup completed. lsbFirst = %d\n",lsbFirst);

   return SPI__RET_SUCCESS;

}


#else

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#define MAX_SPI_CLK_ITR_NUM      10000    // wait spi clock enable iteration loop
#define RX_FIFO_DEPTH            64       // RX fifo depth in ispi version 2.0
#define TX_FIFO_DEPTH            64       // TX fifo depth in ispi version 2.0
#define RX_FIFO_THR              64       // define RX fifo fullness threshold
#define RX_TX_DIS                0        // disable spi rx & tx 
#define TX_EN_RX_DIS             1        // enable spi tx enable, rx disable
#define RX_EN_TX_DIS             2        // enable spi rx
#define RX_TX_EN                 3        // enable spi rx & tx
#define MASTER                   1        // spi master mode
#define SLAVE                    0        // spi slave mode
#define CPHASE_LOW               0        // spi sclk phase low
#define CPHASE_HIGH              1        // spi sclk phase high
#define CPOL_LOW                 0        // spi sclk polarity low (motorola mode)
#define CPOL_HIGH                1        // spi sclk polarity high (TI mode)
#define SAMPLE_8_BITS            0        // 8 bits frame
#define SAMPLE_16_BITS           1        // 16 bits frame
#define SAMPLE_32_BITS           2        // 32 bits frame
#define SPI_TX_MAX_DELAY         10000000

typedef struct
{
   UINT8          pol;
   UINT8          phase;
   MEM_MAPG_addrT deviceBaseAddress;
} SPI_DRVP_DeviceDescT;

typedef struct
{
   MEM_MAPG_addrT deviceBaseAddress;
} SPI_DRVP_gmeDeviceDescT;

/****************************************************************************
 ***************      L O C A L        D A T A            ***************
 ****************************************************************************/
static SPI_DRVP_DeviceDescT      SPI_DRVP_dviceDesc;
static SPI_DRVP_gmeDeviceDescT   GME_DRVP_deviceDesc;

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
static ERRG_codeE SPI_DRVP_clockSetup()
{
   ERRG_codeE  retVal = SPI__RET_SUCCESS;

   UINT32 clk_stat= 0;
   UINT32 itrNum  = 0;
   // GME SW reset
   GME_SW_RESET_SW_FCU_RESET_N_W(0);      // assert spi sw. reset
   GME_SW_RESET_SW_FCU_RESET_N_W(1);      // de-assert spi sw. reset

   // GME clock enable
   GME_CLOCK_ENABLE_FCU_CLK_EN_W(1);      // enable spi clock

   // GME IO MUX Control 
   GME_IO_MUX_CTRL_2_IO_MUX_CTRL_3_W(0)   // set IO pad spi_sdi_0 as input with enabled pull-down
   GME_IO_MUX_CTRL_2_IO_MUX_CTRL_4_W(0)   // set IO pad spi_sdo_0 as bi-dir with enabled pull-down
   GME_IO_MUX_CTRL_2_IO_MUX_CTRL_5_W(0)   // set IO pad spi_scs_n_0 as bi-dir with enabled pull-down
   GME_IO_MUX_CTRL_2_IO_MUX_CTRL_6_W(0)   // set IO pad spi_sclk as bi-dir with enabled pull-down

   //**** wait loop on verifying spi clock is enabled
   // this while shall be replaced by a timeout of 1ms!!!!
   while ((clk_stat==0) && (itrNum < MAX_SPI_CLK_ITR_NUM))
   {
      clk_stat = GME_CLOCK_ENABLE_STATUS_FCU_CLK_EN_R;   // read enabled clock status
      ++itrNum;
   }
   if (itrNum == MAX_SPI_CLK_ITR_NUM)
   {
      retVal = SPI__ERR_ISPI_CLOCK_LOCK_FAIL;            // clock enable wait loop failure
   }
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
static ERRG_codeE SPI_DRVP_init_()
{
   ERRG_codeE  retVal = SPI__RET_SUCCESS;

   retVal = SPI_DRVP_clockSetup();

   if (ERRG_SUCCEEDED(retVal))
   {
      //spi interrupt enable register
      ISPI_ISPI_INT_EN_INT_EN_RX_OVER_W(0);                       // disalbe RX fifo overflow interrupt
      ISPI_ISPI_INT_EN_INT_EN_TX_UNDER_W(0);                      // disable TX fifo underflow interrupt
      ISPI_ISPI_INT_EN_INT_EN_RX_THR_W(0);                        // disable interrupt of RX FIFO fullness over threshold
      ISPI_ISPI_INT_EN_INT_EN_TX_THR_W(0);                        // disable interrupt of TX FIFO available space over threshold
      ISPI_ISPI_INT_EN_INT_EN_RX_TIME_W(0);                       // disable interrupt of RX timeout in slave mode

      //spi command register
      ISPI_ISPI_COMMAND_RX_RESET_W(1);                            // clear RX fifo (self cleared reset)
      ISPI_ISPI_COMMAND_TX_RESET_W(1);                            // clear TX fifo (self cleared reset)

      ISPI_ISPI_DMA_RX_THRESHOLD_W(RX_FIFO_THR);                  // set RX fifo threshold to half of its depth

      //ISPI_RX_TIMER - POR value used: not applicable for master mode.
      //ISPI_CS -  POR value used: cs_0 is enabled out of 4.
      //ISPI_FILTER - POR value used: currently not applicable.
      //ISPI_FILTER_PATTERN -  POR value used: currently not applicable.
      //ISPI_TX_SETUP -    POR value used currently: TBD.
      //ISPI_RX_BIT_ORDER -  POR value used. defines bit order for spi dual/quad lanes. currently single lane is used.
      //ISPI_ACTIVATE_CS -    POR value used.

      //ISPI_CONFIGURATION
      ISPI_ISPI_CONFIGURATION_SPI_EN_W(RX_TX_DIS);                // disable RX & TX
      ISPI_ISPI_CONFIGURATION_TI_MODE_W(CPOL_LOW);                // set spi to motorola mode
      ISPI_ISPI_CONFIGURATION_MASTER_W(MASTER);                   // set spi as master
      ISPI_ISPI_CONFIGURATION_CLK_PHASE_W(CPHASE_LOW);            // serial clk phase set to 0                      
      ISPI_ISPI_CONFIGURATION_CLK_POL_W(CPOL_LOW);                // serial clk idle polarity set to 0 (motorola mode) 
      ISPI_ISPI_CONFIGURATION_TX_PHASE_W(0);                      // TX data is sent on the edge defined by the protocol
      ISPI_ISPI_CONFIGURATION_RX_PHASE_W(0);                      // RX data is sampled on the edge defined by the protocol
      ISPI_ISPI_CONFIGURATION_SAMPLE_BITS_W(SAMPLE_8_BITS);       // Number of bits in frame set to 8
      ISPI_ISPI_CONFIGURATION_CONST_OEN_W(0);                     // Data out pin is in tri-state when not in active data transfer
      ISPI_ISPI_CONFIGURATION_FIFO_PACK_W(0);                     // Every APB access to fifo transfer 8 bits of data
      ISPI_ISPI_CONFIGURATION_LSB_FIRST_W(0);                     // MSB is the first bit in serial transfer
      ISPI_ISPI_CONFIGURATION_RX_MODE_W(0);                       // single data lane for RX
      ISPI_ISPI_CONFIGURATION_CLK_DIV_W(3);                       // set sclk to ~21MHz
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
ERRG_codeE SPI_DRVG_open(UINT8 flashNum, ,char *deviceName)
{
   ERRG_codeE  retVal = SPI__RET_SUCCESS;
   SPI_DRVP_dviceDesc.deviceBaseAddress   = spiInstanceAddress;
   SPI_DRVP_dviceDesc.pol                 = pol;
   SPI_DRVP_dviceDesc.phase               = phase;
   GME_DRVP_deviceDesc.deviceBaseAddress  = gmeInstanceAddress;
   retVal = SPI_DRVP_init_(flashNum);
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
ERRG_codeE SPI_DRVG_spiTransaction(UINT8 flashNum,BYTE *pTxBuffer, UINT32 txLen, BYTE *pRxBuffer, UINT32 rxLen)
{
   ERRG_codeE  retVal      = SPI__RET_SUCCESS;
   UINT32      xmt_count   = 0;
   UINT32      itrNum      = 0;

   // sending tx fifo to serial port
   ISPI_ISPI_ACTIVATE_CS_ACTIVATE_CS_W(1);
   ISPI_ISPI_CONFIGURATION_SPI_EN_W(TX_EN_RX_DIS); // enable TX

   //copy tx buffer to tx fifo
   while(xmt_count < txLen )
   {
      if (ISPI_ISPI_STATUS_TX_ENTRIES_R > 0)       //was: if (ISPI_ISPI_STATUS_TX_ENTRIES_R < TX_FIFO_DEPTH)
      {
         ISPI_ISPI_FIFO_VAL = *pTxBuffer;
         pTxBuffer++;
         xmt_count++;
      }
   }
   // check spi busy flag
   // this while shall be replaced by a timeout of > 100us!!!!
   itrNum =0;
   while ((ISPI_ISPI_STATUS_SPI_BUSY_R == 1) && (itrNum < MAX_SPI_CLK_ITR_NUM))
   {
      ++itrNum;
   }
   // check failure: wait on busy
   if ((itrNum == MAX_SPI_CLK_ITR_NUM) || (ISPI_ISPI_STATUS_TX_ENTRIES_R < TX_FIFO_DEPTH))
   {
      retVal = SPI__ERR_ISPI_TX_FAIL;                 // spi busy timeout or data have been lefted in tx fifo
   }
   // tx complete detected
   ISPI_ISPI_CONFIGURATION_SPI_EN_W(RX_TX_DIS);       // disable tx to prevent tx fifo underrun

   if(pRxBuffer && (ERRG_SUCCEEDED(retVal)))
   {
      ISPI_ISPI_CONFIGURATION_SPI_EN_W(RX_EN_TX_DIS); // enable rx
      xmt_count = 0;
      itrNum = 0;
      // read rx data while not end of data and rx fifo not empty and no timeout
      while (xmt_count < rxLen && (itrNum < MAX_SPI_CLK_ITR_NUM))
      {
         if (ISPI_ISPI_STATUS_RX_ENTRIES_R > 0)
         {
            *(pRxBuffer) = (BYTE)ISPI_ISPI_FIFO_VAL;
            pRxBuffer++;
            xmt_count++;
         }
         itrNum++;
      }
      // tx complete or error detected
      ISPI_ISPI_CONFIGURATION_SPI_EN_W(RX_TX_DIS);    // close spi rx & tx

      // check if error: timeout or short rx length
      if ((itrNum == MAX_SPI_CLK_ITR_NUM) || (xmt_count != rxLen))
      {
         retVal = SPI__ERR_ISPI_RX_FAIL;
      }
   }
   ISPI_ISPI_COMMAND_RX_RESET_W(1);                   // clear RX fifo (self cleared reset)
   ISPI_ISPI_COMMAND_TX_RESET_W(1);                   // clear TX fifo (self cleared reset)
   ISPI_ISPI_ACTIVATE_CS_ACTIVATE_CS_W(0);
   return retVal;
}

ERRG_codeE SPI_DRVG_fullDuplexspiTrans(BYTE *pTxBuffer, BYTE *pRxBuffer, UINT32 len)
{
   int      retVal      = 0;
   UINT32   xmt_count   = 0;
   UINT32   itrNum      = 0;

   // sending tx fifo to serial port
   ISPI_ISPI_ACTIVATE_CS_ACTIVATE_CS_W(1);
   ISPI_ISPI_CONFIGURATION_SPI_EN_W(RX_TX_EN);        // enable TX

   //copy tx buffer to tx fifo
   while (xmt_count < len )
   {
      itrNum =0;
      if (ISPI_ISPI_STATUS_TX_ENTRIES_R > 0)          //was: if (ISPI_ISPI_STATUS_TX_ENTRIES_R < TX_FIFO_DEPTH)
      {
         ISPI_ISPI_FIFO_VAL = *pTxBuffer;
         pTxBuffer++;
         xmt_count++;
      }
      while ((ISPI_ISPI_STATUS_SPI_BUSY_R == 1) && (itrNum < MAX_SPI_CLK_ITR_NUM))
      {
         ++itrNum;
      }
      itrNum =0;
      while (ISPI_ISPI_STATUS_RX_ENTRIES_R > 0 && (itrNum < MAX_SPI_CLK_ITR_NUM))
      {
         *(pRxBuffer) = (BYTE)ISPI_ISPI_FIFO_VAL;
         pRxBuffer++;
         itrNum++;
      }
   }

   ISPI_ISPI_CONFIGURATION_SPI_EN_W(RX_TX_DIS);
   ISPI_ISPI_COMMAND_RX_RESET_W(1);                   // clear RX fifo (self cleared reset)
   ISPI_ISPI_COMMAND_TX_RESET_W(1);                   // clear TX fifo (self cleared reset)
   ISPI_ISPI_ACTIVATE_CS_ACTIVATE_CS_W(0);
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
ERRG_codeE SPI_DRVG_close()
{
   ERRG_codeE  retVal = SPI__RET_SUCCESS;

   return(retVal);
}

#endif //DEFSG_IS_GP_LINUX

#ifdef __cplusplus
}
#endif

