/****************************************************************************
 * 
 *   FileName: msg_box.c
 *
 *   Author: Ram B
 *
 *   Date: 11/2012
 *
 *   Description: 2-process shared memory msg box 
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"
#include "utils.h"

#include <assert.h>

#include "mem_map.h"
#include "os_lyr.h"
#include "msg_box.h"

//Force optimize to 0 in all functions in this module to avoid  
//compiler optimizations causing instruction rerodering and other unexpected stuff.
#if DEFSG_IS_NOT_CEVA
#pragma GCC optimize ("O0") 
#endif

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define MSG_BOXP_READ_DONE          (0)
#define MSG_BOXP_WRITE_DONE         (1)
#define MSG_BOXP_DONE_SIZE          (2)  //Size of access field in bytes
#define MSG_BOXP_MAX_OF_MSGS        (16)
#define MSG_BOXP_MAX_NO_OFFSET      (0)
#define MSG_BOXP_MSG_SIZE           (256)//64
#define MSG_BOXP_MAX_MSG_BOX_SIZE   ((MSG_BOXP_MSG_SIZE+MSG_BOXP_DONE_SIZE)*MSG_BOXP_MAX_OF_MSGS) //(MSG_BOXP_MSG_SIZE*(MSG_BOXP_MAX_OF_MSGS+MSG_BOXP_DONE_SIZE))

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/

typedef struct MSG_BOXP_info
{   
   UINT8          *memStartP; //Physical or vitrual pointer to start of buffer
   UINT32         offset;  //offset from start of buffer.   
   UINT16         msgSize; //Fixed message size - in bytes
   UINT16         maxMsg;  //Max messages in queue
   volatile UINT8 *currP; //Current read or write pointer volatile and should be non-cachable.
   OS_LYRG_mutexT mutex;
} MSG_BOXP_infoT;

typedef struct
{
   MSG_BOXP_infoT reader;
   MSG_BOXP_infoT writer;
} MSG_BOXP_infoPairT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static UINT8 *MSG_BOXP_memBaseP = NULL;

static MSG_BOXP_infoPairT MSG_BOXP_instTbl[MSG_BOXG_NUM_INSTANCES_E] =
{
#if (DEFSG_PROCESSOR == DEFSG_CEVA)
   /* MSG_BOXG_GP_CVE1_E */   /* reader */ { {NULL, MSG_BOXP_MAX_NO_OFFSET,    MSG_BOXP_MSG_SIZE, MSG_BOXP_MAX_OF_MSGS, },  /* writer */ {NULL, MSG_BOXP_MAX_MSG_BOX_SIZE, MSG_BOXP_MSG_SIZE, MSG_BOXP_MAX_OF_MSGS, } },
   /* MSG_BOXG_LOOPBACK_E */  /* reader */ { {NULL, MSG_BOXP_MAX_NO_OFFSET,    MSG_BOXP_MSG_SIZE, MSG_BOXP_MAX_OF_MSGS, },  /* writer */ {NULL, MSG_BOXP_MAX_NO_OFFSET,    MSG_BOXP_MSG_SIZE, MSG_BOXP_MAX_OF_MSGS, } },
#elif (DEFSG_PROCESSOR == DEFSG_GP)
   /* MSG_BOXG_GP_CVE1_E */   /* reader */ { {NULL, MSG_BOXP_MAX_MSG_BOX_SIZE, MSG_BOXP_MSG_SIZE, MSG_BOXP_MAX_OF_MSGS, NULL,{}},  /* writer */ {NULL, MSG_BOXP_MAX_NO_OFFSET,    MSG_BOXP_MSG_SIZE, MSG_BOXP_MAX_OF_MSGS, NULL,{}} },
   /* MSG_BOXG_LOOPBACK_E */  /* reader */ { {NULL, MSG_BOXP_MAX_NO_OFFSET,    MSG_BOXP_MSG_SIZE, MSG_BOXP_MAX_OF_MSGS, NULL,{}},  /* writer */ {NULL, MSG_BOXP_MAX_NO_OFFSET,    MSG_BOXP_MSG_SIZE, MSG_BOXP_MAX_OF_MSGS, NULL,{}} },
#else
#error "Shared Memory non available for processor"
#endif
};
/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static void                MSG_BOXP_copy(volatile UINT8 *destP, const volatile UINT8 *srcP, UINT32 len);
static UINT16              MSG_BOXP_instToIdx(MSG_BOXG_instanceE inst);
static void                MSG_BOXP_incPtr(MSG_BOXP_infoT *msgBoxP);
static void                MSG_BOXP_resetPtr(MSG_BOXP_infoT *msgBoxP);
static MSG_BOXP_infoPairT  *MSG_BOXP_initInst(MSG_BOXG_instanceE inst);
static void                MSG_BOXP_setDone(UINT8 dir, volatile UINT8 *bufP);
static UINT8               MSG_BOXP_getDone(volatile UINT8 *bufP);
static void                MSG_BOXP_initMem(MSG_BOXP_infoT *msgBoxP);
static ERRG_codeE          MSG_BOXP_read(MSG_BOXG_handleT handle, void *bufP, UINT16 len, INT32 isRemove);
static void                MSG_BOXP_showMem(MSG_BOXP_infoT *msgBoxP);

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: MSG_BOXP_copy
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
static void MSG_BOXP_copy(volatile UINT8 *destP, const volatile UINT8 *srcP, UINT32 len)
{
   UINT32 i;
   for(i = 0; i < len; i++)
   {
      destP[i] = srcP[i];
   }
}

/****************************************************************************
*
*  Function Name: MSG_BOXP_instToIdx
*
*  Description:
*  Translation function - msg box instance to internal index
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
static UINT16 MSG_BOXP_instToIdx(MSG_BOXG_instanceE inst)
{
   assert(inst < MSG_BOXG_NUM_INSTANCES_E);
   return inst;
}

/****************************************************************************
*
*  Function Name: MSG_BOXP_incPtr
*
*  Description:
*  Advance msg box pointer
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
static void MSG_BOXP_incPtr(MSG_BOXP_infoT *msgBoxP)
{
   if((msgBoxP->currP + (msgBoxP->msgSize + MSG_BOXP_DONE_SIZE)) <  
      (UINT8 *)msgBoxP->memStartP + (msgBoxP->maxMsg * (msgBoxP->msgSize + MSG_BOXP_DONE_SIZE)) )
   {
      msgBoxP->currP += (msgBoxP->msgSize + MSG_BOXP_DONE_SIZE);
   }
   else
   {
      msgBoxP->currP = (UINT8 *)msgBoxP->memStartP; //wrap-around      
   }
}

/****************************************************************************
*
*  Function Name: MSG_BOXP_resetPtr
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
static void MSG_BOXP_resetPtr(MSG_BOXP_infoT *msgBoxP)
{
   msgBoxP->currP = (UINT8 *)msgBoxP->memStartP; //wrap-around
}

/****************************************************************************
*
*  Function Name: MSG_BOXP_initInst
*
*  Description:
*  Initialize a msg box table entry
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
static MSG_BOXP_infoPairT *MSG_BOXP_initInst(MSG_BOXG_instanceE inst)
{
   MSG_BOXP_infoPairT *msgBoxPairP = NULL;
   MSG_BOXP_infoT *msgBoxP;

   msgBoxPairP = &MSG_BOXP_instTbl[MSG_BOXP_instToIdx(inst)];

   assert(msgBoxPairP);
   assert(MSG_BOXP_memBaseP);

   //Reader
   msgBoxP = &msgBoxPairP->reader;
   msgBoxP->memStartP = MSG_BOXP_memBaseP + msgBoxP->offset;
   msgBoxP->currP = msgBoxP->memStartP;   
   if(OS_LYRG_aquireMutex(&(msgBoxP->mutex)) == FAIL_E)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "MSGBOX: Failed to create mutex\n");
      return NULL;
   }

   //LOGG_PRINT(LOG_ERROR_E, NULL, "MSGBOX: RD: start=%p(offset=%u curr=%p), msgSize=%u, maxMsg=%u\n", msgBoxP->memStartP,msgBoxP->offset, msgBoxP->currP, 
   //      msgBoxP->msgSize, msgBoxP->maxMsg);

   //Writer
   msgBoxP = &msgBoxPairP->writer;
   msgBoxP->memStartP = MSG_BOXP_memBaseP + msgBoxP->offset;
   msgBoxP->currP = msgBoxP->memStartP;   
   if(OS_LYRG_aquireMutex(&(msgBoxP->mutex)) == FAIL_E)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "MSGBOX: Failed to create mutex\n");
      return NULL;
   }

   //LOGG_PRINT(LOG_ERROR_E, NULL, "MSGBOX: WR: start=%p(offset=%u curr=%p), msgSize=%u, maxMsg=%u\n", msgBoxP->memStartP,msgBoxP->offset, msgBoxP->currP, 
   //      msgBoxP->msgSize, msgBoxP->maxMsg);

   //Only writer initializes shared memory
   MSG_BOXP_initMem(msgBoxP);
   //UTILSG_dump_hex(msgBoxP->memStartP, (msgBoxP->maxMsg * (msgBoxP->msgSize + MSG_BOXP_DONE_SIZE)));
   return msgBoxPairP;
}

/****************************************************************************
*
*  Function Name: MSG_BOXP_setDone
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
static void MSG_BOXP_setDone(UINT8 dir, volatile UINT8 *bufP)
{
   //This needs to be atomic. Using 8-bit value to ensure this.
   *bufP = dir;
}

/****************************************************************************
*
*  Function Name: MSG_BOXP_getDone
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
static UINT8 MSG_BOXP_getDone(volatile UINT8 *bufP)
{
   //This needs to be atomic. Using 8-bit value to ensure this.
   return *bufP;
}

//Initialize msg box memory
/****************************************************************************
*
*  Function Name: MSG_BOXP_initMem
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
static void MSG_BOXP_initMem(MSG_BOXP_infoT *msgBoxP)
{
   INT32 i,j;

   MSG_BOXP_resetPtr(msgBoxP);
   for(i = 0; i < msgBoxP->maxMsg; i++)
   {
      for(j = 0; j < (MSG_BOXP_DONE_SIZE + msgBoxP->msgSize); j++)
      {
         msgBoxP->currP[j] = 0x00;
      }
      //Initialize to read-done meaning the msg box is empty.
      MSG_BOXP_setDone(MSG_BOXP_READ_DONE, msgBoxP->currP);
      MSG_BOXP_incPtr(msgBoxP);
   }
   MSG_BOXP_resetPtr(msgBoxP);
}

/****************************************************************************
*
*  Function Name: MSG_BOXP_read
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
static ERRG_codeE MSG_BOXP_read(MSG_BOXG_handleT handle, void *bufP, UINT16 len, INT32 isRemove)
{
   ERRG_codeE     ret      = MSG_BOX__RET_SUCCESS;
   MSG_BOXP_infoT *msgBoxP = &((MSG_BOXP_infoPairT *)handle)->reader;

   if(MSG_BOXP_getDone(msgBoxP->currP) != MSG_BOXP_WRITE_DONE)
   {
      ret = MSG_BOX__ERR_BOX_IS_EMPTY;
   }

   if(ERRG_SUCCEEDED(ret))
   {
      //Copy message data to the buffer
      MSG_BOXP_copy(bufP, &msgBoxP->currP[MSG_BOXP_DONE_SIZE], MIN(msgBoxP->msgSize,len));

      //TODO: memory barrier required here - to avoid LOAD-STORE reordering. To ensure the write-done is set after the data read.
      //For x86: asm volatile("mfence" ::: "memory");
      //Using pthread mutex lock will force memory barrier here (the mutex is not used for inter-processor synchronization).
      OS_LYRG_lockMutex(&msgBoxP->mutex);   

      if(isRemove)
      {
         MSG_BOXP_setDone(MSG_BOXP_READ_DONE, msgBoxP->currP);
      }

      OS_LYRG_unlockMutex(&msgBoxP->mutex);

      if(isRemove)
      {
         MSG_BOXP_incPtr(msgBoxP);
      }
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: MSG_BOXP_showMem
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
static void MSG_BOXP_showMem(MSG_BOXP_infoT *msgBoxP)
{
   INT32 i;
   volatile UINT8 *readP;
   UINT16 msgOffset, doneOffset;

   readP = msgBoxP->memStartP;
   LOGG_PRINT(LOG_INFO_E, NULL, "Memory at %p\n", readP);
   for(i = 0; i < msgBoxP->maxMsg; i++)
   {
      msgOffset = i*(msgBoxP->msgSize+MSG_BOXP_DONE_SIZE);
      doneOffset = msgOffset+MSG_BOXP_DONE_SIZE;
      LOGG_PRINT(LOG_INFO_E, NULL, "Msg %d(%d %d):\n",i, msgOffset,doneOffset);
      UTILSG_dump_hex((UINT8 *)&readP[msgOffset], MSG_BOXP_DONE_SIZE);
      UTILSG_dump_hex((UINT8 *)&readP[doneOffset], msgBoxP->msgSize);
      LOGG_PRINT(LOG_INFO_E, NULL, "\n");
   }
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: MSG_BOXG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
ERRG_codeE MSG_BOXG_init(void)
{
   ERRG_codeE ret = MSG_BOX__RET_SUCCESS;

   //TODO: For now assuming region is mapped for entire life of process so no unmap is implemented.

   if(MSG_BOXP_memBaseP == NULL)
   {
      //Initialize the base pointer to msg box shared emory 
      //MEM_MAPG_getVirtAddr(MEM_MAPG_DDR_MSGBOX_E, (MEM_MAPG_addrT *)(&MSG_BOXP_memBaseP)); - todo

      if(MSG_BOXP_memBaseP == NULL)
      {
         ret = MSG_BOX__ERR_INIT_FAIL;
      }
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: MSG_BOXG_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
ERRG_codeE MSG_BOXG_open(MSG_BOXG_handleT *handleP, MSG_BOXG_instanceE inst)
{
   ERRG_codeE           ret = MSG_BOX__RET_SUCCESS;
   MSG_BOXP_infoPairT   *msgBoxPairP;

   //Initialize a message box
   msgBoxPairP = MSG_BOXP_initInst(inst);
   if(msgBoxPairP == NULL)
   {
      ret = MSG_BOX__ERR_OPEN_FAIL;
   }

   //Return a handle
   *handleP = (MSG_BOXG_handleT)msgBoxPairP;

   return ret;
}

/****************************************************************************
*
*  Function Name: MSG_BOXP_getMsgSize
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
UINT16 MSG_BOXP_getMsgSize(MSG_BOXG_handleT handle)
{
   MSG_BOXP_infoT *msgBoxP = &((MSG_BOXP_infoPairT *)handle)->writer;
   return (msgBoxP->msgSize); //Assuming msg size identical for writer/reader   
}

/****************************************************************************
*
*  Function Name: MSG_BOXG_add
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
ERRG_codeE MSG_BOXG_add(MSG_BOXG_handleT handle, const void *bufP, UINT16 len)
{
   ERRG_codeE     ret      = MSG_BOX__RET_SUCCESS;
   MSG_BOXP_infoT *msgBoxP = &((MSG_BOXP_infoPairT *)handle)->writer;

   if(MSG_BOXP_getDone(msgBoxP->currP) != MSG_BOXP_READ_DONE)
   {
      ret = MSG_BOX__ERR_ADD_MSG_FAIL_BOX_FULL;
   }

   if(ERRG_SUCCEEDED(ret))
   {
      //Copy message data to the queue
      MSG_BOXP_copy(&(msgBoxP->currP[MSG_BOXP_DONE_SIZE]), (const volatile UINT8 *)bufP, MIN(msgBoxP->msgSize,len));

      //TODO: memory barrier required here - to avoid STORE-STORE reordering. To ensure the write-done is set after the data write
      //For x86: asm volatile("mfence" ::: "memory");
      //Using pthread mutex lock will force memory barrier here (the mutex is not used for inter-processor synchronization).
      OS_LYRG_lockMutex(&msgBoxP->mutex);

      //Set the done field - must be done after copying data
      MSG_BOXP_setDone(MSG_BOXP_WRITE_DONE, msgBoxP->currP);   

      OS_LYRG_unlockMutex(&msgBoxP->mutex);

      MSG_BOXP_incPtr(msgBoxP);
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: MSG_BOXG_rem
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
ERRG_codeE MSG_BOXG_rem(MSG_BOXG_handleT handle, void *bufP, UINT16 len)
{
   //Read with removal from msg box
   return MSG_BOXP_read(handle, bufP, len, 1);
}

/****************************************************************************
*
*  Function Name: MSG_BOXG_peek
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
ERRG_codeE MSG_BOXG_peek(MSG_BOXG_handleT handle, void *bufP, UINT16 len)
{
   //Read without removal from msg box
   return MSG_BOXP_read(handle, bufP, len, 0);
}

/****************************************************************************
*
*  Function Name: MSG_BOXG_show
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: msg_box infrastructure
*
****************************************************************************/
void MSG_BOXG_show(UINT8 isWriter)
{
   MSG_BOXP_infoPairT   *msgBoxTableP = &MSG_BOXP_instTbl[MSG_BOXG_GP_CVE1_E];
   MSG_BOXP_infoT       *msgBoxP;

   if(isWriter)
      msgBoxP = &msgBoxTableP->writer;
   else
      msgBoxP = &msgBoxTableP->reader;

   LOGG_PRINT(LOG_ERROR_E, NULL, "MSG Box dir=%d maxMsg=%d msgSize=%d memStart=%p\n", MSG_BOXG_RD_E, msgBoxP->maxMsg, msgBoxP->msgSize, msgBoxP->memStartP);
   MSG_BOXP_showMem(msgBoxP);
}

