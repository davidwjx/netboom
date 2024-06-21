/****************************************************************************
 *
 *   FileName: mem_pool.c
 *
 *   Author:
 *
 *   Date:
 *
 *   Description:
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_types.h"
#include "err_defs.h"

#include <stdlib.h>
#include <stddef.h>
#if DEFSG_IS_OS_LINUX
#include <string.h>
#endif
#include <assert.h>
#if DEFSG_IS_GP
#include <sys/mman.h>
#endif
#include "log.h"
#include "os_lyr.h"
#include "mem_pool.h"


/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define MEM_POOLP_SLEEP_USEC        (1000*5)
#define MEM_POOLP_TIMEOUT_USEC      (MEM_POOLP_SLEEP_USEC *100)


#define MEM_POOLP_MAX_POOLS                    (400)

//Memory Pool
#define MEM_POOLP_BUF_STATUS_USED              (1)
#define MEM_POOLP_BUF_STATUS_OFFSET            (16)
#define MEM_POOLP_BUF_MAGIC_MASK               (0x0000FFFF)
#define MEM_POOLP_BUF_MAGIC_VAL                (0xa5a5)
#define MEM_POOLP_BUF_RSVD_BYTES               (sizeof(UINT32))
#define MEM_POOLP_FREE_BUF_TBL_SIZE            (100)

#define MEM_POOLP_CMEM_HEAP_SPLIT
#define MEM_POOLP_BUFSIZE_ALIGNMENT            (4096)
#define MEM_POOLP_ALIGN_SIZE(buf, alignment)   (((buf) + ((alignment) - 1)) & ~((alignment)-1))

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
typedef enum
{
   MEM_POOLP_BUFF_STATUS_FREE_E          = 0,
   MEM_POOLP_BUFF_STATUS_READY_E         = 1,
   MEM_POOLP_BUFF_STATUS_OPERATION_E     = 2,
   MEM_POOLP_BUFF_STATUS_DONE_E          = 3
} MEM_POOLP_buffStatusE;

typedef struct
{
   MEM_POOLP_buffStatusE      status;
   const char*                name;
} MEM_POOLP_buffConsumerE;

typedef struct
{
   //private stuff
   MEM_POOLG_handleT    pool;//Buffer memory pool
   //public descriptor
   MEM_POOLG_bufDescT   pubDesc;
} MEM_POOLP_privDescT;


static MEM_POOLP_sharedMallocFuncT *MEM_POOLP_sharedMallocFunc=NULL;
static MEM_POOLP_sharedFreeFuncT   *MEM_POOLP_sharedFreecFunc=NULL;

typedef struct
{
   ERRG_codeE (*alloc)(MEM_POOLG_handleT handle, UINT32 size, MEM_POOLG_bufDescT **bufDescP);
   void (*free)(MEM_POOLG_handleT poolP, UINT32 *bufP);

} MEM_POOLP_funcsT;

//Simple static memory pool - one-size buffers.
//Maintain buffer state, in use or free, at start of buffer.
//On free mark buffer as free and add to free table to be the next allocated.
//On allocate use last freed buffer if available or do linear search over buffer list. Then mark state as in use.
typedef struct
{
   MEM_POOLG_typeE         type;
   UINT32                  numBuffers;
   UINT32                  bufferSize;
   UINT8                   *freeTbl[MEM_POOLP_FREE_BUF_TBL_SIZE];
   UINT32                  freeTblIdx;
   void                    *refPtr; // updated only for output streams
   MEM_POOLG_freeCbT       freeCb;
   void                    *freeArg;
   UINT8                   *bufMemP;
   OS_LYRG_mutexT          mutex;
   BOOL                    resetBufPtrInAlloc;
   const MEM_POOLP_funcsT  *funcTblP;
   UINT8                   *memP;
   UINT32                  memBuffIndex;
   UINT32                  bufPysicalAddr;
   void                    *cmemMemP;
   void                    *kerMemP;
   INT32                   DMABuf_FD;
#ifdef MEM_POOLG_BUFF_TRACE
   MEM_POOLG_bufDescT      *buffList[MEM_POOLG_BUFF_TRACE_MAX_BUFFS];
   MEM_POOLP_buffConsumerE buffConsumers[MEM_POOLG_BUFF_TRACE_MAX_BUFFS][MEM_POOLG_BUFF_TRACE_MAX_CONSUMERS];
#endif
} MEM_POOLP_memPoolT;

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static ERRG_codeE  MEM_POOLP_memPoolInit(MEM_POOLG_handleT handle);
static ERRG_codeE MEM_POOLP_allocBuf(MEM_POOLG_handleT handle, UINT32 size, MEM_POOLG_bufDescT **bufDescP);
static void       MEM_POOLP_free(MEM_POOLG_handleT handle, UINT32 *bufP);
static void       MEM_POOLP_freeAll(MEM_POOLG_handleT handle);
static void       MEM_POOLP_closePool(MEM_POOLG_handleT handle);
static void       MEM_POOLP_freeBuf(MEM_POOLG_bufDescT *bufDescP);
static void       MEM_POOLP_show(MEM_POOLG_handleT handle);

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static const MEM_POOLP_funcsT MEM_POOLP_funcTable[MEM_POOLG_NUM_TYPES_E] =
{
   { MEM_POOLP_allocBuf, MEM_POOLP_free},
   { MEM_POOLP_allocBuf, MEM_POOLP_free},
   { MEM_POOLP_allocBuf, MEM_POOLP_free},
   { MEM_POOLP_allocBuf, MEM_POOLP_free},
   { MEM_POOLP_allocBuf, MEM_POOLP_free}
};

static MEM_POOLP_memPoolT MEM_POOLP_memPool[MEM_POOLP_MAX_POOLS];

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/
#if (DEFSG_PROCESSOR == DEFSG_GP)
extern UINT32 MEM_MAPG_getMemRegionTable();
#endif
/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static MEM_POOLP_memPoolT *MEM_POOLP_getPoolH(MEM_POOLG_bufDescT *descP)
{
   UINT32               *bufP;
   MEM_POOLP_privDescT  *descrP;

   //Get buffer start and mem pool handle
   bufP     = (UINT32 *)((UINT8 *)descP - offsetof(MEM_POOLP_privDescT, pubDesc) - MEM_POOLP_BUF_RSVD_BYTES);
   descrP   = (MEM_POOLP_privDescT *)((UINT8 *)bufP + MEM_POOLP_BUF_RSVD_BYTES);
   return (MEM_POOLP_memPoolT *)descrP->pool;
}

void MEM_POOLG_assignShardCBFunc(MEM_POOLP_sharedMallocFuncT *MEM_POOLP_sharedMallocFuncPtr, MEM_POOLP_sharedFreeFuncT *MEM_POOLP_sharedFreeFunc)
{
   MEM_POOLP_sharedMallocFunc = MEM_POOLP_sharedMallocFuncPtr;
   MEM_POOLP_sharedFreecFunc  = MEM_POOLP_sharedFreeFunc;
}

void *MEM_POOLP_sharedMalloc(unsigned int size, void *refPtr)
{

   assert(MEM_POOLP_sharedMallocFunc != NULL);
   return MEM_POOLP_sharedMallocFunc(size,refPtr);
}

void MEM_POOLP_sharedFree(void *ptr,void *refPtr)
{
   assert(MEM_POOLP_sharedFreecFunc != NULL);
   return MEM_POOLP_sharedFreecFunc(ptr,refPtr);
}

/****************************************************************************
*
*  Function Name: MEM_POOLP_allocBuf
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: memory
*
****************************************************************************/
static ERRG_codeE MEM_POOLP_allocBuf(MEM_POOLG_handleT handle, UINT32 size, MEM_POOLG_bufDescT **bufDescP)
{
   ERRG_codeE           ret = MEM_POOL__RET_SUCCESS;
   MEM_POOLP_privDescT  *descrP;
   UINT32               *bufP;
   UINT32               bufSize;
   UINT32               i;
   MEM_POOLP_memPoolT   *memPoolP = (MEM_POOLP_memPoolT *)handle;
   *bufDescP = NULL;
   if(memPoolP == NULL)
       return MEM_POOL__ERR_ALLOC_BUF_FAIL_NULL_HANDLE;
   
   if (size > memPoolP->bufferSize)
       return MEM_POOL__ERR_ALLOC_BUF_FAIL_BIG_BUF_SIZE;
    

         OS_LYRG_lockMutex(&memPoolP->mutex);
         if(memPoolP->freeTblIdx)
         {
            //Access free table in LIFO order
            bufP     = (UINT32 *)(memPoolP->freeTbl[memPoolP->freeTblIdx-1]);
            *bufP    |= (1<<MEM_POOLP_BUF_STATUS_OFFSET);
            descrP   = (MEM_POOLP_privDescT *)(((UINT8 *)bufP) + MEM_POOLP_BUF_RSVD_BYTES);
            //Set start of user accessible buffer
#ifndef MEM_POOLP_CMEM_HEAP_SPLIT
            if(memPoolP->type != MEM_POOLG_TYPE_ALLOC_USER_E)
            {
               descrP->pubDesc.dataP = (UINT8 *)bufP + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
            }
#else
            if((memPoolP->type == MEM_POOLG_TYPE_ALLOC_HEAP_E)||(memPoolP->type == MEM_POOLG_TYPE_ALLOC_SHARED_E))
            {
               descrP->pubDesc.dataP = (UINT8 *)bufP + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
            }
            else if((memPoolP->type == MEM_POOLG_TYPE_ALLOC_CMEM_E)||(memPoolP->type == MEM_POOLG_TYPE_ALLOC_KERSPC_E))
            {
               descrP->pubDesc.dataP = descrP->pubDesc.startP;
            }
#endif

            descrP->pubDesc.dataLen = 0;
            descrP->pubDesc.nextP   = NULL;
            descrP->pubDesc.refCntr = 1;
            if(memPoolP->DMABuf_FD > 0)
            {
               LOGG_PRINT(LOG_DEBUG_E,NULL,"DMABUF_FD: %d \n",  memPoolP->DMABuf_FD);
               descrP->pubDesc.fileDescriptor = memPoolP->DMABuf_FD;
            }
            else
            {
               descrP->pubDesc.fileDescriptor = 0;
            }

            //Output pointer to buffer descriptor
            *bufDescP = &(descrP->pubDesc);

            if(memPoolP->resetBufPtrInAlloc == TRUE)
            {
               (*bufDescP)->dataP = (*bufDescP)->startP;;
            }

            //One less free
            memPoolP->freeTblIdx--;
         }
         else
         {
#ifndef MEM_POOLP_CMEM_HEAP_SPLIT
            if(memPoolP->type != MEM_POOLG_TYPE_ALLOC_USER_E)
            {
               bufSize = memPoolP->bufferSize + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
            }
            else
            {
               bufSize = MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
            }
#else
            if((memPoolP->type == MEM_POOLG_TYPE_ALLOC_HEAP_E)||(memPoolP->type == MEM_POOLG_TYPE_ALLOC_SHARED_E))
            {
               bufSize = memPoolP->bufferSize + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
            }
            else
            {
               bufSize = MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
            }
#endif

            //Linear search over memory pool for first free buffer
            for(i = 0; i < memPoolP->numBuffers; i++)
            {
               bufP =  (UINT32 *)(memPoolP->bufMemP + bufSize*i);
               //bufP =  (UINT32 *)(((UINT8 *)memPoolP->bufMemP) + bufSize*i);
               //Sanity check magic number
               //fprintf(stderr,"[%2d] %p 0x%08x F=%d M=0x%x\n",i, bufP, *bufP, ((*bufP & (1<<MEM_POOLP_BUF_STATUS_OFFSET)) == 0),(*bufP & MEM_POOLP_BUF_MAGIC_MASK) );
               assert((*bufP & MEM_POOLP_BUF_MAGIC_MASK) == MEM_POOLP_BUF_MAGIC_VAL);
               //Check free status
               if((*bufP & (1<<MEM_POOLP_BUF_STATUS_OFFSET)) == 0)
               {
                  //buffer is free, mark as used
                  *bufP |= (1<<MEM_POOLP_BUF_STATUS_OFFSET);
                  //Set start of user accessible buffer
                  descrP = (MEM_POOLP_privDescT *)(((UINT8 *)bufP) + MEM_POOLP_BUF_RSVD_BYTES);
#ifndef MEM_POOLP_CMEM_HEAP_SPLIT
                  if(memPoolP->type != MEM_POOLG_TYPE_ALLOC_USER_E)
                  {
                     descrP->pubDesc.dataP = ((UINT8 *)bufP) + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
                  }
#else
                  if((memPoolP->type == MEM_POOLG_TYPE_ALLOC_HEAP_E)||(memPoolP->type == MEM_POOLG_TYPE_ALLOC_SHARED_E))
                  {
                     descrP->pubDesc.dataP = (UINT8 *)bufP + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
                  }
                  else
                  {
                     descrP->pubDesc.dataP = descrP->pubDesc.startP;
                  }
#endif

                  descrP->pubDesc.dataLen    = 0;
                  descrP->pubDesc.nextP      = NULL;
                  descrP->pubDesc.refCntr    = 1;
                  //Output pointer to buffer descriptor
                  *bufDescP = &(descrP->pubDesc);
                  if(memPoolP->resetBufPtrInAlloc == TRUE)
                  {
                     (*bufDescP)->dataP = (*bufDescP)->startP;;
                  }
                  //fprintf(stderr, "     %p descrP pool=%p start=%p, size=%ld, dataP=%p, dataLen=%d\n", descrP, descrP->pool, descrP->pubDesc.startP, descrP->pubDesc.size, descrP->pubDesc.dataP, descrP->pubDesc.dataLen);
                  break;
               }
            }
            if(i == memPoolP->numBuffers)
            {
               ret = MEM_POOL__ERR_ALLOC_BUF_FAIL_NO_BUF_IN_POOL;
               *bufDescP = NULL; //No free buffers
            }
         }
         OS_LYRG_unlockMutex(&memPoolP->mutex);

   return (ret);
}

/****************************************************************************
*
*  Function Name: MEM_POOLP_free
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: memory
*
****************************************************************************/
static void MEM_POOLP_free(MEM_POOLG_handleT handle, UINT32 *bufP)
{
   MEM_POOLP_memPoolT   *memPoolP = (MEM_POOLP_memPoolT *)handle;
   MEM_POOLG_bufDescT   *bufDescP;

   //Sanity check magic number
   assert((*bufP & MEM_POOLP_BUF_MAGIC_MASK) == MEM_POOLP_BUF_MAGIC_VAL);

   //Sanify check free status
   assert(*bufP & (1<<MEM_POOLP_BUF_STATUS_OFFSET));
   //Mark as free
   *bufP &= ~(1<<MEM_POOLP_BUF_STATUS_OFFSET);

   //Add to free buffer table if room available
   if(memPoolP->freeTblIdx < MEM_POOLP_FREE_BUF_TBL_SIZE)
   {
      memPoolP->freeTbl[memPoolP->freeTblIdx]= (UINT8 *)bufP;
      memPoolP->freeTblIdx++;
   }


   if(memPoolP->freeCb)
   {
      //Invoke user's free function callback with public descriptor
      bufDescP = (MEM_POOLG_bufDescT *)((UINT8 *)bufP + offsetof(MEM_POOLP_privDescT, pubDesc) + MEM_POOLP_BUF_RSVD_BYTES);
      memPoolP->freeCb(memPoolP->freeArg, bufDescP);
   }
}

/****************************************************************************
*
*  Function Name: MEM_POOLP_memPoolInit
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: memory
*
****************************************************************************/
static ERRG_codeE MEM_POOLP_memPoolInit(MEM_POOLG_handleT handle)
{
   ERRG_codeE retVal=MEM_POOL__RET_SUCCESS;
   UINT32               i;
   void                 *memP = NULL;
#ifdef MEM_POOLP_CMEM_HEAP_SPLIT
void                    *cmemMemP = NULL;
#endif
   void                 *kerMemP=NULL;
   UINT32               *bufP;
   MEM_POOLP_privDescT  *descrP;
   UINT32               bufSize, memSize;
   MEM_POOLP_memPoolT   *poolP = (MEM_POOLP_memPoolT *)handle;

#if (DEFSG_PROCESSOR == DEFSG_GP)
    int ret=0;
#endif


#ifndef MEM_POOLP_CMEM_HEAP_SPLIT
   //Allocate memory.
   //NOTE: Not handling issues related to caching or continguity.
   if(poolP->type == MEM_POOLG_TYPE_ALLOC_USER_E )
   {
      bufSize =  MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
   else
   {
       bufSize =  poolP->bufferSize + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
#else
   //Allocate memory.
   //NOTE: Not handling issues related to caching or continguity.
   if((poolP->type == MEM_POOLG_TYPE_ALLOC_HEAP_E) || (poolP->type == MEM_POOLG_TYPE_ALLOC_SHARED_E))
   {
      bufSize =  poolP->bufferSize + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
   else
   {
      bufSize =  MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
#endif

#if (DEFSG_PROCESSOR == DEFSG_GP)
   //Assert buffer size is multiples of 4 bytes and force pointer size alignment
   assert((bufSize & 0x3) == 0);
   memSize = bufSize * poolP->numBuffers;
   if(poolP->type == MEM_POOLG_TYPE_ALLOC_HEAP_E|| poolP->type == MEM_POOLG_TYPE_ALLOC_USER_E)
   {
      ret = posix_memalign(&memP, sizeof(void *), memSize);
   }
   else if (poolP->type == MEM_POOLG_TYPE_ALLOC_CMEM_E)
   {
#ifndef MEM_POOLP_CMEM_HEAP_SPLIT
      memSize = bufSize * poolP->numBuffers;
#ifdef __UART_ON_FPGA__
      retVal = MEM_MAPG_alloc(memSize, &poolP->bufPysicalAddr, (UINT32*)&memP, 0);        //0 - no cache for FPGA
#else
      retVal = MEM_MAPG_alloc(memSize, &poolP->bufPysicalAddr, (UINT32*)&memP, 1);
#endif
      if (ERRG_FAILED(retVal))
      {
         assert(0);
      }
#else
      memSize = bufSize * poolP->numBuffers;
      ret = posix_memalign(&memP, sizeof(void *), memSize);

      retVal = MEM_MAPG_allocWithDMABuf(MEM_POOLP_ALIGN_SIZE(poolP->bufferSize, MEM_POOLP_BUFSIZE_ALIGNMENT) * poolP->numBuffers, &poolP->bufPysicalAddr, (UINT32*)&cmemMemP, 1, &poolP->DMABuf_FD); //todo - allow cache/uncache
      if (ERRG_FAILED(retVal))
      {
         assert(0);
      }
      poolP->cmemMemP = cmemMemP;
#endif
   }
   else if (poolP->type == MEM_POOLG_TYPE_ALLOC_KERSPC_E)
   {
      memSize = bufSize * poolP->numBuffers;
      ret = posix_memalign(&memP, sizeof(void *), memSize);
      retVal = MEM_MAPG_alloc(MEM_POOLP_ALIGN_SIZE(poolP->bufferSize, MEM_POOLP_BUFSIZE_ALIGNMENT) * poolP->numBuffers, &poolP->bufPysicalAddr, (UINT32*)&kerMemP, 2); //todo - allow cache/uncache
      poolP->kerMemP = (void *)kerMemP;

   }
   assert(ret == 0);
#endif

#if (DEFSG_PROCESSOR == DEFSG_HOST)
   if(poolP->type != MEM_POOLG_TYPE_ALLOC_CMEM_E) // shouldn't happen in host
   {
      memSize = bufSize  *  poolP->numBuffers;
      if (poolP->type == MEM_POOLG_TYPE_ALLOC_SHARED_E)
      {
         memP = MEM_POOLP_sharedMalloc(memSize,poolP->refPtr);
      }
      else
      {
         memP = malloc(memSize);
      }
   }

   assert(memP);
#endif
   //Initialize buffers in allocated memory
   for(i = 0; i <  poolP->numBuffers; i++)
   {
      bufP =  (UINT32 *)(((UINT8 *)memP) + bufSize*i);
      //Initialize buffer - set free with magic number set
      *bufP = MEM_POOLP_BUF_MAGIC_VAL;
      //Initialize buffer descriptor
      descrP = (MEM_POOLP_privDescT *)(((UINT8 *)bufP) + MEM_POOLP_BUF_RSVD_BYTES);
      descrP->pool = handle;
      //Initialize public descriptor
      descrP->pubDesc.dataLen = 0;
      descrP->pubDesc.dataP   = NULL;
      descrP->pubDesc.size    =  poolP->bufferSize;
      descrP->pubDesc.refCntr = 0;
#ifndef MEM_POOLP_CMEM_HEAP_SPLIT
      if(poolP->type != MEM_POOLG_TYPE_ALLOC_USER_E)
      {
         descrP->pubDesc.startP = (UINT8 *)bufP + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
      }
      else
      {
         descrP->pubDesc.startP = NULL;;
      }
#else
      if((poolP->type == MEM_POOLG_TYPE_ALLOC_HEAP_E)||(poolP->type == MEM_POOLG_TYPE_ALLOC_SHARED_E))
      {
         descrP->pubDesc.startP = (UINT8 *)bufP + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
      }
      else if ((poolP->type == MEM_POOLG_TYPE_ALLOC_CMEM_E) )
      {
         descrP->pubDesc.startP = (UINT8*)((UINT8*)cmemMemP + (MEM_POOLP_ALIGN_SIZE(poolP->bufferSize, MEM_POOLP_BUFSIZE_ALIGNMENT) * i));
      }
      else if (poolP->type == MEM_POOLG_TYPE_ALLOC_KERSPC_E)
      {

         descrP->pubDesc.startP = (UINT8*)((UINT8*)kerMemP + (MEM_POOLP_ALIGN_SIZE(poolP->bufferSize, MEM_POOLP_BUFSIZE_ALIGNMENT) * i));
       }
      else
      {
         descrP->pubDesc.startP = NULL;
      }
#endif
      descrP->pubDesc.nextP = NULL;
#ifdef MEM_POOLG_BUFF_TRACE
      if (i < MEM_POOLG_BUFF_TRACE_MAX_BUFFS)
      {
         poolP->buffList[i] = &descrP->pubDesc;
      }
#endif
   }

   poolP->bufMemP = (UINT8 *)memP;
   return retVal;
}

/****************************************************************************
*
*  Function Name: MEM_POOLP_freeAll
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: memory
*
****************************************************************************/
static void MEM_POOLP_freeAll(MEM_POOLG_handleT handle)
{
   MEM_POOLP_memPoolT   *memPoolP = (MEM_POOLP_memPoolT *)handle;
   MEM_POOLP_privDescT  *descrP;
   UINT32 i;
   UINT32 *bufP;
   UINT32 bufSize;

   if(memPoolP->type != MEM_POOLG_TYPE_ALLOC_USER_E)
   {
      bufSize = memPoolP->bufferSize + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
   else
   {
      bufSize = MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }

   OS_LYRG_lockMutex(&memPoolP->mutex);

   for(i = 0; i < memPoolP->numBuffers; i++)
   {
      bufP =  (UINT32 *)(memPoolP->bufMemP + bufSize*i);
      //Sanity check magic number
      assert((*bufP & MEM_POOLP_BUF_MAGIC_MASK) == MEM_POOLP_BUF_MAGIC_VAL);
      *bufP = MEM_POOLP_BUF_MAGIC_VAL;
      //Set start of user accessible buffer
      descrP = (MEM_POOLP_privDescT *)(((UINT8 *)bufP) + MEM_POOLP_BUF_RSVD_BYTES);
      descrP->pubDesc.dataP   = NULL;
      descrP->pubDesc.dataLen = 0;
      descrP->pubDesc.size    = memPoolP->bufferSize;
      descrP->pubDesc.refCntr = 0;
      if(memPoolP->type != MEM_POOLG_TYPE_ALLOC_USER_E)
      {
         descrP->pubDesc.startP  = (UINT8 *)bufP + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
      }
      else
      {
         descrP->pubDesc.startP = NULL;
      }
   }
   OS_LYRG_unlockMutex(&memPoolP->mutex);
}

#if ((DEFSG_PROCESSOR == DEFSG_GP) || (DEFSG_PROCESSOR == DEFSG_HOST))
/****************************************************************************
*
*  Function Name: MEM_POOLP_closePool
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: memory
*
****************************************************************************/
static void MEM_POOLP_closePool(MEM_POOLG_handleT handle)
{
   MEM_POOLP_memPoolT *memPoolP = (MEM_POOLP_memPoolT *)handle;

   //MEM_POOLG_show(handle);
   if(OS_LYRG_releaseMutex(&memPoolP->mutex) != SUCCESS_E)
   {
      fprintf(stderr, "MEM_POOL: Failed to destory mutex\n");
   }
   if(memPoolP->type == MEM_POOLG_TYPE_ALLOC_HEAP_E|| memPoolP->type ==MEM_POOLG_TYPE_ALLOC_USER_E)
   {
       free(memPoolP->bufMemP);
   }
   else if (memPoolP->type == MEM_POOLG_TYPE_ALLOC_SHARED_E)
   {
      MEM_POOLP_sharedFree(memPoolP->bufMemP,memPoolP->refPtr);
   }
   else
   {
#if (DEFSG_PROCESSOR == DEFSG_GP)
      ERRG_codeE retVal = MEM_POOL__RET_SUCCESS;
#ifndef MEM_POOLP_CMEM_HEAP_SPLIT
      if (memPoolP->type == MEM_POOLG_TYPE_ALLOC_CMEM_E)
      {
         retVal = MEM_MAPG_free((UINT32*)memPoolP->bufMemP);
         if(ERRG_FAILED(retVal))
         {
            assert(0);
         }
      }
#else
      if (memPoolP->type == MEM_POOLG_TYPE_ALLOC_CMEM_E)
      {
         retVal = MEM_MAPG_free((UINT32*)memPoolP->cmemMemP);
         if(ERRG_FAILED(retVal))
         {
            assert(0);
         }
         free(memPoolP->bufMemP);
      }
      else if (memPoolP->type == MEM_POOLG_TYPE_ALLOC_KERSPC_E)
      {
#if DEFSG_IS_GP
         munmap(memPoolP->kerMemP,memPoolP->bufferSize * memPoolP->numBuffers);
#endif
         free(memPoolP->bufMemP);
      }
#endif
#endif
   }
   memPoolP->bufMemP = NULL;
}
#endif




#if 0
 static BOOL MEM_POOLP_memPoolCheckFree(void)
 {
    int i;
    UINT32 *bufP;
    UINT32 bufSize;
    int numFree = 0;

    MEM_POOLP_memPoolT *memPoolP = &MEM_POOLP_memPool;

    bufSize = memPoolP->bufferSize + MEM_POOLP_BUF_RSVD_BYTES;

    for(i = 0; i < memPoolP->numBuffers; i++)
    {
       bufP =  (UINT32 *)(memPoolP->bufMemP + bufSize*i);

       if((*bufP & (1<<MEM_POOLP_BUF_STATUS_OFFSET)) == 0)
          numFree++;
       //Check free status
    }
    //fprintf(stderr,"Mempool: start=%p bufSize=%d num=%d free=%d\n", memPoolP->bufMemP, memPoolP->bufferSize, memPoolP->numBuffers, numFree);
    return (numFree == memPoolP->numBuffers);
 }
#endif

static void MEM_POOLP_freeBuf(MEM_POOLG_bufDescT *bufDescP)
{
   UINT32               *bufP;
   MEM_POOLP_privDescT  *descrP;
   MEM_POOLP_memPoolT   *memPoolP;

   //Get buffer start and mem pool handle
   bufP     = (UINT32 *)((UINT8 *)bufDescP - offsetof(MEM_POOLP_privDescT, pubDesc) - MEM_POOLP_BUF_RSVD_BYTES);
   descrP   = (MEM_POOLP_privDescT *)((UINT8 *)bufP + MEM_POOLP_BUF_RSVD_BYTES);
   memPoolP = (MEM_POOLP_memPoolT *)descrP->pool;

    if(memPoolP->bufMemP == NULL)
      assert(0);

   OS_LYRG_lockMutex(&memPoolP->mutex);

   if(bufDescP->refCntr)
   {
      bufDescP->refCntr--;
      if(bufDescP->refCntr == 0 && memPoolP->bufMemP)
      {
         //Invoke mem pool free function
         memPoolP->funcTblP->free((MEM_POOLG_handleT)memPoolP, bufP);
      }
   }
   else
   {
      //ref counter can not be 0 when free, unless there is a problem
      assert(0);
   }

   OS_LYRG_unlockMutex(&memPoolP->mutex);
}

static void MEM_POOLP_chaining(MEM_POOLG_bufDescT *headP, MEM_POOLG_bufDescT *tailP)
{
   MEM_POOLP_memPoolT   *memPoolP, *memPoolHeadP;
   MEM_POOLG_bufDescT   *bufDescP ;
   UINT32 refCntHead;

   memPoolHeadP = MEM_POOLP_getPoolH(headP);

   OS_LYRG_lockMutex(&memPoolHeadP->mutex);
   refCntHead = headP->refCntr; //head reference count

   bufDescP = tailP;
   while(bufDescP)
   {
      memPoolP = MEM_POOLP_getPoolH(bufDescP);

      if(memPoolP != memPoolHeadP)
         OS_LYRG_lockMutex(&memPoolP->mutex);

      if(bufDescP->refCntr < refCntHead)
      {
         bufDescP->refCntr = refCntHead; //elevate ref cnt
         bufDescP = bufDescP->nextP;
      }
      else
      {
         bufDescP = NULL;//quit
      }

      if(memPoolP != memPoolHeadP)
         OS_LYRG_unlockMutex(&memPoolP->mutex);
   }

   headP->nextP = tailP; //chain
   OS_LYRG_unlockMutex(&memPoolHeadP->mutex);
}

static void MEM_POOLP_show(MEM_POOLG_handleT handle)
{
   unsigned int i;
   UINT32 *bufP;
   UINT32 bufSize;
   int numFree = 0;
   MEM_POOLP_privDescT *descrP;
   MEM_POOLP_memPoolT *memPoolP = (MEM_POOLP_memPoolT *)handle;

   LOGG_PRINT(LOG_INFO_E, NULL, "Sizeof descr=%u %u)\n", (UINT32)sizeof(MEM_POOLP_privDescT), (UINT32)sizeof(MEM_POOLG_bufDescT));

   if(memPoolP->type != MEM_POOLG_TYPE_ALLOC_USER_E)
   {
      bufSize = memPoolP->bufferSize + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
   else
   {
      bufSize = MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }

   for(i = 0; i < memPoolP->numBuffers; i++)
   {
      bufP =  (UINT32 *)(((UINT8 *)memPoolP->bufMemP) + bufSize*i);
      descrP = (MEM_POOLP_privDescT *)(((UINT8 *)bufP) + MEM_POOLP_BUF_RSVD_BYTES);

      if((*bufP & (1<<MEM_POOLP_BUF_STATUS_OFFSET)) == 0)
         numFree++;
      LOGG_PRINT(LOG_INFO_E, NULL, "[%2d @ %p] %p 0x%08x F=%d(%2d) M=0x%x\n",i, memPoolP->bufMemP, bufP, *bufP, ((*bufP & (1<<MEM_POOLP_BUF_STATUS_OFFSET)) == 0) , numFree,(*bufP & MEM_POOLP_BUF_MAGIC_MASK) );
      LOGG_PRINT(LOG_INFO_E, NULL, "descrP=%p(%p) pool=%p start=%p, size=%d, dataP=%p, dataLen=%d nextP=%p\n",
         descrP, &(descrP->pubDesc), descrP->pool, descrP->pubDesc.startP, descrP->pubDesc.size, descrP->pubDesc.dataP, descrP->pubDesc.dataLen, descrP->pubDesc.nextP);
   }
   for(i = 0; i < memPoolP->freeTblIdx; i++)
   {
      LOGG_PRINT(LOG_INFO_E, NULL,"%p\n", memPoolP->freeTbl[i]);
   }
   LOGG_PRINT(LOG_INFO_E, NULL, "Mempool: start=%p bufSize=%d num=%d free=%d (idx=%d)\n", memPoolP->bufMemP, memPoolP->bufferSize, memPoolP->numBuffers, numFree, memPoolP->freeTblIdx);
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: MEM_POOLG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: memory
*
****************************************************************************/
void MEM_POOLG_init(void)
{
   int i;

   for(i = 0; i < MEM_POOLP_MAX_POOLS; i++)
   {
      memset(&MEM_POOLP_memPool[i], 0x0, sizeof(MEM_POOLP_memPool[i]));
   }
}


static ERRG_codeE MEM_POOLP_findFreeSlot(int* slotIndex)
{
   ERRG_codeE ret = MEM_POOL__RET_SUCCESS;
   int i=0;
   //Initialize memory pool
   //Find empty slot
   for(i = 0; i < MEM_POOLP_MAX_POOLS; i++)
   {
      if(MEM_POOLP_memPool[i].bufMemP == NULL)
      break;
   }

   if(i == MEM_POOLP_MAX_POOLS)
   {
      return MEM_POOL__ERR_OUT_OF_RSRCS;
   }
   *slotIndex = i;
   return ret;
}

/****************************************************************************
*
*  Function Name: MEM_POOLG_initPool
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: memory
*
****************************************************************************/
ERRG_codeE MEM_POOLG_initPool(MEM_POOLG_handleT *handleP, MEM_POOLG_cfgT *cfgP)
{
   int slotIndex=0,ret;
   ERRG_codeE retVal=MEM_POOL__RET_SUCCESS;
   *handleP = NULL;

   if(cfgP->type >= MEM_POOLG_NUM_TYPES_E)
   {
      retVal = MEM_POOL__ERR_INVALID_ARGS;
   }
   retVal = MEM_POOLP_findFreeSlot(&slotIndex);
   if (ERRG_SUCCEEDED(retVal))
   {
      ret = OS_LYRG_aquireMutex(&MEM_POOLP_memPool[slotIndex].mutex);
      assert(ret == SUCCESS_E);

      MEM_POOLP_memPool[slotIndex].type                = cfgP->type;
      MEM_POOLP_memPool[slotIndex].funcTblP            = &MEM_POOLP_funcTable[cfgP->type];
      MEM_POOLP_memPool[slotIndex].bufferSize          = cfgP->bufferSize;
      MEM_POOLP_memPool[slotIndex].numBuffers          = cfgP->numBuffers;
      MEM_POOLP_memPool[slotIndex].freeTblIdx          = 0;
      MEM_POOLP_memPool[slotIndex].freeCb              = cfgP->freeCb;
      MEM_POOLP_memPool[slotIndex].freeArg             = cfgP->freeArg;
      MEM_POOLP_memPool[slotIndex].resetBufPtrInAlloc  = cfgP->resetBufPtrInAlloc;
      MEM_POOLP_memPool[slotIndex].refPtr              = cfgP->refPtr;
      retVal = MEM_POOLP_memPoolInit((MEM_POOLG_handleT)&MEM_POOLP_memPool[slotIndex]);
      if (ERRG_SUCCEEDED(retVal))
      {
         *handleP = (MEM_POOLG_handleT)&MEM_POOLP_memPool[slotIndex];
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to allocate memory \n");
      }
   
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to find free slot \n");
   }
   return retVal;
}

/****************************************************************************
*
*  Function Name: MEM_POOLG_alloc
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: memory
*
****************************************************************************/
ERRG_codeE MEM_POOLG_alloc(MEM_POOLG_handleT handle, UINT32 size, MEM_POOLG_bufDescT **bufDescP)
{
   return ((MEM_POOLP_memPoolT *)handle)->funcTblP->alloc(handle, size, bufDescP);
}


/****************************************************************************
*
*  Function Name: MEM_POOLG_duplicate
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: memory
*
****************************************************************************/
void MEM_POOLG_duplicate(MEM_POOLG_bufDescT *headP)
{
   MEM_POOLP_memPoolT   *memPoolP;
   MEM_POOLG_bufDescT   *bufDescP = headP;

   while(bufDescP)
   {
      //Get buffer start and mem pool handle
      memPoolP = MEM_POOLP_getPoolH(bufDescP);
      OS_LYRG_lockMutex(&memPoolP->mutex);
      {
         bufDescP->refCntr++;
         bufDescP = bufDescP->nextP;
      }
      OS_LYRG_unlockMutex(&memPoolP->mutex);
   }

}


/****************************************************************************
*
*  Function Name: MEM_POOLG_free
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: memory
*
****************************************************************************/
void MEM_POOLG_free(MEM_POOLG_bufDescT *bufDescP)
{
   MEM_POOLG_bufDescT   *curP = bufDescP;
   MEM_POOLG_bufDescT   *nextP;

   //free entire chain
   while(curP)
   {
      nextP = curP->nextP;
      MEM_POOLP_freeBuf(curP);
      curP = nextP;
   }
}

void MEM_POOLG_chain(MEM_POOLG_bufDescT *headP, MEM_POOLG_bufDescT *tailP)
{
   MEM_POOLG_bufDescT *lastP = headP;

   //Get to last descriptor in head list and attach to tail
   while(lastP->nextP)
   {
      lastP = lastP->nextP;
   }
   MEM_POOLP_chaining(lastP, tailP);
}


UINT32 MEM_POOLG_getChainLen(MEM_POOLG_bufDescT *headP)
{
   UINT32  totalLen = 0;
   MEM_POOLG_bufDescT *currP = headP;

   //Get to last descriptor in head list and attach to tail
   while(currP)
   {
      totalLen += currP->dataLen;
      currP = currP->nextP;
   }
   return totalLen;
}

/****************************************************************************
*
*  Function Name: MEM_POOLG_freeAll
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: memory
*
****************************************************************************/
void MEM_POOLG_freeAll(MEM_POOLG_handleT handle)
{
   MEM_POOLP_freeAll(handle);
}


/****************************************************************************
*
*  Function Name: MEM_POOLG_closePool
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: memory
*
****************************************************************************/
void MEM_POOLG_closePool(MEM_POOLG_handleT handle)
{
#if ((DEFSG_PROCESSOR == DEFSG_GP) || (DEFSG_PROCESSOR == DEFSG_HOST))
   MEM_POOLP_closePool(handle);
#endif
}

/****************************************************************************
*
*  Function Name: MEM_POOLG_waitBuffReleaseClosePool
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: memory
*
****************************************************************************/
void MEM_POOLG_waitBuffReleaseClosePool(MEM_POOLG_handleT handle)
{
   UINT32 sleep = 0;

   //wait for system to release all clones before closing the pool
   while(MEM_POOLG_getNumFree(handle) < MEM_POOLG_getNumBufs(handle))
   {
      OS_LYRG_usleep(MEM_POOLP_SLEEP_USEC);
      sleep += MEM_POOLP_SLEEP_USEC;
      if(sleep >= MEM_POOLP_TIMEOUT_USEC)
      {
         LOGG_PRINT(LOG_WARN_E, NULL, "warning! buffers stuck in the system! (%d, %d)\n", MEM_POOLG_getNumFree(handle), MEM_POOLG_getNumBufs(handle));
         //printf("warning! buffers stuck in the system! (%d, %d)\n", MEM_POOLG_getNumFree(handle), MEM_POOLG_getNumBufs(handle));
         break;
      }
   }

   MEM_POOLG_closePool(handle);
}


UINT32 MEM_POOLG_getBufSize(MEM_POOLG_handleT handle)
{
   return ((MEM_POOLP_memPoolT *)handle)->bufferSize;
}

UINT32 MEM_POOLG_getNumBufs(MEM_POOLG_handleT handle)
{
   return ((MEM_POOLP_memPoolT *)handle)->numBuffers;
}

/****************************************************************************
*
*  Function Name: MEM_POOLG_show
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: memory
*
****************************************************************************/
void MEM_POOLG_show(MEM_POOLG_handleT handle)
{
   MEM_POOLP_show(handle);
}

unsigned int MEM_POOLG_getNumFree(MEM_POOLG_handleT handle)
{
   UINT32   i;
   UINT32   *bufP;
   UINT32   bufSize;
   UINT32   numFree = 0;

   MEM_POOLP_memPoolT *memPoolP = (MEM_POOLP_memPoolT *)handle;

#ifndef MEM_POOLP_CMEM_HEAP_SPLIT
   if(memPoolP->type != MEM_POOLG_TYPE_ALLOC_USER_E)
   {
      bufSize = memPoolP->bufferSize + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
   else
   {
      bufSize = MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
#else
   if((memPoolP->type == MEM_POOLG_TYPE_ALLOC_HEAP_E)||(memPoolP->type == MEM_POOLG_TYPE_ALLOC_SHARED_E))
   {
      bufSize = memPoolP->bufferSize + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
   else
   {
      bufSize = MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
#endif

   OS_LYRG_lockMutex(&memPoolP->mutex);

   for(i = 0; i < memPoolP->numBuffers; i++)
   {
      bufP =  (UINT32 *)(memPoolP->bufMemP + bufSize*i);
      if((*bufP & (1<<MEM_POOLP_BUF_STATUS_OFFSET)) == 0)
      {
         numFree++;
      }
   }

   OS_LYRG_unlockMutex(&memPoolP->mutex);
   return numFree;
}

ERRG_codeE MEM_POOLG_getPoolBuffers(MEM_POOLG_handleT handle, MEM_POOLG_bufDescT** buffers, UINT32 numOfBuffers)
{
   UINT32               bufSize;
   UINT32               i;
   MEM_POOLP_privDescT  *privDesc;
   MEM_POOLP_memPoolT   *memPoolP = (MEM_POOLP_memPoolT*)handle;
   ERRG_codeE           ret = MEM_POOL__RET_SUCCESS;

#ifndef MEM_POOLP_CMEM_HEAP_SPLIT
   if(memPoolP->type != MEM_POOLG_TYPE_ALLOC_USER_E)
   {
      bufSize = memPoolP->bufferSize + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
   else
   {
      bufSize = MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
#else
   if((memPoolP->type == MEM_POOLG_TYPE_ALLOC_HEAP_E)||(memPoolP->type == MEM_POOLG_TYPE_ALLOC_SHARED_E))
   {
      bufSize = memPoolP->bufferSize + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
   else
   {
      bufSize = MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
#endif

   if (!buffers || numOfBuffers < memPoolP->numBuffers)
   {
      return MEM_POOL__ERR_INVALID_ARGS;
   }

   for (i = 0; i < memPoolP->numBuffers; i++)
   {
      privDesc = (MEM_POOLP_privDescT*)(memPoolP->bufMemP + bufSize * i + MEM_POOLP_BUF_RSVD_BYTES);
      if (privDesc)
      {
         buffers[i] = &privDesc->pubDesc;
      }
   }
   return ret;
}

ERRG_codeE MEM_POOLG_removeBufFromPool(MEM_POOLG_bufDescT *bufDescP)
{
   ERRG_codeE           ret = MEM_POOL__RET_SUCCESS;
   UINT32               *bufP, *bufTempP;
   MEM_POOLP_privDescT  *descrP;
   MEM_POOLP_memPoolT   *memPoolP;
   UINT32               i;

   //Get buffer start and mem pool handle
   bufP = (UINT32 *)((UINT8 *)bufDescP - offsetof(MEM_POOLP_privDescT, pubDesc) - MEM_POOLP_BUF_RSVD_BYTES);
   descrP = (MEM_POOLP_privDescT *)((UINT8 *)bufP + MEM_POOLP_BUF_RSVD_BYTES);
   memPoolP = (MEM_POOLP_memPoolT *)descrP->pool;

   OS_LYRG_lockMutex(&memPoolP->mutex);
   /* Search buffer in free table, and remove it */
   if(memPoolP->freeTblIdx)
   {
      for (i = 0; i < memPoolP->freeTblIdx; i++)
      {
         bufTempP = (UINT32 *)(memPoolP->freeTbl[i]);
         if (bufTempP == bufP)
         {
            /* fix free array */
            for (; i < memPoolP->freeTblIdx; i++)
            {
               memPoolP->freeTbl[i] = memPoolP->freeTbl[i + 1];
            }
            memPoolP->freeTblIdx--;
            break;
         }
      }
   }

   //Sanity check magic number
   assert((*bufP & MEM_POOLP_BUF_MAGIC_MASK) == MEM_POOLP_BUF_MAGIC_VAL);

   //Sanify check free status
   assert(*bufP & (1<<MEM_POOLP_BUF_STATUS_OFFSET));

   //Mark it as used - this will prevent the pool using this buffer
   *bufP    |= (1<<MEM_POOLP_BUF_STATUS_OFFSET);
   OS_LYRG_unlockMutex(&memPoolP->mutex);

   return ret;
}

#if (DEFSG_PROCESSOR == DEFSG_GP)
ERRG_codeE MEM_POOLG_getDataPhyAddr(MEM_POOLG_bufDescT *bufDescP, UINT32 *physicalAddress)
{
   ERRG_codeE           ret = MEM_POOL__RET_SUCCESS;
   UINT32               *bufP;
   MEM_POOLP_privDescT  *descrP;
   MEM_POOLP_memPoolT   *memPoolP;

   ret = MEM_MAPG_convertVirtualToPhysical((UINT32)bufDescP->dataP, physicalAddress);
   if (ERRG_FAILED(ret))
   {
      //Get buffer start and mem pool handle
      bufP     = (UINT32 *)((UINT8 *)bufDescP - offsetof(MEM_POOLP_privDescT, pubDesc) - MEM_POOLP_BUF_RSVD_BYTES);
      descrP   = (MEM_POOLP_privDescT *)((UINT8 *)bufP + MEM_POOLP_BUF_RSVD_BYTES);
      memPoolP = (MEM_POOLP_memPoolT *)descrP->pool;

      if ((memPoolP->type != MEM_POOLG_TYPE_ALLOC_CMEM_E) && (memPoolP->type != MEM_POOLG_TYPE_ALLOC_KERSPC_E))
      {

         return MEM_POOL__ERR_INVALID_ARGS;
      }
#ifndef MEM_POOLP_CMEM_HEAP_SPLIT
      *physicalAddress = memPoolP->bufPysicalAddr + (bufDescP->dataP - memPoolP->bufMemP);
#else
      if (memPoolP->type == MEM_POOLG_TYPE_ALLOC_KERSPC_E)
      {
        *physicalAddress = MEM_MAPG_getMemRegionTable() + (bufDescP->dataP - (UINT8*)memPoolP->kerMemP);
      }
      else
      *physicalAddress = memPoolP->bufPysicalAddr + (bufDescP->dataP - (UINT8*)memPoolP->cmemMemP);
#endif

   ret = MEM_POOL__RET_SUCCESS;
   }

   return ret;
}
ERRG_codeE MEM_POOLG_getDataPhyAddrUsingStartP(MEM_POOLG_bufDescT *bufDescP, UINT32 *physicalAddress)
{
   ERRG_codeE           ret = MEM_POOL__RET_SUCCESS;
   UINT32               *bufP;
   MEM_POOLP_privDescT  *descrP;
   MEM_POOLP_memPoolT   *memPoolP;

   ret = MEM_MAPG_convertVirtualToPhysical((UINT32)bufDescP->startP, physicalAddress);
   if (ERRG_FAILED(ret))
   {
      //Get buffer start and mem pool handle
      bufP     = (UINT32 *)((UINT8 *)bufDescP - offsetof(MEM_POOLP_privDescT, pubDesc) - MEM_POOLP_BUF_RSVD_BYTES);
      descrP   = (MEM_POOLP_privDescT *)((UINT8 *)bufP + MEM_POOLP_BUF_RSVD_BYTES);
      memPoolP = (MEM_POOLP_memPoolT *)descrP->pool;

      if ((memPoolP->type != MEM_POOLG_TYPE_ALLOC_CMEM_E) && (memPoolP->type != MEM_POOLG_TYPE_ALLOC_KERSPC_E))
      {

         return MEM_POOL__ERR_INVALID_ARGS;
      }
#ifndef MEM_POOLP_CMEM_HEAP_SPLIT
      *physicalAddress = memPoolP->bufPysicalAddr + (bufDescP->startP - memPoolP->bufMemP);
#else
      if (memPoolP->type == MEM_POOLG_TYPE_ALLOC_KERSPC_E)
      {
        *physicalAddress = MEM_MAPG_getMemRegionTable() + (bufDescP->startP - (UINT8*)memPoolP->kerMemP);
      }
      else
      *physicalAddress = memPoolP->bufPysicalAddr + (bufDescP->startP - (UINT8*)memPoolP->cmemMemP);
#endif

   ret = MEM_POOL__RET_SUCCESS;
   }

   return ret;
}
#endif

int MEM_POOLG_check(MEM_POOLG_handleT handle)
{
   UINT32   i;
   UINT32   errCnt = 0;
   UINT32   *bufP;
   UINT32   bufSize;
   MEM_POOLP_privDescT *descrP;
   MEM_POOLP_memPoolT *memPoolP = (MEM_POOLP_memPoolT *)handle;

#ifndef MEM_POOLP_CMEM_HEAP_SPLIT
   if(memPoolP->type != MEM_POOLG_TYPE_ALLOC_USER_E)
   {
      bufSize = memPoolP->bufferSize + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
   else
   {
      bufSize = MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
#else
   if((memPoolP->type == MEM_POOLG_TYPE_ALLOC_HEAP_E)||(memPoolP->type == MEM_POOLG_TYPE_ALLOC_SHARED_E))
   {
      bufSize = memPoolP->bufferSize + MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
   else
   {
      bufSize = MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT);
   }
#endif

   OS_LYRG_lockMutex(&memPoolP->mutex);

   for(i = 0; i < memPoolP->numBuffers; i++)
   {
      bufP =  (UINT32 *)(((UINT8 *)memPoolP->bufMemP) + bufSize*i);
      descrP = (MEM_POOLP_privDescT *)(((UINT8 *)bufP) + MEM_POOLP_BUF_RSVD_BYTES);

      if( (( *bufP & MEM_POOLP_BUF_MAGIC_MASK) != MEM_POOLP_BUF_MAGIC_VAL) || (descrP->pool == NULL))
      {
         errCnt++;
      }
   }

   OS_LYRG_unlockMutex(&memPoolP->mutex);

   if(errCnt > 0)
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "mempool %p: %d errrors\n",handle, errCnt);
   }

   return errCnt;
}

unsigned int MEM_POOLG_getNumReservedBytes(void)
{
   return (MEM_POOLP_BUF_RSVD_BYTES + sizeof(MEM_POOLP_privDescT));
}

#ifdef MEM_POOLG_BUFF_TRACE
void MEM_POOLG_buffTraceUpdateState(const char* consumer, MEM_POOLG_bufDescT* buff, const char* caller)
{
   UINT32 i, j;
   MEM_POOLP_memPoolT *memPoolP = MEM_POOLP_getPoolH(buff);

   for (i = 0; i < memPoolP->numBuffers; i++)
   {
      if (memPoolP->buffList[i] == buff)
      {
         // no consumer indicates to free buffer
         if (!consumer)
         {
            memset(memPoolP->buffConsumers[i], 0, sizeof(memPoolP->buffConsumers[i]));
            //printf("@ from %s: released %p (#%d)\n", caller, buff, i);
            return;
         }

         for (j = 0; j < MEM_POOLG_BUFF_TRACE_MAX_CONSUMERS; j++)
         {
               if (!memPoolP->buffConsumers[i][j].name)
               {
                  memPoolP->buffConsumers[i][j].name = consumer;
                  memPoolP->buffConsumers[i][j].status = MEM_POOLP_BUFF_STATUS_READY_E;
                  //printf("@ from %s: %s allocated %p (#%d) - total %d\n", caller, memPoolP->buffConsumers[i][j].name, buff, i, memPoolP->buffConsumers[i][j].status);
               }
               else if (!strcmp(memPoolP->buffConsumers[i][j].name, consumer))
               {
                  memPoolP->buffConsumers[i][j].status = (MEM_POOLP_buffStatusE)(memPoolP->buffConsumers[i][j].status + 1); // increment
                  //printf("@ from %s: %s updated %p (#%d) - total %d\n", caller, memPoolP->buffConsumers[i][j].name, buff, i, memPoolP->buffConsumers[i][j].status);
               }
               else
               {
                  continue;
               }
               return;
         }
         printf("@ ERROR: from %s, buffer %p has maximal buffConsumers\n", caller, memPoolP->buffList[i]);
      }
   }
}

void MEM_POOLG_buffTracePrint(MEM_POOLG_handleT memPoolHandle, const char* msg)
{
   UINT32 i, j;
   MEM_POOLP_memPoolT *memPoolP = (MEM_POOLP_memPoolT*)memPoolHandle;

   printf("\n@ %s\n@ buffer consumers and status (1-ready, 2-operation/pipe, 3-done):\n", msg);

   printf("@ %d (%p): drain buffer\n", 0, memPoolP->buffList[0]);

   for (i = 1; i < memPoolP->numBuffers; i++)
   {
      if (memPoolP->buffList[i])
      {
         printf("@ %d (%p): ", i, memPoolP->buffList[i]);
         for (j = 0; j < MEM_POOLG_BUFF_TRACE_MAX_CONSUMERS; j++)
         {
            if (memPoolP->buffConsumers[i][j].name)
            {
               printf("%s (%d) ", memPoolP->buffConsumers[i][j].name, memPoolP->buffConsumers[i][j].status);
               continue;
            }
            printf("\n");
            break;
         }
         continue;
      }
      break;
   }
   printf("\n");
}
#endif

