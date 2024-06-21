#ifndef __MIPI_CSI2_DW__
#define __MIPI_CSI2_DW__

//Register offsets
#define MIPI_CSI2_DW_VERSION          (0x00) //Version
#define MIPI_CSI2_DW_N_LANES          (0x04) //Number of active lanes
#define MIPI_CSI2_DW_PHY_SHUTDOWNZ    (0x40) //Phy shudown control
#define MIPI_CSI2_DW_DPHY_RSTZ        (0x44) //Dephy reset control
#define MIPI_CSI2_DW_CSI2_RESETN      (0x08) //CSI-2 controller reset
#define MIPI_CSI2_DW_PHY_STATE        (0x4c) 
//#define MIPI_CSI2_DW_DATA_IDS_1       (0x18) //not in hw
//#define MIPI_CSI2_DW_DATA_IDS_2       (0x1c) //no in hw
#define MIPI_CSI2_DW_ERR_1            (0x20) //Error state register 1
#define MIPI_CSI2_DW_ERR_2            (0x24) //Error state register 2
#define MIPI_CSI2_DW_MSK_1            (0x28) //Mask for error 1
#define MIPI_CSI2_DW_MSK_2            (0x2c) //Mask for error 2
#define MIPI_CSI2_DW_PHY_TEST_CTRL0   (0x50) 
#define MIPI_CSI2_DW_PHY_TEST_CTRL1   (0x54) 

typedef enum
{
   mipi_csi2_dw_lane_clk_e = 0,
   mipi_csi2_dw_lane_1_e,
   mipi_csi2_dw_lane_2_e,
   mipi_csi2_dw_lane_3_e,
   mipi_csi2_dw_lane_4_e,
} mipi_csi2_dw_lane_e;

typedef enum
{
   mipi_csi2_dw_lane_ulp_mode_e =0,
   mipi_csi2_dw_lane_stop_state_e,
   mipi_csi2_dw_lane_activehs_e,

   mipi_csi2_dw_lane_num_states_e,
} mipi_csi2_phy_lane_state_e;

typedef enum
{
   mipi_csi2_dw_pll_80_e = 0,
   mipi_csi2_dw_pll_100_e,
   mipi_csi2_dw_pll_200_e,
   mipi_csi2_dw_pll_300_e,
   mipi_csi2_dw_pll_500_e,
   mipi_csi2_dw_pll_650_e,
   mipi_csi2_dw_pll_1000_e,
   mipi_csi2_dw_pll_1200_e,
   mipi_csi2_dw_num_pll_e,
} mipi_csi2_dw_pll_e;

void mipi_csi2_dw_write_reg(INU_DEFSG_mipiInstE inst,  UINT32 reg_offset, UINT32 val);
UINT32 mipi_csi2_dw_read_reg(INU_DEFSG_mipiInstE inst,  UINT32 reg_offset);
void mipi_csi2_dw_set_num_lanes(INU_DEFSG_mipiInstE inst, unsigned int num_lanes);
void mipi_csi2_dw_dphy_shutdown(INU_DEFSG_mipiInstE inst);
void mipi_csi2_dw_dphy_powerup(INU_DEFSG_mipiInstE inst);
void mipi_csi2_dw_dphy_reset(INU_DEFSG_mipiInstE inst);
void mipi_csi2_dw_dphy_resetout(INU_DEFSG_mipiInstE inst);
void mipi_csi2_dw_reset(INU_DEFSG_mipiInstE inst);
void mipi_csi2_dw_resetout(INU_DEFSG_mipiInstE inst);
mipi_csi2_phy_lane_state_e mipi_csi2_dw_get_lane_state(INU_DEFSG_mipiInstE inst, mipi_csi2_dw_lane_e lane);
UINT32 mipi_csi2_dw_get_err1(INU_DEFSG_mipiInstE inst);
void mipi_csi2_dw_set_msk1(INU_DEFSG_mipiInstE inst, UINT32 msk);
UINT32 mipi_csi2_dw_get_err2(INU_DEFSG_mipiInstE inst);
void mipi_csi2_dw_set_msk2(INU_DEFSG_mipiInstE inst, UINT32 msk);
void mipi_csi2_dw_test_code_program(INU_DEFSG_mipiInstE inst, UINT16 test_code, UINT8 *test_data);
void mipi_csi2_dw_test_code_program1(INU_DEFSG_mipiInstE inst, UINT16 test_code, UINT8 test_data);
UINT8 mipi_csi2_dw_test_code_read(INU_DEFSG_mipiInstE inst, UINT16 test_code);
void mipi_csi2_dw_pre_config(INU_DEFSG_mipiInstE inst);
void mipi_csi2_dw_config_pll(INU_DEFSG_mipiInstE inst, mipi_csi2_dw_pll_e  pll_num);

void mipi_csi2_dw_show_inst_regs(INU_DEFSG_mipiInstE inst);
void mipi_csi2_dw_show_regs(void);

#endif //__MIPI_CSI2_DW__
