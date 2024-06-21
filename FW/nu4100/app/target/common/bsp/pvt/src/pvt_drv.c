/****************************************************************************
 *
 *   FileName: pvt_drv.c
 *
 *   Author:   Igal K.
 *
 *   Date:
 *
 *   Description: Inuitive PVT Driver
 *
 ****************************************************************************/


/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"
#ifdef __cplusplus
      extern "C" {
#endif

#include "nu4100_regs.h"
#include "os_lyr.h"
#include "mem_map.h"


/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define CALIBARED_MODE

   //TS_MACROS//
#define MRS_TS_NUM            (0x0001)
#define TS_BASE_ADDRESS       (0x0080)
#define TS_COMMON_REG_SIZE    (0x0040)
#define TS_IP_REG_SIZE        (0x0040)
#define TS_SIZE   (TS_COMMON_REG_SIZE + (TS_IP_REG_SIZE * MRS_TS_NUM))

   //PD_MACROS//
#define MRS_PD_NUM            (0x0009)
#define PD_BASE_ADDRESS       ((TS_BASE_ADDRESS + TS_SIZE + 0x0040) & 0xFF80)
#define PD_COMMON_REG_SIZE    (0x0040)
#define PD_IP_REG_SIZE        (0x0040)
#define PD_SIZE               (PD_COMMON_REG_SIZE + (PD_IP_REG_SIZE * MRS_PD_NUM))

   //VM_MACROS//
#define MRS_VM_NUM            (0x0001)
#define VM_BASE_ADDRESS       ((PD_BASE_ADDRESS + PD_SIZE +(0x20*MRC_CH_NUM/8)) & (0xFF00 << (MRS_CH_NUM/8)))
#define VM_COMMON_REG_SIZE    (0x0200*(MRS_CH_NUM/8))
#define VM_IP_REG_SIZE        (0x0100*(MRS_CH_NUM/8))
#define VM_SIZE               (VM_COMMON_REG_SIZE + (VM_IP_REG_SIZE * MRS_VM_NUM))

#define   TB_COMP_ID_DATA                     (0x9b487040) 
#define   TB_ID_NUM_DATA                      (0x76543210)
#define   TB_UNLOCK_CODE_DATA                 (0x1acce551)
#define   IP_CONFIG_REG_DATA                  (0x10010901)
#define   TB_UNLOCK_CODE_DATA                 (0x1acce551)
   // Serial Data Adaptor
#define   TB_SDA_ADDR_WIDTH                   (0x3)  

   // APB Bus Defaults
#define   TB_ADDR_WIDTH                       (16)
#define   TB_DATA_WIDTH                       (32)

   // IP Control Register
#define  TB_SDA_IP_CTRL_ADDR (0x0)
#define  TB_SDA_IP_CTRL_MEM  (0x00050b)
#define  TB_SDA_IP_CTRL_RST  (0x01)

   // IP Config Register
#define  TB_SDA_IP_CFG_ADDR  (0x1) 
#define  TB_SDA_IP_CFG_MEM   (0xffffff)
#define  TB_SDA_IP_CFG_RST  (0x00)

   // IP Config A Register
#define TB_SDA_IP_CFGA_ADDR  (0x02)
#define TB_SDA_IP_CFGA_MEM   (0x0001ff)
#define TB_SDA_IP_CFGA_RST   (0x00)

   // IP DATA Register
#define  TB_SDA_IP_DATA_ADDR (0x03)

   // IP AUTO Register
#define TB_SDA_IP_POLL_ADDR  (0x04)
#define TB_SDA_IP_POLL_MEM   (0x0fffff)
#define TB_SDA_IP_POLL_RST   (0x00)

   // IP Timer Register
#define TB_SDA_IP_TMR_ADDR   (0x05) //SDA Slave Register
#define TB_SDA_IP_TMR_MEM    (0x00ffff)
#define TB_SDA_IP_TMR_RST    (0x00) 

   // define offset between slaves address for calculation
#define SLAVES_ADDRESS_OFFSET   (0x40)

#define APB_WRITE(Addr, Val) *(volatile unsigned int *) (Addr) = Val
#define APB_READ(Addr) *(volatile unsigned int *) (Addr)

/**********************************************
   define sensors constans
***********************************************/
#define Cal0    42.15
#define Cal1    205.8
#define Cal2    -5.0
#define Cal3    0.825
#define Cal4    1
#define Cal5    4094
#define Cal6    149
#define Cal7    1.22
#define Cal8    1.44
#define Cal9    178
#define Cal10   -0.28
#define Adatasheet 42.74
#define Bdatasheet 220.5
#define Cdatasheet -0.16

#define G       59.1
#define H       202.8
#define J       -0.16
#define Fclk    6
#define R       14
#define HYST_TS_THRESH  35
#define ALARM_TS_THRESH 32

//should be measured and set
//float Acal  =32.285770;
//float Bcal  =199.333405;
float Acal  =0;
float Bcal  =0;
//float Acal=32.292942;
//float Bcal=199.341766;
//float Acal=31.252516,Bcal=206.941010;


//#define VREF    0.95
//#define VREF    0.7
//#define VREF    0.75
//#define VREF    0.72
//#define VREF    0.65
#define VREF    0.8
//#define VREF    0.88
//#define VREF    0.9
#define TWO_POW_14 16384
#define PRE_SCALE_A       8 
#define PRE_SCALE_B       4 
#define COUNT_WINDOW_W    127
#define N_GATES           91


#define PVT_DRVP_SLEEP_TIME   (30)  // 30us sleep

#define PVT_DRVP_WHILE_LOOP_LIMITED(COND, MAX_ITERATIONS)   {  \
                                                               UINT32   iteration = 0; \
                                                               while((COND) && (iteration++ < MAX_ITERATIONS)) \
                                                               { \
                                                                  OS_LYRG_usleep(PVT_DRVP_SLEEP_TIME); \
                                                               }\
                                                               if(iteration >= MAX_ITERATIONS) \
                                                               {\
                                                                  return GME__ERR_GME_NOT_READY;\
                                                               }\
                                                             };

#define PVT_DRV_DRVP_MAX_ITERATIONS (10000)

typedef struct
{
   UINT32                     deviceBaseAddress;
} GME_DRVP_deviceDescT;

/****************************************************************************
 ***************       L O C A L         F U N C T I O N S              ***************
 ****************************************************************************/
const char* print_slave(int slave_num)
{
    const char* slave_name;
    slave_name = (slave_num==0)?"IOR_PD_0":(slave_num==1)?"IOR_PD_1":
                 (slave_num==2)?"EVP_PD_0":(slave_num==3)?"EVP_PD_1":
                 (slave_num==4)?"DDR_PD_0":(slave_num==5)?"DDR_PD_1":
                 (slave_num==6)?"PPE_PD"  :(slave_num==7)?"IAE_PD":
                 (slave_num==8)?"DPE_PD"  :"";
    return slave_name;
}

/****************************************************************************
 *************************       L O C A L         D A T A              ********************
 ****************************************************************************/
static GME_DRVP_deviceDescT   GME_DRVP_deviceDesc;
static UINT16 vm_sample_count = 0; //16 bit counter register
static UINT16 ts_sample_count = 0; //16 bit counter register

ERRG_codeE PVT_DRVG_readVoltageData(float *chipVolTage);


/****************************************************************************
*
*  Function Name: sdif_lock_busy
*
*  Description:checking SDIF_LOCK&SDIF_BUSY are both low, in auto mode we checking only busy 
*
*  Inputs:
*
*  Outputs:
*
*  Returns: ERRG_codeE
*
*  Context:
*
****************************************************************************/
static ERRG_codeE sdif_lock_busy()
{
   //The SDIF_LOCK flag should be low, indicating 'sdif_auto' mode has not been enabled.
   //Ensure the serial data interface is inactive, by checking the ‘SDIF_BUSY’ flag is low before proceeding.
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_TS_CMN_SDIF_STATUS_SDIF_LOCK_R != 0, PVT_DRV_DRVP_MAX_ITERATIONS);
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_TS_CMN_SDIF_STATUS_SDIF_BUSY_R != 0, PVT_DRV_DRVP_MAX_ITERATIONS);

   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_VM_CMN_SDIF_STATUS_SDIF_LOCK_R != 0, PVT_DRV_DRVP_MAX_ITERATIONS);
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_VM_CMN_SDIF_STATUS_SDIF_BUSY_R != 0, PVT_DRV_DRVP_MAX_ITERATIONS);

   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_PD_CMN_SDIF_STATUS_SDIF_LOCK_R != 0, PVT_DRV_DRVP_MAX_ITERATIONS);
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_PD_CMN_SDIF_STATUS_SDIF_BUSY_R != 0, PVT_DRV_DRVP_MAX_ITERATIONS);

   return PVT__RET_SUCCESS;
}

//ts - Temperature Sensor
//vm-Voltage Monitor
//pd- Process Detector
/****************************************************************************
* //according to PVT controller spec
*  Function Name: PVT_alive
*
*  Description:Verify the PVT device ID and confirm test register access
*
*  Inputs:
*
*  Outputs:
*
*  Returns: ERRG_codeE
*
*  Context:
*
****************************************************************************/
ERRG_codeE PVT_DRVP_alive()
{
   ERRG_codeE ret = PVT__RET_SUCCESS;

   GME_PERIPH_CLOCK_ENABLE_PVT_SENSOR_CLK_EN_W(1);
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_PVT_SENSOR_CLK_EN_R == 0, PVT_DRV_DRVP_MAX_ITERATIONS);

   GME_PVT_PVT_TM_SCRATCH_VAL       =0;
   GME_PVT_PVT_IP_CONFIG_VAL        =0;
   GME_PVT_PVT_REG_LOCK_STATUS_VAL  =0;

//Verify the device ID
   if (GME_PVT_PVT_COMP_ID_VAL!=TB_COMP_ID_DATA)
      LOGG_PRINT(LOG_ERROR_E, NULL,"unexpected data=%x\n",GME_PVT_PVT_COMP_ID_VAL);

// read the component ip id number - value can be changed right now it's 0
   if (GME_PVT_PVT_ID_NUM_VAL!=0)
      LOGG_PRINT(LOG_ERROR_E, NULL,"unexpected data=%x\n",GME_PVT_PVT_ID_NUM_VAL);

//Verify the default reset value of the scratch register equals 0x0
   if (GME_PVT_PVT_TM_SCRATCH_VAL!=0)
      LOGG_PRINT(LOG_ERROR_E, NULL,"unexpected data=%x\n",GME_PVT_PVT_TM_SCRATCH_VAL);

//Write an appropriate test value to the scratch register
   GME_PVT_PVT_TM_SCRATCH_VAL=0xa5a5a5a5;
// read back the test register "rblk_tm_scratch
   if(GME_PVT_PVT_TM_SCRATCH_VAL!=0xa5a5a5a5)
      LOGG_PRINT(LOG_ERROR_E, NULL,"unexpected data=%x\n",GME_PVT_PVT_TM_SCRATCH_VAL);

// write a test value to the test register "rblk_tm_scratch"
   GME_PVT_PVT_TM_SCRATCH_VAL=0x5a5a5a5a;
// read back the test register "rblk_tm_scratch
   if(GME_PVT_PVT_TM_SCRATCH_VAL!=0x5a5a5a5a)
      LOGG_PRINT(LOG_ERROR_E, NULL,"unexpected data=%x\n",GME_PVT_PVT_TM_SCRATCH_VAL);

// read the ip config register
   if(GME_PVT_PVT_IP_CONFIG_VAL!=IP_CONFIG_REG_DATA)
      LOGG_PRINT(LOG_ERROR_E, NULL,"unexpected data=%x\n",GME_PVT_PVT_IP_CONFIG_VAL);

//read the register lock status test value, why
   if(GME_PVT_PVT_REG_LOCK_STATUS_VAL!=0)
      LOGG_PRINT(LOG_ERROR_E, NULL,"unexpected data=%x\n",GME_PVT_PVT_REG_LOCK_STATUS_VAL);

   return ret;
}


/****************************************************************************
* //according to PVT controller spec
*  Function Name: PVT_DRVP_irq_en
*
*  Description:Configure the PVT Controller Alarms
*
*  Inputs:
*
*  Outputs:
*
*  Returns: ERRG_codeE
*
*  Context:
*
****************************************************************************/
ERRG_codeE PVT_DRVP_irq_en()
{
//   UINT32      number_of_process_slaves = 9;
//   UINT32      slave_index;
//   UINT32      n_val_for_hyst_vm_thresh = 0x3eef;//with vref = 0.8 if N = 0x3edf in voltage formula we get V = 0.785[V] 
//   UINT32      n_val_for_alarm_vm_thresh = 0x3ee3;
//   float    n_val_for_hyst_ts_thresh   = 0;
//   float    n_val_for_alarm_ts_thresh  = 0;

// configure masters irq
   GME_PVT_IRQ_EN_VAL            =0xF;
// ts slaves irq enable
   GME_PVT_TS_00_IRQ_ENABLE_VAL  =0x1A;
// pd slaves irq enable
   GME_PVT_PD_00_IRQ_ENABLE_VAL  =0x1A;
   GME_PVT_PD_01_IRQ_ENABLE_VAL  =0x1A;
   GME_PVT_PD_02_IRQ_ENABLE_VAL  =0x1A;
   GME_PVT_PD_03_IRQ_ENABLE_VAL  =0x1A;
   GME_PVT_PD_04_IRQ_ENABLE_VAL  =0x1A;
   GME_PVT_PD_05_IRQ_ENABLE_VAL  =0x1A;
   GME_PVT_PD_06_IRQ_ENABLE_VAL  =0x1A;
   GME_PVT_PD_07_IRQ_ENABLE_VAL  =0x1A;
   GME_PVT_PD_08_IRQ_ENABLE_VAL  =0x1A;

// vm irq enable
   GME_PVT_VM_00_IRQ_ENABLE_VAL =0x2;           //enable irq and enable
// vm alarm a and alarm b irq enable
//   GME_PVT_VM_00_IRQ_ALARMA_ENABLE_VAL=0xFFFF;//enable irq alarm a enable irq for channel 0
//   GME_PVT_VM_00_IRQ_ALARMB_ENABLE_VAL=0xFFFF;//enable irq alarm b enable irq for channel 0

//Configure the PVT Controller Alarms
//Mode 2 Output conversion
// set alarm threshold values
//   n_val_for_hyst_ts_thresh  = (HYST_TS_THRESH  - J*Fclk + 0.5*H - G) * (Cal5/H);
//   n_val_for_alarm_ts_thresh = (ALARM_TS_THRESH - J*Fclk + 0.5*H - G) * (Cal5/H);
//   GME_PVT_TS_00_ALARMA_CFG_VAL=((int)n_val_for_hyst_ts_thresh)<<16|((int)n_val_for_alarm_ts_thresh);//set alarm a thresh
//   GME_PVT_TS_00_ALARMB_CFG_VAL=((int)n_val_for_hyst_ts_thresh-10)<<16|((int)n_val_for_alarm_ts_thresh-10);//set alarm b thresh

//voltage monitor interrupts does not work
//APB_WRITE(GME_PVT_VM_00_CH_00_ALARMA_CFG_REG,(n_val_for_hyst_vm_thresh)<<16|(n_val_for_alarm_vm_thresh));//set alarm a thresh    
//APB_WRITE(GME_PVT_VM_00_CH_00_ALARMB_CFG_REG,(n_val_for_hyst_vm_thresh-10)<<16|(n_val_for_alarm_vm_thresh-10));//set alarm b thresh

   return PVT__RET_SUCCESS;
}

/****************************************************************************
* //according to PVT controller spec
*  Function Name: PVT_DRVP_irq_disable
*
*  Description:Configure the PVT Controller Alarms
*
*  Inputs:
*
*  Outputs:
*
*  Returns: ERRG_codeE
*
*  Context:
*
****************************************************************************/
ERRG_codeE PVT_DRVP_irq_disable()
{
// configure masters irq
   GME_PVT_IRQ_EN_VAL            =0;
// ts slaves irq enable
   GME_PVT_TS_00_IRQ_ENABLE_VAL  =0;
// pd slaves irq enable
   GME_PVT_PD_00_IRQ_ENABLE_VAL  =0;
   GME_PVT_PD_01_IRQ_ENABLE_VAL  =0;
   GME_PVT_PD_02_IRQ_ENABLE_VAL  =0;
   GME_PVT_PD_03_IRQ_ENABLE_VAL  =0;
   GME_PVT_PD_04_IRQ_ENABLE_VAL  =0;
   GME_PVT_PD_05_IRQ_ENABLE_VAL  =0;
   GME_PVT_PD_06_IRQ_ENABLE_VAL  =0;
   GME_PVT_PD_07_IRQ_ENABLE_VAL  =0;
   GME_PVT_PD_08_IRQ_ENABLE_VAL  =0;

// vm irq enable
   GME_PVT_VM_00_IRQ_ENABLE_VAL =0;           //enable irq and enable

   return PVT__RET_SUCCESS;
}


/****************************************************************************
* //according to PVT controller spec
*  Function Name: PVT_DRVP_sample_control
*
*  Description:PVT Sample Control
*
*  Inputs:
*
*  Outputs:
*
*  Returns: ERRG_codeE
*
*  Context:
*
****************************************************************************/
ERRG_codeE PVT_DRVP_sample_control()
{
   if(ERRG_FAILED(sdif_lock_busy()))
      return PVT__ERR_UNEXPECTED;

   //enable sample control
   GME_PVT_VM_CMN_SMPL_CTRL_VAL=0x0;
   GME_PVT_TS_CMN_SMPL_CTRL_VAL=0x0;
   GME_PVT_PD_CMN_SMPL_CTRL_VAL=0x0;

   //clear sample control
   GME_PVT_VM_CMN_SMPL_CLR_SMPL_CTR_DISABLE_W(1);
   GME_PVT_TS_CMN_SMPL_CLR_SMPL_CTR_DISABLE_W(1);
   GME_PVT_PD_CMN_SMPL_CLR_SMPL_CTR_DISABLE_W(1);

   //verify sample counter
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_TS_CMN_SMPL_CNT_SMPL_COUNT_R!= 0, PVT_DRV_DRVP_MAX_ITERATIONS);
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_VM_CMN_SMPL_CNT_SMPL_COUNT_R!= 0, PVT_DRV_DRVP_MAX_ITERATIONS);
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_PD_CMN_SMPL_CNT_SMPL_COUNT_R!= 0, PVT_DRV_DRVP_MAX_ITERATIONS);

   return PVT__RET_SUCCESS;
}


/****************************************************************************
* //according to PVT controller spec
*  Function Name: PVT_DRVP_hi_lo_control
*
*  Description:PVT Sample HiLo Control
*
*  Inputs:
*
*  Outputs:
*
*  Returns: ERRG_codeE
*
*  Context:
*
****************************************************************************/
ERRG_codeE PVT_DRVP_hi_lo_control()
{
   UINT32 pd_offset_channels_offset = 0x40;
   UINT32 vm_offset_channels_offset = 0x10;
   UINT32 i = 0;

   if(ERRG_FAILED(sdif_lock_busy()))
      return PVT__ERR_UNEXPECTED;

//Write to these self-clearing bits to re-initialise the associated register value
//pd hi lo
   for(i = 0;i<9 ; i++)
   {
      APB_WRITE(i*pd_offset_channels_offset + GME_PVT_PD_00_HILO_RESET_REG,0x2);//set clear
   }
//vm hi lo
   for(i = 0;i<16 ; i++)
   {
      APB_WRITE(i*vm_offset_channels_offset + GME_PVT_VM_00_CH_00_HILO_RESET_REG,0x2);
   }

//ts hi lo
   GME_PVT_TS_00_HILO_RESET_VAL=0x2;//set clear
   if (GME_PVT_TS_00_HILO_RESET_VAL!=0)
   {
      return PVT__ERR_UNEXPECTED;
   }
   return PVT__RET_SUCCESS;
}


/****************************************************************************
* //according to PVT controller spec
*  Function Name: PVT_DRVP_clock_synth
*
*  Description:Configure the Clock synthesizer of ts, vm, pd
*
*  Inputs:
*
*  Outputs:
*
*  Returns: ERRG_codeE
*
*  Context:
*
****************************************************************************/
ERRG_codeE PVT_DRVP_clock_synth()
{
   UINT32 write_data;
   UINT32 temp_1;
   UINT32 temp_2;
   UINT32 mrs_clk_freq_mhz = 24;
   UINT32 synth_hi, synth_lo , synth_hi_lo;
   UINT32 synth_ratio;
   double tb_min_clk_freq_mhz;
   double tb_max_clk_freq_mhz;
   double tc_ts_target_clk_freq_mhz;
   double tb_ip_clk_tol_mhz= 0.1;

   if(ERRG_FAILED(sdif_lock_busy()))
      return PVT__ERR_UNEXPECTED;

//calc synth
   //  operating clock frequency
   tb_min_clk_freq_mhz = 4.0 + tb_ip_clk_tol_mhz;
   tb_max_clk_freq_mhz = 8.0 - tb_ip_clk_tol_mhz;

   tc_ts_target_clk_freq_mhz = tb_max_clk_freq_mhz;

   //set up the synth divider
   synth_ratio = (mrs_clk_freq_mhz/tc_ts_target_clk_freq_mhz);

   // allow for potential rounding when assigning to an integer
   if((mrs_clk_freq_mhz/synth_ratio) < tb_min_clk_freq_mhz)
   { // under freq range check
      synth_ratio = synth_ratio -1;
   }
   else if((mrs_clk_freq_mhz/synth_ratio) > tb_max_clk_freq_mhz)
   { // over freq range check
      synth_ratio = synth_ratio +1;
   }

// according to table 90 in spec
   synth_hi = (synth_ratio/2) -1;
   synth_lo = ((synth_ratio/2) + synth_ratio%2) -1;
   temp_1 = synth_lo & 0x000000ff;// set to 6 MHZ
   temp_2 = synth_hi & 0x000000ff;// set to 6 MHZ
   //synth_hi_lo = {synth_hi[7:0], synth_lo[7:0]};
   // temp_1 = (synth_lo & 0x000000ff)+1;//set to 4 MHZ
   // temp_2 = (synth_hi & 0x000000ff)+1;//set to 4 MHZ
    temp_2 = temp_2 << 8;

   synth_hi_lo = temp_2 | temp_1;
   write_data = 0x01010000 | synth_hi_lo;

//config synth
   GME_PVT_VM_CLK_SYNTH_VAL   =write_data;
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_VM_CLK_SYNTH_CLK_SYTH_EN_R!= 1, PVT_DRV_DRVP_MAX_ITERATIONS);

   GME_PVT_PD_CMN_CLK_SYNTH_VAL=write_data;
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_PD_CMN_CLK_SYNTH_CLK_SYTH_EN_R!= 1, PVT_DRV_DRVP_MAX_ITERATIONS);

   GME_PVT_TS_CMN_CLK_SYNTH_VAL=write_data;
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_TS_CMN_CLK_SYNTH_CLK_SYTH_EN_R!= 1, PVT_DRV_DRVP_MAX_ITERATIONS);

   return PVT__RET_SUCCESS;
}

/****************************************************************************
* //according to PVT controller spec
*  Function Name: PVT_DRVP_temp_Calib
*
*  Description:sets A & B calibarted values
*
*  Inputs:
*
*  Outputs:
*
*  Returns: void
*
*  Context:
*
****************************************************************************/
void PVT_DRVP_temp_Calib()
{
   UINT32   i=0, write_data=0;
   //address to ip_cfg
   write_data =   TB_SDA_IP_CFG_ADDR << 24;
   write_data =   write_data | 0x88000000;//The device must be set into Mode 1 to allow and use calibration (cfg<3:0>=0000)
   GME_PVT_TS_CMN_SDIF_VAL=write_data;

   //Cn is a conversion in Vcal Mode and the Output Value Taken
   write_data =   TB_SDA_IP_CFGA_ADDR << 24;
   write_data =   write_data | 0x88000003;//The device mode to be set by the analog select controls to Vcal mode (an_sel<3:0> = <0011>)

   //Run Conversions and Record average Output Value as NBS_CM
   UINT32 CN[30], NN[30];
   float N_bs_cm=0, N_bs_init=0;
   for(i=0;i<16;i++)
   {
      CN[i] = GME_PVT_TS_00_SDIF_DATA_VAL;//The device must be set to provide 12-bit output
      if (((CN[i])&0x10000)>>16==0||((CN[i])&0x20000)>>17==0)//checking if data is valid (bit 17 ) & if the data isnt a signature type 
      {
         CN[i]= CN[i]&0x00FFF;//11:0
         N_bs_cm+=CN[i];
      }
   }
   N_bs_cm=(2*N_bs_cm-CN[0]-CN[15])/30;

   // Measure the applied voltage
   float Vapp=VREF;//measurement is needed
/*   if(ERRG_FAILED(PVT_DRVG_readVoltageData(&Vapp)))
      LOGG_PRINT(LOG_ERROR_E, NULL,"Vapp=%f \n",Vapp);
   else
      LOGG_PRINT(LOG_INFO_E, NULL,"Vapp=%f \n",Vapp);
*/
   float Eq1      = (((N_bs_cm/Cal5)+Cal4)*(Vapp/Cal3))-1.5;
   float tempCal  = Cal0 + (Cal1 * Eq1) + (Cal2*Eq1*Eq1) + (Cal10*Fclk);

   //Set Normal Operation Mode
   write_data =   TB_SDA_IP_CFGA_ADDR << 24;
   write_data =   write_data | 0x88000000;//The device is placed into Normal Mode by control of the analog select pins

   for(i=0;i<30;i++)
   {
      NN[i] = GME_PVT_TS_00_SDIF_DATA_VAL;//The device must be set to provide 12-bit output
      if (((NN[i])&0x10000)>>16==0||((NN[i])&0x20000)>>17==0)//checking if data is valid (bit 17 ) & if the data isnt a signature type 
      {
         N_bs_init+= NN[i]&0x00FFF;//11:0
      }
   }
   N_bs_init/=30;

   float Eq2         = (N_bs_init/Cal5)-0.5;
   float tempInit    = Adatasheet + (Bdatasheet * Eq2) + (Cdatasheet*Fclk);

   float Q           = Cal6*(tempInit - tempCal)/(Eq2*Bdatasheet + Cal9);
   Acal              = Adatasheet - Q*Cal7;
   Bcal              = Bdatasheet - Q*Cal8;
//   printf("Acal=%f,Bcal=%f \n",Acal,Bcal);
}

/****************************************************************************
* //according to PVT controller spec
*  Function Name: PVT_DRVP_config_sda_register
*
*  Description:Configure the SDA Registers
*
*  Inputs:
*
*  Outputs:
*
*  Returns: ERRG_codeE
*
*  Context:
*
****************************************************************************/
ERRG_codeE PVT_DRVP_config_sda_register()
{
   int pd_offset_channels_offset = 0x40;
   int i = 0;
   int write_data;

   if(ERRG_FAILED(sdif_lock_busy()))
      return PVT__ERR_UNEXPECTED;

//READ ip_tmr from slaves
//sda ip tmr
//checking tmr reset values
   write_data =   TB_SDA_IP_TMR_ADDR << 24;
   write_data =   write_data | 0x88000000;//SDIF_WRN, SDIF_PROG

   GME_PVT_PD_CMN_SDIF_VAL=write_data;
   for(i = 0; i<9; i++)
   {
      PVT_DRVP_WHILE_LOOP_LIMITED(APB_READ(i*pd_offset_channels_offset + GME_PVT_PD_00_SDIF_RDATA_REG)!=TB_SDA_IP_TMR_RST, PVT_DRV_DRVP_MAX_ITERATIONS);
   }

   write_data =   TB_SDA_IP_TMR_ADDR << 24;
   write_data =   write_data | 0x88000000;
   GME_PVT_TS_CMN_SDIF_VAL=write_data;
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_TS_00_SDIF_RDATA_VAL!=TB_SDA_IP_TMR_RST, PVT_DRV_DRVP_MAX_ITERATIONS);

   GME_PVT_VM_CMN_SDIF_VAL=write_data;
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_VM_00_SDIF_RDATA_VAL!=TB_SDA_IP_TMR_RST, PVT_DRV_DRVP_MAX_ITERATIONS);

   if(ERRG_FAILED(sdif_lock_busy()))
      return PVT__ERR_UNEXPECTED;

//set SDIF_WARN to write bit in TS/VM/PD_SDIF registers [27]
//set SDIF_PROG bit in TS/VM/PD_SDIF registers [31]
//and address to ip_tmr
#ifndef CALIBARED_MODE
   write_data =   TB_SDA_IP_TMR_ADDR << 24;
   write_data =   write_data | 0x88000100;//power up delay [15:0]= 256

   GME_PVT_TS_CMN_SDIF_VAL=write_data;
#endif
//    address =TS_CMN_SDIF_STATUS_REG;     
//    error_num = error_num + PVT_check_status(0, address,0);

   write_data             = TB_SDA_IP_TMR_ADDR << 24;
   write_data             = write_data | 0x88000040;//power up delay [15:0]= 64
   GME_PVT_VM_CMN_SDIF_VAL= write_data;

   write_data             = TB_SDA_IP_TMR_ADDR << 24;
   write_data             = write_data | 0x88000000;//power up delay [15:0]= 0
   GME_PVT_PD_CMN_SDIF_VAL= write_data;

   if(ERRG_FAILED(sdif_lock_busy()))
      return PVT__ERR_UNEXPECTED;

//ip_polling only in vm
//set SDIF_WARN to write bit in TS/VM/PD_SDIF registers [27]
//set SDIF_PROG bit in VM_SDIF registers [31]
//and address to ip_polling
   write_data =   TB_SDA_IP_POLL_ADDR << 24;
   write_data =   write_data | 0x880300001;  // sel channel 0 and enable channel 0 
   //write_data =   write_data | 0x88030FFFF;   
   GME_PVT_VM_CMN_SDIF_VAL=write_data;


   if(ERRG_FAILED(sdif_lock_busy()))
      return PVT__ERR_UNEXPECTED;
//set SDIF_WARN to write bit in TS/VM/PD_SDIF registers [27]
//set SDIF_PROG bit in TS/VM/PD_SDIF registers [31]
//and address to ip_cfg
#ifndef CALIBARED_MODE
   write_data =   TB_SDA_IP_CFG_ADDR << 24;//CFG0[0:3]=1 using uncalibrated method with g&h with 12 bit resolution cfg0[4:7]=0
   write_data =   write_data | 0x88000001;
   GME_PVT_TS_CMN_SDIF_VAL=write_data;
#endif

   write_data =   TB_SDA_IP_CFG_ADDR << 24;
   write_data =   write_data | 0x88000000;//14 bit resulotion(cfg1==0), vin00 (cfg2==0) 

   GME_PVT_VM_CMN_SDIF_VAL=write_data;

   write_data =   TB_SDA_IP_CFG_ADDR << 24;
   write_data =   write_data | 0x88112000;//cfg2[7:5]=001SVT =built -in chain B=4, cfg3[3:0]==1 pre scale A =8, cfg3[7:4]=1 W =127
   //write_data =   write_data | 0x88114000;//cfg2[7:5]=002LVT =built -in chain B=4, cfg3[3:0]==1 pre scale A =8, cfg3[7:4]=1 W =127
   // write_data =   write_data | 0x88116000;//cfg2[7:5]=003ULVT =built -in chain B=4, cfg3[3:0]==1 pre scale A =8, cfg3[7:4]=1 W =127

   //write_data =   write_data | 0x88112008;//cfg2[7:5]=001 =built -in chain B=4, cfg3[3:0]==1 pre scale A =8, cfg3[7:4]=1 W =127 //signature 
   //write_data =   write_data | 0x88000008;//signature
   GME_PVT_PD_CMN_SDIF_VAL=write_data;

   if(ERRG_FAILED(sdif_lock_busy()))
      return PVT__ERR_UNEXPECTED;

//READ ip_cfga from slaves
//sda ip cfga
//checking cfga reset values
//("error_num %x\n ",error_num);
   write_data =   TB_SDA_IP_CFGA_ADDR << 24;
   write_data =   write_data | 0x88000000;

   GME_PVT_VM_CMN_SDIF_VAL=write_data;

   if(ERRG_FAILED(sdif_lock_busy()))
      return PVT__ERR_UNEXPECTED;

   if (GME_PVT_VM_00_SDIF_RDATA_VAL!=TB_SDA_IP_CFGA_RST)
      return PVT__ERR_UNEXPECTED;

//READ ip_CTRL from slaves
//sda ip CTRL
//checking ctrl reset values
   write_data = TB_SDA_IP_CTRL_ADDR << 24;
   write_data = write_data | 0x88000000;

   GME_PVT_PD_CMN_SDIF_VAL=write_data;
   if(ERRG_FAILED(sdif_lock_busy()))
      return PVT__ERR_UNEXPECTED;

   for(i = 0;i<9 ; i++)
   {
      if ((APB_READ(i*pd_offset_channels_offset + GME_PVT_PD_00_SDIF_RDATA_REG)>>1)!=(TB_SDA_IP_CTRL_RST)>>1)
      {
         return PVT__ERR_UNEXPECTED;
      }
   }
   GME_PVT_TS_CMN_SDIF_VAL=write_data;

   if ((GME_PVT_TS_00_SDIF_RDATA_VAL>>1)!=(TB_SDA_IP_CTRL_RST>>1))
   {
      return PVT__ERR_UNEXPECTED;
   }

   GME_PVT_VM_CMN_SDIF_VAL=write_data;
   if ((GME_PVT_VM_00_SDIF_RDATA_VAL>>1)!=(TB_SDA_IP_CTRL_RST>>1))
   {
      return PVT__ERR_UNEXPECTED;
   }

#ifdef CALIBARED_MODE
   PVT_DRVP_temp_Calib();
#endif

   return PVT__RET_SUCCESS;
}

/****************************************************************************
***********************      G L O B A L     F U N C T I O N S     *********************
****************************************************************************/

/****************************************************************************
*
*  Function Name: PVT_DRVG_init
*
*  Description: initialize PVT hw block
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: temperature
*
****************************************************************************/
ERRG_codeE PVT_DRVG_init()
{
   vm_sample_count = 1;
   ts_sample_count = 1;

   MEM_MAPG_addrT       memVirtAddr;
   MEM_MAPG_getVirtAddr(MEM_MAPG_REG_GME_E, (&memVirtAddr));
   GME_DRVP_deviceDesc.deviceBaseAddress = (UINT32)memVirtAddr;

   //reset to PVT to prevent bad values from previous opertation
   GME_SW_RESET_1_SW_PVT_INF_RESET_N_W(1);
   GME_SW_RESET_1_SW_PVT_INF_RESET_N_W(0);
   GME_SW_RESET_1_SW_PVT_MST_RESET_N_W(1);
   GME_SW_RESET_1_SW_PVT_MST_RESET_N_W(0);

   if (ERRG_FAILED(PVT_DRVP_alive()))
      return PVT__ERR_UNEXPECTED;

   if (ERRG_FAILED(PVT_DRVP_irq_en()))
      return PVT__ERR_UNEXPECTED;

   if (ERRG_FAILED(PVT_DRVP_sample_control()))
      return PVT__ERR_UNEXPECTED;

   if (ERRG_FAILED(PVT_DRVP_hi_lo_control()))
      return PVT__ERR_UNEXPECTED;

   if (ERRG_FAILED(PVT_DRVP_clock_synth()))
      return PVT__ERR_UNEXPECTED;

   if (ERRG_FAILED(PVT_DRVP_config_sda_register()))
      return PVT__ERR_UNEXPECTED;

   return PVT__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: PVT_DRVG_deinit
*
*  Description: deinitialize PVT hw block
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: temperature
*
****************************************************************************/
ERRG_codeE PVT_DRVG_deinit()
{
   UINT32 write_data;

   if(ERRG_FAILED(sdif_lock_busy()))
      return PVT__ERR_UNEXPECTED;

   //power down detectors
   write_data                    =  TB_SDA_IP_CTRL_ADDR << 24;
   write_data                    =  write_data | 0x88000001;
   GME_PVT_TS_CMN_SDIF_VAL       =  write_data;
   GME_PVT_VM_CMN_SDIF_VAL       =  write_data;
   GME_PVT_PD_CMN_SDIF_VAL       =  write_data;

   //disable clock synthesiser
   GME_PVT_TS_CMN_CLK_SYNTH_VAL  =  GME_PVT_TS_CMN_CLK_SYNTH_VAL & (1 << 24);//to be changed
   GME_PVT_PD_CMN_CLK_SYNTH_VAL  =  GME_PVT_PD_CMN_CLK_SYNTH_VAL & (1 << 24);
   GME_PVT_VM_CLK_SYNTH_VAL      =  GME_PVT_VM_CLK_SYNTH_VAL     & (1 << 24);

   if(ERRG_FAILED(sdif_lock_busy()))
      return PVT__ERR_UNEXPECTED;

   PVT_DRVP_irq_disable();

   GME_PERIPH_CLOCK_ENABLE_PVT_SENSOR_CLK_EN_W(0);
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_PVT_SENSOR_CLK_EN_R == 1, PVT_DRV_DRVP_MAX_ITERATIONS);

   return PVT__RET_SUCCESS;
}

/****************************************************************************
* //according to PVT controller spec
*  Function Name: PVT_DRVG_readProcessData
*
*  Description:read PVT Process Data
*
*  Inputs:
*
*  Outputs:
*
*  Returns: pvt process data
*
*  Context:
*
****************************************************************************/
ERRG_codeE PVT_DRVG_readProcessData(float *chipProcess)
{
   int write_data ,pd_slaves_iterations, raw_data, fault, type;
   float temp_output, f_loop, gate_delay=0;

   write_data              =  TB_SDA_IP_CTRL_ADDR << 24;
   write_data              =  write_data | 0x88000104;//pd auto mode, run once 
   GME_PVT_PD_CMN_SDIF_VAL =  write_data;

   if(ERRG_FAILED(sdif_lock_busy()))
      return PVT__ERR_UNEXPECTED;

   for (pd_slaves_iterations = 0;pd_slaves_iterations<3;pd_slaves_iterations++)
   {
      PVT_DRVP_WHILE_LOOP_LIMITED(APB_READ(GME_PVT_PD_00_SDIF_DONE_REG+pd_slaves_iterations*0x40)==0, PVT_DRV_DRVP_MAX_ITERATIONS);
      raw_data    = APB_READ(GME_PVT_PD_00_SDIF_DATA_REG+pd_slaves_iterations*0x40);
      fault       = raw_data&0x00020000;
      type        = raw_data&0x00010000;
      type        = type >> 16;
      LOGG_PRINT(LOG_DEBUG_E, NULL,"pd type  %x\n",type);
      fault       = fault >> 17;
      raw_data    = (0x00000FFF&raw_data);
      LOGG_PRINT(LOG_DEBUG_E, NULL,"pd fault %x\n",fault);
      LOGG_PRINT(LOG_DEBUG_E, NULL,"pd raw data  %x\n",raw_data);
      temp_output =((float)(raw_data));
      f_loop      = (temp_output*(float)PRE_SCALE_A*(float)PRE_SCALE_B*(float)(Fclk))/((float)COUNT_WINDOW_W);
      LOGG_PRINT(LOG_DEBUG_E, NULL,"Process: Frequency of the delay loop structure: %f",f_loop);
      gate_delay  =((float)N_GATES)*f_loop;
      gate_delay  = gate_delay;
      gate_delay  = 1000/gate_delay;
      LOGG_PRINT(LOG_DEBUG_E, NULL,"PD Slave:%s Gate delay value in nanoseconds: %f \n", print_slave(pd_slaves_iterations), gate_delay);
      *(chipProcess++)=gate_delay;
   }
   return PVT__RET_SUCCESS;
}


/****************************************************************************
* //according to PVT controller spec
*  Function Name: PVT_DRVG_readVoltageData
*
*  Description:read PVT Voltage Data
*
*  Inputs:
*
*  Outputs:
*
*  Returns: pvt voltage data
*
*  Context:
*
****************************************************************************/
ERRG_codeE PVT_DRVG_readVoltageData(float *chipVolTage)
{
   UINT32 write_data;
   UINT32 raw_data;
   float  temp_output;
   float  voltage_output   =0;
   // vm run once
   write_data              =  TB_SDA_IP_CTRL_ADDR << 24;
   write_data              =  write_data | 0x88000504;//set_ip_vm_mode auto mode , run once, reset_n=1, cload 
   GME_PVT_VM_CMN_SDIF_VAL =  write_data;

   if(ERRG_FAILED(sdif_lock_busy()))
      return PVT__ERR_UNEXPECTED;

// wait to recieve data
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_IRQ_VM_STATUS_VAL==0, PVT_DRV_DRVP_MAX_ITERATIONS);
// wait to recieve data done
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_VM_00_SDIF_DONE_VAL==0, PVT_DRV_DRVP_MAX_ITERATIONS);

   if (vm_sample_count != GME_PVT_VM_CMN_SMPL_CNT_VAL)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,"VM sample count Error: expected %x, got %x\n",vm_sample_count,GME_PVT_VM_CMN_SMPL_CNT_VAL);
   }
   vm_sample_count++;

// vm only channel 0 is working with vin changing each iteration
   raw_data = GME_PVT_VM_00_CH_00_SDIF_DATA_VAL;
   if (((raw_data)&0x10000)>>16==0||((raw_data)&0x20000)>>17==0)//checking if data is valid (bit 17 ) & if the data isn't a signature type
   {
      raw_data= raw_data&0x0003FFF;
      temp_output=((float)raw_data);//14 bit output
      voltage_output=(((float)VREF/5.0))*(((6.0*(temp_output-0.5))/((float)TWO_POW_14))-1.0);
      LOGG_PRINT(LOG_DEBUG_E, NULL,"Voltage=%f \n",voltage_output);
      GME_PVT_VM_00_IRQ_CLEAR_VAL=0x1;//clear fault irq
      *chipVolTage=  voltage_output;
   }
   else
   {// discard sample
        GME_PVT_VM_00_IRQ_CLEAR_VAL=0x1;//clear fault irq
   }
   return PVT__RET_SUCCESS;
}

/****************************************************************************
* //according to PVT controller spec
*  Function Name: PVT_DRVG_readTemperatureData
*
*  Description:read PVT Temperature Data
*
*  Inputs:
*
*  Outputs:
*
*  Returns: pvt temperature data
*
*  Context:
*
****************************************************************************/
ERRG_codeE PVT_DRVG_readTemperatureData(float *chipTemp)
{
   UINT32 write_data;
   UINT32 raw_data;
   float  temp_output;
   float  temprature=0;
   float  Eqbs;

   // ts run once
   write_data =  TB_SDA_IP_CTRL_ADDR << 24;
   write_data =  write_data | 0x88000104;
   GME_PVT_TS_CMN_SDIF_VAL=write_data;

   if(ERRG_FAILED(sdif_lock_busy()))
      return PVT__ERR_UNEXPECTED;

// wait to recieve data
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_IRQ_TS_STATUS_VAL==0, PVT_DRV_DRVP_MAX_ITERATIONS);
// wait to recieve data done
   PVT_DRVP_WHILE_LOOP_LIMITED(GME_PVT_TS_00_SDIF_DONE_VAL==0, PVT_DRV_DRVP_MAX_ITERATIONS);

   if (ts_sample_count != GME_PVT_TS_CMN_SMPL_CNT_VAL)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,"TS sample count Error: expected %x, got %x\n",ts_sample_count,GME_PVT_TS_CMN_SMPL_CNT_VAL);
   }
   ts_sample_count++;

   raw_data = GME_PVT_TS_00_SDIF_DATA_VAL;
   if (((raw_data)&0x10000)>>16==0||((raw_data)&0x20000)>>17==0)//checking if data is valid (bit 17 ) & if the data isnt a signature type 
   {
      raw_data= raw_data&0x00FFF;//11:0
      //printf("raw_data= %x \n",raw_data);
      temp_output = ((float)(raw_data))/((float)Cal5);
      Eqbs        = temp_output -0.5;
#ifdef CALIBARED_MODE
      temprature  = Acal + (Bcal * Eqbs) +(Cdatasheet*Fclk);
#else
      temprature  = G + (H * Eqbs) +(J*Fclk);
#endif
      //LOGG_PRINT(LOG_INFO_E, NULL,"Temperature=%f \n",temprature);
      GME_PVT_TS_00_IRQ_CLEAR_VAL=0x1;//clear irq and if needed also alarm irq
      *chipTemp =temprature;
   }
   else // discard sample
   {
      GME_PVT_TS_00_IRQ_CLEAR_VAL=0x1;//clear fault irq
   }
   return PVT__RET_SUCCESS;
}


