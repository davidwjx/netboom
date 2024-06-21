/**
	@file   zr_macros.h

	@brief  Common macros

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

#ifndef _ZR_MACROS_H_
#define _ZR_MACROS_H_

#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************
 * pre-processor tricks
 *****************************************************************************/
#define STRINGIFY(x) DO_STRINGIFY(x)
#define DO_STRINGIFY(x...) #x

#define ARGUSE(x) (x);

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

/******************************************************************************
 * Registers access
 *****************************************************************************/
#define ADDR16(addr)    ((volatile uint16 *) (addr))
#define ADDR32(addr)    ((volatile uint32 *) (addr))
#define REG8(addr)      (*((volatile uint8 *) (addr)))
#define REG16(addr)     (*((volatile uint16 *) (addr)))
#define REG32(addr)     (*((volatile uint32 *) (addr)))

#define REG32_SET(addr, val)    (REG32(addr) = (val))

#define BIT_N(n)    (1 << n)

#ifndef USB_FOR_VDK
//! Bit
#define BIT(BitOffset)      ((uint32)1 << (BitOffset))
#endif

//! Sets a bit in the pointed address
#define SET_BIT(pAddr,bitOffset)       (*(pAddr) |=  (1<<(bitOffset)))

//! Sets a bits in the pointed address
#define SET_BITS(pAddr, mask, offset)       (*(pAddr) |=  (mask<<(offset)))

//! Clears a bit in the pointed address
#define CLEAR_BIT(pAddr,bitOffset)     (*(pAddr) &= ~(1<<(bitOffset)))

//! Clears a bits in the pointed address
#define CLEAR_BITS(pAddr, mask, offset)     (*(pAddr) &= ~(mask<<(offset)))

//! Returns the value of a specific bit (0 or 1) shifted to bit zero
#define GET_BIT_VAL(pAddr,bitOffset)    ((*(pAddr) >> (bitOffset)) & 0x1)

//! Returns the value of a specific bit (0 or 1) shifted to bit zero
#define GET_BITS_VAL(pAddr, mask, offset)    ((*(pAddr) >> (offset)) & mask)

/**!
	Modify bit values in a register.  Using the
	algorithm:    (reg_contents & ~clear_mask) | set_mask
 */
#define REG32_MODIFY(addr, clear_mask, set_mask)    \
	(REG32(addr) = ( ( REG32(addr) & ~clear_mask ) | set_mask ))

/******************************************************************************
 * Math functions
 *****************************************************************************/
//! Minimum
#ifndef MIN
#define MIN(a,b)        (((a)<(b))?(a):(b))
#endif
//! Maximum
#ifndef MAX
#define MAX(a,b)        (((a)>(b))?(a):(b))
#endif
//! Absolute
#ifndef ABS
#define ABS(x)      ( (x>=0) ? (x) : (-1*(x)) )
#endif

//! Align to DWORD
#define ALIGN_DWORD(len)        ((len + 3)/4)*4

#define ROUND(x)        ((int)(x < 0 ? (x - 0.5) : (x + 0.5)))

//! Division
#define DIV(x, y) ((x) / (y))
#define MOD(x, y) ((x) % (y))

/******************************************************************************
 * CPU special opcodes
 *****************************************************************************/
//! Wait for interrupt
#ifndef WAIT_FOR_INT
#define WAIT_FOR_INT(x) asm("wfi")
#endif

//! Wait for some kind of deep sleeping operation to take effect
#define WAIT_FOR_SLEEP() while(1) WAIT_FOR_INT(0)

//#define ZR_NOP          asm("nop")

/******************************************************************************
 * Toolchain special directives
 *****************************************************************************/
//! What is the build tool set for some common qualifiers?
//#define ZR_INLINE       __inline__
//#define ZR_INTERRUPT    _Interrupt1
//#define ZR_UNLIKELY     _Rarely
//#define ZR_LIKELY       _Usually
#define ZR_KEEP_ORDER   asm("")

//#ifndef PACKED
//#define PACKED          _Packed
//#endif

#define ZR_printf       printf

#ifdef __cplusplus
}
#endif
#endif /* _ZR_MACROS_H_ */
