/******************************************************************************\
|* Copyright (c) <2010> by Silicon Image, Inc., used with permission by       *|
|* VeriSilicon.                                                               *|
|* Copyright (c) <2020> by VeriSilicon Holdings Co., Ltd. ("VeriSilicon")     *|
|* All Rights Reserved.                                                       *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of VeriSilicon.  This is proprietary information owned or licensed by      *|
|* VeriSilicon.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of VeriSilicon.                                         *|
|*                                                                            *|
\******************************************************************************/

/* VeriSilicon 2020 */

/**
 * @file media_buffer_pool.h
 *
 * @brief
 *          Media Buffer Pool interface
 *
 * <pre>
 *
 *   Principal Author: Joerg Detert
 *   Creation date:    Feb 28, 2008
 *
 * </pre>
 *
 *****************************************************************************/
/**
 * @mainpage Welcome to the MediaBufferPool Interface Documentation
 *
 * Doc-Id: xx-xxx-xxx-xxx (MediaBufferPool Implementation Spec)\n
 * Author: Klaus Kaiser
 *
 * @image html silicon_image_logo.gif
 * \n
 *
 *  This buffer service is the central instance for all multimedia modules, which
 *  transmit data through buffers. All buffers are grouped into buffer pools.
 *  Each buffer pool contains one or multiple buffers with the same size. The buffer
 *  service usually manages buffers with uncompressed data (e.g. YUV, RGB, PCM) or
 *  compressed data (Audio-PES, Video-PES) but could be used for any type of data.
 *  Beside the plain data each buffer holds sideband information, e.g. timestamp
 *  and frame resolution. The following picture shows the buffer pool structure.
 *
 * @image html MediaBufferPool.png
 * \n
 *
 * Allocation and management of memory is done through those Media Buffer Pools.
 * The Buffer Pool is created with a bounded size of a contiguous physical memory block,
 * separated into equally sized smaller blocks, the so called Media Buffer. Furthermore
 * it is possible during initialization to specify a random access or ring buffer like
 * access pattern, supporting different types of applications. For example hardware blocks,
 * like decoders, which accesses content in memory through a ring buffer scheme must be
 * supported. The Media Buffer Pool interface provides functionality to request empty
 * buffers, to put or release filled buffers and to request filled buffers. This is the
 * core functionality made available to connected Media Modules.
 *
 *
 * - @subpage media_buffer_queue
 *
 */

#ifndef MEDIA_BUFFER_POOL_H
#define MEDIA_BUFFER_POOL_H


#if defined (__cplusplus)
extern "C" {
#endif

#include <ebase/types.h>
#include <ebase/dct_assert.h>
#include <scmi/scmi_types.h>

#include "hal/hal_common.h"
#include "media_buffer.h"
#include <cam_device/cam_device_buf_defs.h>


/**
 * Definition of Media Buffer Pool Flags.
 */

/** Buffer pool acts like a ring buffer, that means no random access */
#define BUFPOOL_RINGBUFFER    0x00000001U


/**
 * @brief   By now 3 users can register a callback.
 *          These should be 1 consumer, 1 producer and 1 control instance.
 */
#define MAX_NUM_REGISTERED_CB   3

/**
 * @brief   Parameter id, used when calling @ref MediaBufPoolSetParameter.
 */
typedef int32_t    MediaBufPoolParamId_t;


/**
 * @brief   Used as parameter for @ref MediaPoolNotifyCb
 */
typedef int32_t    MediaBufPoolEvent_t;


/**
 * @brief   Enumerations to indicate which parameter to set
 */
enum MediaBufPoolParameter_e
{
    PARAM_HIGH_WATERMARK  = 0x0000,
    PARAM_LOW_WATERMARK   = 0x0001,
    PARAM_LAST            = 0x0002  /**< Do not use! */
};


/**
 * @brief   Enumerations to indicate which callback to call
 */
enum MediaBufPoolEvent_e
{
    HIGH_WATERMARK_CRITICAL_REGION_ENTERED  = 0x0000,
    HIGH_WATERMARK_CRITICAL_REGION_LEFT     = 0x0001,
    LOW_WATERMARK_CRITICAL_REGION_ENTERED   = 0x0002,
    LOW_WATERMARK_CRITICAL_REGION_LEFT      = 0x0003,
    FULL_BUFFER_ADDED                       = 0x0004,
    EMPTY_BUFFER_ADDED                      = 0x0005,
    FULL_BUFFER_REMOVED                     = 0x0006    /**< Extended queues with external buffer pool issue this callback event. */
};


/**
 * @brief   Callback function that can be registered to receive
 *          Media Buffer Pool events.
 */
typedef void (*MediaBufPoolCbNotify_t)
(
    int32_t              event,
    void*                pUserContext,
    const MediaBuffer_t* pMediaBuffer    //!< Just for information purposes, may be used to handle chained ancillary buffers.
);


/**
 * @brief Structure holding all information to notify a registered user
 */
typedef struct MediaBufPoolNotify_s
{
    MediaBufPoolCbNotify_t fpCallback;      /**< Notification callback */
    void*                  pUserContext;    /**< Pointer to user context to pass to callback */
} MediaBufPoolNotify_t;


/**
 * @brief MediaBufPool pool object, used to maintain a number of
 *        MediaBuffer_t elements. Each MediaBuffer_t can be automatically
 *        associated to a memory chunk. The pool has to be initialized via
 *        MediaBufPoolCreate() and released via MediaBufPoolDestroy().
 */
typedef struct MediaBufPool_s
{
    void*                pBaseAddress;          /**< Base address of buffer pool. Note that */
                                                /**< a buffer pool is a contingous chunk of */
                                                /**< memory. */
    void*                pMetaDataMediaBufBase; /**< Base address of meta data buffer pool for Media Buffers.
                                                     These meta data structures are linked to the Media Buffers. */
    void*                pMetaDataScmiBufBase;  /**< Base address of meta data buffer pool for Scmi Buffers.
                                                     These meta data structures are linked to the Scmi Buffers
                                                     contained in the Media Buffers. */
    uint32_t             bufSize;               /**< Size of each buffer in bytes. */
    uint32_t             metaDataSizeMediaBuf;  /**< Size of meta data associated with media buffer. */
    uint32_t             metaDataSizeScmiBuf;   /**< Size of meta data associated with scmi buffer contained in media buffer. */

    uint16_t             bufNum;                /**< Number of buffers in buffer pool. */
    uint16_t             maxBufNum;
    uint32_t             poolSize;              /**< Size of complete buffer pool in bytes. */
    MediaBuffer_t*       pBufArray;             /**< Array used to manage each buffer in the pool */
    uint32_t             flags;                 /**< Buffer Pool flags. */
    uint16_t             freeBufNum;            /**< Resources count: Number of free buffers */
    uint32_t             fillLevel;             /**< How many buffers are filled with data */
    uint16_t             highWatermark;         /**< if value is reached high watermark callback is triggered */
    uint16_t             lowWatermark;          /**< if value is reached low watermark callback is triggered */
    uint32_t             index;                 /**< Pointer to current index in buffer array (internal use) */
    MediaBufPoolNotify_t notify[MAX_NUM_REGISTERED_CB];   /**< Array with info about users registered for notification */
} MediaBufPool_t;


/**
 * @brief The MediaBufPoolConfig is used to calculate the amount of memory needed to use
 *        pool. The call to @ref MediaBufPoolGetSize will fill the sizes of the needed memory.
 *        The user is then responsible to allocate the memory and pass the structure to the
 *        @ref MediaBufPoolCreate function.
 *
 */
typedef struct MediaBufPoolConfig_s
{
    /* Input parameters */
    uint32_t    bufSize;               /**< Size of memory chunk a buffer represents */
    uint32_t    metaDataSizeMediaBuf;  /**< Size of extra meta data strutucture attached to each Media buffer (pointed to by MediaBuffer_t.pMetaData) */
    uint32_t    metaDataSizeScmiBuf;   /**< Size of extra meta data strutucture attached to each Scmi buffer  (pointed to by MediaBuffer_t.buf.p_meta_data) */
    uint32_t    flags;                 /**< Configuration flags */
    uint16_t    bufNum;                /**< Initial number of buffers */
    uint16_t    bufAlign;              /**< Needed alignment of each memory chunk the buffers represent */
    uint16_t    maxBufNum;             /**< Maximum number of buffers */

    /* Output parameters */
    uint32_t    metaDataMemSize; /**< Size of memory for buffer structures plus extra meta data.
                                      This needs to be directly accesable by CPU. */
    uint32_t	bufMemSize;      /**< Size of complete data memory which is represented by the buffers. */
#ifdef ISP_DEC
    uint32_t   tileMemSize;      /**< Size of dec tile status memory which is represented by the buffers. */
#endif
} MediaBufPoolConfig_t;


/**
 * @brief MediaBufPoolMemory is used to pass memory to the Media Buffer Pool.
 */
typedef struct MediaBufPoolMemory_s
{
    void*	    pMetaDataMemory; /**< Memory for administrative and extra meta data structures (for MediaBuffer and/or ScmiBuffer),
                                    must be of size MediaBufPoolConfig_t.metaDataMemSize as reported by MediaBufPoolGetSize. */
    uint32_t	pBufferMemory;   /**< Actual memory where the data is going to be placed,
                                    must be of size MediaBufPoolConfig_t.bufMemSize as reported by MediaBufPoolGetSize.
                                    Furthermore, must be aligned as specified by MediaBufPoolConfig_t.bufAlign. */
    BufIdentity    *    pHalBufList;
    uint32_t            total_frame;
    BUFF_MODE           buf_work_mode;
} MediaBufPoolMemory_t;


/**
 * @brief   Get a free buffer from the MediaBufferPool.
 *
 * @param   pBufPool Pointer to the MediaBufferPool.
 *
 *****************************************************************************/
MediaBuffer_t* MediaBufPoolGetBuffer(MediaBufPool_t* pBufPool);


/**
 * @brief   Inform MediaBufferPool that buffer has been filled.
 *
 * @param   pBuf  Full buffer to add.
 *
 *****************************************************************************/
void MediaBufPoolBufferFilled(MediaBufPool_t* pBufPool, MediaBuffer_t* pBuf);


/**
 * @brief   Free a buffer from the bufferpool.
 *
 * @param   pBuf    Buffer to free
 *
 *****************************************************************************/
void MediaBufPoolFreeBuffer(MediaBufPool_t* pBufPool, MediaBuffer_t* pBuf);


/**
 * @brief   Calculate the size of the memory the MediaBufferPool needs
 *          for the metadata the internal structure and the array of buffers.
 *          The caller has to assure to fill the structure with the corresponding
 *          information need. He has to fill bufSize, metaDataSize, bufNum,
 *          bufAlign and maxBufNum. With these information the function can
 *          calculate the amount of memory need.
 *          When the function returns the values for bufPoolMemSize and
 *          bufMemSize are filled.
 *
 * @param   pPoolConfig     Configuration parameters for the media buffer pool.
 *
 * @return  Status of operation.
 *****************************************************************************/
ScmiReturnType MediaBufPoolGetSize(MediaBufPoolConfig_t* pPoolConfig);


/**
 * @brief   Create a buffer pool.
 *
 * @param   pBufPool        Pointer to buffer pool object to be created.
 * @param   pPoolConfig     Pointer to config params for buffer pool.
 * @param   poolMemory      Pool memory information.
 *
 * @return  Status of operation.
 *****************************************************************************/
extern ScmiReturnType MediaBufPoolCreate(MediaBufPool_t*         pBufPool,
                                         MediaBufPoolConfig_t*   pPoolConfig,
                                         MediaBufPoolMemory_t    poolMemory);

/**
 * @brief   Create a Flexa buffer pool.
 *
 * @param   pBufPool        Pointer to Flexa buffer pool object to be created.
 * @param   pPoolConfig     Pointer to config params for Flexa buffer pool.
 * @param   poolMemory      Pool memory information.
 * @param   hHal            Hal instance handle.
 * @param   fds[]           File descriptor.
 *
 * @return  Status of operation.
 *****************************************************************************/
extern ScmiReturnType MediaBufPoolCreateFlexa(MediaBufPool_t*         pBufPool,
                                         MediaBufPoolConfig_t*   pPoolConfig,
                                         MediaBufPoolMemory_t    poolMemory,
                                         HalHandle_t hHal, int fds[]);


/*****************************************************************************/
/**
 * @brief   Destroy a buffer pool.
 *
 * @param   pBufPool        Pointer to buffer pool object to be destroyed.
 *
 * @return  Status of operation.
 *****************************************************************************/
extern ScmiReturnType MediaBufPoolDestroy(MediaBufPool_t* pBufPool);

/*****************************************************************************/
/**
 * @brief   Destroy a Flexa buffer pool.
 *
 * @param   pBufPool        Pointer to Flexa buffer pool object to be destroyed.
 *
 * @return  Status of operation.
 *****************************************************************************/
extern ScmiReturnType MediaBufPoolDestroyFlexa(MediaBufPool_t* pBufPool);

/*****************************************************************************/
/**
 * @brief   Reset a buffer pool.
 *
 * @param   pBufPool        Pointer to buffer pool object to be reset.
 *
 * @return  Status of operation.
 *****************************************************************************/
extern ScmiReturnType MediaBufPoolReset(MediaBufPool_t* pBufPool);


/* ***************************************************************************/
/**
 * @brief   Register a callback to get informed about MediaBufPool status.
 *
 * @param   pBufPool        Pointer to buffer pool object.
 * @param   fpCallback      Function to be triggered in case of event.
 * @param   pUserContext    Pointer passed in each call of the callback.
 *
 * @return  Status of operation.
 *****************************************************************************/
extern ScmiReturnType MediaBufPoolRegisterCb(MediaBufPool_t*        pBufPool,
                                             MediaBufPoolCbNotify_t fpCallback,
                                             void*                  pUserContext);


/****************************************************************************/
/**
 * @brief   Deregister a callback from MediaBufPool
 *
 * @param   pBufPool    Pointer to buffer pool object.
 * @param   fpCallback  Function to be deregistered.
 *
 * @return  Status of operation.
 *****************************************************************************/
extern ScmiReturnType MediaBufPoolDeregisterCb(MediaBufPool_t*        pBufPool,
                                               MediaBufPoolCbNotify_t fpCallback);


/****************************************************************************/
/**
 * @brief   Set parameter of MediaBufferPool.
 *
 * @param   pBufPool    Pointer to buffer pool object.
 * @param   paramId     Id of parameter to be set.
 *
 * @return  Status of operation.
 *****************************************************************************/
extern ScmiReturnType MediaBufPoolSetParameter(MediaBufPool_t*       pBufPool,
                                               MediaBufPoolParamId_t paramId,
                                               int32_t               paramValue);


/****************************************************************************/
/**
 * @brief   Get parameter of MediaBufferPool.
 *
 * @param   pBufPool    Pointer to buffer pool object.
 * @param   paramId     Id of parameter to get.
 * @param   pParamValue Address to receive parameter value.
 *
 * @return  Status of operation.
 *****************************************************************************/
extern ScmiReturnType MediaBufPoolGetParameter(MediaBufPool_t*       pBufPool,
                                               MediaBufPoolParamId_t paramId,
                                               int32_t*              pParamValue);


#if defined (__cplusplus)
}
#endif


#endif  /* MEDIA_BUFFER_POOL_H*/

