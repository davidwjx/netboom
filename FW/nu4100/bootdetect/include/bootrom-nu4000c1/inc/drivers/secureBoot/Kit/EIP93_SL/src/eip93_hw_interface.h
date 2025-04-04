/* eip93_hw_interface.h
 *
 * This file contains EIP93 HW interface (memory map).
 * HW register and memory offset definitions and related macros.
 */

/*****************************************************************************
* Copyright (c) 2008-2018 INSIDE Secure B.V. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
* For more information or support, please go to our online support system at
* https://customersupport.insidesecure.com.
* In case you do not have an account for this system, please send an e-mail
* to ESSEmbeddedHW-Support@insidesecure.com.
*****************************************************************************/

#ifndef INCLUDE_GUARD_EIP93_HW_INTERFACE_H
#define INCLUDE_GUARD_EIP93_HW_INTERFACE_H

#define EIP93_REG_WIDTH     4

/*-----------------------------------------------------------------------------
 * Register Map
 */
#define DESP_BASE                   0x00000000
#define EIP93_REG_PE_CTRL_STAT      ((DESP_BASE)+(0x00 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_SOURCE_ADDR    ((DESP_BASE)+(0x01 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_DEST_ADDR      ((DESP_BASE)+(0x02 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_SA_ADDR        ((DESP_BASE)+(0x03 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_ADDR           ((DESP_BASE)+(0x04 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_USER_ID        ((DESP_BASE)+(0x06 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_LENGTH         ((DESP_BASE)+(0x07 * EIP93_REG_WIDTH))

//PACKET ENGINE RING configuration registers
#define PE_RNG_BASE                 0x00000080
#define EIP93_REG_PE_CDR_BASE       ((PE_RNG_BASE)+(0x00 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_RDR_BASE       ((PE_RNG_BASE)+(0x01 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_RING_CFG       ((PE_RNG_BASE)+(0x02 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_RING_THRESH    ((PE_RNG_BASE)+(0x03 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_CD_COUNT       ((PE_RNG_BASE)+(0x04 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_RD_COUNT       ((PE_RNG_BASE)+(0x05 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_RING_RW_PNTR   ((PE_RNG_BASE)+(0x06 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_RING_STAT      ((PE_RNG_BASE)+(0x07 * EIP93_REG_WIDTH))

//PACKET ENGINE configuration registers
#define PE_CONF_BASE                0x00000100
#define EIP93_REG_PE_CONFIG         ((PE_CONF_BASE)+(0x00 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_STATUS         ((PE_CONF_BASE)+(0x01 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_BUF_THRESH     ((PE_CONF_BASE)+(0x03 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_INBUF_COUNT    ((PE_CONF_BASE)+(0x04 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_OUTBUF_COUNT   ((PE_CONF_BASE)+(0x05 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_BUF_PNTR       ((PE_CONF_BASE)+(0x06 * EIP93_REG_WIDTH))

//PACKET ENGINE DMA configuration registers
#define DMA_CONF_BASE               0x00000120
#define EIP93_REG_PE_DMA_CONFIG     ((DMA_CONF_BASE)+(0x00 * EIP93_REG_WIDTH))

//PACKET ENGINE endian configuration registers
#define END_CONF_BASE               0x000001D0
#define EIP93_REG_PE_ENDIAN_CONFIG  ((END_CONF_BASE)+(0x00 * EIP93_REG_WIDTH))

//EIP93 CLOCK control registers
#define CLOCK_BASE                  0x000001E8
#define EIP93_REG_PE_CLOCK_CTRL     ((CLOCK_BASE)+(0x00 * EIP93_REG_WIDTH))

//EIP93 Device Option and Revision Register
#define REV_BASE                    0x000001F4
#define EIP93_REG_PE_OPTION_1       ((REV_BASE)+(0x00 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_OPTION_0       ((REV_BASE)+(0x01 * EIP93_REG_WIDTH))
#define EIP93_REG_PE_REVISION       ((REV_BASE)+(0x02 * EIP93_REG_WIDTH))

//EIP93 Interrupt Control Register
#define INT_BASE                    0x00000200
#define EIP93_REG_INT_UNMASK_STAT   ((INT_BASE)+(0x00 * EIP93_REG_WIDTH))
#define EIP93_REG_INT_MASK_STAT     ((INT_BASE)+(0x01 * EIP93_REG_WIDTH))
#define EIP93_REG_INT_CLR           ((INT_BASE)+(0x01 * EIP93_REG_WIDTH))
#define EIP93_REG_INT_MASK          ((INT_BASE)+(0x02 * EIP93_REG_WIDTH))
#define EIP93_REG_INT_CFG           ((INT_BASE)+(0x03 * EIP93_REG_WIDTH))
#define EIP93_REG_MASK_ENABLE       ((INT_BASE)+(0X04 * EIP93_REG_WIDTH))
#define EIP93_REG_MASK_DISABLE      ((INT_BASE)+(0X05 * EIP93_REG_WIDTH))

//EIP93 SA Record register
#define SA_BASE                     0x00000400
#define EIP93_REG_SA_CMD_0          ((SA_BASE)+(0x00 * EIP93_REG_WIDTH))
#define EIP93_REG_SA_CMD_1          ((SA_BASE)+(0x01 * EIP93_REG_WIDTH))
// Note the remaining SA Structure depends on the used HW configuration

//SA State Structure base
#define STATE_BASE                  0x00000500
#define EIP93_REG_STATE_IV_0        ((STATE_BASE)+(0x00 * EIP93_REG_WIDTH))
#define EIP93_REG_STATE_IV_1        ((STATE_BASE)+(0x01 * EIP93_REG_WIDTH))
// ...

//ARC4 State Structure base
#define EIP93_PE_ARC4STATE_BASEADDR 0x00000700
// ...

//RAM buffer start address
#define EIP93_INPUT_BUFFER          0x00000800
#define EIP93_OUTPUT_BUFFER         0x00000800


/*-----------------------------------------------------------------------------
 * Constants & masks
 */

#define EIP93_SUPPORTED_INTERRUPTS_MASK 0xffff7f00
#define EIP93_PRNG_DT_TEXT_LOWERHALF    0xDEAD
#define EIP93_PRNG_DT_TEXT_UPPERHALF    0xC0DE
#define EIP93_10BITS_MASK               0x3FF
#define EIP93_12BITS_MASK               0xFFF
#define EIP93_4BITS_MASK                0x04
#define EIP93_20BITS_MASK               0xFFFFF

#define EIP93_MIN_DESC_DONE_COUNT    0
#define EIP93_MAX_DESC_DONE_COUNT    15

#define EIP93_MIN_DESC_PENDING_COUNT  0
#define EIP93_MAX_DESC_PENDING_COUNT  1023

#define EIP93_MIN_TIMEOUT_COUNT      0
#define EIP93_MAX_TIMEOUT_COUNT      15

#define EIP93_MIN_PE_INPUT_THRESHOLD   1
#define EIP93_MAX_PE_INPUT_THRESHOLD   511

#define EIP93_MIN_PE_OUTPUT_THRESHOLD   1
#define EIP93_MAX_PE_OUTPUT_THRESHOLD   432

#define EIP93_MIN_PE_RING_SIZE          1
#define EIP93_MAX_PE_RING_SIZE          1023

#define EIP93_MIN_PE_DESCRIPTOR_SIZE    7
#define EIP93_MAX_PE_DESCRIPTOR_SIZE    15


/*-----------------------------------------------------------------------------
 * EIP93 device initialization specifics
 */

/*----------------------------------------------------------------------------
 * Byte Order Reversal Mechanisms Supported in EIP93
 * EIP93_BO_REVERSE_HALF_WORD : reverse the byte order within a half-word
 * EIP93_BO_REVERSE_WORD :  reverse the byte order within a word
 * EIP93_BO_REVERSE_DUAL_WORD : reverse the byte order within a dual-word
 * EIP93_BO_REVERSE_QUAD_WORD : reverse the byte order within a quad-word
 */
typedef enum
{
    EIP93_BO_REVERSE_HALF_WORD = 1,
    EIP93_BO_REVERSE_WORD = 2,
    EIP93_BO_REVERSE_DUAL_WORD = 4,
    EIP93_BO_REVERSE_QUAD_WORD = 8,
} EIP93_Byte_Order_Value_t;

/*----------------------------------------------------------------------------
 * Byte Order Reversal Mechanisms Supported in EIP93 for Target Data
 * EIP93_BO_REVERSE_HALF_WORD : reverse the byte order within a half-word
 * EIP93_BO_REVERSE_WORD :  reverse the byte order within a word
 */
typedef enum
{
    EIP93_BO_REVERSE_HALF_WORD_TD = 1,
    EIP93_BO_REVERSE_WORD_TD = 2,
} EIP93_Byte_Order_Value_TD_t;


// BYTE_ORDER_CFG register values
#define EIP93_BYTE_ORDER_PD         EIP93_BO_REVERSE_WORD
#define EIP93_BYTE_ORDER_SA         EIP93_BO_REVERSE_WORD
#define EIP93_BYTE_ORDER_DATA       EIP93_BO_REVERSE_WORD
#define EIP93_BYTE_ORDER_TD         EIP93_BO_REVERSE_WORD_TD

// INT_CFG register values
#define EIP93_INT_HOST_OUTPUT_TYPE  0
#define EIP93_INT_PULSE_CLEAR       0


#endif /* Include Guard */


/* end of file eip93_hw_interface */
