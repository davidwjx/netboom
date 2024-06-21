/****************************************************************************
 *
 *   FileName: i2s.c
 *
 *   Author:  Arnon C.
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/

/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
 ****************************************************************************/
#include "inu_common.h"
#include "sys_defs.h"
#include "os_lyr.h"
#include "i2s.h"
#include "io_pal.h"
#include "cde_drv.h"
#include "cde_mngr.h"
//#include "mem_mngr.h"
#include "gme_mngr.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************      L O C A L       D E F N I T I O N S     ***************
 ****************************************************************************/

/* common register for all channel */
#define I2SP_IER     (0x000)
#define I2SP_IRER    (0x004)
#define I2SP_ITER    (0x008)
#define I2SP_CER     (0x00C)
#define I2SP_CCR     (0x010)
#define I2SP_RXFFR   (0x014)
#define I2SP_TXFFR   (0x018)
/* DMA read registers */
#define I2SP_RXDMA   (0x1C0)


/* I2STxRxRegisters for all channels */
#define I2SP_LRBR_LTHR(x)  (0x40 * x + 0x020)
#define I2SP_RRBR_RTHR(x)  (0x40 * x + 0x024)
#define I2SP_RER(x)     (0x40 * x + 0x028)
#define I2SP_TER(x)     (0x40 * x + 0x02C)
#define I2SP_RCR(x)     (0x40 * x + 0x030)
#define I2SP_TCR(x)     (0x40 * x + 0x034)
#define I2SP_ISR(x)     (0x40 * x + 0x038)
#define I2SP_IMR(x)     (0x40 * x + 0x03C)
#define I2SP_ROR(x)     (0x40 * x + 0x040)
#define I2SP_TOR(x)     (0x40 * x + 0x044)
#define I2SP_RFCR(x)    (0x40 * x + 0x048)
#define I2SP_TFCR(x)    (0x40 * x + 0x04C)
#define I2SP_RFF(x)     (0x40 * x + 0x050)
#define I2SP_TFF(x)     (0x40 * x + 0x054)

/* I2SCOMPRegisters */
#define I2S_COMP_PARAM_2   (0x01F0)
#define I2S_COMP_PARAM_1   (0x01F4)
#define I2S_COMP_VERSION   (0x01F8)
#define I2S_COMP_TYPE      (0x01FC)

#define I2SP_NUM_OF_HW_CHAN      (1)
#define I2SP_FORMAT              (I2SP_FORMAT_S32_LE)
/* DMA definitions */
#define I2SP_DMA_TRANSFER_WIDTH  (SYS_DEFSG_MEMPOOL_AUDIO_TRANSFER_SIZE_BYTES)
#define I2SP_DMA_NUM_BUFFERS     (SYS_DEFSG_MEMPOOL_GP_AUDIO_NUM_BUFS)

//#define I2SP_TEST_MODE
/****************************************************************************
 ***************      L O C A L       T Y P E D E F S         ***************
 ****************************************************************************/
typedef enum
{
   I2SP_MODULE_STATUS_DUMMY_E = 0,
   I2SP_MODULE_STATUS_CLOSE_E = 1,
   I2SP_MODULE_STATUS_OPEN_E  = 2,
   I2SP_MODULE_STATUS_LAST_E
} I2SP_moduleStatusE;


typedef enum
{
   I2SP_FORMAT_S16_LE = 2,
   I2SP_FORMAT_S24_LE = 6,
   I2SP_FORMAT_S32_LE = 10,
} I2SP_formatE;

typedef struct
{
   I2SG_modeE            mode;
   MEM_POOLG_handleT     poolH;
   I2SP_moduleStatusE    moduleStatus;
   UINT32                bufferIndex;
   UINT32                deviceBaseAddr;
} I2SP_dataBaseT;

/****************************************************************************
 ***************      L O C A L    D A T A                 ***************
****************************************************************************/
static I2SP_dataBaseT        I2SP_dataBase;

/****************************************************************************
*
*  Function Name: I2SP_read_reg
*
*  Description:
*   
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
static UINT32 I2SP_read_reg(UINT32 regAddr)
{
   return *((volatile UINT32*)(I2SP_dataBase.deviceBaseAddr + regAddr));
}

/****************************************************************************
*
*  Function Name: I2SP_write_reg
*
*  Description:
*   
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
static void I2SP_write_reg(UINT32 regAddr, UINT32 val)
{
   *((volatile UINT32*)(I2SP_dataBase.deviceBaseAddr + regAddr)) = val;
}

static void I2SP_dump_reg()
{
   UINT32 IER; 
   UINT32 IRER;   
   UINT32 ITER;   
   UINT32 CER; 
   UINT32 CCR; 
   UINT32 RXFFR;
   UINT32 TXFFR;

   UINT32 channel;
   UINT32 ISR;
   UINT32 IMR;
   UINT32 ROR;
   UINT32 LRBR_LTHR;
   UINT32 RRBR_RTHR;
   UINT32 RER;
   UINT32 TER;
   UINT32 RCR;
   UINT32 TCR;
   UINT32 TOR;
   UINT32 RFCR;
   UINT32 TFCR;
   UINT32 RFF;
   UINT32 TFF;

   IER = I2SP_read_reg(I2SP_IER);
   IRER = I2SP_read_reg(I2SP_IRER);
   ITER = I2SP_read_reg(I2SP_ITER);
   CER = I2SP_read_reg(I2SP_CER);
   CCR = I2SP_read_reg(I2SP_CCR);
   RXFFR = I2SP_read_reg(I2SP_RXFFR);
   TXFFR = I2SP_read_reg(I2SP_TXFFR);

   printf("common regs:\n");
   printf("IER(0x%x) = 0x%x\n",I2SP_IER,IER);
   printf("IRER(0x%x) = 0x%x\n",I2SP_IRER,IRER);
   printf("ITER(0x%x) = 0x%x\n",I2SP_ITER,ITER);
   printf("CER(0x%x) = 0x%x\n",I2SP_CER,CER);
   printf("CCR(0x%x) = 0x%x\n",I2SP_CCR,CCR);
   printf("RXFFR(0x%x) = 0x%x\n",I2SP_RXFFR,RXFFR);
   printf("TXFFR(0x%x) = 0x%x\n",I2SP_TXFFR,TXFFR);
   printf("PARAM_2(0x%x) = 0x%x, PARAM_1(0x%x) = 0x%x, I2S_COMP_VERSION(0x%x) = 0x%x, I2S_COMP_TYPE(0x%x) = 0x%x\n",
            I2S_COMP_PARAM_2,I2SP_read_reg(I2S_COMP_PARAM_2),
            I2S_COMP_PARAM_1,I2SP_read_reg(I2S_COMP_PARAM_1),
            I2S_COMP_VERSION,I2SP_read_reg(I2S_COMP_VERSION),
            I2S_COMP_TYPE,   I2SP_read_reg(I2S_COMP_TYPE));

   for (channel = 0; channel < I2SP_NUM_OF_HW_CHAN; channel++)
   {
      ISR = I2SP_read_reg(I2SP_ISR(channel));
      IMR = I2SP_read_reg(I2SP_IMR(channel));
      ROR = I2SP_read_reg(I2SP_ROR(channel));
      LRBR_LTHR = I2SP_read_reg(I2SP_LRBR_LTHR(channel));
      RRBR_RTHR = I2SP_read_reg(I2SP_RRBR_RTHR(channel));
      RER = I2SP_read_reg(I2SP_RER(channel));
      TER = I2SP_read_reg(I2SP_TER(channel));
      RCR = I2SP_read_reg(I2SP_RCR(channel));
      TCR = I2SP_read_reg(I2SP_TCR(channel));
      TOR = I2SP_read_reg(I2SP_TOR(channel));
      RFCR = I2SP_read_reg(I2SP_RFCR(channel));
      TFCR = I2SP_read_reg(I2SP_TFCR(channel));
      RFF = I2SP_read_reg(I2SP_RFF(channel));
      TFF = I2SP_read_reg(I2SP_TFF(channel));

      printf("channels reg: (channel = %d):\n",channel);
      printf("ISR(0x%x) = 0x%x\n",I2SP_ISR(channel),ISR);
      printf("IMR(0x%x) = 0x%x\n",I2SP_IMR(channel),IMR);
      printf("ROR(0x%x) = 0x%x\n",I2SP_ROR(channel),ROR);
      printf("LRBR_LTHR(0x%x) = 0x%x\n",I2SP_LRBR_LTHR(channel),LRBR_LTHR);
      printf("RRBR_RTHR(0x%x) = 0x%x\n",I2SP_RRBR_RTHR(channel),RRBR_RTHR);
      printf("RER(0x%x) = 0x%x\n",I2SP_RER(channel),RER);
      printf("TER(0x%x) = 0x%x\n",I2SP_TER(channel),TER);
      printf("RCR(0x%x) = 0x%x\n",I2SP_RCR(channel),RCR);
      printf("TCR(0x%x) = 0x%x\n",I2SP_TCR(channel),TCR);
      printf("TOR(0x%x) = 0x%x\n",I2SP_TOR(channel),TOR);
      printf("RFCR(0x%x) = 0x%x\n",I2SP_RFCR(channel),RFCR);
      printf("TFCR(0x%x) = 0x%x\n",I2SP_TFCR(channel),TFCR);
      printf("RFF(0x%x) = 0x%x\n",I2SP_RFF(channel),RFF);
      printf("TFF(0x%x) = 0x%x\n",I2SP_TFF(channel),TFF);
   }

   printf("\n\n");
}

static void I2SP_disable_channels()
{
   UINT32 i = 0;

   if (I2SP_dataBase.mode == I2SP_MODULE_MASTER_MODE_E) 
   {
      for (i = 0; i < I2SP_NUM_OF_HW_CHAN; i++)
         I2SP_write_reg(I2SP_TER(i), 0);
   } 
   else 
   {
      for (i = 0; i < I2SP_NUM_OF_HW_CHAN; i++)
         I2SP_write_reg(I2SP_RER(i), 0);
   }
}


/*static void I2SP_clear_irqs(I2SG_modeE mode)
{
   UINT32 i = 0;

   if (mode == I2SP_MODULE_MASTER_MODE_E) 
   {
      for (i = 0; i < 4; i++)
         I2SP_read_reg(I2SP_TOR(i));
   } 
   else 
   {
      for (i = 0; i < 4; i++)
         I2SP_read_reg(I2SP_ROR(i));
   }
}*/

static void I2SP_disable_irqs(I2SG_modeE mode, UINT32 chan_nr)
{
   UINT32 i, irq;

   if (mode == I2SP_MODULE_MASTER_MODE_E) 
   {
      for (i = 0; i < (chan_nr / 2); i++) 
      {
         irq = I2SP_read_reg(I2SP_IMR(i));
         I2SP_write_reg(I2SP_IMR(i), irq | 0x30);
      }
   } 
   else 
   {
      for (i = 0; i < (chan_nr / 2); i++) 
      {
         irq = I2SP_read_reg(I2SP_IMR(i));
         I2SP_write_reg(I2SP_IMR(i), irq | 0x03);
      }
   }
}

static void I2SP_enable_irqs(I2SG_modeE mode, UINT32 chan_nr)
{
   UINT32 i, irq;

   if (mode == I2SP_MODULE_MASTER_MODE_E) 
   {
      for (i = 0; i < (chan_nr / 2); i++) 
      {
         irq = I2SP_read_reg(I2SP_IMR(i));
         I2SP_write_reg(I2SP_IMR(i), irq & ~0x30);
      }
   } 
   else 
   {
      for (i = 0; i < (chan_nr / 2); i++) 
      {
         irq = I2SP_read_reg(I2SP_IMR(i));
         I2SP_write_reg(I2SP_IMR(i), irq & ~0x03);
      }
   }
}

static void I2SP_config(UINT32 xfer_resolution, UINT32 fifo_th)
{
   UINT32 ch_reg;

   I2SP_disable_channels();

   for (ch_reg = 0; ch_reg < I2SP_NUM_OF_HW_CHAN; ch_reg++) 
   {
      if (I2SP_dataBase.mode == I2SP_MODULE_MASTER_MODE_E) 
      {
         I2SP_write_reg(I2SP_TCR(ch_reg), xfer_resolution);
         I2SP_write_reg(I2SP_TFCR(ch_reg),fifo_th - 1);
         I2SP_write_reg(I2SP_TER(ch_reg), 1);
      } 
      else 
      {
         I2SP_write_reg(I2SP_RCR(ch_reg), xfer_resolution);
         I2SP_write_reg(I2SP_RFCR(ch_reg),fifo_th - 1);
         I2SP_write_reg(I2SP_RER(ch_reg), 1);
      }

   }
}


static void I2SP_hw_params(I2SP_formatE format, UINT32 rate)
{
   UINT32 xfer_resolution=0,data_width,ccr = 0;

   switch (format) {
   case I2SP_FORMAT_S16_LE:
      data_width = 16;
      ccr = 0x00;
      xfer_resolution = 0x02;
      break;

   case I2SP_FORMAT_S24_LE:
      data_width = 24;
      ccr = 0x08;
      xfer_resolution = 0x04;
      break;

   case I2SP_FORMAT_S32_LE:
      data_width = 32;
      ccr = 0x10;
      xfer_resolution = 0x05;
      break;

   }

   FIX_UNUSED_PARAM_WARN(rate);
   FIX_UNUSED_PARAM_WARN(data_width);

   I2SP_config(xfer_resolution,8);

   I2SP_write_reg(I2SP_CCR, ccr);
}

#if 0
static void I2SP_isr(UINT64 timestamp, UINT64 count, void *argP)
{
   FIX_UNUSED_PARAM_WARN(timestamp);
   (void)count;(void)argP;
   UINT32 isr;

   isr = I2SP_read_reg(I2SP_ISR(0));

   if (I2SP_dataBase.mode == I2SP_MODULE_MASTER_MODE_E)
   {
#ifdef I2SP_TEST_MODE
      I2SP_write_reg(I2SP_IER,0);

      printf("received I2S interrupt (tx done). isr = 0x%x, ts = %llu\n",isr,timestamp);

      I2SP_write_reg(I2SP_LRBR_LTHR(0),0xAAAAAAAA);
      I2SP_write_reg(I2SP_RRBR_RTHR(0),0xBBBBBBBB);
      I2SP_write_reg(I2SP_LRBR_LTHR(0),0xCCCCCCCC);
      I2SP_write_reg(I2SP_RRBR_RTHR(0),0xDDDDDDDD);
      I2SP_write_reg(I2SP_LRBR_LTHR(0),0xEEEEEEEE);
      I2SP_write_reg(I2SP_RRBR_RTHR(0),0xFFFFFFFF);
      I2SP_write_reg(I2SP_IER,1);
#endif
   }
   else
   {
      if ((isr & 0x2) == 0x2)
      {
         printf("rx fifo overflow!,isr = 0x%x, ts = %llu\n",isr,timestamp);
         I2SP_disable_channels();
         I2SP_write_reg(I2SP_RFF(0),0x1);
         I2SP_hw_params(I2SP_FORMAT,0);
      }
   }

   I2SP_clear_irqs(I2SP_dataBase.mode);
}
#endif
#ifdef I2SP_TEST_MODE
static void I2SP_dmaCb(CDE_MNGRG_chanCbParamsT *chanCbParamsP, void *argsP)
{
   UINT32 index;
   UINT8* buf;

   FIX_UNUSED_PARAM_WARN(argsP);

   printf("\n%llu, dma ISR, data(%p,%d) = ",chanCbParamsP->timeStamp,
         chanCbParamsP->currBufDescP->dataP,chanCbParamsP->currBufDescP->dataLen);

   CMEM_cacheInv(chanCbParamsP->currBufDescP->dataP,chanCbParamsP->currBufDescP->dataLen);
   buf = (UINT8*)chanCbParamsP->currBufDescP->dataP;

   for (index = 0; index < chanCbParamsP->currBufDescP->dataLen; index++)
   {
      printf("%x",buf[index]);
   }

   printf("\n");

   MEM_POOLG_free(chanCbParamsP->currBufDescP);
}
#endif

static ERRG_codeE I2SP_configDma()
{
#if 1
   ERRG_codeE retCode = (ERRG_codeE)ERR_NOT_SUPPORTED;
#else
   ERRG_codeE retCode = (ERRG_codeE)0;
   CDE_DRVG_ioctlCmdParamsU  dmaParams;
   UINT32 phyAddr,virAddr;
// CDE_DRVG_printChDebufInfoIoctlParamsT printChDebugInfoCmdParamsP;
   MEM_POOLG_cfgT                   poolCfg;

   /* create mem_pool */
   poolCfg.bufferSize[0] = I2SP_DMA_TRANSFER_WIDTH;
   poolCfg.numBuffers[0] = I2SP_DMA_NUM_BUFFERS;
   poolCfg.freeCb        = NULL;
   poolCfg.resetBufPtrInAlloc = FALSE;

   if(I2SP_dataBase.poolH == NULL)
   {
      MEM_POOLG_initPool(&I2SP_dataBase.poolH, MEM_POOLG_TYPE_USER_E, &poolCfg);
   }

   if(I2SP_dataBase.poolH == NULL)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,  "ERROR: Failed to init memory pool for i2s\n");
      retCode = ERR_UNEXPECTED;
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      I2SP_dataBase.bufferIndex = MEM_MNGRG_bufferOpen(I2SP_DMA_TRANSFER_WIDTH * I2SP_DMA_NUM_BUFFERS);
      MEM_MNGRG_getBufferAddr(I2SP_dataBase.bufferIndex, &phyAddr, &virAddr);

      dmaParams.chOpenCmdParams.dmaChanId        = INU_DEFSG_SYS_CH_15_I2S_ID_E;
      dmaParams.chOpenCmdParams.isLut            = 0;
      dmaParams.chOpenCmdParams.coreSelect       = CDE_DRVG_CORE_ANY_E;
      retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_CDE_E), CDE_DRVG_CH_OPEN_IOCTL_CMD_E, &dmaParams);
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      dmaParams.gpChSetParamsCmdParams.newDmaChTransferParams.srcAddr       = 0x081C0000 + I2SP_RXDMA;
      dmaParams.gpChSetParamsCmdParams.newDmaChTransferParams.dstAddr       = phyAddr;
      dmaParams.gpChSetParamsCmdParams.newDmaChTransferParams.interruptOpt  = CDE_DRVG_INTERRUPT_EACH_TRANSFER_E;
      dmaParams.gpChSetParamsCmdParams.newDmaChTransferParams.transferMode  = CDE_DRVG_1D_TRANSFER_MODE_E;
      dmaParams.gpChSetParamsCmdParams.newDmaChTransferParams.transferWidth = I2SP_DMA_TRANSFER_WIDTH;
      dmaParams.gpChSetParamsCmdParams.newDmaChTransferParams.transferHeight= 1;
      dmaParams.gpChSetParamsCmdParams.newDmaChTransferParams.rxPeriphId    = IDMA_200_RX_PERIPH_CLIENT_23_E;
      dmaParams.gpChSetParamsCmdParams.newDmaChTransferParams.txPeriphId    = IDMA_200_TX_PERIPH_CLIENT_SRAM_E;
      retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_CDE_E), CDE_DRVG_GP_CH_SET_PARAMS_IOCTL_CMD_E, &dmaParams);
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = CDE_MNGRG_activateCdeChannel(INU_DEFSG_SYS_CH_15_I2S_ID_E);
   }


// printChDebugInfoCmdParamsP.dmaChanId = INU_DEFSG_SYS_CH_15_I2S_ID_E;
// IO_PALG_ioctl(IO_PALG_getHandle(IO_CDE_E), CDE_DRVG_PRINT_CH_DEBUG_INFO_IOCTL_CMD_E, &printChDebugInfoCmdParamsP);
#endif
   return retCode;
}


static void I2SP_enable(I2SG_modeE mode)
{
   ERRG_codeE retCode = (ERRG_codeE)0;
#ifdef I2SP_TEST_MODE
   UINT32     dummy;
#endif

   /* turn on the I2S devices and config pinmux */
   if (mode == I2SP_MODULE_MASTER_MODE_E)
   {
      GME_MNGRG_I2SconfigMaster();
   }
   else
   {
      GME_MNGRG_I2Sconfigslave();
   }

   /* disable the device incase its open from previous reset */
   I2SP_write_reg(I2SP_IER,0);
   I2SP_disable_channels();
   I2SP_write_reg(I2SP_RFF(0),0x1);
   I2SP_disable_irqs(mode,2);

   /* config DMA I2S channel for slave mode*/
   if (mode == I2SP_MODULE_SLAVE_MODE_E)
   {
#ifdef I2SP_TEST_MODE
      retCode = CDE_MNGRG_registerChannelCb(I2SP_dmaCb, NULL, INU_DEFSG_SYS_CH_15_I2S_ID_E, (INT32*)&dummy);
      if(ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_ERROR_E, retCode, "failed to register to I2S DMA channel\n");
         return;
      }
#endif
      retCode = I2SP_configDma();
      if(ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_ERROR_E, retCode, "failed to configure DMA!\n");
      }
   }

   /* Register I2S interrupt */
//   OS_LYRG_intCtrlRegister(OS_LYRG_INT_I2S_E, I2SP_isr, NULL);
//   OS_LYRG_intCtrlEnable(OS_LYRG_INT_I2S_E); 

   /* config the device for 32bits format and enable the interrupts*/
   I2SP_hw_params(I2SP_FORMAT,0 /* unused */);
   I2SP_enable_irqs(mode,2);

   /* enable the device */
   I2SP_write_reg(I2SP_IER,1);
   
   if (mode == I2SP_MODULE_MASTER_MODE_E)
   {
      I2SP_write_reg(I2SP_ITER, 1);
   }
   else
   {
      I2SP_write_reg(I2SP_IRER, 1);
   }
   
   I2SP_write_reg(I2SP_CER, 1);
}


/****************************************************************************
*
*  Function Name: I2SG_getDmaDoneBufDesc
*
*  Description: open I2S module.
*   
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE I2SG_getDmaDoneBufDesc(UINT32 currentBufferIndex, MEM_POOLG_bufDescT **bufDescP)
{
   UINT32     virtAddr=0;//,dummy;
   ERRG_codeE ret = RET_SUCCESS;

   //MEM_MNGRG_getBufferAddr(I2SP_dataBase.bufferIndex, &dummy, &virtAddr);--fixme
   virtAddr = (virtAddr + (I2SP_DMA_TRANSFER_WIDTH * currentBufferIndex));

   ret = MEM_POOLG_alloc(I2SP_dataBase.poolH, MEM_POOLG_getBufSize(I2SP_dataBase.poolH), bufDescP);
   if(ERRG_SUCCEEDED(ret))
   {
      (*bufDescP)->dataP   = (void*)virtAddr;
      (*bufDescP)->dataLen = I2SP_DMA_TRANSFER_WIDTH;
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"I2S get DMA done buf desc - out of descriptors\n");
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: I2SG_open
*
*  Description: open I2S module.
*   
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE I2SG_open(UINT32 i2sRegistersBaseAddr, I2SG_modeE mode)
{
   ERRG_codeE             i2sDrvOpenRetCode = RET_SUCCESS;
   UINT32 compVer;

   if(I2SP_dataBase.moduleStatus == I2SP_MODULE_STATUS_CLOSE_E)
   {
      // init I2S registers base address
      if(i2sRegistersBaseAddr != 0)
      {
         I2SP_dataBase.deviceBaseAddr = i2sRegistersBaseAddr;
         I2SP_dataBase.mode           = mode;

         compVer = I2SP_read_reg(I2S_COMP_VERSION);
         LOGG_PRINT(LOG_INFO_E,NULL,"Comp version = 0x%x, mode = %s\n",compVer, mode == 0 ? "MASTER":"SLAVE");
         I2SP_enable(mode);
         I2SP_dump_reg();

         I2SP_dataBase.moduleStatus = I2SP_MODULE_STATUS_OPEN_E;
      }
      else
      {
         i2sDrvOpenRetCode = ERR_UNEXPECTED;
      }
   }
   else
   {
      i2sDrvOpenRetCode = ERR_UNEXPECTED;
   }

   return(i2sDrvOpenRetCode);
}


/****************************************************************************
*
*  Function Name: I2SG_close
*
*  Description: close I2S module. init module data base and change the module status
*   
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE I2SG_close()
{
   if(I2SP_dataBase.moduleStatus == I2SP_MODULE_STATUS_OPEN_E)
   {
      I2SP_write_reg(I2SP_IER,0);
      I2SP_disable_channels();
      I2SP_write_reg(I2SP_RFF(0),0x1);
      I2SP_disable_irqs(I2SP_dataBase.mode,2);
      I2SP_dataBase.moduleStatus = I2SP_MODULE_STATUS_CLOSE_E;
   }

   return(RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: I2SG_init
*
*  Description: init of I2S module.
*           you can't access this module without calling this function
*   
*  Inputs:
*
*  Outputs:
*
*  Returns: 
*
*  Context: 
*
****************************************************************************/
ERRG_codeE I2SG_init()
{
   ERRG_codeE i2sDrvInitRetCode = RET_SUCCESS;

   I2SP_dataBase.moduleStatus = I2SP_MODULE_STATUS_CLOSE_E;
   I2SP_dataBase.poolH        = NULL;

   return(i2sDrvInitRetCode);
}


#ifdef __cplusplus
  }
#endif

