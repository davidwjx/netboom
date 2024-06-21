#include "inu_common.h"
#include "mipi.h"
#include "mipi_csi2_dw.h"

#define DW_N_LANES_MASK  (0x3)

#define DW_DHPY_SHUTDOWN (0)
#define DW_DHPY_POWERUP     (1)

#define DW_DHPY_RESET (0)
#define DW_DHPY_RESETOUT (1)

#define DW_CSI2_RESET (0)
#define DW_CSI2_RESETOUT (1)

//#define DW_STATUS_ULP_POS              (0)
#define DW_STATUS_STOP_STATE_POS       (0)
//#define DW_STATUS_CLK_ACTIVE_POS       (8)
//#define DW_STATUS_CLK_ULP_POS          (9)
#define DW_STATUS_CLK_STOP_STATE_POS   (16)

#define DW_TEST_CTRL1_TESTDIN_POS    (0)
#define DW_TEST_CTRL1_TESTDOUT_POS   (8)
#define DW_TEST_CTRL1_TESTEN_POS     (16)

#define DW_TEST_CTRL1_TESTDIN_MASK     (0xFF)
#define DW_TEST_CTRL1_TESTDIN_UNMASK   (0xFFFFFF00)
#define DW_TEST_CTRL1_TESTDOUT_MASK    (0xFF)

#define DW_TEST_CTRL0_TESTCLR_POS   (0)
#define DW_TEST_CTRL0_TESTCLK_POS   (1)


static int mipi_csi2_dw_reg_addr(INU_DEFSG_mipiInstE inst,  UINT32 reg_addr)
{
    return (DPHY_0_BASE_ADDRESS + reg_addr + (inst * 0x1000));
}

static void write_reg(int addr, int wdata)
{
   volatile UINT32 *reg_addr = (volatile UINT32 *)addr;
   //printf("mipi_csi2_dw write_reg 0x%08x=0x%08x\n", reg_addr,wdata);
   *reg_addr = wdata;
}

static int read_reg(int addr)
{
   volatile UINT32 *reg_addr = (volatile UINT32 *)addr;
   return (*reg_addr);
}

UINT32 mipi_csi2_dw_read_reg(INU_DEFSG_mipiInstE inst,  UINT32 reg_offset)
{
   return read_reg(mipi_csi2_dw_reg_addr(inst, reg_offset));
}

void mipi_csi2_dw_write_reg(INU_DEFSG_mipiInstE inst,  UINT32 reg_offset, UINT32 val)
{
   write_reg(mipi_csi2_dw_reg_addr(inst, reg_offset), val);
}

void mipi_csi2_dw_set_num_lanes(INU_DEFSG_mipiInstE inst, unsigned int num_lanes)
{
   //Note: can only be called if d-phy is in stop-state
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_N_LANES), (num_lanes & DW_N_LANES_MASK));
}

void mipi_csi2_dw_dphy_shutdown(INU_DEFSG_mipiInstE inst)
{
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_SHUTDOWNZ), DW_DHPY_SHUTDOWN);
}

void mipi_csi2_dw_dphy_powerup(INU_DEFSG_mipiInstE inst)
{
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_SHUTDOWNZ), DW_DHPY_POWERUP);
}

void mipi_csi2_dw_dphy_reset(INU_DEFSG_mipiInstE inst)
{
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_DPHY_RSTZ), DW_DHPY_RESET);
}

void mipi_csi2_dw_dphy_resetout(INU_DEFSG_mipiInstE inst)
{
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_DPHY_RSTZ), DW_DHPY_RESETOUT);
}

void mipi_csi2_dw_reset(INU_DEFSG_mipiInstE inst)
{
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_CSI2_RESETN), DW_CSI2_RESET);
}

void mipi_csi2_dw_resetout(INU_DEFSG_mipiInstE inst)
{
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_CSI2_RESETN), DW_CSI2_RESETOUT);
}


mipi_csi2_phy_lane_state_e mipi_csi2_dw_get_lane_state(INU_DEFSG_mipiInstE inst, mipi_csi2_dw_lane_e lane)
{
   mipi_csi2_phy_lane_state_e ret = mipi_csi2_dw_lane_num_states_e;
   UINT32 phy_state = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_STATE));

   if(lane == mipi_csi2_dw_lane_clk_e)
   {
      if (phy_state & (1 << DW_STATUS_CLK_STOP_STATE_POS))
         ret = mipi_csi2_dw_lane_stop_state_e;
/*      else if((phy_state & (1 << DW_STATUS_CLK_ULP_POS)) == 0)
         ret = mipi_csi2_dw_lane_ulp_mode_e;
      else if((phy_state & (1 << DW_STATUS_CLK_ACTIVE_POS)))
         ret = mipi_csi2_dw_lane_activehs_e;*/
   }
   else
   {
      if (phy_state & (1 << (DW_STATUS_STOP_STATE_POS + lane -1)))
         ret = mipi_csi2_dw_lane_stop_state_e;
/*      else if(phy_state & (1 << (DW_STATUS_ULP_POS + lane -1)))
         ret = mipi_csi2_dw_lane_ulp_mode_e;
      else
         ret = mipi_csi2_dw_lane_activehs_e;*/
   }
   
   return ret;
}

UINT32 mipi_csi2_dw_get_err1(INU_DEFSG_mipiInstE inst)
{
   return read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_ERR_1)); 
}
void mipi_csi2_dw_set_msk1(INU_DEFSG_mipiInstE inst, UINT32 msk)
{
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_MSK_1),msk); 
}

UINT32 mipi_csi2_dw_get_err2(INU_DEFSG_mipiInstE inst)
{
   return read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_ERR_1)); 
}
void mipi_csi2_dw_set_msk2(INU_DEFSG_mipiInstE inst, UINT32 msk)
{
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_MSK_2),msk); 
}

void mipi_csi2_dw_test_code_program(INU_DEFSG_mipiInstE inst, UINT16 test_code, UINT8 *test_data)
{
   UINT32 val=0;
   //This sequence follows the synopsis datasheet, For writing the 4-bit testcode MSBs
   // Ensure that testclk and testen is set to low
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1));
   val &= ~(1<< DW_TEST_CTRL1_TESTEN_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0));
   val &= ~(1<< DW_TEST_CTRL0_TESTCLK_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0),val);

   //Set testen to high
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1));
   val |= (1<< DW_TEST_CTRL1_TESTEN_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   //Set testclk to high
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0));
   val |= (1<< DW_TEST_CTRL0_TESTCLK_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0),val);

   //Place 0x00 in testdin[7:0]
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1));
   val &= DW_TEST_CTRL1_TESTDIN_UNMASK;
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   //Set testclk to low
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0));
   val &= ~(1<< DW_TEST_CTRL0_TESTCLK_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0),val);

   //Set testen to low
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1));
   val &= ~(1<< DW_TEST_CTRL1_TESTEN_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   //Place the 8-bit word corresponding to the testcode MSBs in testdin
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1));
   val &= ~DW_TEST_CTRL1_TESTDIN_MASK;
   val |=((test_code >> 8) & DW_TEST_CTRL1_TESTDIN_MASK );
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   //Set testclk to high
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0));
   val |= (1<< DW_TEST_CTRL0_TESTCLK_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0),val);

   //For writing the 8-bit testcode LSBs:
   //Set testclk to low
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0));
   val &= ~(1<< DW_TEST_CTRL0_TESTCLK_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL0), val);

   //Set testen to high
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1));
   val |= (1<< DW_TEST_CTRL1_TESTEN_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   //Set testclk to high
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0));
   val |= (1<< DW_TEST_CTRL0_TESTCLK_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0),val);

   //Place the 8-bit word testcode(not test data) in testdin check
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1));
   val &= ~DW_TEST_CTRL1_TESTDIN_MASK;
   val |= (test_code & DW_TEST_CTRL1_TESTDIN_MASK);
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   //Set testclk to low
   val = 0;
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0),val);

   //Set testen to low
   val = 0;
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   //For writing the data
   //Place the 8-bit word corresponding to the page offset in testdin
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1));
   val &= ~DW_TEST_CTRL1_TESTDIN_MASK;
   val |= (*test_data & DW_TEST_CTRL1_TESTDIN_MASK);
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   //Set testclk to high
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0));
   val |= (1<< DW_TEST_CTRL0_TESTCLK_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0),val);

   LOGG_PRINT(LOG_DEBUG_E, NULL,"MIPI_CSI2_DW_PHY_TEST_CTRL1=%x \n",read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1)));
   LOGG_PRINT(LOG_DEBUG_E, NULL,"MIPI_CSI2_DW_PHY_TEST_CTRL0=%x \n",read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0)));
}


void mipi_csi2_dw_test_code_program1(INU_DEFSG_mipiInstE inst, UINT16 test_code, UINT8 test_data)
{
   mipi_csi2_dw_test_code_program(inst, test_code, &test_data);
}

UINT8 mipi_csi2_dw_test_code_read(INU_DEFSG_mipiInstE inst, UINT16 test_code)
{
   UINT32 val=0;
   UINT8 data;
   //This sequence follows the synopsis datasheet, For writing the 4-bit testcode MSBs
   // Ensure that testclk and testen is set to low
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1));
   val &= ~(1<< DW_TEST_CTRL1_TESTEN_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0));
   val &= ~(1<< DW_TEST_CTRL0_TESTCLK_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0),val);

   //Set testen to high
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1));
   val |= (1<< DW_TEST_CTRL1_TESTEN_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   //Set testclk to high
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0));
   val |= (1<< DW_TEST_CTRL0_TESTCLK_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0),val);

   //Place 0x00 in testdin[7:0]
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1));
   val &= DW_TEST_CTRL1_TESTDIN_UNMASK;
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   //Set testclk to low
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0));
   val &= ~(1<< DW_TEST_CTRL0_TESTCLK_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0),val);

   //Set testen to low
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1));
   val &= ~(1<< DW_TEST_CTRL1_TESTEN_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   //Place the 8-bit word corresponding to the testcode MSBs in testdin
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1));
   val &= ~DW_TEST_CTRL1_TESTDIN_MASK;
   val |=((test_code >> 8) & DW_TEST_CTRL1_TESTDIN_MASK );
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   //Set testclk to high
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0));
   val |= (1<< DW_TEST_CTRL0_TESTCLK_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0),val);

   //For writing the 8-bit testcode LSBs:
   //Set testclk to low
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0));
   val &= ~(1<< DW_TEST_CTRL0_TESTCLK_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL0), val);

   //Set testen to high
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1));
   val |= (1<< DW_TEST_CTRL1_TESTEN_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   //Set testclk to high
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0));
   val |= (1<< DW_TEST_CTRL0_TESTCLK_POS);
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0),val);

   //Place the 8-bit word testcode(not test data) in testdin check
   val = read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1));
   val &= ~DW_TEST_CTRL1_TESTDIN_MASK;
   val |= (test_code & DW_TEST_CTRL1_TESTDIN_MASK);
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   //Set testclk to low
   val = 0;
   write_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL0),val);

   //Set testen to low
   val = 0;
   write_reg(mipi_csi2_dw_reg_addr(inst,MIPI_CSI2_DW_PHY_TEST_CTRL1), val);

   data = (((UINT32)read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_TEST_CTRL1))) >> DW_TEST_CTRL1_TESTDOUT_POS) & DW_TEST_CTRL1_TESTDOUT_MASK;

   //printf("Read DPHY-%d's register 0x%x: 0x%x\n", inst, test_code, data);

   return data;
}



void mipi_csi2_dw_config_pll(INU_DEFSG_mipiInstE inst, mipi_csi2_dw_pll_e  pll_num)
{
   if(pll_num == mipi_csi2_dw_pll_80_e)
   {
      mipi_csi2_dw_test_code_program1(inst, 0x19, 0x30);
      mipi_csi2_dw_test_code_program1(inst, 0x44,0x0);
      mipi_csi2_dw_test_code_program1(inst, 0x17,0x2);
      mipi_csi2_dw_test_code_program1(inst, 0x18,0x9);
      mipi_csi2_dw_test_code_program1(inst, 0x18,0x80);
      mipi_csi2_dw_test_code_program1(inst, 0x12,0x0);
      mipi_csi2_dw_test_code_program1(inst, 0x10,0x0);
   }
   else if(pll_num == mipi_csi2_dw_pll_100_e)
   {
      mipi_csi2_dw_test_code_program1(inst,0x19,0x30);
      mipi_csi2_dw_test_code_program1(inst,0x44,0x40); 
      mipi_csi2_dw_test_code_program1(inst,0x17,0x3); 
      mipi_csi2_dw_test_code_program1(inst,0x18,0x11);
      mipi_csi2_dw_test_code_program1(inst,0x18,0x80);
      mipi_csi2_dw_test_code_program1(inst,0x12,0x0); 
      mipi_csi2_dw_test_code_program1(inst,0x10,0x0); 
   }
   else if(pll_num == mipi_csi2_dw_pll_200_e)
   {
      mipi_csi2_dw_test_code_program1(inst,0x19,0x30);
      mipi_csi2_dw_test_code_program1(inst,0x44,0x06); 
      mipi_csi2_dw_test_code_program1(inst,0x17,0x3); 
      mipi_csi2_dw_test_code_program1(inst,0x18,0x01);
      mipi_csi2_dw_test_code_program1(inst,0x18,0x81);
      mipi_csi2_dw_test_code_program1(inst,0x12,0x0); 
      mipi_csi2_dw_test_code_program1(inst,0x10,0x0); 
   }
   else if(pll_num == mipi_csi2_dw_pll_300_e)
   {
      mipi_csi2_dw_test_code_program1(inst,0x19,0x30);
      mipi_csi2_dw_test_code_program1(inst,0x44,0xa); 
      mipi_csi2_dw_test_code_program1(inst,0x17,0x3); 
      mipi_csi2_dw_test_code_program1(inst,0x18,0x11);
      mipi_csi2_dw_test_code_program1(inst,0x18,0x81);
      mipi_csi2_dw_test_code_program1(inst,0x12,0x0); 
      mipi_csi2_dw_test_code_program1(inst,0x10,0x0); 
   }
   else if(pll_num == mipi_csi2_dw_pll_500_e)
   {
      mipi_csi2_dw_test_code_program1(inst,0x19,0x30);
      mipi_csi2_dw_test_code_program1(inst,0x44,0xe); 
      mipi_csi2_dw_test_code_program1(inst,0x17,0x2); 
      mipi_csi2_dw_test_code_program1(inst,0x18,0x1f);
      mipi_csi2_dw_test_code_program1(inst,0x18,0x81);
      mipi_csi2_dw_test_code_program1(inst,0x12,0x0); 
      mipi_csi2_dw_test_code_program1(inst,0x10,0x0); 
   }
   else if(pll_num == mipi_csi2_dw_pll_650_e)
   {
      mipi_csi2_dw_test_code_program1(inst,0x19,0x30);
      mipi_csi2_dw_test_code_program1(inst,0x44,0x10);    
      mipi_csi2_dw_test_code_program1(inst,0x17,0x1); 
      mipi_csi2_dw_test_code_program1(inst,0x18,0x15);    
      mipi_csi2_dw_test_code_program1(inst,0x18,0x81);    
      mipi_csi2_dw_test_code_program1(inst,0x12,0x0);     
      mipi_csi2_dw_test_code_program1(inst,0x10,0x0);     
      mipi_csi2_dw_test_code_program1(inst,0x70,0x87);    
      mipi_csi2_dw_test_code_program1(inst,0x71,0xB5);    
      mipi_csi2_dw_test_code_program1(inst,0x72,0x53);    
      mipi_csi2_dw_test_code_program1(inst,0x73,0xD9);    
   }
   else if(pll_num == mipi_csi2_dw_pll_1000_e)
   {
      mipi_csi2_dw_test_code_program1(inst,0x19,0x30);   // D-PHY PLL Input and Loop Divider Ratio Control bypass
      mipi_csi2_dw_test_code_program1(inst,0x44,0x54);   // D-PHY frequency range for 1000-1050 mbps ;
      mipi_csi2_dw_test_code_program1(inst,0x17,0x2);    // D-PHY PLL divider ratio input divider (Freq=clk_frq/(input_divider+1))
      mipi_csi2_dw_test_code_program1(inst,0x18,0x1B);   // D-PHY PLL Loop (feedback) divider ratio (Freq=VCO_freq/(feedback_divider+1)). First half of register 9'bXXXX00000
      mipi_csi2_dw_test_code_program1(inst,0x18,0x83);   // D-PHY PLL Loop (feedback) divider ratio (Freq=VCO_freq/(feedback_divider+1)). Second half of register 9'b0000XXXXX
      mipi_csi2_dw_test_code_program1(inst,0x70,0x87);   // D-PHY HS Tx data lanes request state time T_LP counter threshold = 7?
      mipi_csi2_dw_test_code_program1(inst,0x71,0xB5);   // D-PHY HS Tx data lanes prepare state time T_HS_prepare counter threshold = 53?
      mipi_csi2_dw_test_code_program1(inst,0x72,0x53);   // D-PHY HS Tx data lanes HS-Zero state time T_HS_zero counter threshold = 19?
      mipi_csi2_dw_test_code_program1(inst,0x73,0xD9);   // D-PHY HS Tx data lanes trail state time T_HS_trail counter threshold = ?
      mipi_csi2_dw_test_code_program1(inst,0x12,0x0);    //PLL LPF and CP control -default values for hsfreqrange
      mipi_csi2_dw_test_code_program1(inst,0x10,0x0);    //VCO Control - defaults for hsfreqrange
   }
   else if(pll_num == mipi_csi2_dw_pll_1200_e)
   {
      mipi_csi2_dw_test_code_program1(inst,0x19,0x30);
      mipi_csi2_dw_test_code_program1(inst,0x44,0x36);
      mipi_csi2_dw_test_code_program1(inst,0x17,0x2); 
      mipi_csi2_dw_test_code_program1(inst,0x18,0x16);
      mipi_csi2_dw_test_code_program1(inst,0x18,0x84);
      mipi_csi2_dw_test_code_program1(inst,0x70,0x87);  
      mipi_csi2_dw_test_code_program1(inst,0x71,0xB5);   
      mipi_csi2_dw_test_code_program1(inst,0x72,0x53);   
      mipi_csi2_dw_test_code_program1(inst,0x73,0xD9);   
      mipi_csi2_dw_test_code_program1(inst,0x12,0x0); 
      mipi_csi2_dw_test_code_program1(inst,0x10,0x0);
   }
}

void mipi_csi2_dw_pre_config(INU_DEFSG_mipiInstE inst) 
{
   //Set testclr to low
   mipi_csi2_dw_write_reg(inst,MIPI_CSI2_DW_PHY_TEST_CTRL0,(0 << DW_TEST_CTRL0_TESTCLR_POS));
   // 3. set testclr = 1'b1 (default)
   mipi_csi2_dw_write_reg(inst,MIPI_CSI2_DW_PHY_TEST_CTRL0,(1 << DW_TEST_CTRL0_TESTCLR_POS));
   // 4. delay 15 ns
   OS_LYRG_usleep(1);
   // 5. set testclr = 1'b0
   mipi_csi2_dw_write_reg(inst,MIPI_CSI2_DW_PHY_TEST_CTRL0,(0 << DW_TEST_CTRL0_TESTCLR_POS));
}

void mipi_csi2_dw_show_regs()
{
   int i;

   for(i = 0; i < INU_DEFSG_MIPI_NUM_INSTANCES_E; i++)
   {
      LOGG_PRINT(LOG_INFO_E, NULL,"mipi csi2 dw regs inst %d\n",i);
      mipi_csi2_dw_show_inst_regs(i);
   }
}
void mipi_csi2_dw_show_inst_regs(INU_DEFSG_mipiInstE inst)
{
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_DW_VERSION 0x%08x = version 0x%08x\n",mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_VERSION), read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_VERSION)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_DW_N_LANES 0x%08x = 0x%08x\n",mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_N_LANES), read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_N_LANES)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_DW_PHY_SHUTDOWNZ 0x%08x = 0x%08x\n",mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_SHUTDOWNZ), read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_SHUTDOWNZ)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_DW_DPHY_RSTZ 0x%08x = 0x%08x\n",mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_DPHY_RSTZ), read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_DPHY_RSTZ)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_DW_CSI2_RESETN 0x%08x = 0x%08x\n",mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_CSI2_RESETN), read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_CSI2_RESETN)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_DW_PHY_STATE 0x%08x = 0x%08x\n",mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_STATE), read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_PHY_STATE)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_DW_ERR_1 0x%08x = 0x%08x\n",mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_ERR_1), read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_ERR_1)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_DW_ERR_2 0x%08x = 0x%08x\n",mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_ERR_2), read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_ERR_2)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_DW_MSK_1 0x%08x = 0x%08x\n",mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_MSK_1), read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_MSK_1)));
   LOGG_PRINT(LOG_INFO_E, NULL, "MIPI_CSI2_DW_MSK_2 0x%08x = 0x%08x\n",mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_MSK_2), read_reg(mipi_csi2_dw_reg_addr(inst, MIPI_CSI2_DW_MSK_2)));
}

