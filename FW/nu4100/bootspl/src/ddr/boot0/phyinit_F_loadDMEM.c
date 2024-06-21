#include "phyinit.h"

//#define DCCM_BASE_ADDR 0x0C150000

//#define DDRPHY_DMEM_SIZE	8192
//extern unsigned short ddrphy_dmem[DDRPHY_DMEM_SIZE];//

// ===========================================================================
void phyinit_F_loadDMEM (unsigned int freq) {

  unsigned char mr1, mr2;

  switch (freq) {
  case 334:
    mr1 = 0x14;
    mr2 = 0x09;
    break;
  case 350:
    mr1 = 0x14;
    mr2 = 0x09;
    break;
  case 533:
    mr1 = 0x14;
    mr2 = 0x09;
    break;
  case 800:
    mr1 = 0x24;
    mr2 = 0x12;
    break;
  case 1000:
    mr1 = 0x34;
    mr2 = 0x1b;
    break;
  case 1067:
    mr1 = 0x34;
    mr2 = 0x1b;
    break;
  case 1200:
    mr1 = 0x44;
    mr2 = 0x24;
    break;
  case 1334:
    mr1 = 0x54;
    mr2 = 0x2d;
    break;
  case 1400:
    mr1 = 0x54;
    mr2 = 0x2d;
    break;
  default: // 1600
    mr1 = 0x54;
    mr2 = 0x2d;
    break;
  }

// //##############################################################
// //
// // (F) Load the 1D DMEM image and write the 1D Message Block parameters for the training firmware 
// // 
// // See PhyInit App Note for detailed description and function usage
// // 
// //##############################################################
// 
// [dwc_ddrphy_phyinit_storeIncvFile] Reading input file: /space/users/erand/DDR_FW/2.00a/firmware/A-2018.05/lpddr4/lpddr4_pmu_train_dmem.incv

// // 1.	Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.
// //       This allows the memory controller unrestricted access to the configuration CSRs. 
dwc_ddrphy_apb_wr(0xd0000,0x0); // DWC_DDRPHYA_APBONLY0_MicroContMuxSel
// // [dwc_ddrphy_phyinit_WriteOutMem] STARTING. offset 0x54000 size 0x342
dwc_ddrphy_apb_wr_d(0x00000,0x000);                        // {MsgMisc, Reserved00}
dwc_ddrphy_apb_wr_d(0x00001,0x0);                          // PmuRevision
dwc_ddrphy_apb_wr_d(0x00002,0x0);                          // {PllBypassEn, Pstate}

 switch (freq) {
 case 334:
   dwc_ddrphy_apb_wr_d(0x00003,0x29c);                        // DRAMFreq
   break;
 case 350:
   dwc_ddrphy_apb_wr_d(0x00003,0x2bc);                        // DRAMFreq
   break;
 case 533:
   dwc_ddrphy_apb_wr_d(0x00003,0x42a);                        // DRAMFreq
   break;
 case 800:
   dwc_ddrphy_apb_wr_d(0x00003,0x640);                        // DRAMFreq
   break;
 case 1000:
   dwc_ddrphy_apb_wr_d(0x00003,0x7d0);                        // DRAMFreq
   break;
 case 1067:
   dwc_ddrphy_apb_wr_d(0x00003,0x856);                        // DRAMFreq
   break;
 case 1200:
   dwc_ddrphy_apb_wr_d(0x00003,0x960);                        // DRAMFreq
   break;
 case 1334:
   dwc_ddrphy_apb_wr_d(0x00003,0xa6c);                        // DRAMFreq
   break;
 case 1400:
   dwc_ddrphy_apb_wr_d(0x00003,0xaf0);                        // DRAMFreq
   break;
 default: // 1600
   dwc_ddrphy_apb_wr_d(0x00003,0xc80);                        // DRAMFreq
 }
#ifndef _ZEBU
dwc_ddrphy_apb_wr_d(0x00004,0x2);                          // {BPZNResVal, DfiFreqRatio}
dwc_ddrphy_apb_wr_d(0x00005,0x0);                          // {PhyDrvImpedance, PhyOdtImpedance}
#ifdef _UNSETDEFAULT
dwc_ddrphy_apb_wr_d(0x00006,0x14);                         // {Lp4Misc, PhyVref}
#else
dwc_ddrphy_apb_wr_d(0x00006,0x14);                         // {Lp4Misc, PhyVref}
#endif
dwc_ddrphy_apb_wr_d(0x00007,0x0);                          // {CsTestFail, Reserved0E}
dwc_ddrphy_apb_wr_d(0x00008,0x131f);                       // SequenceCtrl

#ifdef DDR_MSG_DETAILED_DEBUG
dwc_ddrphy_apb_wr_d(0x00009,0x00);                         // {Reserved13, HdtCtrl}
#elif defined(DDR_MSG_COARSE_DEBUG)
dwc_ddrphy_apb_wr_d(0x00009,0x0a);                         // {Reserved13, HdtCtrl}
#elif defined(DDR_MSG_ASSERTION_MESSAGES)
dwc_ddrphy_apb_wr_d(0x00009,0xc9);                         // {Reserved13, HdtCtrl}
#elif defined(DDR_MSG_FIRMWARE_COMPLETE)
dwc_ddrphy_apb_wr_d(0x00009,0xff);                         // {Reserved13, HdtCtrl}
#else
dwc_ddrphy_apb_wr_d(0x00009,0xc8);                         // {Reserved13, HdtCtrl}
#endif

dwc_ddrphy_apb_wr_d(0x0000a,0x0);                          // {Reserved15, Reserved14}
dwc_ddrphy_apb_wr_d(0x0000b,0x2);                          // {Reserved17, DFIMRLMargin}
dwc_ddrphy_apb_wr_d(0x0000c,0x0);                          // {Lp4Quickboot, UseBroadcastMR}
dwc_ddrphy_apb_wr_d(0x0000d,0x1aa);                        // {CATrainOpt, Reserved1A}
dwc_ddrphy_apb_wr_d(0x0000e,0x0);                          // {Reserved1D, X8Mode}
dwc_ddrphy_apb_wr_d(0x0000f,0x100);                        // {Share2DVrefResult, Reserved1E}
dwc_ddrphy_apb_wr_d(0x00010,0x0);                          // {Reserved21, Reserved20}
dwc_ddrphy_apb_wr_d(0x00011,0x0);                          // PhyConfigOverride
dwc_ddrphy_apb_wr_d(0x00012,0x110);                        // {CsPresentChA, EnabledDQsChA}
dwc_ddrphy_apb_wr_d(0x00013,0x0);                          // {CDD_ChA_RR_0_1, CDD_ChA_RR_1_0}
dwc_ddrphy_apb_wr_d(0x00014,0x0);                          // {CDD_ChA_RW_1_0, CDD_ChA_RW_1_1}
dwc_ddrphy_apb_wr_d(0x00015,0x0);                          // {CDD_ChA_RW_0_0, CDD_ChA_RW_0_1}
dwc_ddrphy_apb_wr_d(0x00016,0x0);                          // {CDD_ChA_WR_1_0, CDD_ChA_WR_1_1}
dwc_ddrphy_apb_wr_d(0x00017,0x0);                          // {CDD_ChA_WR_0_0, CDD_ChA_WR_0_1}
dwc_ddrphy_apb_wr_d(0x00018,0x0);                          // {CDD_ChA_WW_0_1, CDD_ChA_WW_1_0}

dwc_ddrphy_apb_wr_d(0x00019,(((short)mr2) << 8) | (0x00ff & mr1)); // {MR2_A0, MR1_A0}

#ifdef DDR_FULL_BUS_WIDTH_FREQ1400_MODE
dwc_ddrphy_apb_wr_d(0x0001a,0x33);                         // {MR4_A0, MR3_A0}
dwc_ddrphy_apb_wr_d(0x0001b,0x6665);                       // {MR12_A0, MR11_A0}
dwc_ddrphy_apb_wr_d(0x0001c,0x4d28);                       // {MR14_A0, MR13_A0}
dwc_ddrphy_apb_wr_d(0x0001d,0x0);                          // {MR17_A0, MR16_A0}
dwc_ddrphy_apb_wr_d(0x0001e,0x85);                          // {MR24_A0, MR22_A0}
#elif  DDR_FULL_BUS_WIDTH_FREQ800_MODE  // 800MHz
dwc_ddrphy_apb_wr_d(0x0001a,0x33);                         // {MR4_A0, MR3_A0}
dwc_ddrphy_apb_wr_d(0x0001b,0x6d66);                       // {MR12_A0, MR11_A0}
dwc_ddrphy_apb_wr_d(0x0001c,0x4d28);                       // {MR14_A0, MR13_A0}
dwc_ddrphy_apb_wr_d(0x0001d,0x0);                          // {MR17_A0, MR16_A0}
dwc_ddrphy_apb_wr_d(0x0001e,0x5);                          // {MR24_A0, MR22_A0}
#else
dwc_ddrphy_apb_wr_d(0x0001a,0x33);                         // {MR4_A0, MR3_A0}
dwc_ddrphy_apb_wr_d(0x0001b,0x6655);                       // {MR12_A0, MR11_A0}
dwc_ddrphy_apb_wr_d(0x0001c,0x4d2a);                       // {MR14_A0, MR13_A0}
dwc_ddrphy_apb_wr_d(0x0001d,0x0);                          // {MR17_A0, MR16_A0}
dwc_ddrphy_apb_wr_d(0x0001e,0x5);                          // {MR24_A0, MR22_A0}

#endif

dwc_ddrphy_apb_wr_d(0x0001f,(((short)mr2) << 8) | (0x00ff & mr1)); // {MR2_A0, MR1_A0}

#ifdef DDR_FULL_BUS_WIDTH_FREQ1400_MODE
dwc_ddrphy_apb_wr_d(0x00020,0x33);                         // {MR4_A1, MR3_A1}
dwc_ddrphy_apb_wr_d(0x00021,0x6665);                       // {MR12_A1, MR11_A1}
dwc_ddrphy_apb_wr_d(0x00022,0x4d28);                       // {MR14_A1, MR13_A1}
dwc_ddrphy_apb_wr_d(0x00023,0x0);                          // {MR17_A1, MR16_A1}
dwc_ddrphy_apb_wr_d(0x00024,0x85);                          // {MR24_A1, MR22_A1}
#elif DDR_FULL_BUS_WIDTH_FREQ800_MODE
dwc_ddrphy_apb_wr_d(0x00020,0x33);                         // {MR4_A1, MR3_A1}
dwc_ddrphy_apb_wr_d(0x00021,0x6d66);                       // {MR12_A1, MR11_A1}
dwc_ddrphy_apb_wr_d(0x00022,0x4d28);                       // {MR14_A1, MR13_A1}
dwc_ddrphy_apb_wr_d(0x00023,0x0);                          // {MR17_A1, MR16_A1}
dwc_ddrphy_apb_wr_d(0x00024,0x5);                          // {MR24_A1, MR22_A1}
#else
dwc_ddrphy_apb_wr_d(0x00020,0x33);                         // {MR4_A1, MR3_A1}
dwc_ddrphy_apb_wr_d(0x00021,0x6655);                       // {MR12_A1, MR11_A1}
dwc_ddrphy_apb_wr_d(0x00022,0x4d2a);                       // {MR14_A1, MR13_A1}
dwc_ddrphy_apb_wr_d(0x00023,0x0);                          // {MR17_A1, MR16_A1}
dwc_ddrphy_apb_wr_d(0x00024,0x5);                          // {MR24_A1, MR22_A1}


#endif

dwc_ddrphy_apb_wr_d(0x00025,0x0);                          // {Reserved4B, CATerminatingRankChA}
dwc_ddrphy_apb_wr_d(0x00026,0x0);                          // {Reserved4D, Reserved4C}
dwc_ddrphy_apb_wr_d(0x00027,0x0);                          // {Reserved4F, Reserved4E}
dwc_ddrphy_apb_wr_d(0x00028,0x0);                          // {Reserved51, Reserved50}
dwc_ddrphy_apb_wr_d(0x00029,0x0);                          // {Reserved53, Reserved52}
dwc_ddrphy_apb_wr_d(0x0002a,0x0);                          // {Reserved55, Reserved54}

#ifdef DDR_HALF_BUS_WIDTH_MODE
dwc_ddrphy_apb_wr_d(0x0002b,0x0);                          // {EnabledDQsChB, Reserved56}
dwc_ddrphy_apb_wr_d(0x0002c,0x0);                          // {CDD_ChB_RR_1_0, CsPresentChB}
#else
dwc_ddrphy_apb_wr_d(0x0002b,0x1000);                       // {EnabledDQsChB, Reserved56}
dwc_ddrphy_apb_wr_d(0x0002c,0x1);                          // {CDD_ChB_RR_1_0, CsPresentChB}
#endif

dwc_ddrphy_apb_wr_d(0x0002d,0x0);                          // {CDD_ChB_RW_1_1, CDD_ChB_RR_0_1}
dwc_ddrphy_apb_wr_d(0x0002e,0x0);                          // {CDD_ChB_RW_0_1, CDD_ChB_RW_1_0}
dwc_ddrphy_apb_wr_d(0x0002f,0x0);                          // {CDD_ChB_WR_1_1, CDD_ChB_RW_0_0}
dwc_ddrphy_apb_wr_d(0x00030,0x0);                          // {CDD_ChB_WR_0_1, CDD_ChB_WR_1_0}
dwc_ddrphy_apb_wr_d(0x00031,0x0);                          // {CDD_ChB_WW_1_0, CDD_ChB_WR_0_0}

dwc_ddrphy_apb_wr_d(0x00032,(((short)mr1) << 8) | (0x00ff & 0x00)); // {MR1_B0, CDD_ChB_WW_0_1}

#ifdef DDR_FULL_BUS_WIDTH_FREQ1400_MODE

dwc_ddrphy_apb_wr_d(0x00033,(((short)0x33) << 8) | (0x00ff & mr2)); // {MR3_B0, MR2_B0}
dwc_ddrphy_apb_wr_d(0x00034,0x6500);                       // {MR11_B0, MR4_B0}
dwc_ddrphy_apb_wr_d(0x00035,0x2866);                       // {MR13_B0, MR12_B0}
dwc_ddrphy_apb_wr_d(0x00036,0x4d);                         // {MR16_B0, MR14_B0}
dwc_ddrphy_apb_wr_d(0x00037,0x8500);                        // {MR22_B0, MR17_B0}
#elif DDR_FULL_BUS_WIDTH_FREQ800_MODE
dwc_ddrphy_apb_wr_d(0x00033,(((short)0x33) << 8) | (0x00ff & mr2)); // {MR3_B0, MR2_B0}
dwc_ddrphy_apb_wr_d(0x00034,0x6600);                       // {MR11_B0, MR4_B0}
dwc_ddrphy_apb_wr_d(0x00035,0x286d);                       // {MR13_B0, MR12_B0}
dwc_ddrphy_apb_wr_d(0x00036,0x4d);                         // {MR16_B0, MR14_B0}
dwc_ddrphy_apb_wr_d(0x00037,0x500);                        // {MR22_B0, MR17_B0}
#else
dwc_ddrphy_apb_wr_d(0x00033,(((short)0x33) << 8) | (0x00ff & mr2)); // {MR3_B0, MR2_B0}
dwc_ddrphy_apb_wr_d(0x00034,0x5500);                       // {MR11_B0, MR4_B0}
dwc_ddrphy_apb_wr_d(0x00035,0x2866);                       // {MR13_B0, MR12_B0}
dwc_ddrphy_apb_wr_d(0x00036,0x4d);                         // {MR16_B0, MR14_B0}
dwc_ddrphy_apb_wr_d(0x00037,0x8500);                        // {MR22_B0, MR17_B0}

#endif

dwc_ddrphy_apb_wr_d(0x00038,(((short)mr1) << 8) | (0x00ff & 0x00)); // {MR1_B0, CDD_ChB_WW_0_1}

#ifdef DDR_FULL_BUS_WIDTH_FREQ1400_MODE
dwc_ddrphy_apb_wr_d(0x00039,(((short)0x33) << 8) | (0x00ff & mr2)); // {MR3_B0, MR2_B0}
dwc_ddrphy_apb_wr_d(0x0003a,0x6500);                       // {MR11_B1, MR4_B1}
dwc_ddrphy_apb_wr_d(0x0003b,0x2866);                       // {MR13_B1, MR12_B1}
dwc_ddrphy_apb_wr_d(0x0003c,0x4d);                         // {MR16_B1, MR14_B1}
dwc_ddrphy_apb_wr_d(0x0003d,0x8500);                        // {MR22_B1, MR17_B1}
#elif DDR_FULL_BUS_WIDTH_FREQ800_MODE
dwc_ddrphy_apb_wr_d(0x00039,(((short)0x33) << 8) | (0x00ff & mr2)); // {MR3_B0, MR2_B0}
dwc_ddrphy_apb_wr_d(0x0003a,0x6600);                       // {MR11_B1, MR4_B1}
dwc_ddrphy_apb_wr_d(0x0003b,0x286d);                       // {MR13_B1, MR12_B1}
dwc_ddrphy_apb_wr_d(0x0003c,0x4d);                         // {MR16_B1, MR14_B1}
dwc_ddrphy_apb_wr_d(0x0003d,0x500);                        // {MR22_B1, MR17_B1}
#else
dwc_ddrphy_apb_wr_d(0x00039,(((short)0x33) << 8) | (0x00ff & mr2)); // {MR3_B0, MR2_B0}
dwc_ddrphy_apb_wr_d(0x0003a,0x5500);                       // {MR11_B1, MR4_B1}
dwc_ddrphy_apb_wr_d(0x0003b,0x2a66);                       // {MR13_B1, MR12_B1}
dwc_ddrphy_apb_wr_d(0x0003c,0x4d);                         // {MR16_B1, MR14_B1}
dwc_ddrphy_apb_wr_d(0x0003d,0x500);                        // {MR22_B1, MR17_B1}

#endif

dwc_ddrphy_apb_wr_d(0x0003e,0x0);                          // {CATerminatingRankChB, MR24_B1}
dwc_ddrphy_apb_wr_d(0x0003f,0x0);                          // {Reserved7F, Reserved7E}
dwc_ddrphy_apb_wr_d(0x00040,0x0);                          // {Reserved81, Reserved80}
dwc_ddrphy_apb_wr_d(0x00041,0x0);                          // {Reserved83, Reserved82}
dwc_ddrphy_apb_wr_d(0x00042,0x0);                          // {Reserved85, Reserved84}
dwc_ddrphy_apb_wr_d(0x00043,0x0);                          // {Reserved87, Reserved86}
dwc_ddrphy_apb_wr_d(0x00044,0x0);                          // {Reserved89, Reserved88}                 
dwc_ddrphy_apb_wr_d(0x00045,0x0);
dwc_ddrphy_apb_wr_d(0x00046,0x0);
dwc_ddrphy_apb_wr_d(0x00047,0x0);
dwc_ddrphy_apb_wr_d(0x00048,0x0);
dwc_ddrphy_apb_wr_d(0x00049,0x0);
dwc_ddrphy_apb_wr_d(0x0004a,0x0);
dwc_ddrphy_apb_wr_d(0x0004b,0x0);
dwc_ddrphy_apb_wr_d(0x0004c,0x0);
dwc_ddrphy_apb_wr_d(0x0004d,0x0);
dwc_ddrphy_apb_wr_d(0x0004e,0x0);
dwc_ddrphy_apb_wr_d(0x0004f,0x0);
dwc_ddrphy_apb_wr_d(0x00050,0x0);
dwc_ddrphy_apb_wr_d(0x00051,0x0);
dwc_ddrphy_apb_wr_d(0x00052,0x0);
dwc_ddrphy_apb_wr_d(0x00053,0x0);
dwc_ddrphy_apb_wr_d(0x00054,0x0);
dwc_ddrphy_apb_wr_d(0x00055,0x0);
dwc_ddrphy_apb_wr_d(0x00056,0x0);
dwc_ddrphy_apb_wr_d(0x00057,0x0);
dwc_ddrphy_apb_wr_d(0x00058,0x0);
dwc_ddrphy_apb_wr_d(0x00059,0x0);
dwc_ddrphy_apb_wr_d(0x0005a,0x0);
dwc_ddrphy_apb_wr_d(0x0005b,0x0);
dwc_ddrphy_apb_wr_d(0x0005c,0x0);
dwc_ddrphy_apb_wr_d(0x0005d,0x0);
dwc_ddrphy_apb_wr_d(0x0005e,0x0);
dwc_ddrphy_apb_wr_d(0x0005f,0x0);
dwc_ddrphy_apb_wr_d(0x00060,0x0);
dwc_ddrphy_apb_wr_d(0x00061,0x0);
dwc_ddrphy_apb_wr_d(0x00062,0x0);
dwc_ddrphy_apb_wr_d(0x00063,0x0);
dwc_ddrphy_apb_wr_d(0x00064,0x0);
dwc_ddrphy_apb_wr_d(0x00065,0x0);
dwc_ddrphy_apb_wr_d(0x00066,0x0);
dwc_ddrphy_apb_wr_d(0x00067,0x0);
dwc_ddrphy_apb_wr_d(0x00068,0x0);
dwc_ddrphy_apb_wr_d(0x00069,0x0);
dwc_ddrphy_apb_wr_d(0x0006a,0x0);
dwc_ddrphy_apb_wr_d(0x0006b,0x0);
dwc_ddrphy_apb_wr_d(0x0006c,0x0);
dwc_ddrphy_apb_wr_d(0x0006d,0x0);
dwc_ddrphy_apb_wr_d(0x0006e,0x0);
dwc_ddrphy_apb_wr_d(0x0006f,0x0);
dwc_ddrphy_apb_wr_d(0x00070,0x0);
dwc_ddrphy_apb_wr_d(0x00071,0x0);
dwc_ddrphy_apb_wr_d(0x00072,0x0);
dwc_ddrphy_apb_wr_d(0x00073,0x0);
dwc_ddrphy_apb_wr_d(0x00074,0x0);
dwc_ddrphy_apb_wr_d(0x00075,0x0);
dwc_ddrphy_apb_wr_d(0x00076,0x0);
dwc_ddrphy_apb_wr_d(0x00077,0x0);
dwc_ddrphy_apb_wr_d(0x00078,0x0);
dwc_ddrphy_apb_wr_d(0x00079,0x0);
dwc_ddrphy_apb_wr_d(0x0007a,0x0);
dwc_ddrphy_apb_wr_d(0x0007b,0x0);
dwc_ddrphy_apb_wr_d(0x0007c,0x0);
dwc_ddrphy_apb_wr_d(0x0007d,0x0);
dwc_ddrphy_apb_wr_d(0x0007e,0x0);
dwc_ddrphy_apb_wr_d(0x0007f,0x0);
dwc_ddrphy_apb_wr_d(0x00080,0x0);
dwc_ddrphy_apb_wr_d(0x00081,0x0);
dwc_ddrphy_apb_wr_d(0x00082,0x0);
dwc_ddrphy_apb_wr_d(0x00083,0x0);
dwc_ddrphy_apb_wr_d(0x00084,0x0);
dwc_ddrphy_apb_wr_d(0x00085,0x0);
dwc_ddrphy_apb_wr_d(0x00086,0x0);
dwc_ddrphy_apb_wr_d(0x00087,0x0);
dwc_ddrphy_apb_wr_d(0x00088,0x0);
dwc_ddrphy_apb_wr_d(0x00089,0x0);
dwc_ddrphy_apb_wr_d(0x0008a,0x0);
dwc_ddrphy_apb_wr_d(0x0008b,0x0);
dwc_ddrphy_apb_wr_d(0x0008c,0x0);
dwc_ddrphy_apb_wr_d(0x0008d,0x0);
dwc_ddrphy_apb_wr_d(0x0008e,0x0);
dwc_ddrphy_apb_wr_d(0x0008f,0x0);
dwc_ddrphy_apb_wr_d(0x00090,0x0);
dwc_ddrphy_apb_wr_d(0x00091,0x0);
dwc_ddrphy_apb_wr_d(0x00092,0x0);
dwc_ddrphy_apb_wr_d(0x00093,0x0);
dwc_ddrphy_apb_wr_d(0x00094,0x0);
dwc_ddrphy_apb_wr_d(0x00095,0x0);
dwc_ddrphy_apb_wr_d(0x00096,0x0);
dwc_ddrphy_apb_wr_d(0x00097,0x0);
dwc_ddrphy_apb_wr_d(0x00098,0x0);
dwc_ddrphy_apb_wr_d(0x00099,0x0);
dwc_ddrphy_apb_wr_d(0x0009a,0x0);
dwc_ddrphy_apb_wr_d(0x0009b,0x0);
dwc_ddrphy_apb_wr_d(0x0009c,0x0);
dwc_ddrphy_apb_wr_d(0x0009d,0x0);
dwc_ddrphy_apb_wr_d(0x0009e,0x0);
dwc_ddrphy_apb_wr_d(0x0009f,0x0);
dwc_ddrphy_apb_wr_d(0x000a0,0x0);
dwc_ddrphy_apb_wr_d(0x000a1,0x0);
dwc_ddrphy_apb_wr_d(0x000a2,0x0);
dwc_ddrphy_apb_wr_d(0x000a3,0x0);
dwc_ddrphy_apb_wr_d(0x000a4,0x0);
dwc_ddrphy_apb_wr_d(0x000a5,0x0);
dwc_ddrphy_apb_wr_d(0x000a6,0x0);
dwc_ddrphy_apb_wr_d(0x000a7,0x0);
dwc_ddrphy_apb_wr_d(0x000a8,0x0);
dwc_ddrphy_apb_wr_d(0x000a9,0x0);
dwc_ddrphy_apb_wr_d(0x000aa,0x0);
dwc_ddrphy_apb_wr_d(0x000ab,0x0);
dwc_ddrphy_apb_wr_d(0x000ac,0x0);
dwc_ddrphy_apb_wr_d(0x000ad,0x0);
dwc_ddrphy_apb_wr_d(0x000ae,0x0);
dwc_ddrphy_apb_wr_d(0x000af,0x0);
dwc_ddrphy_apb_wr_d(0x000b0,0x0);
dwc_ddrphy_apb_wr_d(0x000b1,0x0);
dwc_ddrphy_apb_wr_d(0x000b2,0x0);
dwc_ddrphy_apb_wr_d(0x000b3,0x0);
dwc_ddrphy_apb_wr_d(0x000b4,0x0);
dwc_ddrphy_apb_wr_d(0x000b5,0x0);
dwc_ddrphy_apb_wr_d(0x000b6,0x0);
dwc_ddrphy_apb_wr_d(0x000b7,0x0);
dwc_ddrphy_apb_wr_d(0x000b8,0x0);
dwc_ddrphy_apb_wr_d(0x000b9,0x0);
dwc_ddrphy_apb_wr_d(0x000ba,0x0);
dwc_ddrphy_apb_wr_d(0x000bb,0x0);
dwc_ddrphy_apb_wr_d(0x000bc,0x0);
dwc_ddrphy_apb_wr_d(0x000bd,0x0);
dwc_ddrphy_apb_wr_d(0x000be,0x0);
dwc_ddrphy_apb_wr_d(0x000bf,0x0);
dwc_ddrphy_apb_wr_d(0x000c0,0x0);
dwc_ddrphy_apb_wr_d(0x000c1,0x0);
dwc_ddrphy_apb_wr_d(0x000c2,0x0);
dwc_ddrphy_apb_wr_d(0x000c3,0x0);
dwc_ddrphy_apb_wr_d(0x000c4,0x0);
dwc_ddrphy_apb_wr_d(0x000c5,0x0);
dwc_ddrphy_apb_wr_d(0x000c6,0x0);
dwc_ddrphy_apb_wr_d(0x000c7,0x0);
dwc_ddrphy_apb_wr_d(0x000c8,0x0);
dwc_ddrphy_apb_wr_d(0x000c9,0x0);
dwc_ddrphy_apb_wr_d(0x000ca,0x0);
dwc_ddrphy_apb_wr_d(0x000cb,0x0);
dwc_ddrphy_apb_wr_d(0x000cc,0x0);
dwc_ddrphy_apb_wr_d(0x000cd,0x0);
dwc_ddrphy_apb_wr_d(0x000ce,0x0);
dwc_ddrphy_apb_wr_d(0x000cf,0x0);
dwc_ddrphy_apb_wr_d(0x000d0,0x0);
dwc_ddrphy_apb_wr_d(0x000d1,0x0);
dwc_ddrphy_apb_wr_d(0x000d2,0x0);
dwc_ddrphy_apb_wr_d(0x000d3,0x0);
dwc_ddrphy_apb_wr_d(0x000d4,0x0);
dwc_ddrphy_apb_wr_d(0x000d5,0x0);
dwc_ddrphy_apb_wr_d(0x000d6,0x0);
dwc_ddrphy_apb_wr_d(0x000d7,0x0);
dwc_ddrphy_apb_wr_d(0x000d8,0x0);
dwc_ddrphy_apb_wr_d(0x000d9,0x0);
dwc_ddrphy_apb_wr_d(0x000da,0x0);
dwc_ddrphy_apb_wr_d(0x000db,0x0);
dwc_ddrphy_apb_wr_d(0x000dc,0x0);
dwc_ddrphy_apb_wr_d(0x000dd,0x0);
dwc_ddrphy_apb_wr_d(0x000de,0x0);
dwc_ddrphy_apb_wr_d(0x000df,0x0);
dwc_ddrphy_apb_wr_d(0x000e0,0x0);
dwc_ddrphy_apb_wr_d(0x000e1,0x0);
dwc_ddrphy_apb_wr_d(0x000e2,0x0);
dwc_ddrphy_apb_wr_d(0x000e3,0x0);
dwc_ddrphy_apb_wr_d(0x000e4,0x0);
dwc_ddrphy_apb_wr_d(0x000e5,0x0);
dwc_ddrphy_apb_wr_d(0x000e6,0x0);
dwc_ddrphy_apb_wr_d(0x000e7,0x0);
dwc_ddrphy_apb_wr_d(0x000e8,0x0);
dwc_ddrphy_apb_wr_d(0x000e9,0x0);
dwc_ddrphy_apb_wr_d(0x000ea,0x0);
dwc_ddrphy_apb_wr_d(0x000eb,0x0);
dwc_ddrphy_apb_wr_d(0x000ec,0x0);
dwc_ddrphy_apb_wr_d(0x000ed,0x0);
dwc_ddrphy_apb_wr_d(0x000ee,0x0);
dwc_ddrphy_apb_wr_d(0x000ef,0x0);
dwc_ddrphy_apb_wr_d(0x000f0,0x0);
dwc_ddrphy_apb_wr_d(0x000f1,0x0);
dwc_ddrphy_apb_wr_d(0x000f2,0x0);
dwc_ddrphy_apb_wr_d(0x000f3,0x0);
dwc_ddrphy_apb_wr_d(0x000f4,0x0);
dwc_ddrphy_apb_wr_d(0x000f5,0x0);
dwc_ddrphy_apb_wr_d(0x000f6,0x0);
dwc_ddrphy_apb_wr_d(0x000f7,0x0);
dwc_ddrphy_apb_wr_d(0x000f8,0x0);
dwc_ddrphy_apb_wr_d(0x000f9,0x0);
dwc_ddrphy_apb_wr_d(0x000fa,0x0);
dwc_ddrphy_apb_wr_d(0x000fb,0x0);
dwc_ddrphy_apb_wr_d(0x000fc,0x0);
dwc_ddrphy_apb_wr_d(0x000fd,0x0);
dwc_ddrphy_apb_wr_d(0x000fe,0x0);
dwc_ddrphy_apb_wr_d(0x000ff,0x0);
dwc_ddrphy_apb_wr_d(0x00100,0x0);
dwc_ddrphy_apb_wr_d(0x00101,0x0);
dwc_ddrphy_apb_wr_d(0x00102,0x0);
dwc_ddrphy_apb_wr_d(0x00103,0x0);
dwc_ddrphy_apb_wr_d(0x00104,0x0);
dwc_ddrphy_apb_wr_d(0x00105,0x0);
dwc_ddrphy_apb_wr_d(0x00106,0x0);
dwc_ddrphy_apb_wr_d(0x00107,0x0);
dwc_ddrphy_apb_wr_d(0x00108,0x0);
dwc_ddrphy_apb_wr_d(0x00109,0x0);
dwc_ddrphy_apb_wr_d(0x0010a,0x0);
dwc_ddrphy_apb_wr_d(0x0010b,0x0);
dwc_ddrphy_apb_wr_d(0x0010c,0x0);
dwc_ddrphy_apb_wr_d(0x0010d,0x0);
dwc_ddrphy_apb_wr_d(0x0010e,0x0);
dwc_ddrphy_apb_wr_d(0x0010f,0x0);
dwc_ddrphy_apb_wr_d(0x00110,0x0);
dwc_ddrphy_apb_wr_d(0x00111,0x0);
dwc_ddrphy_apb_wr_d(0x00112,0x0);
dwc_ddrphy_apb_wr_d(0x00113,0x0);
dwc_ddrphy_apb_wr_d(0x00114,0x0);
dwc_ddrphy_apb_wr_d(0x00115,0x0);
dwc_ddrphy_apb_wr_d(0x00116,0x0);
dwc_ddrphy_apb_wr_d(0x00117,0x0);
dwc_ddrphy_apb_wr_d(0x00118,0x0);
dwc_ddrphy_apb_wr_d(0x00119,0x0);
dwc_ddrphy_apb_wr_d(0x0011a,0x0);
dwc_ddrphy_apb_wr_d(0x0011b,0x0);
dwc_ddrphy_apb_wr_d(0x0011c,0x0);
dwc_ddrphy_apb_wr_d(0x0011d,0x0);
dwc_ddrphy_apb_wr_d(0x0011e,0x0);
dwc_ddrphy_apb_wr_d(0x0011f,0x0);
dwc_ddrphy_apb_wr_d(0x00120,0x0);
dwc_ddrphy_apb_wr_d(0x00121,0x0);
dwc_ddrphy_apb_wr_d(0x00122,0x0);
dwc_ddrphy_apb_wr_d(0x00123,0x0);
dwc_ddrphy_apb_wr_d(0x00124,0x0);
dwc_ddrphy_apb_wr_d(0x00125,0x0);
dwc_ddrphy_apb_wr_d(0x00126,0x0);
dwc_ddrphy_apb_wr_d(0x00127,0x0);
dwc_ddrphy_apb_wr_d(0x00128,0x0);
dwc_ddrphy_apb_wr_d(0x00129,0x0);
dwc_ddrphy_apb_wr_d(0x0012a,0x0);
dwc_ddrphy_apb_wr_d(0x0012b,0x0);
dwc_ddrphy_apb_wr_d(0x0012c,0x0);
dwc_ddrphy_apb_wr_d(0x0012d,0x0);
dwc_ddrphy_apb_wr_d(0x0012e,0x0);
dwc_ddrphy_apb_wr_d(0x0012f,0x0);
dwc_ddrphy_apb_wr_d(0x00130,0x0);
dwc_ddrphy_apb_wr_d(0x00131,0x0);
dwc_ddrphy_apb_wr_d(0x00132,0x0);
dwc_ddrphy_apb_wr_d(0x00133,0x0);
dwc_ddrphy_apb_wr_d(0x00134,0x0);
dwc_ddrphy_apb_wr_d(0x00135,0x0);
dwc_ddrphy_apb_wr_d(0x00136,0x0);
dwc_ddrphy_apb_wr_d(0x00137,0x0);
dwc_ddrphy_apb_wr_d(0x00138,0x0);
dwc_ddrphy_apb_wr_d(0x00139,0x0);
dwc_ddrphy_apb_wr_d(0x0013a,0x0);
dwc_ddrphy_apb_wr_d(0x0013b,0x0);
dwc_ddrphy_apb_wr_d(0x0013c,0x0);
dwc_ddrphy_apb_wr_d(0x0013d,0x0);
dwc_ddrphy_apb_wr_d(0x0013e,0x0);
dwc_ddrphy_apb_wr_d(0x0013f,0x0);
dwc_ddrphy_apb_wr_d(0x00140,0x0);
dwc_ddrphy_apb_wr_d(0x00141,0x0);
dwc_ddrphy_apb_wr_d(0x00142,0x0);
dwc_ddrphy_apb_wr_d(0x00143,0x0);
dwc_ddrphy_apb_wr_d(0x00144,0x0);
dwc_ddrphy_apb_wr_d(0x00145,0x0);
dwc_ddrphy_apb_wr_d(0x00146,0x0);
dwc_ddrphy_apb_wr_d(0x00147,0x0);
dwc_ddrphy_apb_wr_d(0x00148,0x0);
dwc_ddrphy_apb_wr_d(0x00149,0x0);
dwc_ddrphy_apb_wr_d(0x0014a,0x0);
dwc_ddrphy_apb_wr_d(0x0014b,0x0);
dwc_ddrphy_apb_wr_d(0x0014c,0x0);
dwc_ddrphy_apb_wr_d(0x0014d,0x0);
dwc_ddrphy_apb_wr_d(0x0014e,0x0);
dwc_ddrphy_apb_wr_d(0x0014f,0x0);
dwc_ddrphy_apb_wr_d(0x00150,0x0);
dwc_ddrphy_apb_wr_d(0x00151,0x0);
dwc_ddrphy_apb_wr_d(0x00152,0x0);
dwc_ddrphy_apb_wr_d(0x00153,0x0);
dwc_ddrphy_apb_wr_d(0x00154,0x0);
dwc_ddrphy_apb_wr_d(0x00155,0x0);
dwc_ddrphy_apb_wr_d(0x00156,0x0);
dwc_ddrphy_apb_wr_d(0x00157,0x0);
dwc_ddrphy_apb_wr_d(0x00158,0x0);
dwc_ddrphy_apb_wr_d(0x00159,0x0);
dwc_ddrphy_apb_wr_d(0x0015a,0x0);
dwc_ddrphy_apb_wr_d(0x0015b,0x0);
dwc_ddrphy_apb_wr_d(0x0015c,0x0);
dwc_ddrphy_apb_wr_d(0x0015d,0x0);
dwc_ddrphy_apb_wr_d(0x0015e,0x0);
dwc_ddrphy_apb_wr_d(0x0015f,0x0);
dwc_ddrphy_apb_wr_d(0x00160,0x0);
dwc_ddrphy_apb_wr_d(0x00161,0x0);
dwc_ddrphy_apb_wr_d(0x00162,0x0);
dwc_ddrphy_apb_wr_d(0x00163,0x0);
dwc_ddrphy_apb_wr_d(0x00164,0x0);
dwc_ddrphy_apb_wr_d(0x00165,0x0);
dwc_ddrphy_apb_wr_d(0x00166,0x0);
dwc_ddrphy_apb_wr_d(0x00167,0x0);
dwc_ddrphy_apb_wr_d(0x00168,0x0);
dwc_ddrphy_apb_wr_d(0x00169,0x0);
dwc_ddrphy_apb_wr_d(0x0016a,0x0);
dwc_ddrphy_apb_wr_d(0x0016b,0x0);
dwc_ddrphy_apb_wr_d(0x0016c,0x0);
dwc_ddrphy_apb_wr_d(0x0016d,0x0);
dwc_ddrphy_apb_wr_d(0x0016e,0x0);
dwc_ddrphy_apb_wr_d(0x0016f,0x0);
dwc_ddrphy_apb_wr_d(0x00170,0x0);
dwc_ddrphy_apb_wr_d(0x00171,0x0);
dwc_ddrphy_apb_wr_d(0x00172,0x0);
dwc_ddrphy_apb_wr_d(0x00173,0x0);
dwc_ddrphy_apb_wr_d(0x00174,0x0);
dwc_ddrphy_apb_wr_d(0x00175,0x0);
dwc_ddrphy_apb_wr_d(0x00176,0x0);
dwc_ddrphy_apb_wr_d(0x00177,0x0);
dwc_ddrphy_apb_wr_d(0x00178,0x0);
dwc_ddrphy_apb_wr_d(0x00179,0x0);
dwc_ddrphy_apb_wr_d(0x0017a,0x0);
dwc_ddrphy_apb_wr_d(0x0017b,0x0);
dwc_ddrphy_apb_wr_d(0x0017c,0x0);
dwc_ddrphy_apb_wr_d(0x0017d,0x0);
dwc_ddrphy_apb_wr_d(0x0017e,0x0);
dwc_ddrphy_apb_wr_d(0x0017f,0x0);
dwc_ddrphy_apb_wr_d(0x00180,0x0);
dwc_ddrphy_apb_wr_d(0x00181,0x0);
dwc_ddrphy_apb_wr_d(0x00182,0x0);
dwc_ddrphy_apb_wr_d(0x00183,0x0);
dwc_ddrphy_apb_wr_d(0x00184,0x0);
dwc_ddrphy_apb_wr_d(0x00185,0x0);
dwc_ddrphy_apb_wr_d(0x00186,0x0);
dwc_ddrphy_apb_wr_d(0x00187,0x0);
dwc_ddrphy_apb_wr_d(0x00188,0x0);
dwc_ddrphy_apb_wr_d(0x00189,0x0);
dwc_ddrphy_apb_wr_d(0x0018a,0x0);
dwc_ddrphy_apb_wr_d(0x0018b,0x0);
dwc_ddrphy_apb_wr_d(0x0018c,0x0);
dwc_ddrphy_apb_wr_d(0x0018d,0x0);
dwc_ddrphy_apb_wr_d(0x0018e,0x0);
dwc_ddrphy_apb_wr_d(0x0018f,0x0);
dwc_ddrphy_apb_wr_d(0x00190,0x0);
dwc_ddrphy_apb_wr_d(0x00191,0x0);
dwc_ddrphy_apb_wr_d(0x00192,0x0);
dwc_ddrphy_apb_wr_d(0x00193,0x0);
dwc_ddrphy_apb_wr_d(0x00194,0x0);
dwc_ddrphy_apb_wr_d(0x00195,0x0);
dwc_ddrphy_apb_wr_d(0x00196,0x0);
dwc_ddrphy_apb_wr_d(0x00197,0x0);
dwc_ddrphy_apb_wr_d(0x00198,0x0);
dwc_ddrphy_apb_wr_d(0x00199,0x0);
dwc_ddrphy_apb_wr_d(0x0019a,0x0);
dwc_ddrphy_apb_wr_d(0x0019b,0x0);
dwc_ddrphy_apb_wr_d(0x0019c,0x0);
dwc_ddrphy_apb_wr_d(0x0019d,0x0);
dwc_ddrphy_apb_wr_d(0x0019e,0x0);
dwc_ddrphy_apb_wr_d(0x0019f,0x0);
dwc_ddrphy_apb_wr_d(0x001a0,0x0);
dwc_ddrphy_apb_wr_d(0x001a1,0x0);
dwc_ddrphy_apb_wr_d(0x001a2,0x0);
dwc_ddrphy_apb_wr_d(0x001a3,0x0);
dwc_ddrphy_apb_wr_d(0x001a4,0x0);
dwc_ddrphy_apb_wr_d(0x001a5,0x0);
dwc_ddrphy_apb_wr_d(0x001a6,0x0);
dwc_ddrphy_apb_wr_d(0x001a7,0x0);
dwc_ddrphy_apb_wr_d(0x001a8,0x0);
dwc_ddrphy_apb_wr_d(0x001a9,0x0);
dwc_ddrphy_apb_wr_d(0x001aa,0x0);
dwc_ddrphy_apb_wr_d(0x001ab,0x0);
dwc_ddrphy_apb_wr_d(0x001ac,0x0);
dwc_ddrphy_apb_wr_d(0x001ad,0x0);
dwc_ddrphy_apb_wr_d(0x001ae,0x0);
dwc_ddrphy_apb_wr_d(0x001af,0x0);
dwc_ddrphy_apb_wr_d(0x001b0,0x0);
dwc_ddrphy_apb_wr_d(0x001b1,0x0);
dwc_ddrphy_apb_wr_d(0x001b2,0x0);
dwc_ddrphy_apb_wr_d(0x001b3,0x0);
dwc_ddrphy_apb_wr_d(0x001b4,0x0);
dwc_ddrphy_apb_wr_d(0x001b5,0x0);
dwc_ddrphy_apb_wr_d(0x001b6,0x0);
dwc_ddrphy_apb_wr_d(0x001b7,0x0);
dwc_ddrphy_apb_wr_d(0x001b8,0x0);
dwc_ddrphy_apb_wr_d(0x001b9,0x0);
dwc_ddrphy_apb_wr_d(0x001ba,0x0);
dwc_ddrphy_apb_wr_d(0x001bb,0x0);
dwc_ddrphy_apb_wr_d(0x001bc,0x0);
dwc_ddrphy_apb_wr_d(0x001bd,0x0);
dwc_ddrphy_apb_wr_d(0x001be,0x0);
dwc_ddrphy_apb_wr_d(0x001bf,0x0);
dwc_ddrphy_apb_wr_d(0x001c0,0x0);
dwc_ddrphy_apb_wr_d(0x001c1,0x0);
dwc_ddrphy_apb_wr_d(0x001c2,0x0);
dwc_ddrphy_apb_wr_d(0x001c3,0x0);
dwc_ddrphy_apb_wr_d(0x001c4,0x0);
dwc_ddrphy_apb_wr_d(0x001c5,0x0);
dwc_ddrphy_apb_wr_d(0x001c6,0x0);
dwc_ddrphy_apb_wr_d(0x001c7,0x0);
dwc_ddrphy_apb_wr_d(0x001c8,0x0);
dwc_ddrphy_apb_wr_d(0x001c9,0x0);
dwc_ddrphy_apb_wr_d(0x001ca,0x0);
dwc_ddrphy_apb_wr_d(0x001cb,0x0);
dwc_ddrphy_apb_wr_d(0x001cc,0x0);
dwc_ddrphy_apb_wr_d(0x001cd,0x0);
dwc_ddrphy_apb_wr_d(0x001ce,0x0);
dwc_ddrphy_apb_wr_d(0x001cf,0x0);
dwc_ddrphy_apb_wr_d(0x001d0,0x0);
dwc_ddrphy_apb_wr_d(0x001d1,0x0);
dwc_ddrphy_apb_wr_d(0x001d2,0x0);
dwc_ddrphy_apb_wr_d(0x001d3,0x0);
dwc_ddrphy_apb_wr_d(0x001d4,0x0);
dwc_ddrphy_apb_wr_d(0x001d5,0x0);
dwc_ddrphy_apb_wr_d(0x001d6,0x0);
dwc_ddrphy_apb_wr_d(0x001d7,0x0);
dwc_ddrphy_apb_wr_d(0x001d8,0x0);
dwc_ddrphy_apb_wr_d(0x001d9,0x0);
dwc_ddrphy_apb_wr_d(0x001da,0x0);
dwc_ddrphy_apb_wr_d(0x001db,0x0);
dwc_ddrphy_apb_wr_d(0x001dc,0x0);
dwc_ddrphy_apb_wr_d(0x001dd,0x0);
dwc_ddrphy_apb_wr_d(0x001de,0x0);
dwc_ddrphy_apb_wr_d(0x001df,0x0);
dwc_ddrphy_apb_wr_d(0x001e0,0x0);
dwc_ddrphy_apb_wr_d(0x001e1,0x0);
dwc_ddrphy_apb_wr_d(0x001e2,0x0);
dwc_ddrphy_apb_wr_d(0x001e3,0x0);
dwc_ddrphy_apb_wr_d(0x001e4,0x0);
dwc_ddrphy_apb_wr_d(0x001e5,0x0);
dwc_ddrphy_apb_wr_d(0x001e6,0x0);
dwc_ddrphy_apb_wr_d(0x001e7,0x0);
dwc_ddrphy_apb_wr_d(0x001e8,0x0);
dwc_ddrphy_apb_wr_d(0x001e9,0x0);
dwc_ddrphy_apb_wr_d(0x001ea,0x0);
dwc_ddrphy_apb_wr_d(0x001eb,0x0);
dwc_ddrphy_apb_wr_d(0x001ec,0x0);
dwc_ddrphy_apb_wr_d(0x001ed,0x0);
dwc_ddrphy_apb_wr_d(0x001ee,0x0);
dwc_ddrphy_apb_wr_d(0x001ef,0x0);
dwc_ddrphy_apb_wr_d(0x001f0,0x0);
dwc_ddrphy_apb_wr_d(0x001f1,0x0);
dwc_ddrphy_apb_wr_d(0x001f2,0x0);
dwc_ddrphy_apb_wr_d(0x001f3,0x0);
dwc_ddrphy_apb_wr_d(0x001f4,0x0);
dwc_ddrphy_apb_wr_d(0x001f5,0x0);
dwc_ddrphy_apb_wr_d(0x001f6,0x0);
dwc_ddrphy_apb_wr_d(0x001f7,0x0);
dwc_ddrphy_apb_wr_d(0x001f8,0x0);
dwc_ddrphy_apb_wr_d(0x001f9,0x0);
dwc_ddrphy_apb_wr_d(0x001fa,0x0);
dwc_ddrphy_apb_wr_d(0x001fb,0x0);
dwc_ddrphy_apb_wr_d(0x001fc,0x0);
dwc_ddrphy_apb_wr_d(0x001fd,0x0);
dwc_ddrphy_apb_wr_d(0x001fe,0x0);
dwc_ddrphy_apb_wr_d(0x001ff,0x0);
dwc_ddrphy_apb_wr_d(0x00200,0x0);
dwc_ddrphy_apb_wr_d(0x00201,0x0);
dwc_ddrphy_apb_wr_d(0x00202,0x0);
dwc_ddrphy_apb_wr_d(0x00203,0x0);
dwc_ddrphy_apb_wr_d(0x00204,0x0);
dwc_ddrphy_apb_wr_d(0x00205,0xff);
dwc_ddrphy_apb_wr_d(0x00206,0x0);
dwc_ddrphy_apb_wr_d(0x00207,0x0);
dwc_ddrphy_apb_wr_d(0x00208,0x14a);
dwc_ddrphy_apb_wr_d(0x00209,0x181);
dwc_ddrphy_apb_wr_d(0x0020a,0x118);
dwc_ddrphy_apb_wr_d(0x0020b,0x118);
dwc_ddrphy_apb_wr_d(0x0020c,0x16f);
dwc_ddrphy_apb_wr_d(0x0020d,0x16f);
dwc_ddrphy_apb_wr_d(0x0020e,0x159);
dwc_ddrphy_apb_wr_d(0x0020f,0x181);
dwc_ddrphy_apb_wr_d(0x00210,0x120);
dwc_ddrphy_apb_wr_d(0x00211,0x120);
dwc_ddrphy_apb_wr_d(0x00212,0x0);
dwc_ddrphy_apb_wr_d(0x00213,0x0);
dwc_ddrphy_apb_wr_d(0x00214,0x701c);
dwc_ddrphy_apb_wr_d(0x00215,0x61a8);
dwc_ddrphy_apb_wr_d(0x00216,0x35ac);
dwc_ddrphy_apb_wr_d(0x00217,0x35ac);
dwc_ddrphy_apb_wr_d(0x00218,0x125c);
dwc_ddrphy_apb_wr_d(0x00219,0x125c);
dwc_ddrphy_apb_wr_d(0x0021a,0xc738);
dwc_ddrphy_apb_wr_d(0x0021b,0xb0f4);
dwc_ddrphy_apb_wr_d(0x0021c,0x6590);
dwc_ddrphy_apb_wr_d(0x0021d,0x6590);
dwc_ddrphy_apb_wr_d(0x0021e,0x0);
dwc_ddrphy_apb_wr_d(0x0021f,0x0);
dwc_ddrphy_apb_wr_d(0x00220,0x2e9e);
dwc_ddrphy_apb_wr_d(0x00221,0x33);
dwc_ddrphy_apb_wr_d(0x00222,0x0);
dwc_ddrphy_apb_wr_d(0x00223,0x0);
dwc_ddrphy_apb_wr_d(0x00224,0x2820);
dwc_ddrphy_apb_wr_d(0x00225,0x140f);
dwc_ddrphy_apb_wr_d(0x00226,0x2);
dwc_ddrphy_apb_wr_d(0x00227,0x0);
dwc_ddrphy_apb_wr_d(0x00228,0xf01f);
dwc_ddrphy_apb_wr_d(0x00229,0x1);
dwc_ddrphy_apb_wr_d(0x0022a,0x0);
dwc_ddrphy_apb_wr_d(0x0022b,0xffb4);
dwc_ddrphy_apb_wr_d(0x0022c,0x1);
dwc_ddrphy_apb_wr_d(0x0022d,0x1);
dwc_ddrphy_apb_wr_d(0x0022e,0xf0b4);
dwc_ddrphy_apb_wr_d(0x0022f,0x1);
dwc_ddrphy_apb_wr_d(0x00230,0x0);
dwc_ddrphy_apb_wr_d(0x00231,0xf4b4);
dwc_ddrphy_apb_wr_d(0x00232,0x1);
dwc_ddrphy_apb_wr_d(0x00233,0x0);
dwc_ddrphy_apb_wr_d(0x00234,0xf0b9);
dwc_ddrphy_apb_wr_d(0x00235,0x1);
dwc_ddrphy_apb_wr_d(0x00236,0x0);
dwc_ddrphy_apb_wr_d(0x00237,0xf0ba);
dwc_ddrphy_apb_wr_d(0x00238,0x1);
dwc_ddrphy_apb_wr_d(0x00239,0x0);
dwc_ddrphy_apb_wr_d(0x0023a,0xf0bb);
dwc_ddrphy_apb_wr_d(0x0023b,0x1);
dwc_ddrphy_apb_wr_d(0x0023c,0x0);
dwc_ddrphy_apb_wr_d(0x0023d,0xf001);
dwc_ddrphy_apb_wr_d(0x0023e,0x1);
dwc_ddrphy_apb_wr_d(0x0023f,0x0);
dwc_ddrphy_apb_wr_d(0x00240,0xf011);
dwc_ddrphy_apb_wr_d(0x00241,0x1);
dwc_ddrphy_apb_wr_d(0x00242,0x1);
dwc_ddrphy_apb_wr_d(0x00243,0xf012);
dwc_ddrphy_apb_wr_d(0x00244,0x1);
dwc_ddrphy_apb_wr_d(0x00245,0xf000);
dwc_ddrphy_apb_wr_d(0x00246,0xf018);
dwc_ddrphy_apb_wr_d(0x00247,0x1);
dwc_ddrphy_apb_wr_d(0x00248,0x1);
dwc_ddrphy_apb_wr_d(0x00249,0xf013);
dwc_ddrphy_apb_wr_d(0x0024a,0x1);
dwc_ddrphy_apb_wr_d(0x0024b,0x0);
dwc_ddrphy_apb_wr_d(0x0024c,0xf0f9);
dwc_ddrphy_apb_wr_d(0x0024d,0x4);
dwc_ddrphy_apb_wr_d(0x0024e,0x200);
dwc_ddrphy_apb_wr_d(0x0024f,0xf0fa);
dwc_ddrphy_apb_wr_d(0x00250,0x4);
dwc_ddrphy_apb_wr_d(0x00251,0x0);
dwc_ddrphy_apb_wr_d(0x00252,0xf0fb);
dwc_ddrphy_apb_wr_d(0x00253,0x4);
dwc_ddrphy_apb_wr_d(0x00254,0x400);
dwc_ddrphy_apb_wr_d(0x00255,0xff62);
dwc_ddrphy_apb_wr_d(0x00256,0x1);
dwc_ddrphy_apb_wr_d(0x00257,0x0);
dwc_ddrphy_apb_wr_d(0x00258,0xf062);
dwc_ddrphy_apb_wr_d(0x00259,0x1);
dwc_ddrphy_apb_wr_d(0x0025a,0xf);
dwc_ddrphy_apb_wr_d(0x0025b,0xf462);
dwc_ddrphy_apb_wr_d(0x0025c,0x1);
dwc_ddrphy_apb_wr_d(0x0025d,0xf0);
dwc_ddrphy_apb_wr_d(0x0025e,0xf002);
dwc_ddrphy_apb_wr_d(0x0025f,0x1);
dwc_ddrphy_apb_wr_d(0x00260,0x204);
dwc_ddrphy_apb_wr_d(0x00261,0x0);
dwc_ddrphy_apb_wr_d(0x00262,0xf01f);
dwc_ddrphy_apb_wr_d(0x00263,0x1);
dwc_ddrphy_apb_wr_d(0x00264,0x0);
dwc_ddrphy_apb_wr_d(0x00265,0xffb4);
dwc_ddrphy_apb_wr_d(0x00266,0x1);
dwc_ddrphy_apb_wr_d(0x00267,0x1);
dwc_ddrphy_apb_wr_d(0x00268,0xf0b4);
dwc_ddrphy_apb_wr_d(0x00269,0x1);
dwc_ddrphy_apb_wr_d(0x0026a,0x0);
dwc_ddrphy_apb_wr_d(0x0026b,0xf4b4);
dwc_ddrphy_apb_wr_d(0x0026c,0x1);
dwc_ddrphy_apb_wr_d(0x0026d,0x0);
dwc_ddrphy_apb_wr_d(0x0026e,0xf0b9);
dwc_ddrphy_apb_wr_d(0x0026f,0x1);
dwc_ddrphy_apb_wr_d(0x00270,0x0);
dwc_ddrphy_apb_wr_d(0x00271,0xf0ba);
dwc_ddrphy_apb_wr_d(0x00272,0x1);
dwc_ddrphy_apb_wr_d(0x00273,0x0);
dwc_ddrphy_apb_wr_d(0x00274,0xf0bb);
dwc_ddrphy_apb_wr_d(0x00275,0x1);
dwc_ddrphy_apb_wr_d(0x00276,0x0);
dwc_ddrphy_apb_wr_d(0x00277,0xf001);
dwc_ddrphy_apb_wr_d(0x00278,0x1);
dwc_ddrphy_apb_wr_d(0x00279,0x0);
dwc_ddrphy_apb_wr_d(0x0027a,0xf013);
dwc_ddrphy_apb_wr_d(0x0027b,0x1);
dwc_ddrphy_apb_wr_d(0x0027c,0x0);
dwc_ddrphy_apb_wr_d(0x0027d,0xf0f9);
dwc_ddrphy_apb_wr_d(0x0027e,0x4);
dwc_ddrphy_apb_wr_d(0x0027f,0x200);
dwc_ddrphy_apb_wr_d(0x00280,0xf0fa);
dwc_ddrphy_apb_wr_d(0x00281,0x4);
dwc_ddrphy_apb_wr_d(0x00282,0x0);
dwc_ddrphy_apb_wr_d(0x00283,0xf0fb);
dwc_ddrphy_apb_wr_d(0x00284,0x4);
dwc_ddrphy_apb_wr_d(0x00285,0x400);
dwc_ddrphy_apb_wr_d(0x00286,0xf060);
dwc_ddrphy_apb_wr_d(0x00287,0x7);
dwc_ddrphy_apb_wr_d(0x00288,0x8);
dwc_ddrphy_apb_wr_d(0x00289,0xf065);
dwc_ddrphy_apb_wr_d(0x0028a,0x7);
dwc_ddrphy_apb_wr_d(0x0028b,0x0);
dwc_ddrphy_apb_wr_d(0x0028c,0xff62);
dwc_ddrphy_apb_wr_d(0x0028d,0x1);
dwc_ddrphy_apb_wr_d(0x0028e,0x0);
dwc_ddrphy_apb_wr_d(0x0028f,0xf002);
dwc_ddrphy_apb_wr_d(0x00290,0x1);
dwc_ddrphy_apb_wr_d(0x00291,0x220);
dwc_ddrphy_apb_wr_d(0x00292,0x10f8);
dwc_ddrphy_apb_wr_d(0x00293,0x20e8);
dwc_ddrphy_apb_wr_d(0x00294,0xf01f);
dwc_ddrphy_apb_wr_d(0x00295,0x1);
dwc_ddrphy_apb_wr_d(0x00296,0x0);
dwc_ddrphy_apb_wr_d(0x00297,0xf03b);
dwc_ddrphy_apb_wr_d(0x00298,0x2);
dwc_ddrphy_apb_wr_d(0x00299,0x1);
dwc_ddrphy_apb_wr_d(0x0029a,0xffb2);
dwc_ddrphy_apb_wr_d(0x0029b,0x1);
dwc_ddrphy_apb_wr_d(0x0029c,0x0);
dwc_ddrphy_apb_wr_d(0x0029d,0xf0b2);
dwc_ddrphy_apb_wr_d(0x0029e,0x1);
dwc_ddrphy_apb_wr_d(0x0029f,0x1);
dwc_ddrphy_apb_wr_d(0x002a0,0xffb4);
dwc_ddrphy_apb_wr_d(0x002a1,0x1);
dwc_ddrphy_apb_wr_d(0x002a2,0x1);
dwc_ddrphy_apb_wr_d(0x002a3,0xf0b4);
dwc_ddrphy_apb_wr_d(0x002a4,0x1);
dwc_ddrphy_apb_wr_d(0x002a5,0x0);
dwc_ddrphy_apb_wr_d(0x002a6,0xf0b9);
dwc_ddrphy_apb_wr_d(0x002a7,0x1);
dwc_ddrphy_apb_wr_d(0x002a8,0x0);
dwc_ddrphy_apb_wr_d(0x002a9,0xf0ba);
dwc_ddrphy_apb_wr_d(0x002aa,0x1);
dwc_ddrphy_apb_wr_d(0x002ab,0x0);
dwc_ddrphy_apb_wr_d(0x002ac,0xf0bb);
dwc_ddrphy_apb_wr_d(0x002ad,0x1);
dwc_ddrphy_apb_wr_d(0x002ae,0x0);
dwc_ddrphy_apb_wr_d(0x002af,0xf001);
dwc_ddrphy_apb_wr_d(0x002b0,0x1);
dwc_ddrphy_apb_wr_d(0x002b1,0x0);
dwc_ddrphy_apb_wr_d(0x002b2,0xf060);
dwc_ddrphy_apb_wr_d(0x002b3,0x7);
dwc_ddrphy_apb_wr_d(0x002b4,0x1);
dwc_ddrphy_apb_wr_d(0x002b5,0xf065);
dwc_ddrphy_apb_wr_d(0x002b6,0x7);
dwc_ddrphy_apb_wr_d(0x002b7,0x1ff);
dwc_ddrphy_apb_wr_d(0x002b8,0xff26);
dwc_ddrphy_apb_wr_d(0x002b9,0x7);
dwc_ddrphy_apb_wr_d(0x002ba,0x0);
dwc_ddrphy_apb_wr_d(0x002bb,0xff27);
dwc_ddrphy_apb_wr_d(0x002bc,0x7);
dwc_ddrphy_apb_wr_d(0x002bd,0x0);
dwc_ddrphy_apb_wr_d(0x002be,0xf013);
dwc_ddrphy_apb_wr_d(0x002bf,0x1);
dwc_ddrphy_apb_wr_d(0x002c0,0x2);
dwc_ddrphy_apb_wr_d(0x002c1,0xff32);
dwc_ddrphy_apb_wr_d(0x002c2,0x1);
dwc_ddrphy_apb_wr_d(0x002c3,0x800);
dwc_ddrphy_apb_wr_d(0x002c4,0xff62);
dwc_ddrphy_apb_wr_d(0x002c5,0x1);
dwc_ddrphy_apb_wr_d(0x002c6,0x0);
dwc_ddrphy_apb_wr_d(0x002c7,0xf062);
dwc_ddrphy_apb_wr_d(0x002c8,0x1);
dwc_ddrphy_apb_wr_d(0x002c9,0x1);
dwc_ddrphy_apb_wr_d(0x002ca,0xf462);
dwc_ddrphy_apb_wr_d(0x002cb,0x1);
dwc_ddrphy_apb_wr_d(0x002cc,0x1);
dwc_ddrphy_apb_wr_d(0x002cd,0xf002);
dwc_ddrphy_apb_wr_d(0x002ce,0x1);
dwc_ddrphy_apb_wr_d(0x002cf,0x208);
dwc_ddrphy_apb_wr_d(0x002d0,0xf01f);
dwc_ddrphy_apb_wr_d(0x002d1,0x1);
dwc_ddrphy_apb_wr_d(0x002d2,0x0);
dwc_ddrphy_apb_wr_d(0x002d3,0xffb2);
dwc_ddrphy_apb_wr_d(0x002d4,0x1);
dwc_ddrphy_apb_wr_d(0x002d5,0x0);
dwc_ddrphy_apb_wr_d(0x002d6,0xf0b2);
dwc_ddrphy_apb_wr_d(0x002d7,0x1);
dwc_ddrphy_apb_wr_d(0x002d8,0x1);
dwc_ddrphy_apb_wr_d(0x002d9,0xf4b2);
dwc_ddrphy_apb_wr_d(0x002da,0x1);
dwc_ddrphy_apb_wr_d(0x002db,0x1);
dwc_ddrphy_apb_wr_d(0x002dc,0xffb4);
dwc_ddrphy_apb_wr_d(0x002dd,0x1);
dwc_ddrphy_apb_wr_d(0x002de,0x1);
dwc_ddrphy_apb_wr_d(0x002df,0xf0b4);
dwc_ddrphy_apb_wr_d(0x002e0,0x1);
dwc_ddrphy_apb_wr_d(0x002e1,0x0);
dwc_ddrphy_apb_wr_d(0x002e2,0xf4b4);
dwc_ddrphy_apb_wr_d(0x002e3,0x1);
dwc_ddrphy_apb_wr_d(0x002e4,0x0);
dwc_ddrphy_apb_wr_d(0x002e5,0xf0b9);
dwc_ddrphy_apb_wr_d(0x002e6,0x1);
dwc_ddrphy_apb_wr_d(0x002e7,0x0);
dwc_ddrphy_apb_wr_d(0x002e8,0xf0ba);
dwc_ddrphy_apb_wr_d(0x002e9,0x1);
dwc_ddrphy_apb_wr_d(0x002ea,0x0);
dwc_ddrphy_apb_wr_d(0x002eb,0xf0bb);
dwc_ddrphy_apb_wr_d(0x002ec,0x1);
dwc_ddrphy_apb_wr_d(0x002ed,0x0);
dwc_ddrphy_apb_wr_d(0x002ee,0xf011);
dwc_ddrphy_apb_wr_d(0x002ef,0x1);
dwc_ddrphy_apb_wr_d(0x002f0,0x101);
dwc_ddrphy_apb_wr_d(0x002f1,0xf012);
dwc_ddrphy_apb_wr_d(0x002f2,0x1);
dwc_ddrphy_apb_wr_d(0x002f3,0x1);
dwc_ddrphy_apb_wr_d(0x002f4,0xf013);
dwc_ddrphy_apb_wr_d(0x002f5,0x1);
dwc_ddrphy_apb_wr_d(0x002f6,0x2);
dwc_ddrphy_apb_wr_d(0x002f7,0xf018);
dwc_ddrphy_apb_wr_d(0x002f8,0x1);
dwc_ddrphy_apb_wr_d(0x002f9,0x1);
dwc_ddrphy_apb_wr_d(0x002fa,0xf060);
dwc_ddrphy_apb_wr_d(0x002fb,0x7);
dwc_ddrphy_apb_wr_d(0x002fc,0x1);
dwc_ddrphy_apb_wr_d(0x002fd,0xf065);
dwc_ddrphy_apb_wr_d(0x002fe,0x7);
dwc_ddrphy_apb_wr_d(0x002ff,0x1ff);
dwc_ddrphy_apb_wr_d(0x00300,0xff26);
dwc_ddrphy_apb_wr_d(0x00301,0x7);
dwc_ddrphy_apb_wr_d(0x00302,0xffff);
dwc_ddrphy_apb_wr_d(0x00303,0xff27);
dwc_ddrphy_apb_wr_d(0x00304,0x7);
dwc_ddrphy_apb_wr_d(0x00305,0xffff);
dwc_ddrphy_apb_wr_d(0x00306,0xff62);
dwc_ddrphy_apb_wr_d(0x00307,0x1);
dwc_ddrphy_apb_wr_d(0x00308,0x0);
dwc_ddrphy_apb_wr_d(0x00309,0xf062);
dwc_ddrphy_apb_wr_d(0x0030a,0x1);
dwc_ddrphy_apb_wr_d(0x0030b,0x1);
dwc_ddrphy_apb_wr_d(0x0030c,0xf462);
dwc_ddrphy_apb_wr_d(0x0030d,0x1);
dwc_ddrphy_apb_wr_d(0x0030e,0x10);
dwc_ddrphy_apb_wr_d(0x0030f,0xff32);
dwc_ddrphy_apb_wr_d(0x00310,0x1);
dwc_ddrphy_apb_wr_d(0x00311,0x800);
dwc_ddrphy_apb_wr_d(0x00312,0xf002);
dwc_ddrphy_apb_wr_d(0x00313,0x1);
dwc_ddrphy_apb_wr_d(0x00314,0x210);
dwc_ddrphy_apb_wr_d(0x00315,0x0);
dwc_ddrphy_apb_wr_d(0x00316,0xa06);
dwc_ddrphy_apb_wr_d(0x00317,0x140e);
dwc_ddrphy_apb_wr_d(0x00318,0x1c18);
dwc_ddrphy_apb_wr_d(0x00319,0x2420);
dwc_ddrphy_apb_wr_d(0x0031a,0xc06);
dwc_ddrphy_apb_wr_d(0x0031b,0x1610);
dwc_ddrphy_apb_wr_d(0x0031c,0x201c);
dwc_ddrphy_apb_wr_d(0x0031d,0x2824);
dwc_ddrphy_apb_wr_d(0x0031e,0xa06);
dwc_ddrphy_apb_wr_d(0x0031f,0x1610);
dwc_ddrphy_apb_wr_d(0x00320,0x201a);
dwc_ddrphy_apb_wr_d(0x00321,0x2824);
dwc_ddrphy_apb_wr_d(0x00322,0xc06);
dwc_ddrphy_apb_wr_d(0x00323,0x1812);
dwc_ddrphy_apb_wr_d(0x00324,0x241e);
dwc_ddrphy_apb_wr_d(0x00325,0x2c28);
dwc_ddrphy_apb_wr_d(0x00326,0x20d);
dwc_ddrphy_apb_wr_d(0x00327,0x301);
dwc_ddrphy_apb_wr_d(0x00328,0xc0b);
dwc_ddrphy_apb_wr_d(0x00329,0x160e);
dwc_ddrphy_apb_wr_d(0x0032a,0x1004);
dwc_ddrphy_apb_wr_d(0x0032b,0x1811);
dwc_ddrphy_apb_wr_d(0x0032c,0x6e);
dwc_ddrphy_apb_wr_d(0x0032d,0x2);
dwc_ddrphy_apb_wr_d(0x0032e,0x1);
dwc_ddrphy_apb_wr_d(0x0032f,0xfd);
dwc_ddrphy_apb_wr_d(0x00330,0x4);
dwc_ddrphy_apb_wr_d(0x00331,0xf);
dwc_ddrphy_apb_wr_d(0x00332,0x60);
dwc_ddrphy_apb_wr_d(0x00333,0x7);
dwc_ddrphy_apb_wr_d(0x00334,0x0);
dwc_ddrphy_apb_wr_d(0x00335,0xe8);
dwc_ddrphy_apb_wr_d(0x00336,0x4);
dwc_ddrphy_apb_wr_d(0x00337,0xff);
dwc_ddrphy_apb_wr_d(0x00338,0xfc);
dwc_ddrphy_apb_wr_d(0x00339,0x4);
dwc_ddrphy_apb_wr_d(0x0033a,0x404);
dwc_ddrphy_apb_wr_d(0x0033b,0x3a);
dwc_ddrphy_apb_wr_d(0x0033c,0x2);
dwc_ddrphy_apb_wr_d(0x0033d,0x2);
dwc_ddrphy_apb_wr_d(0x0033e,0x11);
dwc_ddrphy_apb_wr_d(0x0033f,0x7);
dwc_ddrphy_apb_wr_d(0x00340,0x0);
dwc_ddrphy_apb_wr_d(0x00341,0x0);

// // [dwc_ddrphy_phyinit_WriteOutMem] DONE.  Index 0x342
// // 2.	Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1. 
// //      This allows the firmware unrestricted access to the configuration CSRs. 
dwc_ddrphy_apb_wr(0xd0000,0x1); // DWC_DDRPHYA_APBONLY0_MicroContMuxSel
// // [phyinit_F_loadDMEM, 1D] End of dwc_ddrphy_phyinit_F_loadDMEM()
// 
// 
  #endif// _ZEBU
}


