
#include "IDMA_200_tasks.h"
#include "IDMA_200_regs.h"
#include "stdio.h"


static unsigned int IDMA_200G_deviceBaseAddress;

void IDMA_200_initBaseAddress(unsigned int dmaRegistersBaseAddr)
{
   IDMA_200G_deviceBaseAddress = dmaRegistersBaseAddr;
}

int IDMA_200_ch_reg_addr(int reg_addr, int core_num, int ch_num)
{
    return (IDMA_200_BASE + reg_addr + (core_num << 11) + (ch_num << 8));
}

int IDMA_200_shared_reg_addr(int reg_addr)
{
    return (IDMA_200_BASE + (1 << 12) + reg_addr);
}

void write_reg(int addr, int wdata)
{
   volatile int *reg_addr = (volatile int *)addr;
   *reg_addr = wdata;
}

int read_reg(int addr)
{
   volatile int *reg_addr = (volatile int *)addr;
   return (*reg_addr);
}

void IDMA_200_config_static0(int core_num       ,
                     int ch_num         ,
                     int rd_burst_max   ,
                          int rd_tokens      ,
                      int rd_outs_max    ,
                      int rd_outstanding ,
                      int rd_incr)
{

      int wdata = 
    rd_burst_max   << IDMA_200_RD_BURST_MAX_SIZE_START |
    rd_tokens      << IDMA_200_RD_TOKENS_START         |
    rd_outs_max    << IDMA_200_RD_OUTS_MAX_START       |
    rd_outstanding << IDMA_200_RD_OUTSTANDING_START    |
    rd_incr        << IDMA_200_RD_INCR_START;

      write_reg(IDMA_200_ch_reg_addr(IDMA_200_STATIC_LINE0, core_num, ch_num), wdata);
}

void IDMA_200_config_static1(
                       int core_num       ,
                     int ch_num         ,
                     int wr_burst_max   ,
                          int wr_tokens      ,
                      int wr_outs_max    ,
                      int wr_outstanding ,
                      int wr_incr)
{

      int wdata = 
    wr_burst_max   << IDMA_200_WR_BURST_MAX_SIZE_START |
    wr_tokens      << IDMA_200_WR_TOKENS_START         |
    wr_outs_max    << IDMA_200_WR_OUTS_MAX_START       |
    wr_outstanding << IDMA_200_WR_OUTSTANDING_START    |
    wr_incr        << IDMA_200_WR_INCR_START;

      write_reg(IDMA_200_ch_reg_addr(IDMA_200_STATIC_LINE1, core_num, ch_num), wdata);
}
    
void IDMA_200_config_static2(      int core_num       ,
                     int ch_num         ,
                 int frame_width    ,
                 int block          ,
                 int joint          ,
                      int auto_retry     ,
                      int cmd_port_num   ,
                       int rd_port_num    ,
                       int wr_port_num    ,
                 int int_num        ,
                 int end_swap)
{

      int wdata = 
    frame_width   << IDMA_200_FRAME_WIDTH_START  |
    block         << IDMA_200_BLOCK_START        |
    joint         << IDMA_200_JOINT_START        |
    auto_retry    << IDMA_200_AUTO_RETRY_START   |
    cmd_port_num  << IDMA_200_CMD_PORT_NUM_START |
    rd_port_num   << IDMA_200_RD_PORT_NUM_START  |
    wr_port_num   << IDMA_200_WR_PORT_NUM_START  |
        int_num       << IDMA_200_INT_NUM_START      |
        end_swap      << IDMA_200_END_SWAP_START;
    
      write_reg(IDMA_200_ch_reg_addr(IDMA_200_STATIC_LINE2, core_num, ch_num), wdata);    
}
        
void IDMA_200_config_static3(int core_num       ,
                     int ch_num         ,
                                 int rd_wait_limit  ,
                                 int wr_wait_limit)
{

      int wdata = 
    rd_wait_limit << IDMA_200_RD_WAIT_LIMIT_START |
    wr_wait_limit << IDMA_200_WR_WAIT_LIMIT_START;
    
      write_reg(IDMA_200_ch_reg_addr(IDMA_200_STATIC_LINE3, core_num, ch_num), wdata);
}
    
void IDMA_200_config_static4(int core_num        ,
                     int ch_num          ,
                                 int rd_periph_num   ,
                                 int rd_periph_delay ,
                                 int rd_periph_block ,
                                 int wr_periph_num   ,
                                 int wr_periph_delay ,
                                 int wr_periph_block)
{

    int wdata = 
    rd_periph_num   << IDMA_200_RD_PERIPH_NUM_START |
    rd_periph_delay << IDMA_200_RD_PERIPH_DELAY_START |
    rd_periph_block << IDMA_200_RD_PERIPH_BLOCK_START |
    wr_periph_num   << IDMA_200_WR_PERIPH_NUM_START |
    wr_periph_delay << IDMA_200_WR_PERIPH_DELAY_START |
    wr_periph_block << IDMA_200_WR_PERIPH_BLOCK_START;
    
      write_reg(IDMA_200_ch_reg_addr(IDMA_200_STATIC_LINE4, core_num, ch_num), wdata);
}
        
void IDMA_200_config_static5(int core_num,
                             int ch_num,
                             int rd_periph_num_single,
                             int wr_periph_num_single)
{

   int wdata = rd_periph_num_single << IDMA_200_RD_PERIPH_NUM_SINGLE_START |
               wr_periph_num_single << IDMA_200_WR_PERIPH_NUM_SINGLE_START;
    
   write_reg(IDMA_200_ch_reg_addr(IDMA_200_STATIC_LINE5, core_num, ch_num), wdata);
}

void IDMA_200_ch_pause(int core_num,
                       int ch_num)
{
   write_reg(IDMA_200_ch_reg_addr(IDMA_200_CH_ENABLE, core_num, ch_num), 0);
}    

void IDMA_200_ch_resume(int core_num,
                        int ch_num)
{
   write_reg(IDMA_200_ch_reg_addr(IDMA_200_CH_ENABLE, core_num, ch_num), 1);
}

void IDMA_200_ch_start(int core_num, int ch_num)
{
   write_reg(IDMA_200_ch_reg_addr(IDMA_200_CH_START, core_num, ch_num), 1);
}

int IDMA_200_get_ch_left_cmd(int core_num,
                             int ch_num)
{
   int left_cmd = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_OUTS, core_num, ch_num));
   return left_cmd;
}    

void IDMA_200_ch_start_all(int core_num        ,
                           int ch_start_mask)
{
      if (core_num == 0)
      {
          write_reg(IDMA_200_shared_reg_addr(IDMA_200_CORE0_START), ch_start_mask);
      }
     else
      {
          write_reg(IDMA_200_shared_reg_addr(IDMA_200_CORE1_START), ch_start_mask);
      }
}

void IDMA_200_config_core_mode(int core_num        ,
                                   int joint)
{
      if (core_num == 0)
      {
          write_reg(IDMA_200_shared_reg_addr(IDMA_200_CORE0_JOINT), joint);
      } else
      {
          write_reg(IDMA_200_shared_reg_addr(IDMA_200_CORE1_JOINT), joint);
      }
}

void IDMA_200_config_core_prio(int core_num, 
                int prio_rd_top, 
                int prio_rd_top_num, 
                int prio_rd_high, 
                int prio_rd_high_num, 
                int prio_wr_top, 
                int prio_wr_top_num, 
                int prio_wr_high, 
                int prio_wr_high_num
                )
{

      int wdata = 
                prio_rd_top      << IDMA_200_CORE0_PRIO_RD_TOP_START      | 
                prio_rd_top_num  << IDMA_200_CORE0_PRIO_RD_TOP_NUM_START  | 
                prio_rd_high     << IDMA_200_CORE0_PRIO_RD_HIGH_START     | 
                prio_rd_high_num << IDMA_200_CORE0_PRIO_RD_HIGH_NUM_START | 
                prio_wr_top      << IDMA_200_CORE0_PRIO_WR_TOP_START      | 
                prio_wr_top_num  << IDMA_200_CORE0_PRIO_WR_TOP_NUM_START  | 
                prio_wr_high     << IDMA_200_CORE0_PRIO_WR_HIGH_START     | 
                prio_wr_high_num << IDMA_200_CORE0_PRIO_WR_HIGH_NUM_START;

      if (core_num == 0)
      {
          write_reg(IDMA_200_shared_reg_addr(IDMA_200_CORE0_PRIO), wdata);
      } else
      {
          write_reg(IDMA_200_shared_reg_addr(IDMA_200_CORE1_PRIO), wdata);
      }
}
    
void IDMA_200_config_cmd_regs(int  core_num  ,
                            int  ch_num    ,
                            int  rd_addr   ,
                            int  wr_addr   ,
                            int  buff_size ,
                            int  list_addr ,
                            int  set_int   ,
                            int  last)
{
  
      if (buff_size != 0)
      {
      write_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE0, core_num, ch_num), rd_addr << IDMA_200_RD_START_ADDR_START);
      write_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE1, core_num, ch_num), wr_addr << IDMA_200_WR_START_ADDR_START);
      }
      write_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE2, core_num, ch_num), buff_size << IDMA_200_BUFF_SIZE_START);
      write_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE3, core_num, ch_num), 
        ((list_addr << IDMA_200_CMD_NEXT_ADDR_START) & 0xFFFFFFF0) | (last << IDMA_200_CMD_LAST_START) | (set_int << IDMA_200_CMD_SET_INT_START));
}

int IDMA_200_get_cmd_rd_addr(int core_num, int ch_num)
{
   return read_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE0, core_num, ch_num));
}

int IDMA_200_get_cmd_wr_addr(int core_num, int ch_num)
{
   return read_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE1, core_num, ch_num));
}

int IDMA_200_get_cmd_size(int core_num, int ch_num)
{
   return read_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE2, core_num, ch_num));
}

int IDMA_200_get_cmd_options(int core_num, int ch_num)
{
   return read_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE3, core_num, ch_num));
}

void IDMA_200_config_cmd_null(int  core_num  ,
                            int  ch_num    ,
                int  list_addr)
{
    IDMA_200_config_cmd_regs(core_num, ch_num, 0, 0, 0, list_addr, 0, 0);
}

void IDMA_200_config_core_clkdiv(int  core_num  ,int clkdiv)
{
      if (core_num == 0)
      {
         write_reg(IDMA_200_shared_reg_addr(IDMA_200_CORE0_CLKDIV), clkdiv);
      } else
      {
         write_reg(IDMA_200_shared_reg_addr(IDMA_200_CORE1_CLKDIV), clkdiv);
      }
}
        
int IDMA_200_get_int_ch(int int_num)
{
      int core_num;
      int ch_num;
      
      int rdata = read_reg(IDMA_200_shared_reg_addr(IDMA_200_PROC0_STATUS + int_num*4));

      if (rdata & (1 << 0)) {
         core_num = 0;
         ch_num = 0;
      } else if (rdata & (1 << 1)) {
         core_num = 0;
         ch_num = 1;
      } else if (rdata & (1 << 2)) {
         core_num = 0;
         ch_num = 2;
      } else if (rdata & (1 << 3)) {
         core_num = 0;
         ch_num = 3;
      } else if (rdata & (1 << 4)) {
         core_num = 0;
         ch_num = 4;
      } else if (rdata & (1 << 5)) {
         core_num = 0;
         ch_num = 5;
      } else if (rdata & (1 << 6)) {
         core_num = 0;
         ch_num = 6;
      } else if (rdata & (1 << 7)) {
         core_num = 0;
         ch_num = 7;
      } else if (rdata & (1 << 8)) {
         core_num = 1;
         ch_num = 0;
      } else if (rdata & (1 << 9)) {
         core_num = 1;
         ch_num = 1;
      } else if (rdata & (1 << 10)) {
         core_num = 1;
         ch_num = 2;
      } else if (rdata & (1 << 11)) {
         core_num = 1;
         ch_num = 3;
      } else if (rdata & (1 << 12)) {
         core_num = 1;
         ch_num = 4;
      } else if (rdata & (1 << 13)) {
         core_num = 1;
         ch_num = 5;
      } else if (rdata & (1 << 14)) {
         core_num = 1;
         ch_num = 6;
      } else if (rdata & (1 << 15)) {
         core_num = 1;
         ch_num = 7;
      } else {
     //Error - interrupt set but status is 0
         core_num = 8;
         ch_num = 8;
      }
     
      return ((core_num << 4) + ch_num);
}


int IDMA_200_int_handler_case(int core_num, int ch_num, int int_stat)
{

      if (int_stat & (1 << IDMA_200_INT_CH_END)) {
     //Received end interrupt
     write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_CLEAR, core_num, ch_num), (1 << IDMA_200_INT_CH_END));
      } 
     if (int_stat & (1 << IDMA_200_INT_RD_SLVERR)) {
     //Received read slave error interrupt
     write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_CLEAR, core_num, ch_num), (1 << IDMA_200_INT_RD_SLVERR));
      } 
     if (int_stat & (1 << IDMA_200_INT_WR_SLVERR)) {
     //Received write slave error interrupt
     write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_CLEAR, core_num, ch_num), (1 << IDMA_200_INT_WR_SLVERR));
      } 
     if (int_stat & (1 << IDMA_200_INT_RD_DECERR)) {
     //Received read decode error interrupt
     write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_CLEAR, core_num, ch_num), (1 << IDMA_200_INT_RD_DECERR));
      } 
     if (int_stat & (1 << IDMA_200_INT_WR_DECERR)) {
     //Received write decode error interrupt
     write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_CLEAR, core_num, ch_num), (1 << IDMA_200_INT_WR_DECERR));
      } 
     if (int_stat & (1 << IDMA_200_INT_OVERFLOW)) {
     //Received fifo overflow interrupt
     write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_CLEAR, core_num, ch_num), (1 << IDMA_200_INT_OVERFLOW));
      } 
     if (int_stat & (1 << IDMA_200_INT_UNDERFLOW)) {
     //Received underflow interrupt
     write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_CLEAR, core_num, ch_num), (1 << IDMA_200_INT_UNDERFLOW));
      } 
     if (int_stat & (1 << IDMA_200_INT_TIMEOUT_R)) {
     //Received read data timeout interrupt
     write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_CLEAR, core_num, ch_num), (1 << IDMA_200_INT_TIMEOUT_R));
      } 
     if (int_stat & (1 << IDMA_200_INT_TIMEOUT_AR)) {
     //Received read command timeout interrupt
     write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_CLEAR, core_num, ch_num), (1 << IDMA_200_INT_TIMEOUT_AR));
      } 
     if (int_stat & (1 << IDMA_200_INT_TIMEOUT_B)) {
     //Received write response timeout interrupt
     write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_CLEAR, core_num, ch_num), (1 << IDMA_200_INT_TIMEOUT_B));
      } 
     if (int_stat & (1 << IDMA_200_INT_TIMEOUT_W)) {
     //Received write data timeout interrupt
     write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_CLEAR, core_num, ch_num), (1 << IDMA_200_INT_TIMEOUT_W));
      } 
     if (int_stat & (1 << IDMA_200_INT_TIMEOUT_AW)) {
     //Received write command timeout interrupt
     write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_CLEAR, core_num, ch_num), (1 << IDMA_200_INT_TIMEOUT_AW));
      } 
     if (int_stat & (1 << IDMA_200_INT_WDT)) {
     //Received watchdog timeout interrupt
     write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_CLEAR, core_num, ch_num), (1 << IDMA_200_INT_WDT));
     } else {
     //Error - interrupt set but status is 0
     return 1;
     }


      return 0;
}
            

int IDMA_200_int_handler(int int_num)
{
      int rdata;
      int core_num;
      int ch_num;

      //get channel
      rdata    = IDMA_200_get_int_ch(int_num);
      core_num = rdata & 0x8;
      ch_num   = rdata & 0x7;

      //get interrupt status
      rdata = read_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_STATUS, core_num, ch_num));

      //handle interrupt
      IDMA_200_int_handler_case(core_num, int_num, rdata);

      return 0;

}


void IDMA_200_interrupts_init(int core_num, int ch_num)
{
   write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_ENABLE, core_num, ch_num), 0);
   write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_RAWSTAT, core_num, ch_num), 0);
}

void IDMA_200_interrupt_enable(int core_num, int ch_num, int int_bit)
{
   int wrdata = (1 << int_bit);
   write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_ENABLE, core_num, ch_num), wrdata);
}

int IDMA_200_interrupts_enable_get(int core_num, int ch_num)
{
   int rdata = read_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_ENABLE, core_num, ch_num));
   return rdata;
}

void IDMA_200_interrupts_enable_set(int core_num, int ch_num, int int_mask)
{
   write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_ENABLE, core_num, ch_num), int_mask);
}


int IDMA_200_get_int_status(int core_num, int ch_num, int int_bit)
{
   int rdata = read_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_STATUS, core_num, ch_num));
   rdata = (rdata >> int_bit) & 1;
   return rdata;
}

void IDMA_200_clear_int_status(int core_num, int ch_num, int int_bit)
{
   int wrdata = (1 << int_bit);
   write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_CLEAR, core_num, ch_num), wrdata);
}

int IDMA_200_get_ch_status(int core_num, int ch_num)
{
   int rdata = read_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_STATUS, core_num, ch_num));
   return rdata;
}

void IDMA_200_clear_ch_status(int core_num, int ch_num, int clear)
{
   write_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_CLEAR, core_num, ch_num), clear);
}

int IDMA_200_get_ch_raw_status(int core_num, int ch_num)
{
   return read_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_RAWSTAT, core_num, ch_num));
}


int IDMA_200_get_chan_buffer_cntr(int core_num, int ch_num)
{
   int buf_count = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CH_CMD_COUNTER, core_num, ch_num));
   buf_count = (buf_count) & 0xFFFF;
   return buf_count;
}

int IDMA_200_get_end_transfer_int_cntr(int core_num, int ch_num)
{
   int int_count = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CH_CMD_COUNTER, core_num, ch_num));
   int_count = (int_count >> IDMA_200_INT_COUNTER_START) & 0x3F;
   return int_count;
}

int IDMA_200_get_chan_active(int core_num, int ch_num)
{
   return read_reg(IDMA_200_ch_reg_addr(IDMA_200_CH_ACTIVE, core_num, ch_num));
}


void IDMA_200_print_channel_regs(int core_num, int ch_num)
{
   int  regVal;

   printf("print core %d channel %d registers:\n", core_num, ch_num);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE0, core_num, ch_num));
   printf("IDMA_200_CMD_LINE0    in address 0x%X = 0x%X\n", (IDMA_200_CMD_LINE0), regVal);
      
   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE1, core_num, ch_num));
   printf("IDMA_200_CMD_LINE1    in address 0x%X = 0x%X\n", (IDMA_200_CMD_LINE1), regVal);
      
   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE2, core_num, ch_num));
   printf("IDMA_200_CMD_LINE2    in address 0x%X = 0x%X\n", (IDMA_200_CMD_LINE2), regVal);
      
   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE3, core_num, ch_num));
   printf("IDMA_200_CMD_LINE3    in address 0x%X = 0x%X\n", (IDMA_200_CMD_LINE3), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_STATIC_LINE0, core_num, ch_num));
   printf("IDMA_200_STATIC_LINE0 in address 0x%X = 0x%X\n", (IDMA_200_STATIC_LINE0), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_STATIC_LINE1, core_num, ch_num));
   printf("IDMA_200_STATIC_LINE1 in address 0x%X = 0x%X\n", (IDMA_200_STATIC_LINE1), regVal);
      
   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_STATIC_LINE2, core_num, ch_num));
   printf("IDMA_200_STATIC_LINE2 in address 0x%X = 0x%X\n", (IDMA_200_STATIC_LINE2), regVal);
      
   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_STATIC_LINE3, core_num, ch_num));
   printf("IDMA_200_STATIC_LINE3 in address 0x%X = 0x%X\n", (IDMA_200_STATIC_LINE3), regVal);
      
   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_STATIC_LINE4, core_num, ch_num));
   printf("IDMA_200_STATIC_LINE4 in address 0x%X = 0x%X\n", (IDMA_200_STATIC_LINE4), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_RESTRICT_REG_OFFSET, core_num, ch_num));
   printf("IDMA_200_RESTRICT_REG in address 0x%X = 0x%X\n", (IDMA_200_RESTRICT_REG_OFFSET), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_RD_OFFSETS, core_num, ch_num));
   printf("IDMA_200_RD_OFFSETS   in address 0x%X = 0x%X\n", (IDMA_200_RD_OFFSETS), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_WR_OFFSETS, core_num, ch_num));
   printf("IDMA_200_WR_OFFSETS   in address 0x%X = 0x%X\n", (IDMA_200_WR_OFFSETS), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_FIFO_FULLNESS, core_num, ch_num));
   printf("IDMA_200_FIFO_FULLNES in address 0x%X = 0x%X\n", (IDMA_200_FIFO_FULLNESS), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_OUTS, core_num, ch_num));
   printf("IDMA_200_CMD_OUTS     in address 0x%X = 0x%X\n", (IDMA_200_CMD_OUTS), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CH_ENABLE, core_num, ch_num));
   printf("IDMA_200_CH_ENABLE    in address 0x%X = 0x%X\n", (IDMA_200_CH_ENABLE), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CH_ACTIVE, core_num, ch_num));
   printf("IDMA_200_CH_ACTIVE    in address 0x%X = 0x%X\n", (IDMA_200_CH_ACTIVE), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CH_CMD_COUNTER, core_num, ch_num));
   printf("IDMA_200_CH_CMD_COUNT in address 0x%X = 0x%X\n", (IDMA_200_CH_CMD_COUNTER), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_RAWSTAT, core_num, ch_num));
   printf("IDMA_200_INT_RAWSTAT  in address 0x%X = 0x%X\n", (IDMA_200_INT_RAWSTAT), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_ENABLE, core_num, ch_num));
   printf("IDMA_200_INT_ENABLE   in address 0x%X = 0x%X\n", (IDMA_200_INT_ENABLE), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_STATUS, core_num, ch_num));
   printf("IDMA_200_INT_STATUS   in address 0x%X = 0x%X\n", (IDMA_200_INT_STATUS), regVal);

}

void IDMA_200_print_channel_status_regs(int core_num, int ch_num)
{
   int  regVal;

   printf("print core %d channel %d registers:\n", core_num, ch_num);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE0, core_num, ch_num));
   printf("IDMA_200_CMD_LINE0 @ %x =%x\n", (IDMA_200_CMD_LINE0), regVal);
      
   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE1, core_num, ch_num));
   printf("IDMA_200_CMD_LINE1 @ %x =%x\n", (IDMA_200_CMD_LINE1), regVal);
      
   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE2, core_num, ch_num));
   printf("IDMA_200_CMD_LINE2 @ %x =%x\n", (IDMA_200_CMD_LINE2), regVal);
      
   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_LINE3, core_num, ch_num));
   printf("IDMA_200_CMD_LINE3 @ %x =%x\n", (IDMA_200_CMD_LINE3), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CMD_OUTS, core_num, ch_num));
   printf("IDMA_200_CMD_OUTS  @ %x =%x\n", (IDMA_200_CMD_OUTS), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CH_ENABLE, core_num, ch_num));
   printf("IDMA_200_CH_ENABLE @ %x =%x\n", (IDMA_200_CH_ENABLE), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CH_ACTIVE, core_num, ch_num));
   printf("IDMA_200_CH_ACTIVE @ %x =%x\n", (IDMA_200_CH_ACTIVE), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_CH_CMD_COUNTER, core_num, ch_num));
   printf("IDMA_200_CH_CMD_COUNT @  %x =%x\n", (IDMA_200_CH_CMD_COUNTER), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_RAWSTAT, core_num, ch_num));
   printf("IDMA_200_INT_RAWSTAT @ %x =%x\n", (IDMA_200_INT_RAWSTAT), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_ENABLE, core_num, ch_num));
   printf("IDMA_200_INT_ENABLE @ %x =%x\n", (IDMA_200_INT_ENABLE), regVal);

   regVal = read_reg(IDMA_200_ch_reg_addr(IDMA_200_INT_STATUS, core_num, ch_num));
   printf("IDMA_200_INT_STATUS @ %x =%x\n", (IDMA_200_INT_STATUS), regVal);

}

void IDMA_200_print_core_general_regs()
{
   int  regVal;

   printf("print dma shared registers\n");

   regVal = read_reg(IDMA_200_shared_reg_addr(IDMA_200_PROC0_STATUS));
   printf("IDMA_200_PROC0_STATUS @ %x =%x\n", (IDMA_200_PROC0_STATUS), regVal);
#if 0
   regVal = read_reg(IDMA_200_shared_reg_addr(IDMA_200_PROC1_STATUS));
   printf("IDMA_200_PROC1_STATUS @ %x =%x\n", (IDMA_200_PROC1_STATUS), regVal);////

   regVal = read_reg(IDMA_200_shared_reg_addr(IDMA_200_PROC2_STATUS));
   printf("IDMA_200_PROC2_STATUS @ %x =%x\n", (IDMA_200_PROC2_STATUS), regVal);///

   regVal = read_reg(IDMA_200_shared_reg_addr(IDMA_200_PROC3_STATUS));
   printf("IDMA_200_PROC3_STATUS@ %x =%x\n", (IDMA_200_PROC3_STATUS), regVal);

   regVal = read_reg(IDMA_200_shared_reg_addr(IDMA_200_PROC4_STATUS));
   printf("IDMA_200_PROC4_STATUS @ %x =%x\n", (IDMA_200_PROC4_STATUS), regVal);

   regVal = read_reg(IDMA_200_shared_reg_addr(IDMA_200_PROC5_STATUS));
   printf("IDMA_200_PROC5_STATUS @ %x =%x\n", (IDMA_200_PROC5_STATUS), regVal);

   regVal = read_reg(IDMA_200_shared_reg_addr(IDMA_200_PROC6_STATUS));
   printf("IDMA_200_PROC6_STATUS @ %x =%x\n", (IDMA_200_PROC6_STATUS), regVal);

   regVal = read_reg(IDMA_200_shared_reg_addr(IDMA_200_PROC7_STATUS));
   printf("IDMA_200_PROC7_STATUS @ %x =%x\n", (IDMA_200_PROC7_STATUS), regVal);

   regVal = read_reg(IDMA_200_shared_reg_addr(IDMA_200_PROC0_STATUS));
   printf("IDMA_200_PROC0_STATUS @ %x =%x\n", (IDMA_200_PROC0_STATUS), regVal);
#endif

   regVal = read_reg(IDMA_200_shared_reg_addr(IDMA_200_CORE0_JOINT));
   printf("IDMA_200_CORE0_JOINT @ %x =%x\n", (IDMA_200_CORE0_JOINT), regVal);

   regVal = read_reg(IDMA_200_shared_reg_addr(IDMA_200_CORE1_JOINT));
   printf("IDMA_200_CORE1_JOINT @ %x =%x\n", (IDMA_200_CORE1_JOINT), regVal);

   regVal = read_reg(IDMA_200_shared_reg_addr(IDMA_200_CORE0_PRIO));
   printf("IDMA_200_CORE0_PRIO @ %x =%x\n", (IDMA_200_CORE0_PRIO), regVal);

   regVal = read_reg(IDMA_200_shared_reg_addr(IDMA_200_CORE1_PRIO));
   printf("IDMA_200_CORE1_PRIO @ %x =%x\n", (IDMA_200_CORE1_PRIO), regVal);

   regVal = read_reg(IDMA_200_shared_reg_addr(IDMA_200_CORE0_CLKDIV));
   printf("IDMA_200_CORE0_CLKDIV @ %x =%x\n", (IDMA_200_CORE0_CLKDIV), regVal);

   regVal = read_reg(IDMA_200_shared_reg_addr(IDMA_200_CORE1_CLKDIV));
   printf("IDMA_200_CORE1_CLKDIV @ %x =%x\n", (IDMA_200_CORE1_CLKDIV), regVal);

}
