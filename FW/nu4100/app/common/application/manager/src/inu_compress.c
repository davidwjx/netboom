#include "inu2_types.h"
#include "inu2_internal.h"
#include "inu_types.h"
#include "err_defs.h"
#include "internal_cmd.h"
#include "mem_pool.h"
#include "string.h"
#include "log.h"
#include "lz4.h"
#include "assert.h"


#define ACCELERATOR (1)
//#define DECOMPRESS_TEST
//#define EXT_STATE_MEMORY

typedef struct inu_compress__privData
{
   inu_data          *dataP;
#ifdef EXT_STATE_MEMORY
   UINT32            stateVirt;
   UINT32            statePhy;
#endif
   MEM_POOLG_handleT memPoolH;
}inu_compress__privData;

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_COMPRESS";

static const char* inu_compress__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_compress__dtor(inu_ref *me)
{
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#ifdef EXT_STATE_MEMORY
   inu_compress *compress = (inu_compress*)me;
   inu_compress__privData *privP = (inu_compress__privData*)compress->privP;
   MEM_MAPG_free(privP->stateVirt);
#endif
   free(((inu_compress*)me)->privP);
}

/* Constructor */
ERRG_codeE inu_compress__ctor(inu_compress *me, inu_compress__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_compress__privData *privP;

   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_compress__privData*)malloc(sizeof(inu_compress__privData));
      if(privP)
      {
         memset(privP, 0, sizeof(inu_compress__privData));
         me->privP = privP;
#if DEFSG_IS_GP
#ifdef EXT_STATE_MEMORY
         ret = MEM_MAPG_alloc(LZ4_sizeofState(),&privP->statePhy,&privP->stateVirt,1);
         if (ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E,ret,"error alloc %d for inu_compress\n",LZ4_sizeofState());
         }
#endif
#endif
      }
      else
      {
         ret = INU_COMPRESS__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}

static void inu_compress__newInput(inu_node *me, inu_node *input)
{
   inu_data *output;
   inu_function__vtable_get()->node_vtable.p_newInput(me, input);
   output = (inu_data*)inu_node__getNextOutputNode(me,NULL);
   if (output)
   {
      //overwrite the output with the input's header
      ((inu_data__VTable*)(output->node.ref.p_vtable))->p_dataHdrSet(output,inu_data__getHdr(input));
   }
}


static void inu_compress__newOutput(inu_node *me, inu_node *output)
{
   inu_compress *compress = (inu_compress*)me;
   inu_compress__privData *privP = (inu_compress__privData*)compress->privP;
   inu_data *input;
   inu_function__vtable_get()->node_vtable.p_newOutput(me, output);
   //save data to ease access
   privP->dataP = (inu_data*)output;

   input = (inu_data*)inu_node__getNextInputNode(me,NULL);
   if (input)
   {
      //overwrite the output with the input's header
      ((inu_data__VTable*)(output->ref.p_vtable))->p_dataHdrSet((inu_data*)output,inu_data__getHdr(input));
   }
}

#if DEFSG_IS_GP
static void inu_compress__operate(inu_function *me, inu_function__operateParamsT *paramsP)
{
   inu_compress           *compress = (inu_compress*)me;
   inu_compress__privData *privP = (inu_compress__privData*)compress->privP;

   MEM_POOLG_bufDescT     *bufDescP = NULL;
   inu_data               *dataP;
   ERRG_codeE             ret;
   UINT32                 dataSize  = inu_data__getMemSize(paramsP->dataInputs[0]);
   void                   *memP     = inu_data__getMemPtr(paramsP->dataInputs[0]);
  
   //allocate mempool
   if (!privP->memPoolH)
   {
      MEM_POOLG_cfgT poolCfg;

      poolCfg.bufferSize         = LZ4_compressBound(dataSize);
      poolCfg.numBuffers         = 3;
      poolCfg.freeCb             = NULL;
      poolCfg.resetBufPtrInAlloc = FALSE;
      poolCfg.type               = MEM_POOLG_TYPE_ALLOC_CMEM_E; //this should be a parameter, and changed to heap if we going to save data to flash
      poolCfg.memP               = NULL;
      ret = MEM_POOLG_initPool(&(privP->memPoolH), &poolCfg);
      if (ERRG_FAILED(ret))
      {
         assert(0);
      }
      else
      {
         printf("mempool init success for size %d\n",poolCfg.bufferSize);
      }
   }

   ret = MEM_POOLG_alloc(privP->memPoolH, MEM_POOLG_getBufSize(privP->memPoolH), &bufDescP);
   if (ERRG_SUCCEEDED(ret))
   {
      UINT64 start,end;
      OS_LYRG_getTimeNsec(&start);

      //invalidate the data, make sure data is up to date in the cache
      inu_fdk__gpCacheInv(memP,dataSize);

      //compress
#ifndef EXT_STATE_MEMORY
      bufDescP->dataLen = LZ4_compress_fast(memP,(char*)bufDescP->dataP,dataSize,bufDescP->size,ACCELERATOR);
#else
      bufDescP->dataLen = LZ4_compress_fast_extState((void*)privP->stateVirt,memP,(char*)bufDescP->dataP,dataSize,bufDescP->size,ACCELERATOR);
#endif
      //flush the cache
      inu_fdk__gpCacheWr(bufDescP->dataP,bufDescP->dataLen); 
      
      OS_LYRG_getTimeNsec(&end);      
      printf("compress time %llu ns compressedSize = %d origSize = %d lz4 compress bound %d accel %d\n",end-start,bufDescP->dataLen,dataSize,LZ4_compressBound(dataSize),ACCELERATOR);
      ret = inu_function__newData(me,privP->dataP,bufDescP,inu_data__getHdr(paramsP->dataInputs[0]),&dataP); //set same hdr as input?
      if (ERRG_FAILED(ret))
      {
         assert(0);
      }
      else
      {
#ifdef DECOMPRESS_TEST
         int decompressedSize = 0 , memcmpRes = -1;
         static void   *dst = NULL;

         if (!dst)
            dst = malloc(dataSize);

         memset(dst,0,dataSize);         
         decompressedSize = LZ4_decompress_safe((const char*)bufDescP->dataP,dst,bufDescP->dataLen,dataSize);

         if (decompressedSize != dataSize)
         {
            OS_LYRG_getTimeNsec(&start);    
            printf("decompress end (%llu ns) decompressedSize = %d, origSize = %d\n",start-end,decompressedSize,dataSize);
            assert(0);
         }
         else
         {
            memcmpRes = memcmp(dst,memP,dataSize);
            printf("memcmp %d\n",memcmpRes);
         }
#endif
         //mark data as compressed
         inu_data__setCompress(dataP);
         inu_function__doneData(me,dataP);
      }
   }
   
   ret = inu_function__doneData(me,paramsP->dataInputs[0]);
   if (ERRG_SUCCEEDED(ret))
   {
      inu_function__complete(me);
   }
}
#endif

static ERRG_codeE inu_compress__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
   ret = inu_function__vtable_get()->p_start(me, startParamP);
   return ret;
}

static ERRG_codeE inu_compress__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
   return ret;
}


void inu_compress__vtable_init(void)
{
   if (!_bool_vtable_initialized) {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_compress__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_compress__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_compress__ctor;

      _vtable.node_vtable.p_newOutput = inu_compress__newOutput;
      _vtable.node_vtable.p_newInput = inu_compress__newInput;
#if DEFSG_IS_GP
      _vtable.p_operate = inu_compress__operate;
#endif
      _vtable.p_start = inu_compress__start;
      _vtable.p_stop  = inu_compress__stop;

      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_compress__vtable_get(void)
{
   inu_compress__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}


