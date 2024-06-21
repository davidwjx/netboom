#ifndef __MIPI_CSI2_TX_SVT_H__
#define __MIPI_CSI2_TX_SVT_H__

//Register offsets
#define MIPI_CSI2_SVT_EN           (0x00) //SVT_En_Reg
#define MIPI_CSI2_SVT_CFG          (0x10) //SVT_Cfg_Reg
#define MIPI_CSI2_SVT_TIMER_CFG_A  (0x14) //SVT_Timer_CFG_A_Reg
#define MIPI_CSI2_SVT_TIMER_CFG_B  (0x18) //SVT_Timer_CFG_B_Reg
#define MIPI_CSI2_SVT_TIMER_CFG_C  (0x1c) //SVT_Timer_CFG_C_Reg 
#define MIPI_CSI2_SVT_FS_HEADER        (0x24) //FS_Header 
#define MIPI_CSI2_SVT_FE_HEADER        (0x28) //FE_Header
#define MIPI_CSI2_SVT_LP_HEADER        (0x2C) //LP Header
#define MIPI_CSI2_SVT_FORCE_LANES      (0x30) //Force_Lanes_Reg
#define MIPI_CSI2_SVT_SOT_CONTROL      (0x40) //SOT_Control_Reg
#define MIPI_CSI2_SVT_CRC_CONTROL      (0x44) //CRC_Control_Reg
#define MIPI_CSI2_SVT_ECC_CONTROL      (0x48) //ECC_Control_Reg
#define MIPI_CSI2_SVT_STATUS           (0x80) //Status_Reg
#define MIPI_CSI2_SVT_VERSION          (0xFC) //Version

typedef enum
{
   mipi_csi2_svt_lane_clk_e = 0,
   mipi_csi2_svt_lane_1_e,
   mipi_csi2_svt_lane_2_e,
   mipi_csi2_svt_lane_3_e,
   mipi_csi2_svt_lane_4_e,
   
} mipi_csi2_svt_lane_e;

typedef enum
{
   mipi_csi2_svt_idle_e =0,
   mipi_csi2_svt_fs_sp_send_e,
   mipi_csi2_svt_fe_sp_send_e,
   mipi_csi2_svt_lp_send_e,
   mipi_csi2_svt_ulps_entry_send_e,
   mipi_csi2_svt_ulps_e
   
} mipi_csi2_svt_status_e;

UINT32 mipi_csi2_svt_read_reg(INU_DEFSG_mipiInstE inst,  UINT32 reg_offset);
void mipi_csi2_svt_write_reg(INU_DEFSG_mipiInstE inst,  UINT32 reg_offset, UINT32 val);
void mipi_csi2_svt_enable(INU_DEFSG_mipiInstE inst); 

void mipi_csi2_svt_disable(INU_DEFSG_mipiInstE inst);
void mipi_csi2_svt_config(INU_DEFSG_mipiInstE inst,unsigned int num_lanes, unsigned int hclk_stop, unsigned int vclk_stop, unsigned int auto_increment);
void mipi_csi2_svt_enter_ulps(INU_DEFSG_mipiInstE inst);
void mipi_csi2_svt_exit_ulps(INU_DEFSG_mipiInstE inst);
void mipi_csi2_svt_config_timer_a_params(INU_DEFSG_mipiInstE inst, unsigned int tlpx, unsigned int tclk_post, unsigned int tclk_pre, unsigned int tclk_prepare);
void mipi_csi2_svt_config_timer_b_params(INU_DEFSG_mipiInstE inst, unsigned int tclk_trail, unsigned int tclk_zero, unsigned int ths_prepare, unsigned int ths_trail);
void mipi_csi2_svt_config_timer_c_params(INU_DEFSG_mipiInstE inst, unsigned int tsync_hold);
void mipi_csi2_svt_set_fs_hdr(INU_DEFSG_mipiInstE inst,unsigned int data_type, unsigned int virtual_channel, unsigned int data);
void mipi_csi2_svt_set_fe_hdr(INU_DEFSG_mipiInstE inst,unsigned int data_type);
void mipi_csi2_svt_set_lp_hdr(INU_DEFSG_mipiInstE inst,unsigned int data_type, unsigned int virtual_channel, unsigned int word_count);
void mipi_csi2_svt_force_lanes(INU_DEFSG_mipiInstE inst, mipi_csi2_svt_lane_e lane, int force_hs, int hs, int force_lp, int lp_dp, int lp_dn);
mipi_csi2_svt_status_e mipi_csi2_svt_get_status(INU_DEFSG_mipiInstE inst);
void mipi_csi2_svt_clear_status(INU_DEFSG_mipiInstE inst);

void mipi_csi2_svt_show_inst_regs(INU_DEFSG_mipiInstE inst);
void mipi_csi2_svt_show_regs(void);
#endif //__mipi_csi2_svt_SVT_H__