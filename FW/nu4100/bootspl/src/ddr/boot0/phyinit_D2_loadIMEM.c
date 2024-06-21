#include "phyinit.h"

#define DDRPHY_IMEM_SIZE	16384
extern unsigned short ddrphy_imem_2D[DDRPHY_IMEM_SIZE];//

// ===========================================================================
void phyinit_D2_loadIMEM(void) {

  dwc_ddrphy_apb_wr(0xd0000,0x0); // DWC_DDRPHYA_APBONLY0_MicroContMuxSel
// // [dwc_ddrphy_phyinit_WriteOutMem] STARTING. offset 0x50000 size 0x4000

  int unsigned i;
#ifndef _ZEBU  
  for (i = 0; i <  DDRPHY_IMEM_SIZE; i++)
    {
      dwc_ddrphy_apb_wr_i(i,ddrphy_imem_2D[i]);
    }

// [dwc_ddrphy_phyinit_WriteOutMem] DONE.  Index 0x4000
// // 2.	Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1. 
// //      This allows the firmware unrestricted access to the configuration CSRs. 
  dwc_ddrphy_apb_wr(0xd0000,0x1); // DWC_DDRPHYA_APBONLY0_MicroContMuxSel
// // [dwc_ddrphy_phyinit_D_loadIMEM, 2D] End of dwc_ddrphy_phyinit_D_loadIMEM()
// // [phyinit_F_loadDMEM, 2D] Start of dwc_ddrphy_phyinit_F_loadDMEM (pstate=0, Train2D=1)
// 
#endif //_ZEBU// 
}
