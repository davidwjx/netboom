/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "common.h"

#include "nu_regs.h"
#include "apb_timer.h"

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define APB_TIMERP_BASE                 (TMR_BASE)
#define APB_TIMERP_SIZE                 (0x14)

#define APB_TIMERP_N_LOAD_COUNT         (0x00)
#define APB_TIMERP_N_CURRENT_VALUE      (0x04)
#define APB_TIMERP_N_CONTROL            (0x08)

#define APB_TIMERP_CONTROL_ENABLE          (1 << 0)
#define APB_TIMERP_CONTROL_USER_DEFINED    (1 << 1)
#define APB_TIMERP_CONTROL_INT             (1 << 2)
#define APB_TIMERP_CONTROL_PWM             (1 << 3)

#define APB_WDTP_BASE                (WDT0_BASE)
#define APB_WDTP_CR                  (0x0)  //Control Register
#define APB_WDTP_TORR                (0x4)  //Timeout Range Register
#define APB_WDTP_CCVR                (0x8)  //Current Counter Value Register
#define APB_WDTP_REG(boff)           (volatile UINT32 *)((UINT32)APB_WDTP_BASE + boff)
#define APB_WDTP_VAL(boff)           (*(APB_WDTP_REG(boff)))

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static volatile UINT32 *APB_TIMERP_baseP = NULL;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static UINT32 APB_TIMERP_read(UINT16 byteOffset)
{
   return *(APB_TIMERP_baseP + (byteOffset >> 2));
}

static void APB_TIMERP_write(UINT32 val, UINT16 byteOffset)
{
   *(APB_TIMERP_baseP + (byteOffset >> 2)) = val;
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

void APB_TIMERG_init(APB_TIMEG_instE inst)
{
   //Inialize the timer to the given frequency.
   UINT32 ctrl = 0;
   APB_TIMERP_baseP = (volatile UINT32 *)(APB_TIMERP_BASE + (inst*APB_TIMERP_SIZE));
   
   // First Disable timer      
   ctrl &= ~APB_TIMERP_CONTROL_ENABLE;
   APB_TIMERP_write(ctrl, APB_TIMERP_N_CONTROL); 

   // Set user-defined mode, no interupt, no pwm
   ctrl |= (APB_TIMERP_CONTROL_USER_DEFINED | APB_TIMERP_CONTROL_INT);
   ctrl &= ~APB_TIMERP_CONTROL_PWM;
   APB_TIMERP_write(ctrl, APB_TIMERP_N_CONTROL);

   //Load timer value
   APB_TIMERG_setLoadCnt(~0);
}

void APB_TIMERG_start(void)
{
   //Enable the timer
   UINT32 ctrl = 0;
   
   ctrl = APB_TIMERP_read(APB_TIMERP_N_CONTROL);
   ctrl |= APB_TIMERP_CONTROL_ENABLE;
   APB_TIMERP_write(ctrl, APB_TIMERP_N_CONTROL);
}

BOOL APB_TIMERG_isStarted(void)
{
   if(APB_TIMERP_read(APB_TIMERP_N_CONTROL) & APB_TIMERP_CONTROL_ENABLE)
      return TRUE;

   return FALSE;
}

UINT32 APB_TIMERG_read(void)
{
   //Read current timer value
   return APB_TIMERP_read(APB_TIMERP_N_CURRENT_VALUE);
}

void APB_TIMERG_setLoadCnt(UINT32 loadCnt)
{
   APB_TIMERP_write(loadCnt, APB_TIMERP_N_LOAD_COUNT);
}
UINT32 APB_TIMERG_getLoadCnt(void)
{
   return APB_TIMERP_read(APB_TIMERP_N_LOAD_COUNT);
}

void APB_TIMERG_stop(void)
{
   //Disable the timer
   UINT32 ctrl = 0;
   
   ctrl = APB_TIMERP_read(APB_TIMERP_N_CONTROL);
   ctrl &= ~APB_TIMERP_CONTROL_ENABLE;
   APB_TIMERP_write(ctrl, APB_TIMERP_N_CONTROL);   
}

void APB_TIMERG_wdtReset(void)
{
   //Simple function to generate a system reset via the GPP WDT (which is connected to GME)
   
   //First set Timeout range register to minimum - which is 2^(16+i) where i is 0.
   //At 24MHz APB clk - this gives a 2.73msec timeout.   
   APB_WDTP_VAL(APB_WDTP_TORR) = 0;
   
   //Set Control Register: Bit 0 - enable, Bit 1 - system reset mode, Bit 2-4 - 2pclk cycles reset
   APB_WDTP_VAL(APB_WDTP_CR) = 1;
}

void APB_TIMERG_wdtDisable(void)
{
   APB_WDTP_VAL(APB_WDTP_CR) = 0;
}

