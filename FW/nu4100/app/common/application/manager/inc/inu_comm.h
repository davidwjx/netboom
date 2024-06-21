#ifndef _INU_COMM_H_
#define _INU_COMM_H_

#include "err_defs.h"
#include "mem_pool.h"
#include "inu_ref.h"
#include "os_lyr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INU_COMM__MAX_NUM_REGISTERED (100)

struct inu_comm;

typedef void (inu_comm__rxDoneCb)(void *ref, void *bufferP, int msgCode);
typedef void (inu_comm__txDoneCb)(void *ref, void *bufferP);

typedef struct
{
   inu_comm__rxDoneCb *rxDoneCb;
   inu_comm__txDoneCb *txDoneCb;
}inu_comm__callbackParams;

typedef struct
{
   int                     assigned;
   void                    *ref;
}inu_comm__registeredRef;

typedef struct inu_comm
{
   inu_comm__registeredRef registeredRef[INU_COMM__MAX_NUM_REGISTERED];
   void                    *conn;
   int                     refCtr;
   int                     id;
   OS_LYRG_mutexT          registeredRefListMutex;
}inu_comm;

typedef struct
{
   int                     id;
}inu_comm__CtorParams;

typedef struct
{
   UINT32            type;
   UINT32            bufferSize;
   UINT32            numBuffers;
   int               rstptr;
}inu_comm__memParams;


ERRG_codeE  inu_comm__register(inu_comm *me, inu_ref *ref);
void        inu_comm__unregister(inu_comm *me, inu_ref *ref);
ERRG_codeE inu_comm__sendIoctl(inu_comm *me, inu_ref *ref, int cmd, void *argP);
ERRG_codeE  inu_comm__sendData(inu_comm *me, inu_ref *ref, int cmd, void *argP, MEM_POOLG_bufDescT *dataBufDescP, void *bufferP, int bufferLen);
ERRG_codeE  inu_comm__new(inu_comm **comm, const inu_comm__CtorParams *ctorParamsP);
void        inu_comm__delete(inu_comm *me);
ERRG_codeE  inu_comm__timedAllocBuffByRefId(void *me, UINT32 refId, UINT32 len, UINT32 timeoutMsec, void **bufDescP);
UINT32      inu_comm__getFullHdrMarshalSize(void);

#ifdef __cplusplus
}
#endif

#endif
