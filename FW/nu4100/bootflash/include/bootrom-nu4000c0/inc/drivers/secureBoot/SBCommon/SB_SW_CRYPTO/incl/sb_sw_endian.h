/**
*  File: sb_sw_endian.h
*
*  Description : Secure Boot Endianness Macros.
*
* Copyright (c) 2007-2018 INSIDE Secure B.V. All Rights Reserved.
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
*/

#ifndef _SB_SW_ENDIAN_H_
#define _SB_SW_ENDIAN_H_

#include "implementation_defs.h"
#include "sbif_ecdsa.h"
#if !defined(SB_CF_BIGENDIAN) && !defined(SB_CF_LITTLEENDIAN)
#include "cf_sb.h"
#endif /* !SB_CF_BIGENDIAN && !SB_CF_LITTLEENDIAN */


/* This file defines following byte order manipulation macros

   Macro name              Result    Param     Effect
   --------------------    ------    -----     ------
   SB_SW_BYTEORDER_SWAP32  uint32_t  uint32_t  Swaps byte order of
                                               input 32-bit value
   SB_SW_BYTEORDER_SWAP16  uint16_t  uint16_t  Swaps byte-order of
                                               input 16-bit value
   SB_SW_CPU_TO_BE32       uint32_t  uint32_t  Converts 32-bit value from CPU
                                               byte-order to big endian
   SB_SW_CPU_TO_LE32       uint32_t  uint32_t  Converts 32-bit value from CPU
                                               byte-order to little endian
   SB_SW_BE32_TO_CPU       uint32_t  uint32_t  Converts 32-bit big endian
                                               value to CPU byte-order
   SB_SW_LE32_TO_CPU       uint32_t  uint32_t  Converts 32-bit little endian
                                               value to CPU byte-order
   SB_SW_CPU_TO_BE16       uint16_t  uint16_t  Converts 16-bit value from CPU
                                               byte-order to big endian
   SB_SW_CPU_TO_LE16       uint16_t  uint16_t  Converts 16-bit value from CPU
                                               byte-order to little endian
   SB_SW_BE16_TO_CPU       uint16_t  uint16_t  Converts 16-bit big endian
                                               value to CPU byte-order
   SB_SW_LE16_TO_CPU       uint16_t  uint16_t  Converts 16-bit little endian
                                               value to CPU byte-order

   Also, following macros are defined for loading 32-bit or 16-bit values from
   memory, in various byte orders:

   Macro name              Result    Param     Effect
   --------------------    ------    -----     ------
   SB_SW_BE32_READ_ALIGNED uint32_t  pointer   Read big endian value from
                                               aligned memory location.
   SB_SW_BE32_READ         uint32_t  pointer   Read big endian value from
                                               any memory location.
   SB_SW_LE32_READ_ALIGNED uint32_t  pointer   Read little endian value from
                                               aligned memory location.
   SB_SW_LE32_READ         uint32_t  pointer   Read little endian value from
                                               any memory location.
   SB_SW_CPU32_READ        uint32_t  pointer   Read 32-bit value from any
                                               memory location.
   SB_SW_BE16_READ_ALIGNED uint16_t  pointer   Read big endian value from
                                               aligned memory location.
   SB_SW_BE16_READ         uint16_t  pointer   Read big endian value from
                                               any memory location.
   SB_SW_LE16_READ_ALIGNED uint16_t  pointer   Read little endian value from
                                               aligned memory location.
   SB_SW_LE16_READ         uint16_t  pointer   Read little endian value from
                                               any memory location.
   SB_SW_CPU16_READ        uint16_t  pointer   Read 16-bit value from any
                                               memory location.

   Also, following macros are provided for storing values to memory
   (either aligned or unaligned addresses):

   Macro name               Param1   Param2    Effect
   --------------------     ------   ------    ------
   SB_SW_BE32_WRITE_ALIGNED pointer  uint32_t  Write big endian value from
                                               aligned memory location.
   SB_SW_BE32_WRITE         pointer  uint32_t  Write big endian value from
                                               any memory location.
   SB_SW_LE32_WRITE_ALIGNED pointer  uint32_t  Write little endian value from
                                               aligned memory location.
   SB_SW_LE32_WRITE         pointer  uint32_t  Write little endian value from
                                               any memory location.
   SB_SW_CPU32_WRITE        pointer  uint32_t  Write 32-bit value from any
                                               memory location.
   SB_SW_BE16_WRITE_ALIGNED pointer  uint16_t  Write big endian value from
                                               aligned memory location.
   SB_SW_BE16_WRITE         pointer  uint16_t  Write big endian value from
                                               any memory location.
   SB_SW_LE16_WRITE_ALIGNED pointer  uint16_t  Write little endian value from
                                               aligned memory location.
   SB_SW_LE16_WRITE         pointer  uint16_t  Write little endian value from
                                               any memory location.
   SB_SW_CPU16_WRITE        pointer  uint16_t  Write 16-bit value from any
                                               memory location.

   Following macros are provided for constants
   Macro name                       Result    Param     Effect
   --------------------             ------    -----     ------
   SB_SW_BYTEORDER_SWAP32_CONSTANT  uint32_t  uint32_t  Swaps byte order of
                                                        input 32-bit constant
                                                        value
   SB_SW_BYTEORDER_SWAP16_CONSTANT  uint16_t  uint16_t  Swaps byte-order of
                                                        input 16-bit constant
                                                        value
   SB_SW_CPU_TO_BE32_CONSTANT       uint32_t  uint32_t  Converts 32-bit
                                                        constant value from CPU
                                                        byte-order to
                                                        big endian byte-order.
   SB_SW_CPU_TO_LE32_CONSTANT       uint32_t  uint32_t  Converts 32-bit
                                                        constant value from CPU
                                                        byte-order to
                                                        little endian
   SB_SW_CPU_TO_BE16_CONSTANT       uint16_t  uint16_t  Converts 16-bit
                                                        constant value from CPU
                                                        byte-order to
                                                        big endian byte-order.
   SB_SW_CPU_TO_LE16_CONSTANT       uint16_t  uint16_t  Converts 16-bit
                                                        constant value from CPU
                                                        byte-order to
                                                        little endian
*/

/* Macro for byte order swapping. The macro might be overridden with
   platform specific assembly instruction. */
#ifndef SB_SW_BYTEORDER_SWAP32
#define SB_SW_BYTEORDER_SWAP32(x) \
  (((x) << 24) | \
   (((x) & 0x0000ff00U) << 8) | \
   (((x) & 0x00ff0000U) >> 8) | \
   ((x) >> 24))
#endif /* SB_SW_BYTEORDER_SWAP32 */

/* Similar macro for half-word entities. */
#ifndef SB_SW_BYTEORDER_SWAP16
#define SB_SW_BYTEORDER_SWAP16(x) ((uint16_t) (((x) << 8) | ((x) >> 8)))
#endif /* SB_SW_BYTEORDER_SWAP16 */

#ifdef SB_CF_BIGENDIAN
/* Define word / byte conversion macros for big endian. */
#define SB_SW_CPU_TO_BE32(x) (x)
#define SB_SW_CPU_TO_LE32(x) SB_SW_BYTEORDER_SWAP32(x)
#define SB_SW_BE32_TO_CPU(x) (x)
#define SB_SW_LE32_TO_CPU(x) SB_SW_BYTEORDER_SWAP32(x)

#define SB_SW_CPU_TO_BE16(x) (x)
#define SB_SW_CPU_TO_LE16(x) SB_SW_BYTEORDER_SWAP16(x)
#define SB_SW_BE16_TO_CPU(x) (x)
#define SB_SW_LE16_TO_CPU(x) SB_SW_BYTEORDER_SWAP16(x)
#elif defined(SB_CF_LITTLEENDIAN)
/* Define word / byte conversion macros for little endian. */

#define SB_SW_CPU_TO_LE32(x) (x)
#define SB_SW_CPU_TO_BE32(x) SB_SW_BYTEORDER_SWAP32(x)
#define SB_SW_LE32_TO_CPU(x) (x)
#define SB_SW_BE32_TO_CPU(x) SB_SW_BYTEORDER_SWAP32(x)

#define SB_SW_CPU_TO_LE16(x) (x)
#define SB_SW_CPU_TO_BE16(x) SB_SW_BYTEORDER_SWAP16(x)
#define SB_SW_LE16_TO_CPU(x) (x)
#define SB_SW_BE16_TO_CPU(x) SB_SW_BYTEORDER_SWAP16(x)
#else
/* Endian is not specified or endian is not big-endian or little-endian. */
#error "Endianness must be specified via SB_CF_BIGENDIAN or SB_CF_LITTLEENDIAN."
#endif /* choose code according to endianness */

/* Macros for reading 32-bit or 16-bit values in specified byte-order. */
#define SB_SW_LE32_READ_ALIGNED(x) SB_SW_LE32_TO_CPU(*(uint32_t*)(x))
#define SB_SW_LE16_READ_ALIGNED(x) SB_SW_LE16_TO_CPU(*(uint16_t*)(x))
#define SB_SW_BE32_READ_ALIGNED(x) SB_SW_BE32_TO_CPU(*(uint32_t*)(x))
#define SB_SW_BE16_READ_ALIGNED(x) SB_SW_BE16_TO_CPU(*(uint16_t*)(x))

/* Macros for writing 32-bit or 16-bit values in specified byte-order. */
#define SB_SW_LE32_WRITE_ALIGNED(x, y) \
    *(uint32_t*)(x) = SB_SW_CPU_TO_LE32(y)
#define SB_SW_LE16_WRITE_ALIGNED(x, y) \
    *(uint16_t*)(x) = SB_SW_CPU_TO_LE16(y)
#define SB_SW_BE32_WRITE_ALIGNED(x, y) \
    *(uint32_t*)(x) = SB_SW_CPU_TO_BE32(y)
#define SB_SW_BE16_WRITE_ALIGNED(x, y) \
    *(uint16_t*)(x) = SB_SW_CPU_TO_BE16(y)

#if !defined(SB_SW_FORCE_ALIGNED_ACCESS) && \
    (defined(SB_SW_UNALIGNED_ACCESS) || \
     defined(__i386__) || defined(__x86_64__))
/* These architectures support unaligned access and therefore aligned
   reads/writes can be directly used for accessing unaligned values. */
#define SB_SW_LE32_READ  SB_SW_LE32_READ_ALIGNED
#define SB_SW_LE16_READ  SB_SW_LE16_READ_ALIGNED
#define SB_SW_BE32_READ  SB_SW_BE32_READ_ALIGNED
#define SB_SW_BE16_READ  SB_SW_BE16_READ_ALIGNED
#define SB_SW_LE32_WRITE SB_SW_LE32_WRITE_ALIGNED
#define SB_SW_LE16_WRITE SB_SW_LE16_WRITE_ALIGNED
#define SB_SW_BE32_WRITE SB_SW_BE32_WRITE_ALIGNED
#define SB_SW_BE16_WRITE SB_SW_BE16_WRITE_ALIGNED

#ifndef SB_SW_CPU32_READ
#ifdef SB_CF_BIGENDIAN
#define SB_SW_CPU32_READ  SB_SW_BE32_READ_ALIGNED
#define SB_SW_CPU32_WRITE SB_SW_BE32_WRITE_ALIGNED
#else
#define SB_SW_CPU32_READ  SB_SW_LE32_READ_ALIGNED
#define SB_SW_CPU32_WRITE SB_SW_LE32_WRITE_ALIGNED
#endif
#endif /* SB_SW_CPU32_READ */

#ifndef SB_SW_CPU16_READ
#ifdef SB_CF_BIGENDIAN
#define SB_SW_CPU16_READ  SB_SW_BE16_READ_ALIGNED
#define SB_SW_CPU16_WRITE SB_SW_BE16_WRITE_ALIGNED
#else
#define SB_SW_CPU16_READ  SB_SW_LE16_READ_ALIGNED
#define SB_SW_CPU16_WRITE SB_SW_LE16_WRITE_ALIGNED
#endif
#endif /* SB_SW_CPU16_READ */

#else /* Unaligned access not available. */

#ifndef SB_SW_CPU32_READ
/* Macro for reading unaligned 32-bit value. */

/* Implemented as inline function: */
static inline uint32_t sb_sw_cpu32_read(const void * const ptr)
{
    uintptr_t ptrInt;
    uintptr_t ptrLow;
    uintptr_t ptrHi;
    uintptr_t ptrDiff;
    uintptr_t ptrDiffRev;
    uint32_t valueLow;
    uint32_t valueHi;

    ptrInt = (uintptr_t) ptr;
    ptrLow = ptrInt & (~3);
    ptrHi = (ptrInt + 3) & (~3);

    ptrDiff = (ptrInt - ptrLow) * 8;
    valueLow = *(uint32_t *)ptrLow;
    valueHi = *(uint32_t *)ptrHi;

    /* Following table describes contents of valueLow and valueHi and
       desired output value:
       (big endian)
       valueLow valueHi  ptrDiff valueOut
       IIJJKKLL IIJJKKLL 0       IIJJKKLL
       IIJJKKLL MMNNOOPP 8       JJKKLLMM
       IIJJKKLL MMNNOOPP 16      KKLLMMNN
       IIJJKKLL MMNNOOPP 24      LLMMNNOO

       (little endian)
       LLKKJJII LLKKJJII 0       LLKKJJII
       LLKKJJII PPOONNMM 8       MMLLKKJJ
       LLKKJJII PPOONNMM 16      NNMMLLKK
       LLKKJJII PPOONNMM 24      OONNMMLL
     */

    ptrDiffRev = (32 - ptrDiff) & 31;
#ifdef SB_CF_BIGENDIAN
    /* Suitable shifting for big endian. */
    return (valueLow << ptrDiff) | (valueHi >> ptrDiffRev);
#else
    /* Little endian. */
    return (valueLow >> ptrDiff) | (valueHi << ptrDiffRev);
#endif /* choose code according to endianness */
}
#define SB_SW_CPU32_READ(x) sb_sw_cpu32_read(x)
#endif /* SB_SW_CPU32_READ */

#ifndef SB_SW_BE32_WRITE
static inline void sb_sw_be32_write(void * const Value_p,
                                    const uint32_t NewValue)
{
    uint8_t * const Value8_p = (uint8_t *) Value_p;
    Value8_p[0] = NewValue >> 24;
    Value8_p[1] = (uint8_t) (NewValue >> 16);
    Value8_p[2] = (uint8_t) (NewValue >> 8);
    Value8_p[3] = (uint8_t) NewValue;
}
#define SB_SW_BE32_WRITE(x, y) sb_sw_be32_write(x, y)
#endif /* SB_SW_BE32_WRITE */

#ifndef SB_SW_LE32_WRITE
static inline void sb_sw_le32_write(void * const Value_p,
                                    const uint32_t NewValue)
{
    uint8_t * const Value8_p = (uint8_t *) Value_p;
    Value8_p[3] = NewValue >> 24;
    Value8_p[2] = (uint8_t) (NewValue >> 16);
    Value8_p[1] = (uint8_t) (NewValue >> 8);
    Value8_p[0] = (uint8_t) NewValue;
}
#define SB_SW_LE32_WRITE(x, y) sb_sw_le32_write(x, y)
#endif /* SB_SW_LE32_WRITE */

#ifndef SB_SW_CPU32_WRITE
#ifdef SB_CF_BIGENDIAN
#define SB_SW_CPU32_WRITE SB_SW_BE32_WRITE
#else
#define SB_SW_CPU32_WRITE SB_SW_LE32_WRITE
#endif
#endif /* SB_SW_CPU32_WRITE */

#ifdef SB_CF_BIGENDIAN
/* Big endian: derive SB_SW_?E32_READ from SB_SW_CPU32_READ */
#ifndef SB_SW_BE32_READ
#define SB_SW_BE32_READ SB_SW_CPU32_READ
#endif
#ifndef SB_SW_LE32_READ
#define SB_SW_LE32_READ(x) SB_SW_BYTEORDER_SWAP32(SB_SW_CPU32_READ(x))
#endif
#else
/* Little endian: derive SB_SW_?E32_READ from SB_SW_CPU32_READ */
#ifndef SB_SW_LE32_READ
#define SB_SW_LE32_READ SB_SW_CPU32_READ
#endif
#ifndef SB_SW_BE32_READ
#define SB_SW_BE32_READ(x) SB_SW_BYTEORDER_SWAP32(SB_SW_CPU32_READ(x))
#endif
#endif /* SB_CF_BIGENDIAN */

#ifndef SB_SW_LE16_READ
#define SB_SW_LE16_READ(x) \
    ((*(uint8_t*)(x)) | (((uint16_t)(((uint8_t*)x)[1])) << 8))
#endif /* SB_SW_LE16_READ*/

#ifndef SB_SW_BE16_READ
#define SB_SW_BE16_READ(x) \
    ((((uint16_t)(*(uint8_t*)(x))) << 8) | (((uint8_t*)x)[1]))
#endif /* SB_SW_BE16_READ*/

#ifndef SB_SW_CPU16_READ
#ifdef SB_CF_BIGENDIAN
#define SB_SW_CPU16_READ SB_SW_BE16_READ
#else
#define SB_SW_CPU16_READ SB_SW_LE16_READ
#endif
#endif /* SB_SW_CPU16_READ */

#ifndef SB_SW_BE16_WRITE
static inline void sb_sw_be16_write(void * const Value_p,
                                    const uint16_t NewValue)
{
    uint8_t * const Value8_p = (uint8_t *) Value_p;
    Value8_p[0] = NewValue >> 8;
    Value8_p[1] = (uint8_t) NewValue;
}
#define SB_SW_BE16_WRITE(x, y) sb_sw_be16_write(x, y)
#endif /* SB_SW_BE16_WRITE */

#ifndef SB_SW_LE16_WRITE
static inline void sb_sw_le16_write(void * const Value_p,
                                    const uint16_t NewValue)
{
    uint8_t * const Value8_p = (uint8_t *) Value_p;
    Value8_p[1] = NewValue >> 8;
    Value8_p[0] = (uint8_t) NewValue;
}
#define SB_SW_LE16_WRITE(x, y) sb_sw_le16_write(x, y)
#endif /* SB_SW_LE16_WRITE */

#ifndef SB_SW_CPU16_WRITE
#ifdef SB_CF_BIGENDIAN
#define SB_SW_CPU16_WRITE SB_SW_BE16_WRITE
#else
#define SB_SW_CPU16_WRITE SB_SW_LE16_WRITE
#endif
#endif /* SB_SW_CPU16_WRITE */

#endif /* unaligned access */

/* Macro for byte order swapping of constant values.
   Reason for having separate macro for constant values is that C compilers
   are able to perform these calculations during compilation time for
   constants. */
#define SB_SW_BYTEORDER_SWAP32_CONSTANT(x) \
  (((x) << 24) | \
   (((x) & 0x0000ff00U) << 8) | \
   (((x) & 0x00ff0000U) >> 8) | \
   ((x) >> 24))

/* Similar macro for half-word entities. */
#define SB_SW_BYTEORDER_SWAP16_CONSTANT(x) \
  ((uint16_t) (((x) << 8) | ((x) >> 8)))

#ifdef SB_CF_BIGENDIAN
/* Define word / byte conversion macros for big endian, constants. */

#define SB_SW_CPU_TO_BE32_CONSTANT(x) (x)
#define SB_SW_CPU_TO_LE32_CONSTANT(x) SB_SW_BYTEORDER_SWAP32_CONSTANT(x)
#define SB_SW_BE32_TO_CPU_CONSTANT(x) (x)
#define SB_SW_LE32_TO_CPU_CONSTANT(x) SB_SW_BYTEORDER_SWAP32_CONSTANT(x)

#define SB_SW_CPU_TO_BE16_CONSTANT(x) (x)
#define SB_SW_CPU_TO_LE16_CONSTANT(x) SB_SW_BYTEORDER_SWAP16_CONSTANT(x)
#define SB_SW_BE16_TO_CPU_CONSTANT(x) (x)
#define SB_SW_LE16_TO_CPU_CONSTANT(x) SB_SW_BYTEORDER_SWAP16_CONSTANT(x)
#else
/* Define word / byte conversion macros for little endian, constants. */

#define SB_SW_CPU_TO_LE32_CONSTANT(x) (x)
#define SB_SW_CPU_TO_BE32_CONSTANT(x) SB_SW_BYTEORDER_SWAP32_CONSTANT(x)
#define SB_SW_LE32_TO_CPU_CONSTANT(x) (x)
#define SB_SW_BE32_TO_CPU_CONSTANT(x) SB_SW_BYTEORDER_SWAP32_CONSTANT(x)

#define SB_SW_CPU_TO_LE16_CONSTANT(x) (x)
#define SB_SW_CPU_TO_BE16_CONSTANT(x) SB_SW_BYTEORDER_SWAP16_CONSTANT(x)
#define SB_SW_LE16_TO_CPU_CONSTANT(x) (x)
#define SB_SW_BE16_TO_CPU_CONSTANT(x) SB_SW_BYTEORDER_SWAP16_CONSTANT(x)
#endif

#endif /* _SB_SW_API_H_ */
