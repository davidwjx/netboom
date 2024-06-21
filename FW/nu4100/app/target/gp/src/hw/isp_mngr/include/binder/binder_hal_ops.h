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
 * @file binder_hal_ops.h
 *
 * @brief   binder Layer
 *
 *          Encapsulates and abstracts services from different hardware platforms.
 *
 *****************************************************************************/

#ifndef __BINDER_HAL_OPS_H__
#define __BINDER_HAL_OPS_H__

#include <ebase/types.h>
#include <ebase/dct_assert.h>
//#include <ebase/trace.h>

#include <common/return_codes.h>
#include <oslayer/oslayer.h>

/******************************************************************************
 * inline implementations of API
 *****************************************************************************/
 #include <hal/hal_common.h>

typedef enum BindOpsType_e {
	BIND_HAL_OPS_GENERAL = 0,
	BIND_HAL_OPS_TILE_MODE,		// sw calculate and split the input large image to two right half and left half image by mcm reading.
	BIND_HAL_OPS_TILE_MODE_MP_CTRL, // hw mp_ctrl module will split the input large image to two right half and left half image.
	BIND_HAL_OPS_TYPE_MAX
} BindOpsType;

// #ifdef __cplusplus
// extern "C"
// {
// #endif

typedef struct HalIrqCtx_s BinderIrqCtx_t;

typedef struct BinderHalApiOps_s {
/*****************************************************************************/
/**
 * @brief   open the low level hal
 * @return  handle of low level context; NULL on failure
 *
 *****************************************************************************/
void *(*open)(
    int ispId,
    BindOpsType BindOpsType,
    uint32_t workmode);

/*****************************************************************************/
/**
 * @brief   close the low level hal
 * @param   Handle   Handle to session as returned by @ref Open.
 * @return  Result of operation.
 *
 *****************************************************************************/
RESULT (*close)
(
    void* Handle
);

/*****************************************************************************/
/**
 * @brief   writes a value to the given address
 * @param   Handle   Handle to binder session as returned by @ref Open.
 * @param   reg_address Address of register to write.
 * @param   value       Value to write into register.
 * @return  none
 *
 * @note    It is required to pass in the full address (base address + offset).
 *****************************************************************************/
void (*writeRegister)
(
    void* Handle,
    uint32_t addr,
    uint32_t val
);


/*****************************************************************************/
/**
 * @brief   reads a value from a given address
 * @param   Handle   Handle to Binder session as returned by @ref Open.
 * @param   reg_address Address of register to read.
 * @return  Register value.
 *
 * @note    It is required to pass in the full address (base address + offset).
 *****************************************************************************/
uint32_t (*readRegister)
(
    void* Handle,
    uint32_t addr
);

/*****************************************************************************/
/**
 * @brief   
 * @param   Handle   Handle to Binder session as returned by @ref Open.
 * @param   reg_address Address of register to read.
 * @return  result   RET_NULL_POINTER RET_SUCCESS
 *
 * @note    It is required to pass in the full address (base address + offset).
 *****************************************************************************/
RESULT (*ioctl)
(
    void* Handle,
    HalModule_e module,
    unsigned int cmd, 
    void *args
);
/*****************************************************************************/
/**
 *          connectIrq()
 *
 *  @brief  Register interrupt service routine with system software.
 *
 *  @param  Handle to binder session as returned by @ref Open
 *  @param  Reference of Binder irq context structure that represent this connection
 *  @param  Interrupt object @ref osInterrupt
 *  @param  Number of the interrupt source, set to 0 if not needed
 *  @param  First interrupt routine, (first level handler) set to NULL if not needed
 *  @param  Second interrupt routine (second level handler)
 *  @param  Context provided when the interrupt routines are called
 *
 *  @warning Add platform specific code to connect to your local interrupt source
 *
 *  @return                     Status of operation
 *  @retval RET_SUCCESS         ISR registered successfully
 *  @retval RET_FAILURE et al.  ISR not registered
 *
 *****************************************************************************/
RESULT (*connectIrq)
(
    void* Handle,
    BinderIrqCtx_t *pIrqCtx,
    uint32_t    int_src,
    osIsrFunc   IsrFunction,
    osDpcFunc   DpcFunction,
    void*       pContext
);

/*****************************************************************************/
/**
 *          disconnectIrq()
 *
 *  @brief  Deregister interrupt service routine from system software.
 *  @param  Handle to binder session as returned by @ref Open
 *  @param  Reference of binder irq context structure that represent this connection
 *
 *  @return                     Status of operation
 *  @retval RET_SUCCESS         ISR deregistered successfully
 *  @retval RET_FAILURE et al.  ISR not properly deregistered
 *
 *****************************************************************************/
RESULT (*disconnectIrq)
(
    void* Handle,
    BinderIrqCtx_t *pIrqCtx
);

/*****************************************************************************/
/**
 *          getHalHandler()
 *
 *  @brief get hal level handler.
 *  @param  Handle to binder session as returned by @ref Open
 *
 *  @return      hanlHandler. NULL on failure
 *
 *****************************************************************************/
HalHandle_t (*getHalHandler)
(
    void* Handle
);

/*****************************************************************************/
/**
 * @brief   tell HAL about another user of low level driver
 *  @param  Handle to binder session as returned by @ref Open
 * @return  Result of operation.
 *
 * @note
 *****************************************************************************/
RESULT (*addRef)(
    void* Handle
);
/*****************************************************************************/
/**
 * @brief   tell HAL about gone user of low level driver
 * @param  Handle to binder session as returned by @ref Open
 * @return  Result of operation.
 *
 * @note    If the internal ref count is zero, the HAL will be closed as well.
 *****************************************************************************/
RESULT (*delRef)(
    void* Handle
);

/*****************************************************************************/
/**
 *          *eventGenerator()
 *
 *  @brief  Cmodel specific interface: Hal ISR and Event generator
 *
 *  @param
 *
 *  @return                     Status of operation
 *  @retval RET_SUCCESS         Input set successfully
 *  @retval RET_FAILURE et al.  Input set fails
 *
 *****************************************************************************/
RESULT (*eventGenerator)
(
    HalIrqCtx_t *pIrqCtx,
    HalIsrSrc_t isr_src,
    uint32_t misValue
);

/*****************************************************************************/
/**
 * @brief   set the ic dev pointer member.It is used for cmodel.
 * @param  Handle to binder session as returned by @ref Open
 * @param  pIcdev  pointer to ic_dev object.
 * @return  Result of operation.
 *
 * @note    If the internal ref count is zero, the HAL will be closed as well.
 *****************************************************************************/
RESULT (*setIcdev)(
    void* Handle,
    void* pIcdev
);

#ifdef HAL_CMODEL
/*****************************************************************************/
/**
 * @brief   cmodel Buffer transfer handler
 * @param  Handle to binder session as returned by @ref Open
 * @param  trans_ctrl contrl value.
 * @return  Result of operation.
 *
 * @note    If the internal ref count is zero, the HAL will be closed as well.
 *****************************************************************************/
RESULT (*bufTransfer)(
    void* Handle,
     uint32_t trans_ctrl
);

/*****************************************************************************/
/**
 * @brief   Cmodel Shd register update process
 * @param  Handle to binder session as returned by @ref Open
 * @return  Result of operation.
 *
 * @note    If the internal ref count is zero, the HAL will be closed as well.
 *****************************************************************************/
RESULT (*shdRegisterUpdate)(
    void* Handle
);

/*****************************************************************************/
/**
 * @brief   cfg dma buffer addr ,format and size
 * @param  Handle to binder session as returned by @ref Open
 * @return  Result of operation.
 *
 * @note    If the internal ref count is zero, the HAL will be closed as well.
 *****************************************************************************/
RESULT (*cfgDma)(
    void* Handle,
    HalCmodelBufConfig_t *pPicBuffer
);

/*****************************************************************************/
/**
 * @brief   cfg mi buffer addr ,format and size
 * @param  Handle to binder session as returned by @ref Open
 * @return  Result of operation.
 *
 * @note    If the internal ref count is zero, the HAL will be closed as well.
 *****************************************************************************/
RESULT (*cfgMi)(
    void* Handle,
     uint32_t path,
    HalCmodelBufConfig_t *pPicBuffer
);

/*****************************************************************************/
/**
 * @brief   run cmodel piplien to process
 * @param  Handle to binder session as returned by @ref Open
 * @return  Result of operation.
 *
 * @note    If the internal ref count is zero, the HAL will be closed as well.
 *****************************************************************************/
RESULT (*ispPipeline)(
    void* Handle
);
#endif
} BinderHalApiOps_t;


// #ifdef __cplusplus
// extern "C"
// }
// #endif
#endif