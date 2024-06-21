/*-----------------------------------------------------------------------------
--
--       This software is confidential and proprietary and may be used
--        only as expressly authorized by a licensing agreement from
--
--                            Verisilicon.
--
--                   (C) COPYRIGHT 2014 VERISILICON
--                            ALL RIGHTS RESERVED
--
--                 The entire notice above must be reproduced
--                  on all copies and should not be removed.
--
-------------------------------------------------------------------------------
--
--  Abstract : The FLEXA memory API
--
-----------------------------------------------------------------------------*/

/*
 * flexamem.h
 *
 * @brief The FLEXA memory API.
 *
 * This file contains the "FLEXA memory API".
 */

#ifndef FLEXA_MEM_H_
#define FLEXA_MEM_H_

#include "flexatype.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLEXA_ALLOCATOR_ID(ch3, ch2, ch1, ch0) \
                          ((u32)(u8)(ch0) | ((u32)(u8)(ch1) << 8) | \
                          ((u32)(u8)(ch2) << 16) | ((u32)(u8)(ch3) << 24))

#define FLEXA_MEM_IMAGE_PLANE       3

#define FLEXA_MEM_COMPRESSED        (1 << 0)
#define FLEXA_MEM_SEGMENTED         (1 << 1)

#define FLEXA_MIF_VMAPPED           (1 << 0)

typedef struct _FLEXA_Plane{
    u32 offset;                                 /* Plane buffer offset from the memory starting address */
    u32 stride;                                 /* Plane pitch in bytes */
    u32 width;                                  /* Plane width in pixels */
    u32 height;                                 /* Plane height in pixels */
    u32 segmentHeight;                          /* Plane segment height in bytes */
    u32 tileFormat;                             /* Plane tile format */
    u32 compressFormat;                         /* Plane compress format */
    u32 tsoffset;                               /* Plane tile status buffer offset  from this plane buffer starting address. */
    int tsh;                                    /* Plane tile status buffer handle. Valid only when tsoffset is 0 */
    void *tsptr;                                /* Plane tile status buffer address */
} FLEXA_Plane;


/**
 * @brief FLEXA memory object
 *
 * This type is the FLEXA memory object which represents a set of
 * DMA buffers, and contain all of related info.
 */
typedef struct _FLEXA_Memory {
    int dev;
    int h;

    u32 size;
    u32 format;

    /** flags of this memory object: FLEXA_MEM_COMPRESSED */
    u32 flags;

    u64 dmaAddr;
    void *ptr;

    /** total segment amount in this memory object */
    u32 segmentCount;

    FLEXA_Plane plane[FLEXA_MEM_IMAGE_PLANE];
} FLEXA_Memory;

/**
 * @brief FLEXA memory info object.
 *
 * This type is the FLEXA memory info object which specifies the info
 * used to allocate a DMA buffer,
 */
typedef struct _FLEXA_MemInfo {
    u32 allocator_id;
    int h;
    u32 size;
    u32 format;

    /** flags of this memory object: FLEXA_MEM_COMPRESSED */
    u32 flags;

    /** total segment amount in this memory object */
    u32 segmentCount;

    u64 dmaAddr;
    void *ptr;

    FLEXA_Plane plane[FLEXA_MEM_IMAGE_PLANE];
} FLEXA_MemInfo;

/**
 * Initialize the module of FLEXA Memory.
 *
 * @return 0: the invocation of the function is successful.
 * @return negative integer: the invocation of the function is failed.
 *
 */
int FLEXA_MemInit(void);

/**
 * Exit the module of FLEXA Memory.
 *
 */
void FLEXA_MemExit(void);

/**
 * Allocate and export the buffers for DMA.
 *
 * @param meminfo [in] a memory info descriptor to speciy buffers to be allocated.
 * @return A handle to a FLEXA_Memory: the invocation of the function is successful.
 * @return -1: the invocation of the function is failed.
 *
 */
int FLEXA_MemExport(FLEXA_MemInfo *meminfo);

/**
 * Import a h associated with the buffers for DMA.
 *
 * @param h [in] a file descriptor associated with the memory is imported.
 * @param dev [in] a handle of a device for which the memory is imported.
 * @param flags [in] flags' combination includes FLEXA_MIF_VMAPPED.
 * @return A pointer to FLEXA_Memory: the invocation of the function is successful.
 * @return NULL: the invocation of the function is failed.
 *
 */

FLEXA_Memory* FLEXA_MemImport(int h, int dev, u32 flags);

/**
 * Map a handle associated with the buffers for user space access.
 *
 * @param memobj [in] a FLEXA memory object pointer.
 * @param offset [in] the offset of the memory associated with handle.
 * @param length [in] the length of the memory associated with handle.
 * @param flags [in] flags of the underline memory mapping.
 * @return the virtual address of the memory: the invocation of the function is successful.
 * @return NULL: the invocation of the function is failed.
 *
 */
void* FLEXA_MemMap(FLEXA_Memory *memobj, u32 offset, u32 length, u64 flags);

/**
 * Unmap a memobj already mapped.
 *
 * @param memobj [in] the memory object to be unmapped.
 * @param addr [in] the virtual address of the memory object.
 * @param length [in] the length of the memory range to be unmapped.
 * @return FLEXA_STATUS_SUCCESS: the invocation of the function is successful.
 * @return Other values: the invocation of the function is failed.
 *
 */
int FLEXA_MemUnmap(FLEXA_Memory *memobj, void *addr, u32 length);

/**
 * Sync a memobj to ensure data coherence.
 *
 * @param memobj [in] the memory object to be synchronized.
 * @return FLEXA_STATUS_SUCCESS: the invocation of the function is successful.
 * @return Other values: the invocation of the function is failed.
 *
 */
int FLEXA_MemSync(FLEXA_Memory *memobj, u64 flags);

/**
 * Release a specified FLEXA memory object imported.
 *
 * @param memobj [in] a pointer to the FLEXA memory object.
 * @return FLEXA_STATUS_SUCCESS: the invocation of the function is successful.
 * @return Other values: the invocation of the function is failed.
 *
 */
int FLEXA_MemRelease(FLEXA_Memory *memobj);

/**
 * Close a specified handle associated with the memory exported.
 *
 * @param h [in] a handle associated with the memory exported.
 * @return FLEXA_STATUS_SUCCESS: the invocation of the function is successful.
 * @return Other values: the invocation of the function is failed.
 *
 */
int FLEXA_MemClose(int h);

#ifdef __cplusplus
}
#endif

#endif /* FLEXA_MEM_H_ */

