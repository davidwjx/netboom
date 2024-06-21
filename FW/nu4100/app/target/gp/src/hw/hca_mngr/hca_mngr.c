/****************************************************************************
*
*   FileName: hca_mngr.c
*
*   Author:  Arnon C.
*
*   Date:
*
*   Description: Hardware Clock Adjust manager.
*
****************************************************************************/

/****************************************************************************
***************               I N C L U D E   F I L E S        *************
****************************************************************************/
#include "inu_common.h"
#include "assert.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "hca_mngr.h"
#include "gme_mngr.h"
#include "gme_drv.h"
#include "log.h"

/****************************************************************************
***************         L O C A L       D E F N I T I O N S  ***************
****************************************************************************/
//#define HCA_MNGRP_DEBUG
   
#define HCA_MNGRP_MAX_CONSUMER_NUM           (12)
#define HCA_MNGRP_MAX_NUMBER_CIIF_DIV_VALUES ((0xF) - 2)//divider has 4 bits, zero&1 are not valid value
#define HCA_MNGRP_MAX_NUMBER_CIIF_CLKS       (GME_DRVG_CIIF_SRC_MAX_NUM_E * HCA_MNGRP_MAX_NUMBER_CIIF_DIV_VALUES) // 3 sources dsp,usb2,usb3. 

/****************************************************************************
***************            L O C A L    T Y P E D E F S      ***************
****************************************************************************/

typedef struct
{
   char      *name;
   UINT32    hwUnitsUsed[HCA_MNGRG_MAX_NUM_HW_UNITS];
   UINT32    inputPixelClockMhz;
   UINT32    currentRequiredClockHz;
} HCA_MNGRP_consumerParamsT;


typedef struct
{
   HCA_MNGRP_consumerParamsT      consumersTable[HCA_MNGRP_MAX_CONSUMER_NUM];
   /* possibleCiifFreqs table hold all the possible ciif clocks, the source & divider requires to reach it */
   UINT32                         possibleCiifFreqs[HCA_MNGRP_MAX_NUMBER_CIIF_CLKS][3];
   UINT32                         maxCiifClockRequired;
   UINT32                         currentCiifClockTableIndex;
   UINT32                         disableFlag;
} HCA_MNGRP_paramsT;


/****************************************************************************
***************       L O C A L         D A T A              ***************
****************************************************************************/
static HCA_MNGRP_paramsT hcaParams;

/****************************************************************************
***************     P R E    D E F I N I T I O N     OF      ***************
***************     L O C A L         F U N C T I O N S      ***************
****************************************************************************/

/****************************************************************************
***************      L O C A L       F U N C T I O N S       ***************
****************************************************************************/

/****************************************************************************
*
*  Function Name: HCA_MNGRP_ciifSrcToString
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
static char *HCA_MNGRP_ciifSrcToString(GME_DRVG_ciifClkSrcE ciifSrc)
{
   switch(ciifSrc)
   {
      case(GME_DRVG_CIIF_SRC_AUDIOPLL_E):
         return "AUDIO";
      break;

      case(GME_DRVG_CIIF_SRC_USB3PLL_E):
         return "USB3";
      break;

      case(GME_DRVG_CIIF_SRC_DSP_E):
         return "DSP";
      break;

      default:
         return "";
         assert(0);         
   }
}


/****************************************************************************
*
*  Function Name: HCA_MNGRP_hwUnitToString
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
static char *HCA_MNGRP_hwUnitToString(UINT32 hwUnit)
{
   switch(hwUnit)
   {
      case(HCA_MNGRG_IAE):
         return "IAE";
      break;

      case(HCA_MNGRG_DPE):
         return "DPE";
      break;

      case(HCA_MNGRG_PPE):
         return "PPE";
      break;

      case(HCA_MNGRG_CVA):
         return "CVA";
      break;

      default:
         return "";
         assert(0);         
   }
}



/****************************************************************************
*
*  Function Name: HCA_MNGRP_ciifSrcToPllType
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
static GME_DRVG_PLLTypesE HCA_MNGRP_ciifSrcToPllType(GME_DRVG_ciifClkSrcE ciifSrc)
{
   GME_DRVG_PLLTypesE pllType;
   switch(ciifSrc)
   {
      case(GME_DRVG_CIIF_SRC_AUDIOPLL_E):
         pllType = GME_DRVG_PLL_AUDIO_E;
      break;

      case(GME_DRVG_CIIF_SRC_USB3PLL_E):
         pllType = GME_DRVG_PLL_USB3_E;
      break;

      case(GME_DRVG_CIIF_SRC_DSP_E):
         pllType = GME_DRVG_PLL_DSP_E;
      break;

      default:
         pllType = GME_DRVG_PLL_USB2_E;
         assert(0);         
   }
   
   return pllType;
}

/****************************************************************************
***************     G L O B A L         F U N C T I O N S     **************
****************************************************************************/

/****************************************************************************
*
*  Function Name: HCA_MNGRG_calcRequiredCiifClock
*
*  Description: The function will process the registered consumers, and decide a suitable
*                   CIIF frequency
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE HCA_MNGRG_calcRequiredCiifClock( void )
{
   ERRG_codeE retCode = HW_MNGR__RET_SUCCESS;
   UINT32 pixelClockFactor = 1, i, maxCiifClockRequired = 0, consumerRequiredCiifClock,cvaLimitMaxFreq=0;
   GME_DRVG_unitClkDivT freqCfg;
   GME_DRVG_ciifClkSrcE ciifClkSrc;

   if (hcaParams.disableFlag)
   {
      return retCode;
   }

   //find the maximum required ciif clock from all the active consumers   
   for (i = 0; i < HCA_MNGRP_MAX_CONSUMER_NUM; i++)
   {
      //consumer is active
      if (hcaParams.consumersTable[i].name)
      {
         consumerRequiredCiifClock = hcaParams.consumersTable[i].inputPixelClockMhz;

         //check if IAE is active (SLU 1:2 ratio) or CVA
         if (hcaParams.consumersTable[i].hwUnitsUsed[HCA_MNGRG_IAE] || 
             hcaParams.consumersTable[i].hwUnitsUsed[HCA_MNGRG_CVA])
         {
            consumerRequiredCiifClock = consumerRequiredCiifClock * 2;
           
         }
         if (hcaParams.consumersTable[i].hwUnitsUsed[HCA_MNGRG_CVA])
            cvaLimitMaxFreq = 1;
             
         //check if DPE
         if (hcaParams.consumersTable[i].hwUnitsUsed[HCA_MNGRG_DPE])
         {
            consumerRequiredCiifClock = consumerRequiredCiifClock * 2;
         }

         hcaParams.consumersTable[i].currentRequiredClockHz = consumerRequiredCiifClock * 1000000; //save for stats
#ifdef HCA_MNGRP_DEBUG
         if (consumerRequiredCiifClock > maxCiifClockRequired)
         {
            LOGG_PRINT(LOG_INFO_E, NULL, "new max freq by %s: %d\n", hcaParams.consumersTable[i].name,consumerRequiredCiifClock); 
         }
#endif
         maxCiifClockRequired = (consumerRequiredCiifClock > maxCiifClockRequired) ? consumerRequiredCiifClock : maxCiifClockRequired;
      }
   }

   //Add safety and convert to Hz
   maxCiifClockRequired = (maxCiifClockRequired * 101) / 100;
   maxCiifClockRequired = maxCiifClockRequired * 1000000;
   hcaParams.maxCiifClockRequired = maxCiifClockRequired; //save for stats
   if (maxCiifClockRequired > 500000000) //c0 signoff IAE freq
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "new max freq by %d exceeds signoff C0 freq, clip\n", maxCiifClockRequired); 
      maxCiifClockRequired = (500000000 - 1);
   }
   if (cvaLimitMaxFreq)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "required ciif freq with CVA %d\n", maxCiifClockRequired); 
      if (maxCiifClockRequired > 360000000)
      {
         maxCiifClockRequired = (360000000 - 1);
         LOGG_PRINT(LOG_INFO_E, NULL, "max ciif freq %d  - limited by CVA\n", maxCiifClockRequired + 1); 
      }
    }

   //find the best ciif clock configuration for supporting this frequency
   for (i = 0; i < HCA_MNGRP_MAX_NUMBER_CIIF_CLKS; i++)
   {
      if (hcaParams.possibleCiifFreqs[i][0] < maxCiifClockRequired)
      {
         if (i == 0)
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to configure ciif, required freq %d is larger then max possible %d\n",
                                           maxCiifClockRequired,hcaParams.possibleCiifFreqs[i][0]);
            assert(0);
         }
         //found a frequency which is lower then required. Use the one before
         break;
      }
   }

   hcaParams.currentCiifClockTableIndex = (i - 1); //save for stats
   ciifClkSrc = hcaParams.possibleCiifFreqs[i-1][1];
   freqCfg.div = hcaParams.possibleCiifFreqs[i-1][2];
   freqCfg.unit = GME_DRVG_HW_UNIT_CIIF_E;
#ifdef HCA_MNGRP_DEBUG
   LOGG_PRINT(LOG_INFO_E, NULL, "Try to setting ciif to freq to %d (src %s, div %d)\n",
                                hcaParams.possibleCiifFreqs[i-1][0],HCA_MNGRP_ciifSrcToString(ciifClkSrc),freqCfg.div);
#endif
   GME_DRVG_setCiifClkSrc(ciifClkSrc);
   retCode = GME_DRVG_changeUnitFreq(&freqCfg);
   if (ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to configure ciif clock div %d and source %d (required freq %d)\n",
                                     freqCfg.div,ciifClkSrc,hcaParams.possibleCiifFreqs[i-1][0]);
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Setting ciif to freq to %d (src %s, div %d)\n",
                                    hcaParams.possibleCiifFreqs[i-1][0],HCA_MNGRP_ciifSrcToString(ciifClkSrc),freqCfg.div);
   }
  
   return retCode;
}


/****************************************************************************
*
*  Function Name: HCA_MNGRG_allocConsumer
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE HCA_MNGRG_allocConsumer( HCA_MNGRG_consumerHandle *handleP, char *name )
{
   ERRG_codeE retCode = HW_MNGR__RET_SUCCESS;
   UINT32 i;

   for (i = 0; i < HCA_MNGRP_MAX_CONSUMER_NUM; i++)
   {
      if (!hcaParams.consumersTable[i].name)
      {
         hcaParams.consumersTable[i].name = strdup(name);
         *handleP = &hcaParams.consumersTable[i];
         break;
      }
   }

   if (i == HCA_MNGRP_MAX_CONSUMER_NUM)
   {
      retCode = HW_MNGR__ERR_OUT_OF_RSRCS;
      LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to allocate new consumer for %s\n",name);
      assert(0);
   }
   return retCode;
}


/****************************************************************************
*
*  Function Name: HCA_MNGRG_addRequirementToConsumer
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE HCA_MNGRG_addRequirementToConsumer( HCA_MNGRG_consumerHandle handle, UINT32 hwUnit )
{
   ERRG_codeE retCode = HW_MNGR__RET_SUCCESS;
   HCA_MNGRP_consumerParamsT *consumerP = (HCA_MNGRP_consumerParamsT*)handle;
   consumerP->hwUnitsUsed[hwUnit] = 1;
#ifdef HCA_MNGRP_DEBUG
   LOGG_PRINT(LOG_INFO_E, NULL, "%s: add unit %s\n",consumerP->name,HCA_MNGRP_hwUnitToString(hwUnit));
#endif
   return retCode;
}


/****************************************************************************
*
*  Function Name: HCA_MNGRG_addInputClockOfConsumer
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE HCA_MNGRG_addInputClockOfConsumer( HCA_MNGRG_consumerHandle handle, UINT32 inputClock )
{
   ERRG_codeE retCode = HW_MNGR__RET_SUCCESS;
   HCA_MNGRP_consumerParamsT *consumerP = (HCA_MNGRP_consumerParamsT*)handle;
   consumerP->inputPixelClockMhz = inputClock;
#ifdef HCA_MNGRP_DEBUG
   LOGG_PRINT(LOG_INFO_E, NULL, "%s: set input clock %d\n",consumerP->name,inputClock);
#endif
   return retCode;
}


/****************************************************************************
*
*  Function Name: HCA_MNGRG_removeConsumer
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE HCA_MNGRG_removeConsumer( HCA_MNGRG_consumerHandle handle )
{
   ERRG_codeE retCode = HW_MNGR__RET_SUCCESS;
   HCA_MNGRP_consumerParamsT *consumerP = (HCA_MNGRP_consumerParamsT*)handle;
   free(consumerP->name);
   memset(consumerP,0,sizeof(HCA_MNGRP_consumerParamsT));
   return retCode;
}

/****************************************************************************
*
*  Function Name: HCA_MNGRG_disableEnable
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
void HCA_MNGRG_disableEnable( UINT32 disableEnable )
{
   hcaParams.disableFlag = disableEnable;
}


/****************************************************************************
*
*  Function Name: HCA_MNGRG_showStats
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE HCA_MNGRG_showStats( void )
{
   UINT32 i,j;
   
   printf("\nConsumers active:\n");
   for (i = 0; i < HCA_MNGRP_MAX_CONSUMER_NUM; i++)
   {
      if (hcaParams.consumersTable[i].name)
      {
         printf("%s: inputPixelClk %d, requiredClk %d, active units: ",
                hcaParams.consumersTable[i].name, 
                hcaParams.consumersTable[i].inputPixelClockMhz*1000000, 
                hcaParams.consumersTable[i].currentRequiredClockHz);

         for (j = 0; j < HCA_MNGRG_MAX_NUM_HW_UNITS; j++)
         {
            if (hcaParams.consumersTable[i].hwUnitsUsed[j])
            {
               printf("%s ",HCA_MNGRP_hwUnitToString(j));
            }
         }
         printf("\n");
      }
   }
   if (hcaParams.disableFlag)
   {
      UINT32 ciifclk;
      GME_DRVG_getCiifClk(&ciifclk);
      printf("\nHCA disabled! Actual ciif clock %d\n",ciifclk);
   }
   else
   {
      printf("\nMax CIIF frequency required: %d, Actual ciif clock %d (src = %s clk = %d div = %d)\n",
                hcaParams.maxCiifClockRequired,
                hcaParams.possibleCiifFreqs[hcaParams.currentCiifClockTableIndex][0],
                HCA_MNGRP_ciifSrcToString(hcaParams.possibleCiifFreqs[hcaParams.currentCiifClockTableIndex][1]),
                GME_DRVG_calc_pll(HCA_MNGRP_ciifSrcToPllType(hcaParams.possibleCiifFreqs[hcaParams.currentCiifClockTableIndex][1])),
                hcaParams.possibleCiifFreqs[hcaParams.currentCiifClockTableIndex][2]);
   }
}


/****************************************************************************
*
*  Function Name: HCA_MNGRG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE HCA_MNGRG_init( void )
{
   ERRG_codeE ret;
   UINT32     possibleFreqs[GME_DRVG_CIIF_SRC_MAX_NUM_E][HCA_MNGRP_MAX_NUMBER_CIIF_DIV_VALUES + 1]; //set one more
   UINT32 i, sourceClkHz;
   GME_DRVG_ciifClkSrcE clkSrc;

   memset(&hcaParams, 0 , sizeof(HCA_MNGRP_paramsT));
   memset(possibleFreqs,0,sizeof(possibleFreqs));

   for (clkSrc = GME_DRVG_CIIF_SRC_USB3PLL_E; clkSrc < GME_DRVG_CIIF_SRC_MAX_NUM_E; clkSrc++)
   {
      sourceClkHz = GME_DRVG_calc_pll(HCA_MNGRP_ciifSrcToPllType(clkSrc));
      for (i = 0; i < HCA_MNGRP_MAX_NUMBER_CIIF_DIV_VALUES; i++)
      {
         possibleFreqs[clkSrc][i] = sourceClkHz / ((i + 2) * 0.5);
#ifdef HCA_MNGRP_DEBUG
         printf("src %d with freq %d, clk %d, div 0x%x\n",clkSrc,sourceClkHz,possibleFreqs[clkSrc][i],(i + 2));
#endif
      }
   }

   //now lets sort the three arrays into the table
   UINT32 index[3] = { 0 , 0 , 0 };
   UINT32 resultTableIndex = 0;
   UINT32 maxFreq = 0, currentMaxFreqSrc = 0, div = 2;

   for(i = 0; i < HCA_MNGRP_MAX_NUMBER_CIIF_CLKS; i++)
   {
      for (clkSrc = GME_DRVG_CIIF_SRC_USB3PLL_E; clkSrc < GME_DRVG_CIIF_SRC_MAX_NUM_E; clkSrc++)
      {
         if ((index[clkSrc] < HCA_MNGRP_MAX_NUMBER_CIIF_CLKS) && (possibleFreqs[clkSrc][index[clkSrc]] > maxFreq))
         {
            maxFreq = possibleFreqs[clkSrc][index[clkSrc]];
            currentMaxFreqSrc = clkSrc;
            div = index[clkSrc];
         }
      }
#ifdef HCA_MNGRP_DEBUG
      LOGG_PRINT(LOG_INFO_E, NULL, "%d: max freq %d, src %d (%d,%d,%d)\n",currentMaxFreqSrc,maxFreq,currentMaxFreqSrc,possibleFreqs[0][index[0]],possibleFreqs[1][index[1]],possibleFreqs[2][index[2]]);
#endif
      hcaParams.possibleCiifFreqs[resultTableIndex][0] = maxFreq;
      hcaParams.possibleCiifFreqs[resultTableIndex][1] = currentMaxFreqSrc;
      hcaParams.possibleCiifFreqs[resultTableIndex][2] = div + 2; //values 0 and 1 are not valid, we adjust div here
      resultTableIndex++;
      index[currentMaxFreqSrc]++;
      maxFreq = 0;
   }

   //sanity check of the array:
   //1. at this point, the array should be sorted.
   for (i = 1; i < HCA_MNGRP_MAX_NUMBER_CIIF_CLKS; i++)
   {
      if (hcaParams.possibleCiifFreqs[i][0] > hcaParams.possibleCiifFreqs[i - 1][0])
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "problem in array: %d[%d] > %d[%d]\n",hcaParams.possibleCiifFreqs[i][0],i,hcaParams.possibleCiifFreqs[i - 1][0], i - 1);
         assert(0);
      }
   }
#ifndef __ZEBU__// If Zebu --> 0   
   // 2. all index should be at max value
   for (clkSrc = GME_DRVG_CIIF_SRC_USB3PLL_E; clkSrc < GME_DRVG_CIIF_SRC_MAX_NUM_E; clkSrc++)
   {
      if (index[clkSrc] != HCA_MNGRP_MAX_NUMBER_CIIF_DIV_VALUES)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "problem with index: %d = %d\n",clkSrc,index[clkSrc]);
         assert(0);
      }
   }
#endif   
}

#ifdef __cplusplus
}
#endif

