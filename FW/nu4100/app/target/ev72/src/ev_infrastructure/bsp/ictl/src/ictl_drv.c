/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include <stdio.h>
#include "ictl_drv.h"
#include "log.h"
#include "nu4100_ictl2_regs.h"


/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define ICTLP_NUM_INT           (32)
#define ICTLP_NUM_HW_PRIORITIES (16)
#define ICTLP_REG_VECTOR_STEP   (8)
#define ICTLP_USE_HW_VECTOR     (0)

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
 
#if ICTLP_USE_HW_VECTOR
typedef void (*ICTLP_prioVecT)(void);
#endif



typedef struct
{
   UINT32 set;

} pICTL_DRVP_prioritySetT;




/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static void ICTL_DRVP_setPriorityLevel(UINT16 intNum, UINT16 priority);
static int  ICTL_DRVP_prNIsr(UINT32 prSetMask);
static int ICTL_DRVP_msb32(UINT32 val);




/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static ICTL_DRVG_isrT ICTLP_isrTbl[ICTLP_NUM_INT];
static pICTL_DRVP_prioritySetT ICTLP_prioTbl[ICTLGP_NUM_PRIO_E];
UINT32   ICTLP_spIsrCnt;
//	UINT32   ICTL_DRVG_deviceBaseAddress;



/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static int ICTL_DRVP_msb32(UINT32 val)
{
   //Simple implementation
   INT16 i;
   for(i = 31; i >= 0; i--)
   {
      if((val & ((UINT32)(1 << i))) != 0)
      {
         //return (31-i);
         return i;
      }
   }
   return 32;
}


static void ICTL_DRVP_setPriorityLevel(UINT16 intNum, UINT16 priority)
{
   volatile UINT32 *addrP;

//      ICTLP_assert((ICTL_IRQ_PR_0_REG & 0x3) == 0); //32-bit alignment

   addrP = (volatile UINT32 *)(((UINT32)ICTL2_IRQ_PR_0_REG) + (intNum * ICTL_IRQ_PR_0_IRQ_PR_0_LEN));
   *addrP = priority;
}



static int ICTL_DRVP_prNIsr(UINT32 prSetMask)
{
   //Read the final status register
   UINT32 status;
   UINT16 intNum;
   int ret = 0;

   //Check active interrupts
   status = ICTL2_IRQ_FINALSTATUS_L_VAL;
   //Filter interrupts not in this priority level
   status &= prSetMask;

   //Find first active interrupt - most significant bit.
   intNum = ICTL_DRVP_msb32(status);
   if(intNum < 32)
   {
      //Invoke ISR for this interrupt
      if(ICTLP_isrTbl[intNum].isr)
      {
         ICTL_DRVP_maskInt(intNum);
         ICTLP_isrTbl[intNum].isr();
         ICTL_DRVP_unMaskInt(intNum);
      }
   }
   else
   {
      ret = 1; //no active isr found
   }

   //LOGG_PRINT(LOG_DEBUG_E, NULL, "ret = %d status =%d prSetMask = %d \n", ret, status, prSetMask);
   return ret;
}




void ICTL_DRVP_maskInt(UINT16 intNum)
{
   UINT32 regVal = 0;

   //Mask interrupt
   regVal = ICTL2_IRQ_INTMASK_L_VAL;
   regVal |= ((UINT32)(1<< intNum));
   ICTL2_IRQ_INTMASK_L_VAL = regVal;
}


void ICTL_DRVP_unMaskInt(UINT16 intNum)
{
   UINT32 regVal = 0;

   //Mask interrupt
   regVal = ICTL2_IRQ_INTMASK_L_VAL;
   regVal &= ~((UINT32)(1<< intNum));
   ICTL2_IRQ_INTMASK_L_VAL = regVal;
}

#if 0
static void ICTL_DRVP_generateForceInt(UINT32 * intNum)
{
   UINT32 regVal;
   UINT32 localIntNum = *intNum;
   regVal = ICTL_IRQ_INTFORCE_L_VAL;
   regVal |= (UINT32)((1 << localIntNum));

   ICTL_IRQ_INTFORCE_L_VAL = regVal;
}

static void ICTL_DRVP_deRegisterIsr(UINT16 intNum)
{
   UINT32 regVal = 0;
   ICTLP_assert(intNum < ICTLP_NUM_INT);

   //Disable interrupt
   regVal = ICTL_IRQ_INTEN_L_VAL;
   regVal &= ~((UINT32)(1<< intNum));
   ICTL_IRQ_INTEN_L_VAL = regVal;

   ICTLP_isrTbl[intNum].isr = NULL;
   ICTLP_prioTbl[ICTLP_isrTbl[intNum].priority].set &=  ~(1 << intNum);
}
static void ICTL_DRVP_setPriorityFilter(UINT16 priority)
{
   ICTLP_assert(priority <= ICTLGP_NUM_PRIO_E);

   ICTL_IRQ_PLEVEL_IRQ_PLEVEL_W(priority);
}

#endif
UINT32 ICTL_DRVG_clearForceInt(UINT16 intNum)
{
   UINT32 regVal;

   regVal = ICTL2_IRQ_INTFORCE_L_VAL;
   regVal &= ~((UINT32)(1 << intNum));
   ICTL2_IRQ_INTFORCE_L_VAL = regVal;

   return ICTL2_IRQ_INTFORCE_L_VAL;
}



/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S     **************
 ****************************************************************************/
void ICTL_DRVG_initIctl(void)
{
   int i;

   //Disable all interrupts
   ICTL2_IRQ_INTEN_L_IRQ_INTEN_L_W(0x0);

   //Mask all interrupts
   ICTL2_IRQ_INTMASK_L_IRQ_INTMASK_L_W(0x0);

   //Set general priority level to lowest level value
   ICTL2_IRQ_PLEVEL_IRQ_PLEVEL_W(ICTLG_PRIO_0_E);

   //Set individual interrupt priority levels to lowest level
   for(i = 0; i < ICTLP_NUM_INT; i++)
   {
      ICTL_DRVP_setPriorityLevel(i, ICTLG_PRIO_0_E);
      ICTLP_isrTbl[i].isr = NULL;
   }

   //Set the default priority vector to NULL
   *(ICTL2_IRQ_VECTOR_DEFAULT_REG) = NULL;

   ICTLP_spIsrCnt = 0;
}

void ICTL_DRVG_registerIsr(ICTL_DRVG_isrT *registerParams)
{
   UINT32 regVal = 0;
   ICTL_DRVG_isrT *localRegisterParams = registerParams;
   
   //Mask interrupt
   regVal = ICTL2_IRQ_INTMASK_L_VAL;
   regVal |= ((UINT32)(1<< localRegisterParams->intNum));
   ICTL2_IRQ_INTMASK_L_VAL = regVal;

   //Add interrupt to internal table
   ICTLP_isrTbl[localRegisterParams->intNum].intNum = localRegisterParams->intNum;
   ICTLP_isrTbl[localRegisterParams->intNum].isr = localRegisterParams->isr;
   //Set interrupt priority in table
   ICTLP_isrTbl[localRegisterParams->intNum].priority = localRegisterParams->priority;

   //Add this interrupt to the priority level's set
   ICTLP_prioTbl[localRegisterParams->priority].set |= (1 << localRegisterParams->intNum);

   //Set interrupt's priority level in HW
   ICTL_DRVP_setPriorityLevel(localRegisterParams->intNum, localRegisterParams->priority);

   //Now enable
   regVal = ICTL2_IRQ_INTEN_L_VAL;
   regVal |= ((UINT32)(1<< localRegisterParams->intNum));
   ICTL2_IRQ_INTEN_L_VAL = regVal;

   ICTL_DRVP_unMaskInt(localRegisterParams->intNum);

   return;
}


//Called in IRQ

_Interrupt void ICTL_DRVG_irqHandler()
{
   UINT16 i;
   int ret;
	
   //Walk the priority levels by decreasing order
   for(i = 0; i < ICTLGP_NUM_PRIO_E; i++)
   {
      ret = ICTL_DRVP_prNIsr(ICTLP_prioTbl[i].set);
//         if(ret > 0)
//            break;
   }
   __asm__ __volatile__ ("dsync " : : : "memory"); //dsync to make sure we cleared the int
//      LOGG_PRINT(LOG_INFO_E, NULL, "interrupt\n");
	
}

