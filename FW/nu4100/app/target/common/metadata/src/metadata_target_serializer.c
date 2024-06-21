#include "inu_common.h"
#include "mem_pool.h"
#include "cmem.h"
#include "metadata_target_serializer.h"
#include "inu_metadata_serializer.h"
ERRG_codeE METADATA_target_serialize(MEM_POOLG_bufDescT *metadata_buffDescListP, INU_Metadata_T* toplevel, UINT32 size)
{
    inu_metadata__serialize(metadata_buffDescListP->dataP,size,toplevel);
    if(CMEM_cacheWb(metadata_buffDescListP->dataP,size) !=0)
    {
       LOGG_PRINT(LOG_INFO_E,NULL,"Metadata buffer invalidation failed");
    }
    //CMEM_cacheWbInvAll();
    return INU_METADATA__RET_SUCCESS;
}
ERRG_codeE METADATA_target_serializeFixedLocationDataOnly(MEM_POOLG_bufDescT *metadata_buffDescListP, INU_Metadata_T* toplevel, UINT32 size)
{
    ERRG_codeE ret = inu_metadata__serializeFixedLocationOnly(metadata_buffDescListP->dataP,size,toplevel);
    if(CMEM_cacheWb(metadata_buffDescListP->dataP,size) !=0)
        LOGG_PRINT(LOG_INFO_E,NULL,"Metadata buffer invalidation failed");
    return ret;
}