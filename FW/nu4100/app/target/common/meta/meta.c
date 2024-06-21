#if 0
#include "inu_common.h"

#include <assert.h>
#include <string.h>

#include "mem_map.h"
#include "meta.h"
#if DEFSG_IS_GP
#include "cmem.h"
#endif

/*
FIXME: only 4 bytes are needed for the meta data pointer. Using 128 due to possible alignment issue with DMA/DDR.
*/
#define METAP_OFFSET_BYTES (128)
//#define METAP_OFFSET_BYTES (0)
//#define METAP_DISABLE 
//#define METAP_SELFTEST

#define P2V_META(p) METAP_phys2Virt(p, MEM_MAPG_CRAM_META_E)
#define V2P_META(v) METAP_virt2Phys(v, MEM_MAPG_CRAM_META_E)
#define P2V_BUFFER(p) METAP_phys2Virt(p, MEM_MAPG_DDR_CDE_BUFFERS_E)
#define V2P_BUFFER(v) METAP_virt2Phys(v, MEM_MAPG_DDR_CDE_BUFFERS_E)

#define METAP_NUM_CHANNELS  (INU_DEFSG_NUM_OF_HW_CHANNELS_E)
/*
* These values have to compile equally on all platforms.
*/
#define METAP_INTERNAL_CH_META_SIZE (sizeof(METAP_chMetaT))
#define METAP_INTERNAL_BUFFER_META_SIZE (sizeof(METAP_metaT))

/*
* Internal meta data for channel - stored in meta area
* Size must be multiple of 4 bytes.
*/
typedef struct ATTR_PACKED
{
   UINT16 ch;
   UINT16 bufMetaSize; //meta size per buffer including internal
   UINT16 numBuffers; //number of buffers for channel
   UINT16 rsvd; //for size
} METAP_chMetaT;

/*
* Internal meta data structure. 1 per buffer. Stored in Meta area.
* Size must be multiple of 4 bytes.
*/
typedef struct ATTR_PACKED
{
   UINT16 ch; //system channel
   UINT16 size; //meta buffer size including this struct
   UINT8 *chMetaPtr; //physical address back to channel meta
   UINT8 *bufferPtr; //physical address back to buffer
} METAP_metaT;

/*
* Global info for meta data. Not stored in meta area.
*/
typedef struct
{
   UINT8 *basePtr;
   UINT8 *basePhysPtr;
   int virtMem;
   UINT32 totalMetaSize;
   UINT32 chMetaSize; //channel meta size
   METAP_chMetaT *chMetaPtrTbl[METAP_NUM_CHANNELS];
}METAP_infoT;

static METAP_infoT METAP_info;


static inline UINT8* METAP_phys2Virt(UINT8 *physP, MEM_MAPG_memMappingE region)
{
   UINT32 virt = 0;

   if(METAP_info.virtMem)
   {
      MEM_MAPG_convertPhysicalToVirtual2((UINT32)physP, &virt, region);
      assert(virt);
      return (UINT8 *)virt; //will be null if failed
   }
   else 
      return physP;
}

static inline UINT8* METAP_virt2Phys(UINT8 *virtP,MEM_MAPG_memMappingE region)
{
   UINT32 phys = 0;

   if(METAP_info.virtMem)
   {
      MEM_MAPG_convertVirtualToPhysical2((UINT32)virtP, &phys, region);
      assert(phys);
      return (UINT8 *)phys; //will be null if failed
   }
   else 
      return virtP;
}


static inline UINT8 *METAP_getMetaPtr(UINT8 *bufferPtr)
{
   //No need to invalidate here - the value does not change after initial write by GP.
   return (UINT8 *)(*((UINT32 *)(bufferPtr - METAP_OFFSET_BYTES)));
}
static inline void METAP_setMetaPtr(UINT8 *bufferPtr, UINT8 *metaPtr)
{
   *((UINT32 *)(bufferPtr - METAP_OFFSET_BYTES)) = (UINT32)metaPtr; 
#if DEFSG_IS_GP
   //Need to flush write for CEVA coherency.
   CMEM_cacheWb((void *)(bufferPtr - METAP_OFFSET_BYTES), sizeof(UINT32));
#endif   
}


#if DEFSG_IS_GP

static void METAP_setBufInternalMeta(METAP_metaT * bufMetaP, UINT16 ch, UINT16 bufMetaSize, UINT8 *bufferP,  UINT8 *chMetaP)
{
   bufMetaP->ch = (UINT16)ch;
   bufMetaP->size = bufMetaSize;
   bufMetaP->bufferPtr = bufferP;
   bufMetaP->chMetaPtr = chMetaP;
}

static void METAP_setChInternalMeta(METAP_chMetaT * chMetaP, UINT16 numBuffers, UINT16 ch, UINT16 bufMetaSize)
{
   chMetaP->numBuffers = numBuffers;
   chMetaP->ch = ch;
   chMetaP->bufMetaSize = bufMetaSize;
}

static void METAP_initChBufMeta(METAP_chMetaT *chMetaP, UINT16 ch, UINT16 bufMetaSize, UINT8 *bufPtrTbl[], UINT16 numBuffers)
{
   unsigned int i;
   UINT8 *bufMetaP;

   LOGG_PRINT(LOG_DEBUG_E, NULL, "Meta initializing channel buffers :%p  ch=%u size=%u numBuffers=%u\n", chMetaP, ch, bufMetaSize, numBuffers);
    
   bufMetaP = (UINT8 *)chMetaP + METAP_INTERNAL_CH_META_SIZE;
   for(i = 0; i < numBuffers; i++)
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "[%d] bufP %p bufMeta %p\n", i, bufPtrTbl[i], bufMetaP);
      memset(bufMetaP,0,bufMetaSize);
      METAP_setBufInternalMeta((METAP_metaT *)bufMetaP, ch, bufMetaSize, bufPtrTbl[i], V2P_META((UINT8 *)chMetaP));
      METAP_setMetaPtr(P2V_BUFFER(bufPtrTbl[i]), V2P_META(bufMetaP)); //Set link to meta data in buffer
      bufMetaP += bufMetaSize;
   }
}
#endif

/* 
*Initailize meta data memory.
*/
ERRG_codeE METAG_init(UINT8 *baseVirtPtr, UINT32 size)
{
   INU_DEFSG_sysChannelIdE ch;
   UINT8 *basePtr;

#ifdef METAP_DISABLE
   return META__RET_SUCCESS;
#endif   

   if((UINT32)baseVirtPtr & 0x3)
      return META__ERR_INVALID_ARGS;

   LOGG_PRINT(LOG_INFO_E, NULL, "Meta init base:%p(v) size=%u. meta offset %u bytes\n", baseVirtPtr, size, METAP_OFFSET_BYTES);
   if((METAP_INTERNAL_CH_META_SIZE % 4) || (METAP_INTERNAL_BUFFER_META_SIZE % 4))
      return META__ERR_UNEXPECTED;//should move this check to compile time.

#if DEFSG_IS_CEVA
   METAP_info.virtMem = 0;//no virtual memory on ceva
#else
   METAP_info.virtMem = 1;
#endif

   METAP_info.basePtr = baseVirtPtr;
   METAP_info.basePhysPtr = V2P_META(baseVirtPtr);
   if((UINT32)METAP_info.basePhysPtr & 0x3)
      return META__ERR_INVALID_ARGS;
      
   METAP_info.totalMetaSize = size;

   METAP_info.chMetaSize = METAP_info.totalMetaSize/METAP_NUM_CHANNELS;
   METAP_info.chMetaSize &= ~0x3; //Align channel meta structure to 4 bytes 
   if(METAP_info.chMetaSize <= METAP_INTERNAL_CH_META_SIZE)
      return META__ERR_INVALID_ARGS;

   LOGG_PRINT(LOG_DEBUG_E, NULL, "Meta internal struct sizes: channel_meta=%u buffer_meta=%u\n", METAP_INTERNAL_CH_META_SIZE, METAP_INTERNAL_BUFFER_META_SIZE);
   LOGG_PRINT(LOG_DEBUG_E, NULL, "Meta channel size=%u\n", METAP_info.chMetaSize);

   basePtr = (METAP_info.virtMem) ?  METAP_info.basePtr : METAP_info.basePhysPtr;
   for(ch = INU_DEFSG_SYS_CH_0_VIDEO_ID_E; ch < METAP_NUM_CHANNELS; ch++)
   {
      METAP_info.chMetaPtrTbl[ch] = (METAP_chMetaT *)(basePtr + (ch * METAP_info.chMetaSize));
      LOGG_PRINT(LOG_DEBUG_E, NULL, " [%u]%p\n", ch, METAP_info.chMetaPtrTbl[ch]);
   }
   
   return META__RET_SUCCESS;
}

#ifdef METAP_SELFTEST

static int METAP_internalChTest(INU_DEFSG_sysChannelIdE ch, unsigned int bufMetaSize, UINT8 *bufPhysPtrTbl[], unsigned int numBuffers)
{
   unsigned int i,size;
   UINT8 *virt, *ptr;
   INU_DEFSG_sysChannelIdE retCh;
   UINT8 *bufMetaP;
   int fail = 0;

   LOGG_PRINT(LOG_INFO_E, NULL, "Running internal test on channel %d\n",ch);

   METAG_showChannelMeta(ch, LOG_INFO_E);

   bufMetaP = (UINT8 *)METAP_info.chMetaPtrTbl[ch] + METAP_INTERNAL_CH_META_SIZE;
   for(i = 0; i < numBuffers; i++)
   {
       virt = P2V_BUFFER(bufPhysPtrTbl[i]);
       
       ptr = METAG_getPtr(virt);
       LOGG_PRINT(LOG_INFO_E, NULL,"METAG_getPtr(%p(v)) returned %p\n",virt,ptr);
       if(bufMetaP + METAP_INTERNAL_BUFFER_META_SIZE != ptr)
       {
          LOGG_PRINT(LOG_ERROR_E, NULL,"error: expected %p\n",bufMetaP);
          fail = 1;
       }
       ptr = METAG_getPtr2(ch, i);
       LOGG_PRINT(LOG_INFO_E, NULL,"METAG_getPtr2(ch=%d,index=%u) returned %p\n",ch,i,ptr);
       if(bufMetaP+ METAP_INTERNAL_BUFFER_META_SIZE != ptr)
       {
          LOGG_PRINT(LOG_ERROR_E, NULL,"error: expected %p\n",bufMetaP);
          fail = 1;
       }
       size = METAG_getSize(virt);
       LOGG_PRINT(LOG_INFO_E, NULL,"METAG_getSize(%p(v)) returned %u\n",virt,size);
       if((bufMetaSize- METAP_INTERNAL_BUFFER_META_SIZE) != size)
       {
          LOGG_PRINT(LOG_ERROR_E, NULL,"error: expected %d\n",bufMetaSize);
          fail = 1;
       }
       retCh = METAG_getChannel(virt);
       LOGG_PRINT(LOG_INFO_E, NULL,"METAG_getChannel(%p(v)) returned %d\n",virt,retCh);
       if(ch !=retCh)
       {
          LOGG_PRINT(LOG_ERROR_E, NULL,"error: expected %d\n",ch);
          fail = 1;
       }

       bufMetaP += bufMetaSize;
   }

   if(fail)
      LOGG_PRINT(LOG_ERROR_E, NULL," internal test failed\n");
   else
      LOGG_PRINT(LOG_INFO_E, NULL,"internal test passed\n");

   return fail;
}
#endif

/* 
* Iniitialize meta data per channel 
* Initialize the internal meta data for the channel and per buffer with the input buffer pointers.
*/
ERRG_codeE METAG_initChanMeta(INU_DEFSG_sysChannelIdE ch, UINT8 *bufPhysPtrTbl[], unsigned int numBuffers)
{
#if DEFSG_IS_GP

#ifdef METAP_DISABLE
   return META__RET_SUCCESS;
#endif   

   METAP_chMetaT *chMetaP;
   UINT16 bufMetaSize;

   if(ch >= METAP_NUM_CHANNELS)
      return META__ERR_NOT_SUPPORTED;
   
   if((!METAP_info.chMetaPtrTbl[ch]) || (!numBuffers))
      return META__ERR_INVALID_ARGS;

   chMetaP = METAP_info.chMetaPtrTbl[ch];

   //equal allotment per buffer
   bufMetaSize = (METAP_info.chMetaSize -METAP_INTERNAL_CH_META_SIZE)/numBuffers;
   bufMetaSize &= ~0x3; //align buffer meta structure to 4 bytes
   if(bufMetaSize <= METAP_INTERNAL_BUFFER_META_SIZE)
     return META__ERR_INVALID_ARGS;

   LOGG_PRINT(LOG_DEBUG_E, NULL, "Meta initializing channel %d. total %u bytes. %u per buffer for %u buffers\n",
      ch, METAP_info.chMetaSize, bufMetaSize, numBuffers);

   METAP_setChInternalMeta(chMetaP, (UINT16)numBuffers, (UINT16)ch, bufMetaSize);
   METAP_initChBufMeta(chMetaP, ch, bufMetaSize, bufPhysPtrTbl, numBuffers);

#ifdef METAP_SELFTEST
   METAP_internalChTest(ch, bufMetaSize, bufPhysPtrTbl, numBuffers);
#endif

   return META__RET_SUCCESS;
#else
   (void)ch;
   (void)bufPhysPtrTbl;
   (void)numBuffers;
   return META__RET_SUCCESS;
#endif

   METAG_showChannelMeta(ch, LOG_DEBUG_E);
}

unsigned int METAG_getOffsetBytes(void)
{
   return METAP_OFFSET_BYTES;
}

/*
* Get pointer to access the user meta data based on buffer pointer.
*/
UINT8 *METAG_getPtr(UINT8 *bufferPtr)
{
   UINT8 *metaP = METAP_getMetaPtr(bufferPtr);
   return P2V_META(metaP) + METAP_INTERNAL_BUFFER_META_SIZE;
}

UINT8 *METAG_getPtr2(INU_DEFSG_sysChannelIdE ch, unsigned int index)
{
   METAP_chMetaT *chMetaP = METAP_info.chMetaPtrTbl[ch];
   return ((UINT8 *)chMetaP + METAP_INTERNAL_CH_META_SIZE) + index*(chMetaP->bufMetaSize) + METAP_INTERNAL_BUFFER_META_SIZE;
}

//Get the meta data buffer size. Return size of user's meta data (i.e., not including internal stuff)
unsigned int METAG_getSize(UINT8 *bufferPtr)
{
   UINT8 *metaP = METAP_getMetaPtr(bufferPtr);
   return ((METAP_metaT *)P2V_META(metaP))->size - METAP_INTERNAL_BUFFER_META_SIZE;
}

INU_DEFSG_sysChannelIdE METAG_getChannel(UINT8 *bufferPtr)
{
   UINT8 *metaP = METAP_getMetaPtr(bufferPtr);
   return (INU_DEFSG_sysChannelIdE)((METAP_metaT *)P2V_META(metaP))->ch;
}

void METAG_showChannelMeta(INU_DEFSG_sysChannelIdE ch, INU_DEFSG_logLevelE loglevel)
{
   unsigned int i;
   METAP_metaT *bufMetaP;
   METAP_chMetaT *chMetaP;

   chMetaP = METAP_info.chMetaPtrTbl[ch];

   LOGG_PRINT(loglevel, NULL, "Meta Data channel entry for channel=%d @%p(v) %p: ch=%d buf_size=%u, num_bufs=%u\n", 
      ch, chMetaP, V2P_META((UINT8 *)chMetaP), chMetaP->ch, chMetaP->bufMetaSize, chMetaP->numBuffers);
   
   bufMetaP = (METAP_metaT *)((UINT8 *)chMetaP + METAP_INTERNAL_CH_META_SIZE);
   for(i = 0; i < chMetaP->numBuffers; i++)
   {
      LOGG_PRINT(loglevel, NULL, "  [%u]@%p(v) %p: ch=%d size=%u ch_ptr %p buf_ptr=%p meta_ptr=%p\n", 
         i, bufMetaP, V2P_META((UINT8 *)bufMetaP), bufMetaP->ch,bufMetaP->size, bufMetaP->chMetaPtr, 
         bufMetaP->bufferPtr, METAP_getMetaPtr(P2V_BUFFER(bufMetaP->bufferPtr)));

      if(bufMetaP->size != chMetaP->bufMetaSize)
      {
         LOGG_PRINT(loglevel, NULL,  "invalid size in internal meta %u != %u\n",bufMetaP->size, chMetaP->bufMetaSize);
         break;
      }
      bufMetaP = (METAP_metaT *)(((UINT8 *)bufMetaP) + bufMetaP->size);
   }
}

#endif
