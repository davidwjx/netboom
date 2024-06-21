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
--  Abstract : The FLEXA common type definitions
--
-----------------------------------------------------------------------------*/

/**
 * @file flexatype.h
 * @brief The FLEXA common type definitions
 *
 * This file contains the "FLEXA common type definitions".
 */

#ifndef FLEXATYPE_H_
#define FLEXATYPE_H_

//#include <stdint.h>
//#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL 0
#endif

#if !defined(CONFIG_FIXED_WIDTH_TYPE)
typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
//typedef long long int64_t;

typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
//typedef unsigned long long uint64_t;
#endif

typedef int8_t    i8;
typedef uint8_t   u8;
typedef int16_t   i16;
typedef uint16_t  u16;
typedef int32_t   i32;
typedef uint32_t  u32;
typedef int64_t   i64;
typedef uint64_t  u64;

typedef unsigned int    atomic_uint;

// #ifdef M64
// typedef unsigned long long addr_t;
// #else
// typedef unsigned int addr_t;
// #endif

typedef void *FLEXA_Handle;

/** Status values returned by each of FLEXA API */
typedef enum {
    FLEXA_STATUS_SUCCESS = 0, /**<  A FLEXA API function call is successful. */
    FLEXA_STATUS_ERROR = -1, /**<  General undefined error. */
    FLEXA_STATUS_NO_SUPPORT = -2, /**<  A FLEXA API function call requires unsupported FLEXA API features. */
    FLEXA_STATUS_INVALID_STREAM = -3, /**<  A FLEXA API function call is requesting an invalid or uninitialized stream. */
    FLEXA_STATUS_INVALID_VALUE = -4, /**<  A FLEXA API function call has an invalid value in the configuration to set. */
    FLEXA_STATUS_TYPE_MISMATCH = -5, /**<  A FLEXA API function call uses a data structure that is mismatched to the specified type. */
    FLEXA_STATUS_OUT_OF_MEMORY = -6, /**<  A FLEXA API function call fails to alloc memory. */
    FLEXA_STATUS_NULL_PTR = -7, /**<  A FLEXA API function call accesses a null pointer. */
    FLEXA_STATUS_NO_MORE_DATA = -8, /**< A FLEXA API function call call a queue. */
} FLEXA_Status;

#ifdef __cplusplus
}
#endif

#endif /* FLEXATYPE_H_ */
