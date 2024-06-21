#include "inu_tsnr_calc.h"
#include "inu2_types.h"
#include "inu2_internal.h"
#include "inu_types.h"
#include "err_defs.h"
#include "internal_cmd.h"
#include "log.h"
#include "assert.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if DEFSG_IS_GP
#include "data_base.h"
#include "cmem.h"
#endif

#define INU_TSNR__NUM_RESULTS_BUFFER 4

typedef struct inu_tsnr_calc__privData
{
   MEM_POOLG_handleT poolH;
   inu_tsnr_data *tsnrDataP;
   inu_tsnr_calc__parameter_list_t params;
	UINT32 tsnrImgConvertWorkspaceP;
}inu_tsnr_calc__privData;

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_TSNR_CALC";

static const char* inu_tsnr_calc__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_tsnr_calc__dtor(inu_ref *me)
{
   inu_tsnr_calc__privData *privP = (inu_tsnr_calc__privData*)((inu_tsnr_calc*)me)->privP;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#if DEFSG_IS_GP
   MEM_POOLG_closePool(privP->poolH);

	if (privP->tsnrImgConvertWorkspaceP)
	{
		MEM_MAPG_free((UINT32*)privP->tsnrImgConvertWorkspaceP);
		privP->tsnrImgConvertWorkspaceP = 0;
	}

#endif
   free(privP);

}

static void inu_tsnr_calc__newOutput(inu_node *me, inu_node *output)
{
   inu_function__vtable_get()->node_vtable.p_newOutput(me,output);
   if (output->ref.refType == INU_TSNR_DATA_REF_TYPE)
   {
      inu_tsnr_calc *tsnr= (inu_tsnr_calc*)me;
      inu_tsnr_calc__privData *privP = (inu_tsnr_calc__privData*)tsnr->privP;
      //save fast_data node for easy access
      privP->tsnrDataP = (inu_tsnr_data*)output;
      //fast creates data, allocate internal data pool for dispatching data
#if DEFSG_IS_GP
      inu_data__allocInternalDuplicatePool((inu_data*)output, INU_TSNR__NUM_RESULTS_BUFFER); //TODO: set properly from calculating "path len time / time per frame"
#endif
   }
}


/* Constructor */
ERRG_codeE inu_tsnr_calc__ctor(inu_tsnr_calc *me, inu_tsnr_calc__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_tsnr_calc__privData *privP;
   ctorParamsP->functionCtorParams.workPriority = INU_FUNCTION__WORK_THR_PRIORITY_LOW;
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_tsnr_calc__privData*)malloc(sizeof(inu_tsnr_calc__privData));
      if(privP)
      {
         memset(privP, 0, sizeof(inu_tsnr_calc__privData));
         me->privP = privP;
#if DEFSG_IS_GP
         memcpy(&privP->params,&ctorParamsP->params,sizeof(inu_tsnr_calc__parameter_list_t));

         //saturation level must be in a correct range
         if (privP->params.saturatedGrayLevelValue == 255)
		 	return INU_TSNR_CALC__ERR_INVALID_ARGS;

	     MEM_POOLG_cfgT cfg;
	  
         cfg.bufferSize = TSNR_MAX_BLOB_SIZE;
         cfg.numBuffers = INU_TSNR__NUM_RESULTS_BUFFER;
         cfg.resetBufPtrInAlloc = 0;
         cfg.freeCb = NULL;
         cfg.freeArg = NULL;
         cfg.type = MEM_POOLG_TYPE_ALLOC_CMEM_E;
         cfg.memP = NULL;
	  
         ret = MEM_POOLG_initPool(&privP->poolH, &cfg);
         if (ERRG_FAILED(ret))
         {
            free(privP);
         }
#endif
      }
      else
      {
         ret = INU_TSNR_CALC__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}

static void inu_tsnr_calc__newInput(inu_node *me, inu_node *input)
{
#if DEFSG_IS_GP
	ERRG_codeE ret;
	inu_image__hdr_t *imgHdrP;
	UINT32     sizeOneFrame;
#endif
   inu_function__vtable_get()->node_vtable.p_newInput(me,input);	
#if DEFSG_IS_GP
   inu_tsnr_calc__privData *privP = (inu_tsnr_calc__privData*)((inu_tsnr_calc*)me)->privP;
   DATA_BASEG_tsnrDataBaseT *tsnrDbP;

   if (input->ref.refType == INU_IMAGE_REF_TYPE)	  
   {
       imgHdrP = (inu_image__hdr_t*)inu_data__getHdr(input);
   	 DATA_BASEG_accessDataBaseNoMutex((UINT8**)&tsnrDbP,DATA_BASEG_TSNR);
       tsnrDbP->imgFormat = imgHdrP->imgDescriptor.format;//save format for sending it to dsp when start tsnr

       if (!privP->tsnrImgConvertWorkspaceP)
       {
	       sizeOneFrame = imgHdrP->imgDescriptor.height * imgHdrP->imgDescriptor.width;
	       ret = MEM_MAPG_alloc(sizeOneFrame, &tsnrDbP->workspacePhyAddr, 
	       &privP->tsnrImgConvertWorkspaceP, 0);
			 if (ERRG_SUCCEEDED(ret))
			 {
				 tsnrDbP->workspaceSize = sizeOneFrame;			 
			 }
			 else
			 {
			    INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"mem alloc failed for tsnr calc workspace (%d)\n",sizeOneFrame);
			 }
       }
   } 
#endif
}


static ERRG_codeE inu_tsnr_calc__start(inu_function *me, inu_function__startParamsT *startParamsP)
{
   ERRG_codeE ret;
   ret = inu_function__vtable_get()->p_start(me, startParamsP);
   
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error inu_tsnr_calc__start\n");
   }
#if DEFSG_IS_GP   
   else
   {
	   ret = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)me),DATA_BASEG_ALG_TSNR,1, ICCG_CMD_TARGET_DSPA);
	   if (ERRG_FAILED(ret))
	   {
	      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
	   }
   }
#endif   
   return ret;
}

static ERRG_codeE inu_tsnr_calc__stop(inu_function *me, inu_function__stopParamsT *stopParamsP)
{
   ERRG_codeE ret;
   
   ret = inu_function__vtable_get()->p_stop(me, stopParamsP);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error inu_tsnr_calc__stop\n");
   }
#if DEFSG_IS_GP
   else
   {
	   ret = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)me),DATA_BASEG_ALG_TSNR,2, ICCG_CMD_TARGET_DSPA);
	   if (ERRG_FAILED(ret))
	   {
	      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
	   }
   }
#endif
   return ret;
}

#if DEFSG_IS_GP
static void inu_tsnr_calc__operate(inu_function *me, inu_function__operateParamsT *inputParamsP)
{
	ERRG_codeE				   ret;
	MEM_POOLG_bufDescT          *bufDescP = NULL;
	inu_data 				   *tsnr_data;
	inu_tsnr_data__hdr_t 	   tsnrHdr;
	inu_tsnr_calc__privData     *privP = (inu_tsnr_calc__privData*)((inu_tsnr_calc*)me)->privP;
	UINT32                      dspTarget;
	int                         i;

	//printf("inu_tsnr_calc__operate me %p privp %p privP->poolH %p allowedBadSNRBlocksThreshold %d\n",me,privP,privP->poolH,privP->params.allowedBadSNRBlocksThreshold);
	ret = MEM_POOLG_alloc(privP->poolH, MEM_POOLG_getBufSize(privP->poolH), &bufDescP);
	if (ERRG_SUCCEEDED(ret) && bufDescP)
	{
		bufDescP->dataLen = TSNR_MAX_BLOB_SIZE;
		tsnrHdr.paramsList.saturatedGrayLevelValue = privP->params.saturatedGrayLevelValue;
		tsnrHdr.paramsList.saturatedThreshold = privP->params.saturatedThreshold;
		tsnrHdr.paramsList.allowedBadSNRBlocksThreshold = privP->params.allowedBadSNRBlocksThreshold;
		tsnrHdr.paramsList.saturationThresholdInImage = privP->params.saturationThresholdInImage;
		tsnrHdr.paramsList.sideToCalc = privP->params.sideToCalc;

		//find image
		for (i = 0; i < inputParamsP->dataInputsNum; i++)
		{
			if (inu_ref__getRefType(inputParamsP->dataInputs[i]) == INU_IMAGE_REF_TYPE)
			{
				inu_image__hdr_t *hdrP = (inu_image__hdr_t*)inu_data__getHdr(inputParamsP->dataInputs[i]);
				tsnrHdr.dataHdr = hdrP->dataHdr;
			}
		}

		ret = inu_function__newData(me, (inu_data*)privP->tsnrDataP, bufDescP, &tsnrHdr, (inu_data**)&tsnr_data);
		if (ERRG_FAILED(ret))
		{
			assert(0);
			return;
		}

		inputParamsP->dataInputs[inputParamsP->dataInputsNum] = tsnr_data;
		inputParamsP->dataInputsNum++;

		dspTarget = 0;// TODO slamDb.dspTarget;
		CMEM_cacheWb(&tsnr_data, sizeof(inu_tsnr_data)); //TODO: if the params will not be changed when alg is active, remove
		ret = inu_function__sendDspMsg(me, inputParamsP, DATA_BASEG_ALG_TSNR, 0, 0, dspTarget);
		if (ERRG_FAILED(ret))
		{
			INU_REF__LOGG_PRINT(me, LOG_ERROR_E, NULL, "error sending dsp message\n");
		}
	}
	else
	{
		INU_REF__LOGG_PRINT(me, LOG_ERROR_E, NULL, "mem alloc failed\n");
	}
	inu_function__complete(me);
}


static void inu_tsnr_calc__dspAck(inu_function *me, inu_function__operateParamsT *inputParamsP, inu_function__coreE dspSource)
{
   UINT32 i;
  (void)dspSource;
  /*
   UINT16 dataInputsNum = inputParamsP->dataInputsNum;

   inu_tsnr_data *dataP = inputParamsP->dataInputs[dataInputsNum-1];
   CMEM_cacheInv((void*)&dataP->tsnr_dataHdr,sizeof(inu_tsnr_data__hdr_t));
   INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"TSNR GP PARAMS nValid %d tsnr %f satIndex %d\n", dataP->tsnr_dataHdr.nValid ,dataP->tsnr_dataHdr.tsnr, dataP->tsnr_dataHdr.satIndex );
*/
	
   for(i=0;i<inputParamsP->dataInputsNum;i++)
   {
      inu_function__doneData(me, inputParamsP->dataInputs[i]);
   }
   inu_function__complete(me);
}
#endif

void inu_tsnr_calc__vtable_init( void )
{
   if (!_bool_vtable_initialized) 
   {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_tsnr_calc__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_tsnr_calc__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_tsnr_calc__ctor;

      _vtable.node_vtable.p_newOutput= inu_tsnr_calc__newOutput;
	   _vtable.node_vtable.p_newInput= inu_tsnr_calc__newInput;

      _vtable.p_start   = inu_tsnr_calc__start;
      _vtable.p_stop    = inu_tsnr_calc__stop;
#if DEFSG_IS_GP
		_vtable.p_operate = inu_tsnr_calc__operate;
	   _vtable.p_dspAck  = inu_tsnr_calc__dspAck;
#endif
      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_tsnr_calc__vtable_get(void)
{
   inu_tsnr_calc__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}
