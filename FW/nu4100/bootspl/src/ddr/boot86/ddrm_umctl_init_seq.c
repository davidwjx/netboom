
#include "nu4000_c0_gme_regs.h"
#include "nu4000_c0_ddrc_mp_regs.h"
#include "nu4000_c0_ddrc_regs.h"
#include "nu4000_c0_ddrp_regs.h"
#include "phyinit.h"
#include "debug.h"

#define __GENERIC_ADDRESS_MAPPING__

#define __NEW_WHILE_LOOP__


// Number of columns, rows and banks of connected DDR die
#define DDR_COLUMN_NUM	(10)
#define DDR_ROW_NUM		(15)
#define DDR_BANK_NUM	(3)

#define DDRM_MAX_ITERATIONS		(0x100000)

#define DDRM_UMCTL_WHILE_LOOP_LIMITED(COND, MAX_ITERATIONS)   {  \
                                                               volatile UINT32   iteration = 0; \
                                                               while((COND) && (iteration++ < MAX_ITERATIONS)) \
                                                               { \
                                                               }\
                                                               if(iteration >= MAX_ITERATIONS) \
                                                               {\
                                                                   debug_printf("ddrm_umctl: Condition failed at line %d", __LINE__ );\
                                                               }\
                                                             };

static void set_registers_for_xxxx_Mbps (unsigned int freq);
static void set_rfshtmg (unsigned int freq);
static void ddrc_config_address_mapping (unsigned int col, unsigned int row, unsigned int bank);
#ifndef __GENERIC_ADDRESS_MAPPING__
static void ddrc_config_address_mapping_old (void);
#endif

void pm_set_registers_for_xxxx_Mbps (unsigned int freq)
{
  set_registers_for_xxxx_Mbps (freq);
}

void pm_set_rfshtmg (unsigned int freq)
{
  set_rfshtmg(freq);
}

void pm_ddrc_config_address_mapping (unsigned int col, unsigned int row, unsigned int bank)
{
  ddrc_config_address_mapping(col, row, bank);
}

// ==========================================================================================
// DDR MCTL configuration
// Derived from Inomize's "ddrm_umtcl_init_seq.sv"
// MCTL = Synopsys DesignWare Cores Enhanced Universal DDR Memory Controller (uMCTL2).
// Synopsys doc name: DWC_ddr_umctl2_databook.pdf
// Doc location: http://svn/repos/inuitive/db/NU4000B0/vlsi/ddrm/trunk/doc/IP/Synopsys
//
unsigned int ddrm_umctl_init_seq (unsigned int freq) {
  
  volatile unsigned int data_val;
#ifdef __NEW_WHILE_LOOP__
  volatile unsigned int iteration = 0;
#else
  volatile unsigned int delay_cnt;
#endif  
  unsigned int res;
  GME_SPARE_0_SPARE_BITS_W(0xa02a0001); // write
  
  // --------------------------------------------
  // DDR-Controller APB reset negation by S/W
  GME_DDRM_CONTROL_HOLD_DDRC_APB_RESET_W(0);  // Register: ddrm_control, Field: hold_ddrc_apb_reset: "release reset when this bit is set to 0"
  
  
  // Now initialize DDRC according to the steps of
  // uMCTL2 Databook Table 6-7 - "DWC_ddr_umctl2 and Memory Initialization with LPDDR4 mPHY_v2":
  
  
  // --------------------------------------------
  // Step 1 - Follow the PHYs power up procedure
  // --------------------------------------------

  // --------------------------------------------
  // Step 2 - Program the DWC_ddr_umctl2 registers
  // --------------------------------------------

  // When running training with the PHY. The following controller registers must be programmed to these values at this stage:
  //INIT0.skip_dram_init = 2b11
  //PWRCTL.selfref_sw = 1b1
  
  DDRC_INIT0_SKIP_DRAM_INIT_W(0x3);
  DDRC_PWRCTL_VAL = 0x00000001; // set bit "selfref_en" Put SDRAM in self-refresh

  // Debug Register1 (1):
  DDRC_DBG1_VAL = 0x00000001; // set bit "dis_dq" to prevent reads/writes
  
  // Low Power Control Register (1)
  
  // Operating Mode Status Register
  // check status is clean and in init state
  data_val = DDRC_STAT_VAL; // read status
  // Status=0, so:
  // CAMs empty,
  // SDRAM is not in Self Refresh (I guess, due to the delay)
  // SDRAM is not in SR-Powerdown
  // SDRAM Operating mode is "Init"
  if (data_val != 0 ) {
    GME_SPARE_0_SPARE_BITS_W(0xa02abad2); // write
    GME_SPARE_0_SPARE_BITS_W(data_val); // write value to observe
//	debug_printf("ddrm_umctl_init_seq: DDRC_STAT_VAL = %x\n", data_val);
  }
  
  // Master reg0
  DDRC_MSTR_LPDDR4_W(0x1); // set lpddr4
  DDRC_MSTR_BURST_RDWR_W(0x8); // burst_rdwr: Burst length=16
  
  // Mode Register Read/Write Control Register 0.
  // Mode Register Read/Write Control Register 1.
  
  // Temperature Derate Enable Register
  DDRC_DERATEEN_RC_DERATE_VALUE_W(0x2); // rc_derate_value=2 (Derate value of tRC for LPDDR4: Derating uses +3)
  DDRC_DERATEEN_DERATE_VALUE_W(0x1);    // derate_value=1 (Derate value: Derating uses +2)
  DDRC_DERATEEN_DERATE_ENABLE_W(0x1);   // derate_enable=1
  
  // Temperature Derate Interval Register
  DDRC_DERATEINT_VAL = 0x9f1a8a5f; // Interval between two MR4 reads, measured in DFI clock cycles. Must not be set to zero!
  
  // Temperature Derate Control Register
  DDRC_DERATECTL_VAL = 0x00000001; // Interrupt enable bit (according to spec, "1" is its reset value)
  
  // Low Power Control Register (2)
  DDRC_PWRCTL_LPDDR4_SR_ALLOWED_W(0x1); // lpddr4_sr_allowed=1
  DDRC_PWRCTL_DIS_CAM_DRAIN_SELFREF_W(0x1); // dis_cam_drain_selfref=1
  DDRC_PWRCTL_SELFREF_SW_W(0x1); // selfref_sw=1
  DDRC_PWRCTL_POWERDOWN_EN_W(0x1); // powerdown_en=1
  DDRC_PWRCTL_SELFREF_EN_W(0x0); // selfref_en=0
  
  // Low Power Timing Register
  DDRC_PWRTMG_VAL = 0x000b7600; // selfref_to_x32=0xb, t_dpd_x4096=0x76, powerdown_to_x32=0
  
  // Hardware Low Power Control Register
  DDRC_HWLPCTL_VAL = 0x00800001; // hw_lp_idle_x32=0x080, hw_lp_exit_idle_en=0, hw_lp_en=1
      
  set_rfshtmg (freq);
  
  // CRC Parity Control Register0
  DDRC_CRCPARCTL0_VAL = 0x00000000; // retry_ctrlupd_enable=0
  
  // DIMM Control Register
  DDRC_DIMMCTL_VAL = 0x00000000; // this seems to be the reset value...(hence NO DIMM in test-bench) 
  
  // DFI Low Power Configuration Register0
  DDRC_DFILPCFG0_VAL = 0x07500030; // dfi_tlp_resp=0x07, dfi_lp_wakeup_dpd=0x5, dfi_lp_en_dpd=0, dfi_lp_wakeup_sr=0x0, dfi_lp_en_sr=0, dfi_lp_wakeup_pd=0x3, dfi_lp_en_pd=0
  
  // DFI Update Register0
  DDRC_DFIUPD0_VAL = 0x40400018; // dis_auto_ctrlupd=0, dis_auto_ctrlupd_srx=1, ctrlupd_pre_srx=0, dfi_t_ctrlup_max=0x040, dfi_t_ctrlup_min=0x018
  
  // DFI Update Register1
  DDRC_DFIUPD1_VAL = 0x00690093; // dfi_t_ctrlupd_interval_min_x1024=0x69, dfi_t_ctrlupd_interval_max_x1024=0x93
  
  // DFI Update Register2
  DDRC_DFIUPD2_VAL = 0x80000000; // same as reset value...
  
  // DFI Miscellaneous Control Register (1)
  DDRC_DFIMISC_VAL = 0x00000000; // dis_dyn_adr_tri=0, dfi_init_complete_en=0
  
  // DM/DBI Control Register
  DDRC_DBICTL_VAL = 0x00000000; // dm_en=0
  
  //  DFI PHY Master
  DDRC_DFIPHYMSTR_VAL = 0x00000000; // dfi_phymstr_en=0

  #ifdef __GENERIC_ADDRESS_MAPPING__

  ddrc_config_address_mapping(DDR_COLUMN_NUM, DDR_ROW_NUM, DDR_BANK_NUM);

  #else
  
  ddrc_config_address_mapping_old();

#endif	// __NEW_ADDRESSING__

  // ODT Configuration Register
  DDRC_ODTCFG_VAL = 0x0f080d24; // wr_odt_hold=0xf, wr_odt_delay=0x08, rd_odt_hold=0xd, rd_odt_delay=0x09
  
  // ODT/Rank Map Register
  DDRC_ODTMAP_VAL = 0x00000000; // disable ODT for all ranks
  
  
  // Scheduler Control Register
  // INMZ: //  DDRC_SCHED_VAL = 0x434f0c03; // rdwr_idle_gap=0x43, go2critical_hysteresis=0x4f, lpr_num_entries=0x0c, pageclose=0, prefer_write=1, force_low_pri_n=1
  //...........................// Note: prefer_write, spec: "If set then the bank selector prefers writes over reads. FOR DEBUG ONLY"!
  
  DDRC_SCHED_VAL = ((0x00<<24) | (0x00<<16) | (0x1e<<8) | (0x0<<2) | (0x0<<1) | (0x0<<0));
  
  // Scheduler Control Register1
  //INMZ: // DDRC_SCHED1_VAL = 0x0000000b; // pageclose_timer=0x0b
  DDRC_SCHED1_VAL = 0x0000000f;

  // High Priority Read CAM Register1
  //INMZ: // DDRC_PERFHPR1_VAL = 0x6700e574; // hpr_xact_run_length=0x67, hpr_max_starve=0xe547
  
  // Low Priority Read CAM Register1
  //INMZ: // DDRC_PERFLPR1_VAL = 0x690027a6; // lpr_xact_run_length=0x69, lpr_max_starve=0x27a6
  
  // Write CAM Register1
  //INMZ: // DDRC_PERFWR1_VAL = 0x9b0051fb; // w_xact_run_length=0x9b, w_max_starve=0x51fb
    
  DDRC_PERFHPR1_HPR_MAX_STARVE_W(0xdb44);
  DDRC_PERFHPR1_HPR_XACT_RUN_LENGTH_W(0xf8);
  DDRC_PERFLPR1_LPR_MAX_STARVE_W(0x2bab);
  DDRC_PERFLPR1_LPR_XACT_RUN_LENGTH_W(0x95);
  DDRC_PERFWR1_W_MAX_STARVE_W(0xc31f);
  DDRC_PERFWR1_W_XACT_RUN_LENGTH_W(0x72);

  // PCFGR_n Registers for 14 ports

   DDRC_MP_PCFGR_0_RD_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGR_0_RD_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGR_0_RD_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGR_0_RD_PORT_PAGEMATCH_EN_W(0x1);

   DDRC_MP_PCFGW_0_WR_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGW_0_WR_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGW_0_WR_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGW_0_WR_PORT_PAGEMATCH_EN_W(0x1);
  //-------------------------------------------
   DDRC_MP_PCFGR_1_RD_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGR_1_RD_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGR_1_RD_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGR_1_RD_PORT_PAGEMATCH_EN_W(0x1);

   DDRC_MP_PCFGW_1_WR_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGW_1_WR_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGW_1_WR_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGW_1_WR_PORT_PAGEMATCH_EN_W(0x1);
  //-------------------------------------------
   DDRC_MP_PCFGR_2_RD_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGR_2_RD_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGR_2_RD_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGR_2_RD_PORT_PAGEMATCH_EN_W(0x1);

   DDRC_MP_PCFGW_2_WR_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGW_2_WR_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGW_2_WR_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGW_2_WR_PORT_PAGEMATCH_EN_W(0x1);
  //-------------------------------------------
   DDRC_MP_PCFGR_3_RD_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGR_3_RD_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGR_3_RD_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGR_3_RD_PORT_PAGEMATCH_EN_W(0x1);

   DDRC_MP_PCFGW_3_WR_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGW_3_WR_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGW_3_WR_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGW_3_WR_PORT_PAGEMATCH_EN_W(0x1);
  //-------------------------------------------
   DDRC_MP_PCFGR_4_RD_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGR_4_RD_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGR_4_RD_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGR_4_RD_PORT_PAGEMATCH_EN_W(0x1);

   DDRC_MP_PCFGW_4_WR_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGW_4_WR_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGW_4_WR_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGW_4_WR_PORT_PAGEMATCH_EN_W(0x1);
  //-------------------------------------------
   DDRC_MP_PCFGR_5_RD_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGR_5_RD_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGR_5_RD_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGR_5_RD_PORT_PAGEMATCH_EN_W(0x1);

   DDRC_MP_PCFGW_5_WR_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGW_5_WR_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGW_5_WR_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGW_5_WR_PORT_PAGEMATCH_EN_W(0x1);
  //-------------------------------------------
   DDRC_MP_PCFGR_6_RD_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGR_6_RD_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGR_6_RD_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGR_6_RD_PORT_PAGEMATCH_EN_W(0x1);

   DDRC_MP_PCFGW_6_WR_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGW_6_WR_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGW_6_WR_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGW_6_WR_PORT_PAGEMATCH_EN_W(0x1);
  //-------------------------------------------
   DDRC_MP_PCFGR_7_RD_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGR_7_RD_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGR_7_RD_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGR_7_RD_PORT_PAGEMATCH_EN_W(0x1);

   DDRC_MP_PCFGW_7_WR_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGW_7_WR_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGW_7_WR_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGW_7_WR_PORT_PAGEMATCH_EN_W(0x1);
  //-------------------------------------------
   DDRC_MP_PCFGR_8_RD_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGR_8_RD_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGR_8_RD_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGR_8_RD_PORT_PAGEMATCH_EN_W(0x1);

   DDRC_MP_PCFGW_8_WR_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGW_8_WR_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGW_8_WR_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGW_8_WR_PORT_PAGEMATCH_EN_W(0x1);
  //-------------------------------------------
   DDRC_MP_PCFGR_9_RD_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGR_9_RD_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGR_9_RD_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGR_9_RD_PORT_PAGEMATCH_EN_W(0x1);

   DDRC_MP_PCFGW_9_WR_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGW_9_WR_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGW_9_WR_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGW_9_WR_PORT_PAGEMATCH_EN_W(0x1);
  //-------------------------------------------
   DDRC_MP_PCFGR_10_RD_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGR_10_RD_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGR_10_RD_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGR_10_RD_PORT_PAGEMATCH_EN_W(0x1);

   DDRC_MP_PCFGW_10_WR_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGW_10_WR_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGW_10_WR_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGW_10_WR_PORT_PAGEMATCH_EN_W(0x1);
  //-------------------------------------------
   DDRC_MP_PCFGR_11_RD_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGR_11_RD_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGR_11_RD_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGR_11_RD_PORT_PAGEMATCH_EN_W(0x1);

   DDRC_MP_PCFGW_12_WR_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGW_12_WR_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGW_12_WR_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGW_12_WR_PORT_PAGEMATCH_EN_W(0x1);
  //-------------------------------------------
   DDRC_MP_PCFGR_13_RD_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGR_13_RD_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGR_13_RD_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGR_13_RD_PORT_PAGEMATCH_EN_W(0x1);

   DDRC_MP_PCFGW_13_WR_PORT_PRIORITY_W(0x40);
   DDRC_MP_PCFGW_13_WR_PORT_AGING_EN_W(0x1);
   DDRC_MP_PCFGW_13_WR_PORT_URGENT_EN_W(0x0);
   DDRC_MP_PCFGW_13_WR_PORT_PAGEMATCH_EN_W(0x1);
  //-------------------------------------------

  // Debug Registers0
  DDRC_DBG0_VAL = 0x00000000; // dis_wc=1 (disable write combine FOR DEBUG ONLY)
  
  // Debug Registers1 (2)
  DDRC_DBG1_VAL = 0x00000000; // clear bit "dis_dq".
  
  // Command Debug Register
  DDRC_DBGCMD_VAL = 0x00000000; // same as reset value...
  
  
  // Software Register Programming Control Enable (1)
  // sw_done: "Program register to 0 to enable quasi-dynamic programming.
  //           Set back register to 1 once programming is done".
  DDRC_SWCTL_VAL = 0x00000001; // same as reset value...
  
  
  // AXI Poison Configuration Register. Common for all AXI ports
  DDRC_POISONCFG_VAL = 0x00100010; // rd_poison_intr_en=1, rd_poison_slverr_en=0, wr_poison_intr_clr=0, wr_poison_intr_en=1, wr_poison_slverr_en=0
  
  // -- Inomize line 328 --
  
  // Inomize code: run here NEW task set_registers_for_3200_Mbps()
  set_registers_for_xxxx_Mbps(freq);
  
  
  // -- Inomize line 340 --
  
  //-------------------------------------------
  // "Multi Port" configuration registers
  
  // Port Common Configuration Register
  DDRC_MP_PCCFG_VAL = 0x00000001; // bl_exp_mode=0, pagematch_limit=0, go2critical_en=1
  
  // Port "n" Control Register
  DDRC_MP_PCTRL_0_VAL  = 0x00000001; // port_en=1
  DDRC_MP_PCTRL_1_VAL  = 0x00000001; // port_en=1
  DDRC_MP_PCTRL_2_VAL  = 0x00000001; // port_en=1
  DDRC_MP_PCTRL_3_VAL  = 0x00000001; // port_en=1
  DDRC_MP_PCTRL_4_VAL  = 0x00000001; // port_en=1
  DDRC_MP_PCTRL_5_VAL  = 0x00000001; // port_en=1
  DDRC_MP_PCTRL_6_VAL  = 0x00000001; // port_en=1
  DDRC_MP_PCTRL_7_VAL  = 0x00000001; // port_en=1
  DDRC_MP_PCTRL_8_VAL  = 0x00000001; // port_en=1
  DDRC_MP_PCTRL_9_VAL  = 0x00000001; // port_en=1
  DDRC_MP_PCTRL_10_VAL = 0x00000001; // port_en=1
  DDRC_MP_PCTRL_11_VAL = 0x00000001; // port_en=1
  DDRC_MP_PCTRL_12_VAL = 0x00000001; // port_en=1
  DDRC_MP_PCTRL_13_VAL = 0x00000001; // port_en=1
  
  //-------------------------------------------
  // Low Power Control Register (3)
  data_val = DDRC_PWRCTL_VAL; // read
  if (data_val != 0x000001a2) { // 0d418
    // lpddr4_sr_allowed=1
    // dis_cam_drain_selfref=1
    // stay_in_selfref=0
    // selfref_sw=1
    // mpsm_en=0
    // en_dfi_dram_clk_disable=0
    // deeppowerdown_en=0
    // powerdown_en=1
    // selfref_en=0
    GME_SPARE_0_SPARE_BITS_W(0xa02abad3); // write
    GME_SPARE_0_SPARE_BITS_W(data_val); // write value to observe
//  	debug_printf("ddrm_umctl_init_seq: DDRC_PWRCTL_VAL = %x\n", data_val);
  }
  // Low Power Control Register (4)
  DDRC_PWRCTL_VAL = 0x000001a2; // 0d418
  
  
  //-------------------------------------------
  // Step 3 - De-assert reset signal core_ddrc_rstn
  
  // Inomize original code:
  //x //env.ddrm_reset_seq.drive_ddrm_reset(1); <==? DDR-Core reset
  //@(posedge ddrm_sigs.ddr_pll_lock);      <==? we already established PLL lock
  //env.ddrm_reset_seq.drive_axi_reset(1);  <==? DDR-AXI reset
  
  // In DDRM integration, DDR-Core reset is the same as DDR-AXI reset
  // Register: ddrm_control ("DDR Macro control register")
  GME_DDRM_CONTROL_HOLD_DDRC_RESET_W(0); // Field: hold_ddrc_reset ("release reset when this bit is set to 0")
  
  
  //-------------------------------------------
  // Step 4
  
  // -- Inomize line 474 --
  
  // Refresh Control Register3 (2)
  DDRC_RFSHCTL3_VAL = 0x00000001;
  
  // Low Power Control Register (5)
  DDRC_PWRCTL_POWERDOWN_EN_W(0x0); // powerdown_en=0
  DDRC_PWRCTL_SELFREF_EN_W(0x0); // selfref_en=0
  
  
  
  //-------------------------------------------
  // Steps 5 - 7
  
  // ...step5
  // Software Register Programming Control Enable (2)
  // sw_done: "Program register to 0 to enable quasi-dynamic programming.
  //           Set back register to 1 once programming is done".
  DDRC_SWCTL_VAL = 0x00000000; //
  
  // ...step6
  // DFI Miscellaneous Control Register (2)
  DDRC_DFIMISC_VAL = 0x00000000; // dis_dyn_adr_tri=0, dfi_init_complete_en=0
  
  
  // ...step7
  // Software Register Programming Control Enable (3)
  // sw_done: "Program register to 0 to enable quasi-dynamic programming.
  //           Set back register to 1 once programming is done".
  DDRC_SWCTL_VAL = 0x00000001; //
  
  // spec: "Require polling SWSTAT.sw_done_ack after setting SWCTL.sw_done to 1"
#ifdef __NEW_WHILE_LOOP__
	DDRM_UMCTL_WHILE_LOOP_LIMITED((DDRC_SWSTAT_SW_DONE_ACK_R == 0), DDRM_MAX_ITERATIONS);
#else
  data_val = DDRC_SWSTAT_SW_DONE_ACK_R ; // Inomize new code
  while (data_val == 0) {
    //x data_val = DDRC_SWSTAT_VAL;
    delay_cnt=0; while (delay_cnt<2) delay_cnt++; // polling delay
    data_val = DDRC_SWSTAT_SW_DONE_ACK_R ; // Inomize new code
  }
#endif  
  GME_SPARE_0_SPARE_BITS_W(0xa02a0004); // write
  
  
  // -- Imonize line 487
  
  //-------------------------------------------
  // Steps 8 - 13
  // "Start PHY initialization and training by accessing relevant PUB registers"

  res = ddrm_phy_init_seq(freq);
  if(res == 1)
  {
  		return 1;
  }
  
  
  //-------------------------------------------
  // Step 14 - Poll the PUB register MASTER.CalBusy=0
  
  // Inomize line 488
  
  // Register: CalBusy = Impedance Calibration Busy Status
  // Field: Read 1 if the calibrator is actively calibrating.
  //        Any changes to calibrator-related CSRs may only be made
  //        if the calibrator is disabled (via CSR CalRun) and this CSR reads 0.
#ifdef __NEW_WHILE_LOOP__
	  DDRM_UMCTL_WHILE_LOOP_LIMITED((DDRP_MASTER_CALBUSY_VAL != 0), DDRM_MAX_ITERATIONS);
#else
  data_val = DDRP_MASTER_CALBUSY_VAL; // env.ddrp_regmodel.ddr_phy_regs.DWC_DDRPHYA_MASTER0.CalBusy.read(.status(status), .value(cal_busy));
  while (data_val != 0) {
    delay_cnt=0; while (delay_cnt<2) delay_cnt++; // polling delay
    data_val = DDRP_MASTER_CALBUSY_VAL; // env.ddrp_regmodel.ddr_phy_regs.DWC_DDRPHYA_MASTER0.CalBusy.read(.status(status), .value(cal_busy));
  }
#endif
  GME_SPARE_0_SPARE_BITS_W(0xa02a0005); // write
  
  // Note:
  // This step explicitly requires polling
  // however, Inomizes code did only a single read without checking the result...
  
  
  //-------------------------------------------
  // Steps 15-24:
  
  // Step15: "Set SWCTL.sw_done to 0"
  // Software Register Programming Control Enable (4)
  // sw_done: "Program register to 0 to enable quasi-dynamic programming.
  //           Set back register to 1 once programming is done".
  DDRC_SWCTL_SW_DONE_W(0x0); //
  
  // Step16: "Set DFIMISC.dfi_init_start to 1"
  // Register = DFI Miscellaneous Control Register
  // Field = dfi_init_start (R/W), When asserted it triggers the PHY init start request.
  DDRC_DFIMISC_DFI_INIT_START_W(0x1); 
  
  // Step17: Set SWCTL.sw_done to 1
  // Software Register Programming Control Enable (5)
  // sw_done: "Program register to 0 to enable quasi-dynamic programming.
  //           Set back register to 1 once programming is done".
  DDRC_SWCTL_SW_DONE_W(0x1); // 
  
  
  // spec: "Require polling SWSTAT.sw_done_ack after setting SWCTL.sw_done to 1"
#ifdef __NEW_WHILE_LOOP__
		DDRM_UMCTL_WHILE_LOOP_LIMITED((DDRC_SWSTAT_SW_DONE_ACK_R == 0), DDRM_MAX_ITERATIONS);
#else
  data_val = DDRC_SWSTAT_SW_DONE_ACK_R; //
  while (data_val == 0) {
    delay_cnt=0; while (delay_cnt<2) delay_cnt++; // polling delay
    data_val = DDRC_SWSTAT_SW_DONE_ACK_R; //
  }
#endif
  GME_SPARE_0_SPARE_BITS_W(0xa02a0006); // write
  
  // Step18: "Poll DFISTAT.dfi_init_complete=1"
#ifdef __NEW_WHILE_LOOP__
		  DDRM_UMCTL_WHILE_LOOP_LIMITED((DDRC_DFISTAT_DFI_INIT_COMPLETE_R == 0), DDRM_MAX_ITERATIONS);
#else
  data_val = DDRC_DFISTAT_DFI_INIT_COMPLETE_R; // DFI Miscellaneous Control Register
  while (data_val == 0) {
    delay_cnt=0; while (delay_cnt<2) delay_cnt++; // polling delay
    data_val = DDRC_DFISTAT_DFI_INIT_COMPLETE_R; // DFI Miscellaneous Control Register
  }
#endif  
  GME_SPARE_0_SPARE_BITS_W(0xa02a0007); // write
  
  
  // Step19: "Set SWCTL.sw_done to 0"
  // Software Register Programming Control Enable (6)
  // sw_done: "Program register to 0 to enable quasi-dynamic programming.
  //           Set back register to 1 once programming is done".
  DDRC_SWCTL_SW_DONE_W(0x0); // 
  
  // Step20: "Set DFIMISC.dfi_init_start to 0"
  // Register = DFI Miscellaneous Control Register
  // Field = dfi_init_start (R/W), When asserted it triggers the PHY init start request.
  DDRC_DFIMISC_DFI_INIT_START_W(0x0); 
  // NOTE: INOMIZE CODE WRITES HERE "1" AGAIN. THIS SEEMS TO BE A MISTAKE!
  
  
  // Step21: "Set DFIMISC.dfi_init_complete_en to 1"
  // Register = DFI Miscellaneous Control Register
  // Field = dfi_init_complete_en, When asserted the dfi_init_complete signal can be used to trigger SDRAM initialisation.
  DDRC_DFIMISC_DFI_INIT_COMPLETE_EN_W(0x1); 
  
  // Step22: "Set PWRCTL.selfref_sw to 0"
  // Low Power Control Register(6)
  // Field = selfref_sw, A value of 1 to this register causes system to move to Self Refresh state immediately,
  //         as long as it is not in INIT or DPD/MPSM operating_mode.
  //         This is referred to as Software Entry/Exit to Self Refresh.
  //         1 - Software Entry to Self Refresh
  //         0 - Software Exit from Self Refresh
  DDRC_PWRCTL_SELFREF_SW_W(0x0); 
  
  
  // Step23: "Set SWCTL.sw_done to 1"
  // Software Register Programming Control Enable (7)
  // sw_done: "Program register to 0 to enable quasi-dynamic programming.
  //           Set back register to 1 once programming is done".
  DDRC_SWCTL_SW_DONE_W(0x1); // 
  
  // spec: "Require polling SWSTAT.sw_done_ack after setting SWCTL.sw_done to 1"
#ifdef __NEW_WHILE_LOOP__
			DDRM_UMCTL_WHILE_LOOP_LIMITED((DDRC_SWSTAT_SW_DONE_ACK_R == 0), DDRM_MAX_ITERATIONS);
#else
  data_val = DDRC_SWSTAT_SW_DONE_ACK_R;
  while (data_val == 0) {
    delay_cnt=0; while (delay_cnt<2) delay_cnt++; // polling delay
    data_val = DDRC_SWSTAT_SW_DONE_ACK_R;
  }
#endif
  GME_SPARE_0_SPARE_BITS_W(0xa02a0008); // write
  
  
  // Step24: "Wait for DWC_ddr_umctl2 to move to normal operating mode by monitoring STAT.operating_mode signal"
  // Register = Operating Mode Status Register
  // Field: for LPDDR4 the operating modes are:
  //        00 - Init
  //        01 - Normal
  //        10 - Power-down
  //        11 - Self refresh
#ifdef __NEW_WHILE_LOOP__
			  DDRM_UMCTL_WHILE_LOOP_LIMITED((DDRC_STAT_OPERATING_MODE_R != 1), DDRM_MAX_ITERATIONS);
#else
  data_val = DDRC_STAT_OPERATING_MODE_R;
  while (data_val != 1) {
    delay_cnt=0; while (delay_cnt<4) delay_cnt++; // polling delay
    data_val = DDRC_STAT_OPERATING_MODE_R;
  }
#endif
  DDRC_RFSHCTL3_VAL = 0x00000000;
  GME_SPARE_0_SPARE_BITS_W(0xa02a0009); // write
  
  return 0;
}


// ==========================================================================================
// The following code is a task in Inomize's SV code
// and they run it twice (why?)
// so we make it a function
static void set_registers_for_xxxx_Mbps (unsigned int freq) {
  
  GME_SPARE_0_SPARE_BITS_W(0xa12a0001); // write
  switch (freq) {

  case 334:
      
    // ***************************
    // DDRCLK=334MHz(668mb/s)
    // ***************************
    // SDRAM Timing Registers
    //                  wr2pre[30:24] | t_faw[21:16] | t_ras_max[14:8] | t_ras_min[5:0]
    DDRC_DRAMTMG0_VAL = ((0x0b << 24) | (0x07 << 16) | (0x05 << 8)     | (0x08 << 0));
#ifdef __DDR_RFSH_ALL_BANKS
    DDRC_DRAMTMG1_VAL = 0x0002040b;
    DDRC_DRAMTMG4_VAL = 0x04040204;
#else  
    DDRC_DRAMTMG1_VAL = 0x0002040b;
    DDRC_DRAMTMG4_VAL = 0x04040204;
#endif
    DDRC_DRAMTMG2_VAL = 0x0305090c;
    DDRC_DRAMTMG3_VAL = 0x01e05000;
    DDRC_DRAMTMG5_VAL = 0x01050808;
    DDRC_DRAMTMG6_VAL = 0x00000002;
    DDRC_DRAMTMG7_VAL = 0x00000501;
    DDRC_DRAMTMG12_VAL = 0x00010500;
    DDRC_DRAMTMG13_VAL = 0x0a100002;
    DDRC_DRAMTMG14_VAL = 0x00000020;
    
    // --------------------------------------------
    // ZQ Control Registers
    DDRC_ZQCTL0_VAL = 0x20a80006;
    DDRC_ZQCTL1_VAL = 0x00903fca;
    
    // --------------------------------------------
    // DFI Timing Registers
    DDRC_DFITMG0_DFI_T_CTRL_DELAY_W(0x2);
    DDRC_DFITMG0_DFI_RDDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
    DDRC_DFITMG0_DFI_T_RDDATA_EN_W(0x5);             // Convert MR2[2:0] according to table, Should be RL - 5. RL = 10
    DDRC_DFITMG0_DFI_WRDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
    DDRC_DFITMG0_DFI_TPHY_WRDATA_W(0x2);             // Set at 2
    DDRC_DFITMG0_DFI_TPHY_WRLAT_W(0x2);              // Convert MR2[5:3] according to table, Should be WL - 4. WL = 6
    DDRC_DFITMG1_VAL = 0x00020303;
    DDRC_DFITMG2_DFI_TPHY_WRCSLAT_W(0x2);            // Equal to dfi_tphy_wrlat
    DDRC_DFITMG2_DFI_TPHY_RDCSLAT_W(0x5);            // Equal to dfi_t_rddata_en
    break;

  case 350:
      
    // ***************************
    // DDRCLK=350MHz(700mb/s)
    // ***************************
    // SDRAM Timing Registers
    //                  wr2pre[30:24] | t_faw[21:16] | t_ras_max[14:8] | t_ras_min[5:0]
    DDRC_DRAMTMG0_VAL = ((0x0b << 24) | (0x08 << 16) | (0x06 << 8)     | (0x08 << 0));
#ifdef __DDR_RFSH_ALL_BANKS
    DDRC_DRAMTMG1_VAL = 0x0002040c;
    DDRC_DRAMTMG4_VAL = 0x04040204;
#else  
    DDRC_DRAMTMG1_VAL = 0x0002040b;
    DDRC_DRAMTMG4_VAL = 0x04040204;
#endif
    DDRC_DRAMTMG2_VAL = 0x0305090c;
    DDRC_DRAMTMG3_VAL = 0x01d05000;
    DDRC_DRAMTMG5_VAL = 0x01050808;
    DDRC_DRAMTMG6_VAL = 0x00000002;
    DDRC_DRAMTMG7_VAL = 0x00000501;
    DDRC_DRAMTMG12_VAL = 0x00010500;
    DDRC_DRAMTMG13_VAL = 0x0a100002;
    DDRC_DRAMTMG14_VAL = 0x00000021;
    
    // --------------------------------------------
    // ZQ Control Registers
    DDRC_ZQCTL0_VAL = 0x20b00006;
    DDRC_ZQCTL1_VAL = 0x009042ec;
    
    // --------------------------------------------
    // DFI Timing Registers
    DDRC_DFITMG0_DFI_T_CTRL_DELAY_W(0x2);
    DDRC_DFITMG0_DFI_RDDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
    DDRC_DFITMG0_DFI_T_RDDATA_EN_W(0x5);             // Convert MR2[2:0] according to table, Should be RL - 5. RL = 10
    DDRC_DFITMG0_DFI_WRDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
    DDRC_DFITMG0_DFI_TPHY_WRDATA_W(0x2);             // Set at 2
    DDRC_DFITMG0_DFI_TPHY_WRLAT_W(0x2);              // Convert MR2[5:3] according to table, Should be WL - 4. WL = 6
    DDRC_DFITMG1_VAL = 0x00020303;
    DDRC_DFITMG2_DFI_TPHY_WRCSLAT_W(0x2);            // Equal to dfi_tphy_wrlat
    DDRC_DFITMG2_DFI_TPHY_RDCSLAT_W(0x5);            // Equal to dfi_t_rddata_en
    break;
  case 1000:
	// Refresh Timing Registers
  //                  wr2pre[30:24] | t_faw[21:16] | t_ras_max[14:8] | t_ras_min[5:0]
  DDRC_DRAMTMG0_VAL = ((0x13 << 24) | (0x14 << 16) | (0x11 << 8)     | (0x15 << 0));
#ifdef __DDR_RFSH_ALL_BANKS
  DDRC_DRAMTMG1_VAL = 0x00040420;
  DDRC_DRAMTMG4_VAL = 0x0904050B;
#else  
  DDRC_DRAMTMG1_VAL = 0x0004041E;
  DDRC_DRAMTMG4_VAL = 0x09040509;
#endif
  DDRC_DRAMTMG2_VAL = 0x050A0E0F;
  DDRC_DRAMTMG3_VAL = 0x00E07000;
  DDRC_DRAMTMG5_VAL = 0x01040808;
  DDRC_DRAMTMG6_VAL = 0x00000004;
  DDRC_DRAMTMG7_VAL = 0x00000401;
  DDRC_DRAMTMG12_VAL = 0x00010400;
  DDRC_DRAMTMG13_VAL = 0x0a100002;
  DDRC_DRAMTMG14_VAL = 0x0000005E;

  // --------------------------------------------
  // ZQ Control Registers
  DDRC_ZQCTL0_VAL = 0x21F4000F;
  DDRC_ZQCTL1_VAL = 0x0190BEBC;

	// --------------------------------------------
	// DFI Timing Registers
	DDRC_DFITMG0_DFI_T_CTRL_DELAY_W(0x2);
	DDRC_DFITMG0_DFI_RDDATA_USE_DFI_PHY_CLK_W(0x1); // Set to 1
	DDRC_DFITMG0_DFI_T_RDDATA_EN_W(0xF); // Convert MR2[2:0] according to table, Should be RL - 5. RL = 10
	DDRC_DFITMG0_DFI_WRDATA_USE_DFI_PHY_CLK_W(0x1); // Set to 1
	DDRC_DFITMG0_DFI_TPHY_WRDATA_W(0x2); // Set at 2
	DDRC_DFITMG0_DFI_TPHY_WRLAT_W(0x6); // Convert MR2[5:3] according to table, Should be WL - 5. WL = 6
	DDRC_DFITMG1_VAL = 0x00020303;
	DDRC_DFITMG2_DFI_TPHY_WRCSLAT_W(0x6); // Equal to dfi_tphy_wrlat
	DDRC_DFITMG2_DFI_TPHY_RDCSLAT_W(0xF); // Equal to dfi_t_rddata_en
    break;
  case 1400:
  // ***************************
  // DDRCLK=1400MHz(2800mb/s)
  // ***************************
  // SDRAM Timing Registers
  //                  wr2pre[30:24] | t_faw[21:16] | t_ras_max[14:8] | t_ras_min[5:0]
  DDRC_DRAMTMG0_VAL = ((0x19 << 24) | (0x1d << 16) | (0x18 << 8)     | (0x1e << 0));
#ifdef __DDR_RFSH_ALL_BANKS
  DDRC_DRAMTMG1_VAL = 0x0005052d;
  DDRC_DRAMTMG4_VAL = 0x0d04080f;
#else  
  DDRC_DRAMTMG1_VAL = 0x0005052b;
  DDRC_DRAMTMG4_VAL = 0x0d04080d;
#endif
  DDRC_DRAMTMG2_VAL = 0x070e1213;
  DDRC_DRAMTMG3_VAL = 0x00e0a000;
  DDRC_DRAMTMG5_VAL = 0x01040808;
  DDRC_DRAMTMG6_VAL = 0x00000005;
  DDRC_DRAMTMG7_VAL = 0x00000401;
  DDRC_DRAMTMG12_VAL = 0x00010400;
  DDRC_DRAMTMG13_VAL = 0x0c100002;
  DDRC_DRAMTMG14_VAL = 0x00000084;
  
  // --------------------------------------------
  // ZQ Control Registers
  DDRC_ZQCTL0_VAL = 0x22bd0016;
  DDRC_ZQCTL1_VAL = 0x02410B22;
  
  // --------------------------------------------
  // DFI Timing Registers
  DDRC_DFITMG0_DFI_T_CTRL_DELAY_W(0x2);
  DDRC_DFITMG0_DFI_RDDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
  DDRC_DFITMG0_DFI_T_RDDATA_EN_W(0x17);            // Convert MR2[2:0] according to table, Should be RL - 5. RL = 10
  DDRC_DFITMG0_DFI_WRDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
  DDRC_DFITMG0_DFI_TPHY_WRDATA_W(0x2);             // Set at 2
  DDRC_DFITMG0_DFI_TPHY_WRLAT_W(0xa);              // Convert MR2[5:3] according to table, Should be WL - 5. WL = 6
  DDRC_DFITMG1_VAL = 0x00020303;
  DDRC_DFITMG2_DFI_TPHY_WRCSLAT_W(0xa);            // Equal to dfi_tphy_wrlat
  DDRC_DFITMG2_DFI_TPHY_RDCSLAT_W(0x17);           // Equal to dfi_t_rddata_en
    break;

  case 1334:
    // ***************************
    // DDRCLK=1334MHz(2667mb/s)
  // ***************************
  // SDRAM Timing Registers
  //                  wr2pre[30:24] | t_faw[21:16] | t_ras_max[14:8] | t_ras_min[5:0]
  DDRC_DRAMTMG0_VAL = ((0x18 << 24) | (0x1b << 16) | (0x16 << 8)     | (0x1c << 0));
#ifdef __DDR_RFSH_ALL_BANKS
  DDRC_DRAMTMG1_VAL = 0x0005052a;
  DDRC_DRAMTMG4_VAL = 0x0c04070e;
#else  
  DDRC_DRAMTMG1_VAL = 0x00050528;
  DDRC_DRAMTMG4_VAL = 0x0c04070c;
#endif
  DDRC_DRAMTMG2_VAL = 0x070e1312;
  DDRC_DRAMTMG3_VAL = 0x00e0a000;
  DDRC_DRAMTMG5_VAL = 0x01040808;
  DDRC_DRAMTMG6_VAL = 0x00000005;
  DDRC_DRAMTMG7_VAL = 0x00000401;
  DDRC_DRAMTMG12_VAL = 0x00010400;
  DDRC_DRAMTMG13_VAL = 0x0c100002;
  DDRC_DRAMTMG14_VAL = 0x0000007d;
  
  // --------------------------------------------
  // ZQ Control Registers
  DDRC_ZQCTL0_VAL = 0x229b0014;
  DDRC_ZQCTL1_VAL = 0x0220FE50;
  
  // --------------------------------------------
  // DFI Timing Registers
  DDRC_DFITMG0_DFI_T_CTRL_DELAY_W(0x2);
  DDRC_DFITMG0_DFI_RDDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
  DDRC_DFITMG0_DFI_T_RDDATA_EN_W(0x17);            // Convert MR2[2:0] according to table, Should be RL - 5. RL = 10
  DDRC_DFITMG0_DFI_WRDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
  DDRC_DFITMG0_DFI_TPHY_WRDATA_W(0x2);             // Set at 2
  DDRC_DFITMG0_DFI_TPHY_WRLAT_W(0xa);              // Convert MR2[5:3] according to table, Should be WL - 5. WL = 6
  DDRC_DFITMG1_VAL = 0x00020303;
  DDRC_DFITMG2_DFI_TPHY_WRCSLAT_W(0xa);            // Equal to dfi_tphy_wrlat
  DDRC_DFITMG2_DFI_TPHY_RDCSLAT_W(0x17);           // Equal to dfi_t_rddata_en
    break;

  case 1200:
    // ***************************
  // DDRCLK=1200MHz(2400mb/s)
  // ***************************
  // SDRAM Timing Registers
  //                  wr2pre[30:24] | t_faw[21:16] | t_ras_max[14:8] | t_ras_min[5:0]
  DDRC_DRAMTMG0_VAL = ((0x16 << 24) | (0x19 << 16) | (0x14 << 8)     | (0x1a << 0));
#ifdef __DDR_RFSH_ALL_BANKS
  DDRC_DRAMTMG1_VAL = 0x00050526;
  DDRC_DRAMTMG4_VAL = 0x0b04070d;
#else  
  DDRC_DRAMTMG1_VAL = 0x00050525;
  DDRC_DRAMTMG4_VAL = 0x0b04070b;
#endif
  DDRC_DRAMTMG2_VAL = 0x060c1111;
  DDRC_DRAMTMG3_VAL = 0x00e09000;
  DDRC_DRAMTMG5_VAL = 0x01040808;
  DDRC_DRAMTMG6_VAL = 0x00000005;
  DDRC_DRAMTMG7_VAL = 0x00000401;
  DDRC_DRAMTMG12_VAL = 0x00010400;
  DDRC_DRAMTMG13_VAL = 0x0b100002;
  DDRC_DRAMTMG14_VAL = 0x00000071;
  
  // --------------------------------------------
  // ZQ Control Registers
  DDRC_ZQCTL0_VAL = 0x22590013;
  DDRC_ZQCTL1_VAL = 0x01F0E4F9;
  
  // --------------------------------------------
  // DFI Timing Registers
  DDRC_DFITMG0_DFI_T_CTRL_DELAY_W(0x2);
  DDRC_DFITMG0_DFI_RDDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
  DDRC_DFITMG0_DFI_T_RDDATA_EN_W(0x13);            // Convert MR2[2:0] according to table, Should be RL - 5. RL = 10
  DDRC_DFITMG0_DFI_WRDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
  DDRC_DFITMG0_DFI_TPHY_WRDATA_W(0x2);             // Set at 2
  DDRC_DFITMG0_DFI_TPHY_WRLAT_W(0x8);              // Convert MR2[5:3] according to table, Should be WL - 5. WL = 6
  DDRC_DFITMG1_VAL = 0x00020303;
  DDRC_DFITMG2_DFI_TPHY_WRCSLAT_W(0x8);            // Equal to dfi_tphy_wrlat
  DDRC_DFITMG2_DFI_TPHY_RDCSLAT_W(0x13);           // Equal to dfi_t_rddata_en
    break;
    
  case 1067:
    // ***************************
  // DDRCLK=1067MHz(2133mb/s)
  // ***************************
  // SDRAM Timing Registers
  //                  wr2pre[30:24] | t_faw[21:16] | t_ras_max[14:8] | t_ras_min[5:0]
  DDRC_DRAMTMG0_VAL = ((0x14 << 24) | (0x16 << 16) | (0x12 << 8)     | (0x17 << 0));
#ifdef __DDR_RFSH_ALL_BANKS
  DDRC_DRAMTMG1_VAL = 0x00040422;
  DDRC_DRAMTMG4_VAL = 0x0a04060c;
#else  
  DDRC_DRAMTMG1_VAL = 0x00040420;
  DDRC_DRAMTMG4_VAL = 0x0a04060a;
#endif
  DDRC_DRAMTMG2_VAL = 0x050a0e0f;
  DDRC_DRAMTMG3_VAL = 0x00e08000;
  DDRC_DRAMTMG5_VAL = 0x01040808;
  DDRC_DRAMTMG6_VAL = 0x00000004;
  DDRC_DRAMTMG7_VAL = 0x00000401;
  DDRC_DRAMTMG12_VAL = 0x00010400;
  DDRC_DRAMTMG13_VAL = 0x0b100002;
  DDRC_DRAMTMG14_VAL = 0x00000064;
  
  // --------------------------------------------
  // ZQ Control Registers
  DDRC_ZQCTL0_VAL = 0x22160010;
  DDRC_ZQCTL1_VAL = 0x01B0CB73;
  
  // --------------------------------------------
  // DFI Timing Registers
  DDRC_DFITMG0_DFI_T_CTRL_DELAY_W(0x2);
  DDRC_DFITMG0_DFI_RDDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
  DDRC_DFITMG0_DFI_T_RDDATA_EN_W(0xf);             // Convert MR2[2:0] according to table, Should be RL - 5. RL = 10
  DDRC_DFITMG0_DFI_WRDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
  DDRC_DFITMG0_DFI_TPHY_WRDATA_W(0x2);             // Set at 2
  DDRC_DFITMG0_DFI_TPHY_WRLAT_W(0x6);              // Convert MR2[5:3] according to table, Should be WL - 5. WL = 6
  DDRC_DFITMG1_VAL = 0x00020303;
  DDRC_DFITMG2_DFI_TPHY_WRCSLAT_W(0x6);            // Equal to dfi_tphy_wrlat
    DDRC_DFITMG2_DFI_TPHY_RDCSLAT_W(0xf);            // Equal to dfi_t_rddata_en
    break;
    
  case 533:
    // ***************************
    // DDRCLK=533MHz(1066mb/s)
  // ***************************
  // SDRAM Timing Registers
  //                  wr2pre[30:24] | t_faw[21:16] | t_ras_max[14:8] | t_ras_min[5:0]
  DDRC_DRAMTMG0_VAL = ((0x0d << 24) | (0x0b << 16) | (0x09 << 8)     | (0x0c << 0));
#ifdef __DDR_RFSH_ALL_BANKS
  DDRC_DRAMTMG1_VAL = 0x00020411;
  DDRC_DRAMTMG4_VAL = 0x05040306;
#else  
  DDRC_DRAMTMG1_VAL = 0x00020410;
  DDRC_DRAMTMG4_VAL = 0x05040305;
#endif
  DDRC_DRAMTMG2_VAL = 0x0305090c;
  DDRC_DRAMTMG3_VAL = 0x01305000;
  DDRC_DRAMTMG5_VAL = 0x01040808;
  DDRC_DRAMTMG6_VAL = 0x00000002;
  DDRC_DRAMTMG7_VAL = 0x00000401;
  DDRC_DRAMTMG12_VAL = 0x00010400;
  DDRC_DRAMTMG13_VAL = 0x0a100002;
  DDRC_DRAMTMG14_VAL = 0x00000032;
  
  // --------------------------------------------
  // ZQ Control Registers
  DDRC_ZQCTL0_VAL = 0x210a0008;
  DDRC_ZQCTL1_VAL = 0x00E06566;

	// --------------------------------------------
	// DFI Timing Registers
	DDRC_DFITMG0_DFI_T_CTRL_DELAY_W(0x2);
	DDRC_DFITMG0_DFI_RDDATA_USE_DFI_PHY_CLK_W(0x1); // Set to 1
	DDRC_DFITMG0_DFI_T_RDDATA_EN_W(0x5); // Convert MR2[2:0] according to table, Should be RL - 5. RL = 10
	DDRC_DFITMG0_DFI_WRDATA_USE_DFI_PHY_CLK_W(0x1); // Set to 1
	DDRC_DFITMG0_DFI_TPHY_WRDATA_W(0x2); // Set at 2
	DDRC_DFITMG0_DFI_TPHY_WRLAT_W(0x2); // Convert MR2[5:3] according to table, Should be WL - 5. WL = 6
	DDRC_DFITMG1_VAL = 0x00020303;
	DDRC_DFITMG2_DFI_TPHY_WRCSLAT_W(0x2); // Equal to dfi_tphy_wrlat
    DDRC_DFITMG2_DFI_TPHY_RDCSLAT_W(0x5);            // Equal to dfi_t_rddata_en
    break;

  case 800:
    // ***************************
    // DDRCLK=800MHz(1600mb/s)
    // ***************************
    // SDRAM Timing Registers
    //                  wr2pre[30:24] | t_faw[21:16] | t_ras_max[14:8] | t_ras_min[5:0]
    DDRC_DRAMTMG0_VAL = ((0x10 << 24) | (0x10 << 16) | (0x0d << 8)     | (0x11 << 0));
#ifdef __DDR_RFSH_ALL_BANKS
    DDRC_DRAMTMG1_VAL = 0x0003041A;
    DDRC_DRAMTMG4_VAL = 0x08040409;
#else  
    DDRC_DRAMTMG1_VAL = 0x00030418;
    DDRC_DRAMTMG4_VAL = 0x08040408;
#endif
    DDRC_DRAMTMG2_VAL = 0x04070b0d;
    DDRC_DRAMTMG3_VAL = 0x00e06000;
    DDRC_DRAMTMG5_VAL = 0x01040808;
    DDRC_DRAMTMG6_VAL = 0x00000004;
    DDRC_DRAMTMG7_VAL = 0x00000401;
    DDRC_DRAMTMG12_VAL = 0x00010400;
    DDRC_DRAMTMG13_VAL = 0x0a100002;
    DDRC_DRAMTMG14_VAL = 0x0000004b;
    
    // --------------------------------------------
    // ZQ Control Registers
    DDRC_ZQCTL0_VAL = 0x2190000c;
    DDRC_ZQCTL1_VAL = 0x01409896;
    
    // --------------------------------------------
    // DFI Timing Registers
    DDRC_DFITMG0_DFI_T_CTRL_DELAY_W(0x2);
    DDRC_DFITMG0_DFI_RDDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
    DDRC_DFITMG0_DFI_T_RDDATA_EN_W(0x9);             // Convert MR2[2:0] according to table, Should be RL - 5. RL = 10
    DDRC_DFITMG0_DFI_WRDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
    DDRC_DFITMG0_DFI_TPHY_WRDATA_W(0x2);             // Set at 2
    DDRC_DFITMG0_DFI_TPHY_WRLAT_W(0x4);              // Convert MR2[5:3] according to table, Should be WL - 5. WL = 6
    DDRC_DFITMG1_VAL = 0x00020303;
    DDRC_DFITMG2_DFI_TPHY_WRCSLAT_W(0x4);            // Equal to dfi_tphy_wrlat
    DDRC_DFITMG2_DFI_TPHY_RDCSLAT_W(0x9);            // Equal to dfi_t_rddata_en
    break;
    
  default:
    // ***************************
    // DDRCLK = 1600MHz(3200mb/s)  
    // ***************************
    // SDRAM Timing Registers
    //                  wr2pre[30:24] | t_faw[21:16] | t_ras_max[14:8] | t_ras_min[5:0]
    DDRC_DRAMTMG0_VAL = ((0x1a << 24) | (0x20 << 16) | (0x1b << 8) | (0x22 << 0));
#ifdef __DDR_RFSH_ALL_BANKS
    DDRC_DRAMTMG1_VAL = 0x00060633;
    DDRC_DRAMTMG4_VAL = 0x0f040811;
#else  
    DDRC_DRAMTMG1_VAL = 0x00060630;
    DDRC_DRAMTMG4_VAL = 0x0f04080f;
#endif
    DDRC_DRAMTMG2_VAL = 0x070e1214;
    DDRC_DRAMTMG3_VAL = 0x00e0c000;
    DDRC_DRAMTMG5_VAL = 0x01040808;
    DDRC_DRAMTMG6_VAL = 0x00000006;
    DDRC_DRAMTMG7_VAL = 0x00000401;
    DDRC_DRAMTMG12_VAL = 0x00010400;
    DDRC_DRAMTMG13_VAL = 0x0c100002;
    DDRC_DRAMTMG14_VAL = 0x00000096;
    
    // --------------------------------------------
    // ZQ Control Registers
    DDRC_ZQCTL0_VAL = 0x23200018;
    DDRC_ZQCTL1_VAL = 0x0281312D;
    
    // --------------------------------------------
    // DFI Timing Registers
    DDRC_DFITMG0_DFI_T_CTRL_DELAY_W(0x2);
    DDRC_DFITMG0_DFI_RDDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
    DDRC_DFITMG0_DFI_T_RDDATA_EN_W(0x17);            // Convert MR2[2:0] according to table, Should be RL - 5.
    DDRC_DFITMG0_DFI_WRDATA_USE_DFI_PHY_CLK_W(0x1);  // Set to 1
    DDRC_DFITMG0_DFI_TPHY_WRDATA_W(0x2);             // Set at 2
    DDRC_DFITMG0_DFI_TPHY_WRLAT_W(0xa);              // Convert MR2[5:3] according to table, Should be WL - 5.
    DDRC_DFITMG1_VAL = 0x00020303;
    DDRC_DFITMG2_DFI_TPHY_WRCSLAT_W(0xa);            // Equal to dfi_tphy_wrlat
    DDRC_DFITMG2_DFI_TPHY_RDCSLAT_W(0x17);           // Equal to dfi_t_rddata_en
  }

#ifdef DDR_HALF_BUS_WIDTH_MODE
  DDRC_MSTR_DATA_BUS_WIDTH_W(1);                   // (Half Bus Width Mode)
#endif
  GME_SPARE_0_SPARE_BITS_W(0xa12a0002); // write
}


#define BANK_INTERNAL_BASE	(0x02)
#define COL_INTERNAL_BASE	(0x02)
#define ROW_INTERNAL_BASE	(0x06)
#define COL_UNUSED_LINE		(0x1F)
#define BANK_UNUSED_LINE	(0x3F)
#define ROW_UNUSED_LINE		(0x0F)
#define COL_MAX_LINE		(10)	// from ADDRMAP_COL_B2  to ADDRMAP_COL_B11
#define BANK_MAX_LINE		(3)		// from ADDRMAP_BANK_B0 to ADDRMAP_BANK_B2
#define ROW_MAX_LINE		(10)	// from ADDRMAP_ROW_B0, ADDRMAP_ROW_B1 ADDRMAP_ROW_B2-B10 to ADDRMAP_ROW_B17

static void ddrc_config_address_mapping(unsigned int col, unsigned int row, unsigned int bank)
{	
	//2 Mapping implementation is -> MSB [row] [bank] [col] LSB
	
	
  unsigned int i;
  unsigned int col_offset = 0;								// Column offset will be always 0 in such implementation
  unsigned int bank_offset = col - BANK_INTERNAL_BASE;	
  unsigned int row_offset = col + bank - ROW_INTERNAL_BASE;

  if( (bank_offset > 31) || (col_offset > 7) || (row_offset > 11) )
	{
		debug_printf("Invalid input params: col - %d, bank - %d, row - %d\n", bank_offset, col_offset, row_offset);
          return;
	}
  // Column assignment
  i = 2;
  DDRC_ADDRMAP2_ADDRMAP_COL_B2_W(((i++<col)?(col_offset):(COL_UNUSED_LINE)));
  DDRC_ADDRMAP2_ADDRMAP_COL_B3_W(((i++<col)?(col_offset):(COL_UNUSED_LINE)));
  DDRC_ADDRMAP2_ADDRMAP_COL_B4_W(((i++<col)?(col_offset):(COL_UNUSED_LINE)));
  DDRC_ADDRMAP2_ADDRMAP_COL_B5_W(((i++<col)?(col_offset):(COL_UNUSED_LINE)));
  DDRC_ADDRMAP3_ADDRMAP_COL_B6_W(((i++<col)?(col_offset):(COL_UNUSED_LINE)));
  DDRC_ADDRMAP3_ADDRMAP_COL_B7_W(((i++<col)?(col_offset):(COL_UNUSED_LINE)));
  DDRC_ADDRMAP3_ADDRMAP_COL_B8_W(((i++<col)?(col_offset):(COL_UNUSED_LINE)));
  DDRC_ADDRMAP3_ADDRMAP_COL_B9_W(((i++<col)?(col_offset):(COL_UNUSED_LINE)));
  DDRC_ADDRMAP4_ADDRMAP_COL_B10_W(((i++<col)?(col_offset):(COL_UNUSED_LINE)));
  DDRC_ADDRMAP4_ADDRMAP_COL_B11_W(((i++<col)?(col_offset):(COL_UNUSED_LINE)));
  
  // Bank assignment
  i = 0;
  DDRC_ADDRMAP1_ADDRMAP_BANK_B0_W(((i++<bank)?(bank_offset):(BANK_UNUSED_LINE)));
  DDRC_ADDRMAP1_ADDRMAP_BANK_B1_W(((i++<bank)?(bank_offset):(BANK_UNUSED_LINE)));
  DDRC_ADDRMAP1_ADDRMAP_BANK_B2_W(((i++<bank)?(bank_offset):(BANK_UNUSED_LINE)));
  
  // Row assignment
  i = 0;
  DDRC_ADDRMAP5_ADDRMAP_ROW_B0_W(((i++<row)?(row_offset):(ROW_UNUSED_LINE)));
  DDRC_ADDRMAP5_ADDRMAP_ROW_B1_W(((i++<row)?(row_offset):(ROW_UNUSED_LINE)));
  DDRC_ADDRMAP5_ADDRMAP_ROW_B2_10_W(((i++<row)?(row_offset):(ROW_UNUSED_LINE)));
  i = 11;
  DDRC_ADDRMAP5_ADDRMAP_ROW_B11_W(((i++<row)?(row_offset):(ROW_UNUSED_LINE)));
  DDRC_ADDRMAP6_ADDRMAP_ROW_B12_W(((i++<row)?(row_offset):(ROW_UNUSED_LINE)));
  DDRC_ADDRMAP6_ADDRMAP_ROW_B13_W(((i++<row)?(row_offset):(ROW_UNUSED_LINE)));
  DDRC_ADDRMAP6_ADDRMAP_ROW_B14_W(((i++<row)?(row_offset):(ROW_UNUSED_LINE)));
  DDRC_ADDRMAP6_ADDRMAP_ROW_B15_W(((i++<row)?(row_offset):(ROW_UNUSED_LINE)));
  DDRC_ADDRMAP7_ADDRMAP_ROW_B16_W(((i++<row)?(row_offset):(ROW_UNUSED_LINE)));
  //	DDRC_ADDRMAP7_ADDRMAP_ROW_B17_W(((i++<row)?(row_offset):(ROW_UNUSED_LINE)));

//	debug_printf("Address mapping registers : MAP1 - %x, MAP2 - %x, MAP3 - %x, MAP4 - %x, MAP5 - %x, MAP6 - %x, MAP7 - %x\n",
//				DDRC_ADDRMAP1_VAL,DDRC_ADDRMAP2_VAL, DDRC_ADDRMAP3_VAL, DDRC_ADDRMAP4_VAL, DDRC_ADDRMAP5_VAL, DDRC_ADDRMAP6_VAL, DDRC_ADDRMAP7_VAL);

  DDRC_ADDRMAP6_LPDDR4_6GB_12GB_24GB_W(0x0);
  DDRC_ADDRMAP6_LPDDR3_6GB_12GB_W(0x0);  		
}


static void set_rfshtmg (unsigned int freq) {

#ifdef __DDR_RFSH_ALL_BANKS
  DDRC_RFSHCTL0_VAL = 0x00210070;

  switch (freq) {
  case 334:
    //DDRC_RFSHTMG_VAL = 0x0014001F;
    DDRC_RFSHTMG_VAL = 0x00200030;    // temp
    break;
    // --------------------------------------------
  case 350:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x00150020;
    break;
  case 1000:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x003D005A;
    break;
    // --------------------------------------------
  case 1400:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x0055007F;
    break;
    // --------------------------------------------
  case 1334:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x00510078; 
    break;
    // --------------------------------------------
  case 1200:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x0049006D;
    break;
    // --------------------------------------------
  case 1067:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x00410060;
    break;
    // --------------------------------------------
  case 533:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x00200030;
    break;
    // --------------------------------------------
  case 800:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x00300048; 
    break;
    // --------------------------------------------
  default:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x00610090; 
  }

#else
  DDRC_RFSHCTL0_VAL = 0x00210074;

  switch (freq) {
  case 334:
    // Refresh Timing Registers
    //DDRC_RFSHTMG1_VAL = 0x00100000;
    DDRC_RFSHTMG_VAL = 0x00040018;  // temp 
    DDRC_RFSHTMG1_VAL = 0x00180000; // temp
    break;
  case 350:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x00020010; 
    DDRC_RFSHTMG1_VAL = 0x00100000;
    break;
  case 1000:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x0007002D;
    DDRC_RFSHTMG1_VAL = 0x002D0000;
    break;
  // --------------------------------------------
  case 1400:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x000A0040; 
    DDRC_RFSHTMG1_VAL = 0x00400000;
    break;
    // --------------------------------------------
  case 1334:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x000A003C; 
    DDRC_RFSHTMG1_VAL = 0x003C0000;
    break;
    // --------------------------------------------
  case 1200:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x00090037; 
    DDRC_RFSHTMG1_VAL = 0x00370000;
    break;
    // --------------------------------------------
  case 1067:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x00080030; 
    DDRC_RFSHTMG1_VAL = 0x00300000; 
    break;
  // --------------------------------------------
  case 533:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x00040018; 
    DDRC_RFSHTMG1_VAL = 0x00180000;
    break;
    // --------------------------------------------
  case 800:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x00060024; 
    DDRC_RFSHTMG1_VAL = 0x00240000;
    break;
    // --------------------------------------------
  default:
    // Refresh Timing Registers
    DDRC_RFSHTMG_VAL = 0x000C0048;
    DDRC_RFSHTMG1_VAL = 0x00480000;
  }
#endif
}

#ifndef __GENERIC_ADDRESS_MAPPING__

static void ddrc_config_address_mapping_old(void) {

  // Address Map Register1
  //x DDRC_ADDRMAP1_VAL = 0x001F1F16; // addrmap_bank_b2=0x1f, addrmap_bank_b1=0x1f, addrmap_bank_b0=0x16
  // Inomize new code
  DDRC_ADDRMAP1_ADDRMAP_BANK_B0_W(0x18); //6'd24
  DDRC_ADDRMAP1_ADDRMAP_BANK_B1_W(0x18); //6'd24
  DDRC_ADDRMAP1_ADDRMAP_BANK_B2_W(0x18); //6'd24
  
  // Address Map Register2
  //x DDRC_ADDRMAP2_VAL = 0x00171717; // addrmap_col_b5=0x0, addrmap_col_b4=0x7, addrmap_col_b3=0x17, addrmap_col_b2=0x7
  //x //..............................// @@@@ according to spec bits 20 and 4 are reserved and therefore must be zero! @@@@
  //x //..............................// @@@@ this seems wierd, because according to spec, b2, b4 and b5 are 4 bits each while b3 is 5 bits @@@@
  // Inomize new code
  DDRC_ADDRMAP2_ADDRMAP_COL_B2_W(0x0);
  DDRC_ADDRMAP2_ADDRMAP_COL_B3_W(0x0);
  DDRC_ADDRMAP2_ADDRMAP_COL_B4_W(0x0);
  DDRC_ADDRMAP2_ADDRMAP_COL_B5_W(0x0);  
  
  // Address Map Register3
  DDRC_ADDRMAP3_VAL = 0x00000000; // addrmap_col_b9=0x00, addrmap_col_b8=0x00, addrmap_col_b7=0x00, addrmap_col_b6=0x00
  
  // Address Map Register4
  //x DDRC_ADDRMAP4_VAL = 0x00000000; // col_addr_shift=0, addrmap_col_b11=0x00, addrmap_col_b10=0x00
  // Inomize new code
  DDRC_ADDRMAP4_ADDRMAP_COL_B10_W(0x1f);
  DDRC_ADDRMAP4_ADDRMAP_COL_B11_W(0x1f);  
  
  // Address Map Register5
  //x DDRC_ADDRMAP5_VAL = 0x00000f0f; // addrmap_row_b11=0x0, addrmap_row_b2_10=0x0, addrmap_row_b1=0xf, addrmap_row_b0=0xf
  // Inomize new code
  DDRC_ADDRMAP5_ADDRMAP_ROW_B0_W(0x4);
  DDRC_ADDRMAP5_ADDRMAP_ROW_B1_W(0x4);
  DDRC_ADDRMAP5_ADDRMAP_ROW_B2_10_W(0xf);
  DDRC_ADDRMAP5_ADDRMAP_ROW_B11_W(0x4);  
  
  // Address Map Register6
  //X DDRC_ADDRMAP6_VAL = 0x040f0404; // lpddr3_6gb_12gb=0, lpddr4_6gb_12gb_24gb=0x0, addrmap_row_b15=0x4, addrmap_row_b14=0xf, addrmap_row_b13=0x4, addrmap_row_b12=0x4
  // Inomize new code
  DDRC_ADDRMAP6_ADDRMAP_ROW_B12_W(0x4);
  DDRC_ADDRMAP6_ADDRMAP_ROW_B13_W(0x4);
  DDRC_ADDRMAP6_ADDRMAP_ROW_B14_W(0x4);
  DDRC_ADDRMAP6_ADDRMAP_ROW_B15_W(0x4);
  DDRC_ADDRMAP6_LPDDR4_6GB_12GB_24GB_W(0x0);
  DDRC_ADDRMAP6_LPDDR3_6GB_12GB_W(0x0);  
  
  // Address Map Register7
  //X DDRC_ADDRMAP7_VAL = 0x0f040404; // addrmap_row_b17=0x4, addrmap_row_b16=0x4
  //X //..............................// @@@@ according to spec bits 31:12 are reserved and so must be zero! @@@@
  // Inomize new code
  DDRC_ADDRMAP7_ADDRMAP_ROW_B16_W(0Xf);
  
  // Address Map Register9
  //X DDRC_ADDRMAP9_VAL = 0x04040404; // addrmap_row_b5=0x4, addrmap_row_b4=0x4, addrmap_row_b3=0x4, addrmap_row_b2=0x4
  // Inomize new code
  DDRC_ADDRMAP9_ADDRMAP_ROW_B2_W(0x4);
  DDRC_ADDRMAP9_ADDRMAP_ROW_B3_W(0x4);
  DDRC_ADDRMAP9_ADDRMAP_ROW_B4_W(0x4);
  DDRC_ADDRMAP9_ADDRMAP_ROW_B5_W(0x4);
  
  // Address Map Register10
  //x DDRC_ADDRMAP10_VAL = 0x04040404; // addrmap_row_b9=0x4, addrmap_row_b8=0x4, addrmap_row_b7=0x4, addrmap_row_b6=0x4
  // Inomize new code
  DDRC_ADDRMAP10_ADDRMAP_ROW_B6_W(0x4);
  DDRC_ADDRMAP10_ADDRMAP_ROW_B7_W(0x4);
  DDRC_ADDRMAP10_ADDRMAP_ROW_B8_W(0x4);
  DDRC_ADDRMAP10_ADDRMAP_ROW_B9_W(0x4);
  
  // Address Map Register11
  //x DDRC_ADDRMAP11_VAL = 0x00000004; // addrmap_cid_b1=0x00, addrmap_cid_b0=0x00, addrmap_row_b10=0x4
  // Inomize new code
  DDRC_ADDRMAP11_ADDRMAP_ROW_B10_W(0x4);
}
#endif
