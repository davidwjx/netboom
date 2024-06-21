#include "string.h"
#include "inu_types.h"
#include "err_defs.h"
#include "mem_pool.h"
#include "os_lyr.h"
#include "mem_pool.h"
#include "log.h"
#include "ppe_mngr.h"

static UINT32 ppeBaseAddress;
typedef struct
{
   UINT32                  deviceBaseAddress;
} IAE_DRVP_deviceDescT;

static IAE_DRVP_deviceDescT   IAE_DRVP_deviceDesc;


#include "nu4100_ppe_regs.h"
#include "nu4100_iae_regs.h"

#include "cde_mngr_new.h"


#define APB_WRITE(Addr, Val) *(volatile unsigned int *) (Addr) = Val

typedef enum
{
   CIIF_BPP_SIZE_MONO   =   ( 8 ),
   CIIF_BPP_SIZE_COLOR =   ( 24 ),
   CIIF_BPP_SIZE_IIC   =   ( 32 ),
   CIIF_BPP_SIZE_DOG   =   ( 96 ),
   CIIF_BPP_SIZE_FREAK   =   ( 608 )
}ciif_bpp_size_e;


typedef struct
{
   UINT32 image_0_width;
   UINT32 image_0_height;
   UINT32 image_1_width;
   UINT32 image_1_height;
   UINT32 hyb_width;
   UINT32 is_hybrid_en;
   UINT32 injection_cram_addr;
   UINT32 is_inj_en;
   UINT32 is_freak_inj_en;
   UINT32 iic_ddr_addr_0;   
   UINT32 iic_ddr_addr_1;
   UINT32 num_of_frames;
   ciif_bpp_size_e input_ciif_size;   
   
}DMA_PL330_test_params_t;


static DMA_PL330_test_params_t param;





//void sys_clocks_init( void );
void dma_init( void );
void frameDoneCb(CDE_MNGRG_userCbParamsT *chanP, void *arg);
void frameGeneratorConfig( UINT32 width, UINT32 height );

typedef enum
{
   PPU_SRC_IAU_0 = 6,
   PPU_SRC_IAU_1 = 7,
   PPU_SRC_AXI_WB_0_STR_0 = 10,
   PPU_SRC_AXI_WB_2_STR_0 = 12,
   PPU_SRC_CVJ_0_STR_0 = 16,
   PPU_SRC_CVJ_1_STR_0 = 18
}ppe_src_select_e;

static void ppe_set_axi_reader_0( UINT32 width, UINT32 height, UINT32 bpp_size, ppe_src_select_e img_src )
{
   (void)height;(void)width;(void)bpp_size;
   // Configure PPE outputs for dma core 0 (READ AXI 0-3)
   PPE_AXI_READ0_CTRL_DATA_SEL_W( img_src );               
   PPE_AXI_READ0_CTRL_INT_STRM_EN_W( 1 );         // en
   PPE_AXI_READ0_CTRL_PXL_DATA_WIDTH_W( 0 );       // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit
   PPE_AXI_READ0_CTRL_PXL_INT_WIDTH_W( 0 );      // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit   
//   PPE_AXI_READ0_CTRL_MUTUAL_STRMS_W( 1 << 0 );   // Need to configure mutual streams for recovery. 
//   PPE_AXI_READ0_CTRL_ERROR_RECOVERY_EN_W( 1 );   // enable recovery mechanism
   PPE_AXI_READ0_AXI_CFG_BURST_LEN_W( 0xf );       
//   PPE_AXI_READ0_AXI_CFG_SINGLE_END_W( 1 );      // 0/1 DMA/PPE control the axi transaction

//   PPE_AXI_READ0_INT_ENABLE_FRAME_START_W( 1 );
//   PPE_AXI_READ0_INT_ENABLE_FRAME_END_W( 1 );
   PPE_AXI_READ0_INT_ENABLE_LINE_BUFFER_OVERFLOW_W( 1 );
//   PPE_AXI_READ0_AXI_BEAT_LAST_VAL_W( ((width * 24 * height) / 128) - 1   );   
}

#if 0
static void ppe_set_axi_reader_1( UINT32 width, UINT32 height, UINT32 bpp_size, ppe_src_select_e img_src )
{
   // Configure PPE outputs for dma core 0 (READ AXI 0-3)
   PPE_AXI_READ1_CTRL_DATA_SEL_W( img_src );               
   PPE_AXI_READ1_CTRL_INT_STRM_EN_W( 1 );         // en
   PPE_AXI_READ1_CTRL_PXL_DATA_WIDTH_W( 0 );       // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit
   PPE_AXI_READ1_CTRL_PXL_INT_WIDTH_W( 0 );      // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit
   PPE_AXI_READ1_CTRL_MUTUAL_STRMS_W( 1 << 1 );   // Need to configure mutual streams for recovery. 
   PPE_AXI_READ1_CTRL_ERROR_RECOVERY_EN_W( 1 );   // enable recovery mechanism
   PPE_AXI_READ1_AXI_CFG_BURST_LEN_W( 0xf );       
   PPE_AXI_READ1_AXI_CFG_SINGLE_END_W( 0 );      // 0/1 DMA/PPE control the axi transaction

   PPE_AXI_READ1_INT_ENABLE_FRAME_START_W( 1 );
   PPE_AXI_READ1_INT_ENABLE_FRAME_END_W( 1 );
   PPE_AXI_READ1_INT_ENABLE_LINE_BUFFER_OVERFLOW_W( 1 );
   PPE_AXI_READ1_AXI_BEAT_LAST_VAL_W( ((width * 24 * height) / 128) - 1   );   
}
#endif

static void ppe_set_axi_reader_4( UINT32 width, UINT32 height, UINT32 bpp_size, ppe_src_select_e img_src )
{
   (void)height;(void)width;(void)bpp_size;
   // Configure PPE outputs for dma core 1 (READ AXI 4-7)
   PPE_AXI_READ4_CTRL_DATA_SEL_W( img_src );               
   PPE_AXI_READ4_CTRL_INT_STRM_EN_W( 1 );         // en
   PPE_AXI_READ4_CTRL_PXL_DATA_WIDTH_W( 0 );       // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit
   PPE_AXI_READ4_CTRL_PXL_INT_WIDTH_W( 0 );      // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit
//   PPE_AXI_READ4_CTRL_MUTUAL_STRMS_W( 1 << 4 );   // Need to configure mutual streams for recovery. 
//   PPE_AXI_READ4_CTRL_ERROR_RECOVERY_EN_W( 1 );   // enable recovery mechanism
   PPE_AXI_READ4_AXI_CFG_BURST_LEN_W( 0xf );       
//   PPE_AXI_READ4_AXI_CFG_SINGLE_END_W( 1 );      // 0/1 DMA/PPE control the axi transaction
   
//   PPE_AXI_READ4_INT_ENABLE_FRAME_START_W( 1 );
//   PPE_AXI_READ4_INT_ENABLE_FRAME_END_W( 1 );
   PPE_AXI_READ4_INT_ENABLE_LINE_BUFFER_OVERFLOW_W( 1 );
//   PPE_AXI_READ4_AXI_BEAT_LAST_VAL_W( ((width * 24 * height) / 128) - 1   );   
}

void CDE_TEST()
{
   unsigned int dummy = 0;

   CDE_MNGRG_channnelInfoT* reader0 = NULL;
   CDE_MNGRG_channnelInfoT* reader1 = NULL;
//   CDE_MNGRG_channnelInfoT writer0;
//   CDE_MNGRG_channnelInfoT writer1;

   CDE_DRVG_channelCfgT      readerConfig;
   CDE_MNGRG_chanCbInfoT   readerCbCfg;

   printf("SW: CDE_TEST start\n");

   memset(&readerConfig,0,sizeof(readerConfig));
   memset(&readerCbCfg,0,sizeof(readerCbCfg));

   MEM_MAPG_addrT       memVirtAddr;

   MEM_MAPG_getVirtAddr(MEM_MAPG_REG_IAE_E, (&memVirtAddr));
   IAE_DRVP_deviceDesc.deviceBaseAddress = (UINT32)memVirtAddr;

   MEM_MAPG_getVirtAddr(MEM_MAPG_REG_PPE_E, (&memVirtAddr));
   ppeBaseAddress = (UINT32)memVirtAddr;
   PPE_MNGRG_init( ppeBaseAddress );

//   CDE_DRVG_channelCfgT   writer0Config;
//   CDE_DRVG_channelCfgT   writer1Config;

   // Test configuration
   param.image_0_width =       640;   //64;   //         // If format has changed, DMA size should be updated
   param.image_0_height =       480;   //64;   //
   param.image_1_width =       640;   //64;   //
   param.image_1_height =       480;   //64;   //

   param.is_hybrid_en =       1;
   param.hyb_width =          param.image_0_width + param.image_1_width;

   param.input_ciif_size =    CIIF_BPP_SIZE_MONO;
   
   param.is_inj_en =          1;
   param.injection_cram_addr = 0x01004000;

   param.is_freak_inj_en =    0;
   param.iic_ddr_addr_0 =       0x01010000;//   IIC_DEFAULT_DDR_ADDRESS;   
   param.iic_ddr_addr_1 =       0x01031000;//   IIC_DEFAULT_DDR_ADDRESS + 0x10000;
   param.num_of_frames =       3;

   
    // call gme init. note: printf() is not working before gpio clock init.
//   sys_clocks_init();
   //dma_init();

#if 0
   // call ddr_init
   ddr_init();
   printf("SW: ddr_init done");
#endif

   CDE_MNGRG_init();
   printf("SW: CDE_MNGRG_init done\n");
   CDE_MNGRG_open();
   printf("SW: CDE_MNGRG_open done\n");


   readerConfig.streamCfg.scenarioType                = CDE_DRVG_SCENARIO_TYPE_STREAM_FIXEDSIZE_E;
   readerConfig.streamCfg.opMode                         = CDE_DRVG_CONTINIOUS_OP_MODE_E;
   readerConfig.streamCfg.frameMode                     = CDE_DRVG_FRAME_MODE_NONE_INTERLEAVE_E;
   readerConfig.streamCfg.transferMode                  = CDE_DRVG_1D_TRANSFER_MODE_E;                        // NOT implemented
   readerConfig.streamCfg.resolutionType               = CDE_DRVG_RESOLUTION_TYPE_FULL_E;                     // NOT implemented
   readerConfig.streamCfg.numOfBuffs                  = 4;

   readerConfig.srcCfg.peripheral                     = CDE_DRVG_PERIPH_CLIENT_AXI_RD0_TX_E;
   readerConfig.srcCfg.dimensionCfg.frameStartX         = 0;                                          // hack for bare metal to pass mem offset

   readerConfig.dstCfg.peripheral                     = CDE_DRVG_PERIPH_CLIENT_MEMORY_RX_E;
   readerConfig.dstCfg.dimensionCfg.bufferHeight         = param.image_0_height;
   readerConfig.dstCfg.dimensionCfg.bufferWidth         = param.image_0_width;
   readerConfig.dstCfg.dimensionCfg.imageDim.pixelSize      = (param.input_ciif_size >> 3);

   readerCbCfg.frameDoneCb.cb                        = frameDoneCb;
   readerCbCfg.frameDoneCb.arg                       = (void*)&dummy;
   readerCbCfg.channelDoneCb.cb                     = NULL;
   readerCbCfg.channelDoneCb.arg                     = NULL;

   CDE_MNGRG_openChannel( &reader0, readerConfig.srcCfg.peripheral, readerConfig.dstCfg.peripheral );

   
   if(reader0->channelStatus != CDE_MNGRG_CHAN_STATUS_OPENED_E) 
   {
      printf("SW: CDE_MNGRG_openChannel reader0 failed\n");
   }
 
   CDE_MNGRG_setupChannel( reader0, &readerConfig, &readerCbCfg );
   if(reader0->channelStatus != CDE_MNGRG_CHAN_STATUS_CONFIGURED_E) 
   {
      printf("SW: CDE_MNGRG_setupChannel reader0 failed\n");
   }

   readerConfig.srcCfg.peripheral               = CDE_DRVG_PERIPH_CLIENT_AXI_RD4_TX_E;
   readerConfig.srcCfg.dimensionCfg.frameStartX   = 1;                                          // hack for bare metal to pass mem offset

   CDE_MNGRG_openChannel( &reader1, readerConfig.srcCfg.peripheral, readerConfig.dstCfg.peripheral );
   if(reader1->channelStatus != CDE_MNGRG_CHAN_STATUS_OPENED_E) 
   {
      printf("SW: CDE_MNGRG_openChannel reader1 failed\n");
   }

   CDE_MNGRG_setupChannel( reader1, &readerConfig, &readerCbCfg );
   if(reader1->channelStatus != CDE_MNGRG_CHAN_STATUS_CONFIGURED_E) 
   {
      printf("SW: CDE_MNGRG_setupChannel reader1 failed\n");
   }

   CDE_MNGRG_startChannel(reader0);
   CDE_MNGRG_startChannel(reader1);

   ppe_set_axi_reader_0( param.image_0_width, param.image_0_height, param.input_ciif_size, PPU_SRC_IAU_0 );
   ppe_set_axi_reader_4( param.image_0_width, param.image_0_height, param.input_ciif_size, PPU_SRC_IAU_1 );
   PPE_PPE_MISC_CTRL_REGISTERS_READY_DONE_W(1);
   printf("SW: readers configuration done\n");


//   ppe_set_axi_writer_0( param.image_0_width, param.image_0_height, param.input_ciif_size );
//   ppe_set_axi_writer_1( param.image_1_width, param.image_1_height, param.input_ciif_size );
   printf("SW: Injection configured\n");

   frameGeneratorConfig( param.image_0_width, param.image_0_height );
   printf("SW: Generator configured\n");

   /*****************************************************************
    *                         End Section                            *
    ******************************************************************/
   printf("SW: End Test\n");
   while(1);
}




#if 0
void sys_clocks_init( void )
{
   //printf("SW: gme init() start");
   //
   // initialize gme
   //
   gme_init();


   //change LRAM clock frequency
   GME_DSP_PLL_CONTROL_VAL = 0xA008609;   //400MHz   // 0xA002A05 value for LRAM clk 250 MHz
   while(!(GME_DSP_PLL_STATUS_LOCK_R));
   GME_DSP_CLOCK_CONFIG_CLK_SRC_W(1);
   GME_FREQ_CHANGE_DSP_GO_BIT_W(1);
   while(!(GME_FRQ_CHG_STATUS_DSP_FREQ_CHG_DONE_R));
   GME_FREQ_CHANGE_VAL = 0x0000000;
   GME_FRQ_CHG_STATUS_CLEAR_DSP_FREQ_CHG_DONE_W(1);
   GME_FRQ_CHG_STATUS_CLEAR_DSP_FREQ_CHG_DONE_W(0);


   //printf("SW: gme init() done");

   //printf("SW: enable_usb3_pll() start");
   //
   // enable usb3 pll
   //
   enable_usb3_pll(); 
   //printf("SW: enable_usb3_pll() done");

   //printf("SW: set power mode start");
   //
   // set power mode
   //
   // 1. enable iae and ppe (pss enabled by default)
   GME_POWER_MODE_IAE_POWER_UP_W(1);
   GME_POWER_MODE_PPE_POWER_UP_W(1);

   // 2. initiate power mode change
   GME_CONTROL_START_POWER_CHANGE_W(1);

   // 3. polling on power mode status bits
   while(!GME_POWER_MODE_STATUS_IAE_POWER_UP_R);
   while(!GME_POWER_MODE_STATUS_PPE_POWER_UP_R);


   // Enable IAE and PPE clk
   GME_CLOCK_ENABLE_IAE_CLK_EN_W(1);
   GME_CLOCK_ENABLE_PPE_CLK_EN_W(1);
   while(!GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R);
   while(!GME_CLOCK_ENABLE_STATUS_PPE_CLK_EN_R);   

   // Enable CVA clk
   GME_CLOCK_ENABLE_CVA_CLK_EN_W(1);
   while(!GME_CLOCK_ENABLE_STATUS_CVA_CLK_EN_R);

   GME_CLOCK_ENABLE_LRAM_CLK_EN_W(1);
   while(!GME_CLOCK_ENABLE_STATUS_LRAM_CLK_EN_R);

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
    printf("SW: Clocks init done");
}
#endif

void dma_init( void )
{
/*
   // Work with APB IF commands
   PSS_DMA_0_CTRL_BOOT_FROM_PC_W(0);
   PSS_DMA_1_CTRL_BOOT_FROM_PC_W(0);
   PSS_DMA_2_CTRL_BOOT_FROM_PC_W(0);
   
   //   Enable dma clocks
   GME_CLOCK_ENABLE_DMA_0_CLK_EN_W(1);
   GME_CLOCK_ENABLE_DMA_1_CLK_EN_W(1);
   GME_CLOCK_ENABLE_DMA_2_CLK_EN_W(1);
   while(!(GME_CLOCK_ENABLE_STATUS_DMA_0_CLK_EN_R));
   while(!(GME_CLOCK_ENABLE_STATUS_DMA_1_CLK_EN_R));
   while(!(GME_CLOCK_ENABLE_STATUS_DMA_2_CLK_EN_R));         // set dma code descriptor before deassertion of dma reset & enable its clk
*/
}


void frameDoneCb(CDE_MNGRG_userCbParamsT *chanP, void *arg)
{
   UINT32 temp = *(UINT32*)arg;
   temp++;

   LOGG_PRINT(LOG_INFO_E, NULL, "frameDoneCb. buffDescriptorP = %p, buffP =%p, frameNum = %d \n",chanP->buffDescriptorP, chanP->buffDescriptorP->dataP, temp);

   
   MEM_POOLG_free(chanP->buffDescriptorP);

}


void frameGeneratorConfig( UINT32 width, UINT32 height )
{
   // enable - enable slu 0/1, iau 0/1
//    IAE_ENABLE_SLU0_EN_W(1);
//    IAE_ENABLE_SLU1_EN_W(1);
    IAE_ENABLE_IAU0_EN_W(1);
    IAE_ENABLE_IAU1_EN_W(1);

    // bypass - bypass all
    APB_WRITE(IAE_BASE+0xC, 0xFFFFFFFF);

    // iim alignment - align 0 & 1
    //IAE_IIM_ALIGNMENT_EN_W(1);

    // iim control (iim <-> iau muxing) - select image gen 0
    IAE_IIM_CONTROL_IAU0_SEL_W(12);
    IAE_IIM_CONTROL_IAU1_SEL_W(12);


   IAE_FGEN0_CONTROL_MODE_W(0x00);
   IAE_FGEN0_CONTROL_FRAME_NUM_W(0);
   IAE_FGEN0_FRAME_SIZE_HORZ_W((width - 1));
   IAE_FGEN0_FRAME_SIZE_VERT_W((height - 1));
   IAE_FGEN0_BLANK_FRAME_W(0xF);
   IAE_FGEN0_BLANK_LINE_W(width);

   IAE_REGISTERS_READY_DONE_W(1);

   IAE_ENABLE_GEN0_EN_W(1);

    
}

