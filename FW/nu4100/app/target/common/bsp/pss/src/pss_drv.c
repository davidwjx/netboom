/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"
#include "pss_drv.h"
#include "nu4100_pss_regs.h"

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define PSS_DRVP_SLEEP_TIME   (10000)  // 10ms sleep
//#if 0
#define PSS_DRVP_WHILE_LOOP_LIMITED(COND, MAX_ITERATIONS)   {  \
                                                               UINT32   iteration = 0; \
                                                               while((COND) && (iteration++ < MAX_ITERATIONS)) \
                                                               { \
                                                                  OS_LYRG_usleep(PSS_DRVP_SLEEP_TIME); \
                                                               }\
                                                               if(iteration >= MAX_ITERATIONS) \
                                                               {\
                                                                  return PSS__ERR_PSS_NOT_READY;\
                                                               }\
                                                             };


#define PSS_B0_ARC_CTRL_VAL *((volatile UINT32 *) (0x80D027C))

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/

typedef struct
{
   UINT32                     deviceBaseAddress;
} PSS_DRVP_deviceDescT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static PSS_DRVP_deviceDescT   PSS_DRVP_deviceDesc;

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S      ***************
 ****************************************************************************/


/****************************************************************************
*
*   Function Name: PSS_DRVG_arcControl
*
*   Description:
*
*   Inputs:
*
*   Outputs: none
*
*   Returns:
*
*   Context: GME control
*
****************************************************************************/
ERRG_codeE PSS_DRVG_arcControl(UINT32 value)
{
   //PSS_ARC_CTRL_RUN_W(1);
   PSS_ARC_CTRL_VAL = value;   //c0 and c1_run  
   return PSS__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: PSS_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/
ERRG_codeE PSS_DRVG_init(UINT32 memVirtAddr)
{
   PSS_DRVP_deviceDesc.deviceBaseAddress = memVirtAddr;
   return PSS__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: PSS_DRVG_dumpRegs
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: IAE control
*
****************************************************************************/
ERRG_codeE PSS_DRVG_dumpRegs()
{
   UINT32 regsOffset[] = {pss_offset_tbl_values};
   UINT32 regsResetVal[] = {pss_regs_reset_val};
   UINT32 reg;

   LOGG_PRINT(LOG_INFO_E, NULL, "Modified PSS registers:\n");
   //compare against reset val
   for (reg = 0; reg < (sizeof(regsOffset)/4); reg++)
   {
      if (*(volatile UINT32 *)(PSS_BASE + regsOffset[reg]) != regsResetVal[reg])
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "0x%04x = 0x%08x\n",regsOffset[reg],*(volatile UINT32 *)(PSS_BASE + regsOffset[reg]));
      }
   }


   return PSS__RET_SUCCESS;
}
/**/
ERRG_codeE PSS_DRVG_enableWithoutDebouncing()
{

/* Email from Inuitive
External Event from GPIO goes through ELU unit before it propagates to the Sensor control FTRIG HW in IAE.

In order to propagate it well, you need to enable ELU and configure some registers in PSS in case its required – Very small configuration for basic work.

In 4100 PSS registers there is an ELU prototype with its registers.
    Enable ELU
        Optional - Enable GPIO_DB clock – this clock does debouncing on the ELU external events  - elu_ext_event_debounce_en  - Defualt is not to use it.
        ELU_CONTROL[ENABLE]
        Change Event polarity if required :  ELU_EVENT_POLARITY_0
*/
   LOGG_PRINT(LOG_INFO_E,NULL,"Enabling PSS ELU Control register for FSG external events \n");
   PSS_ELU_CONTROL_ENABLE_W(1);
   PSS_ELU_EXT_EVENT_DEBOUNCE_EN_EXT_GPIO_DB_EN_W(0);  
   return PSS__RET_SUCCESS;
}  

//#endif

