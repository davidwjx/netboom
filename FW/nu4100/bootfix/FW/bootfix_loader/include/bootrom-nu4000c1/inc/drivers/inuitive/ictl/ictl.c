/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "common.h"

#include "nu_regs.h"
#include "ictl.h"
/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define ICTLP_NUM_INT (64)
#define ICTLP_NUM_HW_PRIORITIES (16)

#define ICTLN(name)  ICTL0_ ## name
#define ICTLP_REG_PR0_ADDR                 (ICTLN(BASE) + ICTL_IRQ_PR_0_OFFSET)
#define ICTLP_REG_PRN_SIZE                 (4)
#define ICTLP_REG_VECTOR0_ADDR             (ICTLN(BASE) + 0x40)
#define ICTLP_REG_VECTOR_STEP              (8)
#define ICTLP_REG_VECTOR_DEFAULT_ADDR      (ICTLN(BASE) + 0x1e8)

#define ICTLP_EN_CHECKS      (1)

#if ICTLP_EN_CHECKS
   #define ICTLP_assert(cond) {if((cond) == 0) while(1);}
#else
   #define ICTLP_assert(cond)
#endif

#define ICTLP_USE_HW_VECTOR   (1)

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
typedef void (*ICTLP_prioVecT)(void);

typedef struct
{
   UINT16 intNum;
   UINT16 priority;
   ICTL_isrT isr;

} ICTLP_isrT;

typedef struct
{
   UINT32 setL;
   UINT32 setH;

} ICTLP_prioritySetT;
/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static void ICTLP_setPriorityLevel(UINT16 intNum, UINT16 priority);
static void ICTLP_setPrioLevelVector(UINT16 priority, UINT32 val);
static int ICTLP_prNIsr(ICTLP_prioritySetT *setP);
static void ICTLP_pr0Isr(void);
static void ICTLP_pr1Isr(void);
static void ICTLP_pr2Isr(void);
static void ICTLP_spuriousIsr(void);

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static ICTLP_isrT ICTLP_isrTbl[ICTLP_NUM_INT];
static ICTLP_prioritySetT ICTLP_prioTbl[ICTLGP_NUM_PRIO_E];
UINT32 ICTLP_spIsrCnt;


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
#if 0
static int ICTLP_msb32(UINT32 val)
{
   //Simple implementation
   UINT16 i;
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
#else
static inline int ICTLP_msb32(UINT32 val)
{
   //Use ARM's CLZ instruction (count leading zeros) - supported in ARMv5 and above.
   int cnt;
   int ret;

   __asm__ __volatile__(" clz %[cnt], %[val] ": [cnt] "=r" (cnt) : [val] "r" (val));

   if(cnt == 32)
   {
      ret = cnt;
   }
   else
   {
      ret = 31-cnt;
   }
   return ret;
}
#endif

static void ICTLP_setPriorityLevel(UINT16 intNum, UINT16 priority)
{
   volatile UINT32 *addrP;

   ICTLP_assert((ICTLP_REG_PR0_ADDR & 0x3) == 0); //32-bit alignment

   addrP = (volatile UINT32 *)((ICTLP_REG_PR0_ADDR) + (intNum * ICTLP_REG_PRN_SIZE));
   *addrP = priority;
}

static void ICTLP_setPrioLevelVector(UINT16 priority, UINT32 val)
{
   volatile UINT32 *addrP;

   ICTLP_assert((ICTLP_REG_VECTOR0_ADDR & 0x3) == 0); //32-bit alignment

   addrP = (volatile UINT32 *)((ICTLP_REG_VECTOR0_ADDR) + (priority * ICTLP_REG_VECTOR_STEP));
   *addrP = val;
}

#if 0
static UINT32 ICTLP_getPrVector(UINT16 priority)
{
   volatile UINT32 *addrP;

   ICTLP_assert((ICTLP_REG_VECTOR0_ADDR & 0x3) == 0); //32-bit alignment
   ICTLP_assert(priority < ICTLP_MAX_HW_PRIORITY);

   addrP = (volatile UINT32 *)((ICTLP_REG_VECTOR0_ADDR) + (priority * ICTLP_REG_VECTOR_STEP));

   return *addrP;
}
#endif

static int ICTLP_prNIsr(ICTLP_prioritySetT *setP)
{
   //Read the final status register
   UINT32 status;
   UINT16 intNum;
   int ret = 0;

   //Check active interrupts for high 32 
   status= ICTLN(IRQ_FINALSTATUS_H_VAL);
   //Filter interrupts not in this priority level
   status &= (setP->setH);

   //Find first active interrupt - most significant bit.
   intNum = ICTLP_msb32(status);
   if(intNum < 32)
   {
      //Invoke ISR for this interrupt
      if(ICTLP_isrTbl[intNum+32].isr)
      {
         ICTLP_isrTbl[intNum+32].isr();
      }
   }
   else
   {
      //No high interrupts - so check active interrupts for lower 32 
      status= ICTLN(IRQ_FINALSTATUS_L_VAL);
      //Filter interrupts not in this priority level
      status &= (setP->setL);
      
      //Find first active interrupt - most significant bit.
      intNum = ICTLP_msb32(status);
      if(intNum < 32)
      {
         //Invoke ISR for this interrupt
         if(ICTLP_isrTbl[intNum].isr)
         {
            ICTLP_isrTbl[intNum].isr();
         }
      }
      else
      {
         ret = 1; //no active isr found
      }
   }
   return ret;
}

static void ICTLP_pr0Isr(void)
{
   int ret;

   ret = ICTLP_prNIsr(&ICTLP_prioTbl[ICTLG_PRIO_0_E]);
   if(ret)
   {
      ICTLP_spuriousIsr();
   }
}

static void ICTLP_pr1Isr(void)
{
   int ret;

   ret = ICTLP_prNIsr(&ICTLP_prioTbl[ICTLG_PRIO_1_E]);
   if(ret)
   {
      ICTLP_spuriousIsr();
   }
}

static void ICTLP_pr2Isr(void)
{
   int ret;

   ret = ICTLP_prNIsr(&ICTLP_prioTbl[ICTLG_PRIO_2_E]);
   if(ret)
   {
      ICTLP_spuriousIsr();
   }
}

static void ICTLP_prDbgIsr(void)
{
   //TODO: received interrupt on unused priority level- should not happen
   return;
}

static void ICTLP_spuriousIsr(void)
{
   ICTLP_spIsrCnt++;
   //TODO: detect interrupt cause
   return;
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
void ICTLG_init(void)
{
   int i;

   //Disable all interrupts
   ICTLN(IRQ_INTEN_L_IRQ_INTEN_L_W(0x0));
   ICTLN(IRQ_INTMASK_L_IRQ_INTMASK_L_W(0x0));
   ICTLN(IRQ_INTEN_H_IRQ_INTEN_H_W(0x0));
   ICTLN(IRQ_INTMASK_H_IRQ_INTMASK_H_W(0x0));

   //Set general priority level to lowest level value
   ICTLN(IRQ_PLEVEL_IRQ_PLEVEL_W(ICTLG_PRIO_0_E));

   //Set individual interrupt priority levels to lowest level
   for(i = 0; i < ICTLP_NUM_INT; i++)
   {
      ICTLP_setPriorityLevel(i, ICTLG_PRIO_0_E);
      ICTLP_isrTbl[i].isr = NULL;
   }

   //Set the priority vector values to NULL in HW
   for(i = 0; i < ICTLP_NUM_HW_PRIORITIES; i++)
   {
      ICTLP_setPrioLevelVector(i, NULL);
   }

   //Initialization of priority vector table in HW
   ICTLP_prioTbl[ICTLG_PRIO_0_E].setL    = 0x0;
   ICTLP_prioTbl[ICTLG_PRIO_0_E].setH    = 0x0;
   ICTLP_setPrioLevelVector(ICTLG_PRIO_0_E, (UINT32)ICTLP_pr0Isr);
   ICTLP_prioTbl[ICTLG_PRIO_1_E].setL    = 0x0;
   ICTLP_prioTbl[ICTLG_PRIO_1_E].setH    = 0x0;
   ICTLP_setPrioLevelVector(ICTLG_PRIO_1_E, (UINT32)ICTLP_pr1Isr);
   ICTLP_prioTbl[ICTLG_PRIO_2_E].setL    = 0x0;
   ICTLP_prioTbl[ICTLG_PRIO_2_E].setH    = 0x0;
   ICTLP_setPrioLevelVector(ICTLG_PRIO_2_E, (UINT32)ICTLP_pr2Isr);
   //Set remainder of HW vector table to debug function
   for(i = ICTLGP_NUM_PRIO_E; i < ICTLP_NUM_HW_PRIORITIES; i++)
   {
      ICTLP_setPrioLevelVector(i, (UINT32)ICTLP_prDbgIsr);
   }

   //Set the default priority vector to NULL
   *((volatile UINT32 *)(ICTLP_REG_VECTOR_DEFAULT_ADDR)) = NULL;

   ICTLP_spIsrCnt = 0;
}


void ICTLG_registerIsr(UINT16 intNum, ICTL_isrT isr, UINT16 priority)
{
   UINT32 regVal = 0;

   ICTLP_assert(intNum < ICTLP_NUM_INT);
   ICTLP_assert(priority < ICTLGP_NUM_PRIO_E);

   //Mask interrupt
   if(intNum < 32)
   {
      regVal = ICTLN(IRQ_INTMASK_L_VAL);
      regVal |= ((UINT32)(1<< intNum));
      ICTLN(IRQ_INTMASK_L_VAL) = regVal;
   }
   else
   {
      regVal = ICTLN(IRQ_INTMASK_H_VAL);
      regVal |= ((UINT32)(1<< (intNum-32)));
      ICTLN(IRQ_INTMASK_H_VAL) = regVal;
   }

   //Add interrupt to internal table
   ICTLP_isrTbl[intNum].intNum = intNum;
   ICTLP_isrTbl[intNum].isr = isr;
   //Set interrupt priority in table
   ICTLP_isrTbl[intNum].priority = priority;

   //Add this interrupt to the priority level's set
   if(intNum < 32)
      ICTLP_prioTbl[priority].setL |= (1 << intNum);
   else
      ICTLP_prioTbl[priority].setH |= (1 << (intNum-32));

   //Set interrupt's priority level in HW
   ICTLP_setPriorityLevel(intNum, priority);

   //Now enable
   if(intNum < 32)
   {
      regVal = ICTLN(IRQ_INTEN_L_VAL);
      regVal |= ((UINT32)(1<< intNum));
      ICTLN(IRQ_INTEN_L_VAL) = regVal;
   }
   else
   {
      regVal = ICTLN(IRQ_INTEN_H_VAL);
      regVal |= ((UINT32)(1<< (intNum-32)));
      ICTLN(IRQ_INTEN_H_VAL) = regVal;
   }
   //NOTE: To start interrupt processing UNMASK the interrupt

   return;
}

void ICTLG_deRegisterIsr(UINT16 intNum)
{
   UINT32 regVal = 0;
   ICTLP_assert(intNum < ICTLP_NUM_INT);

   //Disable interrupt
   if(intNum < 32)
   {
      regVal = ICTLN(IRQ_INTEN_L_VAL);
      regVal &= ~((UINT32)(1<< intNum));
      ICTLN(IRQ_INTEN_L_VAL) = regVal;
   }
   else
   {
      regVal = ICTLN(IRQ_INTEN_H_VAL);
      regVal |= ((UINT32)(1<< (intNum-32)));
      ICTLN(IRQ_INTEN_H_VAL) = regVal;
   }

   ICTLP_isrTbl[intNum].isr = NULL;
   if(intNum < 32)
      ICTLP_prioTbl[ICTLP_isrTbl[intNum].priority].setL &= ~(1 << intNum);
   else
      ICTLP_prioTbl[ICTLP_isrTbl[intNum].priority].setH &= ~(1 << (intNum-32));
}


void ICTLG_maskInt(UINT16 intNum)
{
   UINT32 regVal = 0;
   ICTLP_assert(intNum < ICTLP_NUM_INT);

   //Mask interrupt
   if(intNum < 32)
   {
      regVal = ICTLN(IRQ_INTMASK_L_VAL);
      regVal |= ((UINT32)(1<< intNum));
      ICTLN(IRQ_INTMASK_L_VAL) = regVal;
   }
   else
   {
      regVal = ICTLN(IRQ_INTMASK_H_VAL);
      regVal |= ((UINT32)(1<< (intNum-32)));
      ICTLN(IRQ_INTMASK_H_VAL) = regVal;
   }
}

void ICTLG_unMaskInt(UINT16 intNum)
{
   UINT32 regVal = 0;
   ICTLP_assert(intNum < ICTLP_NUM_INT);

   //Mask interrupt
   if(intNum < 32)
   {
      regVal = ICTLN(IRQ_INTMASK_L_VAL);
      regVal &= ~((UINT32)(1<< intNum));
      ICTLN(IRQ_INTMASK_L_VAL) = regVal;
   }
   else
   {
      regVal = ICTLN(IRQ_INTMASK_H_VAL);
      regVal &= ~((UINT32)(1<< (intNum-32)));
      ICTLN(IRQ_INTMASK_H_VAL) = regVal;
   }
}

//Called in IRQ
void ICTLG_irqHandler(void)
{
#if ICTLP_USE_HW_VECTOR
   UINT32 regVal;

   //Read the IRQ vector value
   regVal = ICTLN(IRQ_VECTOR_VAL);
   if(regVal != NULL)
   {
      //Invoke the vector
      ((ICTLP_prioVecT)regVal)();
   }
   else
   {
      //If returned the default vector (NULL)- nothing is active and this is a spurious interrupt
      ICTLP_spuriousIsr(); //handle spurious ISRs
   }

#else
   UINT16 i;
   int ret;

   //Walk the priority levels by decreasing order
   for(i = 0; i < ICTLGP_NUM_PRIO_E; i++)
   {
      ret = ICTLP_prNIsr(&ICTLP_prioTbl[i]);
      if(ret > 0)
         break;
   }

   if(i == ICTLGP_NUM_PRIO_E)
   {
      ICTLP_spuriousIsr(); //handle spurious ISRs
   }
#endif
}

void ICTLG_generateForceInt(UINT16 intNum)
{
   UINT32 regVal;
   if(intNum < 32)
   {
      regVal = ICTLN(IRQ_INTFORCE_L_VAL);
      regVal |= (UINT32)((1 << intNum));
      ICTLN(IRQ_INTFORCE_L_VAL) = regVal;
   }
   else
   {
      regVal = ICTLN(IRQ_INTFORCE_H_VAL);
      regVal |= (UINT32)((1 << (intNum-32)));
      ICTLN(IRQ_INTFORCE_H_VAL) = regVal;
   }
}

void ICTLG_clearForceInt(UINT16 intNum)
{
   UINT32 regVal;

   if(intNum < 32)
   {
      regVal = ICTLN(IRQ_INTFORCE_L_VAL);
      regVal &= ~((UINT32)(1 << intNum));
      ICTLN(IRQ_INTFORCE_L_VAL) = regVal;
   }
   else
   {
      regVal = ICTLN(IRQ_INTFORCE_H_VAL);
      regVal &= ~((UINT32)(1 << (intNum-32)));
      ICTLN(IRQ_INTFORCE_H_VAL) = regVal;
   }
}

void ICTLG_setPriorityFilter(UINT16 priority)
{
   ICTLP_assert(priority <= ICTLGP_NUM_PRIO_E);

   ICTLN(IRQ_PLEVEL_IRQ_PLEVEL_W(priority));
}

