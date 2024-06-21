#ifndef IDMA_200_TASKS_H
#define IDMA_200_TASKS_H

#define         IDMA_200_INT_CH_END      0
#define         IDMA_200_INT_RD_SLVERR   1
#define         IDMA_200_INT_WR_SLVERR   2
#define         IDMA_200_INT_RD_DECERR   3
#define         IDMA_200_INT_WR_DECERR   4
#define         IDMA_200_INT_OVERFLOW    5
#define         IDMA_200_INT_UNDERFLOW   6
#define         IDMA_200_INT_TIMEOUT_R   7
#define         IDMA_200_INT_TIMEOUT_AR  8
#define         IDMA_200_INT_TIMEOUT_B   9
#define         IDMA_200_INT_TIMEOUT_W   10
#define         IDMA_200_INT_TIMEOUT_AW  11 
#define         IDMA_200_INT_WDT         12

#define IDMA_200_INDEPENDENT_OPERATION_MODE		0
#define IDMA_200_JOINT_OPERATION_MODE			1

#define IDMA_200_PRIO_DISABLE		0
#define IDMA_200_PRIO_ENABLE		1

#define IDMA_200_DEFAULT_CLK_DIV	1

#define IDMA_200_NULL_ADDR       0
#define IDMA_200_NULL_SIZE       0
#define IDMA_200_INT_FALSE       0
#define IDMA_200_INT_TRUE        1
#define IDMA_200_CMD_LAST_FALSE  0
#define IDMA_200_CMD_LAST_TRUE   1
#define IDMA_200_NULL_DESCRIPTOR_ADDR    0

#define IDMA_200_AXI_DATA_WIDTH	         8   // 8 bytes = 64 bits
#define IDMA_200_MAX_ELEMENTS_IN_BURST   16 // elements of 64 bits
#define IDMA_200_MAX_BURST_WIDTH         (IDMA_200_MAX_ELEMENTS_IN_BURST * IDMA_200_AXI_DATA_WIDTH)   // 1024 bits = 128 bytes = 16 elements of 64 bits
#define IDMA_200_DEFAULT_BURST_WIDTH     IDMA_200_MAX_BURST_WIDTH   // 128

#define IDMA_200_DEFAULT_NUM_OF_TOKENS   1   // 1 burst per AXI access

#define IDMA_200_MAX_OUT_STAND_CMD       4   // 4 AXI commands without ack
#define IDMA_200_DEFAULT_OUT_STAND_CMD   IDMA_200_MAX_OUT_STAND_CMD   // 4 AXI commands without ack

#define IDMA_200_NO_OUTSTANDING			 0
#define IDMA_200_OUTSTANDING			 1

#define IDMA_200_CH_NO_INCR_ADDRESSING   0
#define IDMA_200_CH_INCR_ADDRESSING      1

#define IDMA_200_NO_BLOCK_MODE           0
#define IDMA_200_SET_BLOCK_MODE          1

#define IDMA_200_CH_NO_JOINT_MODE        0
#define IDMA_200_CH_JOINT_MODE           1

#define IDMA_200_CH_NO_AUTO_RETRY        0
#define IDMA_200_CH_AUTO_RETRY           1

#define IDMA_200_NO_END_SWAP             0
#define IDMA_200_END_SWAP_16_BITS        1
#define IDMA_200_END_SWAP_32_BITS        2
#define IDMA_200_END_SWAP_64_BITS        3

#define IDMA_200_DEFAULT_CH_WAIT_LIMIT   0

#define IDMA_200_NO_RD_WR_DELAY          0
#define IDMA_200_DEFAULT_PERIPH_DELAY    1

#define IDMA_200_PERIPH_NO_BLOCK_MODE    0
#define IDMA_200_PERIPH_BLOCK_MODE       1

#define IDMA_200_CHANNEL_START_ALL       0xFF
#define IDMA_200_CHANNEL_STOP_ALL        0

typedef enum {
   IDMA_200_CORE_0_E        = 0,
   IDMA_200_CORE_1_E        = 1,
   IDMA_200_NUM_OF_CORES_E
} IDMA_200_coreTypeE;

typedef enum {
   IDMA_200_CHANNEL_0_E = 0,
   IDMA_200_CHANNEL_1_E = 1,
   IDMA_200_CHANNEL_2_E = 2,
   IDMA_200_CHANNEL_3_E = 3,
   IDMA_200_CHANNEL_4_E = 4,
   IDMA_200_CHANNEL_5_E = 5,
   IDMA_200_CHANNEL_6_E = 6,
   IDMA_200_CHANNEL_7_E = 7,
   IDMA_200_NUM_OF_CORE_CHANNELS_E
} IDMA_200_coreChannelsE;

typedef enum {
   IDMA_200_INT_0_E = 0,
   IDMA_200_INT_1_E = 1,
   IDMA_200_INT_2_E = 2,
   IDMA_200_INT_3_E = 3,
   IDMA_200_INT_4_E = 4,
   IDMA_200_INT_5_E = 5,
   IDMA_200_INT_6_E = 6,
   IDMA_200_INT_7_E = 7,
   IDMA_200_NUM_OF_INTERRUPTS_E
} IDMA_200_interruptListE;

#define IDMA_200_INT_0_NUM  IDMA_200_INT_0_E
#define IDMA_200_INT_1_NUM  IDMA_200_INT_1_E
#define IDMA_200_INT_2_NUM  IDMA_200_INT_2_E
#define IDMA_200_NONE_INT_NUM  IDMA_200_INT_7_E

typedef enum {
   IDMA_200_RX_PERIPH_CLIENT_SRAM_E                 = 0,
   IDMA_200_RX_PERIPH_CLIENT_LOW_RES_FIFO_E         = 1,
   IDMA_200_RX_PERIPH_CLIENT_MED_RES_FIFO_E         = 2,
   IDMA_200_RX_PERIPH_CLIENT_ROI_HEAD_FIFO_E        = 3,
   IDMA_200_RX_PERIPH_CLIENT_ROI_RHAND_FIFO_E       = 4,
   IDMA_200_RX_PERIPH_CLIENT_ROI_LHAND_FIFO_E       = 5,
   IDMA_200_RX_PERIPH_CLIENT_ROI_EXTRA_AREA_FIFO_E  = 6,
   IDMA_200_RX_PERIPH_CLIENT_ROI_EYES_FIFO_E        = 7,
   IDMA_200_RX_PERIPH_CLIENT_COLOR_CAM_FIFO_E       = 8,
   IDMA_200_RX_PERIPH_CLIENT_DISPARITY_FIFO_E       = 9,
   IDMA_200_RX_PERIPH_CLIENT_STATISTICS_FIFO_E      = 10,
   IDMA_200_RX_PERIPH_CLIENT_11_E                   = 11,  // Reserved
   IDMA_200_RX_PERIPH_CLIENT_12_E                   = 12,  // Reserved
   IDMA_200_RX_PERIPH_CLIENT_13_E                   = 13,  // Reserved
   IDMA_200_RX_PERIPH_CLIENT_14_E                   = 14,  // Reserved
   IDMA_200_RX_PERIPH_CLIENT_15_E                   = 15,  // Reserved
   IDMA_200_RX_PERIPH_CLIENT_I2C0_E                 = 16,
   IDMA_200_RX_PERIPH_CLIENT_I2C1_E                 = 17,
   IDMA_200_RX_PERIPH_CLIENT_I2C2_E                 = 18,
   IDMA_200_RX_PERIPH_CLIENT_I2C3_E                 = 19,
   IDMA_200_RX_PERIPH_CLIENT_UART_E                 = 20,
   IDMA_200_RX_PERIPH_CLIENT_FCU_E                  = 21,
   IDMA_200_RX_PERIPH_CLIENT_AAU_E                  = 22,
   IDMA_200_RX_PERIPH_CLIENT_23_E                   = 23,  // Reserved
   IDMA_200_RX_PERIPH_CLIENT_24_E                   = 24,  // Reserved
   IDMA_200_RX_PERIPH_CLIENT_25_E                   = 25,  // Reserved
   IDMA_200_RX_PERIPH_CLIENT_26_E                   = 26,  // Reserved
   IDMA_200_RX_PERIPH_CLIENT_27_E                   = 27,  // Reserved
   IDMA_200_RX_PERIPH_CLIENT_28_E                   = 28,  // Reserved
   IDMA_200_RX_PERIPH_CLIENT_29_E                   = 29,  // Reserved
   IDMA_200_RX_PERIPH_CLIENT_30_E                   = 30,  // Reserved
   IDMA_200_RX_PERIPH_CLIENT_INT3_E                 = 31,
   IDMA_200_NUM_OF_RX_PHERIPH_CLIENTS_E
} IDMA_200_rxPeriphClientIdE;

typedef enum {
   IDMA_200_TX_PERIPH_CLIENT_SRAM_E             = 0,
   IDMA_200_TX_PERIPH_CLIENT_IAE_INJECT_FIFO_E  = 1,
   IDMA_200_TX_PERIPH_CLIENT_IAE_MIPI_E         = 2,
   IDMA_200_TX_PERIPH_CLIENT_3_E                = 3,  // Reserved
   IDMA_200_TX_PERIPH_CLIENT_4_E                = 4,  // Reserved
   IDMA_200_TX_PERIPH_CLIENT_5_E                = 5,  // Reserved
   IDMA_200_TX_PERIPH_CLIENT_6_E                = 6,  // Reserved
   IDMA_200_TX_PERIPH_CLIENT_7_E                = 7,  // Reserved
   IDMA_200_TX_PERIPH_CLIENT_8_E                = 8,  // Reserved
   IDMA_200_TX_PERIPH_CLIENT_9_E                = 9,  // Reserved
   IDMA_200_TX_PERIPH_CLIENT_10_E               = 10, // Reserved
   IDMA_200_TX_PERIPH_CLIENT_11_E               = 11, // Reserved
   IDMA_200_TX_PERIPH_CLIENT_12_E               = 12, // Reserved
   IDMA_200_TX_PERIPH_CLIENT_13_E               = 13, // Reserved
   IDMA_200_TX_PERIPH_CLIENT_14_E               = 14, // Reserved
   IDMA_200_TX_PERIPH_CLIENT_15_E               = 15, // Reserved
   IDMA_200_TX_PERIPH_CLIENT_I2C0_E             = 16,
   IDMA_200_TX_PERIPH_CLIENT_I2C1_E             = 17,
   IDMA_200_TX_PERIPH_CLIENT_I2C2_E             = 18,
   IDMA_200_TX_PERIPH_CLIENT_I2C3_E             = 19,
   IDMA_200_TX_PERIPH_CLIENT_UART0_E            = 20,
   IDMA_200_TX_PERIPH_CLIENT_FCU_E              = 21,
   IDMA_200_TX_PERIPH_CLIENT_AAU_I2S_MASTER_E   = 22,
   IDMA_200_TX_PERIPH_CLIENT_AAU_I2S_SLAVE_E    = 23,
   IDMA_200_TX_PERIPH_CLIENT_UART1_E            = 24,
   IDMA_200_TX_PERIPH_CLIENT_UART2_E            = 25,
   IDMA_200_TX_PERIPH_CLIENT_26_E               = 26, // Reserved
   IDMA_200_TX_PERIPH_CLIENT_27_E               = 27, // Reserved
   IDMA_200_TX_PERIPH_CLIENT_28_E               = 28, // Reserved
   IDMA_200_TX_PERIPH_CLIENT_29_E               = 29, // Reserved
   IDMA_200_TX_PERIPH_CLIENT_30_E               = 30, // Reserved
   IDMA_200_TX_PERIPH_CLIENT_INT3_E             = 31,
   IDMA_200_NUM_OF_TX_PHERIPH_CLIENTS_E
} IDMA_200_txPeriphClientIdE;

typedef struct {
   unsigned int x_size:12;    //transfer width in bytes
   unsigned int y_size:13;    //transfer height in bytes
   unsigned int reserved0:7; 
} IDMA_200_blockTransferSize;

typedef struct {
   unsigned int buffer_size:25;    //transfer height in bytes
   unsigned int reserved0:7; 
} IDMA_200_noneBlockTransferSize;

typedef union {
   unsigned int                     val;
   IDMA_200_noneBlockTransferSize   noneBlock;
   IDMA_200_blockTransferSize       block;
} IDMA_200_transferSize;

typedef struct {
    unsigned int set_int:1;    //set interrupt on command completion
    unsigned int last:1;       //last command in list
    unsigned int reserved1:2;  
    unsigned int next_addr:28; //next command address [units of 16 bytes]
} IDMA_200_transferOptionsT;

typedef union {
   unsigned int                val;
   IDMA_200_transferOptionsT   fields;
} IDMA_200_transferOptionsU;

typedef struct {
   unsigned int              rd_addr:32;   //read start address
   unsigned int              wr_addr:32;   //write start address
   IDMA_200_transferSize     transferSize;
   IDMA_200_transferOptionsU transferOpt;
}IDMA_200_cmd;

void IDMA_200_initBaseAddress(unsigned int dmaRegistersBaseAddr);
int IDMA_200_ch_reg_addr(int reg_addr, int core_num, int ch_num); //get full address of channel register
int IDMA_200_shared_reg_addr(int reg_addr); //get full address of general register

 void IDMA_200_config_core_mode(int core_num, int joint); //set independent mode ot joint mode per core 

 void IDMA_200_config_core_prio(int core_num, int prio_rd_top, int prio_rd_top_num, int prio_rd_high, int prio_rd_high_num, int prio_wr_top, int prio_wr_top_num, int prio_wr_high, int prio_wr_high_num); 

 void IDMA_200_config_core_clkdiv(int core_num, int clkdiv); //set core 1 clock divider value 

void write_reg(int addr, int wdata);
int read_reg(int addr);

//configure static registers per channel
void IDMA_200_config_static0(int core_num, int ch_num, int rd_burst_max, int rd_tokens, int rd_outs_max, int rd_outstanding, int rd_incr);
void IDMA_200_config_static1(int core_num, int ch_num, int wr_burst_max, int wr_tokens, int wr_outs_max, int wr_outstanding, int wr_incr); 
void IDMA_200_config_static2(int core_num, int ch_num, int frame_width, int block, int joint, int auto_retry, int cmd_port_num, int rd_port_num, int wr_port_num, int int_num, int end_swap);
void IDMA_200_config_static3(int core_num, int ch_num, int rd_wait_limit, int wr_wait_limit); 
void IDMA_200_config_static4(int core_num, int ch_num, int rd_periph_num, int rd_periph_delay, int rd_periph_block, int wr_periph_num, int wr_periph_delay, int wr_periph_block); 
void IDMA_200_config_static5(int core_num, int ch_num, int rd_periph_num_single, int wr_periph_num_single);

//configure first command into command registers per core
void IDMA_200_config_cmd_regs(int core_num, int ch_num, int rd_addr, int wr_addr, int buff_size, int list_addr, int set_int, int last);
void IDMA_200_config_cmd_null(int core_num, int ch_num, int list_addr);
int IDMA_200_get_cmd_rd_addr(int core_num, int ch_num);
int IDMA_200_get_cmd_wr_addr(int core_num, int ch_num);
int IDMA_200_get_cmd_size(int core_num, int ch_num);
int IDMA_200_get_cmd_options(int core_num, int ch_num);

//start channel
void IDMA_200_ch_start(int core_num, int ch_num);
void IDMA_200_ch_start_all(int core_num, int ch_start_mask);

//pause & resume channel
void IDMA_200_ch_pause(int core_num, int ch_num);
void IDMA_200_ch_resume(int core_num, int ch_num);
int IDMA_200_get_ch_left_cmd(int core_num, int ch_num);

//interrupt handler (int_num is interrupt bit used by the handling processor)
int IDMA_200_get_int_ch(int int_num);
int IDMA_200_int_handler_case(int core_num, int ch_num, int int_stat);
int IDMA_200_int_handler(int int_num);

void IDMA_200_interrupts_init(int core_num, int ch_num);
void IDMA_200_interrupt_enable(int core_num, int ch_num, int int_bit);
int IDMA_200_interrupts_enable_get(int core_num, int ch_num);
void IDMA_200_interrupts_enable_set(int core_num, int ch_num, int int_mask);
int IDMA_200_get_int_status(int core_num, int ch_num, int int_bit);
void IDMA_200_clear_int_status(int core_num, int ch_num, int int_bit);
int IDMA_200_get_ch_status(int core_num, int ch_num);
void IDMA_200_clear_ch_status(int core_num, int ch_num, int clear);
int IDMA_200_get_ch_raw_status(int core_num, int ch_num);
int IDMA_200_get_end_transfer_int_cntr(int core_num, int ch_num);
int IDMA_200_get_chan_buffer_cntr(int core_num, int ch_num);
int IDMA_200_get_chan_active(int core_num, int ch_num);


// debug functions
void IDMA_200_print_channel_regs(int core_num, int ch_num);
void IDMA_200_print_core_general_regs();

#endif
