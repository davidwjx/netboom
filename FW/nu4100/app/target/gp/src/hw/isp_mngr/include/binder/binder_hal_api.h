/******************************************************************************\
|* All Rights Reserved.                                                       *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of VeriSilicon.  This is proprietary information owned or licensed by      *|
|* VeriSilicon.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of VeriSilicon.                                         *|
|*                                                                            *|
\******************************************************************************/

/* VeriSilicon 2021 */

/**
 * @file binder_hal_api.h
 *
 * @brief   binder api *
 *          binder with abstracts services from different hardware platforms.
 *
 *****************************************************************************/

#ifndef __BINDER_HAL_API_H__
#define __BINDER_HAL_API_H__


#include<hal/hal_api.h>
#include<hal/hal_common.h>
#include "binder_hal_ops.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct BinderCtx_s {
    uint32_t ispId;
    BinderHalApiOps_t* pHalApiOps;    //pointer to the hal api ops.
    void*   pPrivateCtx;               //pointer to the lower level context.It determines by the lower level.
    HalHandle_t*   hHal;
    BindOpsType    binderMode;        //indictate this is general or tile mode. 
}BinderCtx_t;
typedef void* BinderHandle_t;


/*****************************************************************************/
/**
 * @brief   call the bindered low level opem api.
 * @return  lower level context pointer. NULL on failure.
 *
 *****************************************************************************/
BinderHandle_t BinderHalOpen
(
    uint32_t ispId,
    BindOpsType BindOpsType,
    uint32_t workmode
);

/*****************************************************************************/
/**
 * @brief   call the bindered low level close api.
 * @return  RESULT of operation.
 *
 *****************************************************************************/
RESULT BinderHalClose
(
    BinderHandle_t binderHandle
);


/*****************************************************************************/
/**
 * @brief   call the bindered low level write register api to write register.
 * @return  RESULT of operation.
 *
 *****************************************************************************/
RESULT BinderHalWriteReg
(
    BinderHandle_t binderHandle,
    uint32_t addr,
    uint32_t val
);


/*****************************************************************************/
/**
 * @brief   call the bindered low level read register api to read register.
 * @return  register value
 *
 *****************************************************************************/
uint32_t BinderHalReadReg
(
    BinderHandle_t binderHandle,
    uint32_t addr
);

/*****************************************************************************/
/**
 * @brief   ioctl.
 * @return  RET_SUCCES
 *
 *****************************************************************************/
RESULT BinderHalIoctl
(
    BinderHandle_t binderHandle,
    HalModule_e module,
    unsigned int cmd, 
    void *args
);

/*****************************************************************************/
/**
 * @brief   call the bindered low level connect irq api to create irq handler 
 * thread.
 * @return  RESULT of operation.
 *
 *****************************************************************************/
RESULT BinderHalConnectIrq
(
    BinderHandle_t binderHandle,
    BinderIrqCtx_t *pIrqCtx,
    uint32_t    int_src,
    osIsrFunc   IsrFunction,
    osDpcFunc   DpcFunction,
    void*       pContext
);

/*****************************************************************************/
/**
 * @brief   call the bindered low level disconnect irq api to destroy irq handler 
 * thread.
 * @return  RESULT of operation.
 *
 *****************************************************************************/
RESULT BinderHalDisconnectIrq
(
    BinderHandle_t binderHandle,
    BinderIrqCtx_t *pIrqCtx
);

/*****************************************************************************/
/**
 * @brief   call the bindered low level disconnect irq api to destroy irq handler 
 * thread.
 * @return  RESULT of operation.
 *
 *****************************************************************************/
RESULT BinderHalDisConnect
(
    BinderHandle_t binderHandle,
    BinderIrqCtx_t *pIrqCtx
);


/*****************************************************************************/
/**
 * @brief   call the bindered low level 
 *           Cmodel specific interface: Hal ISR and Event generator
 * thread.
 * @return  RESULT of operation.
 *
 *****************************************************************************/
RESULT BinderHalEventGenerator
(
    BinderHandle_t binderHandle,
    HalIrqCtx_t *pIrqCtx,
    HalIsrSrc_t isr_src,
    uint32_t misValue
);
/*****************************************************************************/
/**
 * @brief   allocates the given amount of hardware memory
 * @param   binderHandle       Handle to HAL session as returned by @ref BinderHalOpen.
 * @param   byte_size       Amount of memory to allocate.
 * @return  mem_address     Memory block start address in hardware memory; 0 on failure.
 *
 * @note    Chunks of n*4K byte with addresses aligned to 4K are used internally.
 *          The allocator behind the scenes may be very simple, but still will recombine
 *          free'd adjacent blocks. It may perform reasonably well only for at most a
 *          few dozen allocs active at any time. Allocating/freeing video/audio buffers in
 *          realtime at framerate should nevertheless be posssible without noticeable
 *          performance penalties then.
 *****************************************************************************/
uint32_t BinderHalAllocMemory(
     BinderHandle_t binderHandle,
     uint32_t byte_size
);


/*****************************************************************************/
/**
 * @brief   frees the given block of hardware memory
 * @param   binderHandle       Handle to HAL session as returned by @ref BinderHalOpen.
 * @param   mem_address     Start address of memory block in hardware memory.
 * @return  Result of operation.
 *
 * @note    Chunks of n*4K byte with addresses aligned to 4K are used internally.
 *****************************************************************************/
RESULT BinderHalFreeMemory
( 
    BinderHandle_t binderHandle,
    uint32_t mem_address 
);


/*****************************************************************************/
/**
 * @brief   reads a number of data from the memory to a buffer starting a the given address
 * @param   binderHandle       Handle to HAL session as returned by @ref BinderHalOpen.
 * @param   mem_address     Source start address in hardware memory.
 * @param   p_read_buffer   Pointer to local memory holding the data being read.
 * @param   byte_size       Amount of data to read.
 * @return  Result of operation.
 *
 * @note    Certain implementation dependent limitations regarding alignment of
 *          both addresses and transfer size exist!
 *****************************************************************************/
RESULT BinderHalReadMemory
(
    BinderHandle_t binderHandle,
    uint32_t mem_address,
    uint8_t* p_read_buffer,
    uint32_t byte_size
);

/*****************************************************************************/
/**
 * @brief   writes a number of data from a buffer to the memory starting a the given address
 * @param   binderHandle       Handle to HAL session as returned by @ref BinderHalOpen.
 * @param   mem_address     Target start address in hardware memory.
 * @param   p_write_buffer  Pointer to local memory holding the data to be written.
 *                          Undefined on failure.
 * @param   byte_size       Amount of data to write.
 * @return  Result of operation.
 *
 * @note    Certain implementation dependent limitations regarding alignment of
 *          both addresses and transfer size exist!
 *****************************************************************************/
RESULT BinderHalWriteMemory
(
     BinderHandle_t binderHandle,
    uint32_t mem_address,
    uint8_t* p_write_buffer,
    uint32_t byte_size
);


/*****************************************************************************/
/**
 * @brief   maps a number of data from the memory into local memory starting at the given address
 * @param   binderHandle       Handle to HAL session as returned by @ref BinderHalOpen.
 * @param   mem_address     Source start address in hardware memory.
 * @param   byte_size       Amount of data to map.
 * @param   mapping_type    The way the mapping is performed.
 * @param   pp_mapped_buf   Reference to pointer to the mapped local memory.
 * @return  Result of operation.
 *
 * @note    Certain implementation dependent limitations regarding alignment of
 *          both addresses and transfer size exist!
 *****************************************************************************/
RESULT BinderHalMapMemory
(
    BinderHandle_t binderHandle,
    uint32_t mem_address,
    uint32_t byte_size,
    HalMapMemType_t mapping_type,
    void **pp_mapped_buf
);


/*****************************************************************************/
/**
 * @brief   unmaps previously mapped memory from local memory
 * @param   binderHandle       Handle to HAL session as returned by @ref BinderHalOpen.
 * @param   p_mapped_buf    Pointer to local memory to unmap as returned by
 *                          @ref BinderHalMapMemory().
 * @return  Result of operation.
 *
 * @note    Certain implementation dependent limitations regarding alignment of
 *          both addresses and transfer size exist!
 *****************************************************************************/
RESULT BinderHalUnMapMemory(
     BinderHandle_t binderHandle,
     void* p_mapped_buf
);

RESULT BinderHalAddRef(
     BinderHandle_t binderHandle
);

RESULT BinderHalDelRef(
     BinderHandle_t binderHandle
);

HalHandle_t BinderHalGetHalHandler(
     BinderHandle_t binderHandle
);

RESULT BinderHalGetBinderMode(
     BinderHandle_t binderHandle,
     BindOpsType *pBinderMode
);

RESULT BinderHalSetIcdev(
     BinderHandle_t binderHandle,
     void*  pIcdev
);

#ifdef HAL_CMODEL
RESULT BinderHalBufTransfer(
     BinderHandle_t binderHandle,
     uint32_t trans_ctrl
);

RESULT BinderHalShdRegisterUpdate(
     BinderHandle_t binderHandle
);

RESULT BinderHalCfgDma(
     BinderHandle_t binderHandle,
    HalCmodelBufConfig_t* pBufCfg
);

RESULT BinderHalCfgMi(
     BinderHandle_t binderHandle,
     uint32_t path,
    HalCmodelBufConfig_t* pBufCfg
);

RESULT BinderHalIspPipline(
     BinderHandle_t binderHandle
);
#endif

#ifdef __cplusplus
}
#endif
#endif
