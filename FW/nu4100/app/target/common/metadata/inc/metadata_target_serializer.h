#pragma once
#include "inu_metadata.h"
#include "inu_metadata_serializer.h"
#ifdef __cplusplus
extern "C" {
#endif
/*  @brief INU_Metadata Serializer for the target that performs cache invalidation and accepts a mempool buffer
    @param metadata_buffDescListP Pointer to mempool buffer
    @param toplevel Toplevel of metadata structure
    @param size Size in bytes of memory pool buffer
*/
ERRG_codeE METADATA_target_serialize(MEM_POOLG_bufDescT *metadata_buffDescListP, INU_Metadata_T* toplevel, UINT32 size);
/*  @brief INU_Metadata Serializer for fixed location metadata that performs cache invalidation and accepts a mempool buffer
    @param metadata_buffDescListP Pointer to mempool buffer
    @param toplevel Toplevel of metadata structure
    @param size Size in bytes of memory pool buffer
*/
ERRG_codeE METADATA_target_serializeFixedLocationDataOnly(MEM_POOLG_bufDescT *metadata_buffDescListP, INU_Metadata_T* toplevel, UINT32 size);
#ifdef __cplusplus
}
#endif
