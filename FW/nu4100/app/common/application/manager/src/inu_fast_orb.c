#include "inu2_types.h"
#include "inu2_internal.h"
#include "inu_types.h"
#include "err_defs.h"
#include "log.h"
#include "assert.h"

#if DEFSG_IS_GP
#include "data_base.h"
#include "cmem.h"
#endif

typedef struct inu_fast_orb__privData
{
   MEM_POOLG_handleT poolH;
   inu_fast_orb_data     *fastDataP;
}inu_fast_orb__privData;

//#define GP_PRINTS_ENABLE

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_FAST_ORB";
#ifdef GP_PRINTS_ENABLE
int fastOrbFrameNum = 0;
#endif

static const char* inu_fast_orb__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_fast_orb__dtor(inu_ref *me)
{
   inu_fast_orb__privData *privP = (inu_fast_orb__privData*)((inu_fast_orb*)me)->privP;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
   MEM_POOLG_closePool(privP->poolH);
   free(privP);
}

/* Constructor */
ERRG_codeE inu_fast_orb__ctor(inu_fast_orb *me, inu_fast_orb__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_fast_orb__privData *privP;
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_fast_orb__privData*)malloc(sizeof(inu_fast_orb__privData));
      if (!privP)
      {
         return INU_FAST_ORB__ERR_OUT_OF_MEM;
      }
      memset(privP,0,sizeof(inu_fast_orb__privData));
      me->privP = privP;
#if DEFSG_IS_GP
      MEM_POOLG_cfgT cfg;

      cfg.bufferSize = FAST_ORB_OUT_BUF_SIZE;
      cfg.numBuffers = 5;
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
   return ret;
}

static void inu_fast_orb__newOutput(inu_node *me, inu_node *output)
{
   inu_function__vtable_get()->node_vtable.p_newOutput(me,output);
   if (output->ref.refType == INU_FAST_ORB_DATA_REF_TYPE)
   {
      inu_fast_orb *fast = (inu_fast_orb*)me;
      inu_fast_orb__privData *privP = (inu_fast_orb__privData*)fast->privP;
      //save fast_data node for easy access
      privP->fastDataP = (inu_fast_orb_data*)output;
   }
}


static int inu_fast_orb__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   return inu_function__vtable_get()->node_vtable.ref_vtable.p_rxSyncCtrl(me, msgP, msgCode);
}


static ERRG_codeE inu_fast_orb__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
   //inu_image *image = NULL;
   //DATA_BASEG_fastDataDataBaseT *fastDataBaseP;
   //inu_fast_orb__privData *privP = (inu_fast_orb__privData*)((inu_fast_orb*)me)->privP;
   
   ret = inu_function__vtable_get()->p_start(me, startParamP);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error inu_fast_orb__start\n");
   }
/*
   image = inu_node__getNextOutputNode(me,image);
   while(image && image->data.node.ref.refType != INU_IMAGE_REF_TYPE)
   {
      image = inu_node__getNextOutputNode(me,image);
   }

   if(!image)
   {
      assert(0);
   }

//  image->imageHdr.imgDescriptor....
   DATA_BASEG_accessDataBase((UINT8**)&fastDataBaseP,DATA_BASEG_ALG_FAST_ORB);
   fastDataBaseP->dummy;//...
*/

#if DEFSG_IS_GP
   ret = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)me),DATA_BASEG_ALG_FAST_ORB,1,ICCG_CMD_TARGET_DSPA);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
   }
#endif
   return ret;
}

static ERRG_codeE inu_fast_orb__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
   //inu_fast_orb__privData *privP = (inu_fast_orb__privData*)((inu_fast_orb*)me)->privP;
   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
   return ret;
}

#if DEFSG_IS_GP
static void inu_fast_orb__operate(inu_function *me, inu_function__operateParamsT *inputParamsP)
{
	ERRG_codeE                  ret;
	MEM_POOLG_bufDescT          *bufDescP = NULL;
	inu_data                    *fast_orb_data;
	inu_fast_orb__privData      *privP = (inu_fast_orb__privData *)(((inu_fast_orb*)me)->privP);
	inu_fast_orb_data__hdr_t    fastHdr;

	inu_function__vtable_get()->p_operate(me, inputParamsP);

	ret = MEM_POOLG_alloc(privP->poolH, MEM_POOLG_getBufSize(privP->poolH), &bufDescP);
	if (ERRG_SUCCEEDED(ret) && bufDescP)
	{
		inu_data__hdr_t *dataHdrP = inu_data__getHdr(inputParamsP->dataInputs[0]);
		//assumption - only one input. if not, need to search input params for the image
		fastHdr.dataHdr = *dataHdrP;

		ret = inu_function__newData((inu_function*)me, (inu_data*)privP->fastDataP, bufDescP, &fastHdr, (inu_data**)&fast_orb_data);
		if (ERRG_FAILED(ret))
		{
			assert(0);
			return;
		}

		bufDescP->dataLen = FAST_ORB_OUT_BUF_SIZE;//todo: move this to dspack, and set the size ceva generated
		inputParamsP->dataInputs[inputParamsP->dataInputsNum] = fast_orb_data;
		inputParamsP->dataInputsNum++;
		ret = inu_function__sendDspMsg(me, inputParamsP, DATA_BASEG_ALG_FAST_ORB, 0, 0, ICCG_CMD_TARGET_DSPA);
		if (ERRG_FAILED(ret))
		{
			INU_REF__LOGG_PRINT(me, LOG_ERROR_E, NULL, "error sending dsp message\n");
		}
	}
	else
	{
		INU_REF__LOGG_PRINT(me, LOG_ERROR_E, NULL, "mem alloc failed\n");
	}
}


static void inu_fast_orb__dspAck(inu_function *me, inu_function__operateParamsT *inputParamsP, inu_function__coreE dspSource)
{
   UINT32 i;
   inu_image         *imageP = inputParamsP->dataInputs[0];
   inu_fast_orb_data *fast_orb_dataP = inputParamsP->dataInputs[1];
   FIX_UNUSED_PARAM_WARN(dspSource);

	CMEM_cacheWbInv((void*)&fast_orb_dataP->fast_orb_dataHdr, sizeof(inu_fast_orb_data__hdr_t));
   INU_REF__LOGG_PRINT(me, LOG_DEBUG_E,NULL,"image params %d %d\n", imageP->imageHdr.imgDescriptor.width, imageP->imageHdr.imgDescriptor.height);

#ifdef GP_PRINTS_ENABLE
   UINT32 virtAddressX, virtAddressY, virtAddressScore, virtAddressDesc;
   virtAddressX = (UINT32)inu_data__getMemPtr(fast_orb_dataP);
   UINT32 yOffset = fast_orb_dataP->fast_orb_dataHdr.yOffset;
   UINT32 scoreOffset = fast_orb_dataP->fast_orb_dataHdr.scoreOffset;
   UINT32 descOffset = fast_orb_dataP->fast_orb_dataHdr.descOffset;

   
   UINT16 *resultsXP = (UINT16*)virtAddressX;
   UINT16 *resultsYP = (UINT16*)(virtAddressX + yOffset);
   UINT16 *resultsScoreP = (UINT16*)(virtAddressX + scoreOffset);
   UINT32 *desc = (UINT32*)(virtAddressX + descOffset);

   CMEM_cacheWbInvAll();//(void*)resultsXP,FAST_ORB_OUT_BUF_SIZE);

   fastOrbFrameNum++;

   printf( "Frame: %d\n", fastOrbFrameNum);
   printf( "Number of points: %u\n", fast_orb_dataP->fast_orb_dataHdr.numKeyPoints);
   for (i=0; i< fast_orb_dataP->fast_orb_dataHdr.numKeyPoints; i++)
   {
      UINT32 *d = &desc[8*i];
      printf("[%4u, %4u, %4u]    ", resultsXP[i], resultsYP[i], resultsScoreP[i]);
      printf("[%12x, %12x, %12x, %12x, %12x, %12x, %12x, %12x]\n", d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]);
   }
#endif

#if 0
   inu_fast_orb__printFeaturesOnScreen(resultsXP, resultsYP, (UINT8*)ORBdb.data.virtualFrameAddress, 640, fast_orb_dataP->fast_orb_dataHdr.numKeyPoints);
#endif

   INU_REF__LOGG_PRINT(me, LOG_DEBUG_E,NULL,"orb done\n");
   for(i=0;i<inputParamsP->dataInputsNum;i++)
   {
      inu_function__doneData(me, inputParamsP->dataInputs[i]);
   }
   inu_function__complete(me);
}
#endif

void inu_fast_orb__vtable_init(void)
{
   if (!_bool_vtable_initialized) {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_fast_orb__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_fast_orb__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_fast_orb__ctor;
      _vtable.node_vtable.ref_vtable.p_rxSyncCtrl = inu_fast_orb__rxIoctl;

     _vtable.node_vtable.p_newOutput = inu_fast_orb__newOutput;

      _vtable.p_start   = inu_fast_orb__start;
      _vtable.p_stop    = inu_fast_orb__stop;
#if DEFSG_IS_GP
      _vtable.p_dspAck  = inu_fast_orb__dspAck;
		_vtable.p_operate = inu_fast_orb__operate;
#endif

      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_fast_orb__vtable_get(void)
{
   inu_fast_orb__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}
