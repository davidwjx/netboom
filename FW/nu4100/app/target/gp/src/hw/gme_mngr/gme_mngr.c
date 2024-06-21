/****************************************************************************
 *
 *   FileName: gme_mngr.c
 *
 *   Author:  Benny V.
 *
 *   Date:
 *
 *   Description: gme control
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "gme_mngr.h"
#include "gme_drv.h"
#include "gpio_drv.h"
#include "dw_umctl2.h"
#include "inu2.h"
#include "i2c_hl_drv.h"
#include "helsinki.h"
#if (DEFSG_PROCESSOR == DEFSG_GP)
#include "mipi_mngr.h"
#include "hw_regs.h"
#endif
/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static INU_DEFSG_moduleTypeE  modelType;
static INU_DEFSG_baseVersionE baseVersion;


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
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/


/****************************************************************************
***************     G L O B A L         F U N C T I O N S     **************
****************************************************************************/

/****************************************************************************
*
*  Function Name: GME_MNGRG_open
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: gme control
*
****************************************************************************/
ERRG_codeE GME_MNGRG_open()
{
   ERRG_codeE           retCode = GME_MNGR__RET_SUCCESS;
   IO_HANDLE            gpioHandle;

   LOGG_PRINT(LOG_INFO_E, NULL, "Open GPIO Driver \n");
   retCode = IO_PALG_open(&gpioHandle,IO_GPIO_E, NULL);  // open device and load pinmux configuration.
   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "open GPIO driver fail.\n");
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Open DSE Driver \n");
      retCode = DW_MCTLG_init();
   }

   return(retCode);
}

/****************************************************************************
*
*  Function Name: GME_MNGRG_close
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: gme control
*
****************************************************************************/
ERRG_codeE GME_MNGRG_close()
{
   ERRG_codeE  retCode = GME_MNGR__RET_SUCCESS;

   //IO_PALG_close(IO_PALG_getHandle(IO_GME_E));
   IO_PALG_close(IO_PALG_getHandle(IO_GPIO_E));

   return(retCode);
}

/****************************************************************************
*
*  Function Name: GME_MNGRG_init
*
*  Description: reset system HW by writing to GPIO register
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: gme control
*
****************************************************************************/
ERRG_codeE GME_MNGRG_init(UINT32 memVirtAddr)
{
   ERRG_codeE retCode = GME_MNGR__RET_SUCCESS;

   retCode = GME_DRVG_init(memVirtAddr);
/*
   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = GME_MNGRG_powerUp(usbSpeed);
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = GME_MNGRG_resetHw(TRUE);
   }
*/
   return(retCode);
}

/****************************************************************************
*
*  Function Name: GME_MNGRG_resetHw
*
*  Description: reset system HW by writing to GPIO register
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: gme control
*
****************************************************************************/
ERRG_codeE GME_MNGRG_resetHw(BOOL resetI2c)
{
   ERRG_codeE  ret = GME_MNGR__RET_SUCCESS;

   LOGG_PRINT(LOG_INFO_E, NULL, "Resetting HW...\n");

#ifndef __KERNEL_I2C__
   if(resetI2c == TRUE)
   {
      // Implement reset units
      LOGG_PRINT(LOG_INFO_E, NULL, "Resetting I2C1 (i2c0)\n");
      ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_I2C1_E);
      if(ERRG_SUCCEEDED(ret))
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "Resetting I2C2 (i2c1)\n");
         ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_I2C2_E);
      }

      if(ERRG_SUCCEEDED(ret))
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "Resetting I2C3 (i2c2)\n");
         ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_I2C3_E);
      }

      if(ERRG_SUCCEEDED(ret))
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "Resetting I2C4 (i2c3)\n");
         ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_I2C4_E);
      }

      if(ERRG_SUCCEEDED(ret))
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "Resetting I2C5 (i2c4)\n");
         ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_I2C4_E);
      }

      if(ERRG_SUCCEEDED(ret))
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "Resetting I2C6 (i2c5)\n");
         ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_I2C4_E);
      }
   }
#else
   FIX_UNUSED_PARAM_WARN(resetI2c);
#endif

   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Resetting IAE\n");
      ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_IAE_E);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      //LOGG_PRINT(LOG_INFO_E, NULL, "Resetting DPE\n");
      ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_DPE_E);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      //LOGG_PRINT(LOG_INFO_E, NULL, "Resetting PPE\n");
      ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_PPE_E);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      //LOGG_PRINT(LOG_INFO_E, NULL, "Resetting DMA\n");
      ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_DMA_0_E);
      ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_DMA_1_E);
      ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_DMA_2_E);
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: GME_MNGRG_powerUp
*
*  Description: power UP system HW by writing to GME registers
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: gme control
*
****************************************************************************/
ERRG_codeE GME_MNGRG_powerUp(int usbSpeed)
{
   ERRG_codeE           retCode;
#ifndef __KERNEL_I2C__
   IO_HANDLE            gmeHandle;
   GME_DRVG_openParamsT gmeOpenParams;
   IO_HANDLE            gpioHandle;
#endif
   GPIO_DRVG_gpioSetDirParamsT   directionParams;
   GPIO_DRVG_gpioOpenParamsT     params;
   GME_DRVG_unitClkDivT          divParams;
   (void)usbSpeed;

   LOGG_PRINT(LOG_INFO_E, NULL, "Power up start\n");

#ifdef POWER_MODE_OPT
   retCode = GME_DRVG_setPowerMode(GME_DRVG_POWER_PSS_E);
   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = GME_DRVG_setPowerMode(GME_DRVG_POWER_IAE_E);
   }
   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = GME_DRVG_setPowerMode(GME_DRVG_POWER_DPE_E);
   }
   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = GME_DRVG_setPowerMode(GME_DRVG_POWER_PPE_E);
      //LOGG_PRINT(LOG_INFO_E, NULL, "Set power active\n");
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = GME_DRVG_setPowerMode(GME_DRVG_POWER_EVP_E);
      //LOGG_PRINT(LOG_INFO_E, NULL, "Set power active\n");
   }

#else
   retCode = GME_DRVG_setPowerMode(GME_DRVG_POWER_ALL_E);
   //LOGG_PRINT(LOG_INFO_E, NULL, "Set power active\n");
#endif

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = GME_DRVG_enableClk(GME_DRVG_HW_UNIT_OSC_E);
      //LOGG_PRINT(LOG_INFO_E, NULL, "Periph clock source = OSC. Enable OSC clock\n");
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = GME_DRVG_enableClk(GME_DRVG_HW_UNIT_GPIO_E);
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable GPIO clock\n");

      /* GPIO3 is used in some systems as the input for timestamp from the stereo strobe.
               The HW line also goes into the projector. The internal pullup of the chip will set this line to
               high and might burn the system. Therefor we set it to input */
      if(ERRG_SUCCEEDED(retCode))
      {
         params.gpioNum   = GPIO_DRVG_GPIO_3_E;
         IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_OPEN_GPIO_CMD_E, &params);
         //LOGG_PRINT(LOG_INFO_E, NULL, "GPIO: Set GPIO %d\n", params.gpioNum);
      }

      if(ERRG_SUCCEEDED(retCode))
      {
         directionParams.direction = GPIO_DRVG_IN_DIRECTION_E;
         directionParams.gpioNum   = GPIO_DRVG_GPIO_3_E;
         IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &directionParams);
         //LOGG_PRINT(LOG_INFO_E, NULL, "GPIO: Set GPIO %d direction to %d\n", directionParams.gpioNum, directionParams.direction);
      }
      if(ERRG_SUCCEEDED(retCode))
      {
         HELSINKI_init();
      }
   }

#ifndef __KERNEL_I2C__
   if(ERRG_SUCCEEDED(retCode))
   {
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable I2C1 (i2c0) clock\n");
      retCode = GME_DRVG_enableClk(GME_DRVG_HW_UNIT_I2C1_E);
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable I2C2 (i2c1) clock\n");
      retCode = GME_DRVG_enableClk(GME_DRVG_HW_UNIT_I2C2_E);
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable I2C3 (i2c2) clock\n");
      retCode = GME_DRVG_enableClk(GME_DRVG_HW_UNIT_I2C3_E);
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable I2C4 (i2c3) clock\n");
      retCode = GME_DRVG_enableClk(GME_DRVG_HW_UNIT_I2C4_E);
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable I2C5 (i2c4) clock\n");
      retCode = GME_DRVG_enableClk(GME_DRVG_HW_UNIT_I2C5_E);
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable I2C6 (i2c5) clock\n");
      retCode = GME_DRVG_enableClk(GME_DRVG_HW_UNIT_I2C6_E);
   }
#endif

   if(ERRG_SUCCEEDED(retCode))
   {
      GME_DRVG_unitClkDivT freqCfg;
      GME_DRVG_ciifClkSrcE ciifClkSrc;

      /*
      if(usbSpeed == INU_DEFSG_USB_3_E)
      {
         ciifClkSrc = GME_DRVG_CIIF_SRC_USB3PLL_E;
         GME_DRVG_enable_usb3_pll();
      }
      else if(usbSpeed == INU_DEFSG_USB_2_E)
      {
         ciifClkSrc = GME_DRVG_CIIF_SRC_AUDIOPLL_E;
      }
      else
      {
         ciifClkSrc = GME_DRVG_CIIF_SRC_DSP_E;
      }
      */

#ifdef POWER_MODE_OPT1
      //ciifClkSrc = GME_DRVG_CIIF_SRC_DSP_E;
      ciifClkSrc = GME_DRVG_CIIF_SRC_AUDIOPLL_E;

      //LOGG_PRINT(LOG_INFO_E, NULL, "Set CIIF clock source to %d\n", ciifClkSrc);
      GME_DRVG_setCiifClkSrc(ciifClkSrc);
      //freqCfg.div = 0x6; //ciif div field - Clk_out = clk_in / (3MSBs + 0.5* 1LSB). setting 0x6 -> clk_out = clk_in/3
      freqCfg.div = 0x5;
      freqCfg.unit = GME_DRVG_HW_UNIT_CIIF_E;
      retCode = GME_DRVG_changeUnitFreq(&freqCfg);
#else
      /* use the USB3 pll to acheive 250Mhz */
      ciifClkSrc = GME_DRVG_CIIF_SRC_USB3PLL_E;
      GME_DRVG_enable_usb3_pll();

      //LOGG_PRINT(LOG_INFO_E, NULL, "Set CIIF clock source to %d\n", ciifClkSrc);
      GME_DRVG_setCiifClkSrc(ciifClkSrc);
      freqCfg.div = 0x3; //ciif div field - Clk_out = clk_in / (3MSBs + 0.5* 1LSB). setting 0x3 -> clk_out = clk_in/1.5
      freqCfg.unit = GME_DRVG_HW_UNIT_CIIF_E;
      retCode = GME_DRVG_changeUnitFreq(&freqCfg);
#endif
      //Enable parallel CIIF clock on 0 and 1
      freqCfg.div = 0x08;
      freqCfg.unit = GME_DRVG_HW_UNIT_CIIF_PAR_0_E;
      retCode = GME_DRVG_setUnitClockDiv(&freqCfg);
      freqCfg.unit = GME_DRVG_HW_UNIT_CIIF_PAR_1_E;
      retCode = GME_DRVG_setUnitClockDiv(&freqCfg);

   }

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode =  GME_DRVG_enableClk(GME_DRVG_HW_CV0_REF_CLK_EN_E);
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable CV_0 REF clock\n");
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode =  GME_DRVG_enableClk(GME_DRVG_HW_CV1_REF_CLK_EN_E);
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable CV_1 REF clock\n");
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode =  GME_DRVG_enableClk(GME_DRVG_HW_CV2_REF_CLK_EN_E);
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable CV_2 REF clock\n");
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode =  GME_DRVG_enableClk(GME_DRVG_HW_CV3_REF_CLK_EN_E);
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable CV_3 REF clock\n");
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode =  GME_DRVG_enableClk(GME_DRVG_HW_UNIT_LRAM_E);
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable LRAM clock\n");
   }
   if(ERRG_SUCCEEDED(retCode))
   {
      retCode =  GME_DRVG_enableClk(GME_DRVG_HW_UNIT_IAE_E);
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable IAE clock\n");
   }
   if(ERRG_SUCCEEDED(retCode))
   {
      retCode =  GME_DRVG_enableClk(GME_DRVG_HW_UNIT_CNN_E);
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable ISP clock\n");
   }
#ifndef __ZEBU__// If Zebu --> 0
   if(ERRG_SUCCEEDED(retCode))
   {

      retCode =  GME_DRVG_enableClk(GME_DRVG_HW_AUDIO_CLK_EN_E);
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable AUDIO clock\n");
   }


   if(ERRG_SUCCEEDED(retCode))
   {
      divParams.div = 4;
      divParams.unit = GME_DRVG_HW_UNIT_FCU_E;
      retCode = GME_DRVG_changeUnitFreq(&divParams);
   }
#endif
#ifndef __ZEBU__// If Zebu --> 0
#ifndef POWER_MODE_OPT
   if(ERRG_SUCCEEDED(retCode))
   {
      retCode =  GME_DRVG_enableClk(GME_DRVG_HW_AUDIO_EXT_CLK_EN_E);
      //LOGG_PRINT(LOG_INFO_E, NULL, "Enable external AUDIO clock\n");
   }
#else
   if(ERRG_SUCCEEDED(retCode))
   {
      INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();

      if (modelType != INU_DEFSG_BOOT500_E)
      {
         ERRG_codeE retCode2;
         I2C_HL_DRVG_readWriteParametersT i2cConfig;
         UINT8 val,retry = 0;

         //boot52 EOM requires higher voltage from PMIC
         if (modelType == INU_DEFSG_BOOT52_E)
         {
            retry = 0;
            i2cConfig.regAddress           = 0x8;
            i2cConfig.tarAddress           = 0xd2;
            i2cConfig.regAddressAccessSize = 1;
            val = 0xD2;

            //OV9287
            do
            {
               retCode2 = IO_PALG_write(IO_PALG_getHandle(IO_I2C_4_E), &val, 1, &i2cConfig);
               retry++;
            } while((retry < 10) && (ERRG_FAILED(retCode2)));
         }
         else if ((modelType >= INU_DEFSG_BOOT20_E) &&  // FPGA out of reset
                     (modelType <= INU_DEFSG_BOOT24_E))
         {
             GPIO_DRVG_gpioSetValParamsT         gpioParams;
             GPIO_DRVG_gpioSetDirParamsT         gpioDir;

             gpioDir.direction = GPIO_DRVG_OUT_DIRECTION_E;
             gpioDir.gpioNum = 22;
             IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);

             gpioParams.val = 1;
             gpioParams.gpioNum =22;
             IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
         }
      }
   }
#endif

   if (modelType == INU_DEFSG_BOOT83_E)//for grogu model
   {
      GPIO_DRVG_gpioSetValParamsT         gpioParams;
      GPIO_DRVG_gpioSetDirParamsT         gpioDir;

      LOGG_PRINT(LOG_INFO_E, NULL, "model 83 - set GPIOs 8, 36, 37\n");

      //gpio8 set - for common i2c3 for both sensors
      gpioDir.direction = GPIO_DRVG_OUT_DIRECTION_E;
      gpioDir.gpioNum = 8;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);

      gpioParams.val = 1;
      gpioParams.gpioNum = 8;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);

      //gpio36 set - for set power to 8M.   8m_ewr_en='1'
      gpioDir.direction = GPIO_DRVG_OUT_DIRECTION_E;
      gpioDir.gpioNum = 36;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);

      gpioParams.val = 1;
      gpioParams.gpioNum = 36;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);

      //gpio37 set - for set power to 5M.     5m_ewr_en='1'
      gpioDir.direction = GPIO_DRVG_OUT_DIRECTION_E;
      gpioDir.gpioNum = 37;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);

      gpioParams.val = 1;
      gpioParams.gpioNum = 37;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   }
#endif
   if ((modelType == INU_DEFSG_BOOT86_E) || (modelType == INU_DEFSG_BOOT87_E))//for si1 model
   {
      GPIO_DRVG_gpioSetValParamsT         gpioParams;
      GPIO_DRVG_gpioSetDirParamsT         gpioDir;

      LOGG_PRINT(LOG_INFO_E, NULL, "model 86 - set GPIOs 8, 25, 26\n");

      //gpio8 set direction-SID = 0
      gpioDir.direction = GPIO_DRVG_OUT_DIRECTION_E;
      gpioDir.gpioNum = 8;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);

      gpioParams.val = 0;
      gpioParams.gpioNum = 8;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);

      //gpio25 set - for set power to 5M.   5m_pwr_en='1'
      gpioDir.direction = GPIO_DRVG_OUT_DIRECTION_E;
      gpioDir.gpioNum = 25;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);

      gpioParams.val = 1;
      gpioParams.gpioNum = 25;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);

      //gpio26 set - for set power to 8M.     8m_pwr_en='1'
      gpioDir.direction = GPIO_DRVG_OUT_DIRECTION_E;
      gpioDir.gpioNum = 26;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);

      gpioParams.val = 1;
      gpioParams.gpioNum = 26;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   }

   if ( (modelType == INU_DEFSG_BOOT41_E) || (modelType == 10041) )
   {
      GPIO_DRVG_gpioSetValParamsT         gpioParams;
      GPIO_DRVG_gpioSetDirParamsT         gpioDirection;

      LOGG_PRINT(LOG_INFO_E, NULL, "model ITB4.1 - set GPIOs 39 as input,44 as output \n");

      gpioDirection.direction = GPIO_DRVG_IN_DIRECTION_E;
      gpioDirection.gpioNum = GPIO_DRVG_GPIO_39_E;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDirection);


      gpioDirection.direction = GPIO_DRVG_OUT_DIRECTION_E;
      gpioDirection.gpioNum = GPIO_DRVG_GPIO_44_E;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDirection);

      gpioParams.val = 0;
      gpioParams.gpioNum = GPIO_DRVG_GPIO_44_E;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   }

   if ((modelType == INU_DEFSG_BOOT85_E) || (modelType == 10085)  || (modelType == INU_DEFSG_BOOT86_E) || (modelType == INU_DEFSG_BOOT87_E))
   {
      GPIO_DRVG_gpioSetValParamsT         gpioParams;
      GPIO_DRVG_gpioSetDirParamsT         gpioDirection;

      LOGG_PRINT(LOG_INFO_E, NULL, "model 85/86/87 - set GPIOs 39,40/59 as input \n");

      gpioDirection.direction = GPIO_DRVG_IN_DIRECTION_E;
      gpioDirection.gpioNum = GPIO_DRVG_GPIO_39_E;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDirection);


      gpioDirection.direction = GPIO_DRVG_IN_DIRECTION_E;
      if ((modelType == INU_DEFSG_BOOT86_E) || (modelType == INU_DEFSG_BOOT87_E))
      {
         gpioDirection.gpioNum = GPIO_DRVG_GPIO_59_E;
      }
      else
      {
         gpioDirection.gpioNum = GPIO_DRVG_GPIO_40_E;
      }
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDirection);

   }
   if (modelType == INU_DEFSG_BOOT87_E)
   {
      GPIO_DRVG_gpioSetValParamsT         gpioParams;
      GPIO_DRVG_gpioSetDirParamsT         gpioDir;
      GPIO_DRVG_gpioOpenParamsT     gpioOpen;

      LOGG_PRINT(LOG_INFO_E, NULL, "model 87 - set GPIO 11 (camera LED) as output \n");

      gpioOpen.gpioNum   = GPIO_DRVG_GPIO_11_E;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_OPEN_GPIO_CMD_E, &gpioOpen);

      gpioDir.direction = GPIO_DRVG_OUT_DIRECTION_E;
      gpioDir.gpioNum = GPIO_DRVG_GPIO_11_E;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);

      gpioParams.val = 1; //close LED as default till stream is on
      gpioParams.gpioNum =GPIO_DRVG_GPIO_11_E;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   }
   if ((modelType == INU_DEFSG_BOOT84_E) || (modelType == 10084) )
   {
      //from HW team information, this gpio in set as default it is problematic state for sensor to be at shoutdown high
      GPIO_DRVG_gpioSetValParamsT         gpioParams;
      GPIO_DRVG_gpioSetDirParamsT         gpioDirection;

      LOGG_PRINT(LOG_INFO_E, NULL, "model 84 - set GPIOs 61 s output in low \n");

      gpioDirection.direction = GPIO_DRVG_OUT_DIRECTION_E;
      gpioDirection.gpioNum = GPIO_DRVG_GPIO_61_E;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDirection);

      gpioParams.val = 0;
      gpioParams.gpioNum = GPIO_DRVG_GPIO_61_E;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);

   }

   if ((modelType == INU_DEFSG_BOOT310_E) || (modelType == 10310) || (modelType == INU_DEFSG_BOOT311_E) || (modelType == 10311))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Model 1031x: PMIC set reg 0x03 to 0xCA\n");
      HW_REGSG_writeI2cReg(4, 0x03, 0xd2, 1, 1, 0xCA);

      LOGG_PRINT(LOG_INFO_E, NULL, "Model 1031x: PMIC set reg 0x04 to 0x92\n");
      HW_REGSG_writeI2cReg(4, 0x04, 0xd2, 1, 1, 0x92);
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "HW power up done\n");
   }


   return retCode;
}

/****************************************************************************
*
*  Function Name: GME_MNGRG_powerDown
*
*  Description: power UP system HW by writing to  GME registers
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: gme control
*
****************************************************************************/
ERRG_codeE GME_MNGRG_powerDown()
{
   ERRG_codeE                 retCode= GME_MNGR__RET_SUCCESS;
   GPIO_DRVG_gpioCloseParamsT params;

   params.gpioNum   = GPIO_DRVG_GPIO_3_E;
   IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_CLOSE_GPIO_CMD_E, &params);
   LOGG_PRINT(LOG_DEBUG_E, NULL, "GPIO: Set GPIO %d\n", params.gpioNum);

   if(ERRG_SUCCEEDED(retCode))
      retCode = GME_MNGRG_close();
   return retCode;
}

/****************************************************************************
*
*  Function Name: GME_MNGRG_enableCevaClk
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/
ERRG_codeE GME_MNGRG_enableCevaClk()
{
   ERRG_codeE        retCode = GME_MNGR__RET_SUCCESS;

 //  retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GME_E), GME_DRVG_ENABLE_CLK_CMD_E, (void *)GME_DRVG_HW_UNIT_CEVA_E);

   return retCode;
}

/****************************************************************************
*
*  Function Name: GME_MNGRG_disableCevaClk
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/
ERRG_codeE GME_MNGRG_disableCevaClk()
{
   ERRG_codeE        retCode = GME_MNGR__RET_SUCCESS;

  // retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GME_E), GME_DRVG_DISABLE_CLK_CMD_E, (void *)GME_DRVG_HW_UNIT_CEVA_E);

   return retCode;
}

/****************************************************************************
*
*  Function Name: GME_MNGRG_resetCevaUnit
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/
ERRG_codeE GME_MNGRG_resetCevaUnit()
{
   ERRG_codeE        retCode = GME_MNGR__RET_SUCCESS;

 //  retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GME_E), GME_DRVG_RESET_UNIT_CMD_E, (void *)GME_DRVG_HW_UNIT_CEVA_E);

   return retCode;
}


/****************************************************************************
*
*  Function Name: GME_MNGRG_I2Sconfigslave
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/
void GME_MNGRG_I2Sconfigslave()
{
   GME_DRVG_setIoMux(GME_DRVG_HW_AUDIO_CLK_EN_E);
}

/****************************************************************************
*
*  Function Name: GME_MNGRG_I2SconfigMaster
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/
void GME_MNGRG_I2SconfigMaster()
{
   GME_DRVG_unitClkDivT                gmeClkConfig;

   GME_DRVG_setIoMux(GME_DRVG_HW_I2S_CLK_EN_E);
   GME_DRVG_setIoMux(GME_DRVG_HW_I2S_CLK_EN_E);

   gmeClkConfig.unit = GME_DRVG_HW_I2S_CLK_EN_E;
   gmeClkConfig.div  = 15; //will give 1.6mhz
   GME_DRVG_changeUnitFreq(&gmeClkConfig);
}


/****************************************************************************
*
*  Function Name: GME_MNGRG_setUART0MipiClkSrc
*
*  Description: To acheive higher rates for UART, clk needs to be taken from CPU or AUDIO.
*                    Since Audio PLL is not always turned on, we take from CPU.
*                    The UART clk needs to be 29.4912 , so CPU clk needs to be multipile of 29.4912
*
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/
ERRG_codeE GME_MNGRG_setUART0ClkSrc()
{
   ERRG_codeE           retVal = GME_MNGR__RET_SUCCESS;

   GME_DRVG_unitClkDivT freqCfg;


   // Set UART clk src from CPU
   if(ERRG_SUCCEEDED(retVal))
   {
      retVal = GME_DRVG_setUartClkSrc(1);
   }

   // change divider for UART0 ( 973.2096 / 33 = 29.4912 )
   if(ERRG_SUCCEEDED(retVal))
   {
      freqCfg.div = 33;
      freqCfg.unit = GME_DRVG_HW_UNIT_UART0_E;
      retVal = GME_DRVG_changeUnitFreq(&freqCfg);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to set UART clk src\n");
   }

   if(ERRG_SUCCEEDED(retVal))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Set UART0 mipi as clk src complete\n");
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to change UART freq\n");
   }

   return retVal;
}


/****************************************************************************
*
*  Function Name: GME_MNGRG_getHwVersion
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/
UINT32 GME_MNGRG_getHwVersion()
{
   UINT32 hwVersion;

   GME_DRVG_getVersion((void *)&hwVersion);

   return(hwVersion);
}

/****************************************************************************
*
*  Function Name: GME_MNGRG_setModelType
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/
void GME_MNGRG_setModelType(INU_DEFSG_moduleTypeE modelTypeNewVal)
{
   modelType = modelTypeNewVal;
}
/****************************************************************************
*
*  Function Name: GME_MNGRG_getModelType
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/
INU_DEFSG_moduleTypeE GME_MNGRG_getModelType(void)
{
   // Model Type contains the Base Version, and the Boot ID in this format:
   // X0YYY
   // X is the ((Base Version - 1) * 10,000)
   // Y is the Boot ID
   // For example:
   // Base Version #1 and Boot ID #60 is = 60
   // Base Version #2 and Boot ID #65 is = 10065
   // Base Version #3 and Boot ID #51 is = 20051
   // Therefore, in order to get just the Boot ID, need to use modulus by 10,000
   return (modelType % INU_DEFSG_BOOT_FACTOR);
}

/****************************************************************************
*
*  Function Name: GME_MNGRG_getFullModelType
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/

INU_DEFSG_moduleTypeE GME_MNGRG_getFullModelType(void)
{
   // Retuns the full modelType, including the Base Version
   return modelType;
}

/****************************************************************************
*
*  Function Name: GME_MNGRG_setBaseVersion
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/
void GME_MNGRG_setBaseVersion(INU_DEFSG_baseVersionE baseVersionNewVal)
{
   baseVersion = baseVersionNewVal;
}
/****************************************************************************
*
*  Function Name: GME_MNGRG_getBaseVersion
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/

INU_DEFSG_baseVersionE GME_MNGRG_getBaseVersion(void)
{
   return baseVersion;
}

ERRG_codeE GME_MNGRG_resetIdve( bool cdeReset )
{
   ERRG_codeE  ret = GME_MNGR__RET_SUCCESS;
   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Resetting IAE\n");
      ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_IAE_E);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Resetting DPE\n");
      ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_DPE_E);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Resetting PPE\n");
      ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_PPE_E);
   }

   if((ERRG_SUCCEEDED(ret)) && (cdeReset))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Resetting CDE\n");
      ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_DMA_0_E);
      ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_DMA_1_E);
      ret = GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_DMA_2_E);
   }

   return ret;
}

void GME_MNGRG_shutterSeq()
{
   GME_DRVG_shutterSeq();
   return;
}

#ifdef __cplusplus
   }
#endif

