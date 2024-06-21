#ifndef _INU_FUNCTION_API_H_
#define _INU_FUNCTION_API_H_

#include "inu2.h"
#define MIN_FUNCTION_PIPE_SIZE 1
typedef enum
{
   DISABLE,
   ENABLE
}inu_function__state_e;

typedef enum
{
   INU_FUNCTION__WORK_THR_PRIORITY_LOW = 0,
   INU_FUNCTION__WORK_THR_PRIORITY_MEDIUM,
   INU_FUNCTION__WORK_THR_PRIORITY_HIGH,
   INU_FUNCTION__WORK_THR_PRIORITY_NO_THR,
   INU_FUNCTION__WORK_THR_PRIORITY_NUM,
}inu_function__workThrdPriority;

typedef enum
{
   INU_FUNCTION__XM4_CORE_E   = 0,
   INU_FUNCTION__EV62_CORE_E  = 1,
} inu_function__coreE;

#define INU_FUNCTION__MAX_NUM_INPUTS (10)

typedef struct
{
   inu_dataH dataInputs[INU_FUNCTION__MAX_NUM_INPUTS];
   UINT16    dataInputsNum;
} inu_function__operateParamsT;

typedef struct
{
   UINT32 dummy;
} inu_function__startParamsT;

typedef struct
{
   UINT32 dummy;
} inu_function__stopParamsT;

typedef struct
{
   inu_function__state_e state;
   UINT32                activationRefCount;
}inu_function__stateParam;

ERRG_codeE inu_function__start(inu_functionH meH, inu_function__startParamsT *startParamP);
ERRG_codeE inu_function__stop(inu_functionH meH, inu_function__stopParamsT *stopParamP);
void inu_function__getState(inu_functionH meH, inu_function__stateParam *stateParamP);
UINT32 inu_function__getOperateModes(inu_functionH meH);
UINT32 inu_function__isSyncedFunc(inu_functionH meH);
#if DEFSG_IS_GP
int inu_function__hasMoreInput(inu_functionH meH);
#endif
#endif