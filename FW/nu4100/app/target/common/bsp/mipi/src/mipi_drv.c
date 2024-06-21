/****************************************************************************
 *
 *   FileName: mipi_drv.c
 *
 *   Author:  Arnon C.
 *
 *   Date: 
 *
 *   Description: MIPI driver
 *   
 ****************************************************************************/

/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
 ****************************************************************************/
#include "inu_common.h"
#include "os_lyr.h"

#include "nu4k_defs.h"
#include "mipi_mngr.h"
#include "mipi_drv.h"
#include "mipi.h"
#include "cde_mngr.h"
#include "cde_drv.h"
#include "iae_drv.h"
#include "gme_mngr.h"
#include "ppe_drv.h"

#include "assert.h"

//hard coded values
#define MIPI_TX_STOP_STATE_DELAY_USEC  (500*1000) 

#define IO_HANDLE(dev)  IO_PALG_getHandle(dev)

typedef enum {RX, TX} dphyTypeE;


// MIPI CSI register offset
#define VERSION                 0x0     // Read-only register Contains the version of DWC_mipi_csi2_host coded in 32-bit ASCII code.
#define N_LANES                 0x4     // Static read and write register Configures the number of active lanes that the DWC_mipi_csi2_host...
#define CSI2_RESETN             0x8     // Static read and write register Controls the DWC_mipi_csi2_host logic reset state. When activated,...
#define INT_ST_MAIN             0xc     // Clear on read register Contains the status of individual interrupt sources, regardless of the...
#define DATA_IDS_1              0x10    // Static read and write register Programs the data Ids for matching line error reporting. Enables...
#define DATA_IDS_2              0x14    // Static read and write register Programs the data Ids for matching line error reporting. Configures...
#define PHY_SHUTDOWNZ           0x40    // Active low Controls the D-PHY Shutdown mode. In this state, the D-PHY sets the analog and digital...
#define DPHY_RSTZ               0x44    // Active low Controls the Synopsys D-PHY Reset mode. in this state, the D-PHY sets the digital circuitry...
#define PHY_RX                  0x48    // Read-only register Contains the status of RX-related signals from Synopsys D-PHY: - RXULPSESC*...
#define PHY_STOPSTATE           0x4c    // Read-only register Contains the STOPSTATE signal status from Synopsys D-PHY.
#define PHY_TEST_CTRL0          0x50    // Dynamic register Control for vendor specific interface in the PHY. Controls the Synopsys D-PHY...
#define PHY_TEST_CTRL1          0x54    // Dynamic register Control for vendor specific interface in the PHY. Controls the Synopsys D-PHY...
#define PHY2_TEST_CTRL0         0x58    // Dynamic register (available when DPHY LANES > 4) Control for vendor specific interface in the...
#define PHY2_TEST_CTRL1         0x5c    // Dynamic register (available when D-PHY LANES > 4) Control for vendor specific interface in the...
#define IPI_MODE                0x80    // This register selects how the IPI interface generates the video frame. 
#define IPI_VCID                0x84    // This register selects the virtual channel processed by IPI.
#define IPI_DATA_TYPE           0x88    // This register selects the data type processed by IPI.
#define IPI_MEM_FLUSH           0x8c    // This register control the flush of IPI memory.
#define IPI_HSA_TIME            0x90    // This register configures the video Horizontal Synchronism Active (HSA) time.
#define IPI_HBP_TIME            0x94    // This register configures the video Horizontal Back Porch (HBP) time.
#define IPI_HSD_TIME            0x98    // This register configures the video Horizontal Sync Delay (HSD) time.
#define IPI_HLINE_TIME          0x9c    // This register configures the overall time for each video line.
#define IPI_VSA_LINES           0xb0    // This register configures the Vertical Synchronism Active (VSA) period.
#define IPI_VBP_LINES           0xb4    // This register configures the Vertical Back Porch (VBP) period.
#define IPI_VFP_LINES           0xb8    // This register configures the Vertical Front Porch (VFP) period.
#define IPI_VACTIVE_LINES       0xbc    // This register configures the vertical resolution of the video.
#define PHY_CAL                 0xcc    // Clear on read register Contains the CALIBRATION signal status from Synopsys D-PHY.
#define INT_ST_PHY_FATAL        0xe0    // Clear on read register Groups the fatal interruptions caused by PHY Packet discarded. Stores...
#define INT_MSK_PHY_FATAL       0xe4    // Static read and write register Each bit of this register masks the interruption in INT_ST_PHY_FATAL...
#define INT_FORCE_PHY_FATAL     0xe8    // Static read and write register Interrupt Force register is used for test purposes, and allows...
#define INT_ST_PKT_FATAL        0xf0    // Clear on read register Groups the fatal interruption related with Packet construction. Packet...
#define INT_MSK_PKT_FATAL       0xf4    // Static read and write register Each bit of this register masks the interruption from INT_ST_PKT_FATAL...
#define INT_FORCE_PKT_FATAL     0xf8    // Static read and write register Interrupt force register is used for test purposes, and allows...
#define INT_ST_FRAME_FATAL      0x100   // Clear on read register Fatal interruption related with Frame construction. Packet discarded....
#define INT_MSK_FRAME_FATAL     0x104   // Static read and write register Each bit of this register masks the interruption from INT_ST_FRAME_FATAL...
#define INT_FORCE_FRAME_FATAL   0x108   // Static read and write register Interrupt force register is used for test purposes, and allows...
#define INT_ST_PHY              0x110   // Clear on read register Interruption caused by PHY. Groups and notifies which interruption bits...
#define INT_MSK_PHY             0x114   // Static read and write register Each bit of this register masks the interruption from INT_ST_PHY...
#define INT_FORCE_PHY           0x118   // Static read and write register Interrupt force register is used for test purposes, and allows...
#define INT_ST_PKT              0x120   // Clear on read register Interruption related with Packet construction. Packet discarded. Groups...
#define INT_MSK_PKT             0x124   // Static read and write register Each bit of this register masks the interruption from INT_ST_PKT...
#define INT_FORCE_PKT           0x128   // Static read and write register Interrupt force register is used for test purposes, and allows...
#define INT_ST_LINE             0x130   // Clear on read register Interruption related with Line construction. Groups and notifies which...
#define INT_MSK_LINE            0x134   // Static read and write register Each bit of this register masks the interruption from INT_ST_LINE...
#define INT_FORCE_LINE          0x138   // Static read and write register Interrupt force register is used for test purposes, and allows...
#define INT_ST_IPI              0x140   // Clear on read register Fatal Interruption caused by IPI interface. Groups and notifies which...
#define INT_MSK_IPI             0x144   // Static read and write register Each bit of this register masks the interruption from INT_ST_IPI...
#define INT_FORCE_IPI           0x148   // Static read and write register Interrupt force register is used for test purposes, and allows...

// MIPI DSI register offset
#define VERSION                 0x0     // This register contains the version of the DSI host controller.
#define PWR_UP                  0x4     // This register controls the power up of the core.
#define CLKMGR_CFG              0x8     // This register configures the factor for internal dividers to divide lanebyteclk for timeout...
#define DPI_VCID                0xc     // This register configures the Virtual Channel ID for DPI traffic.
#define DPI_COLOR_CODING        0x10    // This register configures DPI color coding.
#define DPI_CFG_POL             0x14    // This register configures the polarity of DPI signals.
#define DPI_LP_CMD_TIM          0x18    // This register configures the timing for low-power commands sent while in video mode.
#define DBI_VCID                0x1c    // This register configures Virtual Channel ID for DBI traffic.
#define DBI_CFG                 0x20    // This register configures the bit width of pixels for DBI.
#define DBI_PARTITIONING_EN     0x24    // This register configures whether DWC_mipi_dsi_host is to partition DBI traffic automatically.
#define DBI_CMDSIZE             0x28    // This register configures the command size and the size for automatic partitioning of DBI...
#define PCKHDL_CFG              0x2c    // This register configures how EoTp, BTA, CRC and ECC are to be used, to meet peripheral's...
#define GEN_VCID                0x30    // This register configures the Virtual Channel ID of READ responses to store and return to Generic...
#define MODE_CFG                0x34    // This register configures the mode of operation between Video or Command Mode. (Commands can still...
#define VID_MODE_CFG            0x38    // This register configures several aspects of Video mode operation, the transmission mode, switching...
#define VID_PKT_SIZE            0x3c    // This register configures the video packet size.
#define VID_NUM_CHUNKS          0x40    // This register configures the number of chunks to use. The data in each chunk has the size provided...
#define VID_NULL_SIZE           0x44    // This register configures the size of null packets.
#define VID_HSA_TIME            0x48    // This register configures the video HSA time.
#define VID_HBP_TIME            0x4c    // This register configures the video HBP time.
#define VID_HLINE_TIME          0x50    // This register configures the overall time for each video line.
#define VID_VSA_LINES           0x54    // This register configures the VSA period.
#define VID_VBP_LINES           0x58    // This register configures the VBP period.
#define VID_VFP_LINES           0x5c    // This register configures the VFP period.
#define VID_VACTIVE_LINES       0x60    // This register configures the vertical resolution of video.
#define EDPI_CMD_SIZE           0x64    // This register configures the size of eDPI packets.
#define CMD_MODE_CFG            0x68    // This register configures several aspect of command mode operation, tearing effect, acknowledge for...
#define GEN_HDR                 0x6c    // This register sets the header for new packets sent using the Generic interface.
#define GEN_PLD_DATA            0x70    // This register sets the payload for packets sent using the Generic interface and, when read, returns...
#define CMD_PKT_STATUS          0x74    // This register configures contains information about the status of FIFOs related to DBI and Generic...
#define TO_CNT_CFG              0x78    // This register configures counters that trigger timeout errors. These are used to warn the system...
#define HS_RD_TO_CNT            0x7c    // This register configures the Peripheral Response timeout after High-Speed Read operations.
#define LP_RD_TO_CNT            0x80    // This register configures the Peripheral Response timeout after Low-Power Read operations.
#define HS_WR_TO_CNT            0x84    // This register configures the Peripheral Response timeout after High-Speed Write operations.
#define LP_WR_TO_CNT            0x88    // This register configures the Peripheral Response timeout after Low-Power Write operations.
#define BTA_TO_CNT              0x8c    // This register configures the Peripheral Response timeout after Bus Turnaround completion.
#define SDF_3D                  0x90    // This register stores 3D control information for VSS packets in video mode.
#define LPCLK_CTRL              0x94    // This register configures the possibility for using non continuous clock in the clock lane.
#define PHY_TMR_LPCLK_CFG       0x98    // This register sets the time that DWC_mipi_dsi_host assumes in calculations for the clock lane to...
#define PHY_TMR_CFG             0x9c    // This register sets the time that DWC_mipi_dsi_host assumes in calculations for the data lanes to...
#define PHY_RSTZ                0xa0    // This register controls resets and the PLL of the D-PHY.
#define PHY_IF_CFG              0xa4    // This register configures the number of active lanes and the minimum time to remain in stop...
#define PHY_ULPS_CTRL           0xa8    // This register configures entering and leaving ULPS in the D-PHY.
#define PHY_TX_TRIGGERS         0xac    // This register configures the pins that activate triggers in the D-PHY.
#define PHY_STATUS              0xb0    // This register contains information about the status of the D-PHY.
#define PHY_TST_CTRL0           0xb4    // This register controls clock and clear pins of the D-PHY vendor specific interface.
#define PHY_TST_CTRL1           0xb8    // This register controls data and enable pins of the D-PHY vendor specific interface.
#define INT_ST0                 0xbc    // This register contains the status of interrupt sources from acknowledge reports and the D-PHY.
#define INT_ST1                 0xc0    // This register contains the status of interrupt sources related to timeouts, ECC, CRC, packet size,...
#define INT_MSK0                0xc4    // This register configures masks for the sources of interrupts that affect the INT_ST0 register. Write...
#define INT_MSK1                0xc8    // This register configures masks for the sources of interrupts that affect the INT_ST1 register.
#define PHY_CAL                 0xcc    // This register controls the skew calibration of D-PHY.
#define INT_FORCE0              0xd8    // This register forces that affect the INT_ST0 register.
#define INT_FORCE1              0xdc    // This register forces interrupts that affect the INT_ST1 register.
#define DSC_PARAMETER           0xf0    // This register configures Display Stream Compression
#define PHY_TMR_RD_CFG          0xf4    // This register configures times related to PHY to perform some operations in lane byte clock...
#define VID_SHADOW_CTRL         0x100   // This register controls dpi shadow feature
#define DPI_VCID_ACT            0x10c   // This register holds the value that controller is using for DPI_VCID.
#define DPI_COLOR_CODING_ACT    0x110   // This register holds the value that controller is using for DPI_COLOR_CODING.
#define DPI_LP_CMD_TIM_ACT      0x118   // This register holds the value that controller is using for DPI_LP_CMD_TIM.
#define VID_MODE_CFG_ACT        0x138   // This register holds the value that controller is using for VID_MODE_CFG.
#define VID_PKT_SIZE_ACT        0x13c   // This register holds the value that controller is using for VID_PKT_SIZE.
#define VID_NUM_CHUNKS_ACT      0x140   // This register holds the value that controller is using for VID_NUM_CHUNKS.
#define VID_NULL_SIZE_ACT       0x144   // This register holds the value that controller is using for VID_NULL_SIZE.
#define VID_HSA_TIME_ACT        0x148   // This register holds the value that controller is using for VID_HSA_TIME.
#define VID_HBP_TIME_ACT        0x14c   // This register holds the value that controller is using for VID_HBP_TIME.
#define VID_HLINE_TIME_ACT      0x150   // This register holds the value that controller is using for VID_HLINE_TIME.
#define VID_VSA_LINES_ACT       0x154   // This register holds the value that controller is using for VID_VSA_LINES.
#define VID_VBP_LINES_ACT       0x158   // This register holds the value that controller is using for VID_VBP_LINES.
#define VID_VFP_LINES_ACT       0x15c   // This register holds the value that controller is using for VID_VFP_LINES.
#define VID_VACTIVE_LINES_ACT   0x160   // This register holds the value that controller is using for VID_VACTIVE_LINES.
#define SDF_3D_ACT              0x190   // This register holds the value that controller is using for SDF_3D.

// PHY STATUS
#define MIPI_DPHY_ENABLE_LANES_BIT 4

// MIPI stop state related

#define MIPI_DSI_STOP_STATE_CLOCK_BIT 2
#define MIPI_DSI_STOP_STATE_LANE0_BIT 4
#define MIPI_DSI_STOP_STATE_LANE1_BIT 7

#define MIPI_CSI_STOP_STATE_CLOCK_BIT 16
#define MIPI_CSI_STOP_STATE_LANE0_BIT 0
#define MIPI_CSI_STOP_STATE_LANE1_BIT 1

#define MIPI_TIMEOUT 500000000

// APB read-write
#define APB_WRITE(Addr, Val) *(volatile unsigned int *) (Addr) = Val;
#define APB_READ(Addr) *(volatile unsigned int *) (Addr)

#define BIT(BitOffset)      ((UINT32)1 << (BitOffset))

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static UINT32 iaeBaseVirtualP; 
static UINT32 ppeBaseVirtualP; 



#define MIPI_DPHY_FREQ_RANGE_NUM 63

int freqRanges[MIPI_DPHY_FREQ_RANGE_NUM] = 
  {
    82.5 , 92.5 , 102.5, 112.5, 122.5, 132.5, 142.5, 152.5, 162.5, 172.5,
    182.5, 192.5, 207.5, 222.5, 237.5, 262.5, 287.5, 312.5, 337.5, 375  ,
    425  , 475  , 525  , 575  , 625  , 675  , 725  , 775  , 825  , 875  ,
    925  , 975  , 1025 , 1075 , 1125 , 1175 , 1225 , 1275 , 1325 , 1375 ,
    1425 , 1475 , 1525 , 1575 , 1625 , 1675 , 1725 , 1775 , 1825 , 1875 ,
    1925 , 1975 , 2025 , 2075 , 2125 , 2175 , 2225 , 2275 , 2325 , 2375 ,
    2425 , 2475 , 2525
  };

int hsfreqrange[MIPI_DPHY_FREQ_RANGE_NUM] = 
  {
    0x0 , 0x10, 0x20, 0x30, 0x1 , 0x11, 0x21, 0x31, 0x2 , 0x12,
    0x22, 0x32, 0x3 , 0x13, 0x23, 0x33, 0x4 , 0x14, 0x25, 0x35,
    0x5 , 0x16, 0x26, 0x37, 0x7 , 0x18, 0x28, 0x39, 0x9 , 0x19,
    0x29, 0x3a, 0xa , 0x1a, 0x2a, 0x3b, 0xb , 0x1b, 0x2b, 0x3c,
    0xc , 0x1c, 0x2c, 0x3d, 0xd , 0x1d, 0x2e, 0x3e, 0xe , 0x1e,
    0x2f, 0x3f, 0xf , 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
    0x47, 0x48, 0x49
  };

int osc_freq_target_tbl[MIPI_DPHY_FREQ_RANGE_NUM] = 
  {
    460, 460, 460, 460, 460, 460, 460, 460, 460, 460,
    460, 460, 460, 460, 460, 460, 460, 460, 460, 460,
    460, 460, 460, 460, 460, 460, 460, 460, 460, 460,
    460, 460, 460, 460, 460, 460, 460, 460, 460, 460,
    460, 460, 460, 285, 295, 304, 313, 322, 331, 341,
    350, 359, 368, 377, 387, 396, 405, 414, 423, 432,
    442, 451, 460
  };

static UINT32 activeTxDphy[4] = {0, 0, 0, 0};

//
// Convert a value representing bitrate to the correct configuration of hsfreqrange register 
// of the MIPI DPHY 
//

int convert_bitrate_to_hsfreqrange(int bitrate)
{
   int i;

   for (i = 0; i < MIPI_DPHY_FREQ_RANGE_NUM; i++ )
   {
      if (bitrate <= freqRanges[i]) return hsfreqrange[i];
   }
   assert(0);
   return -1;
}

//
// Convert a value representing bitrate to the correct configuration of hsfreqrange register 
// of the MIPI DPHY 
//

int convert_bitrate_to_ddl_osc_freq_target(int bitrate)
{
   int i;
    
   for (i = 0; i < MIPI_DPHY_FREQ_RANGE_NUM; i++)
   {
      if (bitrate <= freqRanges[i]) return osc_freq_target_tbl[i];
   }
   assert(0);
   return -1;
}

static void MIPI_DRVP_setDphyFreqRange(int inst, UINT16 pllMHz)
{
   unsigned int hsfreqrange;

   hsfreqrange = convert_bitrate_to_hsfreqrange(pllMHz);
   LOGG_PRINT(LOG_DEBUG_E, NULL,"mipi freq range setting %dMHz (val 0x%x)\n", pllMHz,hsfreqrange);
   mipi_csi2_dw_test_code_program1(inst, 0x2,hsfreqrange); //test code- address of the dphy
}

/*
static void MIPI_DRVP_waitEnableLane(int inst)
{
   UINT8 data_read ,dphy_enable_lanes=0; 

   do 
   {
	   //data_read = read_reg(mipi_csi2_dw_reg_addr(inst, 0xc9));
	   data_read = mipi_csi2_dw_test_code_read(inst, 0xc9,1);//ELAD:SELECT??????????
       dphy_enable_lanes = data_read & (1 << 4);
   }while (!dphy_enable_lanes);
   
}*/


static void MIPI_DRVP_waitStopState(int inst, unsigned int numLanes)
{
   mipi_csi2_phy_lane_state_e lane_clk;
   mipi_csi2_phy_lane_state_e lane_1;
   mipi_csi2_phy_lane_state_e lane_2;
   
   LOGG_PRINT(LOG_DEBUG_E, NULL,"wait for mipi Rx to reach stop state %d\n",inst);


   do 
   {
      if(numLanes > 0)
         lane_1 = mipi_csi2_dw_get_lane_state(inst, mipi_csi2_dw_lane_1_e);
      else
         lane_1 = mipi_csi2_dw_lane_stop_state_e;

      if(numLanes > 1)
         lane_2 =mipi_csi2_dw_get_lane_state(inst, mipi_csi2_dw_lane_2_e);
      else
         lane_2 = mipi_csi2_dw_lane_stop_state_e;

      lane_clk =mipi_csi2_dw_get_lane_state(inst,mipi_csi2_dw_lane_clk_e);
      //OS_LYRG_usleep(1000*10); //need to poll this quickly so we don't miss the stop state
      //LOGG_PRINT(LOG_INFO_E, NULL,"Mipi lane states %d %d %d\n",lane_1, lane_2, lane_clk);
   }while((lane_1 != mipi_csi2_dw_lane_stop_state_e) || (lane_2 !=mipi_csi2_dw_lane_stop_state_e) || (lane_clk != mipi_csi2_dw_lane_stop_state_e));
   LOGG_PRINT(LOG_DEBUG_E, NULL,"Mipi RX reached stop state\n");
}

ERRG_codeE MIPI_DRVG_init(void)
{
   ERRG_codeE ret;
   ret = MEM_MAPG_getVirtAddr(MEM_MAPG_REG_IAE_E, (MEM_MAPG_addrT *)&iaeBaseVirtualP);
   ret = MEM_MAPG_getVirtAddr(MEM_MAPG_REG_PPE_E, (MEM_MAPG_addrT *)&ppeBaseVirtualP);
   return ret;
}

UINT32 MIPI_DRVG_getIaeVirtBase(void)
{
   return iaeBaseVirtualP;
}

UINT32 MIPI_DRVG_getPpeVirtBase(void)
{
   return ppeBaseVirtualP;
}

/****************************************************************************
*
*  Function Name: MIPI_DRVG_cfgRx
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
ERRG_codeE MIPI_DRVG_cfgRx(mipiInstConfigT mipiCfg, UINT8 is4lanes)
{
   ERRG_codeE ret = MIPI__RET_SUCCESS;
//   unsigned int numLanes;
   mipiDphyCfgT dphy;
   int osc_freq_target;
   INU_DEFSG_moduleTypeE model = GME_MNGRG_getModelType();

   dphy = mipiCfg.dphy;
   LOGG_PRINT(LOG_DEBUG_E, ret,"mipi drv configuring mipi %d\n",mipiCfg.mipiInst);

   // 0. set address 0xC9

   ////////////////////////////////////////////////////////////////////////////////////////////////
   //
   //The sequence was taken from SYNOPSYS DesignWare� Cores MIPI D-PHY v1.2 Rx 2L for TSMC 12-nm FFC/1.8V
   //Databook, Limited Customer Availability (LCA) Edition
   // November 2017
   //
   //case3
   ///////////////////////////////////////////////////////////////////////////////////////////////
   
   // 1. set rstz = 1'b0  (default)
   mipi_csi2_dw_dphy_reset(mipiCfg.mipiInst);

   // 2. set shutdownz = 1'b0 (default) 
   mipi_csi2_dw_dphy_shutdown(mipiCfg.mipiInst);
   
   //3. 4. 5.
   mipi_csi2_dw_pre_config(mipiCfg.mipiInst);

   // 6. set hsfreqrange[6:0]
   MIPI_DRVP_setDphyFreqRange(mipiCfg.mipiInst, dphy.pllMhz);     //add support for a lookuptable for hsfreqrange, valid and data var
   mipi_csi2_dw_test_code_program1(mipiCfg.mipiInst, 0x01, 0x20 );

   // 7. set 0xe5 bit 0 to 1'b1
   mipi_csi2_dw_test_code_program1(mipiCfg.mipiInst, 0xe5, 0x1 );

   // 8. set 0xe4[7:4] to 1
   mipi_csi2_dw_test_code_program1(mipiCfg.mipiInst, 0xe4, 0x10 );//cfgclkfreqrange = 27 set bit 4 1'b1
   
   // 9. set rx_rxlp_bias_prog_rw to 1'b1	  
   mipi_csi2_dw_test_code_program1(mipiCfg.mipiInst, 0x1ac, 0x4b ); //bit 6-->1  (set rx_rxlp_bias_prog_rw) bit 3,1,0-->1 default value

   // 10. set rxclk_rxhs_pull_long_channel_if_rw to 1'b1	
   mipi_csi2_dw_test_code_program1(mipiCfg.mipiInst, 0x307, 0x80 );

   // 11. DDL target oscillation frequency
   osc_freq_target = convert_bitrate_to_ddl_osc_freq_target(dphy.pllMhz);
   //LOGG_PRINT(LOG_INFO_E, NULL,"mipi osc_freq_target setting %dMHz (val 0x%x)\n", dphy.pllMhz,osc_freq_target);
   mipi_csi2_dw_test_code_program1(mipiCfg.mipiInst, 0xe2, (osc_freq_target&0xff) );
   mipi_csi2_dw_test_code_program1(mipiCfg.mipiInst, 0xe3, ((osc_freq_target&0xf00)>>8) );
   mipi_csi2_dw_test_code_program1(mipiCfg.mipiInst, 0xe4, 0x11 ); //(ddl_osc_freq_target_ovr_en_rw) set. bit 4, save value from 8.

   mipi_csi2_dw_set_num_lanes(mipiCfg.mipiInst,dphy.laneEnable-1);

   // 12. cfgclkfreqrange = 0x1C (default value)  //ELAD: default?
   // 13. Apply cfg clk
   //IAE_DRVG_rxClkEnCfg(mipiCfg.mipiInst);

   mipi_csi2_dw_resetout(mipiCfg.mipiInst); 

   // 14. set basedir_0 to 1'b1  
   IAE_DRVG_setBaseDir0(mipiCfg.mipiInst,1);//ELAD what we did in A0?
   
   // 15. set forcerxmode_n to 1'b1 
   IAE_DRVG_setForceXMode(mipiCfg.mipiInst,1);

   // 16. set all request inputs to zero
   //

   // 17. delay 15 ns
   OS_LYRG_usleep(1);

   // 18. set enable_n & enableclk to 1'b1
   IAE_DRVG_rxClkEnCfg(mipiCfg.mipiInst);
//   mipi_csi2_dw_set_num_lanes(mipiCfg.mipiInst,dphy.laneEnable-1);
   //After set num lanes we did on A0  mipi_csi2_dw_resetout(mipiCfg.mipiInst); 
   
   // 19. delay 15 ns
   OS_LYRG_usleep(1);
   
   // 20. set noext_burnin_res_cal to 1'b1     ###ofirg: we do use rext	   
   //dphy_write(RX, rx_num, 0x4, 0x10);

    // 21. delay 15 ns
   //OS_LYRG_usleep(1);

   // 22. set shutdownz = 1'b1
   mipi_csi2_dw_dphy_powerup(mipiCfg.mipiInst);

   // 23. delay 15 ns
   OS_LYRG_usleep(1);

   // 24. set rstz = 1'b1
   mipi_csi2_dw_dphy_resetout(mipiCfg.mipiInst);

   OS_LYRG_usleep(1);


      //25. Wait until stopstatedata_n
   MIPI_DRVP_waitStopState(mipiCfg.mipiInst,dphy.laneEnable); //TBD should we wait here for TX side?

   //26.  Set forcerxmode_n = 1'b0
   IAE_DRVG_setForceXMode(mipiCfg.mipiInst,0);

   //wait for enable_lanes (0xc9) to be 1 (VLSI)
   //MIPI_DRVP_waitEnableLane(mipiCfg.mipiInst);//TODO:check if necessary

   //in 4lanes mode, we config csi which config 2 dhys for 2lanes mode for each. here both of phys is reached to stop state (actually we checked one of them and our assumption that both in stop state)
   //now we should config the csi to 4 lanes.
   if (is4lanes)
   {
      LOGG_PRINT(LOG_INFO_E, NULL,"4 lane mode, csi %d configured to 4lane\n", mipiCfg.mipiInst);
      OS_LYRG_usleep(1);

      mipi_csi2_dw_set_num_lanes(mipiCfg.mipiInst,3);
   }
   
   LOGG_PRINT(LOG_DEBUG_E, ret,"mipi drv done enable\n");
   return ret;
}

/****************************************************************************
*
*  Function Name: MIPI_DRVG_resetRx
*
*  Description: 
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
ERRG_codeE MIPI_DRVG_resetRx(mipiInstConfigT mipiCfg)
{
   ERRG_codeE ret = MIPI__RET_SUCCESS;
   LOGG_PRINT(LOG_DEBUG_E, ret,"mipi drv disabling mipi %d\n",mipiCfg.mipiInst);
   mipi_csi2_dw_reset(mipiCfg.mipiInst);
   mipi_csi2_dw_dphy_reset(mipiCfg.mipiInst);
   mipi_csi2_dw_dphy_shutdown(mipiCfg.mipiInst);
   GMEG_mipiDphyDisable(mipiCfg.mipiInst);
   LOGG_PRINT(LOG_DEBUG_E, ret,"mipi drv done disable\n");
   return ret;
}



/****************************************************************************
*
*  Function Name: MIPI_DRVG_cfgTx
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
void dphy_write(dphyTypeE rx0tx1, int dphyNum, int address, int data)
{
    int controllerBaseAddress = 0, phyTestCtrl0Offset = 0, phyTestCtrl1Offset = 0;
 
    // set base address according to RX/TX num
    switch(dphyNum)
    {
        case 0: controllerBaseAddress = rx0tx1 ? DSI_0_BASE_ADDRESS : DPHY_0_BASE_ADDRESS; break;
        case 1: controllerBaseAddress = rx0tx1 ? DSI_1_BASE_ADDRESS : DPHY_1_BASE_ADDRESS; break;
        case 2: controllerBaseAddress = rx0tx1 ? DSI_2_BASE_ADDRESS : DPHY_2_BASE_ADDRESS; break;
        case 3: controllerBaseAddress = rx0tx1 ? DSI_3_BASE_ADDRESS : DPHY_3_BASE_ADDRESS; break;
        case 4: controllerBaseAddress =                               DPHY_4_BASE_ADDRESS; break;
        case 5: controllerBaseAddress =                               DPHY_5_BASE_ADDRESS; break;
    }

    // set registers offset according to RX/TX
    phyTestCtrl0Offset = rx0tx1 ? PHY_TST_CTRL0 : PHY_TEST_CTRL0;
    phyTestCtrl1Offset = rx0tx1 ? PHY_TST_CTRL1 : PHY_TEST_CTRL1;

    // 1. write testcode 4bit MSB
    APB_WRITE(controllerBaseAddress+phyTestCtrl1Offset, 0x10000);                   // testen  = 1
    APB_WRITE(controllerBaseAddress+phyTestCtrl0Offset, 0x2);                       // testclk = 1
    APB_WRITE(controllerBaseAddress+phyTestCtrl1Offset, 0x10000+0x00);              // testdin = 0x00 (address for MSB extension)
    APB_WRITE(controllerBaseAddress+phyTestCtrl0Offset, 0x0);                       // testclk = 0
    APB_WRITE(controllerBaseAddress+phyTestCtrl1Offset, 0x0);                       // testen  = 0
    APB_WRITE(controllerBaseAddress+phyTestCtrl1Offset, ((address&0xf00)>>8));      // testdin = {4'b0000, address[11:8]} (data of MSB extension)
    APB_WRITE(controllerBaseAddress+phyTestCtrl0Offset, 0x2);                       // testclk = 1

    // 2. write testcode 8bit LSB
    APB_WRITE(controllerBaseAddress+phyTestCtrl0Offset, 0x0);                       // testclk = 0
    APB_WRITE(controllerBaseAddress+phyTestCtrl1Offset, 0x10000);                   // testen  = 1
    APB_WRITE(controllerBaseAddress+phyTestCtrl0Offset, 0x2);                       // testclk = 1
    APB_WRITE(controllerBaseAddress+phyTestCtrl1Offset, 0x10000 + (address&0xff));  // testdin = {4'b0000, address[7:0]}
    APB_WRITE(controllerBaseAddress+phyTestCtrl0Offset, 0x0);                       // testclk = 0
    APB_WRITE(controllerBaseAddress+phyTestCtrl1Offset, (address&0xff));            // testen  = 0

    // 3. write data
    APB_WRITE(controllerBaseAddress+phyTestCtrl1Offset, data);                      // testdin = testdata
    APB_WRITE(controllerBaseAddress+phyTestCtrl0Offset, 0x2);                       // testclk = 1
    APB_WRITE(controllerBaseAddress+phyTestCtrl0Offset, 0x0);                       // testclk = 0
}

static int MIPI_DRVP_check_tx_ready(int tx_num, int num_lanes)
{
   unsigned int dsi_base_address = 0, val;
   unsigned int dphy_stopstateclk, dphy_stopstatedata0, dphy_stopstatedata1;
   unsigned int counter;

   switch(tx_num)
   {
      case 0: dsi_base_address = DSI_0_BASE_ADDRESS;
      break;
      case 1: dsi_base_address = DSI_1_BASE_ADDRESS;
      break;
      case 2: dsi_base_address = DSI_2_BASE_ADDRESS;
      break;
      case 3: dsi_base_address = DSI_3_BASE_ADDRESS;
      break;
   }

   counter =0;
   do 
   {
      val = APB_READ(dsi_base_address+PHY_STATUS);
      dphy_stopstateclk   = val & BIT(MIPI_DSI_STOP_STATE_CLOCK_BIT);
      dphy_stopstatedata0 = val & BIT(MIPI_DSI_STOP_STATE_LANE0_BIT);
      dphy_stopstatedata1 = (val & BIT(MIPI_DSI_STOP_STATE_LANE1_BIT)) | (num_lanes==1);
      counter += 1;
   //       OS_LYRG_usleep(2000);
   //       printf("dphy_stopstateclk = %d, dphy_stopstatedata0 = %d, dphy_stopstatedata1 = %d\n",
   //               dphy_stopstateclk, dphy_stopstatedata0, dphy_stopstatedata1);      
   }
   while ((!dphy_stopstateclk | !dphy_stopstatedata0 | !dphy_stopstatedata1) & (counter < MIPI_TIMEOUT));

   return (counter == MIPI_TIMEOUT);
}


static void MIPI_DRVP_config_csi_host (mipiInstConfigT *mipiCfgP )
{
  unsigned int dsi_base_address = 0,tx_base_address = 0, tx_num;

  tx_num = mipiCfgP->mipiInst;

  // set tx base address
  switch(tx_num)
    {
    case 0: 
      dsi_base_address = DSI_0_BASE_ADDRESS;
      tx_base_address  = TX_0_BASE_ADDRESS;
      break;
    case 2: 
      dsi_base_address = DSI_2_BASE_ADDRESS;
      tx_base_address  = TX_1_BASE_ADDRESS;
      break;
    case 3: 
      dsi_base_address = DSI_3_BASE_ADDRESS;
      tx_base_address  = TX_2_BASE_ADDRESS;
      break;
    }

  APB_WRITE(dsi_base_address+PWR_UP, 0x1);                              // dsi power up

  PPE_DRVG_csiControllerTxCfg(tx_num, mipiCfgP->svt.pktSizeBytes, mipiCfgP->svt.mipiVideoFormat, 0x200);

  // TX Controller configuration
  APB_WRITE(tx_base_address+0x10,((mipiCfgP->dphy.laneEnable-1) << 3) | (0x1 << 7));	        // Program the CFG for lanes, auto-increment frame number
  APB_WRITE(tx_base_address,1); // Enable Tx after commiting changes

  // Start transmitting high speed clock from dphy
  APB_WRITE(dsi_base_address+LPCLK_CTRL, 0x1);   
}


/****************************************************************************
*
*  Function Name: MIPI_DRVP_cfgPll
*
*  Description:
*   dwc_mipi_d_t2_tsmc12ffc18ns_databook.pdf:
*
*   3.3.6.1 for PLL config formula:
*   f_out = f_vco = (M / N) * f_clk
*   f_clk = 24Mhz
*   M = m+2
*   N = n+1
*   
*   Following limitations apply:  
*   1250Mhz >= f_out >= 320Mhz
*   N must be between 3 - 12  
*   
*   table 3-13 for other values
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
static void MIPI_DRVP_cfgPll( UINT32 tx_num, UINT32 pllMhz)
{
   dphy_write(TX, tx_num, 0x19, 0x30);  // pll_m_ovr_en & pll_n_ovr_en

   switch(pllMhz)
   {
      case(400):
      {
         //384Mhz: (m=158, M=160)  (n=9,N=10)
         dphy_write(TX, tx_num, 0x18, 0x84);  // pll_m_ovr [9:5] 
         dphy_write(TX, tx_num, 0x18, 0x1E);  // pll_m_ovr [4:0]  
         dphy_write(TX, tx_num, 0x17, 0x09);  // pll_n_ovr
         dphy_write(TX, tx_num, 0x12, 0x0F);  // vco_cntrl_ovr   - 001111
         dphy_write(TX, tx_num, 0x1c, 0x10);  // cpbias_cntrl[6:0] - 0010000
         dphy_write(TX, tx_num, 0x13, 0x10);  // gmp_cntrl[5:4] - 01
         dphy_write(TX, tx_num, 0x0e, 0x0b);  // prop_cntrl[5:0] - 001011
         dphy_write(TX, tx_num, 0x0f, 0x00);
         break;
      }

      case(600):
      {
         //600Mhz: (m=248, M=250)  (n=9,N=10)
         dphy_write(TX, tx_num, 0x18, 0x87);  // pll_m_ovr [9:5] 
         dphy_write(TX, tx_num, 0x18, 0x18);  // pll_m_ovr [4:0]  
         dphy_write(TX, tx_num, 0x17, 0x09);  // pll_n_ovr
         dphy_write(TX, tx_num, 0x12, 0x09);  // vco_cntrl_ovr   - 001001
         dphy_write(TX, tx_num, 0x1c, 0x10);  // cpbias_cntrl[6:0] - 0010000
         dphy_write(TX, tx_num, 0x13, 0x10);  // gmp_cntrl[5:4] - 01
         dphy_write(TX, tx_num, 0x0e, 0x0b);  // prop_cntrl[5:0] - 001011
         dphy_write(TX, tx_num, 0x0f, 0x00);         
         break;
      }

      case(700):
      {
         //696Mhz: (m=288, M=290)  (n=9,N=10)
         dphy_write(TX, tx_num, 0x18, 0x89);  // pll_m_ovr [9:5] 
         dphy_write(TX, tx_num, 0x18, 0x00);  // pll_m_ovr [4:0]  
         dphy_write(TX, tx_num, 0x17, 0x09);  // pll_n_ovr
         dphy_write(TX, tx_num, 0x12, 0x03);  // vco_cntrl_ovr   - 000011
         dphy_write(TX, tx_num, 0x1c, 0x10);  // cpbias_cntrl[6:0] - 0010000
         dphy_write(TX, tx_num, 0x13, 0x10);  // gmp_cntrl[5:4] - 01
         dphy_write(TX, tx_num, 0x0e, 0x0b);  // prop_cntrl[5:0] - 001011
         dphy_write(TX, tx_num, 0x0f, 0x00);         
         break;
      }

      case(800):
      {
         //792Mhz (m=328, M=330)  (n=9,N=10 )
         dphy_write(TX, tx_num, 0x18, 0x8a);  // pll_m_ovr [9:5]
         dphy_write(TX, tx_num, 0x18, 0x08);  // pll_m_ovr [4:0]
         dphy_write(TX, tx_num, 0x17, 0x09);  // pll_n_ovr
         dphy_write(TX, tx_num, 0x12, 0x03);  // vco_cntrl_ovr   - 000011
         dphy_write(TX, tx_num, 0x1c, 0x10);  // cpbias_cntrl[6:0] - 0010000
         dphy_write(TX, tx_num, 0x13, 0x10);  // gmp_cntrl[5:4] - 01
         dphy_write(TX, tx_num, 0x0e, 0x0b);  // prop_cntrl[5:0] - 001011
         dphy_write(TX, tx_num, 0x0f, 0x00);
         break;
      }

      case(900):
      {
         //888Mhz (m=368, M=370)  (n=9,N=10 )
         dphy_write(TX, tx_num, 0x18, 0x8b);  // pll_m_ovr [9:5]
         dphy_write(TX, tx_num, 0x18, 0x10);  // pll_m_ovr [4:0]
         dphy_write(TX, tx_num, 0x17, 0x09);  // pll_n_ovr
         dphy_write(TX, tx_num, 0x12, 0x03);  // vco_cntrl_ovr   - 000011
         dphy_write(TX, tx_num, 0x1c, 0x10);  // cpbias_cntrl[6:0] - 0010000
         dphy_write(TX, tx_num, 0x13, 0x10);  // gmp_cntrl[5:4] - 01
         dphy_write(TX, tx_num, 0x0e, 0x0b);  // prop_cntrl[5:0] - 001011
         dphy_write(TX, tx_num, 0x0f, 0x00);
         break;
      }


      case(1000):
      {
         //1008Mhz: (m=334, M=336)  (n=7,N=8)
         dphy_write(TX, tx_num, 0x18, 0x8a);  // pll_m_ovr [9:5] 
         dphy_write(TX, tx_num, 0x18, 0x0E);  // pll_m_ovr [4:0]  
         dphy_write(TX, tx_num, 0x17, 0x07);  // pll_n_ovr
         dphy_write(TX, tx_num, 0x12, 0x03);  // vco_cntrl_ovr   - 000011
         dphy_write(TX, tx_num, 0x1c, 0x10);  // cpbias_cntrl[6:0] - 0010000
         dphy_write(TX, tx_num, 0x13, 0x10);  // gmp_cntrl[5:4] - 01
         dphy_write(TX, tx_num, 0x0e, 0x0b);  // prop_cntrl[5:0] - 001011
         dphy_write(TX, tx_num, 0x0f, 0x00);         
         break;
      }

      case(1250):
      {
         //1250Mhz: (m=310, M=312)  (n=5,N=6)
         dphy_write(TX, tx_num, 0x18, 0x89);  // pll_m_ovr [9:5] 
         dphy_write(TX, tx_num, 0x18, 0x16);  // pll_m_ovr [4:0]  
         dphy_write(TX, tx_num, 0x17, 0x05);  // pll_n_ovr
         dphy_write(TX, tx_num, 0x12, 0x03);  // vco_cntrl_ovr   - 000001
         dphy_write(TX, tx_num, 0x1c, 0x10);  // cpbias_cntrl[6:0] - 0010000
         dphy_write(TX, tx_num, 0x13, 0x10);  // gmp_cntrl[5:4] - 01
         dphy_write(TX, tx_num, 0x0e, 0x0c);  // prop_cntrl[5:0] - 001100
         dphy_write(TX, tx_num, 0x0f, 0x00);          
         break;
      }

      default:
         assert(0);
         break;
   }

   dphy_write(TX, tx_num, 0x0d, 0x3);   // power up pll
}


/****************************************************************************
*
*  Function Name: MIPI_DRVP_hsFreqRange
*
*  Description: dwc_mipi_d_t2_tsmc12ffc18ns_databook.pdf table 5.7
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
static void MIPI_DRVP_hsFreqRange( UINT32 tx_num, UINT32 pllMhz)
{
   switch(pllMhz)
   {
      case(400):
      {
         dphy_write(TX, tx_num, 0x44, 0x89); //800
         break;
      }
      case(600):
      {
         dphy_write(TX, tx_num, 0x44, 0x8b); //1200
         break;
      }
      case(700):
      {
         dphy_write(TX, tx_num, 0x44, 0x8c); //1400
         break;
      }
      case(800):
      {
         dphy_write(TX, tx_num, 0x44, 0x8d); //1600
         break;
      }
      case(900):
      {
         dphy_write(TX, tx_num, 0x44, 0x8e); //1800
         break;
      }      
      case(1000):
      {
         dphy_write(TX, tx_num, 0x44, 0x8f); //2000
         break;
      }
      case(1250):
      {
         dphy_write(TX, tx_num, 0x44, 0xc9); //2500
         break;
      }
      default:
      {
         assert(0);
         break;
      }
   }
}

static UINT32 MIPI_DRVP_convertToGmeDiv(UINT16 mipi_format)
{
   switch(mipi_format)
   {
      case(RAW6):
      case(RAW7):
         return 0x1;
      case(YUV420_8_LEGACY):
      case(YUV420_8):
      case(RAW8):
         return 0x2;
      case(YUV422_8):
         return 0x5;
      case(YUV420_10):
      case(YUV422_10):
      case(RAW10):
         return 0x3;
      case(RAW12):
         return 0x4;
      case(RGB444):
      case(RGB555):
      case(RGB565):
         return 0x5;
      case(RGB888):
         return 0x6;
      default:
         assert(0);
         return 0;
   }
}


ERRG_codeE MIPI_DRVG_cfgTx( mipiInstConfigT *mipiCfgP )
{
   ERRG_codeE ret = MIPI__RET_SUCCESS;
   unsigned int dsi_base_address = 0, tx_base_address, csi_base_address, tx_num, phy_ready;

   tx_num = mipiCfgP->mipiInst;

   if (activeTxDphy[tx_num])
      return ret;

   // set tx base address
   switch(tx_num)
   {
      case 0: dsi_base_address = DSI_0_BASE_ADDRESS;
      break;
      case 1: dsi_base_address = DSI_1_BASE_ADDRESS;
      break;
      case 2: dsi_base_address = DSI_2_BASE_ADDRESS;
      break;
      case 3: dsi_base_address = DSI_3_BASE_ADDRESS;
      break;
      default:
      assert(0);
   }

    ret = GME_DRVG_enableClk((GME_DRVG_hwUnitE)(GME_DRVG_HW_MIPI_DPHY0_TX_CLK_EN_E + tx_num));
    if (ERRG_FAILED(ret))
    {
       LOGG_PRINT(LOG_ERROR_E, ret,"Failed to enable dphy %d clock\n",tx_num);
       return ret;
    }

    // set clock divider to match video format
    GMEG_mipiDphyTxConfig(tx_num, (( mipiCfgP->dphy.laneEnable - 1 ) << 3) | MIPI_DRVP_convertToGmeDiv(mipiCfgP->dphy.format));

    //
    // configure tx (transmit) dsi and phy
    //
    // configure dphy through dsi - dphy_write(RX/TX, dphy_num, address (offset), data)

    // 1. set rstz = 1'b0
    // 2. set shutdownz = 1'b0 
    APB_WRITE(dsi_base_address+PHY_RSTZ,0x0);
    
    // 3. set testclr = 1'b1 
    APB_WRITE(dsi_base_address+PHY_TST_CTRL0, 1);   
  
    // 4. wait 15ns  - ofirg: how much delay?
    OS_LYRG_usleep(1);

    // 5. set testclr = 1'b0
    APB_WRITE(dsi_base_address+PHY_TST_CTRL0, 0);    

    // 6. set hsfreqrange[6:0] = 7'b1001001
    MIPI_DRVP_hsFreqRange(tx_num, mipiCfgP->dphy.pllMhz);

    // 7a. bypass slew rate calibration
    dphy_write(TX, tx_num, 0xa0, 0x2);

    // 7b. disable slew rate calibration     ofirg: required?
    dphy_write(TX, tx_num, 0xa3, 0x0);

    // 8. set mpll_prg[0] = 1'b1
    dphy_write(TX, tx_num, 0x1f, 0x1); 

    // 9.  cfgclkfreqrange = 24 - signal is tied from TB

    // 10. cfg_clk signal - already activate{ppe_top_tb.ppe_0.ppe_core_0.mipi_tx_wrapper_0.DWC_mipi_dsi_host_dsi_tx_1.u_ph.u_dpips.vidregion[2:0]}d.

    // 11. PLL config
    MIPI_DRVP_cfgPll(tx_num, mipiCfgP->dphy.pllMhz);

    // 12. set basedir_0 = 1'b0  (TX PHYs tied to 1'b0)

    // 13. set all requests to zero - all should be zero.
    
    // 14. delay 15 ns
    OS_LYRG_usleep(1);

    // 15. set enable_n & enableclk to 1'b1 
    APB_WRITE(dsi_base_address+PHY_RSTZ, (0x1 << 2));
    APB_WRITE(dsi_base_address+PHY_IF_CFG, (mipiCfgP->dphy.laneEnable-1));

    // 16. delay 5ns
    OS_LYRG_usleep(1);

    // 17-18 is for the pattern generator
    // 17. set reverse data on lanes 0 & 1
    // dphy_write(TX, tx_num, 0x45, 0x4);  // set lane 0 reverse data
    // dphy_write(TX, tx_num, 0x55, 0x4);  // set lane 0 reverse data 
    // 18. configure pg_mode_r_lane - SYNOPSYS case 8001093471
//     dphy_write(TX, tx_num, 0x4a, 0x7); // pg_mode_r_lane0
    //dphy_write(TX, tx_num, 0x4a, 0x47); // pg_mode_r_lane0
    // dphy_write(TX, tx_num, 0x5a, 0x40); // pg_mode_r_lane1    
 
    // 19. delay 5 ns
    //OS_LYRG_usleep(1);

    //####### NU4000 B0 ONLY - CAL clock bug w/a #####
    // dphy_write(TX, tx_num, 0x21, 0x13);  // cb_cal_en_ovr & cb_cal_en_ovr_en
    // dphy_write(TX, tx_num, 0x22, 0x7C);  // cb_cal_repl_ovr_en & cb_cal_repl_ovr[3:0] = 4'b1111 (This is the stable value)

    // 20. set shutdownz = 1'b1 
    APB_WRITE(dsi_base_address+PHY_RSTZ,0x5);

    // 21. set rstz = 1'b1 (shutdownz remains 1'b1)
    APB_WRITE(dsi_base_address+PHY_RSTZ,0x7); 

    phy_ready = MIPI_DRVP_check_tx_ready (tx_num, mipiCfgP->dphy.laneEnable);
    if (!phy_ready)
    {
       LOGG_PRINT(LOG_INFO_E, NULL,  "Tx stop state detected\n");
    }
    else
    {
       LOGG_PRINT(LOG_ERROR_E, NULL,  "Failed to detect Tx stop state\n");
    }

    MIPI_DRVP_config_csi_host(mipiCfgP);

    activeTxDphy[tx_num] = 1;
    
    return ret;
}


/****************************************************************************
*
*  Function Name: MIPI_DRVG_resetTx
*
*  Description: 
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
ERRG_codeE MIPI_DRVG_resetTx(mipiInstConfigT *mipiCfgP)
{
   ERRG_codeE ret = MIPI__RET_SUCCESS;
   unsigned int dsi_base_address = 0, tx_num;

   tx_num = mipiCfgP->mipiInst;

   if (!activeTxDphy[tx_num])
      return ret;

   // set tx base address
   switch(tx_num)
   {
      case 0: dsi_base_address = DSI_0_BASE_ADDRESS;
      break;
      case 1: dsi_base_address = DSI_1_BASE_ADDRESS;
      break;
      case 2: dsi_base_address = DSI_2_BASE_ADDRESS;
      break;
      case 3: dsi_base_address = DSI_3_BASE_ADDRESS;
      break;
      default:
      assert(0);
   }

   /* Shutdown according to 7.3.1 */
   APB_WRITE(dsi_base_address+PHY_RSTZ,0x0);
   APB_WRITE(dsi_base_address+PHY_TST_CTRL0, 1);
   ret = GME_DRVG_disableClk((GME_DRVG_hwUnitE)(GME_DRVG_HW_MIPI_DPHY0_TX_CLK_EN_E + tx_num));
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret,"Failed to enable dphy %d clock\n",tx_num);
      return ret;
   }
   activeTxDphy[tx_num] = 0;
   LOGG_PRINT(LOG_INFO_E, NULL,"mipi tx disable done\n");
   return ret;
}



void MIPI_DRVG_showRegs(void)
{
   //GME regsisters
/*   GMEG_showMipiRegs();
   //IAE registes
   IAE_DRVG_showMipiRegs();
   //mipi registers
   mipi_csi2_svt_show_regs();
   mipi_csi2_dw_show_regs();*/
}

