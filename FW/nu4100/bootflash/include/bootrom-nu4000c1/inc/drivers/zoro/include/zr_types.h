/**
	@file   zr_types.h

	@brief  Common used types declarations

	@date September, 2010

	@author Uri Shkolnik, Zoro Solutions Ltd

	<b> Copyright (c) 2010-2013 Zoro Solutions Ltd. </b>\n
	43 Hamelacha street, P.O. Box 8786, Poleg Industrial Park, Netanaya, ZIP 42505 Israel\n
	All rights reserved\n\n
	Proprietary rights of Zoro Solutions Ltd are involved in the
	subject matter of this material. All manufacturing, reproduction,
	use, and sales rights pertaining to this subject matter are governed
	by the license agreement. The recipient of this software implicitly
	accepts the terms of the license. This source code is the unpublished
	property and trade secret of Zoro Solutions Ltd.
	It is to be utilized solely under license from Zoro Solutions Ltd and it
	is to be maintained on a confidential basis for internal company use
	only. It is to be protected from disclosure to unauthorized parties,
	both within the Licensee company and outside, in a manner not less stringent
	than that utilized for Licensee's own proprietary internal information.
	No copies of the source or object code are to leave the premises of
	Licensee's business except in strict accordance with the license
	agreement signed by Licensee with Zoro Solutions Ltd.\n\n

	For more details - http://zoro-sw.com
	email: info@zoro-sw.com
*/

#ifndef _ZR_TYPES_H
#define _ZR_TYPES_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "limits.h"
#ifdef USB_FOR_VDK
#include <stdint.h>
#include "ArmTypes.h"
#endif

/** Our favourites */

#if !(defined(WIN32) || defined(USB_FOR_VDK))
typedef unsigned long long      uint64; //!< 64 bit, unsigned signed integer
typedef long long               int64;  //!< 64 bit, signed integer
#endif /* WIN32 */
#ifndef USB_FOR_VDK
typedef unsigned int            uint32; //!< 32 bit, unsigned signed integer
typedef int                     int32;  //!< 32 bit, signed integer
typedef unsigned short          uint16; //!< 32 bit, unsigned signed integer
typedef short                   int16;  //!< 16 bit, signed integer
typedef unsigned char           uint8;  //!< 8 bit, unsigned signed integer
typedef signed char             int8;   //!< 8 bit, signed integer
typedef unsigned char           uchar;  //!< Used for ASCII characters only. not for values of 8 bits.
#endif
#ifndef TRUE
typedef enum {FALSE=0,TRUE=1} boolean;  //!< boolean values
#endif
#if !(defined(WIN32) || defined(USB_FOR_VDK))
#ifndef BOOL_DEFINED
#define BOOL_DEFINED
typedef unsigned long           bool;   //!< should use only values FALSE, or TRUE.
#endif
#endif /* WIN32 */


/* bsd */
typedef unsigned char           u_char;
typedef unsigned short          u_short;
typedef unsigned int            u_int;
typedef unsigned long           u_long;

/* sysv */
typedef unsigned char           unchar;
typedef unsigned short          ushort;
#ifndef USB_FOR_VDK
typedef unsigned int            uint;
#endif
typedef unsigned long           ulong;

typedef signed char __s8;
typedef unsigned char __u8;

typedef signed short __s16;
typedef unsigned short __u16;

typedef signed int __s32;
typedef unsigned int __u32;

#if 0
typedef         __u8            u_int8_t;
typedef         __s8            int8_t;
typedef         __u16           u_int16_t;
typedef         __s16           int16_t;
typedef         __u32           u_int32_t;
typedef         __s32           int32_t;

typedef         __u8            uint8_t;
typedef         __u16           uint16_t;
typedef         __u32           uint32_t;

/** For 3rd parties source code */
typedef uint32                  u32;
typedef uint16                  u16;
typedef uint8                   u8;
#endif

#ifndef __ssize_t_defined
typedef int                     ssize_t;
#endif
#if !(defined(_STDDEF_H_) || defined(size_t) || defined(_SIZE_T_) || defined(USB_FOR_VDK))
typedef unsigned int            size_t;
#endif
typedef int                     gfp_t;
typedef int                     irqreturn_t;
typedef uint32                  dma_addr_t;
typedef void*                   spinlock_t;



#ifndef NULL
	#define NULL    0
#endif

#ifndef MAX_LONG
	#define MAX_LONG    0xFFFFFFFF
#endif

#ifdef __cplusplus
}
#endif

#endif /* _ZR_TYPES_H */
