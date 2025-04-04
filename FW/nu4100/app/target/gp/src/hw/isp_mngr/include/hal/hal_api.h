/******************************************************************************\
|* Copyright 2010, Dream Chip Technologies GmbH. used with permission by      *|
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
 * @file hal_api.h
 *
 * @brief   Hardware Abstraction Layer
 *
 *          Encapsulates and abstracts services from different hardware platforms.
 *
 *****************************************************************************/

#ifndef __HAL_API_H__
#define __HAL_API_H__

#include <ebase/types.h>
#include <ebase/dct_assert.h>
//#include <ebase/trace.h>

#include <common/return_codes.h>
#include <oslayer/oslayer.h>

/******************************************************************************
 * inline implementations of API
 *****************************************************************************/
 #include "hal_common.h"

#if defined( HAL_ALTERA )
  #include "hal_altera_pci.h"
#elif defined (HAL_CMODEL)
  #include "hal_cmodel.h"
#else
  #error "unknow hardware plattform"
#endif


/**
 * @defgroup HAL_API Hardware Abstraction Layer interface
 * @{
 * @brief  Encapsulates and abstracts services from different hardware platforms
 *         in a hardware platform independent way.
 *****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 * HAL device IDs (defined by underlying layer via included header file)
 *****************************************************************************/
//#define HAL_DEVID_OCP         //!< HAL internal use only
//#define HAL_DEVID_MARVIN      //!< MARVIN.*
//#define HAL_DEVID_VDU         //!< VDU.clk + VDU.ocpclk
//#define HAL_DEVID_PCLK        //!< VDU.pclk
//#define HAL_DEVID_I2C_0       //!< HAL internal use only
//#define HAL_DEVID_I2C_1       //!< HAL internal use only
//#define HAL_DEVID_I2C_2       //!< HAL internal use only
//#define HAL_DEVID_CAMREC_1    //!< CAMREC1, not neccessarily the REC that CAM1 is hooked up to; see @ref HAL_DEVID_CAM_1_REC instead
//#define HAL_DEVID_CAM_1       //!< CAM1
//#define HAL_DEVID_CAMPHY_1    //!< CAMPHY1, not neccessarily the PHY that CAM1 is hooked up to; see @ref HAL_DEVID_CAM_1_PHY instead
//#define HAL_DEVID_ALL         //!< just for convenience; all devices included


/******************************************************************************
 * HAL device base addresses (defined by underlying layer via included header file)
 *****************************************************************************/
// HalRegs:
//#define HAL_BASEADDR_MARVIN   //!< Base address of MARVIN module.
//#define HAL_BASEADDR_MARVIN_2 //!< Base address of MARVIN module of 2nd channel.
//#define HAL_BASEADDR_MIPI_1   //!< Base address of MIPI module.
//#define HAL_BASEADDR_MIPI_2   //!< Base address of MIPI module of 2nd channel.
//#define HAL_BASEADDR_VDU      //!< Base address of VDU module.

// HalSysRegs:
//#define HAL_BASEADDR_SYSCTRL  //!< HAL internal use only
//#define HAL_BASEADDR_I2C_0    //!< HAL internal use only
//#define HAL_BASEADDR_I2C_1    //!< HAL internal use only
//#define HAL_BASEADDR_I2C_2    //!< HAL internal use only


/******************************************************************************
 * HAL I2C bus locations
 *****************************************************************************/
//#define HAL_I2C_BUS_CAM_1     //!< Num of I2C bus CAM1 is connected to
//#define HAL_I2C_BUS_CAM_2     //!< Num of I2C bus CAM2 is connected to
//#define HAL_I2C_BUS_HDMI_TX   //!< Num of I2C bus HDMI TX is connected to
//#define HAL_I2C_BUS_CAMPHY_1  //!< Num of I2C bus CAMPHY1 is connected to
//#define HAL_I2C_BUS_CAMPHY_2  //!< Num of I2C bus CAMPHY2 is connected to


/******************************************************************************
 * HAL CAM to PHY mapping
 *****************************************************************************/
//#define HAL_DEVID_CAM_1_PHY   //!< DEVID of CAMPHY CAM1 is connected to
//#define HAL_DEVID_CAM_2_PHY   //!< DEVID of CAMPHY CAM2 is connected to
//#define HAL_DEVID_CAM_1_REC   //!< DEVID of CAMREC CAM1 is connected to
//#define HAL_DEVID_CAM_2_REC   //!< DEVID of CAMREC CAM2 is connected to


/*****************************************************************************/
/**
 * @brief   open the low level driver
 * @return  handle of driver; NULL on failure
 *
 * @note    Sets internal ref count to 1.
 *****************************************************************************/
HalHandle_t HalOpen( int ispID , uint32_t workmode);


/*****************************************************************************/
/**
 * @brief   close the low level driver
 * @param   HalHandle   Handle to HAL session as returned by @ref HalOpen.
 * @return  Result of operation.
 *
 * @note    Decrements internal ref count and closes low level driver if ref count reached zero.
 *****************************************************************************/
RESULT HalClose( HalHandle_t HalHandle );


/*****************************************************************************/
/**
 * @brief   tell HAL about another user of low level driver
 * @param   HalHandle   Handle to HAL session as returned by @ref HalOpen.
 * @return  Result of operation.
 *
 * @note
 *****************************************************************************/
RESULT HalAddRef( HalHandle_t HalHandle );


/*****************************************************************************/
/**
 * @brief   tell HAL about gone user of low level driver
 * @param   HalHandle   Handle to HAL session as returned by @ref HalOpen.
 * @return  Result of operation.
 *
 * @note    If the internal ref count is zero, the HAL will be closed as well.
 *****************************************************************************/
RESULT HalDelRef( HalHandle_t HalHandle );


/*****************************************************************************/
/**
 * @brief   Get the id from the current HAL handle
 * @param   HalHandle   Handle to HAL session as returned by @ref HalOpen.
 * @return  Result of operation.
 *
 * @note    
 *****************************************************************************/
RESULT HalGetId( HalHandle_t HalHandle, uint32_t *id );


/*****************************************************************************/
/**
 * @brief   Check current HAL whether to use FE to Refresh the register
 * @param   HalHandle   Handle to HAL session as returned by @ref HalOpen.
 * @return  Result of operation.
 *
 * @note
 *****************************************************************************/
RESULT HalIsUseFE( HalHandle_t HalHandle, bool_t *fe_using );


/*****************************************************************************/
/**
 * @brief   Set configuration of given CAM devices so that HAL can take care of
 *          signal polarities for reset & power. CAMs that aren't configured
 *          can't be correctly processed by \ref HalSetReset() or @ref HalSetPower()
 *          and will thus lead to a failure if accessed. Default state after
 *          successfull configuration is power down & reset active.
 * @param   HalHandle       Handle to HAL session as returned by @ref HalOpen.
 * @param   dev_mask        Mask of CAM devices to set configuration for.
 * @param   power_lowact    CAM power on signal is low-active.
 * @param   reset_lowact    CAM reset signal is low-active.
 * @param   pclk_negedge    Select negative edge of incoming PClk to sample
 *                          camera data. NOTE: @ref HalSetClock() must be called
 *                          to activate this new setting!
 * @return  Result of operation.
 *
 * @note    Device mask is bitwise OR (|) of HAL_DEVID_CAM_xxx.
 *****************************************************************************/
RESULT HalSetCamConfig( HalHandle_t HalHandle, uint32_t dev_mask, bool_t power_lowact, bool_t reset_lowact, bool_t pclk_negedge );


/*****************************************************************************/
/**
 * @brief   Set configuration of given CAM PHY devices so that HAL can take care of
 *          signal polarities for reset & power. CAM PHYs that aren't configured
 *          can't be correctly processed by \ref HalSetReset() or @ref HalSetPower()
 *          and will thus lead to a failure if accessed. Default state after
 *          successfull configuration is power down & reset active.
 * @param   HalHandle       Handle to HAL session as returned by @ref HalOpen.
 * @param   dev_mask        Mask of CAM PHY devices to set configuration for.
 * @param   power_lowact    CAM PHY power on signal is low-active.
 * @param   reset_lowact    CAM PHY reset signal is low-active.
 *
 * @return  Result of operation.
 *
 * @note    Device mask is bitwise OR (|) of HAL_DEVID_CAMPHY_xxx.
 *****************************************************************************/
RESULT HalSetCamPhyConfig( HalHandle_t HalHandle, uint32_t dev_mask, bool_t power_lowact, bool_t reset_lowact );


/*****************************************************************************/
/**
 * @brief   Enables/Disables reset of given devices. HAL takes care of polarity!
 *          See @ref HalSetCamConfig() for details regarding CAM devices,
 *          and @ref HalSetCamPhyConfig() for details regarding CAMPHY devices.
 * @param   HalHandle   Handle to HAL session as returned by @ref HalOpen.
 * @param   dev_mask    Mask of devices to change reset state.
 * @return  Result of operation.
 *
 * @note    Device mask is bitwise OR (|) of HAL_DEVID_xxx.
 *****************************************************************************/
RESULT HalSetReset( HalHandle_t HalHandle, uint32_t dev_mask, bool_t activate );


/*****************************************************************************/
/**
 * @brief   Enables/Disables power of given devices. HAL takes care of polarity!
 *          See @ref HalSetCamConfig() for details regarding CAM devices,
 *          and @ref HalSetCamPhyConfig() for details regarding CAMPHY devices.
 * @param   HalHandle   Handle to HAL session as returned by @ref HalOpen.
 * @param   dev_mask    Mask of devices to change power state.
 * @return  Result of operation.
 *
 * @note    Device mask is bitwise OR (|) of HAL_DEVID_xxx.
 *****************************************************************************/
RESULT HalSetPower( HalHandle_t HalHandle, uint32_t dev_mask, bool_t activate );


/*****************************************************************************/
/**
 * @brief   Enables+sets/Disables clock of given devices.
 * @param   HalHandle   Handle to HAL session as returned by @ref HalOpen.
 * @param   dev_mask    Mask of devices to change clock settings for.
 * @param   frequency   Sets frequency of given clocks in Hz steps; the
 *                      resulting frequency of the clock is implementation
 *                      dependent, 0 turns clock off.
 * @return  Result of operation.
 *
 * @note    Device mask is bitwise OR (|) of HAL_DEVID_CAM_xxx.
 *****************************************************************************/
RESULT HalSetClock( HalHandle_t HalHandle, uint32_t dev_mask, uint32_t frequency );


/*****************************************************************************/
/**
 * @brief   reads a value from a given address
 * @param   HalHandle   Handle to HAL session as returned by @ref HalOpen.
 * @param   reg_address Address of register to read.
 * @return  Register value.
 *
 * @note    It is required to pass in the full address (base address + offset).
 *****************************************************************************/
INLINE uint32_t HalReadReg( HalHandle_t HalHandle, uint32_t reg_address );


/*****************************************************************************/
/**
 * @brief   writes a value to the given address
 * @param   HalHandle   Handle to HAL session as returned by @ref HalOpen.
 * @param   reg_address Address of register to write.
 * @param   value       Value to write into register.
 * @return  none
 *
 * @note    It is required to pass in the full address (base address + offset).
 *****************************************************************************/
INLINE void HalWriteReg( HalHandle_t HalHandle, uint32_t reg_address, uint32_t value );


/*****************************************************************************/
/**
 * @brief   reads a value from a specific part of the given address
 * @param   HalHandle   Handle to HAL session as returned by @ref HalOpen.
 * @param   reg_address Address of register to read.
 * @param   reg_mask    Mask to apply to register value after being read.
 * @param   shift_mask  Amount to right shift masked register value prior being returned.
 * @return  masked and shifted data of specific register part
 *
 * @note    It is required to pass in the full address (base address + offset).
 *****************************************************************************/
INLINE uint32_t HalReadMaskedReg( HalHandle_t HalHandle, uint32_t reg_address, uint32_t reg_mask, uint32_t shift_mask );


/*****************************************************************************/
/**
 * @brief   writes a value to specific part of the given address
 * @param   HalHandle   Handle to HAL session as returned by @ref HalOpen.
 * @param   reg_address Address of register to write.
 * @param   reg_mask    Mask to isolate parts of register that should be written.
 * @param   shift_mask  Amount to left shift value prior being modified in register.
 * @param   value       Value to write into specified register position.
 * @return  none
 *
 * @note    It is required to pass in the full address (base address + offset).
 *****************************************************************************/
INLINE void HalWriteMaskedReg( HalHandle_t HalHandle, uint32_t reg_address, uint32_t reg_mask, uint32_t shift_mask, uint32_t value );


/*****************************************************************************/
/**
 * @brief   reads a value from a given given system register
 * @param   HalHandle   Handle to HAL session as returned by @ref HalOpen.
 * @param   reg_address Address of system register to read.
 * @return  Register value.
 *
 * @note    It is required to pass in the full address (base address + offset).
 *          DEMO SYSTEM & VALIDATION USE ONLY, NOT INDENDED TO BE PORTABLE.
 *****************************************************************************/
INLINE uint32_t HalReadSysReg( HalHandle_t HalHandle, uint32_t reg_address );


/*****************************************************************************/
/**
 * @brief   writes a value to the given system register
 * @param   HalHandle   Handle to HAL session as returned by @ref HalOpen.
 * @param   reg_address Address of system register to write.
 * @param   value       Value to write into register.
 * @return  none
 *
 * @note    It is required to pass in the full address (base address + offset).
 *          DEMO SYSTEM & VALIDATION USE ONLY, NOT INDENDED TO BE PORTABLE.
 *****************************************************************************/
INLINE void HalWriteSysReg( HalHandle_t HalHandle, uint32_t reg_address, uint32_t value );


/*****************************************************************************/
/**
 * @brief   allocates the given amount of hardware memory
 * @param   HalHandle       Handle to HAL session as returned by @ref HalOpen.
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
uint32_t HalAllocMemory( HalHandle_t HalHandle, uint32_t byte_size, void * ipl_alignment );


/*****************************************************************************/
/**
 * @brief   frees the given block of hardware memory
 * @param   HalHandle       Handle to HAL session as returned by @ref HalOpen.
 * @param   mem_address     Start address of memory block in hardware memory.
 * @return  Result of operation.
 *
 * @note    Chunks of n*4K byte with addresses aligned to 4K are used internally.
 *****************************************************************************/
RESULT HalFreeMemory( HalHandle_t HalHandle, uint32_t mem_address );


/*****************************************************************************/
/**
 * @brief   reads a number of data from the memory to a buffer starting a the given address
 * @param   HalHandle       Handle to HAL session as returned by @ref HalOpen.
 * @param   mem_address     Source start address in hardware memory.
 * @param   p_read_buffer   Pointer to local memory holding the data being read.
 * @param   byte_size       Amount of data to read.
 * @return  Result of operation.
 *
 * @note    Certain implementation dependent limitations regarding alignment of
 *          both addresses and transfer size exist!
 *****************************************************************************/
RESULT HalReadMemory( HalHandle_t HalHandle, uint32_t mem_address, uint8_t* p_read_buffer, uint32_t byte_size );


/*****************************************************************************/
/**
 * @brief   writes a number of data from a buffer to the memory starting a the given address
 * @param   HalHandle       Handle to HAL session as returned by @ref HalOpen.
 * @param   mem_address     Target start address in hardware memory.
 * @param   p_write_buffer  Pointer to local memory holding the data to be written.
 *                          Undefined on failure.
 * @param   byte_size       Amount of data to write.
 * @return  Result of operation.
 *
 * @note    Certain implementation dependent limitations regarding alignment of
 *          both addresses and transfer size exist!
 *****************************************************************************/
RESULT HalWriteMemory( HalHandle_t HalHandle, uint32_t mem_address, uint8_t* p_write_buffer, uint32_t byte_size );


/*****************************************************************************/
/**
 * @brief   maps a number of data from the memory into local memory starting at the given address
 * @param   HalHandle       Handle to HAL session as returned by @ref HalOpen.
 * @param   mem_address     Source start address in hardware memory.
 * @param   byte_size       Amount of data to map.
 * @param   mapping_type    The way the mapping is performed.
 * @param   pp_mapped_buf   Reference to pointer to the mapped local memory.
 * @return  Result of operation.
 *
 * @note    Certain implementation dependent limitations regarding alignment of
 *          both addresses and transfer size exist!
 *****************************************************************************/
RESULT HalMapMemory( HalHandle_t HalHandle, uint32_t mem_address, uint32_t byte_size, HalMapMemType_t mapping_type, void **pp_mapped_buf );


/*****************************************************************************/
/**
 * @brief   unmaps previously mapped memory from local memory
 * @param   HalHandle       Handle to HAL session as returned by @ref HalOpen.
 * @param   p_mapped_buf    Pointer to local memory to unmap as returned by
 *                          @ref HalMapMemory().
 * @return  Result of operation.
 *
 * @note    Certain implementation dependent limitations regarding alignment of
 *          both addresses and transfer size exist!
 *****************************************************************************/
RESULT HalUnMapMemory( HalHandle_t HalHandle, void* p_mapped_buf );

/*****************************************************************************/
/**
 * @brief   reads a number of data from the memory to a buffer starting a the given address
 * @param   HalHandle       Handle to HAL session as returned by @ref HalOpen.
 * @param   bus_num         Number of bus which is to be used.
 * @param   slave_addr      Address of slave to be accessed (supports auto detection of 10bit adresses).
 * @param   reg_address     Address of register to read.
 * @param   reg_addr_size   Size of @ref reg_address in bytes, valid range: 0..4 bytes.
 * @param   preg_value      Pointer to local memory holding the register data being read.
 * @param   reg_size        Size of register in bytes, valid range: 1..4.
 * @return  Result of operation.
 *
 * @note    The register data gets read starting with the least significant byte!
 *****************************************************************************/
INLINE RESULT HalReadI2CReg( HalHandle_t HalHandle, uint8_t bus_num, uint16_t slave_addr, uint32_t reg_address, uint8_t reg_addr_size, void *preg_value, uint8_t reg_size )
{
    if ((reg_size < 1) || (reg_size > 4))
    {
        return RET_INVALID_PARM;
    }

    return 0;
}

/*****************************************************************************/
/**
 * @brief   writes a number of data from a buffer to the memory starting a the given address
 * @param   HalHandle       Handle to HAL session as returned by @ref HalOpen.
 * @param   bus_num         Number of bus which is to be used.
 * @param   slave_addr      Address of slave to be accessed (supports auto detection of 10bit adresses).
 * @param   reg_address     Address of register to write.
 * @param   reg_addr_size   Size of @ref reg_address in bytes, valid range: 0..4 bytes.
 * @param   reg_value       Register data to be written.
 * @param   reg_size        Size of register in bytes, valid range: 1..4.
 * @return  Result of operation.
 *
 * @note    The register data gets written starting with the least significant byte!
 *****************************************************************************/
INLINE RESULT HalWriteI2CReg( HalHandle_t HalHandle, uint8_t bus_num, uint16_t slave_addr, uint32_t reg_address, uint8_t reg_addr_size, uint32_t reg_value, uint8_t reg_size )
{
    if ((reg_size < 1) || (reg_size > 4))
    {
        return RET_INVALID_PARM;
    }

    return 0;
}


/*****************************************************************************/
/**
 *          HalConnectIrq()
 *
 *  @brief  Register interrupt service routine with system software.
 *
 *  @param  Handle to HAL session as returned by @ref HalOpen
 *  @param  Reference of hal irq context structure that represent this connection
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
RESULT HalConnectIrq
(
    HalHandle_t HalHandle,
    HalIrqCtx_t *pIrqCtx,
    uint32_t    int_src,
    osIsrFunc   IsrFunction,
    osDpcFunc   DpcFunction,
    void*       pContext
);

/*****************************************************************************/
/**
 *          HalDisconnectIrq()
 *
 *  @brief  Deregister interrupt service routine from system software.
 *
 *  @param  Reference of hal irq context structure that represent this connection
 *
 *  @return                     Status of operation
 *  @retval RET_SUCCESS         ISR deregistered successfully
 *  @retval RET_FAILURE et al.  ISR not properly deregistered
 *
 *****************************************************************************/
RESULT HalDisconnectIrq
(
    HalIrqCtx_t *pIrqCtx
);

#if defined (HAL_CMODEL)
/*****************************************************************************/
/**
 *          HalCmHwInit()
 *
 *  @brief  Cmodel specific interface: Setup register initial value
 *
 *  @param  ISP instance type
 *
 *  @return                     Status of operation
 *  @retval RET_SUCCESS         Input set successfully
 *  @retval RET_FAILURE et al.  Input set fails
 *
 *****************************************************************************/
RESULT HalCmHwInit(HalHandle_t HalHandle, uint32_t isp_type, const char* iniRegFileName);

/*****************************************************************************/
/**
 *          HalCmInputInfoSetup()
 *
 *  @brief  Cmodel specific interface: Setup input h and v size
 *
 *  @param  h_size, v_size: image size of input port.
 *
 *  @return                     Status of operation
 *  @retval RET_SUCCESS         Input set successfully
 *  @retval RET_FAILURE et al.  Input set fails
 *
 *****************************************************************************/
// Set h and v size for simulation
RESULT HalCmInputInfoSetup(
    unsigned char id,
    unsigned int h_size,
    unsigned int v_size
);

/*****************************************************************************/
/**
 *          HalCmBufTransfer()
 *
 *  @brief  Cmodel specific interface: Buffer transfer for DMA
 *
 *  @param
 *
 *  @return                     Status of operation
 *  @retval RET_SUCCESS         Input set successfully
 *  @retval RET_FAILURE et al.  Input set fails
 *
 *****************************************************************************/
RESULT HalCmBufTransfer(    HalHandle_t HalHandle,  uint32_t trans_ctrl);

/*****************************************************************************/
/**
 *          HalCmYuv12BitFillBuf()
 *
 *  @brief  Cmodel specific interface: yuv 12bit Buffer transfer for DMA
 *
 *  @param
 *
 *  @return                     Status of operation
 *  @retval RET_SUCCESS         Input set successfully
 *  @retval RET_FAILURE et al.  Input set fails
 *
 *****************************************************************************/
RESULT HalCmYuv12BitFillBuf(HalContext_t *pHalCtx, uint32_t memIndex, HalCmOutputPath_t path); // get image data from mi output files and store in the picbuffer.


/*****************************************************************************/
/**
 *          HalCmYuv10BitFillBuf()
 *
 *  @brief  Cmodel specific interface: yuv 10bit Buffer transfer for DMA
 *
 *  @param
 *
 *  @return                     Status of operation
 *  @retval RET_SUCCESS         Input set successfully
 *  @retval RET_FAILURE et al.  Input set fails
 *
 *****************************************************************************/
RESULT HalCmYuv10BitFillBuf(HalContext_t *pHalCtx, uint32_t memIndex, HalCmOutputPath_t path); // get image data from mi output files and store in the picbuffer.

#ifdef ISP_GCMONO
/*****************************************************************************/
/**
 *          HalCmGcmonoBufFill()
 *
 *  @brief  Cmodel specific interface: Buffer transfer for GCMONO
 *
 *  @param
 *
 *  @return                     Status of operation
 *  @retval RET_SUCCESS         Input set successfully
 *  @retval RET_FAILURE et al.  Input set fails
 *
 *****************************************************************************/
RESULT HalCmGcmonoBufFill(HalHandle_t HalHandle, uint8_t* gc_lut, unsigned int lut_length);
#endif

/******************************************************************************
 * Cmodel Specific API: HalCmShdRegisterUpdate
 *****************************************************************************/
// Cmodel Shd register update process
RESULT HalCmShdRegisterUpdate(HalHandle_t HalHandle);


/*****************************************************************************/
/**
 *          HalCmBufConfig()
 *
 *  @brief  Cmodel specific interface: Buffer transfer Configuration
 *
 *  @param
 *
 *  @return                     Status of operation
 *  @retval RET_SUCCESS         Input set successfully
 *  @retval RET_FAILURE et al.  Input set fails
 *
 *****************************************************************************/
RESULT HalCmGetBufConfig(    HalHandle_t HalHandle, HalCmodelBufConfig_t** pCmBufCfg);


/*****************************************************************************/
/**
 *          HalCmISPpipeline()
 *
 *  @brief  Cmodel specific interface: Hal ISP8000Process
 *
 *  @param  none
 *
 *  @return                     Status of operation
 *    none
 *
 *****************************************************************************/
void HalCmISPpipeline(HalHandle_t HalHandle);

/*****************************************************************************/
/**
 *          HalCmDumpBuf()
 *
 *  @brief  Cmodel specific interface: memory dump functions
 *
 *  @param  buf: base address of memory
 *          len: length of memory
 *          str: debug info
 *
 *  @return                     None
 *
 *****************************************************************************/
void HalCmDumpBuf(   char *buf, int len, char* str);

/*****************************************************************************/
/**
 *          HalCmDumpReFill()
 *
 *  @brief  Cmodel specific interface: from Cmodel memory dump
 *      re-pick up and fill buffer
 *
 *  @param  buffer: base address of memory
 *          file: cmodel dump file
 *          buf_len: buffer limitation
 *
 *  @return                     None
 *
 *****************************************************************************/
RESULT HalCmDumpReFill( unsigned char * buffer, const char* file, unsigned int buf_len);

/*****************************************************************************/
/**
 *          HalCmDumpReFillPostProcess()
 *
 *  @brief  Cmodel specific interface: from Cmodel memory dump
 *      re-pick up and fill buffer
 *
 *  @param  pHalCtx: Hal Context
 *          file: cmodel dump file
 *
 *  @return                     None
 *
 *****************************************************************************/
RESULT HalCmReFillPostProcess(HalContext_t *pHalCtx, const char* file);


#endif

/*****************************************************************************/
/**
 *          HalEventGenerator()
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
RESULT HalEventGenerator(    HalIrqCtx_t *pIrqCtx, HalIsrSrc_t isr_src, uint32_t misValue);

#if defined ( HAL_ALTERA )
/*****************************************************************************/
/**
 *          HalFpgaDumpBuf()
 *
 *  @brief  FPGA online dump function: memory dump functions
 *
 *  @param  buf: base address of memory
 *          len: length of memory
 *          str: debug info
 *
 *  @return                     None
 *
 *****************************************************************************/
void HalFpgaDumpBuf (char *buf, int len, char* str);


/*****************************************************************************/
/**
 *          HalFpgaReset()
 *
 *  @brief  FPGA online set FPGA to reset status
 *
 *  @param
 *
 *  @return                     None
 *
 *****************************************************************************/
void HalFpgaReset(HalHandle_t HalHandle);

/*****************************************************************************/
/**
 *          HalFpgaDereset()
 *
 *  @brief  FPGA online set FPGA out of reset mode
 *
 *  @param
 *
 *  @return                     None
 *
 *****************************************************************************/
void HalFpgaDereset(HalHandle_t HalHandle);

void HalFpgaPreSet(HalHandle_t HalHandle);

#endif

/******************************************************************************
 * stuff below here requires the inline API implementations being included
 * as it requires some more hal variant depended header files being loaded
 *****************************************************************************/



#ifdef __cplusplus
}
#endif

//!@} defgroup HAL_API

#endif /* __HAL_API_H__ */
