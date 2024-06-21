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
#include "inu_metadata.h"
#include "inu_metadata_serializer.h"

#if DEFSG_IS_GP
#include "rtc.h"
#include "cmem.h"
#include "metadata_updater.h"
#include "metadata_target_serializer.h"
#endif
/*Injects metadata into an ia*/

typedef struct inu_metadata_injector__privData
{
    inu_data *dataP;
#ifdef EXT_STATE_MEMORY
    UINT32 stateVirt;
    UINT32 statePhy;
#endif
    MEM_POOLG_handleT memPoolH;
    inu_data *input;
    INT32 readerID;
} inu_metadata_injector__privData;

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char *name = "inu_metadata_injector";

static const char *inu_metadata_injector__name(inu_ref *me)
{
    FIX_UNUSED_PARAM_WARN(me);
    return name;
}

static void inu_metadata_injector__dtor(inu_ref *me)
{
    inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#ifdef EXT_STATE_MEMORY
    inu_metadata_injector *metadata_injector = (inu_metadata_injector *)me;
    inu_metadata_injector__privData *privP = (inu_metadata_injector__privData *)metadata_injector->privP;
    MEM_MAPG_free(privP->stateVirt);
#endif
    free(((inu_metadata_injector *)me)->privP);
}

/* Constructor */
ERRG_codeE inu_metadata_injector__ctor(inu_metadata_injector *me, inu_metadata_injector__CtorParams *ctorParamsP)
{
    ERRG_codeE ret;
    inu_metadata_injector__privData *privP;

    ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
    if (ERRG_SUCCEEDED(ret))
    {
        privP = (inu_metadata_injector__privData *)malloc(sizeof(inu_metadata_injector__privData));
        if (privP)
        {
            memset(privP, 0, sizeof(inu_metadata_injector__privData));
            me->privP = privP;
            privP->readerID = ctorParamsP->readerID;
        }
    }
    return ret;
}

static void inu_metadata_injector__newInput(inu_node *me, inu_node *input)
{
    inu_data *output;

    inu_function__vtable_get()->node_vtable.p_newInput(me, input);
    output = (inu_data *)inu_node__getNextOutputNode(me, NULL);
    LOGG_PRINT(LOG_DEBUG_E, NULL, "New Metadata injector Input \n");
}

static void inu_metadata_injector__newOutput(inu_node *me, inu_node *output)
{
    inu_metadata_injector *metadata_injector = (inu_metadata_injector *)me;
    inu_metadata_injector__privData *privP = (inu_metadata_injector__privData *)metadata_injector->privP;
    inu_function__vtable_get()->node_vtable.p_newOutput(me, output);
    // save data to ease access
    privP->dataP = (inu_data *)output;
    /*Store the input to this node*/
    privP->input = (inu_data *)inu_node__getNextInputNode(me, NULL);

    LOGG_PRINT(LOG_DEBUG_E, NULL, "New Metadata injector output \n");
}

static void inu_metadata_injector__operate(inu_function *me, inu_function__operateParamsT *paramsP)
{
#if DEFSG_IS_GP
    inu_data__hdr_t        *hdrP;
    inu_metadata_injector *metadata_injector = (inu_metadata_injector *)me;
    inu_metadata_injector__privData *privP = (inu_metadata_injector__privData *)metadata_injector->privP;
    inu_data               *input = (inu_data*)paramsP->dataInputs[0];

    MEM_POOLG_bufDescT *bufDescP = NULL;
    inu_data *dataP;
    ERRG_codeE ret = RET_SUCCESS;
    UINT32 dataSize = inu_data__getMemSize(paramsP->dataInputs[0]);
    void *memP = inu_data__getMemPtr(paramsP->dataInputs[0]);
    LOGG_PRINT(LOG_DEBUG_E, NULL, "Injector operate function \n");
    inu_data__bufDescPtrGet(paramsP->dataInputs[0], &bufDescP);
    inu_imageH imageInH = paramsP->dataInputs[0];
    inu_image__hdr_t* inputImgHdrP = (inu_image__hdr_t*)inu_data__getHdr(imageInH);

    /*Inject Metadata into the first line of the image */
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Reader ID: %d \n",  privP->readerID);
    /* We will search for a registered metadata channel using the reader ID configured in the SW graph*/
    if (ERRG_SUCCEEDED(METADATA_UPDATER_injectMetadata(bufDescP, privP->readerID,inputImgHdrP)))
    {
        /*Duplicate the buffer since we are going to re-use the input buffer*/
        MEM_POOLG_duplicate(bufDescP);
        /*This creates an inu_data clone*/
        ret = inu_function__newData(me, privP->dataP, bufDescP, inputImgHdrP, (inu_data **)&dataP);
        if (ERRG_SUCCEEDED(ret))
        {
            /*Signal that we have new data to the graph*/
            ret = inu_function__doneData(me, dataP);
        }
    }
    else
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Could not find a registered channel with reader id %d \n", privP->readerID);
    }
    // Free the input
    if (ERRG_SUCCEEDED(ret))
    {
        ret = inu_function__doneData(me, paramsP->dataInputs[0]);
    }
    else
    {
        assert(0);
    }
    inu_function__complete(me);
#endif
}

static ERRG_codeE inu_metadata_injector__start(inu_function *me, inu_function__startParamsT *startParamP)
{
    ERRG_codeE ret;
    ret = inu_function__vtable_get()->p_start(me, startParamP);
    LOGG_PRINT(LOG_INFO_E, NULL, "Starting metadata injector done \n");
    return ret;
}

static ERRG_codeE inu_metadata_injector__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
    ERRG_codeE ret;
    ret = inu_function__vtable_get()->p_stop(me, stopParamP);
    return ret;
}

void inu_metadata_injector__vtable_init(void)
{
    if (!_bool_vtable_initialized)
    {
        inu_function__vtableInitDefaults(&_vtable);
        _vtable.node_vtable.ref_vtable.p_name = inu_metadata_injector__name;
        _vtable.node_vtable.ref_vtable.p_dtor = inu_metadata_injector__dtor;
        _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor *)inu_metadata_injector__ctor;

        _vtable.node_vtable.p_newOutput = inu_metadata_injector__newOutput;
        _vtable.node_vtable.p_newInput = inu_metadata_injector__newInput;
#if DEFSG_IS_GP
        _vtable.p_operate = inu_metadata_injector__operate;
#endif
        _vtable.p_start = inu_metadata_injector__start;
        _vtable.p_stop = inu_metadata_injector__stop;

        _bool_vtable_initialized = true;
    }
}

const inu_function__VTable *inu_metadata_injector__vtable_get(void)
{
    printf("Getting metadata table \n");
    inu_metadata_injector__vtable_init();
    return (const inu_function__VTable *)&_vtable;
}