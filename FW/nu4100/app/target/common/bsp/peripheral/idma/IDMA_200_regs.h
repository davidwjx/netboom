
#ifndef IDMA_200_REGS_H
#define IDMA_200_REGS_H

#define        IDMA_200_BASE                 IDMA_200G_deviceBaseAddress
#define        IDMA_200_ADDRESS_SPAN         (0x2000)

// shared registers offset
#define         IDMA_200_PROC0_STATUS             0x00
#define         IDMA_200_PROC1_STATUS             0x04
#define         IDMA_200_PROC2_STATUS             0x08
#define         IDMA_200_PROC3_STATUS             0x0C
#define         IDMA_200_PROC4_STATUS             0x10
#define         IDMA_200_PROC5_STATUS             0x14
#define         IDMA_200_PROC6_STATUS             0x18
#define         IDMA_200_PROC7_STATUS             0x1C
#define         IDMA_200_CORE0_JOINT              0x30
#define         IDMA_200_CORE1_JOINT              0x34
#define         IDMA_200_CORE0_PRIO               0x38
#define         IDMA_200_CORE1_PRIO               0x3C
#define         IDMA_200_CORE0_CLKDIV             0x40
#define         IDMA_200_CORE1_CLKDIV             0x44
#define         IDMA_200_CORE0_START              0x48
#define         IDMA_200_CORE1_START              0x4C
#define         IDMA_200_PERIPH_RX_CTRL           0x50
#define         IDMA_200_PERIPH_TX_CTRL           0x54

// channel registers offsets
#define         IDMA_200_CMD_LINE0                0x00
#define         IDMA_200_CMD_LINE1                0x04
#define         IDMA_200_CMD_LINE2                0x08
#define         IDMA_200_CMD_LINE3                0x0C
#define         IDMA_200_STATIC_LINE0             0x10
#define         IDMA_200_STATIC_LINE1             0x14
#define         IDMA_200_STATIC_LINE2             0x18
#define         IDMA_200_STATIC_LINE3             0x1C
#define         IDMA_200_STATIC_LINE4             0x20
#define         IDMA_200_STATIC_LINE5             0x24
   
#define         IDMA_200_RESTRICT_REG_OFFSET      0x2C
#define         IDMA_200_RD_OFFSETS               0x30
#define         IDMA_200_WR_OFFSETS               0x34
#define         IDMA_200_FIFO_FULLNESS            0x38
#define         IDMA_200_CMD_OUTS                 0x3C
   
#define         IDMA_200_CH_ENABLE                0x40
#define         IDMA_200_CH_START                 0x44
#define         IDMA_200_CH_ACTIVE                0x48
#define         IDMA_200_CH_CMD_COUNTER           0x50
                                
#define         IDMA_200_INT_RAWSTAT              0xA0
#define         IDMA_200_INT_CLEAR                0xA4
#define         IDMA_200_INT_ENABLE               0xA8
#define         IDMA_200_INT_STATUS               0xAC

// registers fields bit offsets
#define         IDMA_200_RD_START_ADDR_START      0
#define         IDMA_200_WR_START_ADDR_START      0

#define         IDMA_200_BUFF_SIZE_START          0
#define         IDMA_200_X_SIZE_START             0
#define         IDMA_200_Y_SIZE_START             0

#define         IDMA_200_CMD_NEXT_ADDR_START      0
#define         IDMA_200_CMD_LAST_START           1
#define         IDMA_200_CMD_SET_INT_START        0

#define         IDMA_200_RD_BURST_MAX_SIZE_START  0
#define         IDMA_200_RD_TOKENS_START          16
#define         IDMA_200_RD_OUTS_MAX_START        24
#define         IDMA_200_RD_OUTSTANDING_START     30
#define         IDMA_200_RD_INCR_START            31

#define         IDMA_200_WR_BURST_MAX_SIZE_START  0
#define         IDMA_200_WR_TOKENS_START          16
#define         IDMA_200_WR_OUTS_MAX_START        24
#define         IDMA_200_WR_OUTSTANDING_START     30
#define         IDMA_200_WR_INCR_START            31

#define         IDMA_200_FRAME_WIDTH_START        0
#define         IDMA_200_BLOCK_START              15
#define         IDMA_200_JOINT_START              16
#define         IDMA_200_AUTO_RETRY_START         17
#define         IDMA_200_CMD_PORT_NUM_START       20
#define         IDMA_200_RD_PORT_NUM_START        21
#define         IDMA_200_WR_PORT_NUM_START        22
#define         IDMA_200_INT_NUM_START            24
#define         IDMA_200_END_SWAP_START           28

#define         IDMA_200_RD_WAIT_LIMIT_START      0
#define         IDMA_200_WR_WAIT_LIMIT_START      16

#define         IDMA_200_RD_PERIPH_NUM_START      0
#define         IDMA_200_RD_PERIPH_DELAY_START    8
#define         IDMA_200_RD_PERIPH_BLOCK_START    15
#define         IDMA_200_WR_PERIPH_NUM_START      16
#define         IDMA_200_WR_PERIPH_DELAY_START    24
#define         IDMA_200_WR_PERIPH_BLOCK_START    31

#define         IDMA_200_RD_PERIPH_NUM_SINGLE_START   0
#define         IDMA_200_WR_PERIPH_NUM_SINGLE_START   16

#define         IDMA_200_CH_ENABLE_START          0

#define         IDMA_200_CH_START_START           0

#define         IDMA_200_CMD_COUNTER_START        0
#define         IDMA_200_INT_COUNTER_START        16

#define         IDMA_200_INT_PROC_NUM_START       0

#define         IDMA_200_INT_RAW_START            0
#define         IDMA_200_INT_CLR_START            0
#define         IDMA_200_INT_ENABLE_START         0
#define         IDMA_200_INT_STATUS_START         0

#define         IDMA_200_CORE0_PRIO_RD_TOP_NUM_START  0
#define         IDMA_200_CORE0_PRIO_RD_TOP_START      3
#define         IDMA_200_CORE0_PRIO_RD_HIGH_NUM_START 4
#define         IDMA_200_CORE0_PRIO_RD_HIGH_START     7
#define         IDMA_200_CORE0_PRIO_WR_TOP_NUM_START  8
#define         IDMA_200_CORE0_PRIO_WR_TOP_START      11
#define         IDMA_200_CORE0_PRIO_WR_HIGH_NUM_START 12
#define         IDMA_200_CORE0_PRIO_WR_HIGH_START     15

#endif
