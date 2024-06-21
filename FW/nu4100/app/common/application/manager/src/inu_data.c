#include "inu2_types.h"
#include "inu2_internal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"

#include "inu_metadata.h"
#include "inu_comm.h"
#include "mem_pool.h"
#include "inu_metadata_serializer.h"
#if DEFSG_IS_NOT_DSP
#include "internal_cmd.h"
#endif

#include "assert.h"
#if DEFSG_IS_GP
#include "mem_map.h"
#endif

#define INU_DATA__COMPRESSION_FLAG_NUM_BITS (1)
#define INU_DATA__MODE_ID_NUM_BITS          (2)

//compression
#define INU_DATA__COMPRESSION_FLAG_OFFSET            (0)
#define INU_DATA__COMPRESSION_FLAG_MASK              ((1U << INU_DATA__COMPRESSION_FLAG_NUM_BITS) - 1)
#define INU_DATA__COMPRESSION_FLAG_GET(x)            ((x >> INU_DATA__COMPRESSION_FLAG_OFFSET) & INU_DATA__COMPRESSION_FLAG_MASK)
#define INU_DATA__COMPRESSION_FLAG_SET(x,id)         (x = ((x & ~(INU_DATA__COMPRESSION_FLAG_MASK << INU_DATA__COMPRESSION_FLAG_OFFSET)) | ((id & INU_DATA__COMPRESSION_FLAG_MASK) << INU_DATA__COMPRESSION_FLAG_OFFSET)))
//mode
#define INU_DATA__MODE_ID_OFFSET        (INU_DATA__MODE_ID_NUM_BITS)
#define INU_DATA__MODE_ID_MASK          ((1U << INU_DATA__MODE_ID_NUM_BITS) - 1)
#define INU_DATA__MODE_ID_GET(x)        ((x >> INU_DATA__MODE_ID_OFFSET) & INU_DATA__MODE_ID_MASK)
#define INU_DATA__MODE_ID_SET(x,id)     (x = ((x & ~(INU_DATA__MODE_ID_MASK << INU_DATA__MODE_ID_OFFSET)) | ((id & INU_DATA__MODE_ID_MASK) << INU_DATA__MODE_ID_OFFSET)))

//#define MAX_THROUGHPUT_INJECTION

typedef struct inu_data__privData
{
   inu_comm                  *dataCommH;
   MEM_POOLG_bufDescT        *dataBufP;
   MEM_POOLG_bufDescT        *internalDataBufP;
   MEM_POOLG_handleT          dataPool;
#ifdef METADATA
   UINT32                     metadata_size;
#endif
   UINT32                     size;
   UINT16                     chunksTotalNum;
   UINT32                     chunkSize;
   inu_ref__id_t              sourceNode;
   inu_data                  *compositeP;
}inu_data__privData;

static inu_data__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_DATA";

#if DEFSG_IS_NOT_DSP
static const char* inu_data__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_data__dtor(inu_ref *me)
{
   inu_node__vtable_get()->ref_vtable.p_dtor(me);
   inu_data__freeInternalDuplicatePool((inu_data*)me);
   free(((inu_data*)me)->privP);
}

/* Constructor */
ERRG_codeE inu_data__ctor(inu_data *me, inu_data__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_data__privData *privP;
   ret = inu_node__ctor(&me->node, &ctorParamsP->nodeCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_data__privData*)malloc(sizeof(inu_data__privData));
      if (privP)
      {
         memset(privP, 0, sizeof(inu_data__privData));
         me->privP = privP;
         privP->sourceNode = ctorParamsP->sourceNode.id;
         me->hdrOffsetInStruct = (UINT32)((UINT8*)(((inu_data__VTable*)(me->node.ref.p_vtable))->p_dataHdrGet(me)) - ((UINT8*)me));
         inu_data__sizeSet(me, 1024*16, 1024*16);
      }
      else
      {
         ret = INU_DATA__ERR_OUT_OF_MEM;
      }
   }

   return ret;
}

static void inu_data__newInput(inu_node *me, inu_node *input)
{
#if DEFSG_IS_GP
   inu_data *dataP = (inu_data*)me;
#endif
   inu_node__vtable_get()->p_newInput(me,input);
#if DEFSG_IS_GP
   inu_data__privData *privP = (inu_data__privData*)dataP->privP;
#include "gme_mngr.h"
   if (privP->sourceNode == input->ref.id)
   {
      //hack - need to figure out a better way to specify the number of meta datas in a stream (should be as buffers...)
      if (GME_MNGRG_getModelType() == INU_DEFSG_BOOT51_E)
      {
         inu_data__allocInternalDuplicatePool(dataP, 2000);
      }
      else
      {
#ifdef MAX_THROUGHPUT_INJECTION
         inu_data__allocInternalDuplicatePool(dataP, 150 * inu_data__chunkTotalNumGet(dataP) );
#else
         inu_data__allocInternalDuplicatePool(dataP, 150 * inu_data__chunkTotalNumGet(dataP) );   /*Increased by IMDT to allow for increased IMU buffering*/
#endif
      }
   }
#endif
}


static void inu_data__freeData(inu_data *me)
{
   MEM_POOLG_bufDescT *bufDescP = NULL;
   inu_data__bufDescPtrGet(me, &bufDescP);
   if (bufDescP)
   {
#ifdef DATA_DEBUG
 #if DEFSG_IS_GP
      UINT64 usec;
      OS_LYRG_getUsecTime(&usec);
      if(me->node.ref.refType == inu_debug_data_map)
      {
         printf("%llu: free %p (from %s (%p))\n",usec,bufDescP,me->node.ref.p_vtable->p_name((inu_ref*)me),me);
      }
#endif
#endif
#ifdef MEM_POOLG_BUFF_TRACE
        MEM_POOLG_buffTraceUpdateState(NULL, bufDescP, __func__);
#endif
      MEM_POOLG_free(bufDescP);
#if DEFSG_IS_HOST
      free(me->privP);
#endif
   }
}

static UINT32 inu_data__msgCodeGet(inu_data *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return INTERNAL_CMDG_DATA_SEND_E;
}

static void *inu_data__dataHdrGet(inu_data *me)
{
   return &me->dataHdr;
}

static void inu_data__dataHdrSet(inu_data *me, void *hdrP)
{
   memcpy(&me->dataHdr, hdrP, sizeof(inu_data__hdr_t));
}


static inu_data *inu_data__dataDuplicate(inu_data *me, UINT8 *bufP)
{
   inu_data *clone = (inu_data*)bufP;

   if (!clone)
   {
      return NULL;
   }

   //copy all
   memcpy(clone,me,sizeof(inu_data));
   clone->privP = (inu_data__privData*)malloc(sizeof(inu_data__privData));
   if (!clone->privP)
   {
      return NULL;
   }
   memcpy(clone->privP,me->privP,sizeof(inu_data__privData));
   return clone;
}

void inu_data__vtableInitDefaults(inu_data__VTable *vtableP)
{
   inu_node__vtableInitDefaults(&vtableP->node_vtable);
   vtableP->node_vtable.ref_vtable.p_name        = inu_data__name;
   vtableP->node_vtable.ref_vtable.p_dtor        = inu_data__dtor;
   vtableP->node_vtable.ref_vtable.p_ctor        = (inu_ref__Ctor*)inu_data__ctor;

   vtableP->node_vtable.p_newInput  = inu_data__newInput;

   vtableP->p_freeData      = inu_data__freeData;
   vtableP->p_hdrMsgCodeGet = inu_data__msgCodeGet;
   vtableP->p_dataHdrGet    = inu_data__dataHdrGet;
   vtableP->p_dataHdrSet    = inu_data__dataHdrSet;
   vtableP->p_dataDuplicate = inu_data__dataDuplicate;
}

void inu_data__vtable_init(void)
{
   if (!_bool_vtable_initialized)
   {
      inu_data__vtableInitDefaults(&_vtable);
      _bool_vtable_initialized = true;
   }
}

const inu_data__VTable *inu_data__vtable_get(void)
{
   inu_data__vtable_init();
   return &_vtable;
}

/****************************************************************************
*
*  Function Name: inu_data__internalFreeCbWarpper
*
*  Description: CB used when the clone is freed. It then needs to free the real memory (and hdr)
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static ERRG_codeE inu_data__internalFreeCbWarpper(void *arg, MEM_POOLG_bufDescT *bufDescP)
{
   FIX_UNUSED_PARAM_WARN(arg);
   inu_data *clone = (inu_data*)bufDescP->dataP;
   ((inu_data__VTable*)clone->node.ref.p_vtable)->p_freeData(clone);
   return (ERRG_codeE)0;
}


/****************************************************************************
*
*  Function Name: inu_data__allocInternalDuplicatePool
*
*  Description: Allocates a pool of duplicates (clones). The duplicate is of the input inu_data.
*                   The system uses clones to encapsulate the data and descriptor of the data (hdr).
*                   The clone also provides the neccessery tools to access the data and hdr
*                   The implementation uses MEM_POOL for managing the pool of clones. Each clone has
*                   a pointer of his buffDescriptor, used when freeing the clone.
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void inu_data__allocInternalDuplicatePool(inu_data *me, unsigned int poolSize)
{
   UINT32 i;
   ERRG_codeE ret;
   MEM_POOLG_handleT poolH;
   MEM_POOLG_cfgT cfg;
   inu_data__privData *privP = (inu_data__privData*)me->privP;
   MEM_POOLG_bufDescT **bufDescTempArrayP;

   if (privP->dataPool)
      return;

   cfg.bufferSize = inu_factory__getRefSize((inu_ref__types)me->node.ref.refType);
   cfg.numBuffers = poolSize;
   cfg.resetBufPtrInAlloc = 0;
   cfg.freeCb  = inu_data__internalFreeCbWarpper;
   cfg.freeArg = me;
#if DEFSG_IS_GP
   cfg.type = MEM_POOLG_TYPE_ALLOC_CMEM_E;
#else
   cfg.type = MEM_POOLG_TYPE_ALLOC_HEAP_E;
#endif
   cfg.memP = NULL;
   ret = MEM_POOLG_initPool(&poolH, &cfg);

   if (ERRG_FAILED(ret))
   {
      assert(0);
   }

#if DEFSG_IS_HOST
   privP->dataPool = poolH;
#else
   bufDescTempArrayP = (MEM_POOLG_bufDescT**)malloc(sizeof(bufDescTempArrayP)*poolSize);

   privP->dataPool = poolH;

   //alloc and duplicate into the pool
   for (i = 0; i < MEM_POOLG_getNumBufs(privP->dataPool); i++)
   {
      ret = MEM_POOLG_alloc(privP->dataPool,MEM_POOLG_getBufSize(privP->dataPool),&bufDescTempArrayP[i]);
      if (ERRG_SUCCEEDED(ret) && bufDescTempArrayP[i])
      {
         inu_data__privData *clonePrivP;

         //creates a clone (duplicate of the inu_data)
         bufDescTempArrayP[i]->dataP = (UINT8*)((inu_data__VTable*)me->node.ref.p_vtable)->p_dataDuplicate(me,bufDescTempArrayP[i]->dataP);
         if (bufDescTempArrayP[i]->dataP == NULL)
         {
            printf("failed to alloc internal duplicate pool for %s, (%d)\n",me->node.ref.p_vtable->p_name((inu_ref*)me),poolSize);
            assert(0);
         }
         //save in the clone the pointer to the buffDesc. Its needed when freeing the clone, to return the buffDesc back to the pool
         clonePrivP = (inu_data__privData*)((inu_data*)(bufDescTempArrayP[i]->dataP))->privP;
         clonePrivP->internalDataBufP = bufDescTempArrayP[i];
         //save composite ptr
         clonePrivP->compositeP = me;
      }
   }

   //free
   for (i = 0; i < MEM_POOLG_getNumBufs(privP->dataPool); i++)
   {
      MEM_POOLG_free(bufDescTempArrayP[i]);
   }

   free(bufDescTempArrayP);
#endif
 }


/****************************************************************************
*
*  Function Name: inu_data__freeInternalDuplicatePool
*
*  Description: free the pool. wait for system to release all the buffers and then free them
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void inu_data__freeInternalDuplicatePool(inu_data *me)
{
   inu_data__privData *privP = (inu_data__privData*)me->privP;

   if (privP->dataPool)
   {
      MEM_POOLG_waitBuffReleaseClosePool(privP->dataPool);
   }
}


/****************************************************************************
*
*  Function Name: inu_data__allocInternal
*
*  Description: Allocated a clone from the pool. The pool is managed using MEM_POOL, and each
*                   bufDesc points to a clone.
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE inu_data__allocInternal(inu_data *me, inu_data **clone)
{
    if (NULL == me)
        return INU_DATA__ERR_NULL_PTR;

   ERRG_codeE ret = INU_DATA__RET_SUCCESS;

   MEM_POOLG_bufDescT *cloneBufDescP;
   inu_data__privData *privP = (inu_data__privData*)me->privP;

   if(privP->dataPool)
   {
      ret = MEM_POOLG_alloc(privP->dataPool,MEM_POOLG_getBufSize(privP->dataPool),&cloneBufDescP);
     if (ERRG_SUCCEEDED(ret))
      {
#if DEFSG_IS_HOST
          inu_data__privData* clonePrivP;
          cloneBufDescP->dataP = (UINT8*)((inu_data__VTable*)me->node.ref.p_vtable)->p_dataDuplicate(me, cloneBufDescP->dataP);
          if (cloneBufDescP->dataP == NULL)
          {
              printf("failed to alloc internal duplicate pool for %s)\n", me->node.ref.p_vtable->p_name((inu_ref*)me));
              MEM_POOLG_free(cloneBufDescP);
              ret = INU_DATA__ERR_OUT_OF_MEM;
          }
          else
          {
              //save in the clone the pointer to the buffDesc. Its needed when freeing the clone, to return the buffDesc back to the pool
              clonePrivP = (inu_data__privData*)((inu_data*)(cloneBufDescP->dataP))->privP;
              clonePrivP->internalDataBufP = cloneBufDescP;
              //save composite ptr
              clonePrivP->compositeP = me;
              *clone = (inu_data*)cloneBufDescP->dataP;
          }
#else
          *clone = (inu_data*)cloneBufDescP->dataP;
#endif
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "inu_data__allocInternal %s fail: bufSize = %d, numFree = %d, numTotal = %d\n", me->node.ref.p_vtable->p_name((inu_ref*)me),
         MEM_POOLG_getBufSize(privP->dataPool), MEM_POOLG_getNumFree(privP->dataPool),MEM_POOLG_getNumBufs(privP->dataPool));
         ret = INU_DATA__ERR_OUT_OF_RSRCS;
      }
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "inu_data__allocInternal %s fail: pool is null\n",me->node.ref.p_vtable->p_name((inu_ref*)me));
      ret = INU_DATA__ERR_UNEXPECTED;
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: inu_data__freeInternal
*
*  Description: called when a function or data has no longer usage of the clone, and no node to pass it
*                   to. The MEM_POOL manages a refCtr, when the refCtr will reach 0, the free callback
*                   is called, the clone returns to the pool and the data and header are freed.
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void inu_data__freeInternal(inu_data *clone)
{
   inu_data__privData *privP = (inu_data__privData*)clone->privP;
   MEM_POOLG_free(privP->internalDataBufP);
}


/****************************************************************************
*
*  Function Name: inu_data__duplicateInternal
*
*  Description: Used when a clone is dispatched to more then one node.
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void inu_data__duplicateInternal(inu_data *clone)
{
   inu_data__privData *privP = (inu_data__privData*)clone->privP;

   MEM_POOLG_duplicate(privP->internalDataBufP);
}

#if (DEFSG_PROCESSOR == DEFSG_GP)
/****************************************************************************
*
*  Function Name: inu_data__getPhyAddress
*
*  Description: Get the phy address of this clone
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE inu_data__getPhyAddress(inu_data *clone, UINT32 *physicalAddress)
{
   inu_data__privData *privP = (inu_data__privData*)clone->privP;

   return MEM_POOLG_getDataPhyAddr(privP->internalDataBufP, physicalAddress);
}
#endif

void inu_data__bufDescPtrSet(inu_data *me, MEM_POOLG_bufDescT *bufP)
{
   inu_data__privData *privP = (inu_data__privData*)me->privP;
   privP->dataBufP = bufP;
}

UINT32 inu_data__sizeGet(inu_data *me)
{
   inu_data__privData *privP = (inu_data__privData*)me->privP;
   if (privP)
   {
      return privP->size;
   }
   return 0;
}
#ifdef METADATA
void inu_data__metadata_sizeSet(inu_data* me, UINT32 size)
{
    inu_data__privData* privP = (inu_data__privData*)me->privP;
    if (privP)
    {
        privP->metadata_size = size;
    }
}
#endif

void inu_data__sizeSet(inu_data *me, UINT32 size, UINT32 chunkSize)
{
   inu_data__privData *privP = (inu_data__privData*)me->privP;

   if (privP)
   {
      if (chunkSize == 0)
      {
          printf("inu_data__sizeSet: wrong chunk size\n");
          assert(0);
      }

      privP->size = size;
      privP->chunksTotalNum = (UINT16)(size / chunkSize); //assuming we dont have more then 65536 chunks
      if (privP->chunksTotalNum == 1)
      {
         chunkSize = size;
      }
      privP->chunkSize = chunkSize;
#ifdef METADATA
      privP->metadata_size = 0; // By default we assume no metadata, its only enabled for Images
#endif
   }
}

int inu_data__isFromComposite(inu_data *composite, inu_data *clone)
{
   inu_data__privData *privP = (inu_data__privData*)clone->privP;
   return (privP->compositeP == composite);
}

int inu_data__isSameSource(inu_data *data1, inu_data *data2)
{
   inu_data__privData *priv1P = (inu_data__privData*)data1->privP;
   inu_data__privData *priv2P = (inu_data__privData*)data2->privP;
   //printf("source %s %d == %s %d ? \n",data1->node.ref.p_vtable->p_name((inu_ref*)data1),priv1P->sourceNode, data2->node.ref.p_vtable->p_name((inu_ref*)data2) , priv2P->sourceNode);
   return (priv1P->sourceNode == priv2P->sourceNode);
}

int inu_data__isSource(inu_data *data, inu_node *node)
{
   inu_data__privData *privDataP = (inu_data__privData*)data->privP;
   return (privDataP->sourceNode == node->ref.id);
}
#if DEFSG_IS_GP
ERRG_codeE inu_data__getMemPhyPtr(inu_data *data, UINT32 *phyAddressP)
{
   MEM_POOLG_bufDescT *bufDescP;
   ERRG_codeE         ret;

   inu_data__bufDescPtrGet(data, &bufDescP);
   ret = MEM_POOLG_getDataPhyAddr(bufDescP, phyAddressP);

   return ret;
}
#endif
#endif //#if DEFSG_IS_NOT_DSP

void inu_data__bufDescPtrGet(inu_data *me, MEM_POOLG_bufDescT **bufP)
{
   inu_data__privData *privP = (inu_data__privData*)me->privP;
   *bufP = privP->dataBufP;
}

void inu_data__setMode(inu_data__hdr_t *hdrP, int mode)
{
   INU_DATA__MODE_ID_SET(hdrP->flags, mode);
}

void inu_data__setCompress(inu_data *me)
{
   inu_data__hdr_t *hdrP;
   hdrP = (inu_data__hdr_t*)inu_data__getHdr(me);
   INU_DATA__COMPRESSION_FLAG_SET(hdrP->flags, 1);
}


/* API functions */
void *inu_data__getMemPtr(inu_dataH dataH)
{
   inu_data *data = (inu_data*)dataH;
#if DEFSG_IS_DSP
   return data->dataPhyAddr;
#else// if ((DEFSG_IS_GP) || (DEFSG_IS_HOST))
   MEM_POOLG_bufDescT *bufDescP;
   inu_data__bufDescPtrGet(data, &bufDescP);
#ifdef METADATA
   inu_data* me = (inu_data*)dataH;
   inu_data__privData* privP = (inu_data__privData*)me->privP;

   // once METADATA enabled, should there always be a METADATA SIZE offset to frame buffer, no matter whether the target channel-stream has METADATA or not
   // thus post-process will be able to process it cosistantly
   // modified to david @ 2022.11.09
   // need to confirm to be compliant with inu_sandbox

   /*Check that preamble bytes exist, before allowing us to dynamically offset the frame buffer based upon the frame buffer offset field*/
   //if(privP->metadata_size && inu_metadata__verifyPreambleBytesOK((char *)bufDescP->dataP,privP->size) == INU_METADATA__RET_SUCCESS)
   //{
   //    UINT32 framebufferSize = 0;
   //    if (inu_metadata__getFrameBufferOffset((char*)bufDescP->dataP, privP->size, &framebufferSize) == INU_METADATA__RET_SUCCESS)
   //    {
   //        /* We need to offset the framebuffer by metadata_size*/
   //        //printf("dataP: %p, framebufferoffset: %d, framebuffer: %p\n", bufDescP->dataP, framebufferoffset, bufDescP->dataP + framebufferoffset);
   //        return &bufDescP->dataP[framebufferSize];
   //    }
   //    else
   //    {
   //        return &bufDescP->dataP[0];
   //    }
   //}
   //else 
   //{
   //    return &bufDescP->dataP[0];
   //}

   UINT32 framebufferSize = inu_data__getMetadataSize(data);
   //printf("metadataSize: %d\n", framebufferSize);
   return &bufDescP->dataP[framebufferSize];
#endif
   return &bufDescP->dataP[0];
#endif
}

unsigned int inu_data__getMemSize(inu_dataH dataH)
{
   MEM_POOLG_bufDescT *bufDescP;
   inu_data__bufDescPtrGet((inu_data*)dataH, &bufDescP);
   return bufDescP->dataLen;
}

void *inu_data__getHdr(inu_dataH dataH)
{
   inu_data *data = (inu_data*) dataH;
   return (void*)(((UINT8*)data) + data->hdrOffsetInStruct);
}

UINT32 inu_data__chunkSizeGet(inu_dataH dataH)
{
   inu_data *me = (inu_data*) dataH;
   inu_data__privData *privP = (inu_data__privData*)me->privP;
   if (privP)
   {
      return privP->chunkSize;
   }
   return 0;
}

UINT16 inu_data__chunkTotalNumGet(inu_dataH dataH)
{
   inu_data *me = (inu_data*) dataH;
   inu_data__privData *privP = (inu_data__privData*)me->privP;
   if (privP)
   {
      return privP->chunksTotalNum;
   }
   return 0;
}

UINT32 inu_data__getMode(inu_dataH dataH)
{
   inu_data__hdr_t *hdrP = (inu_data__hdr_t*)inu_data__getHdr(dataH);
//   printf("getmode: mode = %d, flags = 0x%x \n",INU_DATA__MODE_ID_GET(hdrP->flags),hdrP->flags);
   return INU_DATA__MODE_ID_GET(hdrP->flags);
}

UINT32 inu_data__isCompressed(inu_dataH dataH)
{
   inu_data__hdr_t *hdrP = (inu_data__hdr_t*)inu_data__getHdr(dataH);
//   printf("getmode: mode = %d, flags = 0x%x \n",INU_DATA__MODE_ID_GET(hdrP->flags),hdrP->flags);
   return INU_DATA__COMPRESSION_FLAG_GET(hdrP->flags);
}
UINT32 inu_data__getMetadataSize(inu_dataH dataH)
{
#ifdef METADATA
    inu_data* me = (inu_data*)dataH;
    inu_data__privData* privP = (inu_data__privData*)me->privP;
    
    if (privP)
    {
        const UINT32 metadata_size = privP->metadata_size;
        return metadata_size;
    }

#endif
    return 0; /*Return 0 for normal images without metadata*/
}

#ifdef METADATA
ERRG_codeE inu_data__getMetadata(inu_dataH dataH, INU_Metadata_T* output)
{
    UINT32 metadata_size = inu_data__getMetadataSize(dataH);
    //printf("metadata_size: %d\n", metadata_size);
    if (metadata_size > 0)
    {
        inu_data* data = (inu_data*)dataH;
        MEM_POOLG_bufDescT* bufDescP;
        inu_data__bufDescPtrGet(data, &bufDescP);
        if (bufDescP->dataP != NULL)
        {
            return inu_metadata__deserialize((char*)bufDescP->dataP, metadata_size, output);
        }
        else
        {
            return INU_DATA__ERR_UNEXPECTED;
        }
    }
    else
    {
        return INU_DATA__ERR_INVALID_ARGS;
    }
}
#endif

