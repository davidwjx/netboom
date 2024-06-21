#ifndef _INU_FUNCTION_H_
#define _INU_FUNCTION_H_

#include "inu2.h"
#include "inu_node.h"
#include "inu_data.h"



struct inu_function;

/*******************************************************************************
*    VTABLE
******************************************************************************/

/**
* function operation
*/
typedef void (inu_function__Operate)(struct inu_function *me, inu_function__operateParamsT *paramsP);


/**
* function start
*/
typedef ERRG_codeE (inu_function__Start)(struct inu_function *me, inu_function__startParamsT *startParamP);


/**
* function stop
*/
typedef ERRG_codeE (inu_function__Stop)(struct inu_function *me, inu_function__stopParamsT *stopParamP);


/**
* function dsp ack
*/
typedef void (inu_function__DspAck)(struct inu_function *me, inu_function__operateParamsT *paramsP, inu_function__coreE dspSource);


/**
* Virtual functions table (vtable)
*/
typedef struct
{
   inu_node__VTable                 node_vtable;
   inu_function__Operate            *p_operate;
   inu_function__Start              *p_start;
   inu_function__Stop               *p_stop;
   inu_function__DspAck             *p_dspAck;
} inu_function__VTable;

typedef struct inu_function
{
   inu_node node;

   void *privP;
}inu_function;

typedef struct inu_function_CtorParamsTag
{
   inu_node__CtorParams           nodeCtorParams;
   inu_function__workThrdPriority workPriority;
   UINT32                         pipeMaxSize;
   UINT32                         inputQueDepth;
   UINT32                         minInptsToOprt;
   UINT32                         mode;
   UINT32                         syncedFunc;
}inu_function__CtorParams;

const inu_function__VTable *inu_function__vtable_get(void);
inu_function *inu_function__new(inu_function__CtorParams *ctorParamsP);
void inu_function__delete(inu_function *node);
ERRG_codeE inu_function__ctor(inu_function *ref, inu_function__CtorParams *ctorParamsP);


void inu_function__setWorkThrdPriority(inu_function *me,    inu_function__workThrdPriority workPriority);
ERRG_codeE inu_function__enqueData(inu_function *me, inu_data *data, inu_data *clone);
void inu_function__vtableInitDefaults(inu_function__VTable *vtableP);
void inu_function__emptyAllInputQues(inu_function *me);
void inu_function__startOperate(inu_function *me);
ERRG_codeE inu_function__complete(inu_function *me);
ERRG_codeE inu_function__newData(inu_function *me, inu_data *data, MEM_POOLG_bufDescT *bufP, void *hdrP, inu_data **newData);
ERRG_codeE inu_function__doneData(inu_function *me, inu_data *data);
#if DEFSG_GP
/** @brief A version of inu_function__doneData that handles the event within the current context with the aim of avoiding a context switch
 * 
 *  @returns Returns an error code
*/
ERRG_codeE inu_function__doneData_handleImmediately(inu_function *me, inu_data *data);
ERRG_codeE inu_function__complete__handleImmediately(inu_function *me);
#endif
ERRG_codeE inu_function__sendDspMsg(inu_function *me, inu_function__operateParamsT *paramsP, UINT32 alg, UINT32 data, UINT32 command, UINT32 dspTarget);

#endif
