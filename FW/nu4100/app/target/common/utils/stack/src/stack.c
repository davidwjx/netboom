/****************************************************************************
 *
 *   FileName: stack.c
 *
 *   Author: 
 *
 *   Date:
 *
 *   Description: simple stack implementation
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_types.h"
#include "inu_stack.h"
#include "log.h"

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
INT32 STACKG_init(STACKG_stackT *stackP, UINT32 size)
{
   if (size > STACKG_MAX_ELEMENTS)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "size exceeds max stack size\n",size);
      return -1;
   }
   else
   {
      stackP->stackSize = size;
      stackP->top       = -1;
   }

   return 0;
}

ERRG_codeE  STACKG_push(STACKG_stackT *stackP,STACKG_elemT elem)
{
   ERRG_codeE           ret = ICC__RET_SUCCESS;

   if (stackP->top == (stackP->stackSize - 1))
   {
      ret = FAIL_E;
   }
   else
   {
      stackP->top++;
      stackP->stack[stackP->top] = elem;
   }   

   return ret;
}

ERRG_codeE  STACKG_pop(STACKG_stackT *stackP, STACKG_elemT *elemP)
{
   ERRG_codeE           ret = ICC__RET_SUCCESS;

   if (stackP->top == - 1)
   {
      elemP = NULL;
      ret = FAIL_E;
   }
   else
   {
      *elemP = stackP->stack[stackP->top];      
      stackP->top = stackP->top - 1;
   }
   
   return ret;
}

unsigned int STACKG_numElements(STACKG_stackT *stackP)
{
   return (stackP->top+1);
}

void STACKG_show(STACKG_stackT *stackP)
{
   INT32 i;
   
   for(i = 0; i < stackP->stackSize; i++)
   {      
      LOGG_PRINT(LOG_INFO_E, NULL, "[%d] %p\n", i, stackP->stack[i]);
   }
}


