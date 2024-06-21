#ifndef NU4000_B0_HYB_REG_H
#define NU4000_B0_HYB_REG_H


/* types */
#include "defs.h"

#if !defined(PREFIX_VAL)
#define PREFIX_VAL *
#endif
 
#if !defined(SetGroupBits32) 
#define SetGroupBits32(reg,position,len,value)\
{       \
        UINT32 tmp=reg;\
        tmp&=~(((UINT32)0xFFFFFFFF>>(32-(len)))<<(position));\
        tmp|=((value&((UINT32)0xFFFFFFFF>>(32-(len))))<<(position));\
        reg=tmp;\
}
#endif

#if !defined(GetGroupBits32)
#define GetGroupBits32(reg,position,len) (((reg) & (((UINT32)0xFFFFFFFF>>(32-(len)))<<(position)))>>(position))
#endif


/*****************************************/
/*   HYB (Prototype: PPE_HYB)                */
/*****************************************/
#define HYB_BASE 0x00000000

/* REGISTER: CFG */

#if defined(_V1) && !defined(CFG_OFFSET)
#define CFG_OFFSET 0x00
#endif

#if !defined(PPE_HYB_CFG_OFFSET)
#define PPE_HYB_CFG_OFFSET 0x00
#endif

#if defined(_V1) && !defined(CFG_REG)
#define CFG_REG ((volatile UINT32 *) (HYB_BASE + PPE_HYB_CFG_OFFSET))
#endif

#if defined(_V1) && !defined(CFG_VAL)
#define CFG_VAL  PREFIX_VAL(CFG_REG)
#endif

#define HYB_CFG_REG ((volatile UINT32 *) (HYB_BASE + PPE_HYB_CFG_OFFSET))
#define HYB_CFG_VAL  PREFIX_VAL(HYB_CFG_REG)

/* FIELDS: */

/* bypass */

#ifndef PPE_HYB_CFG_BYPASS_POS
#define PPE_HYB_CFG_BYPASS_POS      24
#endif

#ifndef PPE_HYB_CFG_BYPASS_LEN
#define PPE_HYB_CFG_BYPASS_LEN      1
#endif

#if defined(_V1) && !defined(CFG_BYPASS_R)
#define CFG_BYPASS_R        GetGroupBits32( (HYB_CFG_VAL),PPE_HYB_CFG_BYPASS_POS, PPE_HYB_CFG_BYPASS_LEN)
#endif

#if defined(_V1) && !defined(CFG_BYPASS_W)
#define CFG_BYPASS_W(value) SetGroupBits32( (HYB_CFG_VAL),PPE_HYB_CFG_BYPASS_POS, PPE_HYB_CFG_BYPASS_LEN,value)
#endif

#define HYB_CFG_BYPASS_R        GetGroupBits32( (HYB_CFG_VAL),PPE_HYB_CFG_BYPASS_POS, PPE_HYB_CFG_BYPASS_LEN)

#define HYB_CFG_BYPASS_W(value) SetGroupBits32( (HYB_CFG_VAL),PPE_HYB_CFG_BYPASS_POS, PPE_HYB_CFG_BYPASS_LEN,value)


/* p0_end */

#ifndef PPE_HYB_CFG_P0_END_POS
#define PPE_HYB_CFG_P0_END_POS      0
#endif

#ifndef PPE_HYB_CFG_P0_END_LEN
#define PPE_HYB_CFG_P0_END_LEN      11
#endif

#if defined(_V1) && !defined(CFG_P0_END_R)
#define CFG_P0_END_R        GetGroupBits32( (HYB_CFG_VAL),PPE_HYB_CFG_P0_END_POS, PPE_HYB_CFG_P0_END_LEN)
#endif

#if defined(_V1) && !defined(CFG_P0_END_W)
#define CFG_P0_END_W(value) SetGroupBits32( (HYB_CFG_VAL),PPE_HYB_CFG_P0_END_POS, PPE_HYB_CFG_P0_END_LEN,value)
#endif

#define HYB_CFG_P0_END_R        GetGroupBits32( (HYB_CFG_VAL),PPE_HYB_CFG_P0_END_POS, PPE_HYB_CFG_P0_END_LEN)

#define HYB_CFG_P0_END_W(value) SetGroupBits32( (HYB_CFG_VAL),PPE_HYB_CFG_P0_END_POS, PPE_HYB_CFG_P0_END_LEN,value)


/* p1_end */

#ifndef PPE_HYB_CFG_P1_END_POS
#define PPE_HYB_CFG_P1_END_POS      12
#endif

#ifndef PPE_HYB_CFG_P1_END_LEN
#define PPE_HYB_CFG_P1_END_LEN      11
#endif

#if defined(_V1) && !defined(CFG_P1_END_R)
#define CFG_P1_END_R        GetGroupBits32( (HYB_CFG_VAL),PPE_HYB_CFG_P1_END_POS, PPE_HYB_CFG_P1_END_LEN)
#endif

#if defined(_V1) && !defined(CFG_P1_END_W)
#define CFG_P1_END_W(value) SetGroupBits32( (HYB_CFG_VAL),PPE_HYB_CFG_P1_END_POS, PPE_HYB_CFG_P1_END_LEN,value)
#endif

#define HYB_CFG_P1_END_R        GetGroupBits32( (HYB_CFG_VAL),PPE_HYB_CFG_P1_END_POS, PPE_HYB_CFG_P1_END_LEN)

#define HYB_CFG_P1_END_W(value) SetGroupBits32( (HYB_CFG_VAL),PPE_HYB_CFG_P1_END_POS, PPE_HYB_CFG_P1_END_LEN,value)


/* REGISTER: PAD_CFG */

#if defined(_V1) && !defined(PAD_CFG_OFFSET)
#define PAD_CFG_OFFSET 0x04
#endif

#if !defined(PPE_HYB_PAD_CFG_OFFSET)
#define PPE_HYB_PAD_CFG_OFFSET 0x04
#endif

#if defined(_V1) && !defined(PAD_CFG_REG)
#define PAD_CFG_REG ((volatile UINT32 *) (HYB_BASE + PPE_HYB_PAD_CFG_OFFSET))
#endif

#if defined(_V1) && !defined(PAD_CFG_VAL)
#define PAD_CFG_VAL  PREFIX_VAL(PAD_CFG_REG)
#endif

#define HYB_PAD_CFG_REG ((volatile UINT32 *) (HYB_BASE + PPE_HYB_PAD_CFG_OFFSET))
#define HYB_PAD_CFG_VAL  PREFIX_VAL(HYB_PAD_CFG_REG)

/* FIELDS: */

/* pad_val */

#ifndef PPE_HYB_PAD_CFG_PAD_VAL_POS
#define PPE_HYB_PAD_CFG_PAD_VAL_POS      0
#endif

#ifndef PPE_HYB_PAD_CFG_PAD_VAL_LEN
#define PPE_HYB_PAD_CFG_PAD_VAL_LEN      24
#endif

#if defined(_V1) && !defined(PAD_CFG_PAD_VAL_R)
#define PAD_CFG_PAD_VAL_R        GetGroupBits32( (HYB_PAD_CFG_VAL),PPE_HYB_PAD_CFG_PAD_VAL_POS, PPE_HYB_PAD_CFG_PAD_VAL_LEN)
#endif

#if defined(_V1) && !defined(PAD_CFG_PAD_VAL_W)
#define PAD_CFG_PAD_VAL_W(value) SetGroupBits32( (HYB_PAD_CFG_VAL),PPE_HYB_PAD_CFG_PAD_VAL_POS, PPE_HYB_PAD_CFG_PAD_VAL_LEN,value)
#endif

#define HYB_PAD_CFG_PAD_VAL_R        GetGroupBits32( (HYB_PAD_CFG_VAL),PPE_HYB_PAD_CFG_PAD_VAL_POS, PPE_HYB_PAD_CFG_PAD_VAL_LEN)

#define HYB_PAD_CFG_PAD_VAL_W(value) SetGroupBits32( (HYB_PAD_CFG_VAL),PPE_HYB_PAD_CFG_PAD_VAL_POS, PPE_HYB_PAD_CFG_PAD_VAL_LEN,value)


/* REGISTER: HYB0 */

#if defined(_V1) && !defined(HYB0_OFFSET)
#define HYB0_OFFSET 0x18
#endif

#if !defined(PPE_HYB_HYB0_OFFSET)
#define PPE_HYB_HYB0_OFFSET 0x18
#endif

#if defined(_V1) && !defined(HYB0_REG)
#define HYB0_REG ((volatile UINT32 *) (HYB_BASE + PPE_HYB_HYB0_OFFSET))
#endif

#if defined(_V1) && !defined(HYB0_VAL)
#define HYB0_VAL  PREFIX_VAL(HYB0_REG)
#endif

#define HYB_HYB0_REG ((volatile UINT32 *) (HYB_BASE + PPE_HYB_HYB0_OFFSET))
#define HYB_HYB0_VAL  PREFIX_VAL(HYB_HYB0_REG)

/* FIELDS: */

/* start */

#ifndef PPE_HYB_HYB0_START_POS
#define PPE_HYB_HYB0_START_POS      0
#endif

#ifndef PPE_HYB_HYB0_START_LEN
#define PPE_HYB_HYB0_START_LEN      11
#endif

#if defined(_V1) && !defined(HYB0_START_R)
#define HYB0_START_R        GetGroupBits32( (HYB_HYB0_VAL),PPE_HYB_HYB0_START_POS, PPE_HYB_HYB0_START_LEN)
#endif

#if defined(_V1) && !defined(HYB0_START_W)
#define HYB0_START_W(value) SetGroupBits32( (HYB_HYB0_VAL),PPE_HYB_HYB0_START_POS, PPE_HYB_HYB0_START_LEN,value)
#endif

#define HYB_HYB0_START_R        GetGroupBits32( (HYB_HYB0_VAL),PPE_HYB_HYB0_START_POS, PPE_HYB_HYB0_START_LEN)

#define HYB_HYB0_START_W(value) SetGroupBits32( (HYB_HYB0_VAL),PPE_HYB_HYB0_START_POS, PPE_HYB_HYB0_START_LEN,value)


/* stop */

#ifndef PPE_HYB_HYB0_STOP_POS
#define PPE_HYB_HYB0_STOP_POS      16
#endif

#ifndef PPE_HYB_HYB0_STOP_LEN
#define PPE_HYB_HYB0_STOP_LEN      11
#endif

#if defined(_V1) && !defined(HYB0_STOP_R)
#define HYB0_STOP_R        GetGroupBits32( (HYB_HYB0_VAL),PPE_HYB_HYB0_STOP_POS, PPE_HYB_HYB0_STOP_LEN)
#endif

#if defined(_V1) && !defined(HYB0_STOP_W)
#define HYB0_STOP_W(value) SetGroupBits32( (HYB_HYB0_VAL),PPE_HYB_HYB0_STOP_POS, PPE_HYB_HYB0_STOP_LEN,value)
#endif

#define HYB_HYB0_STOP_R        GetGroupBits32( (HYB_HYB0_VAL),PPE_HYB_HYB0_STOP_POS, PPE_HYB_HYB0_STOP_LEN)

#define HYB_HYB0_STOP_W(value) SetGroupBits32( (HYB_HYB0_VAL),PPE_HYB_HYB0_STOP_POS, PPE_HYB_HYB0_STOP_LEN,value)


/* REGISTER: HYB1 */

#if defined(_V1) && !defined(HYB1_OFFSET)
#define HYB1_OFFSET 0x1C
#endif

#if !defined(PPE_HYB_HYB1_OFFSET)
#define PPE_HYB_HYB1_OFFSET 0x1C
#endif

#if defined(_V1) && !defined(HYB1_REG)
#define HYB1_REG ((volatile UINT32 *) (HYB_BASE + PPE_HYB_HYB1_OFFSET))
#endif

#if defined(_V1) && !defined(HYB1_VAL)
#define HYB1_VAL  PREFIX_VAL(HYB1_REG)
#endif

#define HYB_HYB1_REG ((volatile UINT32 *) (HYB_BASE + PPE_HYB_HYB1_OFFSET))
#define HYB_HYB1_VAL  PREFIX_VAL(HYB_HYB1_REG)

/* FIELDS: */

/* start */

#ifndef PPE_HYB_HYB1_START_POS
#define PPE_HYB_HYB1_START_POS      0
#endif

#ifndef PPE_HYB_HYB1_START_LEN
#define PPE_HYB_HYB1_START_LEN      11
#endif

#if defined(_V1) && !defined(HYB1_START_R)
#define HYB1_START_R        GetGroupBits32( (HYB_HYB1_VAL),PPE_HYB_HYB1_START_POS, PPE_HYB_HYB1_START_LEN)
#endif

#if defined(_V1) && !defined(HYB1_START_W)
#define HYB1_START_W(value) SetGroupBits32( (HYB_HYB1_VAL),PPE_HYB_HYB1_START_POS, PPE_HYB_HYB1_START_LEN,value)
#endif

#define HYB_HYB1_START_R        GetGroupBits32( (HYB_HYB1_VAL),PPE_HYB_HYB1_START_POS, PPE_HYB_HYB1_START_LEN)

#define HYB_HYB1_START_W(value) SetGroupBits32( (HYB_HYB1_VAL),PPE_HYB_HYB1_START_POS, PPE_HYB_HYB1_START_LEN,value)


/* stop */

#ifndef PPE_HYB_HYB1_STOP_POS
#define PPE_HYB_HYB1_STOP_POS      16
#endif

#ifndef PPE_HYB_HYB1_STOP_LEN
#define PPE_HYB_HYB1_STOP_LEN      11
#endif

#if defined(_V1) && !defined(HYB1_STOP_R)
#define HYB1_STOP_R        GetGroupBits32( (HYB_HYB1_VAL),PPE_HYB_HYB1_STOP_POS, PPE_HYB_HYB1_STOP_LEN)
#endif

#if defined(_V1) && !defined(HYB1_STOP_W)
#define HYB1_STOP_W(value) SetGroupBits32( (HYB_HYB1_VAL),PPE_HYB_HYB1_STOP_POS, PPE_HYB_HYB1_STOP_LEN,value)
#endif

#define HYB_HYB1_STOP_R        GetGroupBits32( (HYB_HYB1_VAL),PPE_HYB_HYB1_STOP_POS, PPE_HYB_HYB1_STOP_LEN)

#define HYB_HYB1_STOP_W(value) SetGroupBits32( (HYB_HYB1_VAL),PPE_HYB_HYB1_STOP_POS, PPE_HYB_HYB1_STOP_LEN,value)


/* OFFSET TABLE: */
#define hyb_offset_tbl_values	CFG_OFFSET, PAD_CFG_OFFSET, HYB0_OFFSET, HYB1_OFFSET


/* REGISTERS RESET VAL: */
#define hyb_regs_reset_val	0x0, 0x0, 0x0, 0x0

#endif

/* End of HYB */
/* ///////////////////////////////////////////////////////////////////////*/
