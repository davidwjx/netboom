#ifndef _INU_DATA_H_
#define _INU_DATA_H_

#include "inu_node.h"
#include "inu2.h"
#include "inu2_types.h"
#include "err_defs.h"
#include "mem_pool.h"
#include "inu_metadata.h"
struct inu_data;

/*******************************************************************************
*    VTABLE
******************************************************************************/

/**
* free data
*/
typedef void (inu_data__FreeData)(struct inu_data *me);

/**
* msg code get
*/
typedef UINT32 (inu_data__HdrMsgCodeGet)(struct inu_data *me);

/**
* data descriptor get
*/
typedef void* (inu_data__HdrGet)(struct inu_data *me);

/**
* data descriptor set
*/
typedef void (inu_data__HdrSet)(struct inu_data *me, void *hdrP);

/**
* data object duplicate
*/
typedef struct inu_data* (inu_data__Duplicate)(struct inu_data *me, UINT8 *bufP);


/**
* Virtual data table (vtable)
*/
typedef struct
{
   inu_node__VTable             node_vtable;
   inu_data__FreeData           *p_freeData;
   inu_data__HdrMsgCodeGet      *p_hdrMsgCodeGet;
   inu_data__HdrGet             *p_dataHdrGet;
   inu_data__HdrSet             *p_dataHdrSet;
   inu_data__Duplicate          *p_dataDuplicate;
} inu_data__VTable;

typedef struct inu_data
{
   inu_node                node;
   inu_data__hdr_t         dataHdr;
   UINT32                  hdrOffsetInStruct;
   UINT32                  dataPhyAddr;
   void                   *privP;
}inu_data;

typedef struct inu_data_CtorParamsTag
{
   inu_node__CtorParams    nodeCtorParams;
   inu_ref__container      sourceNode;
}inu_data__CtorParams;

const inu_data__VTable *inu_data__vtable_get(void);
void inu_data__vtableInitDefaults(inu_data__VTable *vtableP);
void inu_data__vtable_init(void);
ERRG_codeE inu_data__ctor(inu_data *me, inu_data__CtorParams *ctorParamsP);

void inu_data__allocInternalDuplicatePool(inu_data *me, unsigned int poolSize);
void inu_data__freeInternalDuplicatePool(inu_data *me);
ERRG_codeE inu_data__allocInternal(inu_data *me, inu_data **clone);
void inu_data__freeInternal(inu_data *clone);
void inu_data__duplicateInternal(inu_data *clone);
#if (DEFSG_PROCESSOR == DEFSG_GP)
ERRG_codeE inu_data__getPhyAddress(inu_data *clone, UINT32 *physicalAddress);
#endif

void inu_data__bufDescPtrGet(inu_data *me, MEM_POOLG_bufDescT **bufP);
void inu_data__bufDescPtrSet(inu_data *me, MEM_POOLG_bufDescT *bufP);
ERRG_codeE inu_data__getMemPhyPtr(inu_data *data, UINT32 *phyAddressP);

void inu_data__sizeSet(inu_data *me, UINT32 size, UINT32 chunkSize);
UINT32 inu_data__sizeGet(inu_data *me);
int inu_data__isFromComposite(inu_data *composite, inu_data *clone);
int inu_data__isSameSource(inu_data *data1, inu_data *data2);
int inu_data__isSource(inu_data *data, inu_node *node);
void inu_data__setCompress(inu_data *me);
void inu_data__setMode(inu_data__hdr_t *hdrP, int mode);
#ifdef METADATA
void inu_data__metadata_sizeSet(inu_data* me, UINT32 size);
#endif
#endif
