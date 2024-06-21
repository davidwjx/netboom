#ifndef _INU_REF_H_
#define _INU_REF_H_

#include "inu2.h"
#include "stdops.h"
#include "mem_pool.h"
#include "inu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct inu_ref; 
typedef int inu_ref__id_t;

typedef struct
{
   inu_ref__id_t       id;
}inu_ref__container;

typedef enum
{
   INU_REF__STATS_DISABLED = 0,
   INU_REF__STATS_ENABLED,
}inu_ref__statsEnable_e;

typedef struct
{
   inu_ref__container ref;
}inu_ref__DtorParams;

typedef struct
{
   void                    *device;
   void                    *ctrlCommH;
   void                    *dataCommH;
   inu_ref__id_t           id;
   inu_ref__statsEnable_e  statsEnable;
   BOOLEAN                 dataChanDirectFlag;
   char                 userName[MAX_NAME_LEN];
}inu_ref__CtorParams;

/*******************************************************************************
*    VTABLE
******************************************************************************/

/**
* Name
*/
typedef const char* (inu_ref__Name)(struct inu_ref *me);

/**
* Destructor
*/
typedef void (inu_ref__Dtor)(struct inu_ref *me);

/**
* Constructor
*/
typedef ERRG_codeE (inu_ref__Ctor)(struct inu_ref *ref, inu_ref__CtorParams *ctorParamsP);

/**
* Rx sync ctrl
*/
typedef int (inu_ref__rxSyncCtrl)(struct inu_ref *me, void *msgP, int msgCode);

/**
* Rx async ctrl
*/
typedef int (inu_ref__rxAsyncData)(struct inu_ref *me, UINT32 msgCode, UINT8 *msgP, UINT8 *dataP, UINT32 dataLen, void **bufDescP);

/**
* Tx done CB
*/
typedef int (inu_ref__TxDone)(struct inu_ref *me, void *bufP);

/**
* Show stats
*/
typedef void (inu_ref__ShowStats)(struct inu_ref *me);



/**
* Virtual functions table (vtable)
*/
typedef struct 
{
   inu_ref__Name            *p_name;
   inu_ref__Dtor            *p_dtor;
   inu_ref__Ctor            *p_ctor;
   inu_ref__rxSyncCtrl      *p_rxSyncCtrl;
   inu_ref__rxAsyncData     *p_rxAsyncData;
   inu_ref__TxDone          *p_txDone;
   inu_ref__ShowStats       *p_showStats;
} inu_ref__VTable;


typedef struct 
{
   UINT32                     openTimeSec;
   UINT16                     openTimeMs;

   UINT32                     prevStatsTimeSec;
   UINT16                     prevStatsTimeMs;

   UINT32                     chCnt;
   UINT32                     chDropCnt;
}inu_ref__statsT;


typedef struct inu_ref
{
   const inu_ref__VTable      *p_vtable;
   inu_ref__id_t              id;
   inu_ref__statsEnable_e     statsEnable;
   inu_ref__statsT            stats;
   inu_ref__types             refType;
	UINT32                     refSize;
   //private
   void                       *privP;
}inu_ref;

ERRG_codeE inu_ref__ctor(inu_ref *me, inu_ref__CtorParams *ctorParamsP);
void       inu_ref__setVTable(inu_ref* me, const inu_ref__VTable *vtable);
const inu_ref__VTable *inu_ref__vtable_get(void);
void       inu_ref__vtableInitDefaults( inu_ref__VTable *vtableP );

void      *inu_ref__getDevice(inu_ref *me);
void      *inu_ref__getCtrlComm(inu_ref *me);
void      *inu_ref__getDataComm(inu_ref *me);
BOOLEAN    inu_ref__dataChanDirect(inu_ref *ref);
ERRG_codeE inu_ref__sendCtrlSync(inu_ref *me, int cmd, void *argP, UINT32 timeout);
ERRG_codeE inu_ref__remoteNew(inu_ref *me, int cmd, void *argP);
ERRG_codeE inu_ref__remoteDelete(inu_ref *me, int cmd, void *argP);
void       inu_ref__ctrlSyncDone(inu_ref *me, int retVal, void *bufDesc); //consider as a callback
ERRG_codeE inu_ref__copyAndSendDataAsync(inu_ref *me, int cmd, void *argP, void *bufferP, int bufferLen);
ERRG_codeE inu_ref__sendDataAsync(inu_ref *me, int cmd, void *argP, void *dataBufDescP);
ERRG_codeE inu_ref__allocBuf(inu_ref *me, UINT32 len, MEM_POOLG_bufDescT **bufP);
ERRG_codeE inu_ref__timedAllocBuf(inu_ref *me, UINT32 len, UINT32 timeoutMsec, MEM_POOLG_bufDescT **outBufP);
ERRG_codeE inu_ref__removeMemPool(inu_ref *me, MEM_POOLG_handleT memPoolH);
ERRG_codeE inu_ref__addMemPool(inu_ref *me, UINT32 type, UINT32 bufferSize, UINT32 numBuffers,UINT8* buf, int rstptr, MEM_POOLG_handleT *memPoolH);
int        inu_ref__isSame(inu_ref *ref1, inu_ref *ref2);
void      *inu_ref__changeDataComm(inu_ref *me, void *comm);
void       inu_ref__log(inu_ref *ref, INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char *file, const char *func, int line, const char * fmt, ...);
void       inu_ref__deallocStaticPool();
UINT32     inu_ref__getSyncDfltTimeoutMsec();
void       inu_ref__useLongerTimeout(bool useLongerTimeout);
int        inu_ref__getDisableMultiInputSuportOverride(inu_ref *me);

#define INU_REF__LOGG_PRINT(ref, level, errCode, ...) inu_ref__log((inu_ref*)ref,level, (ERRG_codeE)errCode, _FILE_, __func__, __LINE__, __VA_ARGS__)

#ifdef __UART_ON_FPGA__
#define INU_REF__SYNC_DFLT_TIMEOUT_MSEC  (60000*1000)
#else
//Longer timeout. Default value is (50*1000). For testing of MIPI RX side, we set 
//INU_REF__SYNC_DFLT_TIMEOUT_MSEC by "-t" option in SandBoxFW to (5*1000*1000)
#define INU_REF__SYNC_DFLT_TIMEOUT_MSEC  inu_ref__getSyncDfltTimeoutMsec()
#endif

#ifdef __cplusplus
}
#endif

#endif
