/*
------------------------------------------------------------------------
--                   (C) COPYRIGHT 2002-2004 SYNOPSYS INC.
--                             ALL RIGHTS RESERVED
--
--  This software and the associated documentation are confidential and
--  proprietary to Synopsys, Inc.  Your use or disclosure of this software
--  is subject to the terms and conditions of a written license agreement
--  between you, or your company, and Synopsys, Inc.
--
-- File :                       DW_apb_uart_defs.h
-- Author:                      Marc Wall
-- Date :                       $Date: 2004/12/04 $
-- Version      :               $Revision: #7 $
--
--
-- Modification History:
-- Date                 By      Version Change  Description
-- =====================================================================
-- See CVS log
-- =====================================================================
*/

#include "nu_regs.h"

#define i_uart1_DW_APB_UART_BASE UART0_BASE
#define i_uart2_DW_APB_UART_BASE UART1_BASE
#define i_uart3_DW_APB_UART_BASE UART2_BASE

#define i_uart1_UARTReceiveBufferReg_Offset               0x00
#define i_uart1_UARTTransmitHoldingReg_Offset             0x00
#define i_uart1_UARTDivisorLatchLow_Offset                0x00
#define i_uart1_UARTInterruptEnableReg_Offset             0x04
#define i_uart1_UARTDivisorLatchHigh_Offset               0x04
#define i_uart1_UARTInterruptIdentificationReg_Offset     0x08
#define i_uart1_UARTFIFOControlReg_Offset                 0x08
#define i_uart1_UARTLineControlReg_Offset                 0x0C
#define i_uart1_UARTModemControlReg_Offset                0x10
#define i_uart1_UARTLineStatusReg_Offset                  0x14
#define i_uart1_UARTModemStatusReg_Offset                 0x18

#define i_uart1_UARTScratchpadReg_Offset                  0x1C

#define i_uart1_UARTShadowReceiveBufferRegLow_Offset      0x30

#define i_uart1_UARTShadowReceiveBufferRegHigh_Offset     0x6C

#define i_uart1_UARTShadowTransmitHoldingRegLow_Offset    0x30

#define i_uart1_UARTShadowTransmitHoldingRegHigh_Offset   0x6C

#define i_uart1_UARTFIFOAccessReg_Offset                  0x70

#define i_uart1_UARTTransmitFIFOReadReg_Offset            0x74

#define i_uart1_UARTReceiveFIFOWriteReg_Offset            0x78

#define i_uart1_UARTUARTStatusReg_Offset                  0x7C

#define i_uart1_UARTTransmitFIFOLevelReg_Offset           0x80

#define i_uart1_UARTReceiveFIFOLevelReg_Offset            0x84

#define i_uart1_UARTSoftwareResetReg_Offset               0x88

#define i_uart1_UARTShadowRequestToSendReg_Offset         0x8C

#define i_uart1_UARTShadowBreakControlReg_Offset          0x90

#define i_uart1_UARTShadowDMAModeReg_Offset               0x94

#define i_uart1_UARTShadowFIFOEnableReg_Offset            0x98

#define i_uart1_UARTShadowRCVRTriggerReg_Offset           0x9C

#define i_uart1_UARTShadowTXEmptyTriggerReg_Offset        0xA0

#define i_uart1_UARTHaltTXReg_Offset                      0xA4

#define i_uart1_UARTDMASAReg_Offset                       0xA8

#define i_uart1_UARTCIDReg_Offset                         0xF4

#define i_uart1_UARTCVReg_Offset                          0xF8

#define i_uart1_UARTPIDReg_Offset                         0xFC

#define i_uart1_UARTReceiveBufferReg             (i_uart1_DW_APB_UART_BASE + i_uart1_UARTReceiveBufferReg_Offset            )
#define i_uart1_UARTTransmitHoldingReg           (i_uart1_DW_APB_UART_BASE + i_uart1_UARTTransmitHoldingReg_Offset          )
#define i_uart1_UARTDivisorLatchLow              (i_uart1_DW_APB_UART_BASE + i_uart1_UARTDivisorLatchLow_Offset             )
#define i_uart1_UARTInterruptEnableReg           (i_uart1_DW_APB_UART_BASE + i_uart1_UARTInterruptEnableReg_Offset          )
#define i_uart1_UARTDivisorLatchHigh             (i_uart1_DW_APB_UART_BASE + i_uart1_UARTDivisorLatchHigh_Offset            )
#define i_uart1_UARTInterruptIdentificationReg   (i_uart1_DW_APB_UART_BASE + i_uart1_UARTInterruptIdentificationReg_Offset  )
#define i_uart1_UARTFIFOControlReg               (i_uart1_DW_APB_UART_BASE + i_uart1_UARTFIFOControlReg_Offset              )
#define i_uart1_UARTLineControlReg               (i_uart1_DW_APB_UART_BASE + i_uart1_UARTLineControlReg_Offset              )
#define i_uart1_UARTModemControlReg              (i_uart1_DW_APB_UART_BASE + i_uart1_UARTModemControlReg_Offset             )
#define i_uart1_UARTLineStatusReg                (i_uart1_DW_APB_UART_BASE + i_uart1_UARTLineStatusReg_Offset               )
#define i_uart1_UARTModemStatusReg               (i_uart1_DW_APB_UART_BASE + i_uart1_UARTModemStatusReg_Offset              )
#define i_uart1_UARTScratchpadReg                (i_uart1_DW_APB_UART_BASE + i_uart1_UARTScratchpadReg_Offset               )
#define i_uart1_UARTShadowReceiveBufferRegLow    (i_uart1_DW_APB_UART_BASE + i_uart1_UARTShadowReceiveBufferRegLow_Offset   )
#define i_uart1_UARTShadowReceiveBufferRegHigh   (i_uart1_DW_APB_UART_BASE + i_uart1_UARTShadowReceiveBufferRegHigh_Offset  )
#define i_uart1_UARTShadowTransmitHoldingRegLow  (i_uart1_DW_APB_UART_BASE + i_uart1_UARTShadowTransmitHoldingRegLow_Offset )
#define i_uart1_UARTShadowTransmitHoldingRegHigh (i_uart1_DW_APB_UART_BASE + i_uart1_UARTShadowTransmitHoldingRegHigh_Offset)
#define i_uart1_UARTFIFOAccessReg                (i_uart1_DW_APB_UART_BASE + i_uart1_UARTFIFOAccessReg_Offset               )
#define i_uart1_UARTTransmitFIFOReadReg          (i_uart1_DW_APB_UART_BASE + i_uart1_UARTTransmitFIFOReadReg_Offset         )
#define i_uart1_UARTReceiveFIFOWriteReg          (i_uart1_DW_APB_UART_BASE + i_uart1_UARTReceiveFIFOWriteReg_Offset         )
#define i_uart1_UARTUARTStatusReg                (i_uart1_DW_APB_UART_BASE + i_uart1_UARTUARTStatusReg_Offset               )
#define i_uart1_UARTTransmitFIFOLevelReg         (i_uart1_DW_APB_UART_BASE + i_uart1_UARTTransmitFIFOLevelReg_Offset        )
#define i_uart1_UARTReceiveFIFOLevelReg          (i_uart1_DW_APB_UART_BASE + i_uart1_UARTReceiveFIFOLevelReg_Offset         )
#define i_uart1_UARTSoftwareResetReg             (i_uart1_DW_APB_UART_BASE + i_uart1_UARTSoftwareResetReg_Offset            )
#define i_uart1_UARTShadowRequestToSendReg       (i_uart1_DW_APB_UART_BASE + i_uart1_UARTShadowRequestToSendReg_Offset      )
#define i_uart1_UARTShadowBreakControlReg        (i_uart1_DW_APB_UART_BASE + i_uart1_UARTShadowBreakControlReg_Offset       )
#define i_uart1_UARTShadowDMAModeReg             (i_uart1_DW_APB_UART_BASE + i_uart1_UARTShadowDMAModeReg_Offset            )
#define i_uart1_UARTShadowFIFOEnableReg          (i_uart1_DW_APB_UART_BASE + i_uart1_UARTShadowFIFOEnableReg_Offset         )
#define i_uart1_UARTShadowRCVRTriggerReg         (i_uart1_DW_APB_UART_BASE + i_uart1_UARTShadowRCVRTriggerReg_Offset        )
#define i_uart1_UARTShadowTXEmptyTriggerReg      (i_uart1_DW_APB_UART_BASE + i_uart1_UARTShadowTXEmptyTriggerReg_Offset     )
#define i_uart1_UARTHaltTXReg                    (i_uart1_DW_APB_UART_BASE + i_uart1_UARTHaltTXReg_Offset                   )
#define i_uart1_UARTDMASAReg                     (i_uart1_DW_APB_UART_BASE + i_uart1_UARTDMASAReg_Offset                    )
#define i_uart1_UARTCIDReg                       (i_uart1_DW_APB_UART_BASE + i_uart1_UARTCIDReg_Offset                      )
#define i_uart1_UARTCVReg                        (i_uart1_DW_APB_UART_BASE + i_uart1_UARTCVReg_Offset                       )
#define i_uart1_UARTPIDReg                       (i_uart1_DW_APB_UART_BASE + i_uart1_UARTPIDReg_Offset                      )

#define i_uart1_UART_RBR             	       (i_uart1_DW_APB_UART_BASE + i_uart1_UARTReceiveBufferReg_Offset            )
#define i_uart1_UART_THR           	       (i_uart1_DW_APB_UART_BASE + i_uart1_UARTTransmitHoldingReg_Offset          )
#define i_uart1_UART_DLL              	       (i_uart1_DW_APB_UART_BASE + i_uart1_UARTDivisorLatchLow_Offset             )
#define i_uart1_UART_IER           	       (i_uart1_DW_APB_UART_BASE + i_uart1_UARTInterruptEnableReg_Offset          )
#define i_uart1_UART_DLH             	       (i_uart1_DW_APB_UART_BASE + i_uart1_UARTDivisorLatchHigh_Offset            )
#define i_uart1_UART_IIR   		       (i_uart1_DW_APB_UART_BASE + i_uart1_UARTInterruptIdentificationReg_Offset  )
#define i_uart1_UART_FCR               	       (i_uart1_DW_APB_UART_BASE + i_uart1_UARTFIFOControlReg_Offset              )
#define i_uart1_UART_LCR               	       (i_uart1_DW_APB_UART_BASE + i_uart1_UARTLineControlReg_Offset              )
#define i_uart1_UART_MCR              	       (i_uart1_DW_APB_UART_BASE + i_uart1_UARTModemControlReg_Offset             )
#define i_uart1_UART_LSR                	       (i_uart1_DW_APB_UART_BASE + i_uart1_UARTLineStatusReg_Offset               )
#define i_uart1_UART_MSR               	       (i_uart1_DW_APB_UART_BASE + i_uart1_UARTModemStatusReg_Offset              )
#define i_uart1_UART_SCR                	       (i_uart1_DW_APB_UART_BASE + i_uart1_UARTScratchpadReg_Offset               )
#define UARTi_uart1_PING_1BIT_WR (i_uart1_UART_LCR)
#define i_uart1_CC_UART_APB_DATA_WIDTH        16
#define i_uart1_CC_UART_MAX_APB_DATA_WIDTH    32
#define i_uart1_CC_UART_FIFO_MODE             16
#define i_uart1_CC_UART_MEM_SELECT            1
#define i_uart1_CC_UART_MEM_MODE              0
#define i_uart1_CC_UART_CLOCK_MODE            1
#define i_uart1_CC_UART_AFCE_MODE             0
#define i_uart1_CC_UART_THRE_MODE             0
#define i_uart1_CC_UART_SIR_MODE              0
#define i_uart1_CC_UART_CLK_GATE_EN           0
#define i_uart1_CC_UART_FIFO_ACCESS           0
#define i_uart1_CC_UART_DMA_EXTRA             0
#define i_uart1_CC_UART_DMA_POL               1
#define i_uart1_CC_UART_SIR_LP_MODE           0
#define i_uart1_CC_UART_DEBUG                 0
#define i_uart1_CC_UART_BAUD_CLK              1
#define i_uart1_CC_UART_ADDITIONAL_FEATURES   0
#define i_uart1_CC_UART_FIFO_STAT             0
#define i_uart1_CC_UART_SHADOW                0
#define i_uart1_CC_UART_ADD_ENCODED_PARAMS    0
#define i_uart1_CC_UART_LATCH_MODE            0
#define i_uart1_CC_UART_ADDR_SLICE_LHS        8
#define i_uart1_CC_UART_COMP_VERSION          0x3330322a

