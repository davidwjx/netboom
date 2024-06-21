// ===========================================================================
// This C code is derived from Inomize's SystemVerilog of "phyinit_I_loadPIEImage.sv"

// Inomize's code is a combination of
// - the main program "dwc_ddrphy_phyinit_I_loadPIEImage()"
// - and the function "dwc_ddrphy_phyinit_LoadPieProdCode()"
// both have some version in:
// /tools/snps/dw/dwc_lpddr4_multiphy_v2_tsmc12ffc18_2.00a/Latest/phyinit/Latest/software/lpddr4/src/

// However,
// since Inomize may have used a newer version and
// since the main program has parts that depend on user's inputs and
// since function "dwc_ddrphy_phyinit_LoadPieProdCode" has two branches (per DramDataWidth) and
// since function "dwc_ddrphy_phyinit_LoadPieProdCode" uses address offsets and not register names and
// since Inomize claim their code works
// we translated here their SV code as is.



#include "nu4000_c0_gme_regs.h"
#include "nu4000_c0_ddrc_mp_regs.h"
#include "nu4000_c0_ddrc_regs.h"
#include "nu4000_c0_ddrp_regs.h"
#include "phyinit.h"


// ===========================================================================
void phyinit_I_loadPIEImage (unsigned int freq) {

  GME_SPARE_0_SPARE_BITS_W(0xa08a0001); // write
  
// //##############################################################
// //
// // (I) Load PHY Init Engine Image 
// // 
// // Load the PHY Initialization Engine memory with the provided initialization sequence.
// // See PhyInit App Note for detailed description and function usage
// // 
// // For LPDDR3/LPDDR4, this sequence will include the necessary retraining code.
// // 
// //##############################################################
// 
// 
// // Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0. 
// // This allows the memory controller unrestricted access to the configuration CSRs. 
dwc_ddrphy_apb_wr(0xd0000,0x0); // DWC_DDRPHYA_APBONLY0_MicroContMuxSel
// // [phyinit_I_loadPIEImage] Programming PIE Production Code
dwc_ddrphy_apb_wr(0x90000,0x10); // DWC_DDRPHYA_INITENG0_PreSequenceReg0b0s0
dwc_ddrphy_apb_wr(0x90001,0x400); // DWC_DDRPHYA_INITENG0_PreSequenceReg0b0s1
dwc_ddrphy_apb_wr(0x90002,0x10e); // DWC_DDRPHYA_INITENG0_PreSequenceReg0b0s2
dwc_ddrphy_apb_wr(0x90003,0x0); // DWC_DDRPHYA_INITENG0_PreSequenceReg0b1s0
dwc_ddrphy_apb_wr(0x90004,0x0); // DWC_DDRPHYA_INITENG0_PreSequenceReg0b1s1
dwc_ddrphy_apb_wr(0x90005,0x8); // DWC_DDRPHYA_INITENG0_PreSequenceReg0b1s2
dwc_ddrphy_apb_wr(0x90029,0xb); // DWC_DDRPHYA_INITENG0_SequenceReg0b0s0
dwc_ddrphy_apb_wr(0x9002a,0x480); // DWC_DDRPHYA_INITENG0_SequenceReg0b0s1
dwc_ddrphy_apb_wr(0x9002b,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b0s2
dwc_ddrphy_apb_wr(0x9002c,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b1s0
dwc_ddrphy_apb_wr(0x9002d,0x448); // DWC_DDRPHYA_INITENG0_SequenceReg0b1s1
dwc_ddrphy_apb_wr(0x9002e,0x139); // DWC_DDRPHYA_INITENG0_SequenceReg0b1s2
dwc_ddrphy_apb_wr(0x9002f,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b2s0
dwc_ddrphy_apb_wr(0x90030,0x478); // DWC_DDRPHYA_INITENG0_SequenceReg0b2s1
dwc_ddrphy_apb_wr(0x90031,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b2s2
dwc_ddrphy_apb_wr(0x90032,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b3s0
dwc_ddrphy_apb_wr(0x90033,0xe8); // DWC_DDRPHYA_INITENG0_SequenceReg0b3s1
dwc_ddrphy_apb_wr(0x90034,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b3s2
dwc_ddrphy_apb_wr(0x90035,0x2); // DWC_DDRPHYA_INITENG0_SequenceReg0b4s0
dwc_ddrphy_apb_wr(0x90036,0x10); // DWC_DDRPHYA_INITENG0_SequenceReg0b4s1
dwc_ddrphy_apb_wr(0x90037,0x139); // DWC_DDRPHYA_INITENG0_SequenceReg0b4s2
dwc_ddrphy_apb_wr(0x90038,0xb); // DWC_DDRPHYA_INITENG0_SequenceReg0b5s0
dwc_ddrphy_apb_wr(0x90039,0x7c0); // DWC_DDRPHYA_INITENG0_SequenceReg0b5s1
dwc_ddrphy_apb_wr(0x9003a,0x139); // DWC_DDRPHYA_INITENG0_SequenceReg0b5s2
dwc_ddrphy_apb_wr(0x9003b,0x44); // DWC_DDRPHYA_INITENG0_SequenceReg0b6s0
dwc_ddrphy_apb_wr(0x9003c,0x630); // DWC_DDRPHYA_INITENG0_SequenceReg0b6s1
dwc_ddrphy_apb_wr(0x9003d,0x159); // DWC_DDRPHYA_INITENG0_SequenceReg0b6s2
dwc_ddrphy_apb_wr(0x9003e,0x14f); // DWC_DDRPHYA_INITENG0_SequenceReg0b7s0
dwc_ddrphy_apb_wr(0x9003f,0x630); // DWC_DDRPHYA_INITENG0_SequenceReg0b7s1
dwc_ddrphy_apb_wr(0x90040,0x159); // DWC_DDRPHYA_INITENG0_SequenceReg0b7s2
dwc_ddrphy_apb_wr(0x90041,0x47); // DWC_DDRPHYA_INITENG0_SequenceReg0b8s0
dwc_ddrphy_apb_wr(0x90042,0x630); // DWC_DDRPHYA_INITENG0_SequenceReg0b8s1
dwc_ddrphy_apb_wr(0x90043,0x149); // DWC_DDRPHYA_INITENG0_SequenceReg0b8s2
dwc_ddrphy_apb_wr(0x90044,0x4f); // DWC_DDRPHYA_INITENG0_SequenceReg0b9s0
dwc_ddrphy_apb_wr(0x90045,0x630); // DWC_DDRPHYA_INITENG0_SequenceReg0b9s1
dwc_ddrphy_apb_wr(0x90046,0x179); // DWC_DDRPHYA_INITENG0_SequenceReg0b9s2
dwc_ddrphy_apb_wr(0x90047,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b10s0
dwc_ddrphy_apb_wr(0x90048,0xe0); // DWC_DDRPHYA_INITENG0_SequenceReg0b10s1
dwc_ddrphy_apb_wr(0x90049,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b10s2
dwc_ddrphy_apb_wr(0x9004a,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b11s0
dwc_ddrphy_apb_wr(0x9004b,0x7c8); // DWC_DDRPHYA_INITENG0_SequenceReg0b11s1
dwc_ddrphy_apb_wr(0x9004c,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b11s2
dwc_ddrphy_apb_wr(0x9004d,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b12s0
dwc_ddrphy_apb_wr(0x9004e,0x1); // DWC_DDRPHYA_INITENG0_SequenceReg0b12s1
dwc_ddrphy_apb_wr(0x9004f,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b12s2
dwc_ddrphy_apb_wr(0x90050,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b13s0
dwc_ddrphy_apb_wr(0x90051,0x45a); // DWC_DDRPHYA_INITENG0_SequenceReg0b13s1
dwc_ddrphy_apb_wr(0x90052,0x9); // DWC_DDRPHYA_INITENG0_SequenceReg0b13s2
dwc_ddrphy_apb_wr(0x90053,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b14s0
dwc_ddrphy_apb_wr(0x90054,0x448); // DWC_DDRPHYA_INITENG0_SequenceReg0b14s1
dwc_ddrphy_apb_wr(0x90055,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b14s2
dwc_ddrphy_apb_wr(0x90056,0x40); // DWC_DDRPHYA_INITENG0_SequenceReg0b15s0
dwc_ddrphy_apb_wr(0x90057,0x630); // DWC_DDRPHYA_INITENG0_SequenceReg0b15s1
dwc_ddrphy_apb_wr(0x90058,0x179); // DWC_DDRPHYA_INITENG0_SequenceReg0b15s2
dwc_ddrphy_apb_wr(0x90059,0x1); // DWC_DDRPHYA_INITENG0_SequenceReg0b16s0
dwc_ddrphy_apb_wr(0x9005a,0x618); // DWC_DDRPHYA_INITENG0_SequenceReg0b16s1
dwc_ddrphy_apb_wr(0x9005b,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b16s2
dwc_ddrphy_apb_wr(0x9005c,0x40c0); // DWC_DDRPHYA_INITENG0_SequenceReg0b17s0
dwc_ddrphy_apb_wr(0x9005d,0x630); // DWC_DDRPHYA_INITENG0_SequenceReg0b17s1
dwc_ddrphy_apb_wr(0x9005e,0x149); // DWC_DDRPHYA_INITENG0_SequenceReg0b17s2
dwc_ddrphy_apb_wr(0x9005f,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b18s0
dwc_ddrphy_apb_wr(0x90060,0x4); // DWC_DDRPHYA_INITENG0_SequenceReg0b18s1
dwc_ddrphy_apb_wr(0x90061,0x48); // DWC_DDRPHYA_INITENG0_SequenceReg0b18s2
dwc_ddrphy_apb_wr(0x90062,0x4040); // DWC_DDRPHYA_INITENG0_SequenceReg0b19s0
dwc_ddrphy_apb_wr(0x90063,0x630); // DWC_DDRPHYA_INITENG0_SequenceReg0b19s1
dwc_ddrphy_apb_wr(0x90064,0x149); // DWC_DDRPHYA_INITENG0_SequenceReg0b19s2
dwc_ddrphy_apb_wr(0x90065,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b20s0
dwc_ddrphy_apb_wr(0x90066,0x4); // DWC_DDRPHYA_INITENG0_SequenceReg0b20s1
dwc_ddrphy_apb_wr(0x90067,0x48); // DWC_DDRPHYA_INITENG0_SequenceReg0b20s2
dwc_ddrphy_apb_wr(0x90068,0x40); // DWC_DDRPHYA_INITENG0_SequenceReg0b21s0
dwc_ddrphy_apb_wr(0x90069,0x630); // DWC_DDRPHYA_INITENG0_SequenceReg0b21s1
dwc_ddrphy_apb_wr(0x9006a,0x149); // DWC_DDRPHYA_INITENG0_SequenceReg0b21s2
dwc_ddrphy_apb_wr(0x9006b,0x10); // DWC_DDRPHYA_INITENG0_SequenceReg0b22s0
dwc_ddrphy_apb_wr(0x9006c,0x4); // DWC_DDRPHYA_INITENG0_SequenceReg0b22s1
dwc_ddrphy_apb_wr(0x9006d,0x18); // DWC_DDRPHYA_INITENG0_SequenceReg0b22s2
dwc_ddrphy_apb_wr(0x9006e,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b23s0
dwc_ddrphy_apb_wr(0x9006f,0x4); // DWC_DDRPHYA_INITENG0_SequenceReg0b23s1
dwc_ddrphy_apb_wr(0x90070,0x78); // DWC_DDRPHYA_INITENG0_SequenceReg0b23s2
dwc_ddrphy_apb_wr(0x90071,0x549); // DWC_DDRPHYA_INITENG0_SequenceReg0b24s0
dwc_ddrphy_apb_wr(0x90072,0x630); // DWC_DDRPHYA_INITENG0_SequenceReg0b24s1
dwc_ddrphy_apb_wr(0x90073,0x159); // DWC_DDRPHYA_INITENG0_SequenceReg0b24s2
dwc_ddrphy_apb_wr(0x90074,0xd49); // DWC_DDRPHYA_INITENG0_SequenceReg0b25s0
dwc_ddrphy_apb_wr(0x90075,0x630); // DWC_DDRPHYA_INITENG0_SequenceReg0b25s1
dwc_ddrphy_apb_wr(0x90076,0x159); // DWC_DDRPHYA_INITENG0_SequenceReg0b25s2
dwc_ddrphy_apb_wr(0x90077,0x94a); // DWC_DDRPHYA_INITENG0_SequenceReg0b26s0
dwc_ddrphy_apb_wr(0x90078,0x630); // DWC_DDRPHYA_INITENG0_SequenceReg0b26s1
dwc_ddrphy_apb_wr(0x90079,0x159); // DWC_DDRPHYA_INITENG0_SequenceReg0b26s2
dwc_ddrphy_apb_wr(0x9007a,0x441); // DWC_DDRPHYA_INITENG0_SequenceReg0b27s0
dwc_ddrphy_apb_wr(0x9007b,0x630); // DWC_DDRPHYA_INITENG0_SequenceReg0b27s1
dwc_ddrphy_apb_wr(0x9007c,0x149); // DWC_DDRPHYA_INITENG0_SequenceReg0b27s2
dwc_ddrphy_apb_wr(0x9007d,0x42); // DWC_DDRPHYA_INITENG0_SequenceReg0b28s0
dwc_ddrphy_apb_wr(0x9007e,0x630); // DWC_DDRPHYA_INITENG0_SequenceReg0b28s1
dwc_ddrphy_apb_wr(0x9007f,0x149); // DWC_DDRPHYA_INITENG0_SequenceReg0b28s2
dwc_ddrphy_apb_wr(0x90080,0x1); // DWC_DDRPHYA_INITENG0_SequenceReg0b29s0
dwc_ddrphy_apb_wr(0x90081,0x630); // DWC_DDRPHYA_INITENG0_SequenceReg0b29s1
dwc_ddrphy_apb_wr(0x90082,0x149); // DWC_DDRPHYA_INITENG0_SequenceReg0b29s2
dwc_ddrphy_apb_wr(0x90083,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b30s0
dwc_ddrphy_apb_wr(0x90084,0xe0); // DWC_DDRPHYA_INITENG0_SequenceReg0b30s1
dwc_ddrphy_apb_wr(0x90085,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b30s2
dwc_ddrphy_apb_wr(0x90086,0xa); // DWC_DDRPHYA_INITENG0_SequenceReg0b31s0
dwc_ddrphy_apb_wr(0x90087,0x10); // DWC_DDRPHYA_INITENG0_SequenceReg0b31s1
dwc_ddrphy_apb_wr(0x90088,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b31s2
dwc_ddrphy_apb_wr(0x90089,0x9); // DWC_DDRPHYA_INITENG0_SequenceReg0b32s0
dwc_ddrphy_apb_wr(0x9008a,0x3c0); // DWC_DDRPHYA_INITENG0_SequenceReg0b32s1
dwc_ddrphy_apb_wr(0x9008b,0x149); // DWC_DDRPHYA_INITENG0_SequenceReg0b32s2
dwc_ddrphy_apb_wr(0x9008c,0x9); // DWC_DDRPHYA_INITENG0_SequenceReg0b33s0
dwc_ddrphy_apb_wr(0x9008d,0x3c0); // DWC_DDRPHYA_INITENG0_SequenceReg0b33s1
dwc_ddrphy_apb_wr(0x9008e,0x159); // DWC_DDRPHYA_INITENG0_SequenceReg0b33s2
dwc_ddrphy_apb_wr(0x9008f,0x18); // DWC_DDRPHYA_INITENG0_SequenceReg0b34s0
dwc_ddrphy_apb_wr(0x90090,0x10); // DWC_DDRPHYA_INITENG0_SequenceReg0b34s1
dwc_ddrphy_apb_wr(0x90091,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b34s2
dwc_ddrphy_apb_wr(0x90092,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b35s0
dwc_ddrphy_apb_wr(0x90093,0x3c0); // DWC_DDRPHYA_INITENG0_SequenceReg0b35s1
dwc_ddrphy_apb_wr(0x90094,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b35s2
dwc_ddrphy_apb_wr(0x90095,0x18); // DWC_DDRPHYA_INITENG0_SequenceReg0b36s0
dwc_ddrphy_apb_wr(0x90096,0x4); // DWC_DDRPHYA_INITENG0_SequenceReg0b36s1
dwc_ddrphy_apb_wr(0x90097,0x48); // DWC_DDRPHYA_INITENG0_SequenceReg0b36s2
dwc_ddrphy_apb_wr(0x90098,0x18); // DWC_DDRPHYA_INITENG0_SequenceReg0b37s0
dwc_ddrphy_apb_wr(0x90099,0x4); // DWC_DDRPHYA_INITENG0_SequenceReg0b37s1
dwc_ddrphy_apb_wr(0x9009a,0x58); // DWC_DDRPHYA_INITENG0_SequenceReg0b37s2
dwc_ddrphy_apb_wr(0x9009b,0xb); // DWC_DDRPHYA_INITENG0_SequenceReg0b38s0
dwc_ddrphy_apb_wr(0x9009c,0x10); // DWC_DDRPHYA_INITENG0_SequenceReg0b38s1
dwc_ddrphy_apb_wr(0x9009d,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b38s2
dwc_ddrphy_apb_wr(0x9009e,0x1); // DWC_DDRPHYA_INITENG0_SequenceReg0b39s0
dwc_ddrphy_apb_wr(0x9009f,0x10); // DWC_DDRPHYA_INITENG0_SequenceReg0b39s1
dwc_ddrphy_apb_wr(0x900a0,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b39s2
dwc_ddrphy_apb_wr(0x900a1,0x5); // DWC_DDRPHYA_INITENG0_SequenceReg0b40s0
dwc_ddrphy_apb_wr(0x900a2,0x7c0); // DWC_DDRPHYA_INITENG0_SequenceReg0b40s1
dwc_ddrphy_apb_wr(0x900a3,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b40s2
dwc_ddrphy_apb_wr(0x40000,0x811); // DWC_DDRPHYA_ACSM0_AcsmSeq0x0
dwc_ddrphy_apb_wr(0x40020,0x880); // DWC_DDRPHYA_ACSM0_AcsmSeq1x0
dwc_ddrphy_apb_wr(0x40040,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq2x0
dwc_ddrphy_apb_wr(0x40060,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x0
dwc_ddrphy_apb_wr(0x40001,0x4008); // DWC_DDRPHYA_ACSM0_AcsmSeq0x1
dwc_ddrphy_apb_wr(0x40021,0x83); // DWC_DDRPHYA_ACSM0_AcsmSeq1x1
dwc_ddrphy_apb_wr(0x40041,0x4f); // DWC_DDRPHYA_ACSM0_AcsmSeq2x1
dwc_ddrphy_apb_wr(0x40061,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x1
dwc_ddrphy_apb_wr(0x40002,0x4040); // DWC_DDRPHYA_ACSM0_AcsmSeq0x2
dwc_ddrphy_apb_wr(0x40022,0x83); // DWC_DDRPHYA_ACSM0_AcsmSeq1x2
dwc_ddrphy_apb_wr(0x40042,0x51); // DWC_DDRPHYA_ACSM0_AcsmSeq2x2
dwc_ddrphy_apb_wr(0x40062,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x2
dwc_ddrphy_apb_wr(0x40003,0x811); // DWC_DDRPHYA_ACSM0_AcsmSeq0x3
dwc_ddrphy_apb_wr(0x40023,0x880); // DWC_DDRPHYA_ACSM0_AcsmSeq1x3
dwc_ddrphy_apb_wr(0x40043,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq2x3
dwc_ddrphy_apb_wr(0x40063,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x3
dwc_ddrphy_apb_wr(0x40004,0x720); // DWC_DDRPHYA_ACSM0_AcsmSeq0x4
dwc_ddrphy_apb_wr(0x40024,0xf); // DWC_DDRPHYA_ACSM0_AcsmSeq1x4
dwc_ddrphy_apb_wr(0x40044,0x1740); // DWC_DDRPHYA_ACSM0_AcsmSeq2x4
dwc_ddrphy_apb_wr(0x40064,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x4
dwc_ddrphy_apb_wr(0x40005,0x16); // DWC_DDRPHYA_ACSM0_AcsmSeq0x5
dwc_ddrphy_apb_wr(0x40025,0x83); // DWC_DDRPHYA_ACSM0_AcsmSeq1x5
dwc_ddrphy_apb_wr(0x40045,0x4b); // DWC_DDRPHYA_ACSM0_AcsmSeq2x5
dwc_ddrphy_apb_wr(0x40065,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x5
dwc_ddrphy_apb_wr(0x40006,0x716); // DWC_DDRPHYA_ACSM0_AcsmSeq0x6
dwc_ddrphy_apb_wr(0x40026,0xf); // DWC_DDRPHYA_ACSM0_AcsmSeq1x6
dwc_ddrphy_apb_wr(0x40046,0x2001); // DWC_DDRPHYA_ACSM0_AcsmSeq2x6
dwc_ddrphy_apb_wr(0x40066,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x6
dwc_ddrphy_apb_wr(0x40007,0x716); // DWC_DDRPHYA_ACSM0_AcsmSeq0x7
dwc_ddrphy_apb_wr(0x40027,0xf); // DWC_DDRPHYA_ACSM0_AcsmSeq1x7
dwc_ddrphy_apb_wr(0x40047,0x2800); // DWC_DDRPHYA_ACSM0_AcsmSeq2x7
dwc_ddrphy_apb_wr(0x40067,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x7
dwc_ddrphy_apb_wr(0x40008,0x716); // DWC_DDRPHYA_ACSM0_AcsmSeq0x8
dwc_ddrphy_apb_wr(0x40028,0xf); // DWC_DDRPHYA_ACSM0_AcsmSeq1x8
dwc_ddrphy_apb_wr(0x40048,0xf00); // DWC_DDRPHYA_ACSM0_AcsmSeq2x8
dwc_ddrphy_apb_wr(0x40068,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x8
dwc_ddrphy_apb_wr(0x40009,0x720); // DWC_DDRPHYA_ACSM0_AcsmSeq0x9
dwc_ddrphy_apb_wr(0x40029,0xf); // DWC_DDRPHYA_ACSM0_AcsmSeq1x9
dwc_ddrphy_apb_wr(0x40049,0x1400); // DWC_DDRPHYA_ACSM0_AcsmSeq2x9
dwc_ddrphy_apb_wr(0x40069,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x9
dwc_ddrphy_apb_wr(0x4000a,0xe08); // DWC_DDRPHYA_ACSM0_AcsmSeq0x10
dwc_ddrphy_apb_wr(0x4002a,0xc15); // DWC_DDRPHYA_ACSM0_AcsmSeq1x10
dwc_ddrphy_apb_wr(0x4004a,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq2x10
dwc_ddrphy_apb_wr(0x4006a,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x10
dwc_ddrphy_apb_wr(0x4000b,0x623); // DWC_DDRPHYA_ACSM0_AcsmSeq0x11
dwc_ddrphy_apb_wr(0x4002b,0x15); // DWC_DDRPHYA_ACSM0_AcsmSeq1x11
dwc_ddrphy_apb_wr(0x4004b,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq2x11
dwc_ddrphy_apb_wr(0x4006b,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x11
dwc_ddrphy_apb_wr(0x4000c,0x4028); // DWC_DDRPHYA_ACSM0_AcsmSeq0x12
dwc_ddrphy_apb_wr(0x4002c,0x80); // DWC_DDRPHYA_ACSM0_AcsmSeq1x12
dwc_ddrphy_apb_wr(0x4004c,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq2x12
dwc_ddrphy_apb_wr(0x4006c,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x12
dwc_ddrphy_apb_wr(0x4000d,0xe08); // DWC_DDRPHYA_ACSM0_AcsmSeq0x13
dwc_ddrphy_apb_wr(0x4002d,0xc1a); // DWC_DDRPHYA_ACSM0_AcsmSeq1x13
dwc_ddrphy_apb_wr(0x4004d,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq2x13
dwc_ddrphy_apb_wr(0x4006d,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x13
dwc_ddrphy_apb_wr(0x4000e,0x623); // DWC_DDRPHYA_ACSM0_AcsmSeq0x14
dwc_ddrphy_apb_wr(0x4002e,0x1a); // DWC_DDRPHYA_ACSM0_AcsmSeq1x14
dwc_ddrphy_apb_wr(0x4004e,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq2x14
dwc_ddrphy_apb_wr(0x4006e,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x14
dwc_ddrphy_apb_wr(0x4000f,0x4040); // DWC_DDRPHYA_ACSM0_AcsmSeq0x15
dwc_ddrphy_apb_wr(0x4002f,0x80); // DWC_DDRPHYA_ACSM0_AcsmSeq1x15
dwc_ddrphy_apb_wr(0x4004f,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq2x15
dwc_ddrphy_apb_wr(0x4006f,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x15
dwc_ddrphy_apb_wr(0x40010,0x2604); // DWC_DDRPHYA_ACSM0_AcsmSeq0x16
dwc_ddrphy_apb_wr(0x40030,0x15); // DWC_DDRPHYA_ACSM0_AcsmSeq1x16
dwc_ddrphy_apb_wr(0x40050,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq2x16
dwc_ddrphy_apb_wr(0x40070,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x16
dwc_ddrphy_apb_wr(0x40011,0x708); // DWC_DDRPHYA_ACSM0_AcsmSeq0x17
dwc_ddrphy_apb_wr(0x40031,0x5); // DWC_DDRPHYA_ACSM0_AcsmSeq1x17
dwc_ddrphy_apb_wr(0x40051,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq2x17
dwc_ddrphy_apb_wr(0x40071,0x2002); // DWC_DDRPHYA_ACSM0_AcsmSeq3x17
dwc_ddrphy_apb_wr(0x40012,0x8); // DWC_DDRPHYA_ACSM0_AcsmSeq0x18
dwc_ddrphy_apb_wr(0x40032,0x80); // DWC_DDRPHYA_ACSM0_AcsmSeq1x18
dwc_ddrphy_apb_wr(0x40052,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq2x18
dwc_ddrphy_apb_wr(0x40072,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x18
dwc_ddrphy_apb_wr(0x40013,0x2604); // DWC_DDRPHYA_ACSM0_AcsmSeq0x19
dwc_ddrphy_apb_wr(0x40033,0x1a); // DWC_DDRPHYA_ACSM0_AcsmSeq1x19
dwc_ddrphy_apb_wr(0x40053,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq2x19
dwc_ddrphy_apb_wr(0x40073,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x19
dwc_ddrphy_apb_wr(0x40014,0x708); // DWC_DDRPHYA_ACSM0_AcsmSeq0x20
dwc_ddrphy_apb_wr(0x40034,0xa); // DWC_DDRPHYA_ACSM0_AcsmSeq1x20
dwc_ddrphy_apb_wr(0x40054,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq2x20
dwc_ddrphy_apb_wr(0x40074,0x2002); // DWC_DDRPHYA_ACSM0_AcsmSeq3x20
dwc_ddrphy_apb_wr(0x40015,0x4040); // DWC_DDRPHYA_ACSM0_AcsmSeq0x21
dwc_ddrphy_apb_wr(0x40035,0x80); // DWC_DDRPHYA_ACSM0_AcsmSeq1x21
dwc_ddrphy_apb_wr(0x40055,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq2x21
dwc_ddrphy_apb_wr(0x40075,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x21
dwc_ddrphy_apb_wr(0x40016,0x60a); // DWC_DDRPHYA_ACSM0_AcsmSeq0x22
dwc_ddrphy_apb_wr(0x40036,0x15); // DWC_DDRPHYA_ACSM0_AcsmSeq1x22
dwc_ddrphy_apb_wr(0x40056,0x1200); // DWC_DDRPHYA_ACSM0_AcsmSeq2x22
dwc_ddrphy_apb_wr(0x40076,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x22
dwc_ddrphy_apb_wr(0x40017,0x61a); // DWC_DDRPHYA_ACSM0_AcsmSeq0x23
dwc_ddrphy_apb_wr(0x40037,0x15); // DWC_DDRPHYA_ACSM0_AcsmSeq1x23
dwc_ddrphy_apb_wr(0x40057,0x1300); // DWC_DDRPHYA_ACSM0_AcsmSeq2x23
dwc_ddrphy_apb_wr(0x40077,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x23
dwc_ddrphy_apb_wr(0x40018,0x60a); // DWC_DDRPHYA_ACSM0_AcsmSeq0x24
dwc_ddrphy_apb_wr(0x40038,0x1a); // DWC_DDRPHYA_ACSM0_AcsmSeq1x24
dwc_ddrphy_apb_wr(0x40058,0x1200); // DWC_DDRPHYA_ACSM0_AcsmSeq2x24
dwc_ddrphy_apb_wr(0x40078,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x24
dwc_ddrphy_apb_wr(0x40019,0x642); // DWC_DDRPHYA_ACSM0_AcsmSeq0x25
dwc_ddrphy_apb_wr(0x40039,0x1a); // DWC_DDRPHYA_ACSM0_AcsmSeq1x25
dwc_ddrphy_apb_wr(0x40059,0x1300); // DWC_DDRPHYA_ACSM0_AcsmSeq2x25
dwc_ddrphy_apb_wr(0x40079,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x25
dwc_ddrphy_apb_wr(0x4001a,0x4808); // DWC_DDRPHYA_ACSM0_AcsmSeq0x26
dwc_ddrphy_apb_wr(0x4003a,0x880); // DWC_DDRPHYA_ACSM0_AcsmSeq1x26
dwc_ddrphy_apb_wr(0x4005a,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq2x26
dwc_ddrphy_apb_wr(0x4007a,0x0); // DWC_DDRPHYA_ACSM0_AcsmSeq3x26
dwc_ddrphy_apb_wr(0x900a4,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b41s0
dwc_ddrphy_apb_wr(0x900a5,0x790); // DWC_DDRPHYA_INITENG0_SequenceReg0b41s1
dwc_ddrphy_apb_wr(0x900a6,0x11a); // DWC_DDRPHYA_INITENG0_SequenceReg0b41s2
dwc_ddrphy_apb_wr(0x900a7,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b42s0
dwc_ddrphy_apb_wr(0x900a8,0x7aa); // DWC_DDRPHYA_INITENG0_SequenceReg0b42s1
dwc_ddrphy_apb_wr(0x900a9,0x2a); // DWC_DDRPHYA_INITENG0_SequenceReg0b42s2
dwc_ddrphy_apb_wr(0x900aa,0x10); // DWC_DDRPHYA_INITENG0_SequenceReg0b43s0
dwc_ddrphy_apb_wr(0x900ab,0x7b2); // DWC_DDRPHYA_INITENG0_SequenceReg0b43s1
dwc_ddrphy_apb_wr(0x900ac,0x2a); // DWC_DDRPHYA_INITENG0_SequenceReg0b43s2
dwc_ddrphy_apb_wr(0x900ad,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b44s0
dwc_ddrphy_apb_wr(0x900ae,0x7c8); // DWC_DDRPHYA_INITENG0_SequenceReg0b44s1
dwc_ddrphy_apb_wr(0x900af,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b44s2
dwc_ddrphy_apb_wr(0x900b0,0x10); // DWC_DDRPHYA_INITENG0_SequenceReg0b45s0
dwc_ddrphy_apb_wr(0x900b1,0x10); // DWC_DDRPHYA_INITENG0_SequenceReg0b45s1
dwc_ddrphy_apb_wr(0x900b2,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b45s2
dwc_ddrphy_apb_wr(0x900b3,0x10); // DWC_DDRPHYA_INITENG0_SequenceReg0b46s0
dwc_ddrphy_apb_wr(0x900b4,0x2a8); // DWC_DDRPHYA_INITENG0_SequenceReg0b46s1
dwc_ddrphy_apb_wr(0x900b5,0x129); // DWC_DDRPHYA_INITENG0_SequenceReg0b46s2
dwc_ddrphy_apb_wr(0x900b6,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b47s0
dwc_ddrphy_apb_wr(0x900b7,0x370); // DWC_DDRPHYA_INITENG0_SequenceReg0b47s1
dwc_ddrphy_apb_wr(0x900b8,0x129); // DWC_DDRPHYA_INITENG0_SequenceReg0b47s2
dwc_ddrphy_apb_wr(0x900b9,0xa); // DWC_DDRPHYA_INITENG0_SequenceReg0b48s0
dwc_ddrphy_apb_wr(0x900ba,0x3c8); // DWC_DDRPHYA_INITENG0_SequenceReg0b48s1
dwc_ddrphy_apb_wr(0x900bb,0x1a9); // DWC_DDRPHYA_INITENG0_SequenceReg0b48s2
dwc_ddrphy_apb_wr(0x900bc,0xc); // DWC_DDRPHYA_INITENG0_SequenceReg0b49s0
dwc_ddrphy_apb_wr(0x900bd,0x408); // DWC_DDRPHYA_INITENG0_SequenceReg0b49s1
dwc_ddrphy_apb_wr(0x900be,0x199); // DWC_DDRPHYA_INITENG0_SequenceReg0b49s2
dwc_ddrphy_apb_wr(0x900bf,0x14); // DWC_DDRPHYA_INITENG0_SequenceReg0b50s0
dwc_ddrphy_apb_wr(0x900c0,0x790); // DWC_DDRPHYA_INITENG0_SequenceReg0b50s1
dwc_ddrphy_apb_wr(0x900c1,0x11a); // DWC_DDRPHYA_INITENG0_SequenceReg0b50s2
dwc_ddrphy_apb_wr(0x900c2,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b51s0
dwc_ddrphy_apb_wr(0x900c3,0x4); // DWC_DDRPHYA_INITENG0_SequenceReg0b51s1
dwc_ddrphy_apb_wr(0x900c4,0x18); // DWC_DDRPHYA_INITENG0_SequenceReg0b51s2
dwc_ddrphy_apb_wr(0x900c5,0xe); // DWC_DDRPHYA_INITENG0_SequenceReg0b52s0
dwc_ddrphy_apb_wr(0x900c6,0x408); // DWC_DDRPHYA_INITENG0_SequenceReg0b52s1
dwc_ddrphy_apb_wr(0x900c7,0x199); // DWC_DDRPHYA_INITENG0_SequenceReg0b52s2
dwc_ddrphy_apb_wr(0x900c8,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b53s0
dwc_ddrphy_apb_wr(0x900c9,0x8568); // DWC_DDRPHYA_INITENG0_SequenceReg0b53s1
dwc_ddrphy_apb_wr(0x900ca,0x108); // DWC_DDRPHYA_INITENG0_SequenceReg0b53s2
dwc_ddrphy_apb_wr(0x900cb,0x18); // DWC_DDRPHYA_INITENG0_SequenceReg0b54s0
dwc_ddrphy_apb_wr(0x900cc,0x790); // DWC_DDRPHYA_INITENG0_SequenceReg0b54s1
dwc_ddrphy_apb_wr(0x900cd,0x16a); // DWC_DDRPHYA_INITENG0_SequenceReg0b54s2
dwc_ddrphy_apb_wr(0x900ce,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b55s0
dwc_ddrphy_apb_wr(0x900cf,0x1d8); // DWC_DDRPHYA_INITENG0_SequenceReg0b55s1
dwc_ddrphy_apb_wr(0x900d0,0x169); // DWC_DDRPHYA_INITENG0_SequenceReg0b55s2
dwc_ddrphy_apb_wr(0x900d1,0x10); // DWC_DDRPHYA_INITENG0_SequenceReg0b56s0
dwc_ddrphy_apb_wr(0x900d2,0x8558); // DWC_DDRPHYA_INITENG0_SequenceReg0b56s1
dwc_ddrphy_apb_wr(0x900d3,0x168); // DWC_DDRPHYA_INITENG0_SequenceReg0b56s2
dwc_ddrphy_apb_wr(0x900d4,0x70); // DWC_DDRPHYA_INITENG0_SequenceReg0b57s0
dwc_ddrphy_apb_wr(0x900d5,0x788); // DWC_DDRPHYA_INITENG0_SequenceReg0b57s1
dwc_ddrphy_apb_wr(0x900d6,0x16a); // DWC_DDRPHYA_INITENG0_SequenceReg0b57s2
dwc_ddrphy_apb_wr(0x900d7,0x1ff8); // DWC_DDRPHYA_INITENG0_SequenceReg0b58s0
dwc_ddrphy_apb_wr(0x900d8,0x85a8); // DWC_DDRPHYA_INITENG0_SequenceReg0b58s1
dwc_ddrphy_apb_wr(0x900d9,0x1e8); // DWC_DDRPHYA_INITENG0_SequenceReg0b58s2
dwc_ddrphy_apb_wr(0x900da,0x50); // DWC_DDRPHYA_INITENG0_SequenceReg0b59s0
dwc_ddrphy_apb_wr(0x900db,0x798); // DWC_DDRPHYA_INITENG0_SequenceReg0b59s1
dwc_ddrphy_apb_wr(0x900dc,0x16a); // DWC_DDRPHYA_INITENG0_SequenceReg0b59s2
dwc_ddrphy_apb_wr(0x900dd,0x60); // DWC_DDRPHYA_INITENG0_SequenceReg0b60s0
dwc_ddrphy_apb_wr(0x900de,0x7a0); // DWC_DDRPHYA_INITENG0_SequenceReg0b60s1
dwc_ddrphy_apb_wr(0x900df,0x16a); // DWC_DDRPHYA_INITENG0_SequenceReg0b60s2
dwc_ddrphy_apb_wr(0x900e0,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b61s0
dwc_ddrphy_apb_wr(0x900e1,0x8310); // DWC_DDRPHYA_INITENG0_SequenceReg0b61s1
dwc_ddrphy_apb_wr(0x900e2,0x168); // DWC_DDRPHYA_INITENG0_SequenceReg0b61s2
dwc_ddrphy_apb_wr(0x900e3,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b62s0
dwc_ddrphy_apb_wr(0x900e4,0xa310); // DWC_DDRPHYA_INITENG0_SequenceReg0b62s1
dwc_ddrphy_apb_wr(0x900e5,0x168); // DWC_DDRPHYA_INITENG0_SequenceReg0b62s2
dwc_ddrphy_apb_wr(0x900e6,0xa); // DWC_DDRPHYA_INITENG0_SequenceReg0b63s0
dwc_ddrphy_apb_wr(0x900e7,0x408); // DWC_DDRPHYA_INITENG0_SequenceReg0b63s1
dwc_ddrphy_apb_wr(0x900e8,0x169); // DWC_DDRPHYA_INITENG0_SequenceReg0b63s2
dwc_ddrphy_apb_wr(0x900e9,0x6e); // DWC_DDRPHYA_INITENG0_SequenceReg0b64s0
dwc_ddrphy_apb_wr(0x900ea,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b64s1
dwc_ddrphy_apb_wr(0x900eb,0x68); // DWC_DDRPHYA_INITENG0_SequenceReg0b64s2
dwc_ddrphy_apb_wr(0x900ec,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b65s0
dwc_ddrphy_apb_wr(0x900ed,0x408); // DWC_DDRPHYA_INITENG0_SequenceReg0b65s1
dwc_ddrphy_apb_wr(0x900ee,0x169); // DWC_DDRPHYA_INITENG0_SequenceReg0b65s2
dwc_ddrphy_apb_wr(0x900ef,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b66s0
dwc_ddrphy_apb_wr(0x900f0,0x8310); // DWC_DDRPHYA_INITENG0_SequenceReg0b66s1
dwc_ddrphy_apb_wr(0x900f1,0x168); // DWC_DDRPHYA_INITENG0_SequenceReg0b66s2
dwc_ddrphy_apb_wr(0x900f2,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b67s0
dwc_ddrphy_apb_wr(0x900f3,0xa310); // DWC_DDRPHYA_INITENG0_SequenceReg0b67s1
dwc_ddrphy_apb_wr(0x900f4,0x168); // DWC_DDRPHYA_INITENG0_SequenceReg0b67s2
dwc_ddrphy_apb_wr(0x900f5,0x1ff8); // DWC_DDRPHYA_INITENG0_SequenceReg0b68s0
dwc_ddrphy_apb_wr(0x900f6,0x85a8); // DWC_DDRPHYA_INITENG0_SequenceReg0b68s1
dwc_ddrphy_apb_wr(0x900f7,0x1e8); // DWC_DDRPHYA_INITENG0_SequenceReg0b68s2
dwc_ddrphy_apb_wr(0x900f8,0x68); // DWC_DDRPHYA_INITENG0_SequenceReg0b69s0
dwc_ddrphy_apb_wr(0x900f9,0x798); // DWC_DDRPHYA_INITENG0_SequenceReg0b69s1
dwc_ddrphy_apb_wr(0x900fa,0x16a); // DWC_DDRPHYA_INITENG0_SequenceReg0b69s2
dwc_ddrphy_apb_wr(0x900fb,0x78); // DWC_DDRPHYA_INITENG0_SequenceReg0b70s0
dwc_ddrphy_apb_wr(0x900fc,0x7a0); // DWC_DDRPHYA_INITENG0_SequenceReg0b70s1
dwc_ddrphy_apb_wr(0x900fd,0x16a); // DWC_DDRPHYA_INITENG0_SequenceReg0b70s2
dwc_ddrphy_apb_wr(0x900fe,0x68); // DWC_DDRPHYA_INITENG0_SequenceReg0b71s0
dwc_ddrphy_apb_wr(0x900ff,0x790); // DWC_DDRPHYA_INITENG0_SequenceReg0b71s1
dwc_ddrphy_apb_wr(0x90100,0x16a); // DWC_DDRPHYA_INITENG0_SequenceReg0b71s2
dwc_ddrphy_apb_wr(0x90101,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b72s0
dwc_ddrphy_apb_wr(0x90102,0x8b10); // DWC_DDRPHYA_INITENG0_SequenceReg0b72s1
dwc_ddrphy_apb_wr(0x90103,0x168); // DWC_DDRPHYA_INITENG0_SequenceReg0b72s2
dwc_ddrphy_apb_wr(0x90104,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b73s0
dwc_ddrphy_apb_wr(0x90105,0xab10); // DWC_DDRPHYA_INITENG0_SequenceReg0b73s1
dwc_ddrphy_apb_wr(0x90106,0x168); // DWC_DDRPHYA_INITENG0_SequenceReg0b73s2
dwc_ddrphy_apb_wr(0x90107,0xa); // DWC_DDRPHYA_INITENG0_SequenceReg0b74s0
dwc_ddrphy_apb_wr(0x90108,0x408); // DWC_DDRPHYA_INITENG0_SequenceReg0b74s1
dwc_ddrphy_apb_wr(0x90109,0x169); // DWC_DDRPHYA_INITENG0_SequenceReg0b74s2
dwc_ddrphy_apb_wr(0x9010a,0x58); // DWC_DDRPHYA_INITENG0_SequenceReg0b75s0
dwc_ddrphy_apb_wr(0x9010b,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b75s1
dwc_ddrphy_apb_wr(0x9010c,0x68); // DWC_DDRPHYA_INITENG0_SequenceReg0b75s2
dwc_ddrphy_apb_wr(0x9010d,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b76s0
dwc_ddrphy_apb_wr(0x9010e,0x408); // DWC_DDRPHYA_INITENG0_SequenceReg0b76s1
dwc_ddrphy_apb_wr(0x9010f,0x169); // DWC_DDRPHYA_INITENG0_SequenceReg0b76s2
dwc_ddrphy_apb_wr(0x90110,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b77s0
dwc_ddrphy_apb_wr(0x90111,0x8b10); // DWC_DDRPHYA_INITENG0_SequenceReg0b77s1
dwc_ddrphy_apb_wr(0x90112,0x168); // DWC_DDRPHYA_INITENG0_SequenceReg0b77s2
dwc_ddrphy_apb_wr(0x90113,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b78s0
dwc_ddrphy_apb_wr(0x90114,0xab10); // DWC_DDRPHYA_INITENG0_SequenceReg0b78s1
dwc_ddrphy_apb_wr(0x90115,0x168); // DWC_DDRPHYA_INITENG0_SequenceReg0b78s2
dwc_ddrphy_apb_wr(0x90116,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b79s0
dwc_ddrphy_apb_wr(0x90117,0x1d8); // DWC_DDRPHYA_INITENG0_SequenceReg0b79s1
dwc_ddrphy_apb_wr(0x90118,0x169); // DWC_DDRPHYA_INITENG0_SequenceReg0b79s2
dwc_ddrphy_apb_wr(0x90119,0x80); // DWC_DDRPHYA_INITENG0_SequenceReg0b80s0
dwc_ddrphy_apb_wr(0x9011a,0x790); // DWC_DDRPHYA_INITENG0_SequenceReg0b80s1
dwc_ddrphy_apb_wr(0x9011b,0x16a); // DWC_DDRPHYA_INITENG0_SequenceReg0b80s2
dwc_ddrphy_apb_wr(0x9011c,0x18); // DWC_DDRPHYA_INITENG0_SequenceReg0b81s0
dwc_ddrphy_apb_wr(0x9011d,0x7aa); // DWC_DDRPHYA_INITENG0_SequenceReg0b81s1
dwc_ddrphy_apb_wr(0x9011e,0x6a); // DWC_DDRPHYA_INITENG0_SequenceReg0b81s2
dwc_ddrphy_apb_wr(0x9011f,0xa); // DWC_DDRPHYA_INITENG0_SequenceReg0b82s0
dwc_ddrphy_apb_wr(0x90120,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b82s1
dwc_ddrphy_apb_wr(0x90121,0x1e9); // DWC_DDRPHYA_INITENG0_SequenceReg0b82s2
dwc_ddrphy_apb_wr(0x90122,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b83s0
dwc_ddrphy_apb_wr(0x90123,0x8080); // DWC_DDRPHYA_INITENG0_SequenceReg0b83s1
dwc_ddrphy_apb_wr(0x90124,0x108); // DWC_DDRPHYA_INITENG0_SequenceReg0b83s2
dwc_ddrphy_apb_wr(0x90125,0xf); // DWC_DDRPHYA_INITENG0_SequenceReg0b84s0
dwc_ddrphy_apb_wr(0x90126,0x408); // DWC_DDRPHYA_INITENG0_SequenceReg0b84s1
dwc_ddrphy_apb_wr(0x90127,0x169); // DWC_DDRPHYA_INITENG0_SequenceReg0b84s2
dwc_ddrphy_apb_wr(0x90128,0xc); // DWC_DDRPHYA_INITENG0_SequenceReg0b85s0
dwc_ddrphy_apb_wr(0x90129,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b85s1
dwc_ddrphy_apb_wr(0x9012a,0x68); // DWC_DDRPHYA_INITENG0_SequenceReg0b85s2
dwc_ddrphy_apb_wr(0x9012b,0x9); // DWC_DDRPHYA_INITENG0_SequenceReg0b86s0
dwc_ddrphy_apb_wr(0x9012c,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b86s1
dwc_ddrphy_apb_wr(0x9012d,0x1a9); // DWC_DDRPHYA_INITENG0_SequenceReg0b86s2
dwc_ddrphy_apb_wr(0x9012e,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b87s0
dwc_ddrphy_apb_wr(0x9012f,0x408); // DWC_DDRPHYA_INITENG0_SequenceReg0b87s1
dwc_ddrphy_apb_wr(0x90130,0x169); // DWC_DDRPHYA_INITENG0_SequenceReg0b87s2
dwc_ddrphy_apb_wr(0x90131,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b88s0
dwc_ddrphy_apb_wr(0x90132,0x8080); // DWC_DDRPHYA_INITENG0_SequenceReg0b88s1
dwc_ddrphy_apb_wr(0x90133,0x108); // DWC_DDRPHYA_INITENG0_SequenceReg0b88s2
dwc_ddrphy_apb_wr(0x90134,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b89s0
dwc_ddrphy_apb_wr(0x90135,0x7aa); // DWC_DDRPHYA_INITENG0_SequenceReg0b89s1
dwc_ddrphy_apb_wr(0x90136,0x6a); // DWC_DDRPHYA_INITENG0_SequenceReg0b89s2
dwc_ddrphy_apb_wr(0x90137,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b90s0
dwc_ddrphy_apb_wr(0x90138,0x8568); // DWC_DDRPHYA_INITENG0_SequenceReg0b90s1
dwc_ddrphy_apb_wr(0x90139,0x108); // DWC_DDRPHYA_INITENG0_SequenceReg0b90s2
dwc_ddrphy_apb_wr(0x9013a,0xb7); // DWC_DDRPHYA_INITENG0_SequenceReg0b91s0
dwc_ddrphy_apb_wr(0x9013b,0x790); // DWC_DDRPHYA_INITENG0_SequenceReg0b91s1
dwc_ddrphy_apb_wr(0x9013c,0x16a); // DWC_DDRPHYA_INITENG0_SequenceReg0b91s2
dwc_ddrphy_apb_wr(0x9013d,0x1f); // DWC_DDRPHYA_INITENG0_SequenceReg0b92s0
dwc_ddrphy_apb_wr(0x9013e,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b92s1
dwc_ddrphy_apb_wr(0x9013f,0x68); // DWC_DDRPHYA_INITENG0_SequenceReg0b92s2
dwc_ddrphy_apb_wr(0x90140,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b93s0
dwc_ddrphy_apb_wr(0x90141,0x8558); // DWC_DDRPHYA_INITENG0_SequenceReg0b93s1
dwc_ddrphy_apb_wr(0x90142,0x168); // DWC_DDRPHYA_INITENG0_SequenceReg0b93s2
dwc_ddrphy_apb_wr(0x90143,0xf); // DWC_DDRPHYA_INITENG0_SequenceReg0b94s0
dwc_ddrphy_apb_wr(0x90144,0x408); // DWC_DDRPHYA_INITENG0_SequenceReg0b94s1
dwc_ddrphy_apb_wr(0x90145,0x169); // DWC_DDRPHYA_INITENG0_SequenceReg0b94s2
dwc_ddrphy_apb_wr(0x90146,0xc); // DWC_DDRPHYA_INITENG0_SequenceReg0b95s0
dwc_ddrphy_apb_wr(0x90147,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b95s1
dwc_ddrphy_apb_wr(0x90148,0x68); // DWC_DDRPHYA_INITENG0_SequenceReg0b95s2
dwc_ddrphy_apb_wr(0x90149,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b96s0
dwc_ddrphy_apb_wr(0x9014a,0x408); // DWC_DDRPHYA_INITENG0_SequenceReg0b96s1
dwc_ddrphy_apb_wr(0x9014b,0x169); // DWC_DDRPHYA_INITENG0_SequenceReg0b96s2
dwc_ddrphy_apb_wr(0x9014c,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b97s0
dwc_ddrphy_apb_wr(0x9014d,0x8558); // DWC_DDRPHYA_INITENG0_SequenceReg0b97s1
dwc_ddrphy_apb_wr(0x9014e,0x168); // DWC_DDRPHYA_INITENG0_SequenceReg0b97s2
dwc_ddrphy_apb_wr(0x9014f,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b98s0
dwc_ddrphy_apb_wr(0x90150,0x3c8); // DWC_DDRPHYA_INITENG0_SequenceReg0b98s1
dwc_ddrphy_apb_wr(0x90151,0x1a9); // DWC_DDRPHYA_INITENG0_SequenceReg0b98s2
dwc_ddrphy_apb_wr(0x90152,0x3); // DWC_DDRPHYA_INITENG0_SequenceReg0b99s0
dwc_ddrphy_apb_wr(0x90153,0x370); // DWC_DDRPHYA_INITENG0_SequenceReg0b99s1
dwc_ddrphy_apb_wr(0x90154,0x129); // DWC_DDRPHYA_INITENG0_SequenceReg0b99s2
dwc_ddrphy_apb_wr(0x90155,0x20); // DWC_DDRPHYA_INITENG0_SequenceReg0b100s0
dwc_ddrphy_apb_wr(0x90156,0x2aa); // DWC_DDRPHYA_INITENG0_SequenceReg0b100s1
dwc_ddrphy_apb_wr(0x90157,0x9); // DWC_DDRPHYA_INITENG0_SequenceReg0b100s2
dwc_ddrphy_apb_wr(0x90158,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b101s0
dwc_ddrphy_apb_wr(0x90159,0x400); // DWC_DDRPHYA_INITENG0_SequenceReg0b101s1
dwc_ddrphy_apb_wr(0x9015a,0x10e); // DWC_DDRPHYA_INITENG0_SequenceReg0b101s2
dwc_ddrphy_apb_wr(0x9015b,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b102s0
dwc_ddrphy_apb_wr(0x9015c,0xe8); // DWC_DDRPHYA_INITENG0_SequenceReg0b102s1
dwc_ddrphy_apb_wr(0x9015d,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b102s2
dwc_ddrphy_apb_wr(0x9015e,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b103s0
dwc_ddrphy_apb_wr(0x9015f,0x8140); // DWC_DDRPHYA_INITENG0_SequenceReg0b103s1
dwc_ddrphy_apb_wr(0x90160,0x10c); // DWC_DDRPHYA_INITENG0_SequenceReg0b103s2
dwc_ddrphy_apb_wr(0x90161,0x10); // DWC_DDRPHYA_INITENG0_SequenceReg0b104s0
dwc_ddrphy_apb_wr(0x90162,0x8138); // DWC_DDRPHYA_INITENG0_SequenceReg0b104s1
dwc_ddrphy_apb_wr(0x90163,0x10c); // DWC_DDRPHYA_INITENG0_SequenceReg0b104s2
dwc_ddrphy_apb_wr(0x90164,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b105s0
dwc_ddrphy_apb_wr(0x90165,0x7c8); // DWC_DDRPHYA_INITENG0_SequenceReg0b105s1
dwc_ddrphy_apb_wr(0x90166,0x101); // DWC_DDRPHYA_INITENG0_SequenceReg0b105s2
dwc_ddrphy_apb_wr(0x90167,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b106s0
dwc_ddrphy_apb_wr(0x90168,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b106s1
dwc_ddrphy_apb_wr(0x90169,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b106s2
dwc_ddrphy_apb_wr(0x9016a,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b107s0
dwc_ddrphy_apb_wr(0x9016b,0x448); // DWC_DDRPHYA_INITENG0_SequenceReg0b107s1
dwc_ddrphy_apb_wr(0x9016c,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b107s2
dwc_ddrphy_apb_wr(0x9016d,0xf); // DWC_DDRPHYA_INITENG0_SequenceReg0b108s0
dwc_ddrphy_apb_wr(0x9016e,0x7c0); // DWC_DDRPHYA_INITENG0_SequenceReg0b108s1
dwc_ddrphy_apb_wr(0x9016f,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b108s2
dwc_ddrphy_apb_wr(0x90170,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b109s0
dwc_ddrphy_apb_wr(0x90171,0xe8); // DWC_DDRPHYA_INITENG0_SequenceReg0b109s1
dwc_ddrphy_apb_wr(0x90172,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b109s2
dwc_ddrphy_apb_wr(0x90173,0x47); // DWC_DDRPHYA_INITENG0_SequenceReg0b110s0
dwc_ddrphy_apb_wr(0x90174,0x630); // DWC_DDRPHYA_INITENG0_SequenceReg0b110s1
dwc_ddrphy_apb_wr(0x90175,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b110s2
dwc_ddrphy_apb_wr(0x90176,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b111s0
dwc_ddrphy_apb_wr(0x90177,0x618); // DWC_DDRPHYA_INITENG0_SequenceReg0b111s1
dwc_ddrphy_apb_wr(0x90178,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b111s2
dwc_ddrphy_apb_wr(0x90179,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b112s0
dwc_ddrphy_apb_wr(0x9017a,0xe0); // DWC_DDRPHYA_INITENG0_SequenceReg0b112s1
dwc_ddrphy_apb_wr(0x9017b,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b112s2
dwc_ddrphy_apb_wr(0x9017c,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b113s0
dwc_ddrphy_apb_wr(0x9017d,0x7c8); // DWC_DDRPHYA_INITENG0_SequenceReg0b113s1
dwc_ddrphy_apb_wr(0x9017e,0x109); // DWC_DDRPHYA_INITENG0_SequenceReg0b113s2
dwc_ddrphy_apb_wr(0x9017f,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b114s0
dwc_ddrphy_apb_wr(0x90180,0x8140); // DWC_DDRPHYA_INITENG0_SequenceReg0b114s1
dwc_ddrphy_apb_wr(0x90181,0x10c); // DWC_DDRPHYA_INITENG0_SequenceReg0b114s2
dwc_ddrphy_apb_wr(0x90182,0x0); // DWC_DDRPHYA_INITENG0_SequenceReg0b115s0
dwc_ddrphy_apb_wr(0x90183,0x1); // DWC_DDRPHYA_INITENG0_SequenceReg0b115s1
dwc_ddrphy_apb_wr(0x90184,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b115s2
dwc_ddrphy_apb_wr(0x90185,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b116s0
dwc_ddrphy_apb_wr(0x90186,0x4); // DWC_DDRPHYA_INITENG0_SequenceReg0b116s1
dwc_ddrphy_apb_wr(0x90187,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b116s2
dwc_ddrphy_apb_wr(0x90188,0x8); // DWC_DDRPHYA_INITENG0_SequenceReg0b117s0
dwc_ddrphy_apb_wr(0x90189,0x7c8); // DWC_DDRPHYA_INITENG0_SequenceReg0b117s1
dwc_ddrphy_apb_wr(0x9018a,0x101); // DWC_DDRPHYA_INITENG0_SequenceReg0b117s2
dwc_ddrphy_apb_wr(0x90006,0x0); // DWC_DDRPHYA_INITENG0_PostSequenceReg0b0s0
dwc_ddrphy_apb_wr(0x90007,0x0); // DWC_DDRPHYA_INITENG0_PostSequenceReg0b0s1
dwc_ddrphy_apb_wr(0x90008,0x8); // DWC_DDRPHYA_INITENG0_PostSequenceReg0b0s2
dwc_ddrphy_apb_wr(0x90009,0x0); // DWC_DDRPHYA_INITENG0_PostSequenceReg0b1s0
dwc_ddrphy_apb_wr(0x9000a,0x0); // DWC_DDRPHYA_INITENG0_PostSequenceReg0b1s1
dwc_ddrphy_apb_wr(0x9000b,0x0); // DWC_DDRPHYA_INITENG0_PostSequenceReg0b1s2
dwc_ddrphy_apb_wr(0xd00e7,0x400); // DWC_DDRPHYA_APBONLY0_SequencerOverride
dwc_ddrphy_apb_wr(0x90017,0x0); // DWC_DDRPHYA_INITENG0_StartVector0b0
dwc_ddrphy_apb_wr(0x9001f,0x29); // DWC_DDRPHYA_INITENG0_StartVector0b8
dwc_ddrphy_apb_wr(0x90026,0x6a); // DWC_DDRPHYA_INITENG0_StartVector0b15
dwc_ddrphy_apb_wr(0x400d0,0x0); // DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl0
dwc_ddrphy_apb_wr(0x400d1,0x101); // DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl1
dwc_ddrphy_apb_wr(0x400d2,0x105); // DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl2
dwc_ddrphy_apb_wr(0x400d3,0x107); // DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl3
dwc_ddrphy_apb_wr(0x400d4,0x10f); // DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl4
dwc_ddrphy_apb_wr(0x400d5,0x202); // DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl5
dwc_ddrphy_apb_wr(0x400d6,0x20a); // DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl6
dwc_ddrphy_apb_wr(0x400d7,0x20b); // DWC_DDRPHYA_ACSM0_AcsmCsMapCtrl7
dwc_ddrphy_apb_wr(0x2003a,0x2); // DWC_DDRPHYA_MASTER0_DbyteDllModeCntrl

 switch (freq) {
 case 334:
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY0 to 0x4b
   dwc_ddrphy_apb_wr(0x2000b,0x14); // DWC_DDRPHYA_MASTER0_Seq0BDLY0_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY1 to 0x96
   dwc_ddrphy_apb_wr(0x2000c,0x29); // DWC_DDRPHYA_MASTER0_Seq0BDLY1_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY2 to 0x5dc
   dwc_ddrphy_apb_wr(0x2000d,0x1a1); // DWC_DDRPHYA_MASTER0_Seq0BDLY2_p0
   break;
 case 350:
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY0 to 0x4b
   dwc_ddrphy_apb_wr(0x2000b,0x15); // DWC_DDRPHYA_MASTER0_Seq0BDLY0_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY1 to 0x96
   dwc_ddrphy_apb_wr(0x2000c,0x2b); // DWC_DDRPHYA_MASTER0_Seq0BDLY1_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY2 to 0x5dc
   dwc_ddrphy_apb_wr(0x2000d,0x1b5); // DWC_DDRPHYA_MASTER0_Seq0BDLY2_p0
   break;
 case 533:
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY0 to 0x4b
   dwc_ddrphy_apb_wr(0x2000b,0x21); // DWC_DDRPHYA_MASTER0_Seq0BDLY0_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY1 to 0x96
   dwc_ddrphy_apb_wr(0x2000c,0x42); // DWC_DDRPHYA_MASTER0_Seq0BDLY1_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY2 to 0x5dc
   dwc_ddrphy_apb_wr(0x2000d,0x29a); // DWC_DDRPHYA_MASTER0_Seq0BDLY2_p0
   break;
 case 800:
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY0 to 0x4b
   dwc_ddrphy_apb_wr(0x2000b,0x32); // DWC_DDRPHYA_MASTER0_Seq0BDLY0_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY1 to 0x96
   dwc_ddrphy_apb_wr(0x2000c,0x64); // DWC_DDRPHYA_MASTER0_Seq0BDLY1_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY2 to 0x5dc
   dwc_ddrphy_apb_wr(0x2000d,0x3e8); // DWC_DDRPHYA_MASTER0_Seq0BDLY2_p0
   break;
 case 1000:
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY0 to 0x4b
   dwc_ddrphy_apb_wr(0x2000b,0x3e); // DWC_DDRPHYA_MASTER0_Seq0BDLY0_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY1 to 0x96
   dwc_ddrphy_apb_wr(0x2000c,0x7d); // DWC_DDRPHYA_MASTER0_Seq0BDLY1_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY2 to 0x5dc
   dwc_ddrphy_apb_wr(0x2000d,0x4e2); // DWC_DDRPHYA_MASTER0_Seq0BDLY2_p0
   break;
 case 1067:
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY0 to 0x4b
   dwc_ddrphy_apb_wr(0x2000b,0x42); // DWC_DDRPHYA_MASTER0_Seq0BDLY0_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY1 to 0x96
   dwc_ddrphy_apb_wr(0x2000c,0x85); // DWC_DDRPHYA_MASTER0_Seq0BDLY1_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY2 to 0x5dc
   dwc_ddrphy_apb_wr(0x2000d,0x535); // DWC_DDRPHYA_MASTER0_Seq0BDLY2_p0
   break;
 case 1200:
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY0 to 0x4b
   dwc_ddrphy_apb_wr(0x2000b,0x4b); // DWC_DDRPHYA_MASTER0_Seq0BDLY0_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY1 to 0x96
   dwc_ddrphy_apb_wr(0x2000c,0x96); // DWC_DDRPHYA_MASTER0_Seq0BDLY1_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY2 to 0x5dc
   dwc_ddrphy_apb_wr(0x2000d,0x5dc); // DWC_DDRPHYA_MASTER0_Seq0BDLY2_p0
   break;
 case 1334:
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY0 to 0x4b
   dwc_ddrphy_apb_wr(0x2000b,0x53); // DWC_DDRPHYA_MASTER0_Seq0BDLY0_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY1 to 0x96
   dwc_ddrphy_apb_wr(0x2000c,0xa6); // DWC_DDRPHYA_MASTER0_Seq0BDLY1_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY2 to 0x5dc
   dwc_ddrphy_apb_wr(0x2000d,0x683); // DWC_DDRPHYA_MASTER0_Seq0BDLY2_p0
   break;
 case 1400:
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY0 to 0x4b
   dwc_ddrphy_apb_wr(0x2000b,0x57); // DWC_DDRPHYA_MASTER0_Seq0BDLY0_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY1 to 0x96
   dwc_ddrphy_apb_wr(0x2000c,0xaf); // DWC_DDRPHYA_MASTER0_Seq0BDLY1_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY2 to 0x5dc
   dwc_ddrphy_apb_wr(0x2000d,0x6d6); // DWC_DDRPHYA_MASTER0_Seq0BDLY2_p0
   break;
 default: // 1600
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY0 to 0x4b
   dwc_ddrphy_apb_wr(0x2000b,0x64); // DWC_DDRPHYA_MASTER0_Seq0BDLY0_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY1 to 0x96
   dwc_ddrphy_apb_wr(0x2000c,0xc8); // DWC_DDRPHYA_MASTER0_Seq0BDLY1_p0
   // // [phyinit_I_loadPIEImage] Pstate=0,  Memclk=1200MHz, Programming Seq0BDLY2 to 0x5dc
   dwc_ddrphy_apb_wr(0x2000d,0x7d0); // DWC_DDRPHYA_MASTER0_Seq0BDLY2_p0
 }

 switch (freq) {
 case 334:
   dwc_ddrphy_apb_wr(0x2000e,0x10); // DWC_DDRPHYA_MASTER0_Seq0BDLY3_p0
   break;
 case 350:
   dwc_ddrphy_apb_wr(0x2000e,0x10); // DWC_DDRPHYA_MASTER0_Seq0BDLY3_p0
   break;
 case 533:
   dwc_ddrphy_apb_wr(0x2000e,0x21); // DWC_DDRPHYA_MASTER0_Seq0BDLY3_p0
   break;
 default: // all others
   dwc_ddrphy_apb_wr(0x2000e,0x2c); // DWC_DDRPHYA_MASTER0_Seq0BDLY3_p0
 }

dwc_ddrphy_apb_wr(0x9000c,0x0); // DWC_DDRPHYA_INITENG0_Seq0BDisableFlag0
dwc_ddrphy_apb_wr(0x9000d,0x173); // DWC_DDRPHYA_INITENG0_Seq0BDisableFlag1
dwc_ddrphy_apb_wr(0x9000e,0x60); // DWC_DDRPHYA_INITENG0_Seq0BDisableFlag2
dwc_ddrphy_apb_wr(0x9000f,0x6110); // DWC_DDRPHYA_INITENG0_Seq0BDisableFlag3
dwc_ddrphy_apb_wr(0x90010,0x2152); // DWC_DDRPHYA_INITENG0_Seq0BDisableFlag4
dwc_ddrphy_apb_wr(0x90011,0xdfbd); // DWC_DDRPHYA_INITENG0_Seq0BDisableFlag5
dwc_ddrphy_apb_wr(0x90012,0x2060); // DWC_DDRPHYA_INITENG0_Seq0BDisableFlag6
dwc_ddrphy_apb_wr(0x90013,0x6152); // DWC_DDRPHYA_INITENG0_Seq0BDisableFlag7
// // [phyinit_I_loadPIEImage] Enabling Phy Master Interface for DRAM drift compensation

 if ((freq !=533) &&  (freq !=334) && (freq !=350)) { // all but not 533/334/350
   // // [phyinit_I_loadPIEImage] Pstate=0, Memclk=1200MHz, Programming PPTTrainSetup::PhyMstrTrainInterval to 0xa
   // // [phyinit_I_loadPIEImage] Pstate=0, Memclk=1200MHz, Programming PPTTrainSetup::PhyMstrMaxReqToAck to 0x5
   // // [phyinit_I_loadPIEImage] Pstate=0, Memclk=1200MHz, Programming PPTTrainSetup2::PhyMstrFreqOverride to 0x3
   dwc_ddrphy_apb_wr(0x20010,0x5a); // DWC_DDRPHYA_MASTER0_PPTTrainSetup_p0
   dwc_ddrphy_apb_wr(0x20011,0x3); // DWC_DDRPHYA_MASTER0_PPTTrainSetup2_p0
 }
// // [phyinit_I_loadPIEImage] Pstate=0, Programming AcsmPlayback0x0 to 0xe0
dwc_ddrphy_apb_wr(0x40080,0xe0); // DWC_DDRPHYA_ACSM0_AcsmPlayback0x0_p0
// // [phyinit_I_loadPIEImage] Pstate=0, Programming AcsmPlayback1x0 to 0x12
dwc_ddrphy_apb_wr(0x40081,0x12); // DWC_DDRPHYA_ACSM0_AcsmPlayback1x0_p0
// // [phyinit_I_loadPIEImage] Pstate=0, Programming AcsmPlayback0x1 to 0xe0
dwc_ddrphy_apb_wr(0x40082,0xe0); // DWC_DDRPHYA_ACSM0_AcsmPlayback0x1_p0
// // [phyinit_I_loadPIEImage] Pstate=0, Programming AcsmPlayback1x1 to 0x12
dwc_ddrphy_apb_wr(0x40083,0x12); // DWC_DDRPHYA_ACSM0_AcsmPlayback1x1_p0
// // [phyinit_I_loadPIEImage] Pstate=0, Programming AcsmPlayback0x2 to 0xe0
dwc_ddrphy_apb_wr(0x40084,0xe0); // DWC_DDRPHYA_ACSM0_AcsmPlayback0x2_p0
// // [phyinit_I_loadPIEImage] Pstate=0, Programming AcsmPlayback1x2 to 0x12
dwc_ddrphy_apb_wr(0x40085,0x12); // DWC_DDRPHYA_ACSM0_AcsmPlayback1x2_p0
// // [phyinit_I_loadPIEImage] Programing Training Hardware Registers for mission mode retraining
dwc_ddrphy_apb_wr(0x400fd,0xf); // DWC_DDRPHYA_ACSM0_AcsmCtrl13
dwc_ddrphy_apb_wr(0x10011,0x1); // DWC_DDRPHYA_DBYTE0_TsmByte1
dwc_ddrphy_apb_wr(0x10012,0x1); // DWC_DDRPHYA_DBYTE0_TsmByte2
dwc_ddrphy_apb_wr(0x10013,0x180); // DWC_DDRPHYA_DBYTE0_TsmByte3
dwc_ddrphy_apb_wr(0x10018,0x1); // DWC_DDRPHYA_DBYTE0_TsmByte5
dwc_ddrphy_apb_wr(0x10002,0x6209); // DWC_DDRPHYA_DBYTE0_TrainingParam
dwc_ddrphy_apb_wr(0x100b2,0x1); // DWC_DDRPHYA_DBYTE0_Tsm0_i0
dwc_ddrphy_apb_wr(0x101b4,0x1); // DWC_DDRPHYA_DBYTE0_Tsm2_i1
dwc_ddrphy_apb_wr(0x102b4,0x1); // DWC_DDRPHYA_DBYTE0_Tsm2_i2
dwc_ddrphy_apb_wr(0x103b4,0x1); // DWC_DDRPHYA_DBYTE0_Tsm2_i3
dwc_ddrphy_apb_wr(0x104b4,0x1); // DWC_DDRPHYA_DBYTE0_Tsm2_i4
dwc_ddrphy_apb_wr(0x105b4,0x1); // DWC_DDRPHYA_DBYTE0_Tsm2_i5
dwc_ddrphy_apb_wr(0x106b4,0x1); // DWC_DDRPHYA_DBYTE0_Tsm2_i6
dwc_ddrphy_apb_wr(0x107b4,0x1); // DWC_DDRPHYA_DBYTE0_Tsm2_i7
dwc_ddrphy_apb_wr(0x108b4,0x1); // DWC_DDRPHYA_DBYTE0_Tsm2_i8
dwc_ddrphy_apb_wr(0x11011,0x1); // DWC_DDRPHYA_DBYTE1_TsmByte1
dwc_ddrphy_apb_wr(0x11012,0x1); // DWC_DDRPHYA_DBYTE1_TsmByte2
dwc_ddrphy_apb_wr(0x11013,0x180); // DWC_DDRPHYA_DBYTE1_TsmByte3
dwc_ddrphy_apb_wr(0x11018,0x1); // DWC_DDRPHYA_DBYTE1_TsmByte5
dwc_ddrphy_apb_wr(0x11002,0x6209); // DWC_DDRPHYA_DBYTE1_TrainingParam
dwc_ddrphy_apb_wr(0x110b2,0x1); // DWC_DDRPHYA_DBYTE1_Tsm0_i0
dwc_ddrphy_apb_wr(0x111b4,0x1); // DWC_DDRPHYA_DBYTE1_Tsm2_i1
dwc_ddrphy_apb_wr(0x112b4,0x1); // DWC_DDRPHYA_DBYTE1_Tsm2_i2
dwc_ddrphy_apb_wr(0x113b4,0x1); // DWC_DDRPHYA_DBYTE1_Tsm2_i3
dwc_ddrphy_apb_wr(0x114b4,0x1); // DWC_DDRPHYA_DBYTE1_Tsm2_i4
dwc_ddrphy_apb_wr(0x115b4,0x1); // DWC_DDRPHYA_DBYTE1_Tsm2_i5
dwc_ddrphy_apb_wr(0x116b4,0x1); // DWC_DDRPHYA_DBYTE1_Tsm2_i6
dwc_ddrphy_apb_wr(0x117b4,0x1); // DWC_DDRPHYA_DBYTE1_Tsm2_i7
dwc_ddrphy_apb_wr(0x118b4,0x1); // DWC_DDRPHYA_DBYTE1_Tsm2_i8
dwc_ddrphy_apb_wr(0x12011,0x1); // DWC_DDRPHYA_DBYTE2_TsmByte1
dwc_ddrphy_apb_wr(0x12012,0x1); // DWC_DDRPHYA_DBYTE2_TsmByte2
dwc_ddrphy_apb_wr(0x12013,0x180); // DWC_DDRPHYA_DBYTE2_TsmByte3
dwc_ddrphy_apb_wr(0x12018,0x1); // DWC_DDRPHYA_DBYTE2_TsmByte5
dwc_ddrphy_apb_wr(0x12002,0x6209); // DWC_DDRPHYA_DBYTE2_TrainingParam
dwc_ddrphy_apb_wr(0x120b2,0x1); // DWC_DDRPHYA_DBYTE2_Tsm0_i0
dwc_ddrphy_apb_wr(0x121b4,0x1); // DWC_DDRPHYA_DBYTE2_Tsm2_i1
dwc_ddrphy_apb_wr(0x122b4,0x1); // DWC_DDRPHYA_DBYTE2_Tsm2_i2
dwc_ddrphy_apb_wr(0x123b4,0x1); // DWC_DDRPHYA_DBYTE2_Tsm2_i3
dwc_ddrphy_apb_wr(0x124b4,0x1); // DWC_DDRPHYA_DBYTE2_Tsm2_i4
dwc_ddrphy_apb_wr(0x125b4,0x1); // DWC_DDRPHYA_DBYTE2_Tsm2_i5
dwc_ddrphy_apb_wr(0x126b4,0x1); // DWC_DDRPHYA_DBYTE2_Tsm2_i6
dwc_ddrphy_apb_wr(0x127b4,0x1); // DWC_DDRPHYA_DBYTE2_Tsm2_i7
dwc_ddrphy_apb_wr(0x128b4,0x1); // DWC_DDRPHYA_DBYTE2_Tsm2_i8
dwc_ddrphy_apb_wr(0x13011,0x1); // DWC_DDRPHYA_DBYTE3_TsmByte1
dwc_ddrphy_apb_wr(0x13012,0x1); // DWC_DDRPHYA_DBYTE3_TsmByte2
dwc_ddrphy_apb_wr(0x13013,0x180); // DWC_DDRPHYA_DBYTE3_TsmByte3
dwc_ddrphy_apb_wr(0x13018,0x1); // DWC_DDRPHYA_DBYTE3_TsmByte5
dwc_ddrphy_apb_wr(0x13002,0x6209); // DWC_DDRPHYA_DBYTE3_TrainingParam
dwc_ddrphy_apb_wr(0x130b2,0x1); // DWC_DDRPHYA_DBYTE3_Tsm0_i0
dwc_ddrphy_apb_wr(0x131b4,0x1); // DWC_DDRPHYA_DBYTE3_Tsm2_i1
dwc_ddrphy_apb_wr(0x132b4,0x1); // DWC_DDRPHYA_DBYTE3_Tsm2_i2
dwc_ddrphy_apb_wr(0x133b4,0x1); // DWC_DDRPHYA_DBYTE3_Tsm2_i3
dwc_ddrphy_apb_wr(0x134b4,0x1); // DWC_DDRPHYA_DBYTE3_Tsm2_i4
dwc_ddrphy_apb_wr(0x135b4,0x1); // DWC_DDRPHYA_DBYTE3_Tsm2_i5
dwc_ddrphy_apb_wr(0x136b4,0x1); // DWC_DDRPHYA_DBYTE3_Tsm2_i6
dwc_ddrphy_apb_wr(0x137b4,0x1); // DWC_DDRPHYA_DBYTE3_Tsm2_i7
dwc_ddrphy_apb_wr(0x138b4,0x1); // DWC_DDRPHYA_DBYTE3_Tsm2_i8
// // [phyinit_I_loadPIEImage] Turn on calibration and hold idle until dfi_init_start is asserted sequence is triggered.
dwc_ddrphy_apb_wr(0x20089,0x1); // DWC_DDRPHYA_MASTER0_CalZap
// // [phyinit_I_loadPIEImage] Programming CalRate::CalInterval to 0x9
// // [phyinit_I_loadPIEImage] Programming CalRate::CalOnce to 0x0
// // [phyinit_I_loadPIEImage] Programming CalRate::CalRun to 0x1
dwc_ddrphy_apb_wr(0x20088,0x19); // DWC_DDRPHYA_MASTER0_CalRate
// // [phyinit_I_loadPIEImage] Disabling Ucclk (PMU)
dwc_ddrphy_apb_wr(0xc0080,0x2); // DWC_DDRPHYA_DRTUB0_UcclkHclkEnables
// // [phyinit_I_loadPIEImage] Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1. 
dwc_ddrphy_apb_wr(0xd0000,0x1); // DWC_DDRPHYA_APBONLY0_MicroContMuxSel
// // [phyinit_I_loadPIEImage] End of dwc_ddrphy_phyinit_I_loadPIEImage()
// 
// 
  
  GME_SPARE_0_SPARE_BITS_W(0xa08a0002); // write
  
  
}

