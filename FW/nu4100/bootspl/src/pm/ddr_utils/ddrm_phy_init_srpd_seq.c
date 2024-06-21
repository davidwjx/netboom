#include "nu4000_c0_gme_regs.h"
#include "nu4000_c0_ddrc_mp_regs.h"
#include "nu4000_c0_ddrc_regs.h"
#include "nu4000_c0_ddrp_regs.h"
#include "pm.h"

extern void phyinit_I_loadPIEImage (unsigned int freq);
// ==========================================================================================
// DDR PHY configuration
// Derived from Inomize's "ddrm_phy_init_seq.sv"
// Synopsys doc name: dwc_lpddr4_multiphy_v2_pub_databook.pdf
// Doc location: http://svn/repos/inuitive/db/NU4000B0/vlsi/ddrm/trunk/doc/IP/Synopsys
// PUB = Phy Utility Block
//
void ddrm_phy_init_srpd_seq(int freq) {
  
  //volatile unsigned int data_val;
  volatile unsigned int delay_cnt;

  // Go according to the steps of Figure 5-12 "Initialization Sequence"
  
  //-------------------------------------------
  // Step A: Bring up VDD, VDDQ, and VAA 
  //
  // The power supplies can come up and stabilize in any order. 
  // While the power supplies are coming up, all outputs will be unknown and the values of the inputs are don't cares.
  
  
  //-------------------------------------------
  // Step B: Start Clocks and Reset the PHY 
  //
  // The procedure to reset the PHY is as follows: 
  // 
  // 1. Drive PwrOkIn to 0. Note: Reset, DfiClk, and APBCLK can be X. 
  // 2. Start DfiClk and APBCLK 
  // 3. Drive Reset to 1. Note: The combination of PwrOkIn=0 and Reset=1 signals a cold reset to the PHY.
  
  // Doc: "During Cold Reset, the reset signal must be asserted at least 8 DfiClks prior to the assertion of PwrOkIn".  
  // 4. Wait a minimum of 8 cycles. 
  // ==> this is done anyway since it takes much longer than that until this program is running...
  
  // 5. Drive PwrOkIn to 1. Once the PwrOkIn is asserted (and Reset is still asserted), 
  //    DfiClk synchronously switches to any legal input frequency.
  GME_DDRM_CONTROL_DDRP_PWROK_W(1); // Register: ddrm_control, Field: ddrp_pwrok "DDR PHY power OK"
  
  // 6. Wait a minimum of 64 cycles. Note: This is the reset period for the PHY.
  delay_cnt=0;
  while (delay_cnt<65) delay_cnt++;
  
  // 7. Drive Reset to 0. Note: All DFI and APB inputs must be driven at valid reset states before the deassertion of Reset.
  GME_DDRM_CONTROL_HOLD_DDRP_APB_RESET_W(0);  // Register: ddrm_control, Field: hold_ddrp_apb_reset: "release reset when this bit is set to 0"
  GME_DDRM_CONTROL_HOLD_DDRP_RESET_W(0);      // Register: ddrm_control, Field: hold_ddrp_reset: "release reset when this bit is set to 0"


  // 8. The PHY is now in the reset state and is ready to accept APB transactions.
  
  //-------------------------------------------
  // Step C: Initialize PHY Configuration
  
  // Load the required PHY configuration registers for the appropriate mode and memory configuration

  // phyinit_C_initPhyConfig_srpd(freq);
  phyinit_C_initPhyConfig(freq);

  //-------------------------------------------
  // Step D: Load the 1D IMEM image
  
  // Before the PHY training firmware can be run,
  // the firmware program image must be loaded into the instruction memory SRAM
  // phyinit_D_loadIMEM();

  // Uri: "Steps E-H are not required in case of skip training" - Begin
  //-------------------------------------------
  // Step E: Set the PHY input clocks to the desired frequency
  
  // @@@@ Inomize code did nothing here!
  // @@@@ I think this is because frequency has already been set to 800MHz.
  // @@@@ The PUB spec says:
  // @@@@ "For more information, refer to the dwc_ddrphy_phyinit_E_setDfiClk() program in the provided code in the phyinit directory".
  // @@@@ However, in
  // @@@@ /tools/snps/dw/dwc_lpddr4_multiphy_v2_tsmc12ffc18_2.00a/Latest/phyinit/Latest/software/lpddr4/src/
  // @@@@ there is no such program...
  
  
  //-------------------------------------------
  // Step F: Write the Message Block parameters for the training firmware
  
  // In order for the firmware to run, it must be given some information about the system,
  // the memory configuration, and the memory training steps that it is to run.
  // phyinit_F_loadDMEM_srpd();
  phyinit_F_loadDMEM(freq);
  //-------------------------------------------
  //phyinit_G_waitFwDone();
//added for SRPD  - Begin

  ddr_data_training_restore();                // restore post training data
//added for SRPD  - End
  
  //-------------------------------------------
  // Step H: Read the Message Block results
  
  // Spec:
  // "Once the training firmware has completed,
  //  it returns the results of the training in the data memory in the message block structure.
  //  The message block can be read to obtain these results.
  //  For more information, refer to the dwc_ddrphy_phyinit_H_readMsgBlock() program in the provided code in the phyinit directory".
  
  // According to comments inside
  // /tools/snps/dw/dwc_lpddr4_multiphy_v2_tsmc12ffc18_2.00a/Latest/phyinit/Latest/software/lpddr4/src/dwc_ddrphy_phyinit_H_readMsgBlock.c
  // Say:
  // "The procedure is as follows:
  //  - Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
  //  - Read the Firmware Message Block to obtain the results from the training.
  //    The user customizable function dwc_ddrphy_phyinit_userCustom_H_readMsgBlock()
  //    is called for the user to read any specific results from training.
  //  - Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1.
  //  - If training is required at another frequency, repeat the operations starting at step (E)"
  
  // I do not have any application note for dwc_ddrphy_phyinit_userCustom_H_readMsgBlock().
  // It seems that Inomize did not implement it at all!
  
  // Anyway it is probably something like "load IMEM/DMEM" functions
  // only with memory reads instead of writes (@@@@ but which memory? which address)


// Uri: "not required in case of skip training" - end  
  
  //-------------------------------------------
  // Step I: Load PHY Init Engine Image 
  
  // Load the PHY Initialization Engine memory with the provided initialization sequence.

  // Inomize: "<Note: For LPDDR3/LPDDR4, this sequence will include the necessary retraining code.>"
  
  // Spec: 
  // "For more information, refer to thedwc_ddrphy_phyinit_I_loadPIEImage() program in the provided code in the phyinit directory".
  
  // (Inomize redundant step)
  // Register: MicroContMuxSel = PMU Config Mux Select
  // Field: This register controls access to the PHY configuration registers.
  //        1 = MicroController/PHY Init Engine has control of csr bus.
  //        0 = MicroController/PHY Init Engine csr requests are ignored.
  DDRP_APBONLY_MICROCONTMUXSEL_VAL = 0x0; // rf.DWC_DDRPHYA_APBONLY0.MicroContMuxSel.write(.status(status), .value(0));
  
  // actual load function
  phyinit_I_loadPIEImage(freq);

  //-------------------------------------------
  // Step J: Initialize the PHY to Mission Mode through DFI Initialization 
  
  // This is probably what is done in steps 15, 16, 17 as listed in
  // uMCTL2 Databook Table 6-7 - "DWC_ddr_umctl2 and Memory Initialization with LPDDR4 mPHY_v2".

  
  //-------------------------------------------
}

