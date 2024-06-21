/****************************************************************************
 *
 *   FileName: ceva_boot.c
 *
 *   Author:  
 *
 *   Date: 
 *
 *   Description: GP code for booing CEVA
 *   
 ****************************************************************************/
 #if 0
#include "inu_common.h"
#include "ceva_boot.h"
#include "ceva_boot_params.h"
#include "stdio.h"
#include "mem_map.h"
#include "nu4100_regs.h"
#include <string.h>
#include "os_lyr.h"
#include "cmem.h"
#include "gme_drv.h"

#include "data_base.h"
#include "icc_common.h"
#include "icc.h"


/****************************************************************************
***************      L O C A L      D E C L A R A T I O N S    **************
****************************************************************************/
static ERRG_codeE CEVA_BOOTP_loadCevaMemory();
void CEVA_BOOTP_XM4_init();


/****************************************************************************
***************      L O C A L      T Y P E D E F S            **************
****************************************************************************/

typedef struct
{
   UINT32                     deviceBaseAddress;
} GME_DRVP_deviceDescT;

typedef struct
{
   UINT32 pll_on   :1,
          bypass   :1,
		  refdiv   :6,
		  fbdiv    :12,
		  unused   :4,
		  postdiv1 :3,
		  postdiv2 :3,
		  dac      :1,
		  dsm      :1;
} BOARD_pllT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static UINT32         bufPhysicalAddress;
static UINT32         bufVirtualAddress = 0;
static GME_DRVP_deviceDescT   GME_DRVP_deviceDesc;
static UINT16                 sleepjobDescriptorIndex;
static int            cevaIsActive = 0;
/****************************************************************************
***************      L O C A L      D E F I N I T I O N S      **************
****************************************************************************/
#define CEVA_BOOTP_BUFFER_SIZE              (0x700000)  
#define CEVA_BOOTP_BOOT_PARAMS_OFFSET       (0x7D)  //boot params offset from boot copier code pointer, in UINT32 units (=0x1f4)
#define CEVA_BOOTP_BOOT_STATUS              (0x7C)  //boot status offset from boot copier code pointer, in UINT32 units (=0x1f0)
#define CEVA_BOOTP_STATUS_FINISHED          (0xB007900d)

#define CEVA_BOOTP_SLEEP_POLL_PERIOD_US     (200)
#define CEVA_BOOTP_SLEEP_TOTAL_TIME_MS      (5*1000)

/****************************************************************************
**************       L O C A L   F U N C T I O N S             **************
****************************************************************************/


/****************************************************************************
***************      G L O B A L    F U N C T I O N S          **************
****************************************************************************/

/****************************************************************************
*
*  Function Name: CEVA_BOOTG_enableClk
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
static void CEVA_BOOTP_enableClk( void )
{
   GME_CLOCK_ENABLE_DSPA_CLK_EN_W(1);     // enable dsp cpu clock
   while(GME_CLOCK_ENABLE_DSPA_CLK_EN_R == 0);   
}


/****************************************************************************
*
*  Function Name: CEVA_BOOTG_disableClk
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
static void CEVA_BOOTP_disableClk( void )
{
   GME_CLOCK_ENABLE_DSPA_CLK_EN_W(0);     // disable dsp cpu clock
}


/****************************************************************************
*
*  Function Name: CEVA_BOOTP_updateDspPll
*
*  Description: Configure DSP PLL parameters
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
static void CEVA_BOOTP_updateDspPll()
{
	UINT32 foutvco,freq;
	UINT32 *pllRegPtr;

	BOARD_pllT pllReg;

	pllRegPtr = (unsigned int *)&pllReg;

	pllReg.pll_on   = 1;
	pllReg.bypass   = 0;
	pllReg.refdiv   = 12;
//	pllReg.fbdiv    = 620; // 944Mhz valid values from 500MHz-1500MHz
	pllReg.fbdiv    = 944; // 944Mhz valid values from 500MHz-1500MHz
    pllReg.postdiv1 = 2;
    pllReg.postdiv2 = 1;
    pllReg.dac      = 0;
    pllReg.dsm      = 1;
	
   // --------------------------------- //
   // Configure DSP PLL parameters 
   // --------------------------------- //
   GME_DSP_PLL_CONTROL_VAL =  *pllRegPtr;
   GME_FREQ_CHANGE_DSP_FULL_CHG_GO_BIT_W(1);
   while(GME_FRQ_CHG_STATUS_DSP_FULL_FREQ_CHG_DONE_R == 0);
   GME_FREQ_CHANGE_DSP_FULL_CHG_GO_BIT_W(0);
   GME_FRQ_CHG_STATUS_CLEAR_DSP_FULL_FREQ_CHG_DONE_W(1);
   GME_FRQ_CHG_STATUS_CLEAR_DSP_FULL_FREQ_CHG_DONE_W(0);

	//	read configured value
   *pllRegPtr = GME_DSP_PLL_STATUS_VAL;
   foutvco = (24*pllReg.fbdiv)/pllReg.refdiv;
   freq = (foutvco/pllReg.postdiv1)/pllReg.postdiv2;
   LOGG_PRINT(LOG_INFO_E, NULL, "XM4 frequency = %d, pll status = 0x%x\n", freq, *pllRegPtr);
}



/****************************************************************************
*
*  Function Name: CEVA_BOOTP_loadCevaMemory
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
static ERRG_codeE CEVA_BOOTP_loadCevaMemory()
{
   ERRG_codeE     ret = CEVA_BOOT__RET_SUCCESS;
   FILE           *cevaFile;
   UINT32         size, fileSize, copierSize, components;
   MEM_MAPG_addrT oAddressP;
   UINT32         *cevaLoaderParamsP;


   cevaFile   = fopen(CEVA_BOOTG_BINARY_FILE_NAME, "rb");
   if(!cevaFile)
   {
      LOGG_PRINT(LOG_INFO_E,NULL,"Can't find CEVA XM4 files\n");
      return CEVA__ERR_FAILED_TO_OPEN_FILE;
   }
   LOGG_PRINT(LOG_INFO_E,NULL,"opened CEVA XM4 binary files\n");
   
   // load boot_copier.bin to DDR
   MEM_MAPG_getVirtAddr(MEM_MAPG_DDR_CEVA_LOAD_TCM_E, &oAddressP);

   //   cva file should contains 2 components, one is the boot copier and another one is the binary code and data
   size = fread( &components, 4, 1, cevaFile);
   if((components != 2) || (!size))
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"ERROR reading number of components %d, size = %d\n", components, size);
      return CEVA__ERR_FAILED_TO_OPEN_FILE;
   }

   size = fread( &copierSize, 4, 1, cevaFile);
   if(!size) 
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"ERROR reading cevaBootCopier (1)\n");
      return CEVA__ERR_FAILED_TO_OPEN_FILE;
   }
   
   size = fread( oAddressP, 1, copierSize, cevaFile);
   if(!size) 
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"ERROR reading cevaBootCopier (2)\n");
      return CEVA__ERR_FAILED_TO_OPEN_FILE;
   }

   //get image size
   size = fread( &fileSize, 4, 1, cevaFile);
   if(!size) 
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"ERROR reading cevaBootCopier (3)\n");
      return CEVA__ERR_FAILED_TO_OPEN_FILE;
   }

   //get temp memory to place the CEVA file
   ret = MEM_MAPG_alloc(CEVA_BOOTP_BUFFER_SIZE,&bufPhysicalAddress,&bufVirtualAddress,1);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, 0,  "ERROR: Failed to init memory for ceva XM4 boot\n");
      return ret;
   }

   //verify hte image is not bigger than the allocated temp memory
   if (fileSize > CEVA_BOOTP_BUFFER_SIZE)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"CEVA XM4 image size(%dB) exceeds MEM_MAPG_DDR_CEVA_LOAD_SMEM_E section\n", fileSize);
      return CEVA__ERR_FAILED_TO_OPEN_FILE;
   }
   
   // load image.bin to DDR
   size = fread( (void*)bufVirtualAddress, 1, fileSize, cevaFile);
   if(!size)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"ERROR reading cevaCodeFile\n");
      return CEVA__ERR_FAILED_TO_OPEN_FILE;
   }
   CMEM_cacheWb((void*)bufVirtualAddress, fileSize);

   //Update boot copier params
   cevaLoaderParamsP = (UINT32*)oAddressP;
   cevaLoaderParamsP += CEVA_BOOTP_BOOT_PARAMS_OFFSET;//offset 0x1f4 from boot copier code start
   *cevaLoaderParamsP++ = 0; //pointer to the temp buffer for pdma transfer, if 0 then no need for temp buffer
   *cevaLoaderParamsP++ = bufPhysicalAddress; //offset 0x1f8, the address of the image.bin
   *cevaLoaderParamsP++ = 0; //offset 0x1fc, application entry point

   //Close file
   if(cevaFile)
   {
      fclose(cevaFile);
      remove(CEVA_BOOTG_BINARY_FILE_NAME);
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: CEVA_BOOTG_sleep
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
static void CEVA_BOOTG_sleepMsgCb(UINT16 jobDescriptorIndex, ERRG_codeE result, UINT32 dspTarget)
{
   //stub
   (void)jobDescriptorIndex;
   (void)result;
   (void)dspTarget;
}



ERRG_codeE CEVA_BOOTG_sleep( void )
{
   ERRG_codeE                            retCode = CEVA_BOOT__RET_SUCCESS;
   ICCG_cmdT                             cmd;
   DATA_BASEG_iccJobsDescriptorDataBaseT *jobsDbP;
   volatile UINT32 DSP_STATUS;

   if(ERRG_SUCCEEDED(retCode))
   {
      //safe to access without mutex, we got the descriptor
      DATA_BASEG_accessDataBaseNoMutex((UINT8**)(&jobsDbP), DATA_BASEG_JOB_DESCRIPTORS);
      jobsDbP->jobDescriptorPool[sleepjobDescriptorIndex].alg             = DATA_BASEG_SLEEP;
      jobsDbP->jobDescriptorPool[sleepjobDescriptorIndex].cb              = CEVA_BOOTG_sleepMsgCb;
      jobsDbP->jobDescriptorPool[sleepjobDescriptorIndex].data            = 0;
      jobsDbP->jobDescriptorPool[sleepjobDescriptorIndex].arg             = 0;
    
      cmd.cmdType  = ICCG_CMD_REQ;
      cmd.descriptorHandle = sleepjobDescriptorIndex;
      cmd.dspTarget = ICCG_CMD_TARGET_DSPA;
      retCode = ICCG_sendIccCmd(&cmd);
      
      if(ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_ERROR_E, retCode, "Error sending sleep CMD to DSP\n");
      }
      else
      {
         DSP_STATUS = GME_DSP_STATUS_VAL;
         int i = 0;
         //Wait until DSP is in sleep, than its save to close clock
         while (((DSP_STATUS & 0x3) != 0x3) && (i < ((CEVA_BOOTP_SLEEP_TOTAL_TIME_MS*1000) / CEVA_BOOTP_SLEEP_POLL_PERIOD_US)))
         {
            i++;
            OS_LYRG_usleep(CEVA_BOOTP_SLEEP_POLL_PERIOD_US);
            DSP_STATUS = GME_DSP_STATUS_VAL;
         }

         if (i == ((CEVA_BOOTP_SLEEP_TOTAL_TIME_MS*1000) / CEVA_BOOTP_SLEEP_POLL_PERIOD_US))
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Error sending sleep CMD to DSP. DSP_STATUS = 0x%x\n",DSP_STATUS);
         }
         else
         {
            //success, now close the clock
            CEVA_BOOTP_disableClk();
         }
      }
   }
   return retCode;
}


ERRG_codeE CEVA_BOOTG_wakeup( void )
{
   ERRG_codeE  retCode = CEVA_BOOT__RET_SUCCESS;
   UINT32 DSP_STATUS = GME_DSP_STATUS_VAL;

	if((DSP_STATUS & 0x3) != 0x3)
	{
		LOGG_PRINT(LOG_ERROR_E, NULL, "DSP status is invalid 0x%x\n", DSP_STATUS);   	
        return CEVA__ERR_CEVA_ALREADY_RUNNING;
	}

   CEVA_BOOTP_enableClk();
 
   return retCode;
}


/****************************************************************************
*
*  Function Name: CEVA_BOOTG_loadCeva
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE CEVA_BOOTG_loadCeva()
{
   ERRG_codeE                 ret = CEVA_BOOT__RET_SUCCESS;
   static BOOL                cevaBootFlag = FALSE;
   volatile CEVA_BOOTG_bootSyncParamsT *CEVA_BOOTP_cevaBootParams;
   MEM_MAPG_addrT                oAddressP;
   MEM_MAPG_addrT    memVirtAddr;

   //sanity check for memory alignment
   if (sizeof(CEVA_BOOTG_bootSyncParamsT) != MEM_MAPG_CEVA_BOOT_PARAMS_SIZE)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Ceva boot sync params size is different then defined in mem_map\n");
      return 0;
   }

   if (!cevaBootFlag)
   {
      ret = CEVA_BOOTP_loadCevaMemory();

      MEM_MAPG_getVirtAddr(MEM_MAPG_REG_GME_E, (&memVirtAddr));
      GME_DRVP_deviceDesc.deviceBaseAddress = (UINT32)memVirtAddr;
      CEVA_BOOTP_updateDspPll();//Update DSP frequency.
      if(ERRG_SUCCEEDED(ret))
      {
         LOGG_initializeCpLog(MEM_MAPG_CEVA_LOG_ADDRESS,OS_LYRG_CP_MUTEX_CEVA_LOG_E);      
#ifdef POWER_MODE_OPT
         ret = GME_DRVG_setPowerMode(GME_DRVG_POWER_DSP_E);
#endif
         if(ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "error setting power mode for EVP\n");
         }
         else
         {
            cevaBootFlag = TRUE;

            LOGG_PRINT(LOG_INFO_E,NULL,"Load CEVA XM4 Memory Done\n");
            //Update CEVA with shared database address
            MEM_MAPG_getVirtAddr(MEM_MAPG_CRAM_CEVA_BOOT_PARAMS, &oAddressP);
            CEVA_BOOTP_cevaBootParams = (CEVA_BOOTG_bootSyncParamsT *)oAddressP;
            CEVA_BOOTP_cevaBootParams->bootCompleteFlag = 0;
            
            MEM_MAPG_getPhyAddr(MEM_MAPG_LRAM_DATA_BASE_E, &oAddressP);
            CEVA_BOOTP_cevaBootParams->ddrDataBaseAddress = (UINT32)oAddressP;

            CEVA_BOOTP_XM4_init();
         }
      }
   }
   else
   {
      LOGG_PRINT(LOG_WARN_E, NULL, "CEVA XM4 load fail!!! CEVA already running\n");
      ret = CEVA__ERR_CEVA_ALREADY_RUNNING;
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: CEVA_BOOTP_XM4_init
*
*  Description: CEVA XM4 boot
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
void CEVA_BOOTP_XM4_init( )
{
   MEM_MAPG_addrT   codePhysicalAddress;
   UINT32 oVirtualAddrP;
   volatile UINT32 *byPassP;
   UINT32 tmp;
   ERRG_codeE ret = CEVA_BOOT__RET_SUCCESS;
   
   LOGG_PRINT(LOG_INFO_E,NULL,"XM4 init start\n");
   GME_CONTROL_START_POWER_CHANGE_W(1);
   do{
      GME_POWER_MODE_DSP_POWER_UP_W(1);
   }while(!GME_POWER_MODE_STATUS_DSP_POWER_UP_R);
   GME_CONTROL_START_POWER_CHANGE_W(0);
//      CEVA_BOOTP_updateDspPll();//Update DSP frequency


   //Update XM4 with with the boot_loader code address
   MEM_MAPG_getPhyAddr(MEM_MAPG_DDR_CEVA_LOAD_TCM_E, &codePhysicalAddress);//Get the physical address of the CEVA boot loader code
   ret = MEM_MAPG_convertPhysicalToVirtual2(0x80D0268, &oVirtualAddrP, MEM_MAPG_REG_PSS_E); //Take the virtual address of the PSS_DSP_VECTOR_ADDRESS_W register 
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"failed to find virtual address for 0x%x\n",0x80D0268);
   }
   *(UINT32*)oVirtualAddrP = (UINT32)codePhysicalAddress;//Update PSS_DSP_VECTOR_ADDRESS_W with the boot_loader code address
   
   //NU4000 A0 bypass, need to write to 0x80D0260 in order to use vector register
   oVirtualAddrP = oVirtualAddrP - 8;//update adddress to 0x80D0260 for bypass write
   byPassP = (UINT32*)oVirtualAddrP;
   tmp = *byPassP;
   *byPassP = tmp;
      
   //    PSS_DSP_VECTOR_ADDRESS_W(LRAM_BASE_ADDRESS);
   //    PSS_DSP_CTRL_BOOT_W(1); //not needed (maybe for rebooting)

   // --------------------------------- //
   // Enable DSP clocks (CPU and AXI clocks)
   // --------------------------------- //
   GME_CLOCK_ENABLE_DSP_CLK_EN_W(1);      // enable dsp clock for AXI access
   GME_CLOCK_ENABLE_DSPA_CLK_EN_W(1);     // enable dsp cpu clock
   while(GME_CLOCK_ENABLE_DSP_CLK_EN_R == 0);
   while(GME_CLOCK_ENABLE_DSPA_CLK_EN_R == 0); 
}


/****************************************************************************
*
*  Function Name: CEVA_BOOTP_verify
*
*  Description: CEVA XM4 boot
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE CEVA_BOOTG_verify( )
{
   MEM_MAPG_addrT   memVirtAddr,oAddressP;
   CEVA_BOOTG_bootSyncParamsT *bootParamsP;
   volatile UINT32 *statusP;
   UINT32 sleepTime = 0;
   ERRG_codeE ret = CEVA_BOOT__RET_SUCCESS;

   //check if CEVA was booted
   if (GME_CLOCK_ENABLE_DSPA_CLK_EN_R)
   {
      MEM_MAPG_getVirtAddr(MEM_MAPG_CRAM_CEVA_BOOT_PARAMS, &oAddressP);
      bootParamsP = (CEVA_BOOTG_bootSyncParamsT *)oAddressP;
      
      //Check if CEVA XM4 load was successfull  
      MEM_MAPG_getVirtAddr(MEM_MAPG_DDR_CEVA_LOAD_TCM_E, &memVirtAddr);
      statusP = (UINT32*)memVirtAddr;
      statusP += CEVA_BOOTP_BOOT_STATUS;//offset 0x1f0 from boot copier code start

      while((sleepTime < 2000000) && (*statusP != CEVA_BOOTP_STATUS_FINISHED))
      {
         OS_LYRG_usleep(100);
         sleepTime+=100;
         CMEM_cacheInv((void *)statusP, 4);
      }

      //Check CEVA XM4 boot status
      if (*statusP != CEVA_BOOTP_STATUS_FINISHED)
      {
         LOGG_PRINT(LOG_ERROR_E,NULL,"------------------------\n");
         LOGG_PRINT(LOG_ERROR_E,NULL,"XM4 didn't load 0x%x!!!!\n", *statusP);
         LOGG_PRINT(LOG_ERROR_E,NULL,"------------------------\n");
      }
      else
      {
         LOGG_PRINT(LOG_INFO_E,NULL,"XM4 loaded successfully\n");
         cevaIsActive = 1;
         sleepTime = 0;
         while ((bootParamsP->bootCompleteFlag != 1) && (sleepTime < 4000000))
         {
            OS_LYRG_usleep(200);
            sleepTime+=200;
         }

         if (sleepTime == 4000000)
         {
            LOGG_PRINT(LOG_ERROR_E,NULL,"XM4 boot failed\n");
         }
         else
         {
            LOGG_PRINT(LOG_INFO_E,NULL,"XM4 boot successfully\n");
            //put CEVA to sleep
            ret = ICCG_getJobDescriptor(&sleepjobDescriptorIndex);
            CEVA_BOOTG_sleep();

            //Free temp DDR boot memory
            if (bufVirtualAddress)
            {
               ret = MEM_MAPG_free((UINT32*)bufVirtualAddress);
            }
         }
      } 
   }
   return ret;
}

UINT32 CEVA_BOOTG_cevaIsActive()
{
   return cevaIsActive;
}
#endif
