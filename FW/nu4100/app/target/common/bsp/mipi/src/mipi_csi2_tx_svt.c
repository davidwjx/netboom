#include "inu_common.h"

#include "mipi.h"
#include "mipi_csi2_tx_svt.h"

#define SVT_ENABLE  (1)
#define SVT_DISABLE (0)
#define SVT_ENABLE_MASK  (0x1)

#define SVT_CFG_LANES_POS      (3)
#define SVT_CFG_HCLK_STOP_POS  (5)
#define SVT_CFG_VCLK_STOP_POS  (6)
#define SVT_CFG_AUTO_INCR_POS  (7)
#define SVT_CFG_ENTER_ULPS_POS  (8)
#define SVT_CFG_LANES_MASK     (0x3)
#define SVT_CFG_HCLK_STOP_MASK  (0x1)
#define SVT_CFG_VCLK_STOP_MASK  (0x1)
#define SVT_CFG_AUTO_INCR_MASK  (0x1)
#define SVT_CFG_ENTER_ULPS_MASK  (0x1)

#define SVT_ENTER_ULPS (1)
#define SVT_EXIT_ULPS  (0)

#define SVT_TIMER_A_TLPX_POS         (0)
#define SVT_TIMER_A_TCLK_POST_POS    (8)
#define SVT_TIMER_A_TCLK_PRE_POS     (16)
#define SVT_TIMER_A_TCLK_PREPARE_POS (24)

#define SVT_TIMER_B_TCLK_TRAIL_POS   (0)
#define SVT_TIMER_B_TCLK_ZERO_POS    (8)
#define SVT_TIMER_B_THS_PREPARE_POS  (16)
#define SVT_TIMER_B_THS_TRAIL_POS    (24)

#define SVT_TIMER_C_TSYNC_HOLD_SP_POS   (0)

#define SVT_FS_HEADER_DT_POS     (0)
#define SVT_FS_HEADER_VC_POS     (6)
#define SVT_FS_HEADER_DATA_POS   (8)
#define SVT_FS_HEADER_DT_MASK    (0x3f)
#define SVT_FS_HEADER_VC_MASK    (0x3)
#define SVT_FS_HEADER_DATA_MASK  (0xffff)

#define SVT_FE_HEADER_DT_POS     (0)
#define SVT_FE_HEADER_DT_MASK    (0x3f)

#define SVT_LP_HEADER_DT_POS     (0)
#define SVT_LP_HEADER_VC_POS     (6)
#define SVT_LP_HEADER_WC_POS     (8)
#define SVT_LP_HEADER_DT_MASK    (0x3f)
#define SVT_LP_HEADER_VC_MASK    (0x3)
#define SVT_LP_HEADER_WC_MASK    (0xffff)

#define SVT_FORCE_LANES_FORCE_HS_POS     (0)
#define SVT_FORCE_LANES_CLK_HS_POS       (1)
#define SVT_FORCE_LANES_FORCE_LP_POS     (2)
#define SVT_FORCE_LANES_DP_POS           (3)
#define SVT_FORCE_LANES_DN_POS           (4)
#define SVT_FORCE_LANES_LANE_BITS        (6)
#define SVT_FORCE_LANES_LANE_MASK        (0x1F)

static int mipi_csi2_svt_reg_addr(INU_DEFSG_mipiInstE inst,  UINT32 reg_addr)
{
    return (TX_0_BASE_ADDRESS + reg_addr + (inst << 10));
}

static void write_reg(int addr, int wdata)
{
   volatile UINT32 *reg_addr = (volatile UINT32 *)addr;
   //printf("mipi_csi2_svt write_reg 0x%08x=0x%08x\n", addr,wdata);
   *reg_addr = wdata;
}

static int read_reg(int addr)
{
   volatile UINT32 *reg_addr = (volatile UINT32 *)addr;
   return (*reg_addr);
}

UINT32 mipi_csi2_svt_read_reg(INU_DEFSG_mipiInstE inst,  UINT32 reg_offset)
{
   return read_reg(mipi_csi2_svt_reg_addr(inst, reg_offset));
}

void mipi_csi2_svt_write_reg(INU_DEFSG_mipiInstE inst,  UINT32 reg_offset, UINT32 val)
{
   write_reg(mipi_csi2_svt_reg_addr(inst, reg_offset), val);
}

void mipi_csi2_svt_enable(INU_DEFSG_mipiInstE inst)
{
   write_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_EN), SVT_ENABLE); 
}

void mipi_csi2_svt_disable(INU_DEFSG_mipiInstE inst)
{
   write_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_EN),SVT_DISABLE); 
}

void mipi_csi2_svt_config(INU_DEFSG_mipiInstE inst,unsigned int num_lanes, unsigned int hclk_stop, unsigned int vclk_stop, unsigned int auto_increment)
{
   UINT32 curr_cfg = 0;
   UINT32 write_cfg;
   curr_cfg = read_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_CFG)); 
   
   write_cfg =  (((num_lanes & SVT_CFG_LANES_MASK)<< SVT_CFG_LANES_POS) |
                 ((hclk_stop & SVT_CFG_HCLK_STOP_MASK)<< SVT_CFG_HCLK_STOP_POS) |
                 ((vclk_stop & SVT_CFG_VCLK_STOP_MASK)<< SVT_CFG_VCLK_STOP_POS) |
                 ((auto_increment & SVT_CFG_AUTO_INCR_MASK) << SVT_CFG_AUTO_INCR_POS) |
                 (curr_cfg & SVT_CFG_ENTER_ULPS_MASK));
                 
   write_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_CFG),write_cfg); 
}

void mipi_csi2_svt_enter_ulps(INU_DEFSG_mipiInstE inst)
{
   UINT32 curr_cfg = 0;
   UINT32 write_cfg;

   curr_cfg = read_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_CFG));    
   write_cfg = curr_cfg | (SVT_ENTER_ULPS << SVT_CFG_ENTER_ULPS_POS);

   write_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_FORCE_LANES),(UINT32)~0); 
   write_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_CFG),write_cfg); 
}

void mipi_csi2_svt_exit_ulps(INU_DEFSG_mipiInstE inst)
{
   UINT32 curr_cfg = 0;
   UINT32 write_cfg;

   curr_cfg = read_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_CFG));    
   write_cfg = curr_cfg | (SVT_EXIT_ULPS << SVT_CFG_ENTER_ULPS_POS);

   write_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_FORCE_LANES),0); 
   write_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_CFG),write_cfg); 
}

void mipi_csi2_svt_config_timer_a_params(INU_DEFSG_mipiInstE inst, unsigned int tlpx, unsigned int tclk_post, unsigned int tclk_pre, unsigned int tclk_prepare)
{
   UINT32 write_val = ((tlpx << SVT_TIMER_A_TLPX_POS) | 
                       (tclk_post << SVT_TIMER_A_TCLK_POST_POS) |
                       (tclk_pre << SVT_TIMER_A_TCLK_PRE_POS) |
                       (tclk_prepare << SVT_TIMER_A_TCLK_PREPARE_POS));
                       
   write_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_TIMER_CFG_A), write_val); 
}

void mipi_csi2_svt_config_timer_b_params(INU_DEFSG_mipiInstE inst, unsigned int tclk_trail, unsigned int tclk_zero, unsigned int ths_prepare, unsigned int ths_trail)
{
   UINT32 write_val = ((tclk_trail << SVT_TIMER_B_TCLK_TRAIL_POS) | 
                       (tclk_zero << SVT_TIMER_B_TCLK_ZERO_POS) |
                       (ths_prepare << SVT_TIMER_B_THS_PREPARE_POS) |
                       (ths_trail << SVT_TIMER_B_THS_TRAIL_POS));
                       
   write_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_TIMER_CFG_B), write_val); 
}

void mipi_csi2_svt_config_timer_c_params(INU_DEFSG_mipiInstE inst, unsigned int tsync_hold)
{
   UINT32 write_val = (tsync_hold << SVT_TIMER_C_TSYNC_HOLD_SP_POS);
                       
   write_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_TIMER_CFG_C), write_val); 
}

void mipi_csi2_svt_set_fs_hdr(INU_DEFSG_mipiInstE inst,unsigned int data_type, unsigned int virtual_channel, unsigned int data)
{
   UINT32 write_val = (((data_type & SVT_FS_HEADER_DT_MASK) << SVT_FS_HEADER_DT_POS) | 
                     ((virtual_channel & SVT_FS_HEADER_VC_MASK) << SVT_FS_HEADER_VC_POS) |  
                     ((data & SVT_FS_HEADER_DATA_MASK) << SVT_FS_HEADER_DATA_POS));
   write_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_FS_HEADER), write_val);
}

void mipi_csi2_svt_set_fe_hdr(INU_DEFSG_mipiInstE inst,unsigned int data_type)
{
   UINT32 write_val = (data_type & SVT_FE_HEADER_DT_MASK)<< SVT_FE_HEADER_DT_POS;
   write_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_FE_HEADER), write_val);
}

void mipi_csi2_svt_set_lp_hdr(INU_DEFSG_mipiInstE inst,unsigned int data_type, unsigned int virtual_channel, unsigned int word_count)
{
   UINT32 write_val = (((data_type & SVT_LP_HEADER_DT_MASK) << SVT_LP_HEADER_DT_POS) | 
                     ((virtual_channel & SVT_LP_HEADER_VC_MASK) << SVT_LP_HEADER_VC_POS) |  
                     ((word_count & SVT_LP_HEADER_WC_MASK) << SVT_LP_HEADER_WC_POS));

   write_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_LP_HEADER), write_val);
}

void mipi_csi2_svt_force_lanes(INU_DEFSG_mipiInstE inst, mipi_csi2_svt_lane_e lane, int force_hs, int hs, int force_lp, int lp_dp, int lp_dn)
{
   UINT32 lane_cfg = ((force_hs << SVT_FORCE_LANES_FORCE_HS_POS) | (hs << SVT_FORCE_LANES_CLK_HS_POS) |  
                      (force_lp << SVT_FORCE_LANES_FORCE_LP_POS) | (lp_dp << SVT_FORCE_LANES_DP_POS) | (lp_dn << SVT_FORCE_LANES_DN_POS));
   UINT32 force_lanes;
   
   force_lanes = read_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_FORCE_LANES));
   force_lanes &= ~((SVT_FORCE_LANES_LANE_MASK << (lane * SVT_FORCE_LANES_LANE_BITS)));
   force_lanes |= (lane_cfg << (lane * SVT_FORCE_LANES_LANE_BITS));
   write_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_FORCE_LANES), force_lanes);
}

mipi_csi2_svt_status_e mipi_csi2_svt_get_status(INU_DEFSG_mipiInstE inst)
{
   return read_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_STATUS));
}

void mipi_csi2_svt_clear_status(INU_DEFSG_mipiInstE inst)
{
   return write_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_STATUS),0);
}

void mipi_csi2_svt_show_regs()
{
   int i;

   for(i = 0; i < INU_DEFSG_MIPI_NUM_INSTANCES_E; i++)
   {
      LOGG_PRINT(LOG_INFO_E, NULL,"mipi csi2 svt regs inst %d\n",i);
      mipi_csi2_svt_show_inst_regs(i);
   }
}

void mipi_csi2_svt_show_inst_regs(INU_DEFSG_mipiInstE inst)
{
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_SVT_EN 0x%08x = 0x%08x\n", mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_EN), read_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_EN)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_SVT_CFG 0x%08x = 0x%08x\n",mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_CFG), read_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_CFG)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_SVT_TIMER_CFG_A 0x%08x = 0x%08x\n",mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_TIMER_CFG_A), read_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_TIMER_CFG_A)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_SVT_TIMER_CFG_B 0x%08x = 0x%08x\n",mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_TIMER_CFG_B), read_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_TIMER_CFG_B)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_SVT_TIMER_CFG_C 0x%08x = 0x%08x\n",mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_TIMER_CFG_C), read_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_TIMER_CFG_C)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_SVT_FS_HEADER 0x%08x = 0x%08x\n",mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_FS_HEADER), read_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_FS_HEADER)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_SVT_FE_HEADER 0x%08x = 0x%08x\n",mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_FE_HEADER), read_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_FE_HEADER)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_SVT_LP_HEADER 0x%08x = 0x%08x\n",mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_LP_HEADER), read_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_LP_HEADER)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_SVT_STATUS 0x%08x = 0x%08x\n",mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_STATUS), read_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_STATUS)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_SVT_VERSION 0x%08x = 0x%08x\n",mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_VERSION), read_reg(mipi_csi2_svt_reg_addr(inst, MIPI_CSI2_SVT_VERSION)));
}














