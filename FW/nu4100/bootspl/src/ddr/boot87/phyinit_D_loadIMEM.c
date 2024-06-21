// ===========================================================================
// This C code is derived from Inomize's SystemVerilog of "phyinit_imem_1d.sv"
//
// Inomize, probably, generated their code by translating the set of APB writes (to load IMEM image into the SRAM)
// from running of Synopsys C program "dwc_ddrphy_phyinit_D_loadIMEM.c".
//
// This program is in the provided code in the phyinit directory i.e.
// /tools/snps/dw/dwc_lpddr4_multiphy_v2_tsmc12ffc18_2.00a/Latest/phyinit/Latest/software/lpddr4/src/dwc_ddrphy_phyinit_D_loadIMEM.c
// (but maybe Inomize have a newer version...)
//
// There are, apparently, two IMEM images "1D" and "2D", and Inomize took "1D"...

#include "phyinit.h"

//#define ICCM_BASE_ADDR 0x0C140000

#define DDRPHY_IMEM_SIZE	16384
extern unsigned short ddrphy_imem[DDRPHY_IMEM_SIZE];//

// ===========================================================================
void phyinit_D_loadIMEM(void) {

// //##############################################################
// //
// // (D) Load the 1D IMEM image
// // 
// // This function loads the training firmware IMEM image into the SRAM.
// // See PhyInit App Note for detailed description and function usage
// // 
// //##############################################################
// 
// 
// // [dwc_ddrphy_phyinit_D_loadIMEM, 1D] Programming MemResetL to 0x2
  dwc_ddrphy_apb_wr(0x20060,0x2); // DWC_DDRPHYA_MASTER0_MemResetL
// [dwc_ddrphy_phyinit_storeIncvFile] Reading input file: /space/users/erand/DDR_FW/2.00a/firmware/A-2018.05/lpddr4/lpddr4_pmu_train_imem.incv

// // 1.	Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
// //       This allows the memory controller unrestricted access to the configuration CSRs. 
  dwc_ddrphy_apb_wr(0xd0000,0x0); // DWC_DDRPHYA_APBONLY0_MicroContMuxSel
// // [dwc_ddrphy_phyinit_WriteOutMem] STARTING. offset 0x50000 size 0x4000

 int unsigned i;

 for (i = 0; i < DDRPHY_IMEM_SIZE; i++)
   {
     dwc_ddrphy_apb_wr_i(i,ddrphy_imem[i]);
   }

// // [dwc_ddrphy_phyinit_WriteOutMem] DONE.  Index 0x4000
// // 2.	Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1. 
// //      This allows the firmware unrestricted access to the configuration CSRs. 
  dwc_ddrphy_apb_wr(0xd0000,0x1); // DWC_DDRPHYA_APBONLY0_MicroContMuxSel
// // [dwc_ddrphy_phyinit_D_loadIMEM, 1D] End of dwc_ddrphy_phyinit_D_loadIMEM()
// 
// 
}
