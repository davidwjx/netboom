/****************************************************************************
 *
 *   FileName: mem_pool.h
 *
 *   Author:
 *
 *   Date:
 *
 *   Description: Pool and buffer Managmenet API
 *
 ****************************************************************************/
#ifndef MEM_POOL_H
#define MEM_POOL_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define MEM_POOLG_NUM_BANKS  (1)

#if DEFSG_IS_GP
//#define MEM_POOLG_BUFF_TRACE
#define MEM_POOLG_BUFF_TRACE_MAX_BUFFS      (10)
#define MEM_POOLG_BUFF_TRACE_MAX_CONSUMERS  (6)
#endif

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

typedef enum
{
   MEM_POOLG_TYPE_ALLOC_HEAP_E = 0,
   MEM_POOLG_TYPE_ALLOC_USER_E,
   MEM_POOLG_TYPE_ALLOC_CMEM_E,
   MEM_POOLG_TYPE_ALLOC_KERSPC_E,
   MEM_POOLG_TYPE_ALLOC_SHARED_E,
   MEM_POOLG_NUM_TYPES_E
} MEM_POOLG_typeE;


typedef void *MEM_POOLG_handleT;

typedef struct MEM_POOLG_bufDesc
{
   UINT8 *dataP;          //Current pointer to data in buffer
   UINT32 dataLen;        //Current length of data in buffer
   UINT8 *startP;         //Pointer to start of buffer
   UINT32 size;           //Full size of buffer (from original allocate call)
   UINT32 refCntr;        //ReferenceCounter to number of users
   struct MEM_POOLG_bufDesc *nextP; //Pointer to next in chain
   INT32 fileDescriptor;         /*DMA Buf file descriptor*/
} MEM_POOLG_bufDescT;

typedef ERRG_codeE (*MEM_POOLG_freeCbT) (void *arg, MEM_POOLG_bufDescT *bufDescP);
typedef void *MEM_POOLP_sharedMallocFuncT(unsigned int size, void *refPtr);
typedef void MEM_POOLP_sharedFreeFuncT(void *ptr,void *refPtr);

typedef struct
{
   UINT32            bufferSize;
   UINT32            numBuffers;
   MEM_POOLG_freeCbT freeCb;
   void             *freeArg;
   UINT32            resetBufPtrInAlloc;
   MEM_POOLG_typeE   type;
   UINT8             *memP;
   void              *refPtr;
} MEM_POOLG_cfgT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
void MEM_POOLG_init(void);
ERRG_codeE MEM_POOLG_initPool(MEM_POOLG_handleT *handleP, MEM_POOLG_cfgT *cfgP);
ERRG_codeE MEM_POOLG_alloc(MEM_POOLG_handleT handle, UINT32 size, MEM_POOLG_bufDescT **bufDescP);
void MEM_POOLG_duplicate(MEM_POOLG_bufDescT *headP);
void MEM_POOLG_free(MEM_POOLG_bufDescT *bufDescP);
void MEM_POOLG_freeAll(MEM_POOLG_handleT handle);
void MEM_POOLG_chain(MEM_POOLG_bufDescT *headP, MEM_POOLG_bufDescT *tailP);
UINT32 MEM_POOLG_getChainLen(MEM_POOLG_bufDescT *headP);
void MEM_POOLG_show(MEM_POOLG_handleT handle);
void MEM_POOLG_closePool(MEM_POOLG_handleT handle);
void MEM_POOLG_waitBuffReleaseClosePool(MEM_POOLG_handleT handle);
UINT32 MEM_POOLG_getBufSize(MEM_POOLG_handleT handle);
UINT32 MEM_POOLG_getNumBufs(MEM_POOLG_handleT handle);
unsigned int MEM_POOLG_getNumFree(MEM_POOLG_handleT handle);
ERRG_codeE MEM_POOLG_getPoolBuffers(MEM_POOLG_handleT handle, MEM_POOLG_bufDescT** buffers, UINT32 numOfBuffers);
ERRG_codeE MEM_POOLG_removeBufFromPool(MEM_POOLG_bufDescT *bufDescP);
#if (DEFSG_PROCESSOR == DEFSG_GP)
ERRG_codeE MEM_POOLG_getDataPhyAddr(MEM_POOLG_bufDescT *bufDescP, UINT32 *physicalAddress);
ERRG_codeE MEM_POOLG_getDataPhyAddrUsingStartP(MEM_POOLG_bufDescT *bufDescP, UINT32 *physicalAddress);
#endif
int MEM_POOLG_check(MEM_POOLG_handleT handle);
void MEM_POOLG_assignShardCBFunc(MEM_POOLP_sharedMallocFuncT *MEM_POOLP_sharedMallocFuncPtr, MEM_POOLP_sharedFreeFuncT *MEM_POOLP_sharedFreeFunc);
unsigned int MEM_POOLG_getNumReservedBytes();
#ifdef MEM_POOLG_BUFF_TRACE
void MEM_POOLG_buffTraceUpdateState(const char* consumer, MEM_POOLG_bufDescT* buff, const char* caller);
void MEM_POOLG_buffTracePrint(MEM_POOLG_handleT memPoolHandle, const char* msg);
#endif
static inline MEM_POOLG_bufDescT *MEM_POOLG_getLast(MEM_POOLG_bufDescT *d)
{
   while(d->nextP)
      d = d->nextP;
   return d;
}

#ifdef __cplusplus
}
#endif

#endif //MEM_POOL_H
