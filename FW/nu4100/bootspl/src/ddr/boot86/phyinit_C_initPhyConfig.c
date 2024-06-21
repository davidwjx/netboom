// ===========================================================================
// This C code is derived from Inomize's SystemVerilog of "phyinit_C_initPhyConfig.sv"
//
// Inomize, probably, generated thier code by converting the printed messages
// from running of Synopsys C program "dwc_ddrphy_phyinit_C_initPhyConfig".
//
// This program is in the provided code in the phyinit directory i.e.
// /tools/snps/dw/dwc_lpddr4_multiphy_v2_tsmc12ffc18_2.00a/Latest/phyinit/Latest/software/lpddr4/src/dwc_ddrphy_phyinit_C_initPhyConfig.c
// (but maybe Inomize have a newer version...)
//
// The comments prefixed by "[phyinit_C_initPhyConfig]" are printed messages
// with the values as calculated by Synopsys code.





#include "nu4000_c0_ddrp_regs.h"
#include "phyinit.h"

// ===========================================================================
void phyinit_C_initPhyConfig (unsigned int freq) {

// //##############################################################
// //
// // Step (C) Initialize PHY Configuration 
// //
// // Load the required PHY configuration registers for the appropriate mode and memory configuration
// //
// //##############################################################


// // [phyinit_C_initPhyConfig] Start of dwc_ddrphy_phyinit_C_initPhyConfig()
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1600MHz, Programming TxSlewRate::TxPreDrvMode to 0x1
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TxSlewRate::TxPreP to 0xf
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TxSlewRate::TxPreN to 0xf
// // [phyinit_C_initPhyConfig] ### NOTE ### Optimal setting for TxSlewRate::TxPreP and TxSlewRate::TxPreP are technology specific.
// // [phyinit_C_initPhyConfig] ### NOTE ### Please consult the "Output Slew Rate" section of HSpice Model App Note in specific technology for recommended settings
#ifdef DDR_FULL_BUS_WIDTH_FREQ1400_MODE
dwc_ddrphy_apb_wr(0x1005f,0x1ff); // DWC_DDRPHYA_DBYTE0_TxSlewRate_b0_p0
dwc_ddrphy_apb_wr(0x1015f,0x1ff); // DWC_DDRPHYA_DBYTE0_TxSlewRate_b1_p0
dwc_ddrphy_apb_wr(0x1105f,0x1ff); // DWC_DDRPHYA_DBYTE1_TxSlewRate_b0_p0
dwc_ddrphy_apb_wr(0x1115f,0x1ff); // DWC_DDRPHYA_DBYTE1_TxSlewRate_b1_p0
dwc_ddrphy_apb_wr(0x1205f,0x1ff); // DWC_DDRPHYA_DBYTE2_TxSlewRate_b0_p0
dwc_ddrphy_apb_wr(0x1215f,0x1ff); // DWC_DDRPHYA_DBYTE2_TxSlewRate_b1_p0
dwc_ddrphy_apb_wr(0x1305f,0x1ff); // DWC_DDRPHYA_DBYTE3_TxSlewRate_b0_p0
dwc_ddrphy_apb_wr(0x1315f,0x1ff); // DWC_DDRPHYA_DBYTE3_TxSlewRate_b1_p0
#elif DDR_FULL_BUS_WIDTH_FREQ800_MODE  // 800MHz
dwc_ddrphy_apb_wr(0x1005f,0x1ff); // DWC_DDRPHYA_DBYTE0_TxSlewRate_b0_p0
dwc_ddrphy_apb_wr(0x1015f,0x1ff); // DWC_DDRPHYA_DBYTE0_TxSlewRate_b1_p0
dwc_ddrphy_apb_wr(0x1105f,0x1ff); // DWC_DDRPHYA_DBYTE1_TxSlewRate_b0_p0
dwc_ddrphy_apb_wr(0x1115f,0x1ff); // DWC_DDRPHYA_DBYTE1_TxSlewRate_b1_p0
dwc_ddrphy_apb_wr(0x1205f,0x1ff); // DWC_DDRPHYA_DBYTE2_TxSlewRate_b0_p0
dwc_ddrphy_apb_wr(0x1215f,0x1ff); // DWC_DDRPHYA_DBYTE2_TxSlewRate_b1_p0
dwc_ddrphy_apb_wr(0x1305f,0x1ff); // DWC_DDRPHYA_DBYTE3_TxSlewRate_b0_p0
dwc_ddrphy_apb_wr(0x1315f,0x1ff); // DWC_DDRPHYA_DBYTE3_TxSlewRate_b1_p0
#else
dwc_ddrphy_apb_wr(0x1005f,0x1ff); // DWC_DDRPHYA_DBYTE0_TxSlewRate_b0_p0
dwc_ddrphy_apb_wr(0x1015f,0x1ff); // DWC_DDRPHYA_DBYTE0_TxSlewRate_b1_p0
dwc_ddrphy_apb_wr(0x1105f,0x1ff); // DWC_DDRPHYA_DBYTE1_TxSlewRate_b0_p0
dwc_ddrphy_apb_wr(0x1115f,0x1ff); // DWC_DDRPHYA_DBYTE1_TxSlewRate_b1_p0
dwc_ddrphy_apb_wr(0x1205f,0x1ff); // DWC_DDRPHYA_DBYTE2_TxSlewRate_b0_p0
dwc_ddrphy_apb_wr(0x1215f,0x1ff); // DWC_DDRPHYA_DBYTE2_TxSlewRate_b1_p0
dwc_ddrphy_apb_wr(0x1305f,0x1ff); // DWC_DDRPHYA_DBYTE3_TxSlewRate_b0_p0
dwc_ddrphy_apb_wr(0x1315f,0x1ff); // DWC_DDRPHYA_DBYTE3_TxSlewRate_b1_p0
#endif
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreDrvMode to 0x1, ANIB=0
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreP to 0xf, ANIB=0
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreN to 0xf, ANIB=0
// // [phyinit_C_initPhyConfig] ### NOTE ### Optimal setting for ATxSlewRate::ATxPreP and ATxSlewRate::ATxPreP are technology specific.
// // [phyinit_C_initPhyConfig] ### NOTE ### Please consult the "Output Slew Rate" section of HSpice Model App Note in specific technology for recommended settings

dwc_ddrphy_apb_wr(0x55,0x1ff); // DWC_DDRPHYA_ANIB0_ATxSlewRate
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreDrvMode to 0x1, ANIB=1
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreP to 0xf, ANIB=1
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreN to 0xf, ANIB=1
// // [phyinit_C_initPhyConfig] ### NOTE ### Optimal setting for ATxSlewRate::ATxPreP and ATxSlewRate::ATxPreP are technology specific.
// // [phyinit_C_initPhyConfig] ### NOTE ### Please consult the "Output Slew Rate" section of HSpice Model App Note in specific technology for recommended settings

dwc_ddrphy_apb_wr(0x1055,0x1ff); // DWC_DDRPHYA_ANIB1_ATxSlewRate
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreDrvMode to 0x1, ANIB=2
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreP to 0xf, ANIB=2
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreN to 0xf, ANIB=2
// // [phyinit_C_initPhyConfig] ### NOTE ### Optimal setting for ATxSlewRate::ATxPreP and ATxSlewRate::ATxPreP are technology specific.
// // [phyinit_C_initPhyConfig] ### NOTE ### Please consult the "Output Slew Rate" section of HSpice Model App Note in specific technology for recommended settings

dwc_ddrphy_apb_wr(0x2055,0x1ff); // DWC_DDRPHYA_ANIB2_ATxSlewRate
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreDrvMode to 0x1, ANIB=3
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreP to 0xf, ANIB=3
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreN to 0xf, ANIB=3
// // [phyinit_C_initPhyConfig] ### NOTE ### Optimal setting for ATxSlewRate::ATxPreP and ATxSlewRate::ATxPreP are technology specific.
// // [phyinit_C_initPhyConfig] ### NOTE ### Please consult the "Output Slew Rate" section of HSpice Model App Note in specific technology for recommended settings

dwc_ddrphy_apb_wr(0x3055,0x1ff); // DWC_DDRPHYA_ANIB3_ATxSlewRate
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreDrvMode to 0x1, ANIB=4
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreP to 0xf, ANIB=4
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreN to 0xf, ANIB=4
// // [phyinit_C_initPhyConfig] ### NOTE ### Optimal setting for ATxSlewRate::ATxPreP and ATxSlewRate::ATxPreP are technology specific.
// // [phyinit_C_initPhyConfig] ### NOTE ### Please consult the "Output Slew Rate" section of HSpice Model App Note in specific technology for recommended settings

dwc_ddrphy_apb_wr(0x4055,0x1ff); // DWC_DDRPHYA_ANIB4_ATxSlewRate
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreDrvMode to 0x1, ANIB=5
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreP to 0xf, ANIB=5
// // [phyinit_C_initPhyConfig] Programming ATxSlewRate::ATxPreN to 0xf, ANIB=5
// // [phyinit_C_initPhyConfig] ### NOTE ### Optimal setting for ATxSlewRate::ATxPreP and ATxSlewRate::ATxPreP are technology specific.
// // [phyinit_C_initPhyConfig] ### NOTE ### Please consult the "Output Slew Rate" section of HSpice Model App Note in specific technology for recommended settings

dwc_ddrphy_apb_wr(0x5055,0x1ff); // DWC_DDRPHYA_ANIB5_ATxSlewRate
 switch (freq) {
 case 334:
   dwc_ddrphy_apb_wr(0x200c5,0x7); // DWC_DDRPHYA_MASTER0_PllCtrl2_p0
   break;
 case 350:
   dwc_ddrphy_apb_wr(0x200c5,0x7); // DWC_DDRPHYA_MASTER0_PllCtrl2_p0
   break;
 case 533:
   dwc_ddrphy_apb_wr(0x200c5,0x6); // DWC_DDRPHYA_MASTER0_PllCtrl2_p0
 break;
 case 800:
   dwc_ddrphy_apb_wr(0x200c5,0xb); // DWC_DDRPHYA_MASTER0_PllCtrl2_p0
 break;
 case 1000:
   dwc_ddrphy_apb_wr(0x200c5,0xa); // DWC_DDRPHYA_MASTER0_PllCtrl2_p0
 break;
 case 1067:
   dwc_ddrphy_apb_wr(0x200c5,0xa); // DWC_DDRPHYA_MASTER0_PllCtrl2_p0
   break;
 case 1200:
   dwc_ddrphy_apb_wr(0x200c5,0xa); // DWC_DDRPHYA_MASTER0_PllCtrl2_p0
   break;
 case 1334:
   dwc_ddrphy_apb_wr(0x200c5,0x19); // DWC_DDRPHYA_MASTER0_PllCtrl2_p0
   break;
 case 1400:
   dwc_ddrphy_apb_wr(0x200c5,0x19); // DWC_DDRPHYA_MASTER0_PllCtrl2_p0
   break;
 default: // 1600
   dwc_ddrphy_apb_wr(0x200c5,0x19); // DWC_DDRPHYA_MASTER0_PllCtrl2_p0
 }
// // [phyinit_C_initPhyConfig] Pstate=0,  Memclk=1200MHz, Programming PllCtrl2 to a based on DfiClk frequency = 600.

// //##############################################################
// //
// // Program ARdPtrInitVal based on Frequency and PLL Bypass inputs
// // The values programmed here assume ideal properties of DfiClk
// // and Pclk including:
// // - DfiClk skew
// // - DfiClk jitter
// // - DfiClk PVT variations
// // - Pclk skew
// // - Pclk jitter
// //
// // PLL Bypassed mode:
// //     For MemClk frequency > 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 2-6
// //     For MemClk frequency < 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 1-6
// //
// // PLL Enabled mode:
// //     For MemClk frequency > 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 1-6
// //     For MemClk frequency < 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 0-6
// //
// //##############################################################
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming ARdPtrInitVal to 0x2
 switch (freq) {
 case 334:
   dwc_ddrphy_apb_wr(0x2002e,0x1); // DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p0
   break;
 case 350:
   dwc_ddrphy_apb_wr(0x2002e,0x1); // DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p0
   break;
 case 533:
   dwc_ddrphy_apb_wr(0x2002e,0x1); // DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p0
   break;
 case 800:
   dwc_ddrphy_apb_wr(0x2002e,0x1); // DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p0
   break;
 case 1000:
   dwc_ddrphy_apb_wr(0x2002e,0x2); // DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p0
   break;
 case 1067:
   dwc_ddrphy_apb_wr(0x2002e,0x2); // DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p0
   break;
 case 1200:
   dwc_ddrphy_apb_wr(0x2002e,0x2); // DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p0
   break;
 case 1334:
   dwc_ddrphy_apb_wr(0x2002e,0x2); // DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p0
   break;
 case 1400:
   dwc_ddrphy_apb_wr(0x2002e,0x2); // DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p0
   break;
 default: // 1600
   dwc_ddrphy_apb_wr(0x2002e,0x2); // DWC_DDRPHYA_MASTER0_ARdPtrInitVal_p0
 }

// 
// //##############################################################
// // Seq0BGPR4       = 0: Make ProcOdtAlwaysOn = 0 and ProcOdtAlwaysOff = 0 
// //##############################################################
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming ProcOdtCtl: Seq0BGPR4.ProcOdtAlwaysOff  to 0x0
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming ProcOdtCtl: Seq0BGPR4.ProcOdtAlwaysOn   to 0x0
dwc_ddrphy_apb_wr(0x90204,0x0); // DWC_DDRPHYA_INITENG0_Seq0BGPR4_p0
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming DqsPreambleControl::TwoTckRxDqsPre to 0x1
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming DqsPreambleControl::TwoTckTxDqsPre to 0x1
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming DqsPreambleControl::PositionDfeInit to 0x0
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming DqsPreambleControl::LP4TglTwoTckTxDqsPre to 0x1
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming DqsPreambleControl::LP4PostambleExt to 0x1
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming DqsPreambleControl::LP4SttcPreBridgeRxEn to 0x1
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming DqsPreambleControl to 0xe3
dwc_ddrphy_apb_wr(0x20024,0xe3); // DWC_DDRPHYA_MASTER0_DqsPreambleControl_p0
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming DbyteDllModeCntrl to 0x2
dwc_ddrphy_apb_wr(0x2003a,0x2); // DWC_DDRPHYA_MASTER0_DbyteDllModeCntrl
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming DllLockParam to 0x842
dwc_ddrphy_apb_wr(0x2007d,0x842); // DWC_DDRPHYA_MASTER0_DllLockParam_p0
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming DllGainCtl to 0x55
dwc_ddrphy_apb_wr(0x2007c,0x55); // DWC_DDRPHYA_MASTER0_DllGainCtl_p0
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming ProcOdtTimeCtl to 0x2
 switch (freq) {
 case 334:
   dwc_ddrphy_apb_wr(0x20056,0xa); // DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p0
   break;
 case 350:
   dwc_ddrphy_apb_wr(0x20056,0xa); // DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p0
   break;
 case 533:
   dwc_ddrphy_apb_wr(0x20056,0xa); // DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p0
   break;
 case 800:
   dwc_ddrphy_apb_wr(0x20056,0xa); // DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p0
   break;
 case 1000:
   dwc_ddrphy_apb_wr(0x20056,0x2); // DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p0
   break;
 case 1067:
   dwc_ddrphy_apb_wr(0x20056,0x2); // DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p0
   break;
 case 1200:
   dwc_ddrphy_apb_wr(0x20056,0x2); // DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p0
   break;
 case 1334:
   dwc_ddrphy_apb_wr(0x20056,0x3); // DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p0
   break;
 case 1400:
   dwc_ddrphy_apb_wr(0x20056,0x3); // DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p0
   break;
 default:
   dwc_ddrphy_apb_wr(0x20056,0x3); // DWC_DDRPHYA_MASTER0_ProcOdtTimeCtl_p0
 }
#ifdef DDR_FULL_BUS_WIDTH_FREQ1400_MODE

// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TxOdtDrvStren::ODTStrenP to 0x0
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TxOdtDrvStren::ODTStrenN to 0x18
dwc_ddrphy_apb_wr(0x1004d,0x680); // DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b0_p0
dwc_ddrphy_apb_wr(0x1014d,0x680); // DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b1_p0
dwc_ddrphy_apb_wr(0x1104d,0x680); // DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b0_p0
dwc_ddrphy_apb_wr(0x1114d,0x680); // DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b1_p0
dwc_ddrphy_apb_wr(0x1204d,0x680); // DWC_DDRPHYA_DBYTE2_TxOdtDrvStren_b0_p0
dwc_ddrphy_apb_wr(0x1214d,0x680); // DWC_DDRPHYA_DBYTE2_TxOdtDrvStren_b1_p0
dwc_ddrphy_apb_wr(0x1304d,0x680); // DWC_DDRPHYA_DBYTE3_TxOdtDrvStren_b0_p0
dwc_ddrphy_apb_wr(0x1314d,0x680); // DWC_DDRPHYA_DBYTE3_TxOdtDrvStren_b1_p0
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TxImpedanceCtrl1::DrvStrenFSDqP to 0x18
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TxImpedanceCtrl1::DrvStrenFSDqN to 0x18
dwc_ddrphy_apb_wr(0x10049,0xfff); // DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b0_p0
dwc_ddrphy_apb_wr(0x10149,0xfff); // DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b1_p0
dwc_ddrphy_apb_wr(0x11049,0xfff); // DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b0_p0
dwc_ddrphy_apb_wr(0x11149,0xfff); // DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b1_p0
dwc_ddrphy_apb_wr(0x12049,0xfff); // DWC_DDRPHYA_DBYTE2_TxImpedanceCtrl1_b0_p0
dwc_ddrphy_apb_wr(0x12149,0xfff); // DWC_DDRPHYA_DBYTE2_TxImpedanceCtrl1_b1_p0
dwc_ddrphy_apb_wr(0x13049,0xfff); // DWC_DDRPHYA_DBYTE3_TxImpedanceCtrl1_b0_p0
dwc_ddrphy_apb_wr(0x13149,0xfff); // DWC_DDRPHYA_DBYTE3_TxImpedanceCtrl1_b1_p0
#elif DDR_FULL_BUS_WIDTH_FREQ800_MODE
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TxOdtDrvStren::ODTStrenP to 0x0
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TxOdtDrvStren::ODTStrenN to 0x18
dwc_ddrphy_apb_wr(0x1004d,0x680); // DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b0_p0
dwc_ddrphy_apb_wr(0x1014d,0x680); // DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b1_p0
dwc_ddrphy_apb_wr(0x1104d,0x680); // DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b0_p0
dwc_ddrphy_apb_wr(0x1114d,0x680); // DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b1_p0
dwc_ddrphy_apb_wr(0x1204d,0x680); // DWC_DDRPHYA_DBYTE2_TxOdtDrvStren_b0_p0
dwc_ddrphy_apb_wr(0x1214d,0x680); // DWC_DDRPHYA_DBYTE2_TxOdtDrvStren_b1_p0
dwc_ddrphy_apb_wr(0x1304d,0x680); // DWC_DDRPHYA_DBYTE3_TxOdtDrvStren_b0_p0
dwc_ddrphy_apb_wr(0x1314d,0x680); // DWC_DDRPHYA_DBYTE3_TxOdtDrvStren_b1_p0
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TxImpedanceCtrl1::DrvStrenFSDqP to 0x18
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TxImpedanceCtrl1::DrvStrenFSDqN to 0x18
dwc_ddrphy_apb_wr(0x10049,0xfff); // DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b0_p0
dwc_ddrphy_apb_wr(0x10149,0xfff); // DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b1_p0
dwc_ddrphy_apb_wr(0x11049,0xfff); // DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b0_p0
dwc_ddrphy_apb_wr(0x11149,0xfff); // DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b1_p0
dwc_ddrphy_apb_wr(0x12049,0xfff); // DWC_DDRPHYA_DBYTE2_TxImpedanceCtrl1_b0_p0
dwc_ddrphy_apb_wr(0x12149,0xfff); // DWC_DDRPHYA_DBYTE2_TxImpedanceCtrl1_b1_p0
dwc_ddrphy_apb_wr(0x13049,0xfff); // DWC_DDRPHYA_DBYTE3_TxImpedanceCtrl1_b0_p0
dwc_ddrphy_apb_wr(0x13149,0xfff); // DWC_DDRPHYA_DBYTE3_TxImpedanceCtrl1_b1_p0
#else
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TxOdtDrvStren::ODTStrenP to 0x0
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TxOdtDrvStren::ODTStrenN to 0x18
dwc_ddrphy_apb_wr(0x1004d,0x680); // DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b0_p0
dwc_ddrphy_apb_wr(0x1014d,0x680); // DWC_DDRPHYA_DBYTE0_TxOdtDrvStren_b1_p0
dwc_ddrphy_apb_wr(0x1104d,0x680); // DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b0_p0
dwc_ddrphy_apb_wr(0x1114d,0x680); // DWC_DDRPHYA_DBYTE1_TxOdtDrvStren_b1_p0
dwc_ddrphy_apb_wr(0x1204d,0x680); // DWC_DDRPHYA_DBYTE2_TxOdtDrvStren_b0_p0
dwc_ddrphy_apb_wr(0x1214d,0x680); // DWC_DDRPHYA_DBYTE2_TxOdtDrvStren_b1_p0
dwc_ddrphy_apb_wr(0x1304d,0x680); // DWC_DDRPHYA_DBYTE3_TxOdtDrvStren_b0_p0
dwc_ddrphy_apb_wr(0x1314d,0x680); // DWC_DDRPHYA_DBYTE3_TxOdtDrvStren_b1_p0
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TxImpedanceCtrl1::DrvStrenFSDqP to 0x18
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TxImpedanceCtrl1::DrvStrenFSDqN to 0x18
dwc_ddrphy_apb_wr(0x10049,0xfff); // DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b0_p0
dwc_ddrphy_apb_wr(0x10149,0xfff); // DWC_DDRPHYA_DBYTE0_TxImpedanceCtrl1_b1_p0
dwc_ddrphy_apb_wr(0x11049,0xfff); // DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b0_p0
dwc_ddrphy_apb_wr(0x11149,0xfff); // DWC_DDRPHYA_DBYTE1_TxImpedanceCtrl1_b1_p0
dwc_ddrphy_apb_wr(0x12049,0xfff); // DWC_DDRPHYA_DBYTE2_TxImpedanceCtrl1_b0_p0
dwc_ddrphy_apb_wr(0x12149,0xfff); // DWC_DDRPHYA_DBYTE2_TxImpedanceCtrl1_b1_p0
dwc_ddrphy_apb_wr(0x13049,0xfff); // DWC_DDRPHYA_DBYTE3_TxImpedanceCtrl1_b0_p0
dwc_ddrphy_apb_wr(0x13149,0xfff); // DWC_DDRPHYA_DBYTE3_TxImpedanceCtrl1_b1_p0
#endif
// // [phyinit_C_initPhyConfig] Programming ATxImpedance::ADrvStrenP to 0x1f
// // [phyinit_C_initPhyConfig] Programming ATxImpedance::ADrvStrenN to 0x1f
dwc_ddrphy_apb_wr(0x43,0x3ff); // DWC_DDRPHYA_ANIB0_ATxImpedance
dwc_ddrphy_apb_wr(0x1043,0x3ff); // DWC_DDRPHYA_ANIB1_ATxImpedance
dwc_ddrphy_apb_wr(0x2043,0x3ff); // DWC_DDRPHYA_ANIB2_ATxImpedance
dwc_ddrphy_apb_wr(0x3043,0x3ff); // DWC_DDRPHYA_ANIB3_ATxImpedance
dwc_ddrphy_apb_wr(0x4043,0x3ff); // DWC_DDRPHYA_ANIB4_ATxImpedance
dwc_ddrphy_apb_wr(0x5043,0x3ff); // DWC_DDRPHYA_ANIB5_ATxImpedance
#ifdef DDR_HALF_BUS_WIDTH_MODE
// // [phyinit_C_initPhyConfig] Programming DfiMode to 0x3
dwc_ddrphy_apb_wr(0x20018,0x1); // DWC_DDRPHYA_MASTER0_DfiMode
#else
// // [phyinit_C_initPhyConfig] Programming DfiMode to 0x3
dwc_ddrphy_apb_wr(0x20018,0x3); // DWC_DDRPHYA_MASTER0_DfiMode
#endif
// // [phyinit_C_initPhyConfig] Programming DfiCAMode to 0x4
dwc_ddrphy_apb_wr(0x20075,0x4); // DWC_DDRPHYA_MASTER0_DfiCAMode
// // [phyinit_C_initPhyConfig] Programming CalDrvStr0::CalDrvStrPd50 to 0x0
// // [phyinit_C_initPhyConfig] Programming CalDrvStr0::CalDrvStrPu50 to 0x0
dwc_ddrphy_apb_wr(0x20050,0x0); // DWC_DDRPHYA_MASTER0_CalDrvStr0
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming CalUclkInfo::CalUClkTicksPer1uS to 0x258
 switch (freq) {
 case 334:
   dwc_ddrphy_apb_wr(0x20008,0xa7);  // DWC_DDRPHYA_MASTER0_CalUclkInfo_p0
 break;
 case 350:
   dwc_ddrphy_apb_wr(0x20008,0xaf);  // DWC_DDRPHYA_MASTER0_CalUclkInfo_p0
 break;
 case 533:
   dwc_ddrphy_apb_wr(0x20008,0x10b); // DWC_DDRPHYA_MASTER0_CalUclkInfo_p0
 break;
 case 800:
   dwc_ddrphy_apb_wr(0x20008,0x190); // DWC_DDRPHYA_MASTER0_CalUclkInfo_p0
 break;
 case 1000:
   dwc_ddrphy_apb_wr(0x20008,0x1f4); // DWC_DDRPHYA_MASTER0_CalUclkInfo_p0
 break;
 case 1067:
   dwc_ddrphy_apb_wr(0x20008,0x216); // DWC_DDRPHYA_MASTER0_CalUclkInfo_p0
 break;
 case 1200:
   dwc_ddrphy_apb_wr(0x20008,0x258); // DWC_DDRPHYA_MASTER0_CalUclkInfo_p0
 break;
 case 1334:
   dwc_ddrphy_apb_wr(0x20008,0x29b); // DWC_DDRPHYA_MASTER0_CalUclkInfo_p0
   break;
 case 1400:
   dwc_ddrphy_apb_wr(0x20008,0x2bc); // DWC_DDRPHYA_MASTER0_CalUclkInfo_p0
   break;
 default:
   dwc_ddrphy_apb_wr(0x20008,0x320); // DWC_DDRPHYA_MASTER0_CalUclkInfo_p0
 }

// // [phyinit_C_initPhyConfig] Programming CalRate::CalInterval to 0x9
// // [phyinit_C_initPhyConfig] Programming CalRate::CalOnce to 0x0
dwc_ddrphy_apb_wr(0x20088,0x9); // DWC_DDRPHYA_MASTER0_CalRate
// // [phyinit_C_initPhyConfig] Pstate=0, Programming VrefInGlobal::GlobalVrefInSel to 0x4
// // [phyinit_C_initPhyConfig] Pstate=0, Programming VrefInGlobal::GlobalVrefInDAC to 0x20
// // [phyinit_C_initPhyConfig] Pstate=0, Programming VrefInGlobal to 0x104
#ifdef DDR_FULL_BUS_WIDTH_FREQ1400_MODE
dwc_ddrphy_apb_wr(0x200b2,0x114); // DWC_DDRPHYA_MASTER0_VrefInGlobal_p0
#elif DDR_FULL_BUS_WIDTH_FREQ800_MODE // 800MHz
dwc_ddrphy_apb_wr(0x200b2,0x114); // DWC_DDRPHYA_MASTER0_VrefInGlobal_p0
#else
dwc_ddrphy_apb_wr(0x200b2,0x114); // DWC_DDRPHYA_MASTER0_VrefInGlobal_p0
#endif
// // [phyinit_C_initPhyConfig] Pstate=0, Programming DqDqsRcvCntrl::MajorModeDbyte to 0x2
// // [phyinit_C_initPhyConfig] Pstate=0, Programming DqDqsRcvCntrl to 0x5a1
dwc_ddrphy_apb_wr(0x10043,0x5a1); // DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b0_p0
dwc_ddrphy_apb_wr(0x10143,0x5a1); // DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl_b1_p0
dwc_ddrphy_apb_wr(0x11043,0x5a1); // DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b0_p0
dwc_ddrphy_apb_wr(0x11143,0x5a1); // DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl_b1_p0
dwc_ddrphy_apb_wr(0x12043,0x5a1); // DWC_DDRPHYA_DBYTE2_DqDqsRcvCntrl_b0_p0
dwc_ddrphy_apb_wr(0x12143,0x5a1); // DWC_DDRPHYA_DBYTE2_DqDqsRcvCntrl_b1_p0
dwc_ddrphy_apb_wr(0x13043,0x5a1); // DWC_DDRPHYA_DBYTE3_DqDqsRcvCntrl_b0_p0
dwc_ddrphy_apb_wr(0x13143,0x5a1); // DWC_DDRPHYA_DBYTE3_DqDqsRcvCntrl_b1_p0
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming DfiFreqRatio_p0 to 0x1
dwc_ddrphy_apb_wr(0x200fa,0x1); // DWC_DDRPHYA_MASTER0_DfiFreqRatio_p0
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TristateModeCA::DisDynAdrTri_p0 to 0x1
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming TristateModeCA::DDR2TMode_p0 to 0x0
dwc_ddrphy_apb_wr(0x20019,0x1); // DWC_DDRPHYA_MASTER0_TristateModeCA_p0
// // [phyinit_C_initPhyConfig] Programming DfiFreqXlat*
dwc_ddrphy_apb_wr(0x200f0,0x0); // DWC_DDRPHYA_MASTER0_DfiFreqXlat0
dwc_ddrphy_apb_wr(0x200f1,0x0); // DWC_DDRPHYA_MASTER0_DfiFreqXlat1
dwc_ddrphy_apb_wr(0x200f2,0x4444); // DWC_DDRPHYA_MASTER0_DfiFreqXlat2
dwc_ddrphy_apb_wr(0x200f3,0x8888); // DWC_DDRPHYA_MASTER0_DfiFreqXlat3
dwc_ddrphy_apb_wr(0x200f4,0x5555); // DWC_DDRPHYA_MASTER0_DfiFreqXlat4
dwc_ddrphy_apb_wr(0x200f5,0x0); // DWC_DDRPHYA_MASTER0_DfiFreqXlat5
dwc_ddrphy_apb_wr(0x200f6,0x0); // DWC_DDRPHYA_MASTER0_DfiFreqXlat6
dwc_ddrphy_apb_wr(0x200f7,0xf000); // DWC_DDRPHYA_MASTER0_DfiFreqXlat7

#ifdef DDR_HALF_BUS_WIDTH_MODE
  // disable Dbyte2
  DDRP_DBYTE2_DBYTEMISCMODE_VAL = 0x4;
  DDRP_DBYTE2_DQDQSRCVCNTRL1_VAL = 0x7ff;
  // disable Dbyte3
  DDRP_DBYTE3_DBYTEMISCMODE_VAL = 0x4;
  DDRP_DBYTE3_DQDQSRCVCNTRL1_VAL = 0x7ff;
#endif


// // [phyinit_C_initPhyConfig] Disabling Lane 8 Receiver to save power.0
dwc_ddrphy_apb_wr(0x1004a,0x500); // DWC_DDRPHYA_DBYTE0_DqDqsRcvCntrl1
// // [phyinit_C_initPhyConfig] Disabling Lane 8 Receiver to save power.1
dwc_ddrphy_apb_wr(0x1104a,0x500); // DWC_DDRPHYA_DBYTE1_DqDqsRcvCntrl1
// // [phyinit_C_initPhyConfig] Disabling Lane 8 Receiver to save power.2
dwc_ddrphy_apb_wr(0x1204a,0x500); // DWC_DDRPHYA_DBYTE2_DqDqsRcvCntrl1
// // [phyinit_C_initPhyConfig] Disabling Lane 8 Receiver to save power.3
dwc_ddrphy_apb_wr(0x1304a,0x500); // DWC_DDRPHYA_DBYTE3_DqDqsRcvCntrl1
// // [phyinit_C_initPhyConfig] Programming MasterX4Config::X4TG to 0x0
dwc_ddrphy_apb_wr(0x20025,0x0); // DWC_DDRPHYA_MASTER0_MasterX4Config
// // [phyinit_C_initPhyConfig] Pstate=0, Memclk=1200MHz, Programming DMIPinPresent::RdDbiEnabled to 0x0
dwc_ddrphy_apb_wr(0x2002d,0x0); // DWC_DDRPHYA_MASTER0_DMIPinPresent_p0
dwc_ddrphy_apb_wr(0x2002c,0x0); // DWC_DDRPHYA_MASTER0_Acx4AnibDis
// // [phyinit_C_initPhyConfig] End of dwc_ddrphy_phyinit_C_initPhyConfig()
// 
// 
// //##############################################################
// //
// // dwc_ddrphy_phyihunit_userCustom_customPreTrain is a user-editable function.
// //
// // See PhyInit App Note for detailed description and function usage
// //
// //##############################################################
// 
// // [phyinit_userCustom_customPreTrain] Start of dwc_ddrphy_phyinit_userCustom_customPreTrain()
// // [phyinit_userCustom_customPreTrain] End of dwc_ddrphy_phyinit_userCustom_customPreTrain()
// // [dwc_ddrphy_phyinit_D_loadIMEM, 1D] Start of dwc_ddrphy_phyinit_D_loadIMEM (Train2D=0)
// 
//

// Programming ddrp_DBYTE0_Dq0LnSel 

DDRP_DBYTE0_DQ0LNSEL_VAL = 0;
DDRP_DBYTE0_DQ1LNSEL_VAL = 1;
DDRP_DBYTE0_DQ2LNSEL_VAL = 2;
DDRP_DBYTE0_DQ3LNSEL_VAL = 3;
DDRP_DBYTE0_DQ4LNSEL_VAL = 4;
DDRP_DBYTE0_DQ5LNSEL_VAL = 5;
DDRP_DBYTE0_DQ6LNSEL_VAL = 6;
DDRP_DBYTE0_DQ7LNSEL_VAL = 7;

DDRP_DBYTE1_DQ0LNSEL_VAL = 3;
DDRP_DBYTE1_DQ1LNSEL_VAL = 4;
DDRP_DBYTE1_DQ2LNSEL_VAL = 0;
DDRP_DBYTE1_DQ3LNSEL_VAL = 5;
DDRP_DBYTE1_DQ4LNSEL_VAL = 1;
DDRP_DBYTE1_DQ5LNSEL_VAL = 2;
DDRP_DBYTE1_DQ6LNSEL_VAL = 7;
DDRP_DBYTE1_DQ7LNSEL_VAL = 6;

DDRP_DBYTE2_DQ0LNSEL_VAL = 3;
DDRP_DBYTE2_DQ1LNSEL_VAL = 1;
DDRP_DBYTE2_DQ2LNSEL_VAL = 7;
DDRP_DBYTE2_DQ3LNSEL_VAL = 6;
DDRP_DBYTE2_DQ4LNSEL_VAL = 4;
DDRP_DBYTE2_DQ5LNSEL_VAL = 5;
DDRP_DBYTE2_DQ6LNSEL_VAL = 0;
DDRP_DBYTE2_DQ7LNSEL_VAL = 2;

DDRP_DBYTE3_DQ0LNSEL_VAL = 5;
DDRP_DBYTE3_DQ1LNSEL_VAL = 0;
DDRP_DBYTE3_DQ2LNSEL_VAL = 2;
DDRP_DBYTE3_DQ3LNSEL_VAL = 3;
DDRP_DBYTE3_DQ4LNSEL_VAL = 7;
DDRP_DBYTE3_DQ5LNSEL_VAL = 6;
DDRP_DBYTE3_DQ6LNSEL_VAL = 4;
DDRP_DBYTE3_DQ7LNSEL_VAL = 1;


}


