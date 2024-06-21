#if 0
#include "string.h"
#include "inu_types.h"
#include "err_defs.h"
#include "mem_pool.h"
#include "os_lyr.h"
#include "mem_pool.h"
#include "log.h"
#include <errno.h>

#include "inu_cmd.h"

#include "gme_drv.h"

#include "ppe_mngr.h"
#include "iae_mngr.h"
#include "dpe_mngr.h"

#include "cde_mngr_new.h"
#include "sequence_mngr.h"
#include "nu4100_regs.h"


static   int loadNu4000Socxml(char **xmlbuf, unsigned int *xmlsize);
static   void sys_clocks_init( void );
static   void dma_init( void );

void SEQ_TEST( void )
{
   unsigned int   xmlsize;
   char*          xmlbuf = NULL;
   int            ret;
   XMLDB_dbH      testDb = NULL;

   ret = loadNu4000Socxml(&xmlbuf, &xmlsize);
   if (ret < 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " loadNuSocxml failed \n");
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, " loadNuSocxml done \n");
   }

   ret = XMLDB_init();
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " XMLDB_init failed \n");
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, " XMLDB_init done \n");
   }

   ret = XMLDB_open(&testDb);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " XMLDB_open failed \n");
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, " XMLDB_open done \n");
   }

   ret = XMLDB_loadFromBuf(testDb, xmlbuf);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " XMLDB_loadFromBuf failed \n");
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, " XMLDB_loadFromBuf done \n");
   }

   sys_clocks_init();
   dma_init();

   ret = SEQ_MNGRG_init( );
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " SEQ_MNGRG_init failed \n");
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, " SEQ_MNGRG_init done \n");
   }

   ret = CDE_MNGRG_init();
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " CDE_MNGRG_init failed \n");
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, " CDE_MNGRG_init done \n");
   }

   ret = CDE_MNGRG_open();
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " CDE_MNGRG_open failed \n");
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, " CDE_MNGRG_open done \n");
   }

   ret = MIPI_MNGRG_init();
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " MIPI_MNGRG_init failed \n");
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, " MIPI_MNGRG_init done \n");
   }

   ret = MIPI_MNGRG_open();
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " MIPI_MNGRG_open failed \n");
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, " MIPI_MNGRG_open done \n");
   }


   ret = SEQ_MNGRG_configSystem(testDb);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " SEQ_MNGRG_configSystem failed \n");
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, " SEQ_MNGRG_configSystem done \n");
   }
   ret = SEQ_MNGRG_startSystem( testDb );
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " SEQ_MNGRG_startSystem failed \n");
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, " SEQ_MNGRG_startSystem done \n");
   }
   GME_DRVG_dumpRegs();
   IAE_DRVG_dumpRegs();
   PPE_MNGRG_dumpRegs();
   DPE_MNGRG_dumpRegs();

}


static  int loadNu4000Socxml(char **xmlbuf, unsigned int *xmlsize)
{
   int ret = 0;
   char *buf = NULL;
   unsigned int size;
   //open and load xml file
   FILE *xmlfile = fopen("config/nu4k_out_mod.xml", "r");
   if (!xmlfile)
      return -1;

   fseek(xmlfile, 0L, SEEK_END);
   size = ftell(xmlfile);
   fseek(xmlfile, 0L, SEEK_SET);

   buf = (char *)malloc(size + 1);
   if (buf)
   {
      size_t r = fread(buf, size, 1, xmlfile);
      if ((r != 1) && (!feof(xmlfile)))
      {
         printf("read file error %s\n", strerror(errno));
         ret = -1;
         free(buf);
      }
      buf[size] = '\0';//otherwise sxmlc parser chokes
   }
   else
   {
      ret = -1;
   }

   if (ret >= 0)
   {
      *xmlbuf = buf;
      *xmlsize = size;
   }

   fclose(xmlfile);
   return ret;
}

typedef struct
{
   UINT32                     deviceBaseAddress;
} GME_DRVP_deviceDescT;

static GME_DRVP_deviceDescT   GME_DRVP_deviceDesc;

#if 1
void sys_clocks_init( void )
{
      ERRG_codeE            ret;
       MEM_MAPG_addrT           memVirtAddr;
       GME_DRVG_unitClkDivT    unitClkDiv;

       UINT32 busy_wait = 2000; 

       MEM_MAPG_getVirtAddr(MEM_MAPG_REG_GME_E, (&memVirtAddr));
       GME_DRVP_deviceDesc.deviceBaseAddress = (UINT32)memVirtAddr;

       GME_MNGRG_init(GME_DRVP_deviceDesc.deviceBaseAddress);

       ret = GME_DRVG_setCiifClkSrc(GME_DRVG_CIIF_SRC_DSP_E);         // Set DSP PLL
       if(ERRG_FAILED(ret))
       {
         printf("SW: GME_DRVG_CIIF_SRC_DSP_E set clock failed \n");
       }
       unitClkDiv.unit = GME_DRVG_HW_UNIT_CIIF_E;

       unitClkDiv.div = 2;
       GME_DRVG_setUnitClockDiv(&unitClkDiv);
       GME_CIIF_CONTROL_PAR_0_CLK_DIV_W(0x8);
       GME_CIIF_CONTROL_PAR_1_CLK_DIV_W(0x8);

       ret = GME_DRVG_getUnitClockDiv(&unitClkDiv);
       if(ERRG_FAILED(ret))
       {
         printf("SW: GME_DRVG_getUnitClockDiv clock failed \n");
       }

       printf("SW: power set ciif div %d \n",unitClkDiv.div);
       
       ret = GME_DRVG_changeUnitFreq(&unitClkDiv);
       if(ERRG_FAILED(ret))
       {
         printf("SW: GME_DRVG_changeUnitFreq clock failed \n");
       }

       GME_CIIF_CONTROL_PAR_0_CLK_DIV_W(0x8);
       GME_CIIF_CONTROL_PAR_1_CLK_DIV_W(0x8);

       GME_CONTROL_KEEP_USB3_PLL_ON_W(1);

       printf("SW: power set gme status: GME control(0x8C)  = %08x \n, GME CIIF control(0x35C)  = %08x, GME_PLL_LOCK_COUNT_VAL = %08x \n", GME_CONTROL_VAL, GME_CIIF_CONTROL_VAL, GME_PLL_LOCK_COUNT_VAL );

#if 0
       GME_IO_MUX_CTRL_0_VAL |= 0x02;                                 // set IO to debug_clk

       GME_DEBUG_CLK_CONTROL_VAL = 0xA11;                         //usb sys clk       // IAE clk out
       while(busy_wait--);
#endif
       //enable cv0/1 

       // 1. enable iae and ppe (pss enabled by default)
       GME_DRVG_setPowerMode(GME_DRVG_POWER_IAE_E);
       GME_DRVG_setPowerMode(GME_DRVG_POWER_PPE_E);
       printf("SW: power set 1 %s %d\n",__FILE__,__LINE__);

       ret =  GME_DRVG_enableClk(GME_DRVG_HW_UNIT_PPE_E);
       if(ERRG_FAILED(ret))
       {
         printf("SW: GME_DRVG_HW_UNIT_PPE_E enable clock failed \n");
       }
       ret = GME_DRVG_enableClk(GME_DRVG_HW_UNIT_IAE_E);
       if(ERRG_FAILED(ret))
       {
         printf("SW: GME_DRVG_HW_UNIT_IAE_E enable clock failed \n");
       }

       ret = GME_DRVG_enableClk(GME_DRVG_HW_CVRS_CLK_EN_E);
       if(ERRG_FAILED(ret))
       {
         printf("SW: GME_DRVG_HW_CVRS_CLK_EN_E enable clock failed \n");
       }
       ret = GME_DRVG_enableClk(GME_DRVG_HW_CVLS_CLK_EN_E);
       if(ERRG_FAILED(ret))
       {
         printf("SW: GME_DRVG_HW_CVLS_CLK_EN_E enable clock failed \n");
       }


       printf("SW: Clocks init done\n" );
}

#else
void sys_clocks_init( void )
{
   MEM_MAPG_addrT       memVirtAddr;

   MEM_MAPG_getVirtAddr(MEM_MAPG_REG_GME_E, (&memVirtAddr));
   GME_DRVP_deviceDesc.deviceBaseAddress = (UINT32)memVirtAddr;


   GME_MNGRG_init(GME_DRVP_deviceDesc.deviceBaseAddress);

   UINT32 busy_wait = 2000; 
/*
   //string_to_gpio("SW: gme init() start");
   //
   // initialize gme
   //
   gme_init();

   debug_printf("SW: power set -4 \n");


   //change LRAM clock frequency
   GME_DSP_PLL_CONTROL_VAL = 0xA008609;   //400MHz   // 0xA002A05 value for LRAM clk 250 MHz
   while(!(GME_DSP_PLL_STATUS_LOCK_R));
*/   

#if 0   
   printf("SW: power set -3 \n");
   GME_DSP_CLOCK_CONFIG_CLK_SRC_W(1);
   GME_FREQ_CHANGE_DSP_GO_BIT_W(1);
   while(!(GME_FRQ_CHG_STATUS_DSP_FREQ_CHG_DONE_R));
   
   printf("SW: power set -2 \n");
   GME_FREQ_CHANGE_VAL = 0x0000000;
   GME_FRQ_CHG_STATUS_CLEAR_DSP_FREQ_CHG_DONE_W(1);
   GME_FRQ_CHG_STATUS_CLEAR_DSP_FREQ_CHG_DONE_W(0);
#endif

   GME_CIIF_CONTROL_CLK_SRC_W(2);                  // Set DSP PLL

   GME_FREQ_CHANGE_CIIF_GO_BIT_W(1);
   while(!(GME_FRQ_CHG_STATUS_CIIF_FREQ_CHG_DONE_R));
   
   printf("SW: power set -1 \n");
   GME_FREQ_CHANGE_VAL = 0x0000000;
   GME_FRQ_CHG_STATUS_CLEAR_CIIF_FREQ_CHG_DONE_W(1);
   GME_FRQ_CHG_STATUS_CLEAR_CIIF_FREQ_CHG_DONE_W(0);

   GME_CONTROL_KEEP_USB3_PLL_ON_W(1);

   printf("SW: power set gme status: GME control(0x8C)  = %08x \n, GME CIIF control(0x35C)  = %08x, GME_PLL_LOCK_COUNT_VAL = %08x \n", GME_CONTROL_VAL, GME_CIIF_CONTROL_VAL, GME_PLL_LOCK_COUNT_VAL );


   GME_IO_MUX_CTRL_0_VAL |= 0x02;               // set IO to debug_clk

   GME_DEBUG_CLK_CONTROL_VAL = 0xA11;      //usb sys clk      // IAE clk out
   while(busy_wait--);


/*
   //string_to_gpio("SW: gme init() done");

   //string_to_gpio("SW: enable_usb3_pll() start");
   //
   // enable usb3 pll
   //
   enable_usb3_pll(); 
   //string_to_gpio("SW: enable_usb3_pll() done");
*/
   //string_to_gpio("SW: set power mode start");
   //
   // set power mode
   //
   // 1. enable iae and ppe (pss enabled by default)
   GME_POWER_MODE_IAE_POWER_UP_W(1);
   GME_POWER_MODE_PPE_POWER_UP_W(1);
   printf("SW: power set 1 \n");

   // 2. initiate power mode change
   GME_CONTROL_START_POWER_CHANGE_W(1);
   printf("SW: power set 2 \n");

   // 3. polling on power mode status bits
   while(!GME_POWER_MODE_STATUS_IAE_POWER_UP_R);
   while(!GME_POWER_MODE_STATUS_PPE_POWER_UP_R);
   printf("SW: power set 3 \n");


   // Enable IAE and PPE clk
   if(GME_CLOCK_ENABLE_STATUS_PPE_CLK_EN_R == 0)
   {
      GME_CLOCK_ENABLE_PPE_CLK_EN_W(1);
      while(!GME_CLOCK_ENABLE_STATUS_PPE_CLK_EN_R);      
      printf("SW: power set PPE status DONE !!!! = 0x%x \n", GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R);   
   }
   else
   {
      printf("SW: power set PPE status fail = 0x%x \n", GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R);   
   }

   if(GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R == 0)
   {
      GME_CLOCK_ENABLE_IAE_CLK_EN_W(1);
      while(!GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R);      
      printf("SW: power set IAE status DONE !!!! = 0x%x \n", GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R);   
   }
   else
   {
      printf("SW: power set IAE status fail = 0x%x \n", GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R);   
   }


   
//   GME_CLOCK_ENABLE_IAE_CLK_EN_W(1);
//   GME_CLOCK_ENABLE_PPE_CLK_EN_W(1);
//   debug_printf("SW: power set 4 ");   
//   while(!GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R);
//   while(!GME_CLOCK_ENABLE_STATUS_PPE_CLK_EN_R);   
   printf("SW: power set 5 \n");

   // Enable CVA clk
//   GME_CLOCK_ENABLE_CVA_CLK_EN_W(1);
//   while(!GME_CLOCK_ENABLE_STATUS_CVA_CLK_EN_R);

//   GME_CLOCK_ENABLE_LRAM_CLK_EN_W(1);
//   while(!GME_CLOCK_ENABLE_STATUS_LRAM_CLK_EN_R);

#if 0
   if( param.is_inj_en == 0 )
   {
      GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY0_RX_CFG_CLK_EN_W(1);
      GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY1_RX_CFG_CLK_EN_W(1);
      GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY0_TX_CFG_CLK_EN_W(1);
      GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY1_TX_CFG_CLK_EN_W(1);
      //YAEL
      GME_PERIPH_CLOCK_ENABLE_CV_0_CLK_EN_W(1);
      GME_PERIPH_CLOCK_ENABLE_CV_1_CLK_EN_W(1);

      // polling on all clocks
   
      while(!GME_PERIPH_CLOCK_ENABLE_STATUS_MIPI_DPHY0_RX_CFG_CLK_EN_R);
      while(!GME_PERIPH_CLOCK_ENABLE_STATUS_MIPI_DPHY1_RX_CFG_CLK_EN_R);
      while(!GME_PERIPH_CLOCK_ENABLE_STATUS_MIPI_DPHY0_TX_CFG_CLK_EN_R);
      while(!GME_PERIPH_CLOCK_ENABLE_STATUS_MIPI_DPHY1_TX_CFG_CLK_EN_R);
   }
#endif   
   printf("SW: Clocks init done\n" );
}
#endif

void dma_init( void )
{
   // Work with APB IF commands
//   PSS_DMA_0_CTRL_BOOT_FROM_PC_W(0);
//   PSS_DMA_1_CTRL_BOOT_FROM_PC_W(0);
//   PSS_DMA_2_CTRL_BOOT_FROM_PC_W(0);
   
   //   Enable dma clocks
   GME_CLOCK_ENABLE_DMA_0_CLK_EN_W(1);
   GME_CLOCK_ENABLE_DMA_1_CLK_EN_W(1);
   GME_CLOCK_ENABLE_DMA_2_CLK_EN_W(1);
   printf("SW: dma power set 1 \n");
   while(!(GME_CLOCK_ENABLE_STATUS_DMA_0_CLK_EN_R));
   while(!(GME_CLOCK_ENABLE_STATUS_DMA_1_CLK_EN_R));
   while(!(GME_CLOCK_ENABLE_STATUS_DMA_2_CLK_EN_R));         // set dma code descriptor before deassertion of dma reset & enable its clk
   
   printf("SW: dma power set 2 \n");
}

#endif
