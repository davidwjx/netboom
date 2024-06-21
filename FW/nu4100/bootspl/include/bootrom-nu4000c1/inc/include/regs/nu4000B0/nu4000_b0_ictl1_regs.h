#ifndef NU4000_B0_ICTL1_REG_H
#define NU4000_B0_ICTL1_REG_H


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
/*   ICTL1 (Prototype: ICTL)                */
/*****************************************/
#define ICTL1_BASE 0x08120000

/* REGISTER: IRQ_INTEN_L */

#if defined(_V1) && !defined(IRQ_INTEN_L_OFFSET)
#define IRQ_INTEN_L_OFFSET 0x0
#endif

#if !defined(ICTL_IRQ_INTEN_L_OFFSET)
#define ICTL_IRQ_INTEN_L_OFFSET 0x0
#endif

#if defined(_V1) && !defined(IRQ_INTEN_L_REG)
#define IRQ_INTEN_L_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_INTEN_L_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_INTEN_L_VAL)
#define IRQ_INTEN_L_VAL  PREFIX_VAL(IRQ_INTEN_L_REG)
#endif

#define ICTL1_IRQ_INTEN_L_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_INTEN_L_OFFSET))
#define ICTL1_IRQ_INTEN_L_VAL  PREFIX_VAL(ICTL1_IRQ_INTEN_L_REG)

/* FIELDS: */

/* IRQ_INTEN_L */

#ifndef ICTL_IRQ_INTEN_L_IRQ_INTEN_L_POS
#define ICTL_IRQ_INTEN_L_IRQ_INTEN_L_POS      0
#endif

#ifndef ICTL_IRQ_INTEN_L_IRQ_INTEN_L_LEN
#define ICTL_IRQ_INTEN_L_IRQ_INTEN_L_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_INTEN_L_IRQ_INTEN_L_R)
#define IRQ_INTEN_L_IRQ_INTEN_L_R        GetGroupBits32( (ICTL1_IRQ_INTEN_L_VAL),ICTL_IRQ_INTEN_L_IRQ_INTEN_L_POS, ICTL_IRQ_INTEN_L_IRQ_INTEN_L_LEN)
#endif

#if defined(_V1) && !defined(IRQ_INTEN_L_IRQ_INTEN_L_W)
#define IRQ_INTEN_L_IRQ_INTEN_L_W(value) SetGroupBits32( (ICTL1_IRQ_INTEN_L_VAL),ICTL_IRQ_INTEN_L_IRQ_INTEN_L_POS, ICTL_IRQ_INTEN_L_IRQ_INTEN_L_LEN,value)
#endif

#define ICTL1_IRQ_INTEN_L_IRQ_INTEN_L_R        GetGroupBits32( (ICTL1_IRQ_INTEN_L_VAL),ICTL_IRQ_INTEN_L_IRQ_INTEN_L_POS, ICTL_IRQ_INTEN_L_IRQ_INTEN_L_LEN)

#define ICTL1_IRQ_INTEN_L_IRQ_INTEN_L_W(value) SetGroupBits32( (ICTL1_IRQ_INTEN_L_VAL),ICTL_IRQ_INTEN_L_IRQ_INTEN_L_POS, ICTL_IRQ_INTEN_L_IRQ_INTEN_L_LEN,value)


/* REGISTER: IRQ_INTFORCE_L */

#if defined(_V1) && !defined(IRQ_INTFORCE_L_OFFSET)
#define IRQ_INTFORCE_L_OFFSET 0x10
#endif

#if !defined(ICTL_IRQ_INTFORCE_L_OFFSET)
#define ICTL_IRQ_INTFORCE_L_OFFSET 0x10
#endif

#if defined(_V1) && !defined(IRQ_INTFORCE_L_REG)
#define IRQ_INTFORCE_L_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_INTFORCE_L_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_INTFORCE_L_VAL)
#define IRQ_INTFORCE_L_VAL  PREFIX_VAL(IRQ_INTFORCE_L_REG)
#endif

#define ICTL1_IRQ_INTFORCE_L_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_INTFORCE_L_OFFSET))
#define ICTL1_IRQ_INTFORCE_L_VAL  PREFIX_VAL(ICTL1_IRQ_INTFORCE_L_REG)

/* FIELDS: */

/* IRQ_INTFORCE_L */

#ifndef ICTL_IRQ_INTFORCE_L_IRQ_INTFORCE_L_POS
#define ICTL_IRQ_INTFORCE_L_IRQ_INTFORCE_L_POS      0
#endif

#ifndef ICTL_IRQ_INTFORCE_L_IRQ_INTFORCE_L_LEN
#define ICTL_IRQ_INTFORCE_L_IRQ_INTFORCE_L_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_INTFORCE_L_IRQ_INTFORCE_L_R)
#define IRQ_INTFORCE_L_IRQ_INTFORCE_L_R        GetGroupBits32( (ICTL1_IRQ_INTFORCE_L_VAL),ICTL_IRQ_INTFORCE_L_IRQ_INTFORCE_L_POS, ICTL_IRQ_INTFORCE_L_IRQ_INTFORCE_L_LEN)
#endif

#if defined(_V1) && !defined(IRQ_INTFORCE_L_IRQ_INTFORCE_L_W)
#define IRQ_INTFORCE_L_IRQ_INTFORCE_L_W(value) SetGroupBits32( (ICTL1_IRQ_INTFORCE_L_VAL),ICTL_IRQ_INTFORCE_L_IRQ_INTFORCE_L_POS, ICTL_IRQ_INTFORCE_L_IRQ_INTFORCE_L_LEN,value)
#endif

#define ICTL1_IRQ_INTFORCE_L_IRQ_INTFORCE_L_R        GetGroupBits32( (ICTL1_IRQ_INTFORCE_L_VAL),ICTL_IRQ_INTFORCE_L_IRQ_INTFORCE_L_POS, ICTL_IRQ_INTFORCE_L_IRQ_INTFORCE_L_LEN)

#define ICTL1_IRQ_INTFORCE_L_IRQ_INTFORCE_L_W(value) SetGroupBits32( (ICTL1_IRQ_INTFORCE_L_VAL),ICTL_IRQ_INTFORCE_L_IRQ_INTFORCE_L_POS, ICTL_IRQ_INTFORCE_L_IRQ_INTFORCE_L_LEN,value)


/* REGISTER: IRQ_PR_6 */

#if defined(_V1) && !defined(IRQ_PR_6_OFFSET)
#define IRQ_PR_6_OFFSET 0x100
#endif

#if !defined(ICTL_IRQ_PR_6_OFFSET)
#define ICTL_IRQ_PR_6_OFFSET 0x100
#endif

#if defined(_V1) && !defined(IRQ_PR_6_REG)
#define IRQ_PR_6_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_6_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_6_VAL)
#define IRQ_PR_6_VAL  PREFIX_VAL(IRQ_PR_6_REG)
#endif

#define ICTL1_IRQ_PR_6_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_6_OFFSET))
#define ICTL1_IRQ_PR_6_VAL  PREFIX_VAL(ICTL1_IRQ_PR_6_REG)

/* FIELDS: */

/* irq_pr_6 */

#ifndef ICTL_IRQ_PR_6_IRQ_PR_6_POS
#define ICTL_IRQ_PR_6_IRQ_PR_6_POS      0
#endif

#ifndef ICTL_IRQ_PR_6_IRQ_PR_6_LEN
#define ICTL_IRQ_PR_6_IRQ_PR_6_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_6_IRQ_PR_6_R)
#define IRQ_PR_6_IRQ_PR_6_R        GetGroupBits32( (ICTL1_IRQ_PR_6_VAL),ICTL_IRQ_PR_6_IRQ_PR_6_POS, ICTL_IRQ_PR_6_IRQ_PR_6_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_6_IRQ_PR_6_W)
#define IRQ_PR_6_IRQ_PR_6_W(value) SetGroupBits32( (ICTL1_IRQ_PR_6_VAL),ICTL_IRQ_PR_6_IRQ_PR_6_POS, ICTL_IRQ_PR_6_IRQ_PR_6_LEN,value)
#endif

#define ICTL1_IRQ_PR_6_IRQ_PR_6_R        GetGroupBits32( (ICTL1_IRQ_PR_6_VAL),ICTL_IRQ_PR_6_IRQ_PR_6_POS, ICTL_IRQ_PR_6_IRQ_PR_6_LEN)

#define ICTL1_IRQ_PR_6_IRQ_PR_6_W(value) SetGroupBits32( (ICTL1_IRQ_PR_6_VAL),ICTL_IRQ_PR_6_IRQ_PR_6_POS, ICTL_IRQ_PR_6_IRQ_PR_6_LEN,value)


/* RSVD_irq_pr_6 */

#ifndef ICTL_IRQ_PR_6_RSVD_IRQ_PR_6_POS
#define ICTL_IRQ_PR_6_RSVD_IRQ_PR_6_POS      4
#endif

#ifndef ICTL_IRQ_PR_6_RSVD_IRQ_PR_6_LEN
#define ICTL_IRQ_PR_6_RSVD_IRQ_PR_6_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_6_RSVD_IRQ_PR_6_R)
#define IRQ_PR_6_RSVD_IRQ_PR_6_R        GetGroupBits32( (ICTL1_IRQ_PR_6_VAL),ICTL_IRQ_PR_6_RSVD_IRQ_PR_6_POS, ICTL_IRQ_PR_6_RSVD_IRQ_PR_6_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_6_RSVD_IRQ_PR_6_W)
#define IRQ_PR_6_RSVD_IRQ_PR_6_W(value) SetGroupBits32( (ICTL1_IRQ_PR_6_VAL),ICTL_IRQ_PR_6_RSVD_IRQ_PR_6_POS, ICTL_IRQ_PR_6_RSVD_IRQ_PR_6_LEN,value)
#endif

#define ICTL1_IRQ_PR_6_RSVD_IRQ_PR_6_R        GetGroupBits32( (ICTL1_IRQ_PR_6_VAL),ICTL_IRQ_PR_6_RSVD_IRQ_PR_6_POS, ICTL_IRQ_PR_6_RSVD_IRQ_PR_6_LEN)

#define ICTL1_IRQ_PR_6_RSVD_IRQ_PR_6_W(value) SetGroupBits32( (ICTL1_IRQ_PR_6_VAL),ICTL_IRQ_PR_6_RSVD_IRQ_PR_6_POS, ICTL_IRQ_PR_6_RSVD_IRQ_PR_6_LEN,value)


/* REGISTER: IRQ_PR_7 */

#if defined(_V1) && !defined(IRQ_PR_7_OFFSET)
#define IRQ_PR_7_OFFSET 0x104
#endif

#if !defined(ICTL_IRQ_PR_7_OFFSET)
#define ICTL_IRQ_PR_7_OFFSET 0x104
#endif

#if defined(_V1) && !defined(IRQ_PR_7_REG)
#define IRQ_PR_7_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_7_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_7_VAL)
#define IRQ_PR_7_VAL  PREFIX_VAL(IRQ_PR_7_REG)
#endif

#define ICTL1_IRQ_PR_7_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_7_OFFSET))
#define ICTL1_IRQ_PR_7_VAL  PREFIX_VAL(ICTL1_IRQ_PR_7_REG)

/* FIELDS: */

/* irq_pr_7 */

#ifndef ICTL_IRQ_PR_7_IRQ_PR_7_POS
#define ICTL_IRQ_PR_7_IRQ_PR_7_POS      0
#endif

#ifndef ICTL_IRQ_PR_7_IRQ_PR_7_LEN
#define ICTL_IRQ_PR_7_IRQ_PR_7_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_7_IRQ_PR_7_R)
#define IRQ_PR_7_IRQ_PR_7_R        GetGroupBits32( (ICTL1_IRQ_PR_7_VAL),ICTL_IRQ_PR_7_IRQ_PR_7_POS, ICTL_IRQ_PR_7_IRQ_PR_7_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_7_IRQ_PR_7_W)
#define IRQ_PR_7_IRQ_PR_7_W(value) SetGroupBits32( (ICTL1_IRQ_PR_7_VAL),ICTL_IRQ_PR_7_IRQ_PR_7_POS, ICTL_IRQ_PR_7_IRQ_PR_7_LEN,value)
#endif

#define ICTL1_IRQ_PR_7_IRQ_PR_7_R        GetGroupBits32( (ICTL1_IRQ_PR_7_VAL),ICTL_IRQ_PR_7_IRQ_PR_7_POS, ICTL_IRQ_PR_7_IRQ_PR_7_LEN)

#define ICTL1_IRQ_PR_7_IRQ_PR_7_W(value) SetGroupBits32( (ICTL1_IRQ_PR_7_VAL),ICTL_IRQ_PR_7_IRQ_PR_7_POS, ICTL_IRQ_PR_7_IRQ_PR_7_LEN,value)


/* RSVD_irq_pr_7 */

#ifndef ICTL_IRQ_PR_7_RSVD_IRQ_PR_7_POS
#define ICTL_IRQ_PR_7_RSVD_IRQ_PR_7_POS      4
#endif

#ifndef ICTL_IRQ_PR_7_RSVD_IRQ_PR_7_LEN
#define ICTL_IRQ_PR_7_RSVD_IRQ_PR_7_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_7_RSVD_IRQ_PR_7_R)
#define IRQ_PR_7_RSVD_IRQ_PR_7_R        GetGroupBits32( (ICTL1_IRQ_PR_7_VAL),ICTL_IRQ_PR_7_RSVD_IRQ_PR_7_POS, ICTL_IRQ_PR_7_RSVD_IRQ_PR_7_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_7_RSVD_IRQ_PR_7_W)
#define IRQ_PR_7_RSVD_IRQ_PR_7_W(value) SetGroupBits32( (ICTL1_IRQ_PR_7_VAL),ICTL_IRQ_PR_7_RSVD_IRQ_PR_7_POS, ICTL_IRQ_PR_7_RSVD_IRQ_PR_7_LEN,value)
#endif

#define ICTL1_IRQ_PR_7_RSVD_IRQ_PR_7_R        GetGroupBits32( (ICTL1_IRQ_PR_7_VAL),ICTL_IRQ_PR_7_RSVD_IRQ_PR_7_POS, ICTL_IRQ_PR_7_RSVD_IRQ_PR_7_LEN)

#define ICTL1_IRQ_PR_7_RSVD_IRQ_PR_7_W(value) SetGroupBits32( (ICTL1_IRQ_PR_7_VAL),ICTL_IRQ_PR_7_RSVD_IRQ_PR_7_POS, ICTL_IRQ_PR_7_RSVD_IRQ_PR_7_LEN,value)


/* REGISTER: IRQ_PR_8 */

#if defined(_V1) && !defined(IRQ_PR_8_OFFSET)
#define IRQ_PR_8_OFFSET 0x108
#endif

#if !defined(ICTL_IRQ_PR_8_OFFSET)
#define ICTL_IRQ_PR_8_OFFSET 0x108
#endif

#if defined(_V1) && !defined(IRQ_PR_8_REG)
#define IRQ_PR_8_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_8_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_8_VAL)
#define IRQ_PR_8_VAL  PREFIX_VAL(IRQ_PR_8_REG)
#endif

#define ICTL1_IRQ_PR_8_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_8_OFFSET))
#define ICTL1_IRQ_PR_8_VAL  PREFIX_VAL(ICTL1_IRQ_PR_8_REG)

/* FIELDS: */

/* irq_pr_8 */

#ifndef ICTL_IRQ_PR_8_IRQ_PR_8_POS
#define ICTL_IRQ_PR_8_IRQ_PR_8_POS      0
#endif

#ifndef ICTL_IRQ_PR_8_IRQ_PR_8_LEN
#define ICTL_IRQ_PR_8_IRQ_PR_8_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_8_IRQ_PR_8_R)
#define IRQ_PR_8_IRQ_PR_8_R        GetGroupBits32( (ICTL1_IRQ_PR_8_VAL),ICTL_IRQ_PR_8_IRQ_PR_8_POS, ICTL_IRQ_PR_8_IRQ_PR_8_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_8_IRQ_PR_8_W)
#define IRQ_PR_8_IRQ_PR_8_W(value) SetGroupBits32( (ICTL1_IRQ_PR_8_VAL),ICTL_IRQ_PR_8_IRQ_PR_8_POS, ICTL_IRQ_PR_8_IRQ_PR_8_LEN,value)
#endif

#define ICTL1_IRQ_PR_8_IRQ_PR_8_R        GetGroupBits32( (ICTL1_IRQ_PR_8_VAL),ICTL_IRQ_PR_8_IRQ_PR_8_POS, ICTL_IRQ_PR_8_IRQ_PR_8_LEN)

#define ICTL1_IRQ_PR_8_IRQ_PR_8_W(value) SetGroupBits32( (ICTL1_IRQ_PR_8_VAL),ICTL_IRQ_PR_8_IRQ_PR_8_POS, ICTL_IRQ_PR_8_IRQ_PR_8_LEN,value)


/* RSVD_irq_pr_8 */

#ifndef ICTL_IRQ_PR_8_RSVD_IRQ_PR_8_POS
#define ICTL_IRQ_PR_8_RSVD_IRQ_PR_8_POS      4
#endif

#ifndef ICTL_IRQ_PR_8_RSVD_IRQ_PR_8_LEN
#define ICTL_IRQ_PR_8_RSVD_IRQ_PR_8_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_8_RSVD_IRQ_PR_8_R)
#define IRQ_PR_8_RSVD_IRQ_PR_8_R        GetGroupBits32( (ICTL1_IRQ_PR_8_VAL),ICTL_IRQ_PR_8_RSVD_IRQ_PR_8_POS, ICTL_IRQ_PR_8_RSVD_IRQ_PR_8_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_8_RSVD_IRQ_PR_8_W)
#define IRQ_PR_8_RSVD_IRQ_PR_8_W(value) SetGroupBits32( (ICTL1_IRQ_PR_8_VAL),ICTL_IRQ_PR_8_RSVD_IRQ_PR_8_POS, ICTL_IRQ_PR_8_RSVD_IRQ_PR_8_LEN,value)
#endif

#define ICTL1_IRQ_PR_8_RSVD_IRQ_PR_8_R        GetGroupBits32( (ICTL1_IRQ_PR_8_VAL),ICTL_IRQ_PR_8_RSVD_IRQ_PR_8_POS, ICTL_IRQ_PR_8_RSVD_IRQ_PR_8_LEN)

#define ICTL1_IRQ_PR_8_RSVD_IRQ_PR_8_W(value) SetGroupBits32( (ICTL1_IRQ_PR_8_VAL),ICTL_IRQ_PR_8_RSVD_IRQ_PR_8_POS, ICTL_IRQ_PR_8_RSVD_IRQ_PR_8_LEN,value)


/* REGISTER: IRQ_PR_9 */

#if defined(_V1) && !defined(IRQ_PR_9_OFFSET)
#define IRQ_PR_9_OFFSET 0x10c
#endif

#if !defined(ICTL_IRQ_PR_9_OFFSET)
#define ICTL_IRQ_PR_9_OFFSET 0x10c
#endif

#if defined(_V1) && !defined(IRQ_PR_9_REG)
#define IRQ_PR_9_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_9_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_9_VAL)
#define IRQ_PR_9_VAL  PREFIX_VAL(IRQ_PR_9_REG)
#endif

#define ICTL1_IRQ_PR_9_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_9_OFFSET))
#define ICTL1_IRQ_PR_9_VAL  PREFIX_VAL(ICTL1_IRQ_PR_9_REG)

/* FIELDS: */

/* irq_pr_9 */

#ifndef ICTL_IRQ_PR_9_IRQ_PR_9_POS
#define ICTL_IRQ_PR_9_IRQ_PR_9_POS      0
#endif

#ifndef ICTL_IRQ_PR_9_IRQ_PR_9_LEN
#define ICTL_IRQ_PR_9_IRQ_PR_9_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_9_IRQ_PR_9_R)
#define IRQ_PR_9_IRQ_PR_9_R        GetGroupBits32( (ICTL1_IRQ_PR_9_VAL),ICTL_IRQ_PR_9_IRQ_PR_9_POS, ICTL_IRQ_PR_9_IRQ_PR_9_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_9_IRQ_PR_9_W)
#define IRQ_PR_9_IRQ_PR_9_W(value) SetGroupBits32( (ICTL1_IRQ_PR_9_VAL),ICTL_IRQ_PR_9_IRQ_PR_9_POS, ICTL_IRQ_PR_9_IRQ_PR_9_LEN,value)
#endif

#define ICTL1_IRQ_PR_9_IRQ_PR_9_R        GetGroupBits32( (ICTL1_IRQ_PR_9_VAL),ICTL_IRQ_PR_9_IRQ_PR_9_POS, ICTL_IRQ_PR_9_IRQ_PR_9_LEN)

#define ICTL1_IRQ_PR_9_IRQ_PR_9_W(value) SetGroupBits32( (ICTL1_IRQ_PR_9_VAL),ICTL_IRQ_PR_9_IRQ_PR_9_POS, ICTL_IRQ_PR_9_IRQ_PR_9_LEN,value)


/* RSVD_irq_pr_9 */

#ifndef ICTL_IRQ_PR_9_RSVD_IRQ_PR_9_POS
#define ICTL_IRQ_PR_9_RSVD_IRQ_PR_9_POS      4
#endif

#ifndef ICTL_IRQ_PR_9_RSVD_IRQ_PR_9_LEN
#define ICTL_IRQ_PR_9_RSVD_IRQ_PR_9_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_9_RSVD_IRQ_PR_9_R)
#define IRQ_PR_9_RSVD_IRQ_PR_9_R        GetGroupBits32( (ICTL1_IRQ_PR_9_VAL),ICTL_IRQ_PR_9_RSVD_IRQ_PR_9_POS, ICTL_IRQ_PR_9_RSVD_IRQ_PR_9_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_9_RSVD_IRQ_PR_9_W)
#define IRQ_PR_9_RSVD_IRQ_PR_9_W(value) SetGroupBits32( (ICTL1_IRQ_PR_9_VAL),ICTL_IRQ_PR_9_RSVD_IRQ_PR_9_POS, ICTL_IRQ_PR_9_RSVD_IRQ_PR_9_LEN,value)
#endif

#define ICTL1_IRQ_PR_9_RSVD_IRQ_PR_9_R        GetGroupBits32( (ICTL1_IRQ_PR_9_VAL),ICTL_IRQ_PR_9_RSVD_IRQ_PR_9_POS, ICTL_IRQ_PR_9_RSVD_IRQ_PR_9_LEN)

#define ICTL1_IRQ_PR_9_RSVD_IRQ_PR_9_W(value) SetGroupBits32( (ICTL1_IRQ_PR_9_VAL),ICTL_IRQ_PR_9_RSVD_IRQ_PR_9_POS, ICTL_IRQ_PR_9_RSVD_IRQ_PR_9_LEN,value)


/* REGISTER: IRQ_PR_10 */

#if defined(_V1) && !defined(IRQ_PR_10_OFFSET)
#define IRQ_PR_10_OFFSET 0x110
#endif

#if !defined(ICTL_IRQ_PR_10_OFFSET)
#define ICTL_IRQ_PR_10_OFFSET 0x110
#endif

#if defined(_V1) && !defined(IRQ_PR_10_REG)
#define IRQ_PR_10_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_10_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_10_VAL)
#define IRQ_PR_10_VAL  PREFIX_VAL(IRQ_PR_10_REG)
#endif

#define ICTL1_IRQ_PR_10_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_10_OFFSET))
#define ICTL1_IRQ_PR_10_VAL  PREFIX_VAL(ICTL1_IRQ_PR_10_REG)

/* FIELDS: */

/* irq_pr_10 */

#ifndef ICTL_IRQ_PR_10_IRQ_PR_10_POS
#define ICTL_IRQ_PR_10_IRQ_PR_10_POS      0
#endif

#ifndef ICTL_IRQ_PR_10_IRQ_PR_10_LEN
#define ICTL_IRQ_PR_10_IRQ_PR_10_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_10_IRQ_PR_10_R)
#define IRQ_PR_10_IRQ_PR_10_R        GetGroupBits32( (ICTL1_IRQ_PR_10_VAL),ICTL_IRQ_PR_10_IRQ_PR_10_POS, ICTL_IRQ_PR_10_IRQ_PR_10_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_10_IRQ_PR_10_W)
#define IRQ_PR_10_IRQ_PR_10_W(value) SetGroupBits32( (ICTL1_IRQ_PR_10_VAL),ICTL_IRQ_PR_10_IRQ_PR_10_POS, ICTL_IRQ_PR_10_IRQ_PR_10_LEN,value)
#endif

#define ICTL1_IRQ_PR_10_IRQ_PR_10_R        GetGroupBits32( (ICTL1_IRQ_PR_10_VAL),ICTL_IRQ_PR_10_IRQ_PR_10_POS, ICTL_IRQ_PR_10_IRQ_PR_10_LEN)

#define ICTL1_IRQ_PR_10_IRQ_PR_10_W(value) SetGroupBits32( (ICTL1_IRQ_PR_10_VAL),ICTL_IRQ_PR_10_IRQ_PR_10_POS, ICTL_IRQ_PR_10_IRQ_PR_10_LEN,value)


/* RSVD_irq_pr_10 */

#ifndef ICTL_IRQ_PR_10_RSVD_IRQ_PR_10_POS
#define ICTL_IRQ_PR_10_RSVD_IRQ_PR_10_POS      4
#endif

#ifndef ICTL_IRQ_PR_10_RSVD_IRQ_PR_10_LEN
#define ICTL_IRQ_PR_10_RSVD_IRQ_PR_10_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_10_RSVD_IRQ_PR_10_R)
#define IRQ_PR_10_RSVD_IRQ_PR_10_R        GetGroupBits32( (ICTL1_IRQ_PR_10_VAL),ICTL_IRQ_PR_10_RSVD_IRQ_PR_10_POS, ICTL_IRQ_PR_10_RSVD_IRQ_PR_10_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_10_RSVD_IRQ_PR_10_W)
#define IRQ_PR_10_RSVD_IRQ_PR_10_W(value) SetGroupBits32( (ICTL1_IRQ_PR_10_VAL),ICTL_IRQ_PR_10_RSVD_IRQ_PR_10_POS, ICTL_IRQ_PR_10_RSVD_IRQ_PR_10_LEN,value)
#endif

#define ICTL1_IRQ_PR_10_RSVD_IRQ_PR_10_R        GetGroupBits32( (ICTL1_IRQ_PR_10_VAL),ICTL_IRQ_PR_10_RSVD_IRQ_PR_10_POS, ICTL_IRQ_PR_10_RSVD_IRQ_PR_10_LEN)

#define ICTL1_IRQ_PR_10_RSVD_IRQ_PR_10_W(value) SetGroupBits32( (ICTL1_IRQ_PR_10_VAL),ICTL_IRQ_PR_10_RSVD_IRQ_PR_10_POS, ICTL_IRQ_PR_10_RSVD_IRQ_PR_10_LEN,value)


/* REGISTER: IRQ_PR_11 */

#if defined(_V1) && !defined(IRQ_PR_11_OFFSET)
#define IRQ_PR_11_OFFSET 0x114
#endif

#if !defined(ICTL_IRQ_PR_11_OFFSET)
#define ICTL_IRQ_PR_11_OFFSET 0x114
#endif

#if defined(_V1) && !defined(IRQ_PR_11_REG)
#define IRQ_PR_11_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_11_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_11_VAL)
#define IRQ_PR_11_VAL  PREFIX_VAL(IRQ_PR_11_REG)
#endif

#define ICTL1_IRQ_PR_11_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_11_OFFSET))
#define ICTL1_IRQ_PR_11_VAL  PREFIX_VAL(ICTL1_IRQ_PR_11_REG)

/* FIELDS: */

/* irq_pr_11 */

#ifndef ICTL_IRQ_PR_11_IRQ_PR_11_POS
#define ICTL_IRQ_PR_11_IRQ_PR_11_POS      0
#endif

#ifndef ICTL_IRQ_PR_11_IRQ_PR_11_LEN
#define ICTL_IRQ_PR_11_IRQ_PR_11_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_11_IRQ_PR_11_R)
#define IRQ_PR_11_IRQ_PR_11_R        GetGroupBits32( (ICTL1_IRQ_PR_11_VAL),ICTL_IRQ_PR_11_IRQ_PR_11_POS, ICTL_IRQ_PR_11_IRQ_PR_11_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_11_IRQ_PR_11_W)
#define IRQ_PR_11_IRQ_PR_11_W(value) SetGroupBits32( (ICTL1_IRQ_PR_11_VAL),ICTL_IRQ_PR_11_IRQ_PR_11_POS, ICTL_IRQ_PR_11_IRQ_PR_11_LEN,value)
#endif

#define ICTL1_IRQ_PR_11_IRQ_PR_11_R        GetGroupBits32( (ICTL1_IRQ_PR_11_VAL),ICTL_IRQ_PR_11_IRQ_PR_11_POS, ICTL_IRQ_PR_11_IRQ_PR_11_LEN)

#define ICTL1_IRQ_PR_11_IRQ_PR_11_W(value) SetGroupBits32( (ICTL1_IRQ_PR_11_VAL),ICTL_IRQ_PR_11_IRQ_PR_11_POS, ICTL_IRQ_PR_11_IRQ_PR_11_LEN,value)


/* RSVD_irq_pr_11 */

#ifndef ICTL_IRQ_PR_11_RSVD_IRQ_PR_11_POS
#define ICTL_IRQ_PR_11_RSVD_IRQ_PR_11_POS      4
#endif

#ifndef ICTL_IRQ_PR_11_RSVD_IRQ_PR_11_LEN
#define ICTL_IRQ_PR_11_RSVD_IRQ_PR_11_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_11_RSVD_IRQ_PR_11_R)
#define IRQ_PR_11_RSVD_IRQ_PR_11_R        GetGroupBits32( (ICTL1_IRQ_PR_11_VAL),ICTL_IRQ_PR_11_RSVD_IRQ_PR_11_POS, ICTL_IRQ_PR_11_RSVD_IRQ_PR_11_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_11_RSVD_IRQ_PR_11_W)
#define IRQ_PR_11_RSVD_IRQ_PR_11_W(value) SetGroupBits32( (ICTL1_IRQ_PR_11_VAL),ICTL_IRQ_PR_11_RSVD_IRQ_PR_11_POS, ICTL_IRQ_PR_11_RSVD_IRQ_PR_11_LEN,value)
#endif

#define ICTL1_IRQ_PR_11_RSVD_IRQ_PR_11_R        GetGroupBits32( (ICTL1_IRQ_PR_11_VAL),ICTL_IRQ_PR_11_RSVD_IRQ_PR_11_POS, ICTL_IRQ_PR_11_RSVD_IRQ_PR_11_LEN)

#define ICTL1_IRQ_PR_11_RSVD_IRQ_PR_11_W(value) SetGroupBits32( (ICTL1_IRQ_PR_11_VAL),ICTL_IRQ_PR_11_RSVD_IRQ_PR_11_POS, ICTL_IRQ_PR_11_RSVD_IRQ_PR_11_LEN,value)


/* REGISTER: IRQ_PR_12 */

#if defined(_V1) && !defined(IRQ_PR_12_OFFSET)
#define IRQ_PR_12_OFFSET 0x118
#endif

#if !defined(ICTL_IRQ_PR_12_OFFSET)
#define ICTL_IRQ_PR_12_OFFSET 0x118
#endif

#if defined(_V1) && !defined(IRQ_PR_12_REG)
#define IRQ_PR_12_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_12_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_12_VAL)
#define IRQ_PR_12_VAL  PREFIX_VAL(IRQ_PR_12_REG)
#endif

#define ICTL1_IRQ_PR_12_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_12_OFFSET))
#define ICTL1_IRQ_PR_12_VAL  PREFIX_VAL(ICTL1_IRQ_PR_12_REG)

/* FIELDS: */

/* irq_pr_12 */

#ifndef ICTL_IRQ_PR_12_IRQ_PR_12_POS
#define ICTL_IRQ_PR_12_IRQ_PR_12_POS      0
#endif

#ifndef ICTL_IRQ_PR_12_IRQ_PR_12_LEN
#define ICTL_IRQ_PR_12_IRQ_PR_12_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_12_IRQ_PR_12_R)
#define IRQ_PR_12_IRQ_PR_12_R        GetGroupBits32( (ICTL1_IRQ_PR_12_VAL),ICTL_IRQ_PR_12_IRQ_PR_12_POS, ICTL_IRQ_PR_12_IRQ_PR_12_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_12_IRQ_PR_12_W)
#define IRQ_PR_12_IRQ_PR_12_W(value) SetGroupBits32( (ICTL1_IRQ_PR_12_VAL),ICTL_IRQ_PR_12_IRQ_PR_12_POS, ICTL_IRQ_PR_12_IRQ_PR_12_LEN,value)
#endif

#define ICTL1_IRQ_PR_12_IRQ_PR_12_R        GetGroupBits32( (ICTL1_IRQ_PR_12_VAL),ICTL_IRQ_PR_12_IRQ_PR_12_POS, ICTL_IRQ_PR_12_IRQ_PR_12_LEN)

#define ICTL1_IRQ_PR_12_IRQ_PR_12_W(value) SetGroupBits32( (ICTL1_IRQ_PR_12_VAL),ICTL_IRQ_PR_12_IRQ_PR_12_POS, ICTL_IRQ_PR_12_IRQ_PR_12_LEN,value)


/* RSVD_irq_pr_12 */

#ifndef ICTL_IRQ_PR_12_RSVD_IRQ_PR_12_POS
#define ICTL_IRQ_PR_12_RSVD_IRQ_PR_12_POS      4
#endif

#ifndef ICTL_IRQ_PR_12_RSVD_IRQ_PR_12_LEN
#define ICTL_IRQ_PR_12_RSVD_IRQ_PR_12_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_12_RSVD_IRQ_PR_12_R)
#define IRQ_PR_12_RSVD_IRQ_PR_12_R        GetGroupBits32( (ICTL1_IRQ_PR_12_VAL),ICTL_IRQ_PR_12_RSVD_IRQ_PR_12_POS, ICTL_IRQ_PR_12_RSVD_IRQ_PR_12_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_12_RSVD_IRQ_PR_12_W)
#define IRQ_PR_12_RSVD_IRQ_PR_12_W(value) SetGroupBits32( (ICTL1_IRQ_PR_12_VAL),ICTL_IRQ_PR_12_RSVD_IRQ_PR_12_POS, ICTL_IRQ_PR_12_RSVD_IRQ_PR_12_LEN,value)
#endif

#define ICTL1_IRQ_PR_12_RSVD_IRQ_PR_12_R        GetGroupBits32( (ICTL1_IRQ_PR_12_VAL),ICTL_IRQ_PR_12_RSVD_IRQ_PR_12_POS, ICTL_IRQ_PR_12_RSVD_IRQ_PR_12_LEN)

#define ICTL1_IRQ_PR_12_RSVD_IRQ_PR_12_W(value) SetGroupBits32( (ICTL1_IRQ_PR_12_VAL),ICTL_IRQ_PR_12_RSVD_IRQ_PR_12_POS, ICTL_IRQ_PR_12_RSVD_IRQ_PR_12_LEN,value)


/* REGISTER: IRQ_PR_13 */

#if defined(_V1) && !defined(IRQ_PR_13_OFFSET)
#define IRQ_PR_13_OFFSET 0x11c
#endif

#if !defined(ICTL_IRQ_PR_13_OFFSET)
#define ICTL_IRQ_PR_13_OFFSET 0x11c
#endif

#if defined(_V1) && !defined(IRQ_PR_13_REG)
#define IRQ_PR_13_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_13_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_13_VAL)
#define IRQ_PR_13_VAL  PREFIX_VAL(IRQ_PR_13_REG)
#endif

#define ICTL1_IRQ_PR_13_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_13_OFFSET))
#define ICTL1_IRQ_PR_13_VAL  PREFIX_VAL(ICTL1_IRQ_PR_13_REG)

/* FIELDS: */

/* irq_pr_13 */

#ifndef ICTL_IRQ_PR_13_IRQ_PR_13_POS
#define ICTL_IRQ_PR_13_IRQ_PR_13_POS      0
#endif

#ifndef ICTL_IRQ_PR_13_IRQ_PR_13_LEN
#define ICTL_IRQ_PR_13_IRQ_PR_13_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_13_IRQ_PR_13_R)
#define IRQ_PR_13_IRQ_PR_13_R        GetGroupBits32( (ICTL1_IRQ_PR_13_VAL),ICTL_IRQ_PR_13_IRQ_PR_13_POS, ICTL_IRQ_PR_13_IRQ_PR_13_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_13_IRQ_PR_13_W)
#define IRQ_PR_13_IRQ_PR_13_W(value) SetGroupBits32( (ICTL1_IRQ_PR_13_VAL),ICTL_IRQ_PR_13_IRQ_PR_13_POS, ICTL_IRQ_PR_13_IRQ_PR_13_LEN,value)
#endif

#define ICTL1_IRQ_PR_13_IRQ_PR_13_R        GetGroupBits32( (ICTL1_IRQ_PR_13_VAL),ICTL_IRQ_PR_13_IRQ_PR_13_POS, ICTL_IRQ_PR_13_IRQ_PR_13_LEN)

#define ICTL1_IRQ_PR_13_IRQ_PR_13_W(value) SetGroupBits32( (ICTL1_IRQ_PR_13_VAL),ICTL_IRQ_PR_13_IRQ_PR_13_POS, ICTL_IRQ_PR_13_IRQ_PR_13_LEN,value)


/* RSVD_irq_pr_13 */

#ifndef ICTL_IRQ_PR_13_RSVD_IRQ_PR_13_POS
#define ICTL_IRQ_PR_13_RSVD_IRQ_PR_13_POS      4
#endif

#ifndef ICTL_IRQ_PR_13_RSVD_IRQ_PR_13_LEN
#define ICTL_IRQ_PR_13_RSVD_IRQ_PR_13_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_13_RSVD_IRQ_PR_13_R)
#define IRQ_PR_13_RSVD_IRQ_PR_13_R        GetGroupBits32( (ICTL1_IRQ_PR_13_VAL),ICTL_IRQ_PR_13_RSVD_IRQ_PR_13_POS, ICTL_IRQ_PR_13_RSVD_IRQ_PR_13_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_13_RSVD_IRQ_PR_13_W)
#define IRQ_PR_13_RSVD_IRQ_PR_13_W(value) SetGroupBits32( (ICTL1_IRQ_PR_13_VAL),ICTL_IRQ_PR_13_RSVD_IRQ_PR_13_POS, ICTL_IRQ_PR_13_RSVD_IRQ_PR_13_LEN,value)
#endif

#define ICTL1_IRQ_PR_13_RSVD_IRQ_PR_13_R        GetGroupBits32( (ICTL1_IRQ_PR_13_VAL),ICTL_IRQ_PR_13_RSVD_IRQ_PR_13_POS, ICTL_IRQ_PR_13_RSVD_IRQ_PR_13_LEN)

#define ICTL1_IRQ_PR_13_RSVD_IRQ_PR_13_W(value) SetGroupBits32( (ICTL1_IRQ_PR_13_VAL),ICTL_IRQ_PR_13_RSVD_IRQ_PR_13_POS, ICTL_IRQ_PR_13_RSVD_IRQ_PR_13_LEN,value)


/* REGISTER: IRQ_PR_14 */

#if defined(_V1) && !defined(IRQ_PR_14_OFFSET)
#define IRQ_PR_14_OFFSET 0x120
#endif

#if !defined(ICTL_IRQ_PR_14_OFFSET)
#define ICTL_IRQ_PR_14_OFFSET 0x120
#endif

#if defined(_V1) && !defined(IRQ_PR_14_REG)
#define IRQ_PR_14_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_14_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_14_VAL)
#define IRQ_PR_14_VAL  PREFIX_VAL(IRQ_PR_14_REG)
#endif

#define ICTL1_IRQ_PR_14_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_14_OFFSET))
#define ICTL1_IRQ_PR_14_VAL  PREFIX_VAL(ICTL1_IRQ_PR_14_REG)

/* FIELDS: */

/* irq_pr_14 */

#ifndef ICTL_IRQ_PR_14_IRQ_PR_14_POS
#define ICTL_IRQ_PR_14_IRQ_PR_14_POS      0
#endif

#ifndef ICTL_IRQ_PR_14_IRQ_PR_14_LEN
#define ICTL_IRQ_PR_14_IRQ_PR_14_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_14_IRQ_PR_14_R)
#define IRQ_PR_14_IRQ_PR_14_R        GetGroupBits32( (ICTL1_IRQ_PR_14_VAL),ICTL_IRQ_PR_14_IRQ_PR_14_POS, ICTL_IRQ_PR_14_IRQ_PR_14_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_14_IRQ_PR_14_W)
#define IRQ_PR_14_IRQ_PR_14_W(value) SetGroupBits32( (ICTL1_IRQ_PR_14_VAL),ICTL_IRQ_PR_14_IRQ_PR_14_POS, ICTL_IRQ_PR_14_IRQ_PR_14_LEN,value)
#endif

#define ICTL1_IRQ_PR_14_IRQ_PR_14_R        GetGroupBits32( (ICTL1_IRQ_PR_14_VAL),ICTL_IRQ_PR_14_IRQ_PR_14_POS, ICTL_IRQ_PR_14_IRQ_PR_14_LEN)

#define ICTL1_IRQ_PR_14_IRQ_PR_14_W(value) SetGroupBits32( (ICTL1_IRQ_PR_14_VAL),ICTL_IRQ_PR_14_IRQ_PR_14_POS, ICTL_IRQ_PR_14_IRQ_PR_14_LEN,value)


/* RSVD_irq_pr_14 */

#ifndef ICTL_IRQ_PR_14_RSVD_IRQ_PR_14_POS
#define ICTL_IRQ_PR_14_RSVD_IRQ_PR_14_POS      4
#endif

#ifndef ICTL_IRQ_PR_14_RSVD_IRQ_PR_14_LEN
#define ICTL_IRQ_PR_14_RSVD_IRQ_PR_14_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_14_RSVD_IRQ_PR_14_R)
#define IRQ_PR_14_RSVD_IRQ_PR_14_R        GetGroupBits32( (ICTL1_IRQ_PR_14_VAL),ICTL_IRQ_PR_14_RSVD_IRQ_PR_14_POS, ICTL_IRQ_PR_14_RSVD_IRQ_PR_14_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_14_RSVD_IRQ_PR_14_W)
#define IRQ_PR_14_RSVD_IRQ_PR_14_W(value) SetGroupBits32( (ICTL1_IRQ_PR_14_VAL),ICTL_IRQ_PR_14_RSVD_IRQ_PR_14_POS, ICTL_IRQ_PR_14_RSVD_IRQ_PR_14_LEN,value)
#endif

#define ICTL1_IRQ_PR_14_RSVD_IRQ_PR_14_R        GetGroupBits32( (ICTL1_IRQ_PR_14_VAL),ICTL_IRQ_PR_14_RSVD_IRQ_PR_14_POS, ICTL_IRQ_PR_14_RSVD_IRQ_PR_14_LEN)

#define ICTL1_IRQ_PR_14_RSVD_IRQ_PR_14_W(value) SetGroupBits32( (ICTL1_IRQ_PR_14_VAL),ICTL_IRQ_PR_14_RSVD_IRQ_PR_14_POS, ICTL_IRQ_PR_14_RSVD_IRQ_PR_14_LEN,value)


/* REGISTER: IRQ_PR_15 */

#if defined(_V1) && !defined(IRQ_PR_15_OFFSET)
#define IRQ_PR_15_OFFSET 0x124
#endif

#if !defined(ICTL_IRQ_PR_15_OFFSET)
#define ICTL_IRQ_PR_15_OFFSET 0x124
#endif

#if defined(_V1) && !defined(IRQ_PR_15_REG)
#define IRQ_PR_15_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_15_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_15_VAL)
#define IRQ_PR_15_VAL  PREFIX_VAL(IRQ_PR_15_REG)
#endif

#define ICTL1_IRQ_PR_15_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_15_OFFSET))
#define ICTL1_IRQ_PR_15_VAL  PREFIX_VAL(ICTL1_IRQ_PR_15_REG)

/* FIELDS: */

/* irq_pr_15 */

#ifndef ICTL_IRQ_PR_15_IRQ_PR_15_POS
#define ICTL_IRQ_PR_15_IRQ_PR_15_POS      0
#endif

#ifndef ICTL_IRQ_PR_15_IRQ_PR_15_LEN
#define ICTL_IRQ_PR_15_IRQ_PR_15_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_15_IRQ_PR_15_R)
#define IRQ_PR_15_IRQ_PR_15_R        GetGroupBits32( (ICTL1_IRQ_PR_15_VAL),ICTL_IRQ_PR_15_IRQ_PR_15_POS, ICTL_IRQ_PR_15_IRQ_PR_15_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_15_IRQ_PR_15_W)
#define IRQ_PR_15_IRQ_PR_15_W(value) SetGroupBits32( (ICTL1_IRQ_PR_15_VAL),ICTL_IRQ_PR_15_IRQ_PR_15_POS, ICTL_IRQ_PR_15_IRQ_PR_15_LEN,value)
#endif

#define ICTL1_IRQ_PR_15_IRQ_PR_15_R        GetGroupBits32( (ICTL1_IRQ_PR_15_VAL),ICTL_IRQ_PR_15_IRQ_PR_15_POS, ICTL_IRQ_PR_15_IRQ_PR_15_LEN)

#define ICTL1_IRQ_PR_15_IRQ_PR_15_W(value) SetGroupBits32( (ICTL1_IRQ_PR_15_VAL),ICTL_IRQ_PR_15_IRQ_PR_15_POS, ICTL_IRQ_PR_15_IRQ_PR_15_LEN,value)


/* RSVD_irq_pr_15 */

#ifndef ICTL_IRQ_PR_15_RSVD_IRQ_PR_15_POS
#define ICTL_IRQ_PR_15_RSVD_IRQ_PR_15_POS      4
#endif

#ifndef ICTL_IRQ_PR_15_RSVD_IRQ_PR_15_LEN
#define ICTL_IRQ_PR_15_RSVD_IRQ_PR_15_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_15_RSVD_IRQ_PR_15_R)
#define IRQ_PR_15_RSVD_IRQ_PR_15_R        GetGroupBits32( (ICTL1_IRQ_PR_15_VAL),ICTL_IRQ_PR_15_RSVD_IRQ_PR_15_POS, ICTL_IRQ_PR_15_RSVD_IRQ_PR_15_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_15_RSVD_IRQ_PR_15_W)
#define IRQ_PR_15_RSVD_IRQ_PR_15_W(value) SetGroupBits32( (ICTL1_IRQ_PR_15_VAL),ICTL_IRQ_PR_15_RSVD_IRQ_PR_15_POS, ICTL_IRQ_PR_15_RSVD_IRQ_PR_15_LEN,value)
#endif

#define ICTL1_IRQ_PR_15_RSVD_IRQ_PR_15_R        GetGroupBits32( (ICTL1_IRQ_PR_15_VAL),ICTL_IRQ_PR_15_RSVD_IRQ_PR_15_POS, ICTL_IRQ_PR_15_RSVD_IRQ_PR_15_LEN)

#define ICTL1_IRQ_PR_15_RSVD_IRQ_PR_15_W(value) SetGroupBits32( (ICTL1_IRQ_PR_15_VAL),ICTL_IRQ_PR_15_RSVD_IRQ_PR_15_POS, ICTL_IRQ_PR_15_RSVD_IRQ_PR_15_LEN,value)


/* REGISTER: IRQ_PR_16 */

#if defined(_V1) && !defined(IRQ_PR_16_OFFSET)
#define IRQ_PR_16_OFFSET 0x128
#endif

#if !defined(ICTL_IRQ_PR_16_OFFSET)
#define ICTL_IRQ_PR_16_OFFSET 0x128
#endif

#if defined(_V1) && !defined(IRQ_PR_16_REG)
#define IRQ_PR_16_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_16_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_16_VAL)
#define IRQ_PR_16_VAL  PREFIX_VAL(IRQ_PR_16_REG)
#endif

#define ICTL1_IRQ_PR_16_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_16_OFFSET))
#define ICTL1_IRQ_PR_16_VAL  PREFIX_VAL(ICTL1_IRQ_PR_16_REG)

/* FIELDS: */

/* irq_pr_16 */

#ifndef ICTL_IRQ_PR_16_IRQ_PR_16_POS
#define ICTL_IRQ_PR_16_IRQ_PR_16_POS      0
#endif

#ifndef ICTL_IRQ_PR_16_IRQ_PR_16_LEN
#define ICTL_IRQ_PR_16_IRQ_PR_16_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_16_IRQ_PR_16_R)
#define IRQ_PR_16_IRQ_PR_16_R        GetGroupBits32( (ICTL1_IRQ_PR_16_VAL),ICTL_IRQ_PR_16_IRQ_PR_16_POS, ICTL_IRQ_PR_16_IRQ_PR_16_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_16_IRQ_PR_16_W)
#define IRQ_PR_16_IRQ_PR_16_W(value) SetGroupBits32( (ICTL1_IRQ_PR_16_VAL),ICTL_IRQ_PR_16_IRQ_PR_16_POS, ICTL_IRQ_PR_16_IRQ_PR_16_LEN,value)
#endif

#define ICTL1_IRQ_PR_16_IRQ_PR_16_R        GetGroupBits32( (ICTL1_IRQ_PR_16_VAL),ICTL_IRQ_PR_16_IRQ_PR_16_POS, ICTL_IRQ_PR_16_IRQ_PR_16_LEN)

#define ICTL1_IRQ_PR_16_IRQ_PR_16_W(value) SetGroupBits32( (ICTL1_IRQ_PR_16_VAL),ICTL_IRQ_PR_16_IRQ_PR_16_POS, ICTL_IRQ_PR_16_IRQ_PR_16_LEN,value)


/* RSVD_irq_pr_16 */

#ifndef ICTL_IRQ_PR_16_RSVD_IRQ_PR_16_POS
#define ICTL_IRQ_PR_16_RSVD_IRQ_PR_16_POS      4
#endif

#ifndef ICTL_IRQ_PR_16_RSVD_IRQ_PR_16_LEN
#define ICTL_IRQ_PR_16_RSVD_IRQ_PR_16_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_16_RSVD_IRQ_PR_16_R)
#define IRQ_PR_16_RSVD_IRQ_PR_16_R        GetGroupBits32( (ICTL1_IRQ_PR_16_VAL),ICTL_IRQ_PR_16_RSVD_IRQ_PR_16_POS, ICTL_IRQ_PR_16_RSVD_IRQ_PR_16_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_16_RSVD_IRQ_PR_16_W)
#define IRQ_PR_16_RSVD_IRQ_PR_16_W(value) SetGroupBits32( (ICTL1_IRQ_PR_16_VAL),ICTL_IRQ_PR_16_RSVD_IRQ_PR_16_POS, ICTL_IRQ_PR_16_RSVD_IRQ_PR_16_LEN,value)
#endif

#define ICTL1_IRQ_PR_16_RSVD_IRQ_PR_16_R        GetGroupBits32( (ICTL1_IRQ_PR_16_VAL),ICTL_IRQ_PR_16_RSVD_IRQ_PR_16_POS, ICTL_IRQ_PR_16_RSVD_IRQ_PR_16_LEN)

#define ICTL1_IRQ_PR_16_RSVD_IRQ_PR_16_W(value) SetGroupBits32( (ICTL1_IRQ_PR_16_VAL),ICTL_IRQ_PR_16_RSVD_IRQ_PR_16_POS, ICTL_IRQ_PR_16_RSVD_IRQ_PR_16_LEN,value)


/* REGISTER: IRQ_PR_17 */

#if defined(_V1) && !defined(IRQ_PR_17_OFFSET)
#define IRQ_PR_17_OFFSET 0x12c
#endif

#if !defined(ICTL_IRQ_PR_17_OFFSET)
#define ICTL_IRQ_PR_17_OFFSET 0x12c
#endif

#if defined(_V1) && !defined(IRQ_PR_17_REG)
#define IRQ_PR_17_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_17_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_17_VAL)
#define IRQ_PR_17_VAL  PREFIX_VAL(IRQ_PR_17_REG)
#endif

#define ICTL1_IRQ_PR_17_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_17_OFFSET))
#define ICTL1_IRQ_PR_17_VAL  PREFIX_VAL(ICTL1_IRQ_PR_17_REG)

/* FIELDS: */

/* irq_pr_17 */

#ifndef ICTL_IRQ_PR_17_IRQ_PR_17_POS
#define ICTL_IRQ_PR_17_IRQ_PR_17_POS      0
#endif

#ifndef ICTL_IRQ_PR_17_IRQ_PR_17_LEN
#define ICTL_IRQ_PR_17_IRQ_PR_17_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_17_IRQ_PR_17_R)
#define IRQ_PR_17_IRQ_PR_17_R        GetGroupBits32( (ICTL1_IRQ_PR_17_VAL),ICTL_IRQ_PR_17_IRQ_PR_17_POS, ICTL_IRQ_PR_17_IRQ_PR_17_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_17_IRQ_PR_17_W)
#define IRQ_PR_17_IRQ_PR_17_W(value) SetGroupBits32( (ICTL1_IRQ_PR_17_VAL),ICTL_IRQ_PR_17_IRQ_PR_17_POS, ICTL_IRQ_PR_17_IRQ_PR_17_LEN,value)
#endif

#define ICTL1_IRQ_PR_17_IRQ_PR_17_R        GetGroupBits32( (ICTL1_IRQ_PR_17_VAL),ICTL_IRQ_PR_17_IRQ_PR_17_POS, ICTL_IRQ_PR_17_IRQ_PR_17_LEN)

#define ICTL1_IRQ_PR_17_IRQ_PR_17_W(value) SetGroupBits32( (ICTL1_IRQ_PR_17_VAL),ICTL_IRQ_PR_17_IRQ_PR_17_POS, ICTL_IRQ_PR_17_IRQ_PR_17_LEN,value)


/* RSVD_irq_pr_17 */

#ifndef ICTL_IRQ_PR_17_RSVD_IRQ_PR_17_POS
#define ICTL_IRQ_PR_17_RSVD_IRQ_PR_17_POS      4
#endif

#ifndef ICTL_IRQ_PR_17_RSVD_IRQ_PR_17_LEN
#define ICTL_IRQ_PR_17_RSVD_IRQ_PR_17_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_17_RSVD_IRQ_PR_17_R)
#define IRQ_PR_17_RSVD_IRQ_PR_17_R        GetGroupBits32( (ICTL1_IRQ_PR_17_VAL),ICTL_IRQ_PR_17_RSVD_IRQ_PR_17_POS, ICTL_IRQ_PR_17_RSVD_IRQ_PR_17_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_17_RSVD_IRQ_PR_17_W)
#define IRQ_PR_17_RSVD_IRQ_PR_17_W(value) SetGroupBits32( (ICTL1_IRQ_PR_17_VAL),ICTL_IRQ_PR_17_RSVD_IRQ_PR_17_POS, ICTL_IRQ_PR_17_RSVD_IRQ_PR_17_LEN,value)
#endif

#define ICTL1_IRQ_PR_17_RSVD_IRQ_PR_17_R        GetGroupBits32( (ICTL1_IRQ_PR_17_VAL),ICTL_IRQ_PR_17_RSVD_IRQ_PR_17_POS, ICTL_IRQ_PR_17_RSVD_IRQ_PR_17_LEN)

#define ICTL1_IRQ_PR_17_RSVD_IRQ_PR_17_W(value) SetGroupBits32( (ICTL1_IRQ_PR_17_VAL),ICTL_IRQ_PR_17_RSVD_IRQ_PR_17_POS, ICTL_IRQ_PR_17_RSVD_IRQ_PR_17_LEN,value)


/* REGISTER: IRQ_PR_18 */

#if defined(_V1) && !defined(IRQ_PR_18_OFFSET)
#define IRQ_PR_18_OFFSET 0x130
#endif

#if !defined(ICTL_IRQ_PR_18_OFFSET)
#define ICTL_IRQ_PR_18_OFFSET 0x130
#endif

#if defined(_V1) && !defined(IRQ_PR_18_REG)
#define IRQ_PR_18_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_18_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_18_VAL)
#define IRQ_PR_18_VAL  PREFIX_VAL(IRQ_PR_18_REG)
#endif

#define ICTL1_IRQ_PR_18_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_18_OFFSET))
#define ICTL1_IRQ_PR_18_VAL  PREFIX_VAL(ICTL1_IRQ_PR_18_REG)

/* FIELDS: */

/* irq_pr_18 */

#ifndef ICTL_IRQ_PR_18_IRQ_PR_18_POS
#define ICTL_IRQ_PR_18_IRQ_PR_18_POS      0
#endif

#ifndef ICTL_IRQ_PR_18_IRQ_PR_18_LEN
#define ICTL_IRQ_PR_18_IRQ_PR_18_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_18_IRQ_PR_18_R)
#define IRQ_PR_18_IRQ_PR_18_R        GetGroupBits32( (ICTL1_IRQ_PR_18_VAL),ICTL_IRQ_PR_18_IRQ_PR_18_POS, ICTL_IRQ_PR_18_IRQ_PR_18_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_18_IRQ_PR_18_W)
#define IRQ_PR_18_IRQ_PR_18_W(value) SetGroupBits32( (ICTL1_IRQ_PR_18_VAL),ICTL_IRQ_PR_18_IRQ_PR_18_POS, ICTL_IRQ_PR_18_IRQ_PR_18_LEN,value)
#endif

#define ICTL1_IRQ_PR_18_IRQ_PR_18_R        GetGroupBits32( (ICTL1_IRQ_PR_18_VAL),ICTL_IRQ_PR_18_IRQ_PR_18_POS, ICTL_IRQ_PR_18_IRQ_PR_18_LEN)

#define ICTL1_IRQ_PR_18_IRQ_PR_18_W(value) SetGroupBits32( (ICTL1_IRQ_PR_18_VAL),ICTL_IRQ_PR_18_IRQ_PR_18_POS, ICTL_IRQ_PR_18_IRQ_PR_18_LEN,value)


/* RSVD_irq_pr_18 */

#ifndef ICTL_IRQ_PR_18_RSVD_IRQ_PR_18_POS
#define ICTL_IRQ_PR_18_RSVD_IRQ_PR_18_POS      4
#endif

#ifndef ICTL_IRQ_PR_18_RSVD_IRQ_PR_18_LEN
#define ICTL_IRQ_PR_18_RSVD_IRQ_PR_18_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_18_RSVD_IRQ_PR_18_R)
#define IRQ_PR_18_RSVD_IRQ_PR_18_R        GetGroupBits32( (ICTL1_IRQ_PR_18_VAL),ICTL_IRQ_PR_18_RSVD_IRQ_PR_18_POS, ICTL_IRQ_PR_18_RSVD_IRQ_PR_18_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_18_RSVD_IRQ_PR_18_W)
#define IRQ_PR_18_RSVD_IRQ_PR_18_W(value) SetGroupBits32( (ICTL1_IRQ_PR_18_VAL),ICTL_IRQ_PR_18_RSVD_IRQ_PR_18_POS, ICTL_IRQ_PR_18_RSVD_IRQ_PR_18_LEN,value)
#endif

#define ICTL1_IRQ_PR_18_RSVD_IRQ_PR_18_R        GetGroupBits32( (ICTL1_IRQ_PR_18_VAL),ICTL_IRQ_PR_18_RSVD_IRQ_PR_18_POS, ICTL_IRQ_PR_18_RSVD_IRQ_PR_18_LEN)

#define ICTL1_IRQ_PR_18_RSVD_IRQ_PR_18_W(value) SetGroupBits32( (ICTL1_IRQ_PR_18_VAL),ICTL_IRQ_PR_18_RSVD_IRQ_PR_18_POS, ICTL_IRQ_PR_18_RSVD_IRQ_PR_18_LEN,value)


/* REGISTER: IRQ_PR_19 */

#if defined(_V1) && !defined(IRQ_PR_19_OFFSET)
#define IRQ_PR_19_OFFSET 0x134
#endif

#if !defined(ICTL_IRQ_PR_19_OFFSET)
#define ICTL_IRQ_PR_19_OFFSET 0x134
#endif

#if defined(_V1) && !defined(IRQ_PR_19_REG)
#define IRQ_PR_19_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_19_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_19_VAL)
#define IRQ_PR_19_VAL  PREFIX_VAL(IRQ_PR_19_REG)
#endif

#define ICTL1_IRQ_PR_19_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_19_OFFSET))
#define ICTL1_IRQ_PR_19_VAL  PREFIX_VAL(ICTL1_IRQ_PR_19_REG)

/* FIELDS: */

/* irq_pr_19 */

#ifndef ICTL_IRQ_PR_19_IRQ_PR_19_POS
#define ICTL_IRQ_PR_19_IRQ_PR_19_POS      0
#endif

#ifndef ICTL_IRQ_PR_19_IRQ_PR_19_LEN
#define ICTL_IRQ_PR_19_IRQ_PR_19_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_19_IRQ_PR_19_R)
#define IRQ_PR_19_IRQ_PR_19_R        GetGroupBits32( (ICTL1_IRQ_PR_19_VAL),ICTL_IRQ_PR_19_IRQ_PR_19_POS, ICTL_IRQ_PR_19_IRQ_PR_19_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_19_IRQ_PR_19_W)
#define IRQ_PR_19_IRQ_PR_19_W(value) SetGroupBits32( (ICTL1_IRQ_PR_19_VAL),ICTL_IRQ_PR_19_IRQ_PR_19_POS, ICTL_IRQ_PR_19_IRQ_PR_19_LEN,value)
#endif

#define ICTL1_IRQ_PR_19_IRQ_PR_19_R        GetGroupBits32( (ICTL1_IRQ_PR_19_VAL),ICTL_IRQ_PR_19_IRQ_PR_19_POS, ICTL_IRQ_PR_19_IRQ_PR_19_LEN)

#define ICTL1_IRQ_PR_19_IRQ_PR_19_W(value) SetGroupBits32( (ICTL1_IRQ_PR_19_VAL),ICTL_IRQ_PR_19_IRQ_PR_19_POS, ICTL_IRQ_PR_19_IRQ_PR_19_LEN,value)


/* RSVD_irq_pr_19 */

#ifndef ICTL_IRQ_PR_19_RSVD_IRQ_PR_19_POS
#define ICTL_IRQ_PR_19_RSVD_IRQ_PR_19_POS      4
#endif

#ifndef ICTL_IRQ_PR_19_RSVD_IRQ_PR_19_LEN
#define ICTL_IRQ_PR_19_RSVD_IRQ_PR_19_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_19_RSVD_IRQ_PR_19_R)
#define IRQ_PR_19_RSVD_IRQ_PR_19_R        GetGroupBits32( (ICTL1_IRQ_PR_19_VAL),ICTL_IRQ_PR_19_RSVD_IRQ_PR_19_POS, ICTL_IRQ_PR_19_RSVD_IRQ_PR_19_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_19_RSVD_IRQ_PR_19_W)
#define IRQ_PR_19_RSVD_IRQ_PR_19_W(value) SetGroupBits32( (ICTL1_IRQ_PR_19_VAL),ICTL_IRQ_PR_19_RSVD_IRQ_PR_19_POS, ICTL_IRQ_PR_19_RSVD_IRQ_PR_19_LEN,value)
#endif

#define ICTL1_IRQ_PR_19_RSVD_IRQ_PR_19_R        GetGroupBits32( (ICTL1_IRQ_PR_19_VAL),ICTL_IRQ_PR_19_RSVD_IRQ_PR_19_POS, ICTL_IRQ_PR_19_RSVD_IRQ_PR_19_LEN)

#define ICTL1_IRQ_PR_19_RSVD_IRQ_PR_19_W(value) SetGroupBits32( (ICTL1_IRQ_PR_19_VAL),ICTL_IRQ_PR_19_RSVD_IRQ_PR_19_POS, ICTL_IRQ_PR_19_RSVD_IRQ_PR_19_LEN,value)


/* REGISTER: IRQ_PR_20 */

#if defined(_V1) && !defined(IRQ_PR_20_OFFSET)
#define IRQ_PR_20_OFFSET 0x138
#endif

#if !defined(ICTL_IRQ_PR_20_OFFSET)
#define ICTL_IRQ_PR_20_OFFSET 0x138
#endif

#if defined(_V1) && !defined(IRQ_PR_20_REG)
#define IRQ_PR_20_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_20_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_20_VAL)
#define IRQ_PR_20_VAL  PREFIX_VAL(IRQ_PR_20_REG)
#endif

#define ICTL1_IRQ_PR_20_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_20_OFFSET))
#define ICTL1_IRQ_PR_20_VAL  PREFIX_VAL(ICTL1_IRQ_PR_20_REG)

/* FIELDS: */

/* irq_pr_20 */

#ifndef ICTL_IRQ_PR_20_IRQ_PR_20_POS
#define ICTL_IRQ_PR_20_IRQ_PR_20_POS      0
#endif

#ifndef ICTL_IRQ_PR_20_IRQ_PR_20_LEN
#define ICTL_IRQ_PR_20_IRQ_PR_20_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_20_IRQ_PR_20_R)
#define IRQ_PR_20_IRQ_PR_20_R        GetGroupBits32( (ICTL1_IRQ_PR_20_VAL),ICTL_IRQ_PR_20_IRQ_PR_20_POS, ICTL_IRQ_PR_20_IRQ_PR_20_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_20_IRQ_PR_20_W)
#define IRQ_PR_20_IRQ_PR_20_W(value) SetGroupBits32( (ICTL1_IRQ_PR_20_VAL),ICTL_IRQ_PR_20_IRQ_PR_20_POS, ICTL_IRQ_PR_20_IRQ_PR_20_LEN,value)
#endif

#define ICTL1_IRQ_PR_20_IRQ_PR_20_R        GetGroupBits32( (ICTL1_IRQ_PR_20_VAL),ICTL_IRQ_PR_20_IRQ_PR_20_POS, ICTL_IRQ_PR_20_IRQ_PR_20_LEN)

#define ICTL1_IRQ_PR_20_IRQ_PR_20_W(value) SetGroupBits32( (ICTL1_IRQ_PR_20_VAL),ICTL_IRQ_PR_20_IRQ_PR_20_POS, ICTL_IRQ_PR_20_IRQ_PR_20_LEN,value)


/* RSVD_irq_pr_20 */

#ifndef ICTL_IRQ_PR_20_RSVD_IRQ_PR_20_POS
#define ICTL_IRQ_PR_20_RSVD_IRQ_PR_20_POS      4
#endif

#ifndef ICTL_IRQ_PR_20_RSVD_IRQ_PR_20_LEN
#define ICTL_IRQ_PR_20_RSVD_IRQ_PR_20_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_20_RSVD_IRQ_PR_20_R)
#define IRQ_PR_20_RSVD_IRQ_PR_20_R        GetGroupBits32( (ICTL1_IRQ_PR_20_VAL),ICTL_IRQ_PR_20_RSVD_IRQ_PR_20_POS, ICTL_IRQ_PR_20_RSVD_IRQ_PR_20_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_20_RSVD_IRQ_PR_20_W)
#define IRQ_PR_20_RSVD_IRQ_PR_20_W(value) SetGroupBits32( (ICTL1_IRQ_PR_20_VAL),ICTL_IRQ_PR_20_RSVD_IRQ_PR_20_POS, ICTL_IRQ_PR_20_RSVD_IRQ_PR_20_LEN,value)
#endif

#define ICTL1_IRQ_PR_20_RSVD_IRQ_PR_20_R        GetGroupBits32( (ICTL1_IRQ_PR_20_VAL),ICTL_IRQ_PR_20_RSVD_IRQ_PR_20_POS, ICTL_IRQ_PR_20_RSVD_IRQ_PR_20_LEN)

#define ICTL1_IRQ_PR_20_RSVD_IRQ_PR_20_W(value) SetGroupBits32( (ICTL1_IRQ_PR_20_VAL),ICTL_IRQ_PR_20_RSVD_IRQ_PR_20_POS, ICTL_IRQ_PR_20_RSVD_IRQ_PR_20_LEN,value)


/* REGISTER: IRQ_PR_21 */

#if defined(_V1) && !defined(IRQ_PR_21_OFFSET)
#define IRQ_PR_21_OFFSET 0x13c
#endif

#if !defined(ICTL_IRQ_PR_21_OFFSET)
#define ICTL_IRQ_PR_21_OFFSET 0x13c
#endif

#if defined(_V1) && !defined(IRQ_PR_21_REG)
#define IRQ_PR_21_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_21_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_21_VAL)
#define IRQ_PR_21_VAL  PREFIX_VAL(IRQ_PR_21_REG)
#endif

#define ICTL1_IRQ_PR_21_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_21_OFFSET))
#define ICTL1_IRQ_PR_21_VAL  PREFIX_VAL(ICTL1_IRQ_PR_21_REG)

/* FIELDS: */

/* irq_pr_21 */

#ifndef ICTL_IRQ_PR_21_IRQ_PR_21_POS
#define ICTL_IRQ_PR_21_IRQ_PR_21_POS      0
#endif

#ifndef ICTL_IRQ_PR_21_IRQ_PR_21_LEN
#define ICTL_IRQ_PR_21_IRQ_PR_21_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_21_IRQ_PR_21_R)
#define IRQ_PR_21_IRQ_PR_21_R        GetGroupBits32( (ICTL1_IRQ_PR_21_VAL),ICTL_IRQ_PR_21_IRQ_PR_21_POS, ICTL_IRQ_PR_21_IRQ_PR_21_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_21_IRQ_PR_21_W)
#define IRQ_PR_21_IRQ_PR_21_W(value) SetGroupBits32( (ICTL1_IRQ_PR_21_VAL),ICTL_IRQ_PR_21_IRQ_PR_21_POS, ICTL_IRQ_PR_21_IRQ_PR_21_LEN,value)
#endif

#define ICTL1_IRQ_PR_21_IRQ_PR_21_R        GetGroupBits32( (ICTL1_IRQ_PR_21_VAL),ICTL_IRQ_PR_21_IRQ_PR_21_POS, ICTL_IRQ_PR_21_IRQ_PR_21_LEN)

#define ICTL1_IRQ_PR_21_IRQ_PR_21_W(value) SetGroupBits32( (ICTL1_IRQ_PR_21_VAL),ICTL_IRQ_PR_21_IRQ_PR_21_POS, ICTL_IRQ_PR_21_IRQ_PR_21_LEN,value)


/* RSVD_irq_pr_21 */

#ifndef ICTL_IRQ_PR_21_RSVD_IRQ_PR_21_POS
#define ICTL_IRQ_PR_21_RSVD_IRQ_PR_21_POS      4
#endif

#ifndef ICTL_IRQ_PR_21_RSVD_IRQ_PR_21_LEN
#define ICTL_IRQ_PR_21_RSVD_IRQ_PR_21_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_21_RSVD_IRQ_PR_21_R)
#define IRQ_PR_21_RSVD_IRQ_PR_21_R        GetGroupBits32( (ICTL1_IRQ_PR_21_VAL),ICTL_IRQ_PR_21_RSVD_IRQ_PR_21_POS, ICTL_IRQ_PR_21_RSVD_IRQ_PR_21_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_21_RSVD_IRQ_PR_21_W)
#define IRQ_PR_21_RSVD_IRQ_PR_21_W(value) SetGroupBits32( (ICTL1_IRQ_PR_21_VAL),ICTL_IRQ_PR_21_RSVD_IRQ_PR_21_POS, ICTL_IRQ_PR_21_RSVD_IRQ_PR_21_LEN,value)
#endif

#define ICTL1_IRQ_PR_21_RSVD_IRQ_PR_21_R        GetGroupBits32( (ICTL1_IRQ_PR_21_VAL),ICTL_IRQ_PR_21_RSVD_IRQ_PR_21_POS, ICTL_IRQ_PR_21_RSVD_IRQ_PR_21_LEN)

#define ICTL1_IRQ_PR_21_RSVD_IRQ_PR_21_W(value) SetGroupBits32( (ICTL1_IRQ_PR_21_VAL),ICTL_IRQ_PR_21_RSVD_IRQ_PR_21_POS, ICTL_IRQ_PR_21_RSVD_IRQ_PR_21_LEN,value)


/* REGISTER: IRQ_INTFORCE_H */

#if defined(_V1) && !defined(IRQ_INTFORCE_H_OFFSET)
#define IRQ_INTFORCE_H_OFFSET 0x14
#endif

#if !defined(ICTL_IRQ_INTFORCE_H_OFFSET)
#define ICTL_IRQ_INTFORCE_H_OFFSET 0x14
#endif

#if defined(_V1) && !defined(IRQ_INTFORCE_H_REG)
#define IRQ_INTFORCE_H_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_INTFORCE_H_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_INTFORCE_H_VAL)
#define IRQ_INTFORCE_H_VAL  PREFIX_VAL(IRQ_INTFORCE_H_REG)
#endif

#define ICTL1_IRQ_INTFORCE_H_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_INTFORCE_H_OFFSET))
#define ICTL1_IRQ_INTFORCE_H_VAL  PREFIX_VAL(ICTL1_IRQ_INTFORCE_H_REG)

/* FIELDS: */

/* IRQ_INTFORCE_H */

#ifndef ICTL_IRQ_INTFORCE_H_IRQ_INTFORCE_H_POS
#define ICTL_IRQ_INTFORCE_H_IRQ_INTFORCE_H_POS      0
#endif

#ifndef ICTL_IRQ_INTFORCE_H_IRQ_INTFORCE_H_LEN
#define ICTL_IRQ_INTFORCE_H_IRQ_INTFORCE_H_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_INTFORCE_H_IRQ_INTFORCE_H_R)
#define IRQ_INTFORCE_H_IRQ_INTFORCE_H_R        GetGroupBits32( (ICTL1_IRQ_INTFORCE_H_VAL),ICTL_IRQ_INTFORCE_H_IRQ_INTFORCE_H_POS, ICTL_IRQ_INTFORCE_H_IRQ_INTFORCE_H_LEN)
#endif

#if defined(_V1) && !defined(IRQ_INTFORCE_H_IRQ_INTFORCE_H_W)
#define IRQ_INTFORCE_H_IRQ_INTFORCE_H_W(value) SetGroupBits32( (ICTL1_IRQ_INTFORCE_H_VAL),ICTL_IRQ_INTFORCE_H_IRQ_INTFORCE_H_POS, ICTL_IRQ_INTFORCE_H_IRQ_INTFORCE_H_LEN,value)
#endif

#define ICTL1_IRQ_INTFORCE_H_IRQ_INTFORCE_H_R        GetGroupBits32( (ICTL1_IRQ_INTFORCE_H_VAL),ICTL_IRQ_INTFORCE_H_IRQ_INTFORCE_H_POS, ICTL_IRQ_INTFORCE_H_IRQ_INTFORCE_H_LEN)

#define ICTL1_IRQ_INTFORCE_H_IRQ_INTFORCE_H_W(value) SetGroupBits32( (ICTL1_IRQ_INTFORCE_H_VAL),ICTL_IRQ_INTFORCE_H_IRQ_INTFORCE_H_POS, ICTL_IRQ_INTFORCE_H_IRQ_INTFORCE_H_LEN,value)


/* REGISTER: IRQ_PR_22 */

#if defined(_V1) && !defined(IRQ_PR_22_OFFSET)
#define IRQ_PR_22_OFFSET 0x140
#endif

#if !defined(ICTL_IRQ_PR_22_OFFSET)
#define ICTL_IRQ_PR_22_OFFSET 0x140
#endif

#if defined(_V1) && !defined(IRQ_PR_22_REG)
#define IRQ_PR_22_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_22_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_22_VAL)
#define IRQ_PR_22_VAL  PREFIX_VAL(IRQ_PR_22_REG)
#endif

#define ICTL1_IRQ_PR_22_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_22_OFFSET))
#define ICTL1_IRQ_PR_22_VAL  PREFIX_VAL(ICTL1_IRQ_PR_22_REG)

/* FIELDS: */

/* irq_pr_22 */

#ifndef ICTL_IRQ_PR_22_IRQ_PR_22_POS
#define ICTL_IRQ_PR_22_IRQ_PR_22_POS      0
#endif

#ifndef ICTL_IRQ_PR_22_IRQ_PR_22_LEN
#define ICTL_IRQ_PR_22_IRQ_PR_22_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_22_IRQ_PR_22_R)
#define IRQ_PR_22_IRQ_PR_22_R        GetGroupBits32( (ICTL1_IRQ_PR_22_VAL),ICTL_IRQ_PR_22_IRQ_PR_22_POS, ICTL_IRQ_PR_22_IRQ_PR_22_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_22_IRQ_PR_22_W)
#define IRQ_PR_22_IRQ_PR_22_W(value) SetGroupBits32( (ICTL1_IRQ_PR_22_VAL),ICTL_IRQ_PR_22_IRQ_PR_22_POS, ICTL_IRQ_PR_22_IRQ_PR_22_LEN,value)
#endif

#define ICTL1_IRQ_PR_22_IRQ_PR_22_R        GetGroupBits32( (ICTL1_IRQ_PR_22_VAL),ICTL_IRQ_PR_22_IRQ_PR_22_POS, ICTL_IRQ_PR_22_IRQ_PR_22_LEN)

#define ICTL1_IRQ_PR_22_IRQ_PR_22_W(value) SetGroupBits32( (ICTL1_IRQ_PR_22_VAL),ICTL_IRQ_PR_22_IRQ_PR_22_POS, ICTL_IRQ_PR_22_IRQ_PR_22_LEN,value)


/* RSVD_irq_pr_22 */

#ifndef ICTL_IRQ_PR_22_RSVD_IRQ_PR_22_POS
#define ICTL_IRQ_PR_22_RSVD_IRQ_PR_22_POS      4
#endif

#ifndef ICTL_IRQ_PR_22_RSVD_IRQ_PR_22_LEN
#define ICTL_IRQ_PR_22_RSVD_IRQ_PR_22_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_22_RSVD_IRQ_PR_22_R)
#define IRQ_PR_22_RSVD_IRQ_PR_22_R        GetGroupBits32( (ICTL1_IRQ_PR_22_VAL),ICTL_IRQ_PR_22_RSVD_IRQ_PR_22_POS, ICTL_IRQ_PR_22_RSVD_IRQ_PR_22_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_22_RSVD_IRQ_PR_22_W)
#define IRQ_PR_22_RSVD_IRQ_PR_22_W(value) SetGroupBits32( (ICTL1_IRQ_PR_22_VAL),ICTL_IRQ_PR_22_RSVD_IRQ_PR_22_POS, ICTL_IRQ_PR_22_RSVD_IRQ_PR_22_LEN,value)
#endif

#define ICTL1_IRQ_PR_22_RSVD_IRQ_PR_22_R        GetGroupBits32( (ICTL1_IRQ_PR_22_VAL),ICTL_IRQ_PR_22_RSVD_IRQ_PR_22_POS, ICTL_IRQ_PR_22_RSVD_IRQ_PR_22_LEN)

#define ICTL1_IRQ_PR_22_RSVD_IRQ_PR_22_W(value) SetGroupBits32( (ICTL1_IRQ_PR_22_VAL),ICTL_IRQ_PR_22_RSVD_IRQ_PR_22_POS, ICTL_IRQ_PR_22_RSVD_IRQ_PR_22_LEN,value)


/* REGISTER: IRQ_PR_23 */

#if defined(_V1) && !defined(IRQ_PR_23_OFFSET)
#define IRQ_PR_23_OFFSET 0x144
#endif

#if !defined(ICTL_IRQ_PR_23_OFFSET)
#define ICTL_IRQ_PR_23_OFFSET 0x144
#endif

#if defined(_V1) && !defined(IRQ_PR_23_REG)
#define IRQ_PR_23_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_23_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_23_VAL)
#define IRQ_PR_23_VAL  PREFIX_VAL(IRQ_PR_23_REG)
#endif

#define ICTL1_IRQ_PR_23_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_23_OFFSET))
#define ICTL1_IRQ_PR_23_VAL  PREFIX_VAL(ICTL1_IRQ_PR_23_REG)

/* FIELDS: */

/* irq_pr_23 */

#ifndef ICTL_IRQ_PR_23_IRQ_PR_23_POS
#define ICTL_IRQ_PR_23_IRQ_PR_23_POS      0
#endif

#ifndef ICTL_IRQ_PR_23_IRQ_PR_23_LEN
#define ICTL_IRQ_PR_23_IRQ_PR_23_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_23_IRQ_PR_23_R)
#define IRQ_PR_23_IRQ_PR_23_R        GetGroupBits32( (ICTL1_IRQ_PR_23_VAL),ICTL_IRQ_PR_23_IRQ_PR_23_POS, ICTL_IRQ_PR_23_IRQ_PR_23_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_23_IRQ_PR_23_W)
#define IRQ_PR_23_IRQ_PR_23_W(value) SetGroupBits32( (ICTL1_IRQ_PR_23_VAL),ICTL_IRQ_PR_23_IRQ_PR_23_POS, ICTL_IRQ_PR_23_IRQ_PR_23_LEN,value)
#endif

#define ICTL1_IRQ_PR_23_IRQ_PR_23_R        GetGroupBits32( (ICTL1_IRQ_PR_23_VAL),ICTL_IRQ_PR_23_IRQ_PR_23_POS, ICTL_IRQ_PR_23_IRQ_PR_23_LEN)

#define ICTL1_IRQ_PR_23_IRQ_PR_23_W(value) SetGroupBits32( (ICTL1_IRQ_PR_23_VAL),ICTL_IRQ_PR_23_IRQ_PR_23_POS, ICTL_IRQ_PR_23_IRQ_PR_23_LEN,value)


/* RSVD_irq_pr_23 */

#ifndef ICTL_IRQ_PR_23_RSVD_IRQ_PR_23_POS
#define ICTL_IRQ_PR_23_RSVD_IRQ_PR_23_POS      4
#endif

#ifndef ICTL_IRQ_PR_23_RSVD_IRQ_PR_23_LEN
#define ICTL_IRQ_PR_23_RSVD_IRQ_PR_23_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_23_RSVD_IRQ_PR_23_R)
#define IRQ_PR_23_RSVD_IRQ_PR_23_R        GetGroupBits32( (ICTL1_IRQ_PR_23_VAL),ICTL_IRQ_PR_23_RSVD_IRQ_PR_23_POS, ICTL_IRQ_PR_23_RSVD_IRQ_PR_23_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_23_RSVD_IRQ_PR_23_W)
#define IRQ_PR_23_RSVD_IRQ_PR_23_W(value) SetGroupBits32( (ICTL1_IRQ_PR_23_VAL),ICTL_IRQ_PR_23_RSVD_IRQ_PR_23_POS, ICTL_IRQ_PR_23_RSVD_IRQ_PR_23_LEN,value)
#endif

#define ICTL1_IRQ_PR_23_RSVD_IRQ_PR_23_R        GetGroupBits32( (ICTL1_IRQ_PR_23_VAL),ICTL_IRQ_PR_23_RSVD_IRQ_PR_23_POS, ICTL_IRQ_PR_23_RSVD_IRQ_PR_23_LEN)

#define ICTL1_IRQ_PR_23_RSVD_IRQ_PR_23_W(value) SetGroupBits32( (ICTL1_IRQ_PR_23_VAL),ICTL_IRQ_PR_23_RSVD_IRQ_PR_23_POS, ICTL_IRQ_PR_23_RSVD_IRQ_PR_23_LEN,value)


/* REGISTER: IRQ_PR_24 */

#if defined(_V1) && !defined(IRQ_PR_24_OFFSET)
#define IRQ_PR_24_OFFSET 0x148
#endif

#if !defined(ICTL_IRQ_PR_24_OFFSET)
#define ICTL_IRQ_PR_24_OFFSET 0x148
#endif

#if defined(_V1) && !defined(IRQ_PR_24_REG)
#define IRQ_PR_24_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_24_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_24_VAL)
#define IRQ_PR_24_VAL  PREFIX_VAL(IRQ_PR_24_REG)
#endif

#define ICTL1_IRQ_PR_24_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_24_OFFSET))
#define ICTL1_IRQ_PR_24_VAL  PREFIX_VAL(ICTL1_IRQ_PR_24_REG)

/* FIELDS: */

/* irq_pr_24 */

#ifndef ICTL_IRQ_PR_24_IRQ_PR_24_POS
#define ICTL_IRQ_PR_24_IRQ_PR_24_POS      0
#endif

#ifndef ICTL_IRQ_PR_24_IRQ_PR_24_LEN
#define ICTL_IRQ_PR_24_IRQ_PR_24_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_24_IRQ_PR_24_R)
#define IRQ_PR_24_IRQ_PR_24_R        GetGroupBits32( (ICTL1_IRQ_PR_24_VAL),ICTL_IRQ_PR_24_IRQ_PR_24_POS, ICTL_IRQ_PR_24_IRQ_PR_24_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_24_IRQ_PR_24_W)
#define IRQ_PR_24_IRQ_PR_24_W(value) SetGroupBits32( (ICTL1_IRQ_PR_24_VAL),ICTL_IRQ_PR_24_IRQ_PR_24_POS, ICTL_IRQ_PR_24_IRQ_PR_24_LEN,value)
#endif

#define ICTL1_IRQ_PR_24_IRQ_PR_24_R        GetGroupBits32( (ICTL1_IRQ_PR_24_VAL),ICTL_IRQ_PR_24_IRQ_PR_24_POS, ICTL_IRQ_PR_24_IRQ_PR_24_LEN)

#define ICTL1_IRQ_PR_24_IRQ_PR_24_W(value) SetGroupBits32( (ICTL1_IRQ_PR_24_VAL),ICTL_IRQ_PR_24_IRQ_PR_24_POS, ICTL_IRQ_PR_24_IRQ_PR_24_LEN,value)


/* RSVD_irq_pr_24 */

#ifndef ICTL_IRQ_PR_24_RSVD_IRQ_PR_24_POS
#define ICTL_IRQ_PR_24_RSVD_IRQ_PR_24_POS      4
#endif

#ifndef ICTL_IRQ_PR_24_RSVD_IRQ_PR_24_LEN
#define ICTL_IRQ_PR_24_RSVD_IRQ_PR_24_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_24_RSVD_IRQ_PR_24_R)
#define IRQ_PR_24_RSVD_IRQ_PR_24_R        GetGroupBits32( (ICTL1_IRQ_PR_24_VAL),ICTL_IRQ_PR_24_RSVD_IRQ_PR_24_POS, ICTL_IRQ_PR_24_RSVD_IRQ_PR_24_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_24_RSVD_IRQ_PR_24_W)
#define IRQ_PR_24_RSVD_IRQ_PR_24_W(value) SetGroupBits32( (ICTL1_IRQ_PR_24_VAL),ICTL_IRQ_PR_24_RSVD_IRQ_PR_24_POS, ICTL_IRQ_PR_24_RSVD_IRQ_PR_24_LEN,value)
#endif

#define ICTL1_IRQ_PR_24_RSVD_IRQ_PR_24_R        GetGroupBits32( (ICTL1_IRQ_PR_24_VAL),ICTL_IRQ_PR_24_RSVD_IRQ_PR_24_POS, ICTL_IRQ_PR_24_RSVD_IRQ_PR_24_LEN)

#define ICTL1_IRQ_PR_24_RSVD_IRQ_PR_24_W(value) SetGroupBits32( (ICTL1_IRQ_PR_24_VAL),ICTL_IRQ_PR_24_RSVD_IRQ_PR_24_POS, ICTL_IRQ_PR_24_RSVD_IRQ_PR_24_LEN,value)


/* REGISTER: IRQ_PR_25 */

#if defined(_V1) && !defined(IRQ_PR_25_OFFSET)
#define IRQ_PR_25_OFFSET 0x14c
#endif

#if !defined(ICTL_IRQ_PR_25_OFFSET)
#define ICTL_IRQ_PR_25_OFFSET 0x14c
#endif

#if defined(_V1) && !defined(IRQ_PR_25_REG)
#define IRQ_PR_25_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_25_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_25_VAL)
#define IRQ_PR_25_VAL  PREFIX_VAL(IRQ_PR_25_REG)
#endif

#define ICTL1_IRQ_PR_25_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_25_OFFSET))
#define ICTL1_IRQ_PR_25_VAL  PREFIX_VAL(ICTL1_IRQ_PR_25_REG)

/* FIELDS: */

/* irq_pr_25 */

#ifndef ICTL_IRQ_PR_25_IRQ_PR_25_POS
#define ICTL_IRQ_PR_25_IRQ_PR_25_POS      0
#endif

#ifndef ICTL_IRQ_PR_25_IRQ_PR_25_LEN
#define ICTL_IRQ_PR_25_IRQ_PR_25_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_25_IRQ_PR_25_R)
#define IRQ_PR_25_IRQ_PR_25_R        GetGroupBits32( (ICTL1_IRQ_PR_25_VAL),ICTL_IRQ_PR_25_IRQ_PR_25_POS, ICTL_IRQ_PR_25_IRQ_PR_25_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_25_IRQ_PR_25_W)
#define IRQ_PR_25_IRQ_PR_25_W(value) SetGroupBits32( (ICTL1_IRQ_PR_25_VAL),ICTL_IRQ_PR_25_IRQ_PR_25_POS, ICTL_IRQ_PR_25_IRQ_PR_25_LEN,value)
#endif

#define ICTL1_IRQ_PR_25_IRQ_PR_25_R        GetGroupBits32( (ICTL1_IRQ_PR_25_VAL),ICTL_IRQ_PR_25_IRQ_PR_25_POS, ICTL_IRQ_PR_25_IRQ_PR_25_LEN)

#define ICTL1_IRQ_PR_25_IRQ_PR_25_W(value) SetGroupBits32( (ICTL1_IRQ_PR_25_VAL),ICTL_IRQ_PR_25_IRQ_PR_25_POS, ICTL_IRQ_PR_25_IRQ_PR_25_LEN,value)


/* RSVD_irq_pr_25 */

#ifndef ICTL_IRQ_PR_25_RSVD_IRQ_PR_25_POS
#define ICTL_IRQ_PR_25_RSVD_IRQ_PR_25_POS      4
#endif

#ifndef ICTL_IRQ_PR_25_RSVD_IRQ_PR_25_LEN
#define ICTL_IRQ_PR_25_RSVD_IRQ_PR_25_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_25_RSVD_IRQ_PR_25_R)
#define IRQ_PR_25_RSVD_IRQ_PR_25_R        GetGroupBits32( (ICTL1_IRQ_PR_25_VAL),ICTL_IRQ_PR_25_RSVD_IRQ_PR_25_POS, ICTL_IRQ_PR_25_RSVD_IRQ_PR_25_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_25_RSVD_IRQ_PR_25_W)
#define IRQ_PR_25_RSVD_IRQ_PR_25_W(value) SetGroupBits32( (ICTL1_IRQ_PR_25_VAL),ICTL_IRQ_PR_25_RSVD_IRQ_PR_25_POS, ICTL_IRQ_PR_25_RSVD_IRQ_PR_25_LEN,value)
#endif

#define ICTL1_IRQ_PR_25_RSVD_IRQ_PR_25_R        GetGroupBits32( (ICTL1_IRQ_PR_25_VAL),ICTL_IRQ_PR_25_RSVD_IRQ_PR_25_POS, ICTL_IRQ_PR_25_RSVD_IRQ_PR_25_LEN)

#define ICTL1_IRQ_PR_25_RSVD_IRQ_PR_25_W(value) SetGroupBits32( (ICTL1_IRQ_PR_25_VAL),ICTL_IRQ_PR_25_RSVD_IRQ_PR_25_POS, ICTL_IRQ_PR_25_RSVD_IRQ_PR_25_LEN,value)


/* REGISTER: IRQ_PR_26 */

#if defined(_V1) && !defined(IRQ_PR_26_OFFSET)
#define IRQ_PR_26_OFFSET 0x150
#endif

#if !defined(ICTL_IRQ_PR_26_OFFSET)
#define ICTL_IRQ_PR_26_OFFSET 0x150
#endif

#if defined(_V1) && !defined(IRQ_PR_26_REG)
#define IRQ_PR_26_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_26_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_26_VAL)
#define IRQ_PR_26_VAL  PREFIX_VAL(IRQ_PR_26_REG)
#endif

#define ICTL1_IRQ_PR_26_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_26_OFFSET))
#define ICTL1_IRQ_PR_26_VAL  PREFIX_VAL(ICTL1_IRQ_PR_26_REG)

/* FIELDS: */

/* irq_pr_26 */

#ifndef ICTL_IRQ_PR_26_IRQ_PR_26_POS
#define ICTL_IRQ_PR_26_IRQ_PR_26_POS      0
#endif

#ifndef ICTL_IRQ_PR_26_IRQ_PR_26_LEN
#define ICTL_IRQ_PR_26_IRQ_PR_26_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_26_IRQ_PR_26_R)
#define IRQ_PR_26_IRQ_PR_26_R        GetGroupBits32( (ICTL1_IRQ_PR_26_VAL),ICTL_IRQ_PR_26_IRQ_PR_26_POS, ICTL_IRQ_PR_26_IRQ_PR_26_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_26_IRQ_PR_26_W)
#define IRQ_PR_26_IRQ_PR_26_W(value) SetGroupBits32( (ICTL1_IRQ_PR_26_VAL),ICTL_IRQ_PR_26_IRQ_PR_26_POS, ICTL_IRQ_PR_26_IRQ_PR_26_LEN,value)
#endif

#define ICTL1_IRQ_PR_26_IRQ_PR_26_R        GetGroupBits32( (ICTL1_IRQ_PR_26_VAL),ICTL_IRQ_PR_26_IRQ_PR_26_POS, ICTL_IRQ_PR_26_IRQ_PR_26_LEN)

#define ICTL1_IRQ_PR_26_IRQ_PR_26_W(value) SetGroupBits32( (ICTL1_IRQ_PR_26_VAL),ICTL_IRQ_PR_26_IRQ_PR_26_POS, ICTL_IRQ_PR_26_IRQ_PR_26_LEN,value)


/* RSVD_irq_pr_26 */

#ifndef ICTL_IRQ_PR_26_RSVD_IRQ_PR_26_POS
#define ICTL_IRQ_PR_26_RSVD_IRQ_PR_26_POS      4
#endif

#ifndef ICTL_IRQ_PR_26_RSVD_IRQ_PR_26_LEN
#define ICTL_IRQ_PR_26_RSVD_IRQ_PR_26_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_26_RSVD_IRQ_PR_26_R)
#define IRQ_PR_26_RSVD_IRQ_PR_26_R        GetGroupBits32( (ICTL1_IRQ_PR_26_VAL),ICTL_IRQ_PR_26_RSVD_IRQ_PR_26_POS, ICTL_IRQ_PR_26_RSVD_IRQ_PR_26_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_26_RSVD_IRQ_PR_26_W)
#define IRQ_PR_26_RSVD_IRQ_PR_26_W(value) SetGroupBits32( (ICTL1_IRQ_PR_26_VAL),ICTL_IRQ_PR_26_RSVD_IRQ_PR_26_POS, ICTL_IRQ_PR_26_RSVD_IRQ_PR_26_LEN,value)
#endif

#define ICTL1_IRQ_PR_26_RSVD_IRQ_PR_26_R        GetGroupBits32( (ICTL1_IRQ_PR_26_VAL),ICTL_IRQ_PR_26_RSVD_IRQ_PR_26_POS, ICTL_IRQ_PR_26_RSVD_IRQ_PR_26_LEN)

#define ICTL1_IRQ_PR_26_RSVD_IRQ_PR_26_W(value) SetGroupBits32( (ICTL1_IRQ_PR_26_VAL),ICTL_IRQ_PR_26_RSVD_IRQ_PR_26_POS, ICTL_IRQ_PR_26_RSVD_IRQ_PR_26_LEN,value)


/* REGISTER: IRQ_PR_27 */

#if defined(_V1) && !defined(IRQ_PR_27_OFFSET)
#define IRQ_PR_27_OFFSET 0x154
#endif

#if !defined(ICTL_IRQ_PR_27_OFFSET)
#define ICTL_IRQ_PR_27_OFFSET 0x154
#endif

#if defined(_V1) && !defined(IRQ_PR_27_REG)
#define IRQ_PR_27_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_27_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_27_VAL)
#define IRQ_PR_27_VAL  PREFIX_VAL(IRQ_PR_27_REG)
#endif

#define ICTL1_IRQ_PR_27_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_27_OFFSET))
#define ICTL1_IRQ_PR_27_VAL  PREFIX_VAL(ICTL1_IRQ_PR_27_REG)

/* FIELDS: */

/* irq_pr_27 */

#ifndef ICTL_IRQ_PR_27_IRQ_PR_27_POS
#define ICTL_IRQ_PR_27_IRQ_PR_27_POS      0
#endif

#ifndef ICTL_IRQ_PR_27_IRQ_PR_27_LEN
#define ICTL_IRQ_PR_27_IRQ_PR_27_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_27_IRQ_PR_27_R)
#define IRQ_PR_27_IRQ_PR_27_R        GetGroupBits32( (ICTL1_IRQ_PR_27_VAL),ICTL_IRQ_PR_27_IRQ_PR_27_POS, ICTL_IRQ_PR_27_IRQ_PR_27_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_27_IRQ_PR_27_W)
#define IRQ_PR_27_IRQ_PR_27_W(value) SetGroupBits32( (ICTL1_IRQ_PR_27_VAL),ICTL_IRQ_PR_27_IRQ_PR_27_POS, ICTL_IRQ_PR_27_IRQ_PR_27_LEN,value)
#endif

#define ICTL1_IRQ_PR_27_IRQ_PR_27_R        GetGroupBits32( (ICTL1_IRQ_PR_27_VAL),ICTL_IRQ_PR_27_IRQ_PR_27_POS, ICTL_IRQ_PR_27_IRQ_PR_27_LEN)

#define ICTL1_IRQ_PR_27_IRQ_PR_27_W(value) SetGroupBits32( (ICTL1_IRQ_PR_27_VAL),ICTL_IRQ_PR_27_IRQ_PR_27_POS, ICTL_IRQ_PR_27_IRQ_PR_27_LEN,value)


/* RSVD_irq_pr_27 */

#ifndef ICTL_IRQ_PR_27_RSVD_IRQ_PR_27_POS
#define ICTL_IRQ_PR_27_RSVD_IRQ_PR_27_POS      4
#endif

#ifndef ICTL_IRQ_PR_27_RSVD_IRQ_PR_27_LEN
#define ICTL_IRQ_PR_27_RSVD_IRQ_PR_27_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_27_RSVD_IRQ_PR_27_R)
#define IRQ_PR_27_RSVD_IRQ_PR_27_R        GetGroupBits32( (ICTL1_IRQ_PR_27_VAL),ICTL_IRQ_PR_27_RSVD_IRQ_PR_27_POS, ICTL_IRQ_PR_27_RSVD_IRQ_PR_27_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_27_RSVD_IRQ_PR_27_W)
#define IRQ_PR_27_RSVD_IRQ_PR_27_W(value) SetGroupBits32( (ICTL1_IRQ_PR_27_VAL),ICTL_IRQ_PR_27_RSVD_IRQ_PR_27_POS, ICTL_IRQ_PR_27_RSVD_IRQ_PR_27_LEN,value)
#endif

#define ICTL1_IRQ_PR_27_RSVD_IRQ_PR_27_R        GetGroupBits32( (ICTL1_IRQ_PR_27_VAL),ICTL_IRQ_PR_27_RSVD_IRQ_PR_27_POS, ICTL_IRQ_PR_27_RSVD_IRQ_PR_27_LEN)

#define ICTL1_IRQ_PR_27_RSVD_IRQ_PR_27_W(value) SetGroupBits32( (ICTL1_IRQ_PR_27_VAL),ICTL_IRQ_PR_27_RSVD_IRQ_PR_27_POS, ICTL_IRQ_PR_27_RSVD_IRQ_PR_27_LEN,value)


/* REGISTER: IRQ_PR_28 */

#if defined(_V1) && !defined(IRQ_PR_28_OFFSET)
#define IRQ_PR_28_OFFSET 0x158
#endif

#if !defined(ICTL_IRQ_PR_28_OFFSET)
#define ICTL_IRQ_PR_28_OFFSET 0x158
#endif

#if defined(_V1) && !defined(IRQ_PR_28_REG)
#define IRQ_PR_28_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_28_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_28_VAL)
#define IRQ_PR_28_VAL  PREFIX_VAL(IRQ_PR_28_REG)
#endif

#define ICTL1_IRQ_PR_28_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_28_OFFSET))
#define ICTL1_IRQ_PR_28_VAL  PREFIX_VAL(ICTL1_IRQ_PR_28_REG)

/* FIELDS: */

/* irq_pr_28 */

#ifndef ICTL_IRQ_PR_28_IRQ_PR_28_POS
#define ICTL_IRQ_PR_28_IRQ_PR_28_POS      0
#endif

#ifndef ICTL_IRQ_PR_28_IRQ_PR_28_LEN
#define ICTL_IRQ_PR_28_IRQ_PR_28_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_28_IRQ_PR_28_R)
#define IRQ_PR_28_IRQ_PR_28_R        GetGroupBits32( (ICTL1_IRQ_PR_28_VAL),ICTL_IRQ_PR_28_IRQ_PR_28_POS, ICTL_IRQ_PR_28_IRQ_PR_28_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_28_IRQ_PR_28_W)
#define IRQ_PR_28_IRQ_PR_28_W(value) SetGroupBits32( (ICTL1_IRQ_PR_28_VAL),ICTL_IRQ_PR_28_IRQ_PR_28_POS, ICTL_IRQ_PR_28_IRQ_PR_28_LEN,value)
#endif

#define ICTL1_IRQ_PR_28_IRQ_PR_28_R        GetGroupBits32( (ICTL1_IRQ_PR_28_VAL),ICTL_IRQ_PR_28_IRQ_PR_28_POS, ICTL_IRQ_PR_28_IRQ_PR_28_LEN)

#define ICTL1_IRQ_PR_28_IRQ_PR_28_W(value) SetGroupBits32( (ICTL1_IRQ_PR_28_VAL),ICTL_IRQ_PR_28_IRQ_PR_28_POS, ICTL_IRQ_PR_28_IRQ_PR_28_LEN,value)


/* RSVD_irq_pr_28 */

#ifndef ICTL_IRQ_PR_28_RSVD_IRQ_PR_28_POS
#define ICTL_IRQ_PR_28_RSVD_IRQ_PR_28_POS      4
#endif

#ifndef ICTL_IRQ_PR_28_RSVD_IRQ_PR_28_LEN
#define ICTL_IRQ_PR_28_RSVD_IRQ_PR_28_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_28_RSVD_IRQ_PR_28_R)
#define IRQ_PR_28_RSVD_IRQ_PR_28_R        GetGroupBits32( (ICTL1_IRQ_PR_28_VAL),ICTL_IRQ_PR_28_RSVD_IRQ_PR_28_POS, ICTL_IRQ_PR_28_RSVD_IRQ_PR_28_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_28_RSVD_IRQ_PR_28_W)
#define IRQ_PR_28_RSVD_IRQ_PR_28_W(value) SetGroupBits32( (ICTL1_IRQ_PR_28_VAL),ICTL_IRQ_PR_28_RSVD_IRQ_PR_28_POS, ICTL_IRQ_PR_28_RSVD_IRQ_PR_28_LEN,value)
#endif

#define ICTL1_IRQ_PR_28_RSVD_IRQ_PR_28_R        GetGroupBits32( (ICTL1_IRQ_PR_28_VAL),ICTL_IRQ_PR_28_RSVD_IRQ_PR_28_POS, ICTL_IRQ_PR_28_RSVD_IRQ_PR_28_LEN)

#define ICTL1_IRQ_PR_28_RSVD_IRQ_PR_28_W(value) SetGroupBits32( (ICTL1_IRQ_PR_28_VAL),ICTL_IRQ_PR_28_RSVD_IRQ_PR_28_POS, ICTL_IRQ_PR_28_RSVD_IRQ_PR_28_LEN,value)


/* REGISTER: IRQ_PR_29 */

#if defined(_V1) && !defined(IRQ_PR_29_OFFSET)
#define IRQ_PR_29_OFFSET 0x15c
#endif

#if !defined(ICTL_IRQ_PR_29_OFFSET)
#define ICTL_IRQ_PR_29_OFFSET 0x15c
#endif

#if defined(_V1) && !defined(IRQ_PR_29_REG)
#define IRQ_PR_29_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_29_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_29_VAL)
#define IRQ_PR_29_VAL  PREFIX_VAL(IRQ_PR_29_REG)
#endif

#define ICTL1_IRQ_PR_29_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_29_OFFSET))
#define ICTL1_IRQ_PR_29_VAL  PREFIX_VAL(ICTL1_IRQ_PR_29_REG)

/* FIELDS: */

/* irq_pr_29 */

#ifndef ICTL_IRQ_PR_29_IRQ_PR_29_POS
#define ICTL_IRQ_PR_29_IRQ_PR_29_POS      0
#endif

#ifndef ICTL_IRQ_PR_29_IRQ_PR_29_LEN
#define ICTL_IRQ_PR_29_IRQ_PR_29_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_29_IRQ_PR_29_R)
#define IRQ_PR_29_IRQ_PR_29_R        GetGroupBits32( (ICTL1_IRQ_PR_29_VAL),ICTL_IRQ_PR_29_IRQ_PR_29_POS, ICTL_IRQ_PR_29_IRQ_PR_29_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_29_IRQ_PR_29_W)
#define IRQ_PR_29_IRQ_PR_29_W(value) SetGroupBits32( (ICTL1_IRQ_PR_29_VAL),ICTL_IRQ_PR_29_IRQ_PR_29_POS, ICTL_IRQ_PR_29_IRQ_PR_29_LEN,value)
#endif

#define ICTL1_IRQ_PR_29_IRQ_PR_29_R        GetGroupBits32( (ICTL1_IRQ_PR_29_VAL),ICTL_IRQ_PR_29_IRQ_PR_29_POS, ICTL_IRQ_PR_29_IRQ_PR_29_LEN)

#define ICTL1_IRQ_PR_29_IRQ_PR_29_W(value) SetGroupBits32( (ICTL1_IRQ_PR_29_VAL),ICTL_IRQ_PR_29_IRQ_PR_29_POS, ICTL_IRQ_PR_29_IRQ_PR_29_LEN,value)


/* RSVD_irq_pr_29 */

#ifndef ICTL_IRQ_PR_29_RSVD_IRQ_PR_29_POS
#define ICTL_IRQ_PR_29_RSVD_IRQ_PR_29_POS      4
#endif

#ifndef ICTL_IRQ_PR_29_RSVD_IRQ_PR_29_LEN
#define ICTL_IRQ_PR_29_RSVD_IRQ_PR_29_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_29_RSVD_IRQ_PR_29_R)
#define IRQ_PR_29_RSVD_IRQ_PR_29_R        GetGroupBits32( (ICTL1_IRQ_PR_29_VAL),ICTL_IRQ_PR_29_RSVD_IRQ_PR_29_POS, ICTL_IRQ_PR_29_RSVD_IRQ_PR_29_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_29_RSVD_IRQ_PR_29_W)
#define IRQ_PR_29_RSVD_IRQ_PR_29_W(value) SetGroupBits32( (ICTL1_IRQ_PR_29_VAL),ICTL_IRQ_PR_29_RSVD_IRQ_PR_29_POS, ICTL_IRQ_PR_29_RSVD_IRQ_PR_29_LEN,value)
#endif

#define ICTL1_IRQ_PR_29_RSVD_IRQ_PR_29_R        GetGroupBits32( (ICTL1_IRQ_PR_29_VAL),ICTL_IRQ_PR_29_RSVD_IRQ_PR_29_POS, ICTL_IRQ_PR_29_RSVD_IRQ_PR_29_LEN)

#define ICTL1_IRQ_PR_29_RSVD_IRQ_PR_29_W(value) SetGroupBits32( (ICTL1_IRQ_PR_29_VAL),ICTL_IRQ_PR_29_RSVD_IRQ_PR_29_POS, ICTL_IRQ_PR_29_RSVD_IRQ_PR_29_LEN,value)


/* REGISTER: IRQ_PR_30 */

#if defined(_V1) && !defined(IRQ_PR_30_OFFSET)
#define IRQ_PR_30_OFFSET 0x160
#endif

#if !defined(ICTL_IRQ_PR_30_OFFSET)
#define ICTL_IRQ_PR_30_OFFSET 0x160
#endif

#if defined(_V1) && !defined(IRQ_PR_30_REG)
#define IRQ_PR_30_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_30_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_30_VAL)
#define IRQ_PR_30_VAL  PREFIX_VAL(IRQ_PR_30_REG)
#endif

#define ICTL1_IRQ_PR_30_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_30_OFFSET))
#define ICTL1_IRQ_PR_30_VAL  PREFIX_VAL(ICTL1_IRQ_PR_30_REG)

/* FIELDS: */

/* irq_pr_30 */

#ifndef ICTL_IRQ_PR_30_IRQ_PR_30_POS
#define ICTL_IRQ_PR_30_IRQ_PR_30_POS      0
#endif

#ifndef ICTL_IRQ_PR_30_IRQ_PR_30_LEN
#define ICTL_IRQ_PR_30_IRQ_PR_30_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_30_IRQ_PR_30_R)
#define IRQ_PR_30_IRQ_PR_30_R        GetGroupBits32( (ICTL1_IRQ_PR_30_VAL),ICTL_IRQ_PR_30_IRQ_PR_30_POS, ICTL_IRQ_PR_30_IRQ_PR_30_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_30_IRQ_PR_30_W)
#define IRQ_PR_30_IRQ_PR_30_W(value) SetGroupBits32( (ICTL1_IRQ_PR_30_VAL),ICTL_IRQ_PR_30_IRQ_PR_30_POS, ICTL_IRQ_PR_30_IRQ_PR_30_LEN,value)
#endif

#define ICTL1_IRQ_PR_30_IRQ_PR_30_R        GetGroupBits32( (ICTL1_IRQ_PR_30_VAL),ICTL_IRQ_PR_30_IRQ_PR_30_POS, ICTL_IRQ_PR_30_IRQ_PR_30_LEN)

#define ICTL1_IRQ_PR_30_IRQ_PR_30_W(value) SetGroupBits32( (ICTL1_IRQ_PR_30_VAL),ICTL_IRQ_PR_30_IRQ_PR_30_POS, ICTL_IRQ_PR_30_IRQ_PR_30_LEN,value)


/* RSVD_irq_pr_30 */

#ifndef ICTL_IRQ_PR_30_RSVD_IRQ_PR_30_POS
#define ICTL_IRQ_PR_30_RSVD_IRQ_PR_30_POS      4
#endif

#ifndef ICTL_IRQ_PR_30_RSVD_IRQ_PR_30_LEN
#define ICTL_IRQ_PR_30_RSVD_IRQ_PR_30_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_30_RSVD_IRQ_PR_30_R)
#define IRQ_PR_30_RSVD_IRQ_PR_30_R        GetGroupBits32( (ICTL1_IRQ_PR_30_VAL),ICTL_IRQ_PR_30_RSVD_IRQ_PR_30_POS, ICTL_IRQ_PR_30_RSVD_IRQ_PR_30_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_30_RSVD_IRQ_PR_30_W)
#define IRQ_PR_30_RSVD_IRQ_PR_30_W(value) SetGroupBits32( (ICTL1_IRQ_PR_30_VAL),ICTL_IRQ_PR_30_RSVD_IRQ_PR_30_POS, ICTL_IRQ_PR_30_RSVD_IRQ_PR_30_LEN,value)
#endif

#define ICTL1_IRQ_PR_30_RSVD_IRQ_PR_30_R        GetGroupBits32( (ICTL1_IRQ_PR_30_VAL),ICTL_IRQ_PR_30_RSVD_IRQ_PR_30_POS, ICTL_IRQ_PR_30_RSVD_IRQ_PR_30_LEN)

#define ICTL1_IRQ_PR_30_RSVD_IRQ_PR_30_W(value) SetGroupBits32( (ICTL1_IRQ_PR_30_VAL),ICTL_IRQ_PR_30_RSVD_IRQ_PR_30_POS, ICTL_IRQ_PR_30_RSVD_IRQ_PR_30_LEN,value)


/* REGISTER: IRQ_PR_31 */

#if defined(_V1) && !defined(IRQ_PR_31_OFFSET)
#define IRQ_PR_31_OFFSET 0x164
#endif

#if !defined(ICTL_IRQ_PR_31_OFFSET)
#define ICTL_IRQ_PR_31_OFFSET 0x164
#endif

#if defined(_V1) && !defined(IRQ_PR_31_REG)
#define IRQ_PR_31_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_31_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_31_VAL)
#define IRQ_PR_31_VAL  PREFIX_VAL(IRQ_PR_31_REG)
#endif

#define ICTL1_IRQ_PR_31_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_31_OFFSET))
#define ICTL1_IRQ_PR_31_VAL  PREFIX_VAL(ICTL1_IRQ_PR_31_REG)

/* FIELDS: */

/* irq_pr_31 */

#ifndef ICTL_IRQ_PR_31_IRQ_PR_31_POS
#define ICTL_IRQ_PR_31_IRQ_PR_31_POS      0
#endif

#ifndef ICTL_IRQ_PR_31_IRQ_PR_31_LEN
#define ICTL_IRQ_PR_31_IRQ_PR_31_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_31_IRQ_PR_31_R)
#define IRQ_PR_31_IRQ_PR_31_R        GetGroupBits32( (ICTL1_IRQ_PR_31_VAL),ICTL_IRQ_PR_31_IRQ_PR_31_POS, ICTL_IRQ_PR_31_IRQ_PR_31_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_31_IRQ_PR_31_W)
#define IRQ_PR_31_IRQ_PR_31_W(value) SetGroupBits32( (ICTL1_IRQ_PR_31_VAL),ICTL_IRQ_PR_31_IRQ_PR_31_POS, ICTL_IRQ_PR_31_IRQ_PR_31_LEN,value)
#endif

#define ICTL1_IRQ_PR_31_IRQ_PR_31_R        GetGroupBits32( (ICTL1_IRQ_PR_31_VAL),ICTL_IRQ_PR_31_IRQ_PR_31_POS, ICTL_IRQ_PR_31_IRQ_PR_31_LEN)

#define ICTL1_IRQ_PR_31_IRQ_PR_31_W(value) SetGroupBits32( (ICTL1_IRQ_PR_31_VAL),ICTL_IRQ_PR_31_IRQ_PR_31_POS, ICTL_IRQ_PR_31_IRQ_PR_31_LEN,value)


/* RSVD_irq_pr_31 */

#ifndef ICTL_IRQ_PR_31_RSVD_IRQ_PR_31_POS
#define ICTL_IRQ_PR_31_RSVD_IRQ_PR_31_POS      4
#endif

#ifndef ICTL_IRQ_PR_31_RSVD_IRQ_PR_31_LEN
#define ICTL_IRQ_PR_31_RSVD_IRQ_PR_31_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_31_RSVD_IRQ_PR_31_R)
#define IRQ_PR_31_RSVD_IRQ_PR_31_R        GetGroupBits32( (ICTL1_IRQ_PR_31_VAL),ICTL_IRQ_PR_31_RSVD_IRQ_PR_31_POS, ICTL_IRQ_PR_31_RSVD_IRQ_PR_31_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_31_RSVD_IRQ_PR_31_W)
#define IRQ_PR_31_RSVD_IRQ_PR_31_W(value) SetGroupBits32( (ICTL1_IRQ_PR_31_VAL),ICTL_IRQ_PR_31_RSVD_IRQ_PR_31_POS, ICTL_IRQ_PR_31_RSVD_IRQ_PR_31_LEN,value)
#endif

#define ICTL1_IRQ_PR_31_RSVD_IRQ_PR_31_R        GetGroupBits32( (ICTL1_IRQ_PR_31_VAL),ICTL_IRQ_PR_31_RSVD_IRQ_PR_31_POS, ICTL_IRQ_PR_31_RSVD_IRQ_PR_31_LEN)

#define ICTL1_IRQ_PR_31_RSVD_IRQ_PR_31_W(value) SetGroupBits32( (ICTL1_IRQ_PR_31_VAL),ICTL_IRQ_PR_31_RSVD_IRQ_PR_31_POS, ICTL_IRQ_PR_31_RSVD_IRQ_PR_31_LEN,value)


/* REGISTER: IRQ_PR_32 */

#if defined(_V1) && !defined(IRQ_PR_32_OFFSET)
#define IRQ_PR_32_OFFSET 0x168
#endif

#if !defined(ICTL_IRQ_PR_32_OFFSET)
#define ICTL_IRQ_PR_32_OFFSET 0x168
#endif

#if defined(_V1) && !defined(IRQ_PR_32_REG)
#define IRQ_PR_32_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_32_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_32_VAL)
#define IRQ_PR_32_VAL  PREFIX_VAL(IRQ_PR_32_REG)
#endif

#define ICTL1_IRQ_PR_32_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_32_OFFSET))
#define ICTL1_IRQ_PR_32_VAL  PREFIX_VAL(ICTL1_IRQ_PR_32_REG)

/* FIELDS: */

/* irq_pr_32 */

#ifndef ICTL_IRQ_PR_32_IRQ_PR_32_POS
#define ICTL_IRQ_PR_32_IRQ_PR_32_POS      0
#endif

#ifndef ICTL_IRQ_PR_32_IRQ_PR_32_LEN
#define ICTL_IRQ_PR_32_IRQ_PR_32_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_32_IRQ_PR_32_R)
#define IRQ_PR_32_IRQ_PR_32_R        GetGroupBits32( (ICTL1_IRQ_PR_32_VAL),ICTL_IRQ_PR_32_IRQ_PR_32_POS, ICTL_IRQ_PR_32_IRQ_PR_32_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_32_IRQ_PR_32_W)
#define IRQ_PR_32_IRQ_PR_32_W(value) SetGroupBits32( (ICTL1_IRQ_PR_32_VAL),ICTL_IRQ_PR_32_IRQ_PR_32_POS, ICTL_IRQ_PR_32_IRQ_PR_32_LEN,value)
#endif

#define ICTL1_IRQ_PR_32_IRQ_PR_32_R        GetGroupBits32( (ICTL1_IRQ_PR_32_VAL),ICTL_IRQ_PR_32_IRQ_PR_32_POS, ICTL_IRQ_PR_32_IRQ_PR_32_LEN)

#define ICTL1_IRQ_PR_32_IRQ_PR_32_W(value) SetGroupBits32( (ICTL1_IRQ_PR_32_VAL),ICTL_IRQ_PR_32_IRQ_PR_32_POS, ICTL_IRQ_PR_32_IRQ_PR_32_LEN,value)


/* RSVD_irq_pr_32 */

#ifndef ICTL_IRQ_PR_32_RSVD_IRQ_PR_32_POS
#define ICTL_IRQ_PR_32_RSVD_IRQ_PR_32_POS      4
#endif

#ifndef ICTL_IRQ_PR_32_RSVD_IRQ_PR_32_LEN
#define ICTL_IRQ_PR_32_RSVD_IRQ_PR_32_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_32_RSVD_IRQ_PR_32_R)
#define IRQ_PR_32_RSVD_IRQ_PR_32_R        GetGroupBits32( (ICTL1_IRQ_PR_32_VAL),ICTL_IRQ_PR_32_RSVD_IRQ_PR_32_POS, ICTL_IRQ_PR_32_RSVD_IRQ_PR_32_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_32_RSVD_IRQ_PR_32_W)
#define IRQ_PR_32_RSVD_IRQ_PR_32_W(value) SetGroupBits32( (ICTL1_IRQ_PR_32_VAL),ICTL_IRQ_PR_32_RSVD_IRQ_PR_32_POS, ICTL_IRQ_PR_32_RSVD_IRQ_PR_32_LEN,value)
#endif

#define ICTL1_IRQ_PR_32_RSVD_IRQ_PR_32_R        GetGroupBits32( (ICTL1_IRQ_PR_32_VAL),ICTL_IRQ_PR_32_RSVD_IRQ_PR_32_POS, ICTL_IRQ_PR_32_RSVD_IRQ_PR_32_LEN)

#define ICTL1_IRQ_PR_32_RSVD_IRQ_PR_32_W(value) SetGroupBits32( (ICTL1_IRQ_PR_32_VAL),ICTL_IRQ_PR_32_RSVD_IRQ_PR_32_POS, ICTL_IRQ_PR_32_RSVD_IRQ_PR_32_LEN,value)


/* REGISTER: IRQ_PR_33 */

#if defined(_V1) && !defined(IRQ_PR_33_OFFSET)
#define IRQ_PR_33_OFFSET 0x16c
#endif

#if !defined(ICTL_IRQ_PR_33_OFFSET)
#define ICTL_IRQ_PR_33_OFFSET 0x16c
#endif

#if defined(_V1) && !defined(IRQ_PR_33_REG)
#define IRQ_PR_33_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_33_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_33_VAL)
#define IRQ_PR_33_VAL  PREFIX_VAL(IRQ_PR_33_REG)
#endif

#define ICTL1_IRQ_PR_33_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_33_OFFSET))
#define ICTL1_IRQ_PR_33_VAL  PREFIX_VAL(ICTL1_IRQ_PR_33_REG)

/* FIELDS: */

/* irq_pr_33 */

#ifndef ICTL_IRQ_PR_33_IRQ_PR_33_POS
#define ICTL_IRQ_PR_33_IRQ_PR_33_POS      0
#endif

#ifndef ICTL_IRQ_PR_33_IRQ_PR_33_LEN
#define ICTL_IRQ_PR_33_IRQ_PR_33_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_33_IRQ_PR_33_R)
#define IRQ_PR_33_IRQ_PR_33_R        GetGroupBits32( (ICTL1_IRQ_PR_33_VAL),ICTL_IRQ_PR_33_IRQ_PR_33_POS, ICTL_IRQ_PR_33_IRQ_PR_33_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_33_IRQ_PR_33_W)
#define IRQ_PR_33_IRQ_PR_33_W(value) SetGroupBits32( (ICTL1_IRQ_PR_33_VAL),ICTL_IRQ_PR_33_IRQ_PR_33_POS, ICTL_IRQ_PR_33_IRQ_PR_33_LEN,value)
#endif

#define ICTL1_IRQ_PR_33_IRQ_PR_33_R        GetGroupBits32( (ICTL1_IRQ_PR_33_VAL),ICTL_IRQ_PR_33_IRQ_PR_33_POS, ICTL_IRQ_PR_33_IRQ_PR_33_LEN)

#define ICTL1_IRQ_PR_33_IRQ_PR_33_W(value) SetGroupBits32( (ICTL1_IRQ_PR_33_VAL),ICTL_IRQ_PR_33_IRQ_PR_33_POS, ICTL_IRQ_PR_33_IRQ_PR_33_LEN,value)


/* RSVD_irq_pr_33 */

#ifndef ICTL_IRQ_PR_33_RSVD_IRQ_PR_33_POS
#define ICTL_IRQ_PR_33_RSVD_IRQ_PR_33_POS      4
#endif

#ifndef ICTL_IRQ_PR_33_RSVD_IRQ_PR_33_LEN
#define ICTL_IRQ_PR_33_RSVD_IRQ_PR_33_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_33_RSVD_IRQ_PR_33_R)
#define IRQ_PR_33_RSVD_IRQ_PR_33_R        GetGroupBits32( (ICTL1_IRQ_PR_33_VAL),ICTL_IRQ_PR_33_RSVD_IRQ_PR_33_POS, ICTL_IRQ_PR_33_RSVD_IRQ_PR_33_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_33_RSVD_IRQ_PR_33_W)
#define IRQ_PR_33_RSVD_IRQ_PR_33_W(value) SetGroupBits32( (ICTL1_IRQ_PR_33_VAL),ICTL_IRQ_PR_33_RSVD_IRQ_PR_33_POS, ICTL_IRQ_PR_33_RSVD_IRQ_PR_33_LEN,value)
#endif

#define ICTL1_IRQ_PR_33_RSVD_IRQ_PR_33_R        GetGroupBits32( (ICTL1_IRQ_PR_33_VAL),ICTL_IRQ_PR_33_RSVD_IRQ_PR_33_POS, ICTL_IRQ_PR_33_RSVD_IRQ_PR_33_LEN)

#define ICTL1_IRQ_PR_33_RSVD_IRQ_PR_33_W(value) SetGroupBits32( (ICTL1_IRQ_PR_33_VAL),ICTL_IRQ_PR_33_RSVD_IRQ_PR_33_POS, ICTL_IRQ_PR_33_RSVD_IRQ_PR_33_LEN,value)


/* REGISTER: IRQ_PR_34 */

#if defined(_V1) && !defined(IRQ_PR_34_OFFSET)
#define IRQ_PR_34_OFFSET 0x170
#endif

#if !defined(ICTL_IRQ_PR_34_OFFSET)
#define ICTL_IRQ_PR_34_OFFSET 0x170
#endif

#if defined(_V1) && !defined(IRQ_PR_34_REG)
#define IRQ_PR_34_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_34_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_34_VAL)
#define IRQ_PR_34_VAL  PREFIX_VAL(IRQ_PR_34_REG)
#endif

#define ICTL1_IRQ_PR_34_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_34_OFFSET))
#define ICTL1_IRQ_PR_34_VAL  PREFIX_VAL(ICTL1_IRQ_PR_34_REG)

/* FIELDS: */

/* irq_pr_34 */

#ifndef ICTL_IRQ_PR_34_IRQ_PR_34_POS
#define ICTL_IRQ_PR_34_IRQ_PR_34_POS      0
#endif

#ifndef ICTL_IRQ_PR_34_IRQ_PR_34_LEN
#define ICTL_IRQ_PR_34_IRQ_PR_34_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_34_IRQ_PR_34_R)
#define IRQ_PR_34_IRQ_PR_34_R        GetGroupBits32( (ICTL1_IRQ_PR_34_VAL),ICTL_IRQ_PR_34_IRQ_PR_34_POS, ICTL_IRQ_PR_34_IRQ_PR_34_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_34_IRQ_PR_34_W)
#define IRQ_PR_34_IRQ_PR_34_W(value) SetGroupBits32( (ICTL1_IRQ_PR_34_VAL),ICTL_IRQ_PR_34_IRQ_PR_34_POS, ICTL_IRQ_PR_34_IRQ_PR_34_LEN,value)
#endif

#define ICTL1_IRQ_PR_34_IRQ_PR_34_R        GetGroupBits32( (ICTL1_IRQ_PR_34_VAL),ICTL_IRQ_PR_34_IRQ_PR_34_POS, ICTL_IRQ_PR_34_IRQ_PR_34_LEN)

#define ICTL1_IRQ_PR_34_IRQ_PR_34_W(value) SetGroupBits32( (ICTL1_IRQ_PR_34_VAL),ICTL_IRQ_PR_34_IRQ_PR_34_POS, ICTL_IRQ_PR_34_IRQ_PR_34_LEN,value)


/* RSVD_irq_pr_34 */

#ifndef ICTL_IRQ_PR_34_RSVD_IRQ_PR_34_POS
#define ICTL_IRQ_PR_34_RSVD_IRQ_PR_34_POS      4
#endif

#ifndef ICTL_IRQ_PR_34_RSVD_IRQ_PR_34_LEN
#define ICTL_IRQ_PR_34_RSVD_IRQ_PR_34_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_34_RSVD_IRQ_PR_34_R)
#define IRQ_PR_34_RSVD_IRQ_PR_34_R        GetGroupBits32( (ICTL1_IRQ_PR_34_VAL),ICTL_IRQ_PR_34_RSVD_IRQ_PR_34_POS, ICTL_IRQ_PR_34_RSVD_IRQ_PR_34_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_34_RSVD_IRQ_PR_34_W)
#define IRQ_PR_34_RSVD_IRQ_PR_34_W(value) SetGroupBits32( (ICTL1_IRQ_PR_34_VAL),ICTL_IRQ_PR_34_RSVD_IRQ_PR_34_POS, ICTL_IRQ_PR_34_RSVD_IRQ_PR_34_LEN,value)
#endif

#define ICTL1_IRQ_PR_34_RSVD_IRQ_PR_34_R        GetGroupBits32( (ICTL1_IRQ_PR_34_VAL),ICTL_IRQ_PR_34_RSVD_IRQ_PR_34_POS, ICTL_IRQ_PR_34_RSVD_IRQ_PR_34_LEN)

#define ICTL1_IRQ_PR_34_RSVD_IRQ_PR_34_W(value) SetGroupBits32( (ICTL1_IRQ_PR_34_VAL),ICTL_IRQ_PR_34_RSVD_IRQ_PR_34_POS, ICTL_IRQ_PR_34_RSVD_IRQ_PR_34_LEN,value)


/* REGISTER: IRQ_PR_35 */

#if defined(_V1) && !defined(IRQ_PR_35_OFFSET)
#define IRQ_PR_35_OFFSET 0x174
#endif

#if !defined(ICTL_IRQ_PR_35_OFFSET)
#define ICTL_IRQ_PR_35_OFFSET 0x174
#endif

#if defined(_V1) && !defined(IRQ_PR_35_REG)
#define IRQ_PR_35_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_35_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_35_VAL)
#define IRQ_PR_35_VAL  PREFIX_VAL(IRQ_PR_35_REG)
#endif

#define ICTL1_IRQ_PR_35_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_35_OFFSET))
#define ICTL1_IRQ_PR_35_VAL  PREFIX_VAL(ICTL1_IRQ_PR_35_REG)

/* FIELDS: */

/* irq_pr_35 */

#ifndef ICTL_IRQ_PR_35_IRQ_PR_35_POS
#define ICTL_IRQ_PR_35_IRQ_PR_35_POS      0
#endif

#ifndef ICTL_IRQ_PR_35_IRQ_PR_35_LEN
#define ICTL_IRQ_PR_35_IRQ_PR_35_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_35_IRQ_PR_35_R)
#define IRQ_PR_35_IRQ_PR_35_R        GetGroupBits32( (ICTL1_IRQ_PR_35_VAL),ICTL_IRQ_PR_35_IRQ_PR_35_POS, ICTL_IRQ_PR_35_IRQ_PR_35_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_35_IRQ_PR_35_W)
#define IRQ_PR_35_IRQ_PR_35_W(value) SetGroupBits32( (ICTL1_IRQ_PR_35_VAL),ICTL_IRQ_PR_35_IRQ_PR_35_POS, ICTL_IRQ_PR_35_IRQ_PR_35_LEN,value)
#endif

#define ICTL1_IRQ_PR_35_IRQ_PR_35_R        GetGroupBits32( (ICTL1_IRQ_PR_35_VAL),ICTL_IRQ_PR_35_IRQ_PR_35_POS, ICTL_IRQ_PR_35_IRQ_PR_35_LEN)

#define ICTL1_IRQ_PR_35_IRQ_PR_35_W(value) SetGroupBits32( (ICTL1_IRQ_PR_35_VAL),ICTL_IRQ_PR_35_IRQ_PR_35_POS, ICTL_IRQ_PR_35_IRQ_PR_35_LEN,value)


/* RSVD_irq_pr_35 */

#ifndef ICTL_IRQ_PR_35_RSVD_IRQ_PR_35_POS
#define ICTL_IRQ_PR_35_RSVD_IRQ_PR_35_POS      4
#endif

#ifndef ICTL_IRQ_PR_35_RSVD_IRQ_PR_35_LEN
#define ICTL_IRQ_PR_35_RSVD_IRQ_PR_35_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_35_RSVD_IRQ_PR_35_R)
#define IRQ_PR_35_RSVD_IRQ_PR_35_R        GetGroupBits32( (ICTL1_IRQ_PR_35_VAL),ICTL_IRQ_PR_35_RSVD_IRQ_PR_35_POS, ICTL_IRQ_PR_35_RSVD_IRQ_PR_35_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_35_RSVD_IRQ_PR_35_W)
#define IRQ_PR_35_RSVD_IRQ_PR_35_W(value) SetGroupBits32( (ICTL1_IRQ_PR_35_VAL),ICTL_IRQ_PR_35_RSVD_IRQ_PR_35_POS, ICTL_IRQ_PR_35_RSVD_IRQ_PR_35_LEN,value)
#endif

#define ICTL1_IRQ_PR_35_RSVD_IRQ_PR_35_R        GetGroupBits32( (ICTL1_IRQ_PR_35_VAL),ICTL_IRQ_PR_35_RSVD_IRQ_PR_35_POS, ICTL_IRQ_PR_35_RSVD_IRQ_PR_35_LEN)

#define ICTL1_IRQ_PR_35_RSVD_IRQ_PR_35_W(value) SetGroupBits32( (ICTL1_IRQ_PR_35_VAL),ICTL_IRQ_PR_35_RSVD_IRQ_PR_35_POS, ICTL_IRQ_PR_35_RSVD_IRQ_PR_35_LEN,value)


/* REGISTER: IRQ_PR_36 */

#if defined(_V1) && !defined(IRQ_PR_36_OFFSET)
#define IRQ_PR_36_OFFSET 0x178
#endif

#if !defined(ICTL_IRQ_PR_36_OFFSET)
#define ICTL_IRQ_PR_36_OFFSET 0x178
#endif

#if defined(_V1) && !defined(IRQ_PR_36_REG)
#define IRQ_PR_36_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_36_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_36_VAL)
#define IRQ_PR_36_VAL  PREFIX_VAL(IRQ_PR_36_REG)
#endif

#define ICTL1_IRQ_PR_36_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_36_OFFSET))
#define ICTL1_IRQ_PR_36_VAL  PREFIX_VAL(ICTL1_IRQ_PR_36_REG)

/* FIELDS: */

/* irq_pr_36 */

#ifndef ICTL_IRQ_PR_36_IRQ_PR_36_POS
#define ICTL_IRQ_PR_36_IRQ_PR_36_POS      0
#endif

#ifndef ICTL_IRQ_PR_36_IRQ_PR_36_LEN
#define ICTL_IRQ_PR_36_IRQ_PR_36_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_36_IRQ_PR_36_R)
#define IRQ_PR_36_IRQ_PR_36_R        GetGroupBits32( (ICTL1_IRQ_PR_36_VAL),ICTL_IRQ_PR_36_IRQ_PR_36_POS, ICTL_IRQ_PR_36_IRQ_PR_36_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_36_IRQ_PR_36_W)
#define IRQ_PR_36_IRQ_PR_36_W(value) SetGroupBits32( (ICTL1_IRQ_PR_36_VAL),ICTL_IRQ_PR_36_IRQ_PR_36_POS, ICTL_IRQ_PR_36_IRQ_PR_36_LEN,value)
#endif

#define ICTL1_IRQ_PR_36_IRQ_PR_36_R        GetGroupBits32( (ICTL1_IRQ_PR_36_VAL),ICTL_IRQ_PR_36_IRQ_PR_36_POS, ICTL_IRQ_PR_36_IRQ_PR_36_LEN)

#define ICTL1_IRQ_PR_36_IRQ_PR_36_W(value) SetGroupBits32( (ICTL1_IRQ_PR_36_VAL),ICTL_IRQ_PR_36_IRQ_PR_36_POS, ICTL_IRQ_PR_36_IRQ_PR_36_LEN,value)


/* RSVD_irq_pr_36 */

#ifndef ICTL_IRQ_PR_36_RSVD_IRQ_PR_36_POS
#define ICTL_IRQ_PR_36_RSVD_IRQ_PR_36_POS      4
#endif

#ifndef ICTL_IRQ_PR_36_RSVD_IRQ_PR_36_LEN
#define ICTL_IRQ_PR_36_RSVD_IRQ_PR_36_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_36_RSVD_IRQ_PR_36_R)
#define IRQ_PR_36_RSVD_IRQ_PR_36_R        GetGroupBits32( (ICTL1_IRQ_PR_36_VAL),ICTL_IRQ_PR_36_RSVD_IRQ_PR_36_POS, ICTL_IRQ_PR_36_RSVD_IRQ_PR_36_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_36_RSVD_IRQ_PR_36_W)
#define IRQ_PR_36_RSVD_IRQ_PR_36_W(value) SetGroupBits32( (ICTL1_IRQ_PR_36_VAL),ICTL_IRQ_PR_36_RSVD_IRQ_PR_36_POS, ICTL_IRQ_PR_36_RSVD_IRQ_PR_36_LEN,value)
#endif

#define ICTL1_IRQ_PR_36_RSVD_IRQ_PR_36_R        GetGroupBits32( (ICTL1_IRQ_PR_36_VAL),ICTL_IRQ_PR_36_RSVD_IRQ_PR_36_POS, ICTL_IRQ_PR_36_RSVD_IRQ_PR_36_LEN)

#define ICTL1_IRQ_PR_36_RSVD_IRQ_PR_36_W(value) SetGroupBits32( (ICTL1_IRQ_PR_36_VAL),ICTL_IRQ_PR_36_RSVD_IRQ_PR_36_POS, ICTL_IRQ_PR_36_RSVD_IRQ_PR_36_LEN,value)


/* REGISTER: IRQ_PR_37 */

#if defined(_V1) && !defined(IRQ_PR_37_OFFSET)
#define IRQ_PR_37_OFFSET 0x17c
#endif

#if !defined(ICTL_IRQ_PR_37_OFFSET)
#define ICTL_IRQ_PR_37_OFFSET 0x17c
#endif

#if defined(_V1) && !defined(IRQ_PR_37_REG)
#define IRQ_PR_37_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_37_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_37_VAL)
#define IRQ_PR_37_VAL  PREFIX_VAL(IRQ_PR_37_REG)
#endif

#define ICTL1_IRQ_PR_37_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_37_OFFSET))
#define ICTL1_IRQ_PR_37_VAL  PREFIX_VAL(ICTL1_IRQ_PR_37_REG)

/* FIELDS: */

/* irq_pr_37 */

#ifndef ICTL_IRQ_PR_37_IRQ_PR_37_POS
#define ICTL_IRQ_PR_37_IRQ_PR_37_POS      0
#endif

#ifndef ICTL_IRQ_PR_37_IRQ_PR_37_LEN
#define ICTL_IRQ_PR_37_IRQ_PR_37_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_37_IRQ_PR_37_R)
#define IRQ_PR_37_IRQ_PR_37_R        GetGroupBits32( (ICTL1_IRQ_PR_37_VAL),ICTL_IRQ_PR_37_IRQ_PR_37_POS, ICTL_IRQ_PR_37_IRQ_PR_37_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_37_IRQ_PR_37_W)
#define IRQ_PR_37_IRQ_PR_37_W(value) SetGroupBits32( (ICTL1_IRQ_PR_37_VAL),ICTL_IRQ_PR_37_IRQ_PR_37_POS, ICTL_IRQ_PR_37_IRQ_PR_37_LEN,value)
#endif

#define ICTL1_IRQ_PR_37_IRQ_PR_37_R        GetGroupBits32( (ICTL1_IRQ_PR_37_VAL),ICTL_IRQ_PR_37_IRQ_PR_37_POS, ICTL_IRQ_PR_37_IRQ_PR_37_LEN)

#define ICTL1_IRQ_PR_37_IRQ_PR_37_W(value) SetGroupBits32( (ICTL1_IRQ_PR_37_VAL),ICTL_IRQ_PR_37_IRQ_PR_37_POS, ICTL_IRQ_PR_37_IRQ_PR_37_LEN,value)


/* RSVD_irq_pr_37 */

#ifndef ICTL_IRQ_PR_37_RSVD_IRQ_PR_37_POS
#define ICTL_IRQ_PR_37_RSVD_IRQ_PR_37_POS      4
#endif

#ifndef ICTL_IRQ_PR_37_RSVD_IRQ_PR_37_LEN
#define ICTL_IRQ_PR_37_RSVD_IRQ_PR_37_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_37_RSVD_IRQ_PR_37_R)
#define IRQ_PR_37_RSVD_IRQ_PR_37_R        GetGroupBits32( (ICTL1_IRQ_PR_37_VAL),ICTL_IRQ_PR_37_RSVD_IRQ_PR_37_POS, ICTL_IRQ_PR_37_RSVD_IRQ_PR_37_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_37_RSVD_IRQ_PR_37_W)
#define IRQ_PR_37_RSVD_IRQ_PR_37_W(value) SetGroupBits32( (ICTL1_IRQ_PR_37_VAL),ICTL_IRQ_PR_37_RSVD_IRQ_PR_37_POS, ICTL_IRQ_PR_37_RSVD_IRQ_PR_37_LEN,value)
#endif

#define ICTL1_IRQ_PR_37_RSVD_IRQ_PR_37_R        GetGroupBits32( (ICTL1_IRQ_PR_37_VAL),ICTL_IRQ_PR_37_RSVD_IRQ_PR_37_POS, ICTL_IRQ_PR_37_RSVD_IRQ_PR_37_LEN)

#define ICTL1_IRQ_PR_37_RSVD_IRQ_PR_37_W(value) SetGroupBits32( (ICTL1_IRQ_PR_37_VAL),ICTL_IRQ_PR_37_RSVD_IRQ_PR_37_POS, ICTL_IRQ_PR_37_RSVD_IRQ_PR_37_LEN,value)


/* REGISTER: IRQ_RAWSTATUS_L */

#if defined(_V1) && !defined(IRQ_RAWSTATUS_L_OFFSET)
#define IRQ_RAWSTATUS_L_OFFSET 0x18
#endif

#if !defined(ICTL_IRQ_RAWSTATUS_L_OFFSET)
#define ICTL_IRQ_RAWSTATUS_L_OFFSET 0x18
#endif

#if defined(_V1) && !defined(IRQ_RAWSTATUS_L_REG)
#define IRQ_RAWSTATUS_L_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_RAWSTATUS_L_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_RAWSTATUS_L_VAL)
#define IRQ_RAWSTATUS_L_VAL  PREFIX_VAL(IRQ_RAWSTATUS_L_REG)
#endif

#define ICTL1_IRQ_RAWSTATUS_L_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_RAWSTATUS_L_OFFSET))
#define ICTL1_IRQ_RAWSTATUS_L_VAL  PREFIX_VAL(ICTL1_IRQ_RAWSTATUS_L_REG)

/* FIELDS: */

/* IRQ_RAWSTATUS_L */

#ifndef ICTL_IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_POS
#define ICTL_IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_POS      0
#endif

#ifndef ICTL_IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_LEN
#define ICTL_IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_R)
#define IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_R        GetGroupBits32( (ICTL1_IRQ_RAWSTATUS_L_VAL),ICTL_IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_POS, ICTL_IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_LEN)
#endif

#if defined(_V1) && !defined(IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_W)
#define IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_W(value) SetGroupBits32( (ICTL1_IRQ_RAWSTATUS_L_VAL),ICTL_IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_POS, ICTL_IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_LEN,value)
#endif

#define ICTL1_IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_R        GetGroupBits32( (ICTL1_IRQ_RAWSTATUS_L_VAL),ICTL_IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_POS, ICTL_IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_LEN)

#define ICTL1_IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_W(value) SetGroupBits32( (ICTL1_IRQ_RAWSTATUS_L_VAL),ICTL_IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_POS, ICTL_IRQ_RAWSTATUS_L_IRQ_RAWSTATUS_L_LEN,value)


/* REGISTER: IRQ_PR_38 */

#if defined(_V1) && !defined(IRQ_PR_38_OFFSET)
#define IRQ_PR_38_OFFSET 0x180
#endif

#if !defined(ICTL_IRQ_PR_38_OFFSET)
#define ICTL_IRQ_PR_38_OFFSET 0x180
#endif

#if defined(_V1) && !defined(IRQ_PR_38_REG)
#define IRQ_PR_38_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_38_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_38_VAL)
#define IRQ_PR_38_VAL  PREFIX_VAL(IRQ_PR_38_REG)
#endif

#define ICTL1_IRQ_PR_38_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_38_OFFSET))
#define ICTL1_IRQ_PR_38_VAL  PREFIX_VAL(ICTL1_IRQ_PR_38_REG)

/* FIELDS: */

/* irq_pr_38 */

#ifndef ICTL_IRQ_PR_38_IRQ_PR_38_POS
#define ICTL_IRQ_PR_38_IRQ_PR_38_POS      0
#endif

#ifndef ICTL_IRQ_PR_38_IRQ_PR_38_LEN
#define ICTL_IRQ_PR_38_IRQ_PR_38_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_38_IRQ_PR_38_R)
#define IRQ_PR_38_IRQ_PR_38_R        GetGroupBits32( (ICTL1_IRQ_PR_38_VAL),ICTL_IRQ_PR_38_IRQ_PR_38_POS, ICTL_IRQ_PR_38_IRQ_PR_38_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_38_IRQ_PR_38_W)
#define IRQ_PR_38_IRQ_PR_38_W(value) SetGroupBits32( (ICTL1_IRQ_PR_38_VAL),ICTL_IRQ_PR_38_IRQ_PR_38_POS, ICTL_IRQ_PR_38_IRQ_PR_38_LEN,value)
#endif

#define ICTL1_IRQ_PR_38_IRQ_PR_38_R        GetGroupBits32( (ICTL1_IRQ_PR_38_VAL),ICTL_IRQ_PR_38_IRQ_PR_38_POS, ICTL_IRQ_PR_38_IRQ_PR_38_LEN)

#define ICTL1_IRQ_PR_38_IRQ_PR_38_W(value) SetGroupBits32( (ICTL1_IRQ_PR_38_VAL),ICTL_IRQ_PR_38_IRQ_PR_38_POS, ICTL_IRQ_PR_38_IRQ_PR_38_LEN,value)


/* RSVD_irq_pr_38 */

#ifndef ICTL_IRQ_PR_38_RSVD_IRQ_PR_38_POS
#define ICTL_IRQ_PR_38_RSVD_IRQ_PR_38_POS      4
#endif

#ifndef ICTL_IRQ_PR_38_RSVD_IRQ_PR_38_LEN
#define ICTL_IRQ_PR_38_RSVD_IRQ_PR_38_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_38_RSVD_IRQ_PR_38_R)
#define IRQ_PR_38_RSVD_IRQ_PR_38_R        GetGroupBits32( (ICTL1_IRQ_PR_38_VAL),ICTL_IRQ_PR_38_RSVD_IRQ_PR_38_POS, ICTL_IRQ_PR_38_RSVD_IRQ_PR_38_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_38_RSVD_IRQ_PR_38_W)
#define IRQ_PR_38_RSVD_IRQ_PR_38_W(value) SetGroupBits32( (ICTL1_IRQ_PR_38_VAL),ICTL_IRQ_PR_38_RSVD_IRQ_PR_38_POS, ICTL_IRQ_PR_38_RSVD_IRQ_PR_38_LEN,value)
#endif

#define ICTL1_IRQ_PR_38_RSVD_IRQ_PR_38_R        GetGroupBits32( (ICTL1_IRQ_PR_38_VAL),ICTL_IRQ_PR_38_RSVD_IRQ_PR_38_POS, ICTL_IRQ_PR_38_RSVD_IRQ_PR_38_LEN)

#define ICTL1_IRQ_PR_38_RSVD_IRQ_PR_38_W(value) SetGroupBits32( (ICTL1_IRQ_PR_38_VAL),ICTL_IRQ_PR_38_RSVD_IRQ_PR_38_POS, ICTL_IRQ_PR_38_RSVD_IRQ_PR_38_LEN,value)


/* REGISTER: IRQ_PR_39 */

#if defined(_V1) && !defined(IRQ_PR_39_OFFSET)
#define IRQ_PR_39_OFFSET 0x184
#endif

#if !defined(ICTL_IRQ_PR_39_OFFSET)
#define ICTL_IRQ_PR_39_OFFSET 0x184
#endif

#if defined(_V1) && !defined(IRQ_PR_39_REG)
#define IRQ_PR_39_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_39_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_39_VAL)
#define IRQ_PR_39_VAL  PREFIX_VAL(IRQ_PR_39_REG)
#endif

#define ICTL1_IRQ_PR_39_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_39_OFFSET))
#define ICTL1_IRQ_PR_39_VAL  PREFIX_VAL(ICTL1_IRQ_PR_39_REG)

/* FIELDS: */

/* irq_pr_39 */

#ifndef ICTL_IRQ_PR_39_IRQ_PR_39_POS
#define ICTL_IRQ_PR_39_IRQ_PR_39_POS      0
#endif

#ifndef ICTL_IRQ_PR_39_IRQ_PR_39_LEN
#define ICTL_IRQ_PR_39_IRQ_PR_39_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_39_IRQ_PR_39_R)
#define IRQ_PR_39_IRQ_PR_39_R        GetGroupBits32( (ICTL1_IRQ_PR_39_VAL),ICTL_IRQ_PR_39_IRQ_PR_39_POS, ICTL_IRQ_PR_39_IRQ_PR_39_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_39_IRQ_PR_39_W)
#define IRQ_PR_39_IRQ_PR_39_W(value) SetGroupBits32( (ICTL1_IRQ_PR_39_VAL),ICTL_IRQ_PR_39_IRQ_PR_39_POS, ICTL_IRQ_PR_39_IRQ_PR_39_LEN,value)
#endif

#define ICTL1_IRQ_PR_39_IRQ_PR_39_R        GetGroupBits32( (ICTL1_IRQ_PR_39_VAL),ICTL_IRQ_PR_39_IRQ_PR_39_POS, ICTL_IRQ_PR_39_IRQ_PR_39_LEN)

#define ICTL1_IRQ_PR_39_IRQ_PR_39_W(value) SetGroupBits32( (ICTL1_IRQ_PR_39_VAL),ICTL_IRQ_PR_39_IRQ_PR_39_POS, ICTL_IRQ_PR_39_IRQ_PR_39_LEN,value)


/* RSVD_irq_pr_39 */

#ifndef ICTL_IRQ_PR_39_RSVD_IRQ_PR_39_POS
#define ICTL_IRQ_PR_39_RSVD_IRQ_PR_39_POS      4
#endif

#ifndef ICTL_IRQ_PR_39_RSVD_IRQ_PR_39_LEN
#define ICTL_IRQ_PR_39_RSVD_IRQ_PR_39_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_39_RSVD_IRQ_PR_39_R)
#define IRQ_PR_39_RSVD_IRQ_PR_39_R        GetGroupBits32( (ICTL1_IRQ_PR_39_VAL),ICTL_IRQ_PR_39_RSVD_IRQ_PR_39_POS, ICTL_IRQ_PR_39_RSVD_IRQ_PR_39_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_39_RSVD_IRQ_PR_39_W)
#define IRQ_PR_39_RSVD_IRQ_PR_39_W(value) SetGroupBits32( (ICTL1_IRQ_PR_39_VAL),ICTL_IRQ_PR_39_RSVD_IRQ_PR_39_POS, ICTL_IRQ_PR_39_RSVD_IRQ_PR_39_LEN,value)
#endif

#define ICTL1_IRQ_PR_39_RSVD_IRQ_PR_39_R        GetGroupBits32( (ICTL1_IRQ_PR_39_VAL),ICTL_IRQ_PR_39_RSVD_IRQ_PR_39_POS, ICTL_IRQ_PR_39_RSVD_IRQ_PR_39_LEN)

#define ICTL1_IRQ_PR_39_RSVD_IRQ_PR_39_W(value) SetGroupBits32( (ICTL1_IRQ_PR_39_VAL),ICTL_IRQ_PR_39_RSVD_IRQ_PR_39_POS, ICTL_IRQ_PR_39_RSVD_IRQ_PR_39_LEN,value)


/* REGISTER: IRQ_PR_40 */

#if defined(_V1) && !defined(IRQ_PR_40_OFFSET)
#define IRQ_PR_40_OFFSET 0x188
#endif

#if !defined(ICTL_IRQ_PR_40_OFFSET)
#define ICTL_IRQ_PR_40_OFFSET 0x188
#endif

#if defined(_V1) && !defined(IRQ_PR_40_REG)
#define IRQ_PR_40_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_40_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_40_VAL)
#define IRQ_PR_40_VAL  PREFIX_VAL(IRQ_PR_40_REG)
#endif

#define ICTL1_IRQ_PR_40_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_40_OFFSET))
#define ICTL1_IRQ_PR_40_VAL  PREFIX_VAL(ICTL1_IRQ_PR_40_REG)

/* FIELDS: */

/* irq_pr_40 */

#ifndef ICTL_IRQ_PR_40_IRQ_PR_40_POS
#define ICTL_IRQ_PR_40_IRQ_PR_40_POS      0
#endif

#ifndef ICTL_IRQ_PR_40_IRQ_PR_40_LEN
#define ICTL_IRQ_PR_40_IRQ_PR_40_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_40_IRQ_PR_40_R)
#define IRQ_PR_40_IRQ_PR_40_R        GetGroupBits32( (ICTL1_IRQ_PR_40_VAL),ICTL_IRQ_PR_40_IRQ_PR_40_POS, ICTL_IRQ_PR_40_IRQ_PR_40_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_40_IRQ_PR_40_W)
#define IRQ_PR_40_IRQ_PR_40_W(value) SetGroupBits32( (ICTL1_IRQ_PR_40_VAL),ICTL_IRQ_PR_40_IRQ_PR_40_POS, ICTL_IRQ_PR_40_IRQ_PR_40_LEN,value)
#endif

#define ICTL1_IRQ_PR_40_IRQ_PR_40_R        GetGroupBits32( (ICTL1_IRQ_PR_40_VAL),ICTL_IRQ_PR_40_IRQ_PR_40_POS, ICTL_IRQ_PR_40_IRQ_PR_40_LEN)

#define ICTL1_IRQ_PR_40_IRQ_PR_40_W(value) SetGroupBits32( (ICTL1_IRQ_PR_40_VAL),ICTL_IRQ_PR_40_IRQ_PR_40_POS, ICTL_IRQ_PR_40_IRQ_PR_40_LEN,value)


/* RSVD_irq_pr_40 */

#ifndef ICTL_IRQ_PR_40_RSVD_IRQ_PR_40_POS
#define ICTL_IRQ_PR_40_RSVD_IRQ_PR_40_POS      4
#endif

#ifndef ICTL_IRQ_PR_40_RSVD_IRQ_PR_40_LEN
#define ICTL_IRQ_PR_40_RSVD_IRQ_PR_40_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_40_RSVD_IRQ_PR_40_R)
#define IRQ_PR_40_RSVD_IRQ_PR_40_R        GetGroupBits32( (ICTL1_IRQ_PR_40_VAL),ICTL_IRQ_PR_40_RSVD_IRQ_PR_40_POS, ICTL_IRQ_PR_40_RSVD_IRQ_PR_40_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_40_RSVD_IRQ_PR_40_W)
#define IRQ_PR_40_RSVD_IRQ_PR_40_W(value) SetGroupBits32( (ICTL1_IRQ_PR_40_VAL),ICTL_IRQ_PR_40_RSVD_IRQ_PR_40_POS, ICTL_IRQ_PR_40_RSVD_IRQ_PR_40_LEN,value)
#endif

#define ICTL1_IRQ_PR_40_RSVD_IRQ_PR_40_R        GetGroupBits32( (ICTL1_IRQ_PR_40_VAL),ICTL_IRQ_PR_40_RSVD_IRQ_PR_40_POS, ICTL_IRQ_PR_40_RSVD_IRQ_PR_40_LEN)

#define ICTL1_IRQ_PR_40_RSVD_IRQ_PR_40_W(value) SetGroupBits32( (ICTL1_IRQ_PR_40_VAL),ICTL_IRQ_PR_40_RSVD_IRQ_PR_40_POS, ICTL_IRQ_PR_40_RSVD_IRQ_PR_40_LEN,value)


/* REGISTER: IRQ_PR_41 */

#if defined(_V1) && !defined(IRQ_PR_41_OFFSET)
#define IRQ_PR_41_OFFSET 0x18c
#endif

#if !defined(ICTL_IRQ_PR_41_OFFSET)
#define ICTL_IRQ_PR_41_OFFSET 0x18c
#endif

#if defined(_V1) && !defined(IRQ_PR_41_REG)
#define IRQ_PR_41_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_41_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_41_VAL)
#define IRQ_PR_41_VAL  PREFIX_VAL(IRQ_PR_41_REG)
#endif

#define ICTL1_IRQ_PR_41_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_41_OFFSET))
#define ICTL1_IRQ_PR_41_VAL  PREFIX_VAL(ICTL1_IRQ_PR_41_REG)

/* FIELDS: */

/* irq_pr_41 */

#ifndef ICTL_IRQ_PR_41_IRQ_PR_41_POS
#define ICTL_IRQ_PR_41_IRQ_PR_41_POS      0
#endif

#ifndef ICTL_IRQ_PR_41_IRQ_PR_41_LEN
#define ICTL_IRQ_PR_41_IRQ_PR_41_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_41_IRQ_PR_41_R)
#define IRQ_PR_41_IRQ_PR_41_R        GetGroupBits32( (ICTL1_IRQ_PR_41_VAL),ICTL_IRQ_PR_41_IRQ_PR_41_POS, ICTL_IRQ_PR_41_IRQ_PR_41_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_41_IRQ_PR_41_W)
#define IRQ_PR_41_IRQ_PR_41_W(value) SetGroupBits32( (ICTL1_IRQ_PR_41_VAL),ICTL_IRQ_PR_41_IRQ_PR_41_POS, ICTL_IRQ_PR_41_IRQ_PR_41_LEN,value)
#endif

#define ICTL1_IRQ_PR_41_IRQ_PR_41_R        GetGroupBits32( (ICTL1_IRQ_PR_41_VAL),ICTL_IRQ_PR_41_IRQ_PR_41_POS, ICTL_IRQ_PR_41_IRQ_PR_41_LEN)

#define ICTL1_IRQ_PR_41_IRQ_PR_41_W(value) SetGroupBits32( (ICTL1_IRQ_PR_41_VAL),ICTL_IRQ_PR_41_IRQ_PR_41_POS, ICTL_IRQ_PR_41_IRQ_PR_41_LEN,value)


/* RSVD_irq_pr_41 */

#ifndef ICTL_IRQ_PR_41_RSVD_IRQ_PR_41_POS
#define ICTL_IRQ_PR_41_RSVD_IRQ_PR_41_POS      4
#endif

#ifndef ICTL_IRQ_PR_41_RSVD_IRQ_PR_41_LEN
#define ICTL_IRQ_PR_41_RSVD_IRQ_PR_41_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_41_RSVD_IRQ_PR_41_R)
#define IRQ_PR_41_RSVD_IRQ_PR_41_R        GetGroupBits32( (ICTL1_IRQ_PR_41_VAL),ICTL_IRQ_PR_41_RSVD_IRQ_PR_41_POS, ICTL_IRQ_PR_41_RSVD_IRQ_PR_41_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_41_RSVD_IRQ_PR_41_W)
#define IRQ_PR_41_RSVD_IRQ_PR_41_W(value) SetGroupBits32( (ICTL1_IRQ_PR_41_VAL),ICTL_IRQ_PR_41_RSVD_IRQ_PR_41_POS, ICTL_IRQ_PR_41_RSVD_IRQ_PR_41_LEN,value)
#endif

#define ICTL1_IRQ_PR_41_RSVD_IRQ_PR_41_R        GetGroupBits32( (ICTL1_IRQ_PR_41_VAL),ICTL_IRQ_PR_41_RSVD_IRQ_PR_41_POS, ICTL_IRQ_PR_41_RSVD_IRQ_PR_41_LEN)

#define ICTL1_IRQ_PR_41_RSVD_IRQ_PR_41_W(value) SetGroupBits32( (ICTL1_IRQ_PR_41_VAL),ICTL_IRQ_PR_41_RSVD_IRQ_PR_41_POS, ICTL_IRQ_PR_41_RSVD_IRQ_PR_41_LEN,value)


/* REGISTER: IRQ_PR_42 */

#if defined(_V1) && !defined(IRQ_PR_42_OFFSET)
#define IRQ_PR_42_OFFSET 0x190
#endif

#if !defined(ICTL_IRQ_PR_42_OFFSET)
#define ICTL_IRQ_PR_42_OFFSET 0x190
#endif

#if defined(_V1) && !defined(IRQ_PR_42_REG)
#define IRQ_PR_42_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_42_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_42_VAL)
#define IRQ_PR_42_VAL  PREFIX_VAL(IRQ_PR_42_REG)
#endif

#define ICTL1_IRQ_PR_42_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_42_OFFSET))
#define ICTL1_IRQ_PR_42_VAL  PREFIX_VAL(ICTL1_IRQ_PR_42_REG)

/* FIELDS: */

/* irq_pr_42 */

#ifndef ICTL_IRQ_PR_42_IRQ_PR_42_POS
#define ICTL_IRQ_PR_42_IRQ_PR_42_POS      0
#endif

#ifndef ICTL_IRQ_PR_42_IRQ_PR_42_LEN
#define ICTL_IRQ_PR_42_IRQ_PR_42_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_42_IRQ_PR_42_R)
#define IRQ_PR_42_IRQ_PR_42_R        GetGroupBits32( (ICTL1_IRQ_PR_42_VAL),ICTL_IRQ_PR_42_IRQ_PR_42_POS, ICTL_IRQ_PR_42_IRQ_PR_42_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_42_IRQ_PR_42_W)
#define IRQ_PR_42_IRQ_PR_42_W(value) SetGroupBits32( (ICTL1_IRQ_PR_42_VAL),ICTL_IRQ_PR_42_IRQ_PR_42_POS, ICTL_IRQ_PR_42_IRQ_PR_42_LEN,value)
#endif

#define ICTL1_IRQ_PR_42_IRQ_PR_42_R        GetGroupBits32( (ICTL1_IRQ_PR_42_VAL),ICTL_IRQ_PR_42_IRQ_PR_42_POS, ICTL_IRQ_PR_42_IRQ_PR_42_LEN)

#define ICTL1_IRQ_PR_42_IRQ_PR_42_W(value) SetGroupBits32( (ICTL1_IRQ_PR_42_VAL),ICTL_IRQ_PR_42_IRQ_PR_42_POS, ICTL_IRQ_PR_42_IRQ_PR_42_LEN,value)


/* RSVD_irq_pr_42 */

#ifndef ICTL_IRQ_PR_42_RSVD_IRQ_PR_42_POS
#define ICTL_IRQ_PR_42_RSVD_IRQ_PR_42_POS      4
#endif

#ifndef ICTL_IRQ_PR_42_RSVD_IRQ_PR_42_LEN
#define ICTL_IRQ_PR_42_RSVD_IRQ_PR_42_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_42_RSVD_IRQ_PR_42_R)
#define IRQ_PR_42_RSVD_IRQ_PR_42_R        GetGroupBits32( (ICTL1_IRQ_PR_42_VAL),ICTL_IRQ_PR_42_RSVD_IRQ_PR_42_POS, ICTL_IRQ_PR_42_RSVD_IRQ_PR_42_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_42_RSVD_IRQ_PR_42_W)
#define IRQ_PR_42_RSVD_IRQ_PR_42_W(value) SetGroupBits32( (ICTL1_IRQ_PR_42_VAL),ICTL_IRQ_PR_42_RSVD_IRQ_PR_42_POS, ICTL_IRQ_PR_42_RSVD_IRQ_PR_42_LEN,value)
#endif

#define ICTL1_IRQ_PR_42_RSVD_IRQ_PR_42_R        GetGroupBits32( (ICTL1_IRQ_PR_42_VAL),ICTL_IRQ_PR_42_RSVD_IRQ_PR_42_POS, ICTL_IRQ_PR_42_RSVD_IRQ_PR_42_LEN)

#define ICTL1_IRQ_PR_42_RSVD_IRQ_PR_42_W(value) SetGroupBits32( (ICTL1_IRQ_PR_42_VAL),ICTL_IRQ_PR_42_RSVD_IRQ_PR_42_POS, ICTL_IRQ_PR_42_RSVD_IRQ_PR_42_LEN,value)


/* REGISTER: IRQ_PR_43 */

#if defined(_V1) && !defined(IRQ_PR_43_OFFSET)
#define IRQ_PR_43_OFFSET 0x194
#endif

#if !defined(ICTL_IRQ_PR_43_OFFSET)
#define ICTL_IRQ_PR_43_OFFSET 0x194
#endif

#if defined(_V1) && !defined(IRQ_PR_43_REG)
#define IRQ_PR_43_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_43_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_43_VAL)
#define IRQ_PR_43_VAL  PREFIX_VAL(IRQ_PR_43_REG)
#endif

#define ICTL1_IRQ_PR_43_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_43_OFFSET))
#define ICTL1_IRQ_PR_43_VAL  PREFIX_VAL(ICTL1_IRQ_PR_43_REG)

/* FIELDS: */

/* irq_pr_43 */

#ifndef ICTL_IRQ_PR_43_IRQ_PR_43_POS
#define ICTL_IRQ_PR_43_IRQ_PR_43_POS      0
#endif

#ifndef ICTL_IRQ_PR_43_IRQ_PR_43_LEN
#define ICTL_IRQ_PR_43_IRQ_PR_43_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_43_IRQ_PR_43_R)
#define IRQ_PR_43_IRQ_PR_43_R        GetGroupBits32( (ICTL1_IRQ_PR_43_VAL),ICTL_IRQ_PR_43_IRQ_PR_43_POS, ICTL_IRQ_PR_43_IRQ_PR_43_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_43_IRQ_PR_43_W)
#define IRQ_PR_43_IRQ_PR_43_W(value) SetGroupBits32( (ICTL1_IRQ_PR_43_VAL),ICTL_IRQ_PR_43_IRQ_PR_43_POS, ICTL_IRQ_PR_43_IRQ_PR_43_LEN,value)
#endif

#define ICTL1_IRQ_PR_43_IRQ_PR_43_R        GetGroupBits32( (ICTL1_IRQ_PR_43_VAL),ICTL_IRQ_PR_43_IRQ_PR_43_POS, ICTL_IRQ_PR_43_IRQ_PR_43_LEN)

#define ICTL1_IRQ_PR_43_IRQ_PR_43_W(value) SetGroupBits32( (ICTL1_IRQ_PR_43_VAL),ICTL_IRQ_PR_43_IRQ_PR_43_POS, ICTL_IRQ_PR_43_IRQ_PR_43_LEN,value)


/* RSVD_irq_pr_43 */

#ifndef ICTL_IRQ_PR_43_RSVD_IRQ_PR_43_POS
#define ICTL_IRQ_PR_43_RSVD_IRQ_PR_43_POS      4
#endif

#ifndef ICTL_IRQ_PR_43_RSVD_IRQ_PR_43_LEN
#define ICTL_IRQ_PR_43_RSVD_IRQ_PR_43_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_43_RSVD_IRQ_PR_43_R)
#define IRQ_PR_43_RSVD_IRQ_PR_43_R        GetGroupBits32( (ICTL1_IRQ_PR_43_VAL),ICTL_IRQ_PR_43_RSVD_IRQ_PR_43_POS, ICTL_IRQ_PR_43_RSVD_IRQ_PR_43_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_43_RSVD_IRQ_PR_43_W)
#define IRQ_PR_43_RSVD_IRQ_PR_43_W(value) SetGroupBits32( (ICTL1_IRQ_PR_43_VAL),ICTL_IRQ_PR_43_RSVD_IRQ_PR_43_POS, ICTL_IRQ_PR_43_RSVD_IRQ_PR_43_LEN,value)
#endif

#define ICTL1_IRQ_PR_43_RSVD_IRQ_PR_43_R        GetGroupBits32( (ICTL1_IRQ_PR_43_VAL),ICTL_IRQ_PR_43_RSVD_IRQ_PR_43_POS, ICTL_IRQ_PR_43_RSVD_IRQ_PR_43_LEN)

#define ICTL1_IRQ_PR_43_RSVD_IRQ_PR_43_W(value) SetGroupBits32( (ICTL1_IRQ_PR_43_VAL),ICTL_IRQ_PR_43_RSVD_IRQ_PR_43_POS, ICTL_IRQ_PR_43_RSVD_IRQ_PR_43_LEN,value)


/* REGISTER: IRQ_PR_44 */

#if defined(_V1) && !defined(IRQ_PR_44_OFFSET)
#define IRQ_PR_44_OFFSET 0x198
#endif

#if !defined(ICTL_IRQ_PR_44_OFFSET)
#define ICTL_IRQ_PR_44_OFFSET 0x198
#endif

#if defined(_V1) && !defined(IRQ_PR_44_REG)
#define IRQ_PR_44_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_44_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_44_VAL)
#define IRQ_PR_44_VAL  PREFIX_VAL(IRQ_PR_44_REG)
#endif

#define ICTL1_IRQ_PR_44_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_44_OFFSET))
#define ICTL1_IRQ_PR_44_VAL  PREFIX_VAL(ICTL1_IRQ_PR_44_REG)

/* FIELDS: */

/* irq_pr_44 */

#ifndef ICTL_IRQ_PR_44_IRQ_PR_44_POS
#define ICTL_IRQ_PR_44_IRQ_PR_44_POS      0
#endif

#ifndef ICTL_IRQ_PR_44_IRQ_PR_44_LEN
#define ICTL_IRQ_PR_44_IRQ_PR_44_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_44_IRQ_PR_44_R)
#define IRQ_PR_44_IRQ_PR_44_R        GetGroupBits32( (ICTL1_IRQ_PR_44_VAL),ICTL_IRQ_PR_44_IRQ_PR_44_POS, ICTL_IRQ_PR_44_IRQ_PR_44_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_44_IRQ_PR_44_W)
#define IRQ_PR_44_IRQ_PR_44_W(value) SetGroupBits32( (ICTL1_IRQ_PR_44_VAL),ICTL_IRQ_PR_44_IRQ_PR_44_POS, ICTL_IRQ_PR_44_IRQ_PR_44_LEN,value)
#endif

#define ICTL1_IRQ_PR_44_IRQ_PR_44_R        GetGroupBits32( (ICTL1_IRQ_PR_44_VAL),ICTL_IRQ_PR_44_IRQ_PR_44_POS, ICTL_IRQ_PR_44_IRQ_PR_44_LEN)

#define ICTL1_IRQ_PR_44_IRQ_PR_44_W(value) SetGroupBits32( (ICTL1_IRQ_PR_44_VAL),ICTL_IRQ_PR_44_IRQ_PR_44_POS, ICTL_IRQ_PR_44_IRQ_PR_44_LEN,value)


/* RSVD_irq_pr_44 */

#ifndef ICTL_IRQ_PR_44_RSVD_IRQ_PR_44_POS
#define ICTL_IRQ_PR_44_RSVD_IRQ_PR_44_POS      4
#endif

#ifndef ICTL_IRQ_PR_44_RSVD_IRQ_PR_44_LEN
#define ICTL_IRQ_PR_44_RSVD_IRQ_PR_44_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_44_RSVD_IRQ_PR_44_R)
#define IRQ_PR_44_RSVD_IRQ_PR_44_R        GetGroupBits32( (ICTL1_IRQ_PR_44_VAL),ICTL_IRQ_PR_44_RSVD_IRQ_PR_44_POS, ICTL_IRQ_PR_44_RSVD_IRQ_PR_44_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_44_RSVD_IRQ_PR_44_W)
#define IRQ_PR_44_RSVD_IRQ_PR_44_W(value) SetGroupBits32( (ICTL1_IRQ_PR_44_VAL),ICTL_IRQ_PR_44_RSVD_IRQ_PR_44_POS, ICTL_IRQ_PR_44_RSVD_IRQ_PR_44_LEN,value)
#endif

#define ICTL1_IRQ_PR_44_RSVD_IRQ_PR_44_R        GetGroupBits32( (ICTL1_IRQ_PR_44_VAL),ICTL_IRQ_PR_44_RSVD_IRQ_PR_44_POS, ICTL_IRQ_PR_44_RSVD_IRQ_PR_44_LEN)

#define ICTL1_IRQ_PR_44_RSVD_IRQ_PR_44_W(value) SetGroupBits32( (ICTL1_IRQ_PR_44_VAL),ICTL_IRQ_PR_44_RSVD_IRQ_PR_44_POS, ICTL_IRQ_PR_44_RSVD_IRQ_PR_44_LEN,value)


/* REGISTER: IRQ_PR_45 */

#if defined(_V1) && !defined(IRQ_PR_45_OFFSET)
#define IRQ_PR_45_OFFSET 0x19c
#endif

#if !defined(ICTL_IRQ_PR_45_OFFSET)
#define ICTL_IRQ_PR_45_OFFSET 0x19c
#endif

#if defined(_V1) && !defined(IRQ_PR_45_REG)
#define IRQ_PR_45_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_45_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_45_VAL)
#define IRQ_PR_45_VAL  PREFIX_VAL(IRQ_PR_45_REG)
#endif

#define ICTL1_IRQ_PR_45_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_45_OFFSET))
#define ICTL1_IRQ_PR_45_VAL  PREFIX_VAL(ICTL1_IRQ_PR_45_REG)

/* FIELDS: */

/* irq_pr_45 */

#ifndef ICTL_IRQ_PR_45_IRQ_PR_45_POS
#define ICTL_IRQ_PR_45_IRQ_PR_45_POS      0
#endif

#ifndef ICTL_IRQ_PR_45_IRQ_PR_45_LEN
#define ICTL_IRQ_PR_45_IRQ_PR_45_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_45_IRQ_PR_45_R)
#define IRQ_PR_45_IRQ_PR_45_R        GetGroupBits32( (ICTL1_IRQ_PR_45_VAL),ICTL_IRQ_PR_45_IRQ_PR_45_POS, ICTL_IRQ_PR_45_IRQ_PR_45_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_45_IRQ_PR_45_W)
#define IRQ_PR_45_IRQ_PR_45_W(value) SetGroupBits32( (ICTL1_IRQ_PR_45_VAL),ICTL_IRQ_PR_45_IRQ_PR_45_POS, ICTL_IRQ_PR_45_IRQ_PR_45_LEN,value)
#endif

#define ICTL1_IRQ_PR_45_IRQ_PR_45_R        GetGroupBits32( (ICTL1_IRQ_PR_45_VAL),ICTL_IRQ_PR_45_IRQ_PR_45_POS, ICTL_IRQ_PR_45_IRQ_PR_45_LEN)

#define ICTL1_IRQ_PR_45_IRQ_PR_45_W(value) SetGroupBits32( (ICTL1_IRQ_PR_45_VAL),ICTL_IRQ_PR_45_IRQ_PR_45_POS, ICTL_IRQ_PR_45_IRQ_PR_45_LEN,value)


/* RSVD_irq_pr_45 */

#ifndef ICTL_IRQ_PR_45_RSVD_IRQ_PR_45_POS
#define ICTL_IRQ_PR_45_RSVD_IRQ_PR_45_POS      4
#endif

#ifndef ICTL_IRQ_PR_45_RSVD_IRQ_PR_45_LEN
#define ICTL_IRQ_PR_45_RSVD_IRQ_PR_45_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_45_RSVD_IRQ_PR_45_R)
#define IRQ_PR_45_RSVD_IRQ_PR_45_R        GetGroupBits32( (ICTL1_IRQ_PR_45_VAL),ICTL_IRQ_PR_45_RSVD_IRQ_PR_45_POS, ICTL_IRQ_PR_45_RSVD_IRQ_PR_45_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_45_RSVD_IRQ_PR_45_W)
#define IRQ_PR_45_RSVD_IRQ_PR_45_W(value) SetGroupBits32( (ICTL1_IRQ_PR_45_VAL),ICTL_IRQ_PR_45_RSVD_IRQ_PR_45_POS, ICTL_IRQ_PR_45_RSVD_IRQ_PR_45_LEN,value)
#endif

#define ICTL1_IRQ_PR_45_RSVD_IRQ_PR_45_R        GetGroupBits32( (ICTL1_IRQ_PR_45_VAL),ICTL_IRQ_PR_45_RSVD_IRQ_PR_45_POS, ICTL_IRQ_PR_45_RSVD_IRQ_PR_45_LEN)

#define ICTL1_IRQ_PR_45_RSVD_IRQ_PR_45_W(value) SetGroupBits32( (ICTL1_IRQ_PR_45_VAL),ICTL_IRQ_PR_45_RSVD_IRQ_PR_45_POS, ICTL_IRQ_PR_45_RSVD_IRQ_PR_45_LEN,value)


/* REGISTER: IRQ_PR_46 */

#if defined(_V1) && !defined(IRQ_PR_46_OFFSET)
#define IRQ_PR_46_OFFSET 0x1a0
#endif

#if !defined(ICTL_IRQ_PR_46_OFFSET)
#define ICTL_IRQ_PR_46_OFFSET 0x1a0
#endif

#if defined(_V1) && !defined(IRQ_PR_46_REG)
#define IRQ_PR_46_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_46_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_46_VAL)
#define IRQ_PR_46_VAL  PREFIX_VAL(IRQ_PR_46_REG)
#endif

#define ICTL1_IRQ_PR_46_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_46_OFFSET))
#define ICTL1_IRQ_PR_46_VAL  PREFIX_VAL(ICTL1_IRQ_PR_46_REG)

/* FIELDS: */

/* irq_pr_46 */

#ifndef ICTL_IRQ_PR_46_IRQ_PR_46_POS
#define ICTL_IRQ_PR_46_IRQ_PR_46_POS      0
#endif

#ifndef ICTL_IRQ_PR_46_IRQ_PR_46_LEN
#define ICTL_IRQ_PR_46_IRQ_PR_46_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_46_IRQ_PR_46_R)
#define IRQ_PR_46_IRQ_PR_46_R        GetGroupBits32( (ICTL1_IRQ_PR_46_VAL),ICTL_IRQ_PR_46_IRQ_PR_46_POS, ICTL_IRQ_PR_46_IRQ_PR_46_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_46_IRQ_PR_46_W)
#define IRQ_PR_46_IRQ_PR_46_W(value) SetGroupBits32( (ICTL1_IRQ_PR_46_VAL),ICTL_IRQ_PR_46_IRQ_PR_46_POS, ICTL_IRQ_PR_46_IRQ_PR_46_LEN,value)
#endif

#define ICTL1_IRQ_PR_46_IRQ_PR_46_R        GetGroupBits32( (ICTL1_IRQ_PR_46_VAL),ICTL_IRQ_PR_46_IRQ_PR_46_POS, ICTL_IRQ_PR_46_IRQ_PR_46_LEN)

#define ICTL1_IRQ_PR_46_IRQ_PR_46_W(value) SetGroupBits32( (ICTL1_IRQ_PR_46_VAL),ICTL_IRQ_PR_46_IRQ_PR_46_POS, ICTL_IRQ_PR_46_IRQ_PR_46_LEN,value)


/* RSVD_irq_pr_46 */

#ifndef ICTL_IRQ_PR_46_RSVD_IRQ_PR_46_POS
#define ICTL_IRQ_PR_46_RSVD_IRQ_PR_46_POS      4
#endif

#ifndef ICTL_IRQ_PR_46_RSVD_IRQ_PR_46_LEN
#define ICTL_IRQ_PR_46_RSVD_IRQ_PR_46_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_46_RSVD_IRQ_PR_46_R)
#define IRQ_PR_46_RSVD_IRQ_PR_46_R        GetGroupBits32( (ICTL1_IRQ_PR_46_VAL),ICTL_IRQ_PR_46_RSVD_IRQ_PR_46_POS, ICTL_IRQ_PR_46_RSVD_IRQ_PR_46_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_46_RSVD_IRQ_PR_46_W)
#define IRQ_PR_46_RSVD_IRQ_PR_46_W(value) SetGroupBits32( (ICTL1_IRQ_PR_46_VAL),ICTL_IRQ_PR_46_RSVD_IRQ_PR_46_POS, ICTL_IRQ_PR_46_RSVD_IRQ_PR_46_LEN,value)
#endif

#define ICTL1_IRQ_PR_46_RSVD_IRQ_PR_46_R        GetGroupBits32( (ICTL1_IRQ_PR_46_VAL),ICTL_IRQ_PR_46_RSVD_IRQ_PR_46_POS, ICTL_IRQ_PR_46_RSVD_IRQ_PR_46_LEN)

#define ICTL1_IRQ_PR_46_RSVD_IRQ_PR_46_W(value) SetGroupBits32( (ICTL1_IRQ_PR_46_VAL),ICTL_IRQ_PR_46_RSVD_IRQ_PR_46_POS, ICTL_IRQ_PR_46_RSVD_IRQ_PR_46_LEN,value)


/* REGISTER: IRQ_PR_47 */

#if defined(_V1) && !defined(IRQ_PR_47_OFFSET)
#define IRQ_PR_47_OFFSET 0x1a4
#endif

#if !defined(ICTL_IRQ_PR_47_OFFSET)
#define ICTL_IRQ_PR_47_OFFSET 0x1a4
#endif

#if defined(_V1) && !defined(IRQ_PR_47_REG)
#define IRQ_PR_47_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_47_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_47_VAL)
#define IRQ_PR_47_VAL  PREFIX_VAL(IRQ_PR_47_REG)
#endif

#define ICTL1_IRQ_PR_47_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_47_OFFSET))
#define ICTL1_IRQ_PR_47_VAL  PREFIX_VAL(ICTL1_IRQ_PR_47_REG)

/* FIELDS: */

/* irq_pr_47 */

#ifndef ICTL_IRQ_PR_47_IRQ_PR_47_POS
#define ICTL_IRQ_PR_47_IRQ_PR_47_POS      0
#endif

#ifndef ICTL_IRQ_PR_47_IRQ_PR_47_LEN
#define ICTL_IRQ_PR_47_IRQ_PR_47_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_47_IRQ_PR_47_R)
#define IRQ_PR_47_IRQ_PR_47_R        GetGroupBits32( (ICTL1_IRQ_PR_47_VAL),ICTL_IRQ_PR_47_IRQ_PR_47_POS, ICTL_IRQ_PR_47_IRQ_PR_47_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_47_IRQ_PR_47_W)
#define IRQ_PR_47_IRQ_PR_47_W(value) SetGroupBits32( (ICTL1_IRQ_PR_47_VAL),ICTL_IRQ_PR_47_IRQ_PR_47_POS, ICTL_IRQ_PR_47_IRQ_PR_47_LEN,value)
#endif

#define ICTL1_IRQ_PR_47_IRQ_PR_47_R        GetGroupBits32( (ICTL1_IRQ_PR_47_VAL),ICTL_IRQ_PR_47_IRQ_PR_47_POS, ICTL_IRQ_PR_47_IRQ_PR_47_LEN)

#define ICTL1_IRQ_PR_47_IRQ_PR_47_W(value) SetGroupBits32( (ICTL1_IRQ_PR_47_VAL),ICTL_IRQ_PR_47_IRQ_PR_47_POS, ICTL_IRQ_PR_47_IRQ_PR_47_LEN,value)


/* RSVD_irq_pr_47 */

#ifndef ICTL_IRQ_PR_47_RSVD_IRQ_PR_47_POS
#define ICTL_IRQ_PR_47_RSVD_IRQ_PR_47_POS      4
#endif

#ifndef ICTL_IRQ_PR_47_RSVD_IRQ_PR_47_LEN
#define ICTL_IRQ_PR_47_RSVD_IRQ_PR_47_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_47_RSVD_IRQ_PR_47_R)
#define IRQ_PR_47_RSVD_IRQ_PR_47_R        GetGroupBits32( (ICTL1_IRQ_PR_47_VAL),ICTL_IRQ_PR_47_RSVD_IRQ_PR_47_POS, ICTL_IRQ_PR_47_RSVD_IRQ_PR_47_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_47_RSVD_IRQ_PR_47_W)
#define IRQ_PR_47_RSVD_IRQ_PR_47_W(value) SetGroupBits32( (ICTL1_IRQ_PR_47_VAL),ICTL_IRQ_PR_47_RSVD_IRQ_PR_47_POS, ICTL_IRQ_PR_47_RSVD_IRQ_PR_47_LEN,value)
#endif

#define ICTL1_IRQ_PR_47_RSVD_IRQ_PR_47_R        GetGroupBits32( (ICTL1_IRQ_PR_47_VAL),ICTL_IRQ_PR_47_RSVD_IRQ_PR_47_POS, ICTL_IRQ_PR_47_RSVD_IRQ_PR_47_LEN)

#define ICTL1_IRQ_PR_47_RSVD_IRQ_PR_47_W(value) SetGroupBits32( (ICTL1_IRQ_PR_47_VAL),ICTL_IRQ_PR_47_RSVD_IRQ_PR_47_POS, ICTL_IRQ_PR_47_RSVD_IRQ_PR_47_LEN,value)


/* REGISTER: IRQ_PR_48 */

#if defined(_V1) && !defined(IRQ_PR_48_OFFSET)
#define IRQ_PR_48_OFFSET 0x1a8
#endif

#if !defined(ICTL_IRQ_PR_48_OFFSET)
#define ICTL_IRQ_PR_48_OFFSET 0x1a8
#endif

#if defined(_V1) && !defined(IRQ_PR_48_REG)
#define IRQ_PR_48_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_48_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_48_VAL)
#define IRQ_PR_48_VAL  PREFIX_VAL(IRQ_PR_48_REG)
#endif

#define ICTL1_IRQ_PR_48_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_48_OFFSET))
#define ICTL1_IRQ_PR_48_VAL  PREFIX_VAL(ICTL1_IRQ_PR_48_REG)

/* FIELDS: */

/* irq_pr_48 */

#ifndef ICTL_IRQ_PR_48_IRQ_PR_48_POS
#define ICTL_IRQ_PR_48_IRQ_PR_48_POS      0
#endif

#ifndef ICTL_IRQ_PR_48_IRQ_PR_48_LEN
#define ICTL_IRQ_PR_48_IRQ_PR_48_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_48_IRQ_PR_48_R)
#define IRQ_PR_48_IRQ_PR_48_R        GetGroupBits32( (ICTL1_IRQ_PR_48_VAL),ICTL_IRQ_PR_48_IRQ_PR_48_POS, ICTL_IRQ_PR_48_IRQ_PR_48_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_48_IRQ_PR_48_W)
#define IRQ_PR_48_IRQ_PR_48_W(value) SetGroupBits32( (ICTL1_IRQ_PR_48_VAL),ICTL_IRQ_PR_48_IRQ_PR_48_POS, ICTL_IRQ_PR_48_IRQ_PR_48_LEN,value)
#endif

#define ICTL1_IRQ_PR_48_IRQ_PR_48_R        GetGroupBits32( (ICTL1_IRQ_PR_48_VAL),ICTL_IRQ_PR_48_IRQ_PR_48_POS, ICTL_IRQ_PR_48_IRQ_PR_48_LEN)

#define ICTL1_IRQ_PR_48_IRQ_PR_48_W(value) SetGroupBits32( (ICTL1_IRQ_PR_48_VAL),ICTL_IRQ_PR_48_IRQ_PR_48_POS, ICTL_IRQ_PR_48_IRQ_PR_48_LEN,value)


/* RSVD_irq_pr_48 */

#ifndef ICTL_IRQ_PR_48_RSVD_IRQ_PR_48_POS
#define ICTL_IRQ_PR_48_RSVD_IRQ_PR_48_POS      4
#endif

#ifndef ICTL_IRQ_PR_48_RSVD_IRQ_PR_48_LEN
#define ICTL_IRQ_PR_48_RSVD_IRQ_PR_48_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_48_RSVD_IRQ_PR_48_R)
#define IRQ_PR_48_RSVD_IRQ_PR_48_R        GetGroupBits32( (ICTL1_IRQ_PR_48_VAL),ICTL_IRQ_PR_48_RSVD_IRQ_PR_48_POS, ICTL_IRQ_PR_48_RSVD_IRQ_PR_48_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_48_RSVD_IRQ_PR_48_W)
#define IRQ_PR_48_RSVD_IRQ_PR_48_W(value) SetGroupBits32( (ICTL1_IRQ_PR_48_VAL),ICTL_IRQ_PR_48_RSVD_IRQ_PR_48_POS, ICTL_IRQ_PR_48_RSVD_IRQ_PR_48_LEN,value)
#endif

#define ICTL1_IRQ_PR_48_RSVD_IRQ_PR_48_R        GetGroupBits32( (ICTL1_IRQ_PR_48_VAL),ICTL_IRQ_PR_48_RSVD_IRQ_PR_48_POS, ICTL_IRQ_PR_48_RSVD_IRQ_PR_48_LEN)

#define ICTL1_IRQ_PR_48_RSVD_IRQ_PR_48_W(value) SetGroupBits32( (ICTL1_IRQ_PR_48_VAL),ICTL_IRQ_PR_48_RSVD_IRQ_PR_48_POS, ICTL_IRQ_PR_48_RSVD_IRQ_PR_48_LEN,value)


/* REGISTER: IRQ_PR_49 */

#if defined(_V1) && !defined(IRQ_PR_49_OFFSET)
#define IRQ_PR_49_OFFSET 0x1ac
#endif

#if !defined(ICTL_IRQ_PR_49_OFFSET)
#define ICTL_IRQ_PR_49_OFFSET 0x1ac
#endif

#if defined(_V1) && !defined(IRQ_PR_49_REG)
#define IRQ_PR_49_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_49_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_49_VAL)
#define IRQ_PR_49_VAL  PREFIX_VAL(IRQ_PR_49_REG)
#endif

#define ICTL1_IRQ_PR_49_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_49_OFFSET))
#define ICTL1_IRQ_PR_49_VAL  PREFIX_VAL(ICTL1_IRQ_PR_49_REG)

/* FIELDS: */

/* irq_pr_49 */

#ifndef ICTL_IRQ_PR_49_IRQ_PR_49_POS
#define ICTL_IRQ_PR_49_IRQ_PR_49_POS      0
#endif

#ifndef ICTL_IRQ_PR_49_IRQ_PR_49_LEN
#define ICTL_IRQ_PR_49_IRQ_PR_49_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_49_IRQ_PR_49_R)
#define IRQ_PR_49_IRQ_PR_49_R        GetGroupBits32( (ICTL1_IRQ_PR_49_VAL),ICTL_IRQ_PR_49_IRQ_PR_49_POS, ICTL_IRQ_PR_49_IRQ_PR_49_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_49_IRQ_PR_49_W)
#define IRQ_PR_49_IRQ_PR_49_W(value) SetGroupBits32( (ICTL1_IRQ_PR_49_VAL),ICTL_IRQ_PR_49_IRQ_PR_49_POS, ICTL_IRQ_PR_49_IRQ_PR_49_LEN,value)
#endif

#define ICTL1_IRQ_PR_49_IRQ_PR_49_R        GetGroupBits32( (ICTL1_IRQ_PR_49_VAL),ICTL_IRQ_PR_49_IRQ_PR_49_POS, ICTL_IRQ_PR_49_IRQ_PR_49_LEN)

#define ICTL1_IRQ_PR_49_IRQ_PR_49_W(value) SetGroupBits32( (ICTL1_IRQ_PR_49_VAL),ICTL_IRQ_PR_49_IRQ_PR_49_POS, ICTL_IRQ_PR_49_IRQ_PR_49_LEN,value)


/* RSVD_irq_pr_49 */

#ifndef ICTL_IRQ_PR_49_RSVD_IRQ_PR_49_POS
#define ICTL_IRQ_PR_49_RSVD_IRQ_PR_49_POS      4
#endif

#ifndef ICTL_IRQ_PR_49_RSVD_IRQ_PR_49_LEN
#define ICTL_IRQ_PR_49_RSVD_IRQ_PR_49_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_49_RSVD_IRQ_PR_49_R)
#define IRQ_PR_49_RSVD_IRQ_PR_49_R        GetGroupBits32( (ICTL1_IRQ_PR_49_VAL),ICTL_IRQ_PR_49_RSVD_IRQ_PR_49_POS, ICTL_IRQ_PR_49_RSVD_IRQ_PR_49_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_49_RSVD_IRQ_PR_49_W)
#define IRQ_PR_49_RSVD_IRQ_PR_49_W(value) SetGroupBits32( (ICTL1_IRQ_PR_49_VAL),ICTL_IRQ_PR_49_RSVD_IRQ_PR_49_POS, ICTL_IRQ_PR_49_RSVD_IRQ_PR_49_LEN,value)
#endif

#define ICTL1_IRQ_PR_49_RSVD_IRQ_PR_49_R        GetGroupBits32( (ICTL1_IRQ_PR_49_VAL),ICTL_IRQ_PR_49_RSVD_IRQ_PR_49_POS, ICTL_IRQ_PR_49_RSVD_IRQ_PR_49_LEN)

#define ICTL1_IRQ_PR_49_RSVD_IRQ_PR_49_W(value) SetGroupBits32( (ICTL1_IRQ_PR_49_VAL),ICTL_IRQ_PR_49_RSVD_IRQ_PR_49_POS, ICTL_IRQ_PR_49_RSVD_IRQ_PR_49_LEN,value)


/* REGISTER: IRQ_PR_50 */

#if defined(_V1) && !defined(IRQ_PR_50_OFFSET)
#define IRQ_PR_50_OFFSET 0x1b0
#endif

#if !defined(ICTL_IRQ_PR_50_OFFSET)
#define ICTL_IRQ_PR_50_OFFSET 0x1b0
#endif

#if defined(_V1) && !defined(IRQ_PR_50_REG)
#define IRQ_PR_50_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_50_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_50_VAL)
#define IRQ_PR_50_VAL  PREFIX_VAL(IRQ_PR_50_REG)
#endif

#define ICTL1_IRQ_PR_50_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_50_OFFSET))
#define ICTL1_IRQ_PR_50_VAL  PREFIX_VAL(ICTL1_IRQ_PR_50_REG)

/* FIELDS: */

/* irq_pr_50 */

#ifndef ICTL_IRQ_PR_50_IRQ_PR_50_POS
#define ICTL_IRQ_PR_50_IRQ_PR_50_POS      0
#endif

#ifndef ICTL_IRQ_PR_50_IRQ_PR_50_LEN
#define ICTL_IRQ_PR_50_IRQ_PR_50_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_50_IRQ_PR_50_R)
#define IRQ_PR_50_IRQ_PR_50_R        GetGroupBits32( (ICTL1_IRQ_PR_50_VAL),ICTL_IRQ_PR_50_IRQ_PR_50_POS, ICTL_IRQ_PR_50_IRQ_PR_50_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_50_IRQ_PR_50_W)
#define IRQ_PR_50_IRQ_PR_50_W(value) SetGroupBits32( (ICTL1_IRQ_PR_50_VAL),ICTL_IRQ_PR_50_IRQ_PR_50_POS, ICTL_IRQ_PR_50_IRQ_PR_50_LEN,value)
#endif

#define ICTL1_IRQ_PR_50_IRQ_PR_50_R        GetGroupBits32( (ICTL1_IRQ_PR_50_VAL),ICTL_IRQ_PR_50_IRQ_PR_50_POS, ICTL_IRQ_PR_50_IRQ_PR_50_LEN)

#define ICTL1_IRQ_PR_50_IRQ_PR_50_W(value) SetGroupBits32( (ICTL1_IRQ_PR_50_VAL),ICTL_IRQ_PR_50_IRQ_PR_50_POS, ICTL_IRQ_PR_50_IRQ_PR_50_LEN,value)


/* RSVD_irq_pr_50 */

#ifndef ICTL_IRQ_PR_50_RSVD_IRQ_PR_50_POS
#define ICTL_IRQ_PR_50_RSVD_IRQ_PR_50_POS      4
#endif

#ifndef ICTL_IRQ_PR_50_RSVD_IRQ_PR_50_LEN
#define ICTL_IRQ_PR_50_RSVD_IRQ_PR_50_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_50_RSVD_IRQ_PR_50_R)
#define IRQ_PR_50_RSVD_IRQ_PR_50_R        GetGroupBits32( (ICTL1_IRQ_PR_50_VAL),ICTL_IRQ_PR_50_RSVD_IRQ_PR_50_POS, ICTL_IRQ_PR_50_RSVD_IRQ_PR_50_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_50_RSVD_IRQ_PR_50_W)
#define IRQ_PR_50_RSVD_IRQ_PR_50_W(value) SetGroupBits32( (ICTL1_IRQ_PR_50_VAL),ICTL_IRQ_PR_50_RSVD_IRQ_PR_50_POS, ICTL_IRQ_PR_50_RSVD_IRQ_PR_50_LEN,value)
#endif

#define ICTL1_IRQ_PR_50_RSVD_IRQ_PR_50_R        GetGroupBits32( (ICTL1_IRQ_PR_50_VAL),ICTL_IRQ_PR_50_RSVD_IRQ_PR_50_POS, ICTL_IRQ_PR_50_RSVD_IRQ_PR_50_LEN)

#define ICTL1_IRQ_PR_50_RSVD_IRQ_PR_50_W(value) SetGroupBits32( (ICTL1_IRQ_PR_50_VAL),ICTL_IRQ_PR_50_RSVD_IRQ_PR_50_POS, ICTL_IRQ_PR_50_RSVD_IRQ_PR_50_LEN,value)


/* REGISTER: IRQ_PR_51 */

#if defined(_V1) && !defined(IRQ_PR_51_OFFSET)
#define IRQ_PR_51_OFFSET 0x1b4
#endif

#if !defined(ICTL_IRQ_PR_51_OFFSET)
#define ICTL_IRQ_PR_51_OFFSET 0x1b4
#endif

#if defined(_V1) && !defined(IRQ_PR_51_REG)
#define IRQ_PR_51_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_51_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_51_VAL)
#define IRQ_PR_51_VAL  PREFIX_VAL(IRQ_PR_51_REG)
#endif

#define ICTL1_IRQ_PR_51_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_51_OFFSET))
#define ICTL1_IRQ_PR_51_VAL  PREFIX_VAL(ICTL1_IRQ_PR_51_REG)

/* FIELDS: */

/* irq_pr_51 */

#ifndef ICTL_IRQ_PR_51_IRQ_PR_51_POS
#define ICTL_IRQ_PR_51_IRQ_PR_51_POS      0
#endif

#ifndef ICTL_IRQ_PR_51_IRQ_PR_51_LEN
#define ICTL_IRQ_PR_51_IRQ_PR_51_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_51_IRQ_PR_51_R)
#define IRQ_PR_51_IRQ_PR_51_R        GetGroupBits32( (ICTL1_IRQ_PR_51_VAL),ICTL_IRQ_PR_51_IRQ_PR_51_POS, ICTL_IRQ_PR_51_IRQ_PR_51_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_51_IRQ_PR_51_W)
#define IRQ_PR_51_IRQ_PR_51_W(value) SetGroupBits32( (ICTL1_IRQ_PR_51_VAL),ICTL_IRQ_PR_51_IRQ_PR_51_POS, ICTL_IRQ_PR_51_IRQ_PR_51_LEN,value)
#endif

#define ICTL1_IRQ_PR_51_IRQ_PR_51_R        GetGroupBits32( (ICTL1_IRQ_PR_51_VAL),ICTL_IRQ_PR_51_IRQ_PR_51_POS, ICTL_IRQ_PR_51_IRQ_PR_51_LEN)

#define ICTL1_IRQ_PR_51_IRQ_PR_51_W(value) SetGroupBits32( (ICTL1_IRQ_PR_51_VAL),ICTL_IRQ_PR_51_IRQ_PR_51_POS, ICTL_IRQ_PR_51_IRQ_PR_51_LEN,value)


/* RSVD_irq_pr_51 */

#ifndef ICTL_IRQ_PR_51_RSVD_IRQ_PR_51_POS
#define ICTL_IRQ_PR_51_RSVD_IRQ_PR_51_POS      4
#endif

#ifndef ICTL_IRQ_PR_51_RSVD_IRQ_PR_51_LEN
#define ICTL_IRQ_PR_51_RSVD_IRQ_PR_51_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_51_RSVD_IRQ_PR_51_R)
#define IRQ_PR_51_RSVD_IRQ_PR_51_R        GetGroupBits32( (ICTL1_IRQ_PR_51_VAL),ICTL_IRQ_PR_51_RSVD_IRQ_PR_51_POS, ICTL_IRQ_PR_51_RSVD_IRQ_PR_51_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_51_RSVD_IRQ_PR_51_W)
#define IRQ_PR_51_RSVD_IRQ_PR_51_W(value) SetGroupBits32( (ICTL1_IRQ_PR_51_VAL),ICTL_IRQ_PR_51_RSVD_IRQ_PR_51_POS, ICTL_IRQ_PR_51_RSVD_IRQ_PR_51_LEN,value)
#endif

#define ICTL1_IRQ_PR_51_RSVD_IRQ_PR_51_R        GetGroupBits32( (ICTL1_IRQ_PR_51_VAL),ICTL_IRQ_PR_51_RSVD_IRQ_PR_51_POS, ICTL_IRQ_PR_51_RSVD_IRQ_PR_51_LEN)

#define ICTL1_IRQ_PR_51_RSVD_IRQ_PR_51_W(value) SetGroupBits32( (ICTL1_IRQ_PR_51_VAL),ICTL_IRQ_PR_51_RSVD_IRQ_PR_51_POS, ICTL_IRQ_PR_51_RSVD_IRQ_PR_51_LEN,value)


/* REGISTER: IRQ_PR_52 */

#if defined(_V1) && !defined(IRQ_PR_52_OFFSET)
#define IRQ_PR_52_OFFSET 0x1b8
#endif

#if !defined(ICTL_IRQ_PR_52_OFFSET)
#define ICTL_IRQ_PR_52_OFFSET 0x1b8
#endif

#if defined(_V1) && !defined(IRQ_PR_52_REG)
#define IRQ_PR_52_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_52_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_52_VAL)
#define IRQ_PR_52_VAL  PREFIX_VAL(IRQ_PR_52_REG)
#endif

#define ICTL1_IRQ_PR_52_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_52_OFFSET))
#define ICTL1_IRQ_PR_52_VAL  PREFIX_VAL(ICTL1_IRQ_PR_52_REG)

/* FIELDS: */

/* irq_pr_52 */

#ifndef ICTL_IRQ_PR_52_IRQ_PR_52_POS
#define ICTL_IRQ_PR_52_IRQ_PR_52_POS      0
#endif

#ifndef ICTL_IRQ_PR_52_IRQ_PR_52_LEN
#define ICTL_IRQ_PR_52_IRQ_PR_52_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_52_IRQ_PR_52_R)
#define IRQ_PR_52_IRQ_PR_52_R        GetGroupBits32( (ICTL1_IRQ_PR_52_VAL),ICTL_IRQ_PR_52_IRQ_PR_52_POS, ICTL_IRQ_PR_52_IRQ_PR_52_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_52_IRQ_PR_52_W)
#define IRQ_PR_52_IRQ_PR_52_W(value) SetGroupBits32( (ICTL1_IRQ_PR_52_VAL),ICTL_IRQ_PR_52_IRQ_PR_52_POS, ICTL_IRQ_PR_52_IRQ_PR_52_LEN,value)
#endif

#define ICTL1_IRQ_PR_52_IRQ_PR_52_R        GetGroupBits32( (ICTL1_IRQ_PR_52_VAL),ICTL_IRQ_PR_52_IRQ_PR_52_POS, ICTL_IRQ_PR_52_IRQ_PR_52_LEN)

#define ICTL1_IRQ_PR_52_IRQ_PR_52_W(value) SetGroupBits32( (ICTL1_IRQ_PR_52_VAL),ICTL_IRQ_PR_52_IRQ_PR_52_POS, ICTL_IRQ_PR_52_IRQ_PR_52_LEN,value)


/* RSVD_irq_pr_52 */

#ifndef ICTL_IRQ_PR_52_RSVD_IRQ_PR_52_POS
#define ICTL_IRQ_PR_52_RSVD_IRQ_PR_52_POS      4
#endif

#ifndef ICTL_IRQ_PR_52_RSVD_IRQ_PR_52_LEN
#define ICTL_IRQ_PR_52_RSVD_IRQ_PR_52_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_52_RSVD_IRQ_PR_52_R)
#define IRQ_PR_52_RSVD_IRQ_PR_52_R        GetGroupBits32( (ICTL1_IRQ_PR_52_VAL),ICTL_IRQ_PR_52_RSVD_IRQ_PR_52_POS, ICTL_IRQ_PR_52_RSVD_IRQ_PR_52_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_52_RSVD_IRQ_PR_52_W)
#define IRQ_PR_52_RSVD_IRQ_PR_52_W(value) SetGroupBits32( (ICTL1_IRQ_PR_52_VAL),ICTL_IRQ_PR_52_RSVD_IRQ_PR_52_POS, ICTL_IRQ_PR_52_RSVD_IRQ_PR_52_LEN,value)
#endif

#define ICTL1_IRQ_PR_52_RSVD_IRQ_PR_52_R        GetGroupBits32( (ICTL1_IRQ_PR_52_VAL),ICTL_IRQ_PR_52_RSVD_IRQ_PR_52_POS, ICTL_IRQ_PR_52_RSVD_IRQ_PR_52_LEN)

#define ICTL1_IRQ_PR_52_RSVD_IRQ_PR_52_W(value) SetGroupBits32( (ICTL1_IRQ_PR_52_VAL),ICTL_IRQ_PR_52_RSVD_IRQ_PR_52_POS, ICTL_IRQ_PR_52_RSVD_IRQ_PR_52_LEN,value)


/* REGISTER: IRQ_PR_53 */

#if defined(_V1) && !defined(IRQ_PR_53_OFFSET)
#define IRQ_PR_53_OFFSET 0x1bc
#endif

#if !defined(ICTL_IRQ_PR_53_OFFSET)
#define ICTL_IRQ_PR_53_OFFSET 0x1bc
#endif

#if defined(_V1) && !defined(IRQ_PR_53_REG)
#define IRQ_PR_53_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_53_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_53_VAL)
#define IRQ_PR_53_VAL  PREFIX_VAL(IRQ_PR_53_REG)
#endif

#define ICTL1_IRQ_PR_53_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_53_OFFSET))
#define ICTL1_IRQ_PR_53_VAL  PREFIX_VAL(ICTL1_IRQ_PR_53_REG)

/* FIELDS: */

/* irq_pr_53 */

#ifndef ICTL_IRQ_PR_53_IRQ_PR_53_POS
#define ICTL_IRQ_PR_53_IRQ_PR_53_POS      0
#endif

#ifndef ICTL_IRQ_PR_53_IRQ_PR_53_LEN
#define ICTL_IRQ_PR_53_IRQ_PR_53_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_53_IRQ_PR_53_R)
#define IRQ_PR_53_IRQ_PR_53_R        GetGroupBits32( (ICTL1_IRQ_PR_53_VAL),ICTL_IRQ_PR_53_IRQ_PR_53_POS, ICTL_IRQ_PR_53_IRQ_PR_53_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_53_IRQ_PR_53_W)
#define IRQ_PR_53_IRQ_PR_53_W(value) SetGroupBits32( (ICTL1_IRQ_PR_53_VAL),ICTL_IRQ_PR_53_IRQ_PR_53_POS, ICTL_IRQ_PR_53_IRQ_PR_53_LEN,value)
#endif

#define ICTL1_IRQ_PR_53_IRQ_PR_53_R        GetGroupBits32( (ICTL1_IRQ_PR_53_VAL),ICTL_IRQ_PR_53_IRQ_PR_53_POS, ICTL_IRQ_PR_53_IRQ_PR_53_LEN)

#define ICTL1_IRQ_PR_53_IRQ_PR_53_W(value) SetGroupBits32( (ICTL1_IRQ_PR_53_VAL),ICTL_IRQ_PR_53_IRQ_PR_53_POS, ICTL_IRQ_PR_53_IRQ_PR_53_LEN,value)


/* RSVD_irq_pr_53 */

#ifndef ICTL_IRQ_PR_53_RSVD_IRQ_PR_53_POS
#define ICTL_IRQ_PR_53_RSVD_IRQ_PR_53_POS      4
#endif

#ifndef ICTL_IRQ_PR_53_RSVD_IRQ_PR_53_LEN
#define ICTL_IRQ_PR_53_RSVD_IRQ_PR_53_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_53_RSVD_IRQ_PR_53_R)
#define IRQ_PR_53_RSVD_IRQ_PR_53_R        GetGroupBits32( (ICTL1_IRQ_PR_53_VAL),ICTL_IRQ_PR_53_RSVD_IRQ_PR_53_POS, ICTL_IRQ_PR_53_RSVD_IRQ_PR_53_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_53_RSVD_IRQ_PR_53_W)
#define IRQ_PR_53_RSVD_IRQ_PR_53_W(value) SetGroupBits32( (ICTL1_IRQ_PR_53_VAL),ICTL_IRQ_PR_53_RSVD_IRQ_PR_53_POS, ICTL_IRQ_PR_53_RSVD_IRQ_PR_53_LEN,value)
#endif

#define ICTL1_IRQ_PR_53_RSVD_IRQ_PR_53_R        GetGroupBits32( (ICTL1_IRQ_PR_53_VAL),ICTL_IRQ_PR_53_RSVD_IRQ_PR_53_POS, ICTL_IRQ_PR_53_RSVD_IRQ_PR_53_LEN)

#define ICTL1_IRQ_PR_53_RSVD_IRQ_PR_53_W(value) SetGroupBits32( (ICTL1_IRQ_PR_53_VAL),ICTL_IRQ_PR_53_RSVD_IRQ_PR_53_POS, ICTL_IRQ_PR_53_RSVD_IRQ_PR_53_LEN,value)


/* REGISTER: IRQ_RAWSTATUS_H */

#if defined(_V1) && !defined(IRQ_RAWSTATUS_H_OFFSET)
#define IRQ_RAWSTATUS_H_OFFSET 0x1c
#endif

#if !defined(ICTL_IRQ_RAWSTATUS_H_OFFSET)
#define ICTL_IRQ_RAWSTATUS_H_OFFSET 0x1c
#endif

#if defined(_V1) && !defined(IRQ_RAWSTATUS_H_REG)
#define IRQ_RAWSTATUS_H_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_RAWSTATUS_H_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_RAWSTATUS_H_VAL)
#define IRQ_RAWSTATUS_H_VAL  PREFIX_VAL(IRQ_RAWSTATUS_H_REG)
#endif

#define ICTL1_IRQ_RAWSTATUS_H_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_RAWSTATUS_H_OFFSET))
#define ICTL1_IRQ_RAWSTATUS_H_VAL  PREFIX_VAL(ICTL1_IRQ_RAWSTATUS_H_REG)

/* FIELDS: */

/* IRQ_RAWSTATUS_H */

#ifndef ICTL_IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_POS
#define ICTL_IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_POS      0
#endif

#ifndef ICTL_IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_LEN
#define ICTL_IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_R)
#define IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_R        GetGroupBits32( (ICTL1_IRQ_RAWSTATUS_H_VAL),ICTL_IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_POS, ICTL_IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_LEN)
#endif

#if defined(_V1) && !defined(IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_W)
#define IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_W(value) SetGroupBits32( (ICTL1_IRQ_RAWSTATUS_H_VAL),ICTL_IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_POS, ICTL_IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_LEN,value)
#endif

#define ICTL1_IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_R        GetGroupBits32( (ICTL1_IRQ_RAWSTATUS_H_VAL),ICTL_IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_POS, ICTL_IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_LEN)

#define ICTL1_IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_W(value) SetGroupBits32( (ICTL1_IRQ_RAWSTATUS_H_VAL),ICTL_IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_POS, ICTL_IRQ_RAWSTATUS_H_IRQ_RAWSTATUS_H_LEN,value)


/* REGISTER: IRQ_PR_54 */

#if defined(_V1) && !defined(IRQ_PR_54_OFFSET)
#define IRQ_PR_54_OFFSET 0x1c0
#endif

#if !defined(ICTL_IRQ_PR_54_OFFSET)
#define ICTL_IRQ_PR_54_OFFSET 0x1c0
#endif

#if defined(_V1) && !defined(IRQ_PR_54_REG)
#define IRQ_PR_54_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_54_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_54_VAL)
#define IRQ_PR_54_VAL  PREFIX_VAL(IRQ_PR_54_REG)
#endif

#define ICTL1_IRQ_PR_54_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_54_OFFSET))
#define ICTL1_IRQ_PR_54_VAL  PREFIX_VAL(ICTL1_IRQ_PR_54_REG)

/* FIELDS: */

/* irq_pr_54 */

#ifndef ICTL_IRQ_PR_54_IRQ_PR_54_POS
#define ICTL_IRQ_PR_54_IRQ_PR_54_POS      0
#endif

#ifndef ICTL_IRQ_PR_54_IRQ_PR_54_LEN
#define ICTL_IRQ_PR_54_IRQ_PR_54_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_54_IRQ_PR_54_R)
#define IRQ_PR_54_IRQ_PR_54_R        GetGroupBits32( (ICTL1_IRQ_PR_54_VAL),ICTL_IRQ_PR_54_IRQ_PR_54_POS, ICTL_IRQ_PR_54_IRQ_PR_54_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_54_IRQ_PR_54_W)
#define IRQ_PR_54_IRQ_PR_54_W(value) SetGroupBits32( (ICTL1_IRQ_PR_54_VAL),ICTL_IRQ_PR_54_IRQ_PR_54_POS, ICTL_IRQ_PR_54_IRQ_PR_54_LEN,value)
#endif

#define ICTL1_IRQ_PR_54_IRQ_PR_54_R        GetGroupBits32( (ICTL1_IRQ_PR_54_VAL),ICTL_IRQ_PR_54_IRQ_PR_54_POS, ICTL_IRQ_PR_54_IRQ_PR_54_LEN)

#define ICTL1_IRQ_PR_54_IRQ_PR_54_W(value) SetGroupBits32( (ICTL1_IRQ_PR_54_VAL),ICTL_IRQ_PR_54_IRQ_PR_54_POS, ICTL_IRQ_PR_54_IRQ_PR_54_LEN,value)


/* RSVD_irq_pr_54 */

#ifndef ICTL_IRQ_PR_54_RSVD_IRQ_PR_54_POS
#define ICTL_IRQ_PR_54_RSVD_IRQ_PR_54_POS      4
#endif

#ifndef ICTL_IRQ_PR_54_RSVD_IRQ_PR_54_LEN
#define ICTL_IRQ_PR_54_RSVD_IRQ_PR_54_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_54_RSVD_IRQ_PR_54_R)
#define IRQ_PR_54_RSVD_IRQ_PR_54_R        GetGroupBits32( (ICTL1_IRQ_PR_54_VAL),ICTL_IRQ_PR_54_RSVD_IRQ_PR_54_POS, ICTL_IRQ_PR_54_RSVD_IRQ_PR_54_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_54_RSVD_IRQ_PR_54_W)
#define IRQ_PR_54_RSVD_IRQ_PR_54_W(value) SetGroupBits32( (ICTL1_IRQ_PR_54_VAL),ICTL_IRQ_PR_54_RSVD_IRQ_PR_54_POS, ICTL_IRQ_PR_54_RSVD_IRQ_PR_54_LEN,value)
#endif

#define ICTL1_IRQ_PR_54_RSVD_IRQ_PR_54_R        GetGroupBits32( (ICTL1_IRQ_PR_54_VAL),ICTL_IRQ_PR_54_RSVD_IRQ_PR_54_POS, ICTL_IRQ_PR_54_RSVD_IRQ_PR_54_LEN)

#define ICTL1_IRQ_PR_54_RSVD_IRQ_PR_54_W(value) SetGroupBits32( (ICTL1_IRQ_PR_54_VAL),ICTL_IRQ_PR_54_RSVD_IRQ_PR_54_POS, ICTL_IRQ_PR_54_RSVD_IRQ_PR_54_LEN,value)


/* REGISTER: IRQ_PR_55 */

#if defined(_V1) && !defined(IRQ_PR_55_OFFSET)
#define IRQ_PR_55_OFFSET 0x1c4
#endif

#if !defined(ICTL_IRQ_PR_55_OFFSET)
#define ICTL_IRQ_PR_55_OFFSET 0x1c4
#endif

#if defined(_V1) && !defined(IRQ_PR_55_REG)
#define IRQ_PR_55_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_55_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_55_VAL)
#define IRQ_PR_55_VAL  PREFIX_VAL(IRQ_PR_55_REG)
#endif

#define ICTL1_IRQ_PR_55_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_55_OFFSET))
#define ICTL1_IRQ_PR_55_VAL  PREFIX_VAL(ICTL1_IRQ_PR_55_REG)

/* FIELDS: */

/* irq_pr_55 */

#ifndef ICTL_IRQ_PR_55_IRQ_PR_55_POS
#define ICTL_IRQ_PR_55_IRQ_PR_55_POS      0
#endif

#ifndef ICTL_IRQ_PR_55_IRQ_PR_55_LEN
#define ICTL_IRQ_PR_55_IRQ_PR_55_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_55_IRQ_PR_55_R)
#define IRQ_PR_55_IRQ_PR_55_R        GetGroupBits32( (ICTL1_IRQ_PR_55_VAL),ICTL_IRQ_PR_55_IRQ_PR_55_POS, ICTL_IRQ_PR_55_IRQ_PR_55_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_55_IRQ_PR_55_W)
#define IRQ_PR_55_IRQ_PR_55_W(value) SetGroupBits32( (ICTL1_IRQ_PR_55_VAL),ICTL_IRQ_PR_55_IRQ_PR_55_POS, ICTL_IRQ_PR_55_IRQ_PR_55_LEN,value)
#endif

#define ICTL1_IRQ_PR_55_IRQ_PR_55_R        GetGroupBits32( (ICTL1_IRQ_PR_55_VAL),ICTL_IRQ_PR_55_IRQ_PR_55_POS, ICTL_IRQ_PR_55_IRQ_PR_55_LEN)

#define ICTL1_IRQ_PR_55_IRQ_PR_55_W(value) SetGroupBits32( (ICTL1_IRQ_PR_55_VAL),ICTL_IRQ_PR_55_IRQ_PR_55_POS, ICTL_IRQ_PR_55_IRQ_PR_55_LEN,value)


/* RSVD_irq_pr_55 */

#ifndef ICTL_IRQ_PR_55_RSVD_IRQ_PR_55_POS
#define ICTL_IRQ_PR_55_RSVD_IRQ_PR_55_POS      4
#endif

#ifndef ICTL_IRQ_PR_55_RSVD_IRQ_PR_55_LEN
#define ICTL_IRQ_PR_55_RSVD_IRQ_PR_55_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_55_RSVD_IRQ_PR_55_R)
#define IRQ_PR_55_RSVD_IRQ_PR_55_R        GetGroupBits32( (ICTL1_IRQ_PR_55_VAL),ICTL_IRQ_PR_55_RSVD_IRQ_PR_55_POS, ICTL_IRQ_PR_55_RSVD_IRQ_PR_55_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_55_RSVD_IRQ_PR_55_W)
#define IRQ_PR_55_RSVD_IRQ_PR_55_W(value) SetGroupBits32( (ICTL1_IRQ_PR_55_VAL),ICTL_IRQ_PR_55_RSVD_IRQ_PR_55_POS, ICTL_IRQ_PR_55_RSVD_IRQ_PR_55_LEN,value)
#endif

#define ICTL1_IRQ_PR_55_RSVD_IRQ_PR_55_R        GetGroupBits32( (ICTL1_IRQ_PR_55_VAL),ICTL_IRQ_PR_55_RSVD_IRQ_PR_55_POS, ICTL_IRQ_PR_55_RSVD_IRQ_PR_55_LEN)

#define ICTL1_IRQ_PR_55_RSVD_IRQ_PR_55_W(value) SetGroupBits32( (ICTL1_IRQ_PR_55_VAL),ICTL_IRQ_PR_55_RSVD_IRQ_PR_55_POS, ICTL_IRQ_PR_55_RSVD_IRQ_PR_55_LEN,value)


/* REGISTER: IRQ_PR_56 */

#if defined(_V1) && !defined(IRQ_PR_56_OFFSET)
#define IRQ_PR_56_OFFSET 0x1c8
#endif

#if !defined(ICTL_IRQ_PR_56_OFFSET)
#define ICTL_IRQ_PR_56_OFFSET 0x1c8
#endif

#if defined(_V1) && !defined(IRQ_PR_56_REG)
#define IRQ_PR_56_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_56_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_56_VAL)
#define IRQ_PR_56_VAL  PREFIX_VAL(IRQ_PR_56_REG)
#endif

#define ICTL1_IRQ_PR_56_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_56_OFFSET))
#define ICTL1_IRQ_PR_56_VAL  PREFIX_VAL(ICTL1_IRQ_PR_56_REG)

/* FIELDS: */

/* irq_pr_56 */

#ifndef ICTL_IRQ_PR_56_IRQ_PR_56_POS
#define ICTL_IRQ_PR_56_IRQ_PR_56_POS      0
#endif

#ifndef ICTL_IRQ_PR_56_IRQ_PR_56_LEN
#define ICTL_IRQ_PR_56_IRQ_PR_56_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_56_IRQ_PR_56_R)
#define IRQ_PR_56_IRQ_PR_56_R        GetGroupBits32( (ICTL1_IRQ_PR_56_VAL),ICTL_IRQ_PR_56_IRQ_PR_56_POS, ICTL_IRQ_PR_56_IRQ_PR_56_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_56_IRQ_PR_56_W)
#define IRQ_PR_56_IRQ_PR_56_W(value) SetGroupBits32( (ICTL1_IRQ_PR_56_VAL),ICTL_IRQ_PR_56_IRQ_PR_56_POS, ICTL_IRQ_PR_56_IRQ_PR_56_LEN,value)
#endif

#define ICTL1_IRQ_PR_56_IRQ_PR_56_R        GetGroupBits32( (ICTL1_IRQ_PR_56_VAL),ICTL_IRQ_PR_56_IRQ_PR_56_POS, ICTL_IRQ_PR_56_IRQ_PR_56_LEN)

#define ICTL1_IRQ_PR_56_IRQ_PR_56_W(value) SetGroupBits32( (ICTL1_IRQ_PR_56_VAL),ICTL_IRQ_PR_56_IRQ_PR_56_POS, ICTL_IRQ_PR_56_IRQ_PR_56_LEN,value)


/* RSVD_irq_pr_56 */

#ifndef ICTL_IRQ_PR_56_RSVD_IRQ_PR_56_POS
#define ICTL_IRQ_PR_56_RSVD_IRQ_PR_56_POS      4
#endif

#ifndef ICTL_IRQ_PR_56_RSVD_IRQ_PR_56_LEN
#define ICTL_IRQ_PR_56_RSVD_IRQ_PR_56_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_56_RSVD_IRQ_PR_56_R)
#define IRQ_PR_56_RSVD_IRQ_PR_56_R        GetGroupBits32( (ICTL1_IRQ_PR_56_VAL),ICTL_IRQ_PR_56_RSVD_IRQ_PR_56_POS, ICTL_IRQ_PR_56_RSVD_IRQ_PR_56_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_56_RSVD_IRQ_PR_56_W)
#define IRQ_PR_56_RSVD_IRQ_PR_56_W(value) SetGroupBits32( (ICTL1_IRQ_PR_56_VAL),ICTL_IRQ_PR_56_RSVD_IRQ_PR_56_POS, ICTL_IRQ_PR_56_RSVD_IRQ_PR_56_LEN,value)
#endif

#define ICTL1_IRQ_PR_56_RSVD_IRQ_PR_56_R        GetGroupBits32( (ICTL1_IRQ_PR_56_VAL),ICTL_IRQ_PR_56_RSVD_IRQ_PR_56_POS, ICTL_IRQ_PR_56_RSVD_IRQ_PR_56_LEN)

#define ICTL1_IRQ_PR_56_RSVD_IRQ_PR_56_W(value) SetGroupBits32( (ICTL1_IRQ_PR_56_VAL),ICTL_IRQ_PR_56_RSVD_IRQ_PR_56_POS, ICTL_IRQ_PR_56_RSVD_IRQ_PR_56_LEN,value)


/* REGISTER: IRQ_PR_57 */

#if defined(_V1) && !defined(IRQ_PR_57_OFFSET)
#define IRQ_PR_57_OFFSET 0x1cc
#endif

#if !defined(ICTL_IRQ_PR_57_OFFSET)
#define ICTL_IRQ_PR_57_OFFSET 0x1cc
#endif

#if defined(_V1) && !defined(IRQ_PR_57_REG)
#define IRQ_PR_57_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_57_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_57_VAL)
#define IRQ_PR_57_VAL  PREFIX_VAL(IRQ_PR_57_REG)
#endif

#define ICTL1_IRQ_PR_57_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_57_OFFSET))
#define ICTL1_IRQ_PR_57_VAL  PREFIX_VAL(ICTL1_IRQ_PR_57_REG)

/* FIELDS: */

/* irq_pr_57 */

#ifndef ICTL_IRQ_PR_57_IRQ_PR_57_POS
#define ICTL_IRQ_PR_57_IRQ_PR_57_POS      0
#endif

#ifndef ICTL_IRQ_PR_57_IRQ_PR_57_LEN
#define ICTL_IRQ_PR_57_IRQ_PR_57_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_57_IRQ_PR_57_R)
#define IRQ_PR_57_IRQ_PR_57_R        GetGroupBits32( (ICTL1_IRQ_PR_57_VAL),ICTL_IRQ_PR_57_IRQ_PR_57_POS, ICTL_IRQ_PR_57_IRQ_PR_57_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_57_IRQ_PR_57_W)
#define IRQ_PR_57_IRQ_PR_57_W(value) SetGroupBits32( (ICTL1_IRQ_PR_57_VAL),ICTL_IRQ_PR_57_IRQ_PR_57_POS, ICTL_IRQ_PR_57_IRQ_PR_57_LEN,value)
#endif

#define ICTL1_IRQ_PR_57_IRQ_PR_57_R        GetGroupBits32( (ICTL1_IRQ_PR_57_VAL),ICTL_IRQ_PR_57_IRQ_PR_57_POS, ICTL_IRQ_PR_57_IRQ_PR_57_LEN)

#define ICTL1_IRQ_PR_57_IRQ_PR_57_W(value) SetGroupBits32( (ICTL1_IRQ_PR_57_VAL),ICTL_IRQ_PR_57_IRQ_PR_57_POS, ICTL_IRQ_PR_57_IRQ_PR_57_LEN,value)


/* RSVD_irq_pr_57 */

#ifndef ICTL_IRQ_PR_57_RSVD_IRQ_PR_57_POS
#define ICTL_IRQ_PR_57_RSVD_IRQ_PR_57_POS      4
#endif

#ifndef ICTL_IRQ_PR_57_RSVD_IRQ_PR_57_LEN
#define ICTL_IRQ_PR_57_RSVD_IRQ_PR_57_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_57_RSVD_IRQ_PR_57_R)
#define IRQ_PR_57_RSVD_IRQ_PR_57_R        GetGroupBits32( (ICTL1_IRQ_PR_57_VAL),ICTL_IRQ_PR_57_RSVD_IRQ_PR_57_POS, ICTL_IRQ_PR_57_RSVD_IRQ_PR_57_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_57_RSVD_IRQ_PR_57_W)
#define IRQ_PR_57_RSVD_IRQ_PR_57_W(value) SetGroupBits32( (ICTL1_IRQ_PR_57_VAL),ICTL_IRQ_PR_57_RSVD_IRQ_PR_57_POS, ICTL_IRQ_PR_57_RSVD_IRQ_PR_57_LEN,value)
#endif

#define ICTL1_IRQ_PR_57_RSVD_IRQ_PR_57_R        GetGroupBits32( (ICTL1_IRQ_PR_57_VAL),ICTL_IRQ_PR_57_RSVD_IRQ_PR_57_POS, ICTL_IRQ_PR_57_RSVD_IRQ_PR_57_LEN)

#define ICTL1_IRQ_PR_57_RSVD_IRQ_PR_57_W(value) SetGroupBits32( (ICTL1_IRQ_PR_57_VAL),ICTL_IRQ_PR_57_RSVD_IRQ_PR_57_POS, ICTL_IRQ_PR_57_RSVD_IRQ_PR_57_LEN,value)


/* REGISTER: IRQ_PR_58 */

#if defined(_V1) && !defined(IRQ_PR_58_OFFSET)
#define IRQ_PR_58_OFFSET 0x1d0
#endif

#if !defined(ICTL_IRQ_PR_58_OFFSET)
#define ICTL_IRQ_PR_58_OFFSET 0x1d0
#endif

#if defined(_V1) && !defined(IRQ_PR_58_REG)
#define IRQ_PR_58_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_58_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_58_VAL)
#define IRQ_PR_58_VAL  PREFIX_VAL(IRQ_PR_58_REG)
#endif

#define ICTL1_IRQ_PR_58_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_58_OFFSET))
#define ICTL1_IRQ_PR_58_VAL  PREFIX_VAL(ICTL1_IRQ_PR_58_REG)

/* FIELDS: */

/* irq_pr_58 */

#ifndef ICTL_IRQ_PR_58_IRQ_PR_58_POS
#define ICTL_IRQ_PR_58_IRQ_PR_58_POS      0
#endif

#ifndef ICTL_IRQ_PR_58_IRQ_PR_58_LEN
#define ICTL_IRQ_PR_58_IRQ_PR_58_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_58_IRQ_PR_58_R)
#define IRQ_PR_58_IRQ_PR_58_R        GetGroupBits32( (ICTL1_IRQ_PR_58_VAL),ICTL_IRQ_PR_58_IRQ_PR_58_POS, ICTL_IRQ_PR_58_IRQ_PR_58_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_58_IRQ_PR_58_W)
#define IRQ_PR_58_IRQ_PR_58_W(value) SetGroupBits32( (ICTL1_IRQ_PR_58_VAL),ICTL_IRQ_PR_58_IRQ_PR_58_POS, ICTL_IRQ_PR_58_IRQ_PR_58_LEN,value)
#endif

#define ICTL1_IRQ_PR_58_IRQ_PR_58_R        GetGroupBits32( (ICTL1_IRQ_PR_58_VAL),ICTL_IRQ_PR_58_IRQ_PR_58_POS, ICTL_IRQ_PR_58_IRQ_PR_58_LEN)

#define ICTL1_IRQ_PR_58_IRQ_PR_58_W(value) SetGroupBits32( (ICTL1_IRQ_PR_58_VAL),ICTL_IRQ_PR_58_IRQ_PR_58_POS, ICTL_IRQ_PR_58_IRQ_PR_58_LEN,value)


/* RSVD_irq_pr_58 */

#ifndef ICTL_IRQ_PR_58_RSVD_IRQ_PR_58_POS
#define ICTL_IRQ_PR_58_RSVD_IRQ_PR_58_POS      4
#endif

#ifndef ICTL_IRQ_PR_58_RSVD_IRQ_PR_58_LEN
#define ICTL_IRQ_PR_58_RSVD_IRQ_PR_58_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_58_RSVD_IRQ_PR_58_R)
#define IRQ_PR_58_RSVD_IRQ_PR_58_R        GetGroupBits32( (ICTL1_IRQ_PR_58_VAL),ICTL_IRQ_PR_58_RSVD_IRQ_PR_58_POS, ICTL_IRQ_PR_58_RSVD_IRQ_PR_58_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_58_RSVD_IRQ_PR_58_W)
#define IRQ_PR_58_RSVD_IRQ_PR_58_W(value) SetGroupBits32( (ICTL1_IRQ_PR_58_VAL),ICTL_IRQ_PR_58_RSVD_IRQ_PR_58_POS, ICTL_IRQ_PR_58_RSVD_IRQ_PR_58_LEN,value)
#endif

#define ICTL1_IRQ_PR_58_RSVD_IRQ_PR_58_R        GetGroupBits32( (ICTL1_IRQ_PR_58_VAL),ICTL_IRQ_PR_58_RSVD_IRQ_PR_58_POS, ICTL_IRQ_PR_58_RSVD_IRQ_PR_58_LEN)

#define ICTL1_IRQ_PR_58_RSVD_IRQ_PR_58_W(value) SetGroupBits32( (ICTL1_IRQ_PR_58_VAL),ICTL_IRQ_PR_58_RSVD_IRQ_PR_58_POS, ICTL_IRQ_PR_58_RSVD_IRQ_PR_58_LEN,value)


/* REGISTER: IRQ_PR_59 */

#if defined(_V1) && !defined(IRQ_PR_59_OFFSET)
#define IRQ_PR_59_OFFSET 0x1d4
#endif

#if !defined(ICTL_IRQ_PR_59_OFFSET)
#define ICTL_IRQ_PR_59_OFFSET 0x1d4
#endif

#if defined(_V1) && !defined(IRQ_PR_59_REG)
#define IRQ_PR_59_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_59_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_59_VAL)
#define IRQ_PR_59_VAL  PREFIX_VAL(IRQ_PR_59_REG)
#endif

#define ICTL1_IRQ_PR_59_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_59_OFFSET))
#define ICTL1_IRQ_PR_59_VAL  PREFIX_VAL(ICTL1_IRQ_PR_59_REG)

/* FIELDS: */

/* irq_pr_59 */

#ifndef ICTL_IRQ_PR_59_IRQ_PR_59_POS
#define ICTL_IRQ_PR_59_IRQ_PR_59_POS      0
#endif

#ifndef ICTL_IRQ_PR_59_IRQ_PR_59_LEN
#define ICTL_IRQ_PR_59_IRQ_PR_59_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_59_IRQ_PR_59_R)
#define IRQ_PR_59_IRQ_PR_59_R        GetGroupBits32( (ICTL1_IRQ_PR_59_VAL),ICTL_IRQ_PR_59_IRQ_PR_59_POS, ICTL_IRQ_PR_59_IRQ_PR_59_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_59_IRQ_PR_59_W)
#define IRQ_PR_59_IRQ_PR_59_W(value) SetGroupBits32( (ICTL1_IRQ_PR_59_VAL),ICTL_IRQ_PR_59_IRQ_PR_59_POS, ICTL_IRQ_PR_59_IRQ_PR_59_LEN,value)
#endif

#define ICTL1_IRQ_PR_59_IRQ_PR_59_R        GetGroupBits32( (ICTL1_IRQ_PR_59_VAL),ICTL_IRQ_PR_59_IRQ_PR_59_POS, ICTL_IRQ_PR_59_IRQ_PR_59_LEN)

#define ICTL1_IRQ_PR_59_IRQ_PR_59_W(value) SetGroupBits32( (ICTL1_IRQ_PR_59_VAL),ICTL_IRQ_PR_59_IRQ_PR_59_POS, ICTL_IRQ_PR_59_IRQ_PR_59_LEN,value)


/* RSVD_irq_pr_59 */

#ifndef ICTL_IRQ_PR_59_RSVD_IRQ_PR_59_POS
#define ICTL_IRQ_PR_59_RSVD_IRQ_PR_59_POS      4
#endif

#ifndef ICTL_IRQ_PR_59_RSVD_IRQ_PR_59_LEN
#define ICTL_IRQ_PR_59_RSVD_IRQ_PR_59_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_59_RSVD_IRQ_PR_59_R)
#define IRQ_PR_59_RSVD_IRQ_PR_59_R        GetGroupBits32( (ICTL1_IRQ_PR_59_VAL),ICTL_IRQ_PR_59_RSVD_IRQ_PR_59_POS, ICTL_IRQ_PR_59_RSVD_IRQ_PR_59_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_59_RSVD_IRQ_PR_59_W)
#define IRQ_PR_59_RSVD_IRQ_PR_59_W(value) SetGroupBits32( (ICTL1_IRQ_PR_59_VAL),ICTL_IRQ_PR_59_RSVD_IRQ_PR_59_POS, ICTL_IRQ_PR_59_RSVD_IRQ_PR_59_LEN,value)
#endif

#define ICTL1_IRQ_PR_59_RSVD_IRQ_PR_59_R        GetGroupBits32( (ICTL1_IRQ_PR_59_VAL),ICTL_IRQ_PR_59_RSVD_IRQ_PR_59_POS, ICTL_IRQ_PR_59_RSVD_IRQ_PR_59_LEN)

#define ICTL1_IRQ_PR_59_RSVD_IRQ_PR_59_W(value) SetGroupBits32( (ICTL1_IRQ_PR_59_VAL),ICTL_IRQ_PR_59_RSVD_IRQ_PR_59_POS, ICTL_IRQ_PR_59_RSVD_IRQ_PR_59_LEN,value)


/* REGISTER: IRQ_PR_60 */

#if defined(_V1) && !defined(IRQ_PR_60_OFFSET)
#define IRQ_PR_60_OFFSET 0x1d8
#endif

#if !defined(ICTL_IRQ_PR_60_OFFSET)
#define ICTL_IRQ_PR_60_OFFSET 0x1d8
#endif

#if defined(_V1) && !defined(IRQ_PR_60_REG)
#define IRQ_PR_60_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_60_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_60_VAL)
#define IRQ_PR_60_VAL  PREFIX_VAL(IRQ_PR_60_REG)
#endif

#define ICTL1_IRQ_PR_60_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_60_OFFSET))
#define ICTL1_IRQ_PR_60_VAL  PREFIX_VAL(ICTL1_IRQ_PR_60_REG)

/* FIELDS: */

/* irq_pr_60 */

#ifndef ICTL_IRQ_PR_60_IRQ_PR_60_POS
#define ICTL_IRQ_PR_60_IRQ_PR_60_POS      0
#endif

#ifndef ICTL_IRQ_PR_60_IRQ_PR_60_LEN
#define ICTL_IRQ_PR_60_IRQ_PR_60_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_60_IRQ_PR_60_R)
#define IRQ_PR_60_IRQ_PR_60_R        GetGroupBits32( (ICTL1_IRQ_PR_60_VAL),ICTL_IRQ_PR_60_IRQ_PR_60_POS, ICTL_IRQ_PR_60_IRQ_PR_60_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_60_IRQ_PR_60_W)
#define IRQ_PR_60_IRQ_PR_60_W(value) SetGroupBits32( (ICTL1_IRQ_PR_60_VAL),ICTL_IRQ_PR_60_IRQ_PR_60_POS, ICTL_IRQ_PR_60_IRQ_PR_60_LEN,value)
#endif

#define ICTL1_IRQ_PR_60_IRQ_PR_60_R        GetGroupBits32( (ICTL1_IRQ_PR_60_VAL),ICTL_IRQ_PR_60_IRQ_PR_60_POS, ICTL_IRQ_PR_60_IRQ_PR_60_LEN)

#define ICTL1_IRQ_PR_60_IRQ_PR_60_W(value) SetGroupBits32( (ICTL1_IRQ_PR_60_VAL),ICTL_IRQ_PR_60_IRQ_PR_60_POS, ICTL_IRQ_PR_60_IRQ_PR_60_LEN,value)


/* RSVD_irq_pr_60 */

#ifndef ICTL_IRQ_PR_60_RSVD_IRQ_PR_60_POS
#define ICTL_IRQ_PR_60_RSVD_IRQ_PR_60_POS      4
#endif

#ifndef ICTL_IRQ_PR_60_RSVD_IRQ_PR_60_LEN
#define ICTL_IRQ_PR_60_RSVD_IRQ_PR_60_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_60_RSVD_IRQ_PR_60_R)
#define IRQ_PR_60_RSVD_IRQ_PR_60_R        GetGroupBits32( (ICTL1_IRQ_PR_60_VAL),ICTL_IRQ_PR_60_RSVD_IRQ_PR_60_POS, ICTL_IRQ_PR_60_RSVD_IRQ_PR_60_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_60_RSVD_IRQ_PR_60_W)
#define IRQ_PR_60_RSVD_IRQ_PR_60_W(value) SetGroupBits32( (ICTL1_IRQ_PR_60_VAL),ICTL_IRQ_PR_60_RSVD_IRQ_PR_60_POS, ICTL_IRQ_PR_60_RSVD_IRQ_PR_60_LEN,value)
#endif

#define ICTL1_IRQ_PR_60_RSVD_IRQ_PR_60_R        GetGroupBits32( (ICTL1_IRQ_PR_60_VAL),ICTL_IRQ_PR_60_RSVD_IRQ_PR_60_POS, ICTL_IRQ_PR_60_RSVD_IRQ_PR_60_LEN)

#define ICTL1_IRQ_PR_60_RSVD_IRQ_PR_60_W(value) SetGroupBits32( (ICTL1_IRQ_PR_60_VAL),ICTL_IRQ_PR_60_RSVD_IRQ_PR_60_POS, ICTL_IRQ_PR_60_RSVD_IRQ_PR_60_LEN,value)


/* REGISTER: IRQ_PR_61 */

#if defined(_V1) && !defined(IRQ_PR_61_OFFSET)
#define IRQ_PR_61_OFFSET 0x1dc
#endif

#if !defined(ICTL_IRQ_PR_61_OFFSET)
#define ICTL_IRQ_PR_61_OFFSET 0x1dc
#endif

#if defined(_V1) && !defined(IRQ_PR_61_REG)
#define IRQ_PR_61_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_61_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_61_VAL)
#define IRQ_PR_61_VAL  PREFIX_VAL(IRQ_PR_61_REG)
#endif

#define ICTL1_IRQ_PR_61_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_61_OFFSET))
#define ICTL1_IRQ_PR_61_VAL  PREFIX_VAL(ICTL1_IRQ_PR_61_REG)

/* FIELDS: */

/* irq_pr_61 */

#ifndef ICTL_IRQ_PR_61_IRQ_PR_61_POS
#define ICTL_IRQ_PR_61_IRQ_PR_61_POS      0
#endif

#ifndef ICTL_IRQ_PR_61_IRQ_PR_61_LEN
#define ICTL_IRQ_PR_61_IRQ_PR_61_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_61_IRQ_PR_61_R)
#define IRQ_PR_61_IRQ_PR_61_R        GetGroupBits32( (ICTL1_IRQ_PR_61_VAL),ICTL_IRQ_PR_61_IRQ_PR_61_POS, ICTL_IRQ_PR_61_IRQ_PR_61_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_61_IRQ_PR_61_W)
#define IRQ_PR_61_IRQ_PR_61_W(value) SetGroupBits32( (ICTL1_IRQ_PR_61_VAL),ICTL_IRQ_PR_61_IRQ_PR_61_POS, ICTL_IRQ_PR_61_IRQ_PR_61_LEN,value)
#endif

#define ICTL1_IRQ_PR_61_IRQ_PR_61_R        GetGroupBits32( (ICTL1_IRQ_PR_61_VAL),ICTL_IRQ_PR_61_IRQ_PR_61_POS, ICTL_IRQ_PR_61_IRQ_PR_61_LEN)

#define ICTL1_IRQ_PR_61_IRQ_PR_61_W(value) SetGroupBits32( (ICTL1_IRQ_PR_61_VAL),ICTL_IRQ_PR_61_IRQ_PR_61_POS, ICTL_IRQ_PR_61_IRQ_PR_61_LEN,value)


/* RSVD_irq_pr_61 */

#ifndef ICTL_IRQ_PR_61_RSVD_IRQ_PR_61_POS
#define ICTL_IRQ_PR_61_RSVD_IRQ_PR_61_POS      4
#endif

#ifndef ICTL_IRQ_PR_61_RSVD_IRQ_PR_61_LEN
#define ICTL_IRQ_PR_61_RSVD_IRQ_PR_61_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_61_RSVD_IRQ_PR_61_R)
#define IRQ_PR_61_RSVD_IRQ_PR_61_R        GetGroupBits32( (ICTL1_IRQ_PR_61_VAL),ICTL_IRQ_PR_61_RSVD_IRQ_PR_61_POS, ICTL_IRQ_PR_61_RSVD_IRQ_PR_61_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_61_RSVD_IRQ_PR_61_W)
#define IRQ_PR_61_RSVD_IRQ_PR_61_W(value) SetGroupBits32( (ICTL1_IRQ_PR_61_VAL),ICTL_IRQ_PR_61_RSVD_IRQ_PR_61_POS, ICTL_IRQ_PR_61_RSVD_IRQ_PR_61_LEN,value)
#endif

#define ICTL1_IRQ_PR_61_RSVD_IRQ_PR_61_R        GetGroupBits32( (ICTL1_IRQ_PR_61_VAL),ICTL_IRQ_PR_61_RSVD_IRQ_PR_61_POS, ICTL_IRQ_PR_61_RSVD_IRQ_PR_61_LEN)

#define ICTL1_IRQ_PR_61_RSVD_IRQ_PR_61_W(value) SetGroupBits32( (ICTL1_IRQ_PR_61_VAL),ICTL_IRQ_PR_61_RSVD_IRQ_PR_61_POS, ICTL_IRQ_PR_61_RSVD_IRQ_PR_61_LEN,value)


/* REGISTER: IRQ_PR_62 */

#if defined(_V1) && !defined(IRQ_PR_62_OFFSET)
#define IRQ_PR_62_OFFSET 0x1e0
#endif

#if !defined(ICTL_IRQ_PR_62_OFFSET)
#define ICTL_IRQ_PR_62_OFFSET 0x1e0
#endif

#if defined(_V1) && !defined(IRQ_PR_62_REG)
#define IRQ_PR_62_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_62_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_62_VAL)
#define IRQ_PR_62_VAL  PREFIX_VAL(IRQ_PR_62_REG)
#endif

#define ICTL1_IRQ_PR_62_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_62_OFFSET))
#define ICTL1_IRQ_PR_62_VAL  PREFIX_VAL(ICTL1_IRQ_PR_62_REG)

/* FIELDS: */

/* irq_pr_62 */

#ifndef ICTL_IRQ_PR_62_IRQ_PR_62_POS
#define ICTL_IRQ_PR_62_IRQ_PR_62_POS      0
#endif

#ifndef ICTL_IRQ_PR_62_IRQ_PR_62_LEN
#define ICTL_IRQ_PR_62_IRQ_PR_62_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_62_IRQ_PR_62_R)
#define IRQ_PR_62_IRQ_PR_62_R        GetGroupBits32( (ICTL1_IRQ_PR_62_VAL),ICTL_IRQ_PR_62_IRQ_PR_62_POS, ICTL_IRQ_PR_62_IRQ_PR_62_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_62_IRQ_PR_62_W)
#define IRQ_PR_62_IRQ_PR_62_W(value) SetGroupBits32( (ICTL1_IRQ_PR_62_VAL),ICTL_IRQ_PR_62_IRQ_PR_62_POS, ICTL_IRQ_PR_62_IRQ_PR_62_LEN,value)
#endif

#define ICTL1_IRQ_PR_62_IRQ_PR_62_R        GetGroupBits32( (ICTL1_IRQ_PR_62_VAL),ICTL_IRQ_PR_62_IRQ_PR_62_POS, ICTL_IRQ_PR_62_IRQ_PR_62_LEN)

#define ICTL1_IRQ_PR_62_IRQ_PR_62_W(value) SetGroupBits32( (ICTL1_IRQ_PR_62_VAL),ICTL_IRQ_PR_62_IRQ_PR_62_POS, ICTL_IRQ_PR_62_IRQ_PR_62_LEN,value)


/* RSVD_irq_pr_62 */

#ifndef ICTL_IRQ_PR_62_RSVD_IRQ_PR_62_POS
#define ICTL_IRQ_PR_62_RSVD_IRQ_PR_62_POS      4
#endif

#ifndef ICTL_IRQ_PR_62_RSVD_IRQ_PR_62_LEN
#define ICTL_IRQ_PR_62_RSVD_IRQ_PR_62_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_62_RSVD_IRQ_PR_62_R)
#define IRQ_PR_62_RSVD_IRQ_PR_62_R        GetGroupBits32( (ICTL1_IRQ_PR_62_VAL),ICTL_IRQ_PR_62_RSVD_IRQ_PR_62_POS, ICTL_IRQ_PR_62_RSVD_IRQ_PR_62_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_62_RSVD_IRQ_PR_62_W)
#define IRQ_PR_62_RSVD_IRQ_PR_62_W(value) SetGroupBits32( (ICTL1_IRQ_PR_62_VAL),ICTL_IRQ_PR_62_RSVD_IRQ_PR_62_POS, ICTL_IRQ_PR_62_RSVD_IRQ_PR_62_LEN,value)
#endif

#define ICTL1_IRQ_PR_62_RSVD_IRQ_PR_62_R        GetGroupBits32( (ICTL1_IRQ_PR_62_VAL),ICTL_IRQ_PR_62_RSVD_IRQ_PR_62_POS, ICTL_IRQ_PR_62_RSVD_IRQ_PR_62_LEN)

#define ICTL1_IRQ_PR_62_RSVD_IRQ_PR_62_W(value) SetGroupBits32( (ICTL1_IRQ_PR_62_VAL),ICTL_IRQ_PR_62_RSVD_IRQ_PR_62_POS, ICTL_IRQ_PR_62_RSVD_IRQ_PR_62_LEN,value)


/* REGISTER: IRQ_PR_63 */

#if defined(_V1) && !defined(IRQ_PR_63_OFFSET)
#define IRQ_PR_63_OFFSET 0x1e4
#endif

#if !defined(ICTL_IRQ_PR_63_OFFSET)
#define ICTL_IRQ_PR_63_OFFSET 0x1e4
#endif

#if defined(_V1) && !defined(IRQ_PR_63_REG)
#define IRQ_PR_63_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_63_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_63_VAL)
#define IRQ_PR_63_VAL  PREFIX_VAL(IRQ_PR_63_REG)
#endif

#define ICTL1_IRQ_PR_63_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_63_OFFSET))
#define ICTL1_IRQ_PR_63_VAL  PREFIX_VAL(ICTL1_IRQ_PR_63_REG)

/* FIELDS: */

/* irq_pr_63 */

#ifndef ICTL_IRQ_PR_63_IRQ_PR_63_POS
#define ICTL_IRQ_PR_63_IRQ_PR_63_POS      0
#endif

#ifndef ICTL_IRQ_PR_63_IRQ_PR_63_LEN
#define ICTL_IRQ_PR_63_IRQ_PR_63_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_63_IRQ_PR_63_R)
#define IRQ_PR_63_IRQ_PR_63_R        GetGroupBits32( (ICTL1_IRQ_PR_63_VAL),ICTL_IRQ_PR_63_IRQ_PR_63_POS, ICTL_IRQ_PR_63_IRQ_PR_63_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_63_IRQ_PR_63_W)
#define IRQ_PR_63_IRQ_PR_63_W(value) SetGroupBits32( (ICTL1_IRQ_PR_63_VAL),ICTL_IRQ_PR_63_IRQ_PR_63_POS, ICTL_IRQ_PR_63_IRQ_PR_63_LEN,value)
#endif

#define ICTL1_IRQ_PR_63_IRQ_PR_63_R        GetGroupBits32( (ICTL1_IRQ_PR_63_VAL),ICTL_IRQ_PR_63_IRQ_PR_63_POS, ICTL_IRQ_PR_63_IRQ_PR_63_LEN)

#define ICTL1_IRQ_PR_63_IRQ_PR_63_W(value) SetGroupBits32( (ICTL1_IRQ_PR_63_VAL),ICTL_IRQ_PR_63_IRQ_PR_63_POS, ICTL_IRQ_PR_63_IRQ_PR_63_LEN,value)


/* RSVD_irq_pr_63 */

#ifndef ICTL_IRQ_PR_63_RSVD_IRQ_PR_63_POS
#define ICTL_IRQ_PR_63_RSVD_IRQ_PR_63_POS      4
#endif

#ifndef ICTL_IRQ_PR_63_RSVD_IRQ_PR_63_LEN
#define ICTL_IRQ_PR_63_RSVD_IRQ_PR_63_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_63_RSVD_IRQ_PR_63_R)
#define IRQ_PR_63_RSVD_IRQ_PR_63_R        GetGroupBits32( (ICTL1_IRQ_PR_63_VAL),ICTL_IRQ_PR_63_RSVD_IRQ_PR_63_POS, ICTL_IRQ_PR_63_RSVD_IRQ_PR_63_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_63_RSVD_IRQ_PR_63_W)
#define IRQ_PR_63_RSVD_IRQ_PR_63_W(value) SetGroupBits32( (ICTL1_IRQ_PR_63_VAL),ICTL_IRQ_PR_63_RSVD_IRQ_PR_63_POS, ICTL_IRQ_PR_63_RSVD_IRQ_PR_63_LEN,value)
#endif

#define ICTL1_IRQ_PR_63_RSVD_IRQ_PR_63_R        GetGroupBits32( (ICTL1_IRQ_PR_63_VAL),ICTL_IRQ_PR_63_RSVD_IRQ_PR_63_POS, ICTL_IRQ_PR_63_RSVD_IRQ_PR_63_LEN)

#define ICTL1_IRQ_PR_63_RSVD_IRQ_PR_63_W(value) SetGroupBits32( (ICTL1_IRQ_PR_63_VAL),ICTL_IRQ_PR_63_RSVD_IRQ_PR_63_POS, ICTL_IRQ_PR_63_RSVD_IRQ_PR_63_LEN,value)


/* REGISTER: IRQ_VECTOR_DEFAULT */

#if defined(_V1) && !defined(IRQ_VECTOR_DEFAULT_OFFSET)
#define IRQ_VECTOR_DEFAULT_OFFSET 0x1e8
#endif

#if !defined(ICTL_IRQ_VECTOR_DEFAULT_OFFSET)
#define ICTL_IRQ_VECTOR_DEFAULT_OFFSET 0x1e8
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_DEFAULT_REG)
#define IRQ_VECTOR_DEFAULT_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_DEFAULT_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_DEFAULT_VAL)
#define IRQ_VECTOR_DEFAULT_VAL  PREFIX_VAL(IRQ_VECTOR_DEFAULT_REG)
#endif

#define ICTL1_IRQ_VECTOR_DEFAULT_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_DEFAULT_OFFSET))
#define ICTL1_IRQ_VECTOR_DEFAULT_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_DEFAULT_REG)

/* FIELDS: */

/* IRQ_VECTOR_DEFAULT */

#ifndef ICTL_IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_POS
#define ICTL_IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_LEN
#define ICTL_IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_R)
#define IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_DEFAULT_VAL),ICTL_IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_POS, ICTL_IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_W)
#define IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_DEFAULT_VAL),ICTL_IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_POS, ICTL_IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_DEFAULT_VAL),ICTL_IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_POS, ICTL_IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_LEN)

#define ICTL1_IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_DEFAULT_VAL),ICTL_IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_POS, ICTL_IRQ_VECTOR_DEFAULT_IRQ_VECTOR_DEFAULT_LEN,value)


/* REGISTER: IRQ_STATUS_L */

#if defined(_V1) && !defined(IRQ_STATUS_L_OFFSET)
#define IRQ_STATUS_L_OFFSET 0x20
#endif

#if !defined(ICTL_IRQ_STATUS_L_OFFSET)
#define ICTL_IRQ_STATUS_L_OFFSET 0x20
#endif

#if defined(_V1) && !defined(IRQ_STATUS_L_REG)
#define IRQ_STATUS_L_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_STATUS_L_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_STATUS_L_VAL)
#define IRQ_STATUS_L_VAL  PREFIX_VAL(IRQ_STATUS_L_REG)
#endif

#define ICTL1_IRQ_STATUS_L_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_STATUS_L_OFFSET))
#define ICTL1_IRQ_STATUS_L_VAL  PREFIX_VAL(ICTL1_IRQ_STATUS_L_REG)

/* FIELDS: */

/* IRQ_STATUS_L */

#ifndef ICTL_IRQ_STATUS_L_IRQ_STATUS_L_POS
#define ICTL_IRQ_STATUS_L_IRQ_STATUS_L_POS      0
#endif

#ifndef ICTL_IRQ_STATUS_L_IRQ_STATUS_L_LEN
#define ICTL_IRQ_STATUS_L_IRQ_STATUS_L_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_STATUS_L_IRQ_STATUS_L_R)
#define IRQ_STATUS_L_IRQ_STATUS_L_R        GetGroupBits32( (ICTL1_IRQ_STATUS_L_VAL),ICTL_IRQ_STATUS_L_IRQ_STATUS_L_POS, ICTL_IRQ_STATUS_L_IRQ_STATUS_L_LEN)
#endif

#if defined(_V1) && !defined(IRQ_STATUS_L_IRQ_STATUS_L_W)
#define IRQ_STATUS_L_IRQ_STATUS_L_W(value) SetGroupBits32( (ICTL1_IRQ_STATUS_L_VAL),ICTL_IRQ_STATUS_L_IRQ_STATUS_L_POS, ICTL_IRQ_STATUS_L_IRQ_STATUS_L_LEN,value)
#endif

#define ICTL1_IRQ_STATUS_L_IRQ_STATUS_L_R        GetGroupBits32( (ICTL1_IRQ_STATUS_L_VAL),ICTL_IRQ_STATUS_L_IRQ_STATUS_L_POS, ICTL_IRQ_STATUS_L_IRQ_STATUS_L_LEN)

#define ICTL1_IRQ_STATUS_L_IRQ_STATUS_L_W(value) SetGroupBits32( (ICTL1_IRQ_STATUS_L_VAL),ICTL_IRQ_STATUS_L_IRQ_STATUS_L_POS, ICTL_IRQ_STATUS_L_IRQ_STATUS_L_LEN,value)


/* REGISTER: IRQ_STATUS_H */

#if defined(_V1) && !defined(IRQ_STATUS_H_OFFSET)
#define IRQ_STATUS_H_OFFSET 0x24
#endif

#if !defined(ICTL_IRQ_STATUS_H_OFFSET)
#define ICTL_IRQ_STATUS_H_OFFSET 0x24
#endif

#if defined(_V1) && !defined(IRQ_STATUS_H_REG)
#define IRQ_STATUS_H_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_STATUS_H_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_STATUS_H_VAL)
#define IRQ_STATUS_H_VAL  PREFIX_VAL(IRQ_STATUS_H_REG)
#endif

#define ICTL1_IRQ_STATUS_H_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_STATUS_H_OFFSET))
#define ICTL1_IRQ_STATUS_H_VAL  PREFIX_VAL(ICTL1_IRQ_STATUS_H_REG)

/* FIELDS: */

/* IRQ_STATUS_H */

#ifndef ICTL_IRQ_STATUS_H_IRQ_STATUS_H_POS
#define ICTL_IRQ_STATUS_H_IRQ_STATUS_H_POS      0
#endif

#ifndef ICTL_IRQ_STATUS_H_IRQ_STATUS_H_LEN
#define ICTL_IRQ_STATUS_H_IRQ_STATUS_H_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_STATUS_H_IRQ_STATUS_H_R)
#define IRQ_STATUS_H_IRQ_STATUS_H_R        GetGroupBits32( (ICTL1_IRQ_STATUS_H_VAL),ICTL_IRQ_STATUS_H_IRQ_STATUS_H_POS, ICTL_IRQ_STATUS_H_IRQ_STATUS_H_LEN)
#endif

#if defined(_V1) && !defined(IRQ_STATUS_H_IRQ_STATUS_H_W)
#define IRQ_STATUS_H_IRQ_STATUS_H_W(value) SetGroupBits32( (ICTL1_IRQ_STATUS_H_VAL),ICTL_IRQ_STATUS_H_IRQ_STATUS_H_POS, ICTL_IRQ_STATUS_H_IRQ_STATUS_H_LEN,value)
#endif

#define ICTL1_IRQ_STATUS_H_IRQ_STATUS_H_R        GetGroupBits32( (ICTL1_IRQ_STATUS_H_VAL),ICTL_IRQ_STATUS_H_IRQ_STATUS_H_POS, ICTL_IRQ_STATUS_H_IRQ_STATUS_H_LEN)

#define ICTL1_IRQ_STATUS_H_IRQ_STATUS_H_W(value) SetGroupBits32( (ICTL1_IRQ_STATUS_H_VAL),ICTL_IRQ_STATUS_H_IRQ_STATUS_H_POS, ICTL_IRQ_STATUS_H_IRQ_STATUS_H_LEN,value)


/* REGISTER: IRQ_MASKSTATUS_L */

#if defined(_V1) && !defined(IRQ_MASKSTATUS_L_OFFSET)
#define IRQ_MASKSTATUS_L_OFFSET 0x28
#endif

#if !defined(ICTL_IRQ_MASKSTATUS_L_OFFSET)
#define ICTL_IRQ_MASKSTATUS_L_OFFSET 0x28
#endif

#if defined(_V1) && !defined(IRQ_MASKSTATUS_L_REG)
#define IRQ_MASKSTATUS_L_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_MASKSTATUS_L_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_MASKSTATUS_L_VAL)
#define IRQ_MASKSTATUS_L_VAL  PREFIX_VAL(IRQ_MASKSTATUS_L_REG)
#endif

#define ICTL1_IRQ_MASKSTATUS_L_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_MASKSTATUS_L_OFFSET))
#define ICTL1_IRQ_MASKSTATUS_L_VAL  PREFIX_VAL(ICTL1_IRQ_MASKSTATUS_L_REG)

/* FIELDS: */

/* IRQ_MASKSTATUS_L */

#ifndef ICTL_IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_POS
#define ICTL_IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_POS      0
#endif

#ifndef ICTL_IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_LEN
#define ICTL_IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_R)
#define IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_R        GetGroupBits32( (ICTL1_IRQ_MASKSTATUS_L_VAL),ICTL_IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_POS, ICTL_IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_LEN)
#endif

#if defined(_V1) && !defined(IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_W)
#define IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_W(value) SetGroupBits32( (ICTL1_IRQ_MASKSTATUS_L_VAL),ICTL_IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_POS, ICTL_IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_LEN,value)
#endif

#define ICTL1_IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_R        GetGroupBits32( (ICTL1_IRQ_MASKSTATUS_L_VAL),ICTL_IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_POS, ICTL_IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_LEN)

#define ICTL1_IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_W(value) SetGroupBits32( (ICTL1_IRQ_MASKSTATUS_L_VAL),ICTL_IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_POS, ICTL_IRQ_MASKSTATUS_L_IRQ_MASKSTATUS_L_LEN,value)


/* REGISTER: IRQ_MASKSTATUS_H */

#if defined(_V1) && !defined(IRQ_MASKSTATUS_H_OFFSET)
#define IRQ_MASKSTATUS_H_OFFSET 0x2c
#endif

#if !defined(ICTL_IRQ_MASKSTATUS_H_OFFSET)
#define ICTL_IRQ_MASKSTATUS_H_OFFSET 0x2c
#endif

#if defined(_V1) && !defined(IRQ_MASKSTATUS_H_REG)
#define IRQ_MASKSTATUS_H_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_MASKSTATUS_H_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_MASKSTATUS_H_VAL)
#define IRQ_MASKSTATUS_H_VAL  PREFIX_VAL(IRQ_MASKSTATUS_H_REG)
#endif

#define ICTL1_IRQ_MASKSTATUS_H_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_MASKSTATUS_H_OFFSET))
#define ICTL1_IRQ_MASKSTATUS_H_VAL  PREFIX_VAL(ICTL1_IRQ_MASKSTATUS_H_REG)

/* FIELDS: */

/* IRQ_MASKSTATUS_H */

#ifndef ICTL_IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_POS
#define ICTL_IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_POS      0
#endif

#ifndef ICTL_IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_LEN
#define ICTL_IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_R)
#define IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_R        GetGroupBits32( (ICTL1_IRQ_MASKSTATUS_H_VAL),ICTL_IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_POS, ICTL_IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_LEN)
#endif

#if defined(_V1) && !defined(IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_W)
#define IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_W(value) SetGroupBits32( (ICTL1_IRQ_MASKSTATUS_H_VAL),ICTL_IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_POS, ICTL_IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_LEN,value)
#endif

#define ICTL1_IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_R        GetGroupBits32( (ICTL1_IRQ_MASKSTATUS_H_VAL),ICTL_IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_POS, ICTL_IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_LEN)

#define ICTL1_IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_W(value) SetGroupBits32( (ICTL1_IRQ_MASKSTATUS_H_VAL),ICTL_IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_POS, ICTL_IRQ_MASKSTATUS_H_IRQ_MASKSTATUS_H_LEN,value)


/* REGISTER: IRQ_FINALSTATUS_L */

#if defined(_V1) && !defined(IRQ_FINALSTATUS_L_OFFSET)
#define IRQ_FINALSTATUS_L_OFFSET 0x30
#endif

#if !defined(ICTL_IRQ_FINALSTATUS_L_OFFSET)
#define ICTL_IRQ_FINALSTATUS_L_OFFSET 0x30
#endif

#if defined(_V1) && !defined(IRQ_FINALSTATUS_L_REG)
#define IRQ_FINALSTATUS_L_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_FINALSTATUS_L_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_FINALSTATUS_L_VAL)
#define IRQ_FINALSTATUS_L_VAL  PREFIX_VAL(IRQ_FINALSTATUS_L_REG)
#endif

#define ICTL1_IRQ_FINALSTATUS_L_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_FINALSTATUS_L_OFFSET))
#define ICTL1_IRQ_FINALSTATUS_L_VAL  PREFIX_VAL(ICTL1_IRQ_FINALSTATUS_L_REG)

/* FIELDS: */

/* IRQ_FINALSTATUS_L */

#ifndef ICTL_IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_POS
#define ICTL_IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_POS      0
#endif

#ifndef ICTL_IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_LEN
#define ICTL_IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_R)
#define IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_R        GetGroupBits32( (ICTL1_IRQ_FINALSTATUS_L_VAL),ICTL_IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_POS, ICTL_IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_LEN)
#endif

#if defined(_V1) && !defined(IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_W)
#define IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_W(value) SetGroupBits32( (ICTL1_IRQ_FINALSTATUS_L_VAL),ICTL_IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_POS, ICTL_IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_LEN,value)
#endif

#define ICTL1_IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_R        GetGroupBits32( (ICTL1_IRQ_FINALSTATUS_L_VAL),ICTL_IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_POS, ICTL_IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_LEN)

#define ICTL1_IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_W(value) SetGroupBits32( (ICTL1_IRQ_FINALSTATUS_L_VAL),ICTL_IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_POS, ICTL_IRQ_FINALSTATUS_L_IRQ_FINALSTATUS_L_LEN,value)


/* REGISTER: IRQ_FINALSTATUS_H */

#if defined(_V1) && !defined(IRQ_FINALSTATUS_H_OFFSET)
#define IRQ_FINALSTATUS_H_OFFSET 0x34
#endif

#if !defined(ICTL_IRQ_FINALSTATUS_H_OFFSET)
#define ICTL_IRQ_FINALSTATUS_H_OFFSET 0x34
#endif

#if defined(_V1) && !defined(IRQ_FINALSTATUS_H_REG)
#define IRQ_FINALSTATUS_H_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_FINALSTATUS_H_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_FINALSTATUS_H_VAL)
#define IRQ_FINALSTATUS_H_VAL  PREFIX_VAL(IRQ_FINALSTATUS_H_REG)
#endif

#define ICTL1_IRQ_FINALSTATUS_H_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_FINALSTATUS_H_OFFSET))
#define ICTL1_IRQ_FINALSTATUS_H_VAL  PREFIX_VAL(ICTL1_IRQ_FINALSTATUS_H_REG)

/* FIELDS: */

/* IRQ_FINALSTATUS_H */

#ifndef ICTL_IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_POS
#define ICTL_IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_POS      0
#endif

#ifndef ICTL_IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_LEN
#define ICTL_IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_R)
#define IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_R        GetGroupBits32( (ICTL1_IRQ_FINALSTATUS_H_VAL),ICTL_IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_POS, ICTL_IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_LEN)
#endif

#if defined(_V1) && !defined(IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_W)
#define IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_W(value) SetGroupBits32( (ICTL1_IRQ_FINALSTATUS_H_VAL),ICTL_IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_POS, ICTL_IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_LEN,value)
#endif

#define ICTL1_IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_R        GetGroupBits32( (ICTL1_IRQ_FINALSTATUS_H_VAL),ICTL_IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_POS, ICTL_IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_LEN)

#define ICTL1_IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_W(value) SetGroupBits32( (ICTL1_IRQ_FINALSTATUS_H_VAL),ICTL_IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_POS, ICTL_IRQ_FINALSTATUS_H_IRQ_FINALSTATUS_H_LEN,value)


/* REGISTER: IRQ_VECTOR */

#if defined(_V1) && !defined(IRQ_VECTOR_OFFSET)
#define IRQ_VECTOR_OFFSET 0x38
#endif

#if !defined(ICTL_IRQ_VECTOR_OFFSET)
#define ICTL_IRQ_VECTOR_OFFSET 0x38
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_REG)
#define IRQ_VECTOR_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_VAL)
#define IRQ_VECTOR_VAL  PREFIX_VAL(IRQ_VECTOR_REG)
#endif

#define ICTL1_IRQ_VECTOR_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_OFFSET))
#define ICTL1_IRQ_VECTOR_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_REG)

/* FIELDS: */

/* IRQ_VECTOR */

#ifndef ICTL_IRQ_VECTOR_IRQ_VECTOR_POS
#define ICTL_IRQ_VECTOR_IRQ_VECTOR_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_IRQ_VECTOR_LEN
#define ICTL_IRQ_VECTOR_IRQ_VECTOR_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_IRQ_VECTOR_R)
#define IRQ_VECTOR_IRQ_VECTOR_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_VAL),ICTL_IRQ_VECTOR_IRQ_VECTOR_POS, ICTL_IRQ_VECTOR_IRQ_VECTOR_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_IRQ_VECTOR_W)
#define IRQ_VECTOR_IRQ_VECTOR_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_VAL),ICTL_IRQ_VECTOR_IRQ_VECTOR_POS, ICTL_IRQ_VECTOR_IRQ_VECTOR_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_IRQ_VECTOR_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_VAL),ICTL_IRQ_VECTOR_IRQ_VECTOR_POS, ICTL_IRQ_VECTOR_IRQ_VECTOR_LEN)

#define ICTL1_IRQ_VECTOR_IRQ_VECTOR_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_VAL),ICTL_IRQ_VECTOR_IRQ_VECTOR_POS, ICTL_IRQ_VECTOR_IRQ_VECTOR_LEN,value)


/* REGISTER: IRQ_INTEN_H */

#if defined(_V1) && !defined(IRQ_INTEN_H_OFFSET)
#define IRQ_INTEN_H_OFFSET 0x4
#endif

#if !defined(ICTL_IRQ_INTEN_H_OFFSET)
#define ICTL_IRQ_INTEN_H_OFFSET 0x4
#endif

#if defined(_V1) && !defined(IRQ_INTEN_H_REG)
#define IRQ_INTEN_H_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_INTEN_H_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_INTEN_H_VAL)
#define IRQ_INTEN_H_VAL  PREFIX_VAL(IRQ_INTEN_H_REG)
#endif

#define ICTL1_IRQ_INTEN_H_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_INTEN_H_OFFSET))
#define ICTL1_IRQ_INTEN_H_VAL  PREFIX_VAL(ICTL1_IRQ_INTEN_H_REG)

/* FIELDS: */

/* IRQ_INTEN_H */

#ifndef ICTL_IRQ_INTEN_H_IRQ_INTEN_H_POS
#define ICTL_IRQ_INTEN_H_IRQ_INTEN_H_POS      0
#endif

#ifndef ICTL_IRQ_INTEN_H_IRQ_INTEN_H_LEN
#define ICTL_IRQ_INTEN_H_IRQ_INTEN_H_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_INTEN_H_IRQ_INTEN_H_R)
#define IRQ_INTEN_H_IRQ_INTEN_H_R        GetGroupBits32( (ICTL1_IRQ_INTEN_H_VAL),ICTL_IRQ_INTEN_H_IRQ_INTEN_H_POS, ICTL_IRQ_INTEN_H_IRQ_INTEN_H_LEN)
#endif

#if defined(_V1) && !defined(IRQ_INTEN_H_IRQ_INTEN_H_W)
#define IRQ_INTEN_H_IRQ_INTEN_H_W(value) SetGroupBits32( (ICTL1_IRQ_INTEN_H_VAL),ICTL_IRQ_INTEN_H_IRQ_INTEN_H_POS, ICTL_IRQ_INTEN_H_IRQ_INTEN_H_LEN,value)
#endif

#define ICTL1_IRQ_INTEN_H_IRQ_INTEN_H_R        GetGroupBits32( (ICTL1_IRQ_INTEN_H_VAL),ICTL_IRQ_INTEN_H_IRQ_INTEN_H_POS, ICTL_IRQ_INTEN_H_IRQ_INTEN_H_LEN)

#define ICTL1_IRQ_INTEN_H_IRQ_INTEN_H_W(value) SetGroupBits32( (ICTL1_IRQ_INTEN_H_VAL),ICTL_IRQ_INTEN_H_IRQ_INTEN_H_POS, ICTL_IRQ_INTEN_H_IRQ_INTEN_H_LEN,value)


/* REGISTER: IRQ_VECTOR_0 */

#if defined(_V1) && !defined(IRQ_VECTOR_0_OFFSET)
#define IRQ_VECTOR_0_OFFSET 0x40
#endif

#if !defined(ICTL_IRQ_VECTOR_0_OFFSET)
#define ICTL_IRQ_VECTOR_0_OFFSET 0x40
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_0_REG)
#define IRQ_VECTOR_0_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_0_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_0_VAL)
#define IRQ_VECTOR_0_VAL  PREFIX_VAL(IRQ_VECTOR_0_REG)
#endif

#define ICTL1_IRQ_VECTOR_0_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_0_OFFSET))
#define ICTL1_IRQ_VECTOR_0_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_0_REG)

/* FIELDS: */

/* IRQ_VECTOR_0 */

#ifndef ICTL_IRQ_VECTOR_0_IRQ_VECTOR_0_POS
#define ICTL_IRQ_VECTOR_0_IRQ_VECTOR_0_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_0_IRQ_VECTOR_0_LEN
#define ICTL_IRQ_VECTOR_0_IRQ_VECTOR_0_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_0_IRQ_VECTOR_0_R)
#define IRQ_VECTOR_0_IRQ_VECTOR_0_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_0_VAL),ICTL_IRQ_VECTOR_0_IRQ_VECTOR_0_POS, ICTL_IRQ_VECTOR_0_IRQ_VECTOR_0_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_0_IRQ_VECTOR_0_W)
#define IRQ_VECTOR_0_IRQ_VECTOR_0_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_0_VAL),ICTL_IRQ_VECTOR_0_IRQ_VECTOR_0_POS, ICTL_IRQ_VECTOR_0_IRQ_VECTOR_0_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_0_IRQ_VECTOR_0_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_0_VAL),ICTL_IRQ_VECTOR_0_IRQ_VECTOR_0_POS, ICTL_IRQ_VECTOR_0_IRQ_VECTOR_0_LEN)

#define ICTL1_IRQ_VECTOR_0_IRQ_VECTOR_0_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_0_VAL),ICTL_IRQ_VECTOR_0_IRQ_VECTOR_0_POS, ICTL_IRQ_VECTOR_0_IRQ_VECTOR_0_LEN,value)


/* REGISTER: IRQ_VECTOR_1 */

#if defined(_V1) && !defined(IRQ_VECTOR_1_OFFSET)
#define IRQ_VECTOR_1_OFFSET 0x48
#endif

#if !defined(ICTL_IRQ_VECTOR_1_OFFSET)
#define ICTL_IRQ_VECTOR_1_OFFSET 0x48
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_1_REG)
#define IRQ_VECTOR_1_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_1_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_1_VAL)
#define IRQ_VECTOR_1_VAL  PREFIX_VAL(IRQ_VECTOR_1_REG)
#endif

#define ICTL1_IRQ_VECTOR_1_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_1_OFFSET))
#define ICTL1_IRQ_VECTOR_1_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_1_REG)

/* FIELDS: */

/* IRQ_VECTOR_1 */

#ifndef ICTL_IRQ_VECTOR_1_IRQ_VECTOR_1_POS
#define ICTL_IRQ_VECTOR_1_IRQ_VECTOR_1_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_1_IRQ_VECTOR_1_LEN
#define ICTL_IRQ_VECTOR_1_IRQ_VECTOR_1_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_1_IRQ_VECTOR_1_R)
#define IRQ_VECTOR_1_IRQ_VECTOR_1_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_1_VAL),ICTL_IRQ_VECTOR_1_IRQ_VECTOR_1_POS, ICTL_IRQ_VECTOR_1_IRQ_VECTOR_1_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_1_IRQ_VECTOR_1_W)
#define IRQ_VECTOR_1_IRQ_VECTOR_1_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_1_VAL),ICTL_IRQ_VECTOR_1_IRQ_VECTOR_1_POS, ICTL_IRQ_VECTOR_1_IRQ_VECTOR_1_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_1_IRQ_VECTOR_1_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_1_VAL),ICTL_IRQ_VECTOR_1_IRQ_VECTOR_1_POS, ICTL_IRQ_VECTOR_1_IRQ_VECTOR_1_LEN)

#define ICTL1_IRQ_VECTOR_1_IRQ_VECTOR_1_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_1_VAL),ICTL_IRQ_VECTOR_1_IRQ_VECTOR_1_POS, ICTL_IRQ_VECTOR_1_IRQ_VECTOR_1_LEN,value)


/* REGISTER: IRQ_VECTOR_2 */

#if defined(_V1) && !defined(IRQ_VECTOR_2_OFFSET)
#define IRQ_VECTOR_2_OFFSET 0x50
#endif

#if !defined(ICTL_IRQ_VECTOR_2_OFFSET)
#define ICTL_IRQ_VECTOR_2_OFFSET 0x50
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_2_REG)
#define IRQ_VECTOR_2_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_2_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_2_VAL)
#define IRQ_VECTOR_2_VAL  PREFIX_VAL(IRQ_VECTOR_2_REG)
#endif

#define ICTL1_IRQ_VECTOR_2_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_2_OFFSET))
#define ICTL1_IRQ_VECTOR_2_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_2_REG)

/* FIELDS: */

/* IRQ_VECTOR_2 */

#ifndef ICTL_IRQ_VECTOR_2_IRQ_VECTOR_2_POS
#define ICTL_IRQ_VECTOR_2_IRQ_VECTOR_2_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_2_IRQ_VECTOR_2_LEN
#define ICTL_IRQ_VECTOR_2_IRQ_VECTOR_2_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_2_IRQ_VECTOR_2_R)
#define IRQ_VECTOR_2_IRQ_VECTOR_2_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_2_VAL),ICTL_IRQ_VECTOR_2_IRQ_VECTOR_2_POS, ICTL_IRQ_VECTOR_2_IRQ_VECTOR_2_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_2_IRQ_VECTOR_2_W)
#define IRQ_VECTOR_2_IRQ_VECTOR_2_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_2_VAL),ICTL_IRQ_VECTOR_2_IRQ_VECTOR_2_POS, ICTL_IRQ_VECTOR_2_IRQ_VECTOR_2_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_2_IRQ_VECTOR_2_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_2_VAL),ICTL_IRQ_VECTOR_2_IRQ_VECTOR_2_POS, ICTL_IRQ_VECTOR_2_IRQ_VECTOR_2_LEN)

#define ICTL1_IRQ_VECTOR_2_IRQ_VECTOR_2_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_2_VAL),ICTL_IRQ_VECTOR_2_IRQ_VECTOR_2_POS, ICTL_IRQ_VECTOR_2_IRQ_VECTOR_2_LEN,value)


/* REGISTER: IRQ_VECTOR_3 */

#if defined(_V1) && !defined(IRQ_VECTOR_3_OFFSET)
#define IRQ_VECTOR_3_OFFSET 0x58
#endif

#if !defined(ICTL_IRQ_VECTOR_3_OFFSET)
#define ICTL_IRQ_VECTOR_3_OFFSET 0x58
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_3_REG)
#define IRQ_VECTOR_3_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_3_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_3_VAL)
#define IRQ_VECTOR_3_VAL  PREFIX_VAL(IRQ_VECTOR_3_REG)
#endif

#define ICTL1_IRQ_VECTOR_3_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_3_OFFSET))
#define ICTL1_IRQ_VECTOR_3_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_3_REG)

/* FIELDS: */

/* IRQ_VECTOR_3 */

#ifndef ICTL_IRQ_VECTOR_3_IRQ_VECTOR_3_POS
#define ICTL_IRQ_VECTOR_3_IRQ_VECTOR_3_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_3_IRQ_VECTOR_3_LEN
#define ICTL_IRQ_VECTOR_3_IRQ_VECTOR_3_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_3_IRQ_VECTOR_3_R)
#define IRQ_VECTOR_3_IRQ_VECTOR_3_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_3_VAL),ICTL_IRQ_VECTOR_3_IRQ_VECTOR_3_POS, ICTL_IRQ_VECTOR_3_IRQ_VECTOR_3_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_3_IRQ_VECTOR_3_W)
#define IRQ_VECTOR_3_IRQ_VECTOR_3_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_3_VAL),ICTL_IRQ_VECTOR_3_IRQ_VECTOR_3_POS, ICTL_IRQ_VECTOR_3_IRQ_VECTOR_3_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_3_IRQ_VECTOR_3_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_3_VAL),ICTL_IRQ_VECTOR_3_IRQ_VECTOR_3_POS, ICTL_IRQ_VECTOR_3_IRQ_VECTOR_3_LEN)

#define ICTL1_IRQ_VECTOR_3_IRQ_VECTOR_3_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_3_VAL),ICTL_IRQ_VECTOR_3_IRQ_VECTOR_3_POS, ICTL_IRQ_VECTOR_3_IRQ_VECTOR_3_LEN,value)


/* REGISTER: IRQ_VECTOR_4 */

#if defined(_V1) && !defined(IRQ_VECTOR_4_OFFSET)
#define IRQ_VECTOR_4_OFFSET 0x60
#endif

#if !defined(ICTL_IRQ_VECTOR_4_OFFSET)
#define ICTL_IRQ_VECTOR_4_OFFSET 0x60
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_4_REG)
#define IRQ_VECTOR_4_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_4_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_4_VAL)
#define IRQ_VECTOR_4_VAL  PREFIX_VAL(IRQ_VECTOR_4_REG)
#endif

#define ICTL1_IRQ_VECTOR_4_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_4_OFFSET))
#define ICTL1_IRQ_VECTOR_4_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_4_REG)

/* FIELDS: */

/* IRQ_VECTOR_4 */

#ifndef ICTL_IRQ_VECTOR_4_IRQ_VECTOR_4_POS
#define ICTL_IRQ_VECTOR_4_IRQ_VECTOR_4_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_4_IRQ_VECTOR_4_LEN
#define ICTL_IRQ_VECTOR_4_IRQ_VECTOR_4_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_4_IRQ_VECTOR_4_R)
#define IRQ_VECTOR_4_IRQ_VECTOR_4_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_4_VAL),ICTL_IRQ_VECTOR_4_IRQ_VECTOR_4_POS, ICTL_IRQ_VECTOR_4_IRQ_VECTOR_4_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_4_IRQ_VECTOR_4_W)
#define IRQ_VECTOR_4_IRQ_VECTOR_4_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_4_VAL),ICTL_IRQ_VECTOR_4_IRQ_VECTOR_4_POS, ICTL_IRQ_VECTOR_4_IRQ_VECTOR_4_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_4_IRQ_VECTOR_4_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_4_VAL),ICTL_IRQ_VECTOR_4_IRQ_VECTOR_4_POS, ICTL_IRQ_VECTOR_4_IRQ_VECTOR_4_LEN)

#define ICTL1_IRQ_VECTOR_4_IRQ_VECTOR_4_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_4_VAL),ICTL_IRQ_VECTOR_4_IRQ_VECTOR_4_POS, ICTL_IRQ_VECTOR_4_IRQ_VECTOR_4_LEN,value)


/* REGISTER: IRQ_VECTOR_5 */

#if defined(_V1) && !defined(IRQ_VECTOR_5_OFFSET)
#define IRQ_VECTOR_5_OFFSET 0x68
#endif

#if !defined(ICTL_IRQ_VECTOR_5_OFFSET)
#define ICTL_IRQ_VECTOR_5_OFFSET 0x68
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_5_REG)
#define IRQ_VECTOR_5_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_5_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_5_VAL)
#define IRQ_VECTOR_5_VAL  PREFIX_VAL(IRQ_VECTOR_5_REG)
#endif

#define ICTL1_IRQ_VECTOR_5_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_5_OFFSET))
#define ICTL1_IRQ_VECTOR_5_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_5_REG)

/* FIELDS: */

/* IRQ_VECTOR_5 */

#ifndef ICTL_IRQ_VECTOR_5_IRQ_VECTOR_5_POS
#define ICTL_IRQ_VECTOR_5_IRQ_VECTOR_5_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_5_IRQ_VECTOR_5_LEN
#define ICTL_IRQ_VECTOR_5_IRQ_VECTOR_5_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_5_IRQ_VECTOR_5_R)
#define IRQ_VECTOR_5_IRQ_VECTOR_5_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_5_VAL),ICTL_IRQ_VECTOR_5_IRQ_VECTOR_5_POS, ICTL_IRQ_VECTOR_5_IRQ_VECTOR_5_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_5_IRQ_VECTOR_5_W)
#define IRQ_VECTOR_5_IRQ_VECTOR_5_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_5_VAL),ICTL_IRQ_VECTOR_5_IRQ_VECTOR_5_POS, ICTL_IRQ_VECTOR_5_IRQ_VECTOR_5_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_5_IRQ_VECTOR_5_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_5_VAL),ICTL_IRQ_VECTOR_5_IRQ_VECTOR_5_POS, ICTL_IRQ_VECTOR_5_IRQ_VECTOR_5_LEN)

#define ICTL1_IRQ_VECTOR_5_IRQ_VECTOR_5_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_5_VAL),ICTL_IRQ_VECTOR_5_IRQ_VECTOR_5_POS, ICTL_IRQ_VECTOR_5_IRQ_VECTOR_5_LEN,value)


/* REGISTER: IRQ_VECTOR_6 */

#if defined(_V1) && !defined(IRQ_VECTOR_6_OFFSET)
#define IRQ_VECTOR_6_OFFSET 0x70
#endif

#if !defined(ICTL_IRQ_VECTOR_6_OFFSET)
#define ICTL_IRQ_VECTOR_6_OFFSET 0x70
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_6_REG)
#define IRQ_VECTOR_6_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_6_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_6_VAL)
#define IRQ_VECTOR_6_VAL  PREFIX_VAL(IRQ_VECTOR_6_REG)
#endif

#define ICTL1_IRQ_VECTOR_6_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_6_OFFSET))
#define ICTL1_IRQ_VECTOR_6_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_6_REG)

/* FIELDS: */

/* IRQ_VECTOR_6 */

#ifndef ICTL_IRQ_VECTOR_6_IRQ_VECTOR_6_POS
#define ICTL_IRQ_VECTOR_6_IRQ_VECTOR_6_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_6_IRQ_VECTOR_6_LEN
#define ICTL_IRQ_VECTOR_6_IRQ_VECTOR_6_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_6_IRQ_VECTOR_6_R)
#define IRQ_VECTOR_6_IRQ_VECTOR_6_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_6_VAL),ICTL_IRQ_VECTOR_6_IRQ_VECTOR_6_POS, ICTL_IRQ_VECTOR_6_IRQ_VECTOR_6_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_6_IRQ_VECTOR_6_W)
#define IRQ_VECTOR_6_IRQ_VECTOR_6_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_6_VAL),ICTL_IRQ_VECTOR_6_IRQ_VECTOR_6_POS, ICTL_IRQ_VECTOR_6_IRQ_VECTOR_6_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_6_IRQ_VECTOR_6_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_6_VAL),ICTL_IRQ_VECTOR_6_IRQ_VECTOR_6_POS, ICTL_IRQ_VECTOR_6_IRQ_VECTOR_6_LEN)

#define ICTL1_IRQ_VECTOR_6_IRQ_VECTOR_6_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_6_VAL),ICTL_IRQ_VECTOR_6_IRQ_VECTOR_6_POS, ICTL_IRQ_VECTOR_6_IRQ_VECTOR_6_LEN,value)


/* REGISTER: IRQ_VECTOR_7 */

#if defined(_V1) && !defined(IRQ_VECTOR_7_OFFSET)
#define IRQ_VECTOR_7_OFFSET 0x78
#endif

#if !defined(ICTL_IRQ_VECTOR_7_OFFSET)
#define ICTL_IRQ_VECTOR_7_OFFSET 0x78
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_7_REG)
#define IRQ_VECTOR_7_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_7_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_7_VAL)
#define IRQ_VECTOR_7_VAL  PREFIX_VAL(IRQ_VECTOR_7_REG)
#endif

#define ICTL1_IRQ_VECTOR_7_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_7_OFFSET))
#define ICTL1_IRQ_VECTOR_7_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_7_REG)

/* FIELDS: */

/* IRQ_VECTOR_7 */

#ifndef ICTL_IRQ_VECTOR_7_IRQ_VECTOR_7_POS
#define ICTL_IRQ_VECTOR_7_IRQ_VECTOR_7_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_7_IRQ_VECTOR_7_LEN
#define ICTL_IRQ_VECTOR_7_IRQ_VECTOR_7_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_7_IRQ_VECTOR_7_R)
#define IRQ_VECTOR_7_IRQ_VECTOR_7_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_7_VAL),ICTL_IRQ_VECTOR_7_IRQ_VECTOR_7_POS, ICTL_IRQ_VECTOR_7_IRQ_VECTOR_7_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_7_IRQ_VECTOR_7_W)
#define IRQ_VECTOR_7_IRQ_VECTOR_7_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_7_VAL),ICTL_IRQ_VECTOR_7_IRQ_VECTOR_7_POS, ICTL_IRQ_VECTOR_7_IRQ_VECTOR_7_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_7_IRQ_VECTOR_7_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_7_VAL),ICTL_IRQ_VECTOR_7_IRQ_VECTOR_7_POS, ICTL_IRQ_VECTOR_7_IRQ_VECTOR_7_LEN)

#define ICTL1_IRQ_VECTOR_7_IRQ_VECTOR_7_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_7_VAL),ICTL_IRQ_VECTOR_7_IRQ_VECTOR_7_POS, ICTL_IRQ_VECTOR_7_IRQ_VECTOR_7_LEN,value)


/* REGISTER: IRQ_INTMASK_L */

#if defined(_V1) && !defined(IRQ_INTMASK_L_OFFSET)
#define IRQ_INTMASK_L_OFFSET 0x8
#endif

#if !defined(ICTL_IRQ_INTMASK_L_OFFSET)
#define ICTL_IRQ_INTMASK_L_OFFSET 0x8
#endif

#if defined(_V1) && !defined(IRQ_INTMASK_L_REG)
#define IRQ_INTMASK_L_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_INTMASK_L_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_INTMASK_L_VAL)
#define IRQ_INTMASK_L_VAL  PREFIX_VAL(IRQ_INTMASK_L_REG)
#endif

#define ICTL1_IRQ_INTMASK_L_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_INTMASK_L_OFFSET))
#define ICTL1_IRQ_INTMASK_L_VAL  PREFIX_VAL(ICTL1_IRQ_INTMASK_L_REG)

/* FIELDS: */

/* IRQ_INTMASK_L */

#ifndef ICTL_IRQ_INTMASK_L_IRQ_INTMASK_L_POS
#define ICTL_IRQ_INTMASK_L_IRQ_INTMASK_L_POS      0
#endif

#ifndef ICTL_IRQ_INTMASK_L_IRQ_INTMASK_L_LEN
#define ICTL_IRQ_INTMASK_L_IRQ_INTMASK_L_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_INTMASK_L_IRQ_INTMASK_L_R)
#define IRQ_INTMASK_L_IRQ_INTMASK_L_R        GetGroupBits32( (ICTL1_IRQ_INTMASK_L_VAL),ICTL_IRQ_INTMASK_L_IRQ_INTMASK_L_POS, ICTL_IRQ_INTMASK_L_IRQ_INTMASK_L_LEN)
#endif

#if defined(_V1) && !defined(IRQ_INTMASK_L_IRQ_INTMASK_L_W)
#define IRQ_INTMASK_L_IRQ_INTMASK_L_W(value) SetGroupBits32( (ICTL1_IRQ_INTMASK_L_VAL),ICTL_IRQ_INTMASK_L_IRQ_INTMASK_L_POS, ICTL_IRQ_INTMASK_L_IRQ_INTMASK_L_LEN,value)
#endif

#define ICTL1_IRQ_INTMASK_L_IRQ_INTMASK_L_R        GetGroupBits32( (ICTL1_IRQ_INTMASK_L_VAL),ICTL_IRQ_INTMASK_L_IRQ_INTMASK_L_POS, ICTL_IRQ_INTMASK_L_IRQ_INTMASK_L_LEN)

#define ICTL1_IRQ_INTMASK_L_IRQ_INTMASK_L_W(value) SetGroupBits32( (ICTL1_IRQ_INTMASK_L_VAL),ICTL_IRQ_INTMASK_L_IRQ_INTMASK_L_POS, ICTL_IRQ_INTMASK_L_IRQ_INTMASK_L_LEN,value)


/* REGISTER: IRQ_VECTOR_8 */

#if defined(_V1) && !defined(IRQ_VECTOR_8_OFFSET)
#define IRQ_VECTOR_8_OFFSET 0x80
#endif

#if !defined(ICTL_IRQ_VECTOR_8_OFFSET)
#define ICTL_IRQ_VECTOR_8_OFFSET 0x80
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_8_REG)
#define IRQ_VECTOR_8_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_8_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_8_VAL)
#define IRQ_VECTOR_8_VAL  PREFIX_VAL(IRQ_VECTOR_8_REG)
#endif

#define ICTL1_IRQ_VECTOR_8_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_8_OFFSET))
#define ICTL1_IRQ_VECTOR_8_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_8_REG)

/* FIELDS: */

/* IRQ_VECTOR_8 */

#ifndef ICTL_IRQ_VECTOR_8_IRQ_VECTOR_8_POS
#define ICTL_IRQ_VECTOR_8_IRQ_VECTOR_8_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_8_IRQ_VECTOR_8_LEN
#define ICTL_IRQ_VECTOR_8_IRQ_VECTOR_8_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_8_IRQ_VECTOR_8_R)
#define IRQ_VECTOR_8_IRQ_VECTOR_8_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_8_VAL),ICTL_IRQ_VECTOR_8_IRQ_VECTOR_8_POS, ICTL_IRQ_VECTOR_8_IRQ_VECTOR_8_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_8_IRQ_VECTOR_8_W)
#define IRQ_VECTOR_8_IRQ_VECTOR_8_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_8_VAL),ICTL_IRQ_VECTOR_8_IRQ_VECTOR_8_POS, ICTL_IRQ_VECTOR_8_IRQ_VECTOR_8_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_8_IRQ_VECTOR_8_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_8_VAL),ICTL_IRQ_VECTOR_8_IRQ_VECTOR_8_POS, ICTL_IRQ_VECTOR_8_IRQ_VECTOR_8_LEN)

#define ICTL1_IRQ_VECTOR_8_IRQ_VECTOR_8_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_8_VAL),ICTL_IRQ_VECTOR_8_IRQ_VECTOR_8_POS, ICTL_IRQ_VECTOR_8_IRQ_VECTOR_8_LEN,value)


/* REGISTER: IRQ_VECTOR_9 */

#if defined(_V1) && !defined(IRQ_VECTOR_9_OFFSET)
#define IRQ_VECTOR_9_OFFSET 0x88
#endif

#if !defined(ICTL_IRQ_VECTOR_9_OFFSET)
#define ICTL_IRQ_VECTOR_9_OFFSET 0x88
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_9_REG)
#define IRQ_VECTOR_9_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_9_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_9_VAL)
#define IRQ_VECTOR_9_VAL  PREFIX_VAL(IRQ_VECTOR_9_REG)
#endif

#define ICTL1_IRQ_VECTOR_9_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_9_OFFSET))
#define ICTL1_IRQ_VECTOR_9_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_9_REG)

/* FIELDS: */

/* IRQ_VECTOR_9 */

#ifndef ICTL_IRQ_VECTOR_9_IRQ_VECTOR_9_POS
#define ICTL_IRQ_VECTOR_9_IRQ_VECTOR_9_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_9_IRQ_VECTOR_9_LEN
#define ICTL_IRQ_VECTOR_9_IRQ_VECTOR_9_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_9_IRQ_VECTOR_9_R)
#define IRQ_VECTOR_9_IRQ_VECTOR_9_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_9_VAL),ICTL_IRQ_VECTOR_9_IRQ_VECTOR_9_POS, ICTL_IRQ_VECTOR_9_IRQ_VECTOR_9_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_9_IRQ_VECTOR_9_W)
#define IRQ_VECTOR_9_IRQ_VECTOR_9_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_9_VAL),ICTL_IRQ_VECTOR_9_IRQ_VECTOR_9_POS, ICTL_IRQ_VECTOR_9_IRQ_VECTOR_9_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_9_IRQ_VECTOR_9_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_9_VAL),ICTL_IRQ_VECTOR_9_IRQ_VECTOR_9_POS, ICTL_IRQ_VECTOR_9_IRQ_VECTOR_9_LEN)

#define ICTL1_IRQ_VECTOR_9_IRQ_VECTOR_9_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_9_VAL),ICTL_IRQ_VECTOR_9_IRQ_VECTOR_9_POS, ICTL_IRQ_VECTOR_9_IRQ_VECTOR_9_LEN,value)


/* REGISTER: IRQ_VECTOR_10 */

#if defined(_V1) && !defined(IRQ_VECTOR_10_OFFSET)
#define IRQ_VECTOR_10_OFFSET 0x90
#endif

#if !defined(ICTL_IRQ_VECTOR_10_OFFSET)
#define ICTL_IRQ_VECTOR_10_OFFSET 0x90
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_10_REG)
#define IRQ_VECTOR_10_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_10_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_10_VAL)
#define IRQ_VECTOR_10_VAL  PREFIX_VAL(IRQ_VECTOR_10_REG)
#endif

#define ICTL1_IRQ_VECTOR_10_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_10_OFFSET))
#define ICTL1_IRQ_VECTOR_10_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_10_REG)

/* FIELDS: */

/* IRQ_VECTOR_10 */

#ifndef ICTL_IRQ_VECTOR_10_IRQ_VECTOR_10_POS
#define ICTL_IRQ_VECTOR_10_IRQ_VECTOR_10_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_10_IRQ_VECTOR_10_LEN
#define ICTL_IRQ_VECTOR_10_IRQ_VECTOR_10_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_10_IRQ_VECTOR_10_R)
#define IRQ_VECTOR_10_IRQ_VECTOR_10_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_10_VAL),ICTL_IRQ_VECTOR_10_IRQ_VECTOR_10_POS, ICTL_IRQ_VECTOR_10_IRQ_VECTOR_10_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_10_IRQ_VECTOR_10_W)
#define IRQ_VECTOR_10_IRQ_VECTOR_10_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_10_VAL),ICTL_IRQ_VECTOR_10_IRQ_VECTOR_10_POS, ICTL_IRQ_VECTOR_10_IRQ_VECTOR_10_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_10_IRQ_VECTOR_10_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_10_VAL),ICTL_IRQ_VECTOR_10_IRQ_VECTOR_10_POS, ICTL_IRQ_VECTOR_10_IRQ_VECTOR_10_LEN)

#define ICTL1_IRQ_VECTOR_10_IRQ_VECTOR_10_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_10_VAL),ICTL_IRQ_VECTOR_10_IRQ_VECTOR_10_POS, ICTL_IRQ_VECTOR_10_IRQ_VECTOR_10_LEN,value)


/* REGISTER: IRQ_VECTOR_11 */

#if defined(_V1) && !defined(IRQ_VECTOR_11_OFFSET)
#define IRQ_VECTOR_11_OFFSET 0x98
#endif

#if !defined(ICTL_IRQ_VECTOR_11_OFFSET)
#define ICTL_IRQ_VECTOR_11_OFFSET 0x98
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_11_REG)
#define IRQ_VECTOR_11_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_11_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_11_VAL)
#define IRQ_VECTOR_11_VAL  PREFIX_VAL(IRQ_VECTOR_11_REG)
#endif

#define ICTL1_IRQ_VECTOR_11_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_11_OFFSET))
#define ICTL1_IRQ_VECTOR_11_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_11_REG)

/* FIELDS: */

/* IRQ_VECTOR_11 */

#ifndef ICTL_IRQ_VECTOR_11_IRQ_VECTOR_11_POS
#define ICTL_IRQ_VECTOR_11_IRQ_VECTOR_11_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_11_IRQ_VECTOR_11_LEN
#define ICTL_IRQ_VECTOR_11_IRQ_VECTOR_11_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_11_IRQ_VECTOR_11_R)
#define IRQ_VECTOR_11_IRQ_VECTOR_11_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_11_VAL),ICTL_IRQ_VECTOR_11_IRQ_VECTOR_11_POS, ICTL_IRQ_VECTOR_11_IRQ_VECTOR_11_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_11_IRQ_VECTOR_11_W)
#define IRQ_VECTOR_11_IRQ_VECTOR_11_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_11_VAL),ICTL_IRQ_VECTOR_11_IRQ_VECTOR_11_POS, ICTL_IRQ_VECTOR_11_IRQ_VECTOR_11_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_11_IRQ_VECTOR_11_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_11_VAL),ICTL_IRQ_VECTOR_11_IRQ_VECTOR_11_POS, ICTL_IRQ_VECTOR_11_IRQ_VECTOR_11_LEN)

#define ICTL1_IRQ_VECTOR_11_IRQ_VECTOR_11_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_11_VAL),ICTL_IRQ_VECTOR_11_IRQ_VECTOR_11_POS, ICTL_IRQ_VECTOR_11_IRQ_VECTOR_11_LEN,value)


/* REGISTER: IRQ_VECTOR_12 */

#if defined(_V1) && !defined(IRQ_VECTOR_12_OFFSET)
#define IRQ_VECTOR_12_OFFSET 0xa0
#endif

#if !defined(ICTL_IRQ_VECTOR_12_OFFSET)
#define ICTL_IRQ_VECTOR_12_OFFSET 0xa0
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_12_REG)
#define IRQ_VECTOR_12_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_12_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_12_VAL)
#define IRQ_VECTOR_12_VAL  PREFIX_VAL(IRQ_VECTOR_12_REG)
#endif

#define ICTL1_IRQ_VECTOR_12_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_12_OFFSET))
#define ICTL1_IRQ_VECTOR_12_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_12_REG)

/* FIELDS: */

/* IRQ_VECTOR_12 */

#ifndef ICTL_IRQ_VECTOR_12_IRQ_VECTOR_12_POS
#define ICTL_IRQ_VECTOR_12_IRQ_VECTOR_12_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_12_IRQ_VECTOR_12_LEN
#define ICTL_IRQ_VECTOR_12_IRQ_VECTOR_12_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_12_IRQ_VECTOR_12_R)
#define IRQ_VECTOR_12_IRQ_VECTOR_12_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_12_VAL),ICTL_IRQ_VECTOR_12_IRQ_VECTOR_12_POS, ICTL_IRQ_VECTOR_12_IRQ_VECTOR_12_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_12_IRQ_VECTOR_12_W)
#define IRQ_VECTOR_12_IRQ_VECTOR_12_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_12_VAL),ICTL_IRQ_VECTOR_12_IRQ_VECTOR_12_POS, ICTL_IRQ_VECTOR_12_IRQ_VECTOR_12_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_12_IRQ_VECTOR_12_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_12_VAL),ICTL_IRQ_VECTOR_12_IRQ_VECTOR_12_POS, ICTL_IRQ_VECTOR_12_IRQ_VECTOR_12_LEN)

#define ICTL1_IRQ_VECTOR_12_IRQ_VECTOR_12_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_12_VAL),ICTL_IRQ_VECTOR_12_IRQ_VECTOR_12_POS, ICTL_IRQ_VECTOR_12_IRQ_VECTOR_12_LEN,value)


/* REGISTER: IRQ_VECTOR_13 */

#if defined(_V1) && !defined(IRQ_VECTOR_13_OFFSET)
#define IRQ_VECTOR_13_OFFSET 0xa8
#endif

#if !defined(ICTL_IRQ_VECTOR_13_OFFSET)
#define ICTL_IRQ_VECTOR_13_OFFSET 0xa8
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_13_REG)
#define IRQ_VECTOR_13_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_13_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_13_VAL)
#define IRQ_VECTOR_13_VAL  PREFIX_VAL(IRQ_VECTOR_13_REG)
#endif

#define ICTL1_IRQ_VECTOR_13_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_13_OFFSET))
#define ICTL1_IRQ_VECTOR_13_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_13_REG)

/* FIELDS: */

/* IRQ_VECTOR_13 */

#ifndef ICTL_IRQ_VECTOR_13_IRQ_VECTOR_13_POS
#define ICTL_IRQ_VECTOR_13_IRQ_VECTOR_13_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_13_IRQ_VECTOR_13_LEN
#define ICTL_IRQ_VECTOR_13_IRQ_VECTOR_13_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_13_IRQ_VECTOR_13_R)
#define IRQ_VECTOR_13_IRQ_VECTOR_13_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_13_VAL),ICTL_IRQ_VECTOR_13_IRQ_VECTOR_13_POS, ICTL_IRQ_VECTOR_13_IRQ_VECTOR_13_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_13_IRQ_VECTOR_13_W)
#define IRQ_VECTOR_13_IRQ_VECTOR_13_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_13_VAL),ICTL_IRQ_VECTOR_13_IRQ_VECTOR_13_POS, ICTL_IRQ_VECTOR_13_IRQ_VECTOR_13_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_13_IRQ_VECTOR_13_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_13_VAL),ICTL_IRQ_VECTOR_13_IRQ_VECTOR_13_POS, ICTL_IRQ_VECTOR_13_IRQ_VECTOR_13_LEN)

#define ICTL1_IRQ_VECTOR_13_IRQ_VECTOR_13_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_13_VAL),ICTL_IRQ_VECTOR_13_IRQ_VECTOR_13_POS, ICTL_IRQ_VECTOR_13_IRQ_VECTOR_13_LEN,value)


/* REGISTER: IRQ_VECTOR_14 */

#if defined(_V1) && !defined(IRQ_VECTOR_14_OFFSET)
#define IRQ_VECTOR_14_OFFSET 0xb0
#endif

#if !defined(ICTL_IRQ_VECTOR_14_OFFSET)
#define ICTL_IRQ_VECTOR_14_OFFSET 0xb0
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_14_REG)
#define IRQ_VECTOR_14_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_14_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_14_VAL)
#define IRQ_VECTOR_14_VAL  PREFIX_VAL(IRQ_VECTOR_14_REG)
#endif

#define ICTL1_IRQ_VECTOR_14_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_14_OFFSET))
#define ICTL1_IRQ_VECTOR_14_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_14_REG)

/* FIELDS: */

/* IRQ_VECTOR_14 */

#ifndef ICTL_IRQ_VECTOR_14_IRQ_VECTOR_14_POS
#define ICTL_IRQ_VECTOR_14_IRQ_VECTOR_14_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_14_IRQ_VECTOR_14_LEN
#define ICTL_IRQ_VECTOR_14_IRQ_VECTOR_14_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_14_IRQ_VECTOR_14_R)
#define IRQ_VECTOR_14_IRQ_VECTOR_14_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_14_VAL),ICTL_IRQ_VECTOR_14_IRQ_VECTOR_14_POS, ICTL_IRQ_VECTOR_14_IRQ_VECTOR_14_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_14_IRQ_VECTOR_14_W)
#define IRQ_VECTOR_14_IRQ_VECTOR_14_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_14_VAL),ICTL_IRQ_VECTOR_14_IRQ_VECTOR_14_POS, ICTL_IRQ_VECTOR_14_IRQ_VECTOR_14_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_14_IRQ_VECTOR_14_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_14_VAL),ICTL_IRQ_VECTOR_14_IRQ_VECTOR_14_POS, ICTL_IRQ_VECTOR_14_IRQ_VECTOR_14_LEN)

#define ICTL1_IRQ_VECTOR_14_IRQ_VECTOR_14_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_14_VAL),ICTL_IRQ_VECTOR_14_IRQ_VECTOR_14_POS, ICTL_IRQ_VECTOR_14_IRQ_VECTOR_14_LEN,value)


/* REGISTER: IRQ_VECTOR_15 */

#if defined(_V1) && !defined(IRQ_VECTOR_15_OFFSET)
#define IRQ_VECTOR_15_OFFSET 0xb8
#endif

#if !defined(ICTL_IRQ_VECTOR_15_OFFSET)
#define ICTL_IRQ_VECTOR_15_OFFSET 0xb8
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_15_REG)
#define IRQ_VECTOR_15_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_15_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_15_VAL)
#define IRQ_VECTOR_15_VAL  PREFIX_VAL(IRQ_VECTOR_15_REG)
#endif

#define ICTL1_IRQ_VECTOR_15_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_VECTOR_15_OFFSET))
#define ICTL1_IRQ_VECTOR_15_VAL  PREFIX_VAL(ICTL1_IRQ_VECTOR_15_REG)

/* FIELDS: */

/* IRQ_VECTOR_15 */

#ifndef ICTL_IRQ_VECTOR_15_IRQ_VECTOR_15_POS
#define ICTL_IRQ_VECTOR_15_IRQ_VECTOR_15_POS      0
#endif

#ifndef ICTL_IRQ_VECTOR_15_IRQ_VECTOR_15_LEN
#define ICTL_IRQ_VECTOR_15_IRQ_VECTOR_15_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_15_IRQ_VECTOR_15_R)
#define IRQ_VECTOR_15_IRQ_VECTOR_15_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_15_VAL),ICTL_IRQ_VECTOR_15_IRQ_VECTOR_15_POS, ICTL_IRQ_VECTOR_15_IRQ_VECTOR_15_LEN)
#endif

#if defined(_V1) && !defined(IRQ_VECTOR_15_IRQ_VECTOR_15_W)
#define IRQ_VECTOR_15_IRQ_VECTOR_15_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_15_VAL),ICTL_IRQ_VECTOR_15_IRQ_VECTOR_15_POS, ICTL_IRQ_VECTOR_15_IRQ_VECTOR_15_LEN,value)
#endif

#define ICTL1_IRQ_VECTOR_15_IRQ_VECTOR_15_R        GetGroupBits32( (ICTL1_IRQ_VECTOR_15_VAL),ICTL_IRQ_VECTOR_15_IRQ_VECTOR_15_POS, ICTL_IRQ_VECTOR_15_IRQ_VECTOR_15_LEN)

#define ICTL1_IRQ_VECTOR_15_IRQ_VECTOR_15_W(value) SetGroupBits32( (ICTL1_IRQ_VECTOR_15_VAL),ICTL_IRQ_VECTOR_15_IRQ_VECTOR_15_POS, ICTL_IRQ_VECTOR_15_IRQ_VECTOR_15_LEN,value)


/* REGISTER: IRQ_INTMASK_H */

#if defined(_V1) && !defined(IRQ_INTMASK_H_OFFSET)
#define IRQ_INTMASK_H_OFFSET 0xc
#endif

#if !defined(ICTL_IRQ_INTMASK_H_OFFSET)
#define ICTL_IRQ_INTMASK_H_OFFSET 0xc
#endif

#if defined(_V1) && !defined(IRQ_INTMASK_H_REG)
#define IRQ_INTMASK_H_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_INTMASK_H_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_INTMASK_H_VAL)
#define IRQ_INTMASK_H_VAL  PREFIX_VAL(IRQ_INTMASK_H_REG)
#endif

#define ICTL1_IRQ_INTMASK_H_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_INTMASK_H_OFFSET))
#define ICTL1_IRQ_INTMASK_H_VAL  PREFIX_VAL(ICTL1_IRQ_INTMASK_H_REG)

/* FIELDS: */

/* IRQ_INTMASK_H */

#ifndef ICTL_IRQ_INTMASK_H_IRQ_INTMASK_H_POS
#define ICTL_IRQ_INTMASK_H_IRQ_INTMASK_H_POS      0
#endif

#ifndef ICTL_IRQ_INTMASK_H_IRQ_INTMASK_H_LEN
#define ICTL_IRQ_INTMASK_H_IRQ_INTMASK_H_LEN      32
#endif

#if defined(_V1) && !defined(IRQ_INTMASK_H_IRQ_INTMASK_H_R)
#define IRQ_INTMASK_H_IRQ_INTMASK_H_R        GetGroupBits32( (ICTL1_IRQ_INTMASK_H_VAL),ICTL_IRQ_INTMASK_H_IRQ_INTMASK_H_POS, ICTL_IRQ_INTMASK_H_IRQ_INTMASK_H_LEN)
#endif

#if defined(_V1) && !defined(IRQ_INTMASK_H_IRQ_INTMASK_H_W)
#define IRQ_INTMASK_H_IRQ_INTMASK_H_W(value) SetGroupBits32( (ICTL1_IRQ_INTMASK_H_VAL),ICTL_IRQ_INTMASK_H_IRQ_INTMASK_H_POS, ICTL_IRQ_INTMASK_H_IRQ_INTMASK_H_LEN,value)
#endif

#define ICTL1_IRQ_INTMASK_H_IRQ_INTMASK_H_R        GetGroupBits32( (ICTL1_IRQ_INTMASK_H_VAL),ICTL_IRQ_INTMASK_H_IRQ_INTMASK_H_POS, ICTL_IRQ_INTMASK_H_IRQ_INTMASK_H_LEN)

#define ICTL1_IRQ_INTMASK_H_IRQ_INTMASK_H_W(value) SetGroupBits32( (ICTL1_IRQ_INTMASK_H_VAL),ICTL_IRQ_INTMASK_H_IRQ_INTMASK_H_POS, ICTL_IRQ_INTMASK_H_IRQ_INTMASK_H_LEN,value)


/* REGISTER: FIQ_INTEN */

#if defined(_V1) && !defined(FIQ_INTEN_OFFSET)
#define FIQ_INTEN_OFFSET 0xc0
#endif

#if !defined(ICTL_FIQ_INTEN_OFFSET)
#define ICTL_FIQ_INTEN_OFFSET 0xc0
#endif

#if defined(_V1) && !defined(FIQ_INTEN_REG)
#define FIQ_INTEN_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_FIQ_INTEN_OFFSET))
#endif

#if defined(_V1) && !defined(FIQ_INTEN_VAL)
#define FIQ_INTEN_VAL  PREFIX_VAL(FIQ_INTEN_REG)
#endif

#define ICTL1_FIQ_INTEN_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_FIQ_INTEN_OFFSET))
#define ICTL1_FIQ_INTEN_VAL  PREFIX_VAL(ICTL1_FIQ_INTEN_REG)

/* FIELDS: */

/* FIQ_INTEN */

#ifndef ICTL_FIQ_INTEN_FIQ_INTEN_POS
#define ICTL_FIQ_INTEN_FIQ_INTEN_POS      0
#endif

#ifndef ICTL_FIQ_INTEN_FIQ_INTEN_LEN
#define ICTL_FIQ_INTEN_FIQ_INTEN_LEN      8
#endif

#if defined(_V1) && !defined(FIQ_INTEN_FIQ_INTEN_R)
#define FIQ_INTEN_FIQ_INTEN_R        GetGroupBits32( (ICTL1_FIQ_INTEN_VAL),ICTL_FIQ_INTEN_FIQ_INTEN_POS, ICTL_FIQ_INTEN_FIQ_INTEN_LEN)
#endif

#if defined(_V1) && !defined(FIQ_INTEN_FIQ_INTEN_W)
#define FIQ_INTEN_FIQ_INTEN_W(value) SetGroupBits32( (ICTL1_FIQ_INTEN_VAL),ICTL_FIQ_INTEN_FIQ_INTEN_POS, ICTL_FIQ_INTEN_FIQ_INTEN_LEN,value)
#endif

#define ICTL1_FIQ_INTEN_FIQ_INTEN_R        GetGroupBits32( (ICTL1_FIQ_INTEN_VAL),ICTL_FIQ_INTEN_FIQ_INTEN_POS, ICTL_FIQ_INTEN_FIQ_INTEN_LEN)

#define ICTL1_FIQ_INTEN_FIQ_INTEN_W(value) SetGroupBits32( (ICTL1_FIQ_INTEN_VAL),ICTL_FIQ_INTEN_FIQ_INTEN_POS, ICTL_FIQ_INTEN_FIQ_INTEN_LEN,value)


/* RSVD_FIQ_INTEN */

#ifndef ICTL_FIQ_INTEN_RSVD_FIQ_INTEN_POS
#define ICTL_FIQ_INTEN_RSVD_FIQ_INTEN_POS      8
#endif

#ifndef ICTL_FIQ_INTEN_RSVD_FIQ_INTEN_LEN
#define ICTL_FIQ_INTEN_RSVD_FIQ_INTEN_LEN      24
#endif

#if defined(_V1) && !defined(FIQ_INTEN_RSVD_FIQ_INTEN_R)
#define FIQ_INTEN_RSVD_FIQ_INTEN_R        GetGroupBits32( (ICTL1_FIQ_INTEN_VAL),ICTL_FIQ_INTEN_RSVD_FIQ_INTEN_POS, ICTL_FIQ_INTEN_RSVD_FIQ_INTEN_LEN)
#endif

#if defined(_V1) && !defined(FIQ_INTEN_RSVD_FIQ_INTEN_W)
#define FIQ_INTEN_RSVD_FIQ_INTEN_W(value) SetGroupBits32( (ICTL1_FIQ_INTEN_VAL),ICTL_FIQ_INTEN_RSVD_FIQ_INTEN_POS, ICTL_FIQ_INTEN_RSVD_FIQ_INTEN_LEN,value)
#endif

#define ICTL1_FIQ_INTEN_RSVD_FIQ_INTEN_R        GetGroupBits32( (ICTL1_FIQ_INTEN_VAL),ICTL_FIQ_INTEN_RSVD_FIQ_INTEN_POS, ICTL_FIQ_INTEN_RSVD_FIQ_INTEN_LEN)

#define ICTL1_FIQ_INTEN_RSVD_FIQ_INTEN_W(value) SetGroupBits32( (ICTL1_FIQ_INTEN_VAL),ICTL_FIQ_INTEN_RSVD_FIQ_INTEN_POS, ICTL_FIQ_INTEN_RSVD_FIQ_INTEN_LEN,value)


/* REGISTER: FIQ_INTMASK */

#if defined(_V1) && !defined(FIQ_INTMASK_OFFSET)
#define FIQ_INTMASK_OFFSET 0xc4
#endif

#if !defined(ICTL_FIQ_INTMASK_OFFSET)
#define ICTL_FIQ_INTMASK_OFFSET 0xc4
#endif

#if defined(_V1) && !defined(FIQ_INTMASK_REG)
#define FIQ_INTMASK_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_FIQ_INTMASK_OFFSET))
#endif

#if defined(_V1) && !defined(FIQ_INTMASK_VAL)
#define FIQ_INTMASK_VAL  PREFIX_VAL(FIQ_INTMASK_REG)
#endif

#define ICTL1_FIQ_INTMASK_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_FIQ_INTMASK_OFFSET))
#define ICTL1_FIQ_INTMASK_VAL  PREFIX_VAL(ICTL1_FIQ_INTMASK_REG)

/* FIELDS: */

/* FIQ_INTMASK */

#ifndef ICTL_FIQ_INTMASK_FIQ_INTMASK_POS
#define ICTL_FIQ_INTMASK_FIQ_INTMASK_POS      0
#endif

#ifndef ICTL_FIQ_INTMASK_FIQ_INTMASK_LEN
#define ICTL_FIQ_INTMASK_FIQ_INTMASK_LEN      8
#endif

#if defined(_V1) && !defined(FIQ_INTMASK_FIQ_INTMASK_R)
#define FIQ_INTMASK_FIQ_INTMASK_R        GetGroupBits32( (ICTL1_FIQ_INTMASK_VAL),ICTL_FIQ_INTMASK_FIQ_INTMASK_POS, ICTL_FIQ_INTMASK_FIQ_INTMASK_LEN)
#endif

#if defined(_V1) && !defined(FIQ_INTMASK_FIQ_INTMASK_W)
#define FIQ_INTMASK_FIQ_INTMASK_W(value) SetGroupBits32( (ICTL1_FIQ_INTMASK_VAL),ICTL_FIQ_INTMASK_FIQ_INTMASK_POS, ICTL_FIQ_INTMASK_FIQ_INTMASK_LEN,value)
#endif

#define ICTL1_FIQ_INTMASK_FIQ_INTMASK_R        GetGroupBits32( (ICTL1_FIQ_INTMASK_VAL),ICTL_FIQ_INTMASK_FIQ_INTMASK_POS, ICTL_FIQ_INTMASK_FIQ_INTMASK_LEN)

#define ICTL1_FIQ_INTMASK_FIQ_INTMASK_W(value) SetGroupBits32( (ICTL1_FIQ_INTMASK_VAL),ICTL_FIQ_INTMASK_FIQ_INTMASK_POS, ICTL_FIQ_INTMASK_FIQ_INTMASK_LEN,value)


/* RSVD_FIQ_INTMASK */

#ifndef ICTL_FIQ_INTMASK_RSVD_FIQ_INTMASK_POS
#define ICTL_FIQ_INTMASK_RSVD_FIQ_INTMASK_POS      8
#endif

#ifndef ICTL_FIQ_INTMASK_RSVD_FIQ_INTMASK_LEN
#define ICTL_FIQ_INTMASK_RSVD_FIQ_INTMASK_LEN      24
#endif

#if defined(_V1) && !defined(FIQ_INTMASK_RSVD_FIQ_INTMASK_R)
#define FIQ_INTMASK_RSVD_FIQ_INTMASK_R        GetGroupBits32( (ICTL1_FIQ_INTMASK_VAL),ICTL_FIQ_INTMASK_RSVD_FIQ_INTMASK_POS, ICTL_FIQ_INTMASK_RSVD_FIQ_INTMASK_LEN)
#endif

#if defined(_V1) && !defined(FIQ_INTMASK_RSVD_FIQ_INTMASK_W)
#define FIQ_INTMASK_RSVD_FIQ_INTMASK_W(value) SetGroupBits32( (ICTL1_FIQ_INTMASK_VAL),ICTL_FIQ_INTMASK_RSVD_FIQ_INTMASK_POS, ICTL_FIQ_INTMASK_RSVD_FIQ_INTMASK_LEN,value)
#endif

#define ICTL1_FIQ_INTMASK_RSVD_FIQ_INTMASK_R        GetGroupBits32( (ICTL1_FIQ_INTMASK_VAL),ICTL_FIQ_INTMASK_RSVD_FIQ_INTMASK_POS, ICTL_FIQ_INTMASK_RSVD_FIQ_INTMASK_LEN)

#define ICTL1_FIQ_INTMASK_RSVD_FIQ_INTMASK_W(value) SetGroupBits32( (ICTL1_FIQ_INTMASK_VAL),ICTL_FIQ_INTMASK_RSVD_FIQ_INTMASK_POS, ICTL_FIQ_INTMASK_RSVD_FIQ_INTMASK_LEN,value)


/* REGISTER: FIQ_INTFORCE */

#if defined(_V1) && !defined(FIQ_INTFORCE_OFFSET)
#define FIQ_INTFORCE_OFFSET 0xc8
#endif

#if !defined(ICTL_FIQ_INTFORCE_OFFSET)
#define ICTL_FIQ_INTFORCE_OFFSET 0xc8
#endif

#if defined(_V1) && !defined(FIQ_INTFORCE_REG)
#define FIQ_INTFORCE_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_FIQ_INTFORCE_OFFSET))
#endif

#if defined(_V1) && !defined(FIQ_INTFORCE_VAL)
#define FIQ_INTFORCE_VAL  PREFIX_VAL(FIQ_INTFORCE_REG)
#endif

#define ICTL1_FIQ_INTFORCE_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_FIQ_INTFORCE_OFFSET))
#define ICTL1_FIQ_INTFORCE_VAL  PREFIX_VAL(ICTL1_FIQ_INTFORCE_REG)

/* FIELDS: */

/* FIQ_INTFORCE */

#ifndef ICTL_FIQ_INTFORCE_FIQ_INTFORCE_POS
#define ICTL_FIQ_INTFORCE_FIQ_INTFORCE_POS      0
#endif

#ifndef ICTL_FIQ_INTFORCE_FIQ_INTFORCE_LEN
#define ICTL_FIQ_INTFORCE_FIQ_INTFORCE_LEN      8
#endif

#if defined(_V1) && !defined(FIQ_INTFORCE_FIQ_INTFORCE_R)
#define FIQ_INTFORCE_FIQ_INTFORCE_R        GetGroupBits32( (ICTL1_FIQ_INTFORCE_VAL),ICTL_FIQ_INTFORCE_FIQ_INTFORCE_POS, ICTL_FIQ_INTFORCE_FIQ_INTFORCE_LEN)
#endif

#if defined(_V1) && !defined(FIQ_INTFORCE_FIQ_INTFORCE_W)
#define FIQ_INTFORCE_FIQ_INTFORCE_W(value) SetGroupBits32( (ICTL1_FIQ_INTFORCE_VAL),ICTL_FIQ_INTFORCE_FIQ_INTFORCE_POS, ICTL_FIQ_INTFORCE_FIQ_INTFORCE_LEN,value)
#endif

#define ICTL1_FIQ_INTFORCE_FIQ_INTFORCE_R        GetGroupBits32( (ICTL1_FIQ_INTFORCE_VAL),ICTL_FIQ_INTFORCE_FIQ_INTFORCE_POS, ICTL_FIQ_INTFORCE_FIQ_INTFORCE_LEN)

#define ICTL1_FIQ_INTFORCE_FIQ_INTFORCE_W(value) SetGroupBits32( (ICTL1_FIQ_INTFORCE_VAL),ICTL_FIQ_INTFORCE_FIQ_INTFORCE_POS, ICTL_FIQ_INTFORCE_FIQ_INTFORCE_LEN,value)


/* RSVD_FIQ_INTFORCE */

#ifndef ICTL_FIQ_INTFORCE_RSVD_FIQ_INTFORCE_POS
#define ICTL_FIQ_INTFORCE_RSVD_FIQ_INTFORCE_POS      8
#endif

#ifndef ICTL_FIQ_INTFORCE_RSVD_FIQ_INTFORCE_LEN
#define ICTL_FIQ_INTFORCE_RSVD_FIQ_INTFORCE_LEN      24
#endif

#if defined(_V1) && !defined(FIQ_INTFORCE_RSVD_FIQ_INTFORCE_R)
#define FIQ_INTFORCE_RSVD_FIQ_INTFORCE_R        GetGroupBits32( (ICTL1_FIQ_INTFORCE_VAL),ICTL_FIQ_INTFORCE_RSVD_FIQ_INTFORCE_POS, ICTL_FIQ_INTFORCE_RSVD_FIQ_INTFORCE_LEN)
#endif

#if defined(_V1) && !defined(FIQ_INTFORCE_RSVD_FIQ_INTFORCE_W)
#define FIQ_INTFORCE_RSVD_FIQ_INTFORCE_W(value) SetGroupBits32( (ICTL1_FIQ_INTFORCE_VAL),ICTL_FIQ_INTFORCE_RSVD_FIQ_INTFORCE_POS, ICTL_FIQ_INTFORCE_RSVD_FIQ_INTFORCE_LEN,value)
#endif

#define ICTL1_FIQ_INTFORCE_RSVD_FIQ_INTFORCE_R        GetGroupBits32( (ICTL1_FIQ_INTFORCE_VAL),ICTL_FIQ_INTFORCE_RSVD_FIQ_INTFORCE_POS, ICTL_FIQ_INTFORCE_RSVD_FIQ_INTFORCE_LEN)

#define ICTL1_FIQ_INTFORCE_RSVD_FIQ_INTFORCE_W(value) SetGroupBits32( (ICTL1_FIQ_INTFORCE_VAL),ICTL_FIQ_INTFORCE_RSVD_FIQ_INTFORCE_POS, ICTL_FIQ_INTFORCE_RSVD_FIQ_INTFORCE_LEN,value)


/* REGISTER: FIQ_RAWSTATUS */

#if defined(_V1) && !defined(FIQ_RAWSTATUS_OFFSET)
#define FIQ_RAWSTATUS_OFFSET 0xcc
#endif

#if !defined(ICTL_FIQ_RAWSTATUS_OFFSET)
#define ICTL_FIQ_RAWSTATUS_OFFSET 0xcc
#endif

#if defined(_V1) && !defined(FIQ_RAWSTATUS_REG)
#define FIQ_RAWSTATUS_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_FIQ_RAWSTATUS_OFFSET))
#endif

#if defined(_V1) && !defined(FIQ_RAWSTATUS_VAL)
#define FIQ_RAWSTATUS_VAL  PREFIX_VAL(FIQ_RAWSTATUS_REG)
#endif

#define ICTL1_FIQ_RAWSTATUS_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_FIQ_RAWSTATUS_OFFSET))
#define ICTL1_FIQ_RAWSTATUS_VAL  PREFIX_VAL(ICTL1_FIQ_RAWSTATUS_REG)

/* FIELDS: */

/* FIQ_RAWSTATUS */

#ifndef ICTL_FIQ_RAWSTATUS_FIQ_RAWSTATUS_POS
#define ICTL_FIQ_RAWSTATUS_FIQ_RAWSTATUS_POS      0
#endif

#ifndef ICTL_FIQ_RAWSTATUS_FIQ_RAWSTATUS_LEN
#define ICTL_FIQ_RAWSTATUS_FIQ_RAWSTATUS_LEN      8
#endif

#if defined(_V1) && !defined(FIQ_RAWSTATUS_FIQ_RAWSTATUS_R)
#define FIQ_RAWSTATUS_FIQ_RAWSTATUS_R        GetGroupBits32( (ICTL1_FIQ_RAWSTATUS_VAL),ICTL_FIQ_RAWSTATUS_FIQ_RAWSTATUS_POS, ICTL_FIQ_RAWSTATUS_FIQ_RAWSTATUS_LEN)
#endif

#if defined(_V1) && !defined(FIQ_RAWSTATUS_FIQ_RAWSTATUS_W)
#define FIQ_RAWSTATUS_FIQ_RAWSTATUS_W(value) SetGroupBits32( (ICTL1_FIQ_RAWSTATUS_VAL),ICTL_FIQ_RAWSTATUS_FIQ_RAWSTATUS_POS, ICTL_FIQ_RAWSTATUS_FIQ_RAWSTATUS_LEN,value)
#endif

#define ICTL1_FIQ_RAWSTATUS_FIQ_RAWSTATUS_R        GetGroupBits32( (ICTL1_FIQ_RAWSTATUS_VAL),ICTL_FIQ_RAWSTATUS_FIQ_RAWSTATUS_POS, ICTL_FIQ_RAWSTATUS_FIQ_RAWSTATUS_LEN)

#define ICTL1_FIQ_RAWSTATUS_FIQ_RAWSTATUS_W(value) SetGroupBits32( (ICTL1_FIQ_RAWSTATUS_VAL),ICTL_FIQ_RAWSTATUS_FIQ_RAWSTATUS_POS, ICTL_FIQ_RAWSTATUS_FIQ_RAWSTATUS_LEN,value)


/* RSVD_FIQ_RAWSTATUS */

#ifndef ICTL_FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_POS
#define ICTL_FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_POS      8
#endif

#ifndef ICTL_FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_LEN
#define ICTL_FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_LEN      24
#endif

#if defined(_V1) && !defined(FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_R)
#define FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_R        GetGroupBits32( (ICTL1_FIQ_RAWSTATUS_VAL),ICTL_FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_POS, ICTL_FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_LEN)
#endif

#if defined(_V1) && !defined(FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_W)
#define FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_W(value) SetGroupBits32( (ICTL1_FIQ_RAWSTATUS_VAL),ICTL_FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_POS, ICTL_FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_LEN,value)
#endif

#define ICTL1_FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_R        GetGroupBits32( (ICTL1_FIQ_RAWSTATUS_VAL),ICTL_FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_POS, ICTL_FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_LEN)

#define ICTL1_FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_W(value) SetGroupBits32( (ICTL1_FIQ_RAWSTATUS_VAL),ICTL_FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_POS, ICTL_FIQ_RAWSTATUS_RSVD_FIQ_RAWSTATUS_LEN,value)


/* REGISTER: FIQ_STATUS */

#if defined(_V1) && !defined(FIQ_STATUS_OFFSET)
#define FIQ_STATUS_OFFSET 0xd0
#endif

#if !defined(ICTL_FIQ_STATUS_OFFSET)
#define ICTL_FIQ_STATUS_OFFSET 0xd0
#endif

#if defined(_V1) && !defined(FIQ_STATUS_REG)
#define FIQ_STATUS_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_FIQ_STATUS_OFFSET))
#endif

#if defined(_V1) && !defined(FIQ_STATUS_VAL)
#define FIQ_STATUS_VAL  PREFIX_VAL(FIQ_STATUS_REG)
#endif

#define ICTL1_FIQ_STATUS_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_FIQ_STATUS_OFFSET))
#define ICTL1_FIQ_STATUS_VAL  PREFIX_VAL(ICTL1_FIQ_STATUS_REG)

/* FIELDS: */

/* FIQ_STATUS */

#ifndef ICTL_FIQ_STATUS_FIQ_STATUS_POS
#define ICTL_FIQ_STATUS_FIQ_STATUS_POS      0
#endif

#ifndef ICTL_FIQ_STATUS_FIQ_STATUS_LEN
#define ICTL_FIQ_STATUS_FIQ_STATUS_LEN      8
#endif

#if defined(_V1) && !defined(FIQ_STATUS_FIQ_STATUS_R)
#define FIQ_STATUS_FIQ_STATUS_R        GetGroupBits32( (ICTL1_FIQ_STATUS_VAL),ICTL_FIQ_STATUS_FIQ_STATUS_POS, ICTL_FIQ_STATUS_FIQ_STATUS_LEN)
#endif

#if defined(_V1) && !defined(FIQ_STATUS_FIQ_STATUS_W)
#define FIQ_STATUS_FIQ_STATUS_W(value) SetGroupBits32( (ICTL1_FIQ_STATUS_VAL),ICTL_FIQ_STATUS_FIQ_STATUS_POS, ICTL_FIQ_STATUS_FIQ_STATUS_LEN,value)
#endif

#define ICTL1_FIQ_STATUS_FIQ_STATUS_R        GetGroupBits32( (ICTL1_FIQ_STATUS_VAL),ICTL_FIQ_STATUS_FIQ_STATUS_POS, ICTL_FIQ_STATUS_FIQ_STATUS_LEN)

#define ICTL1_FIQ_STATUS_FIQ_STATUS_W(value) SetGroupBits32( (ICTL1_FIQ_STATUS_VAL),ICTL_FIQ_STATUS_FIQ_STATUS_POS, ICTL_FIQ_STATUS_FIQ_STATUS_LEN,value)


/* RSVD_FIQ_STATUS */

#ifndef ICTL_FIQ_STATUS_RSVD_FIQ_STATUS_POS
#define ICTL_FIQ_STATUS_RSVD_FIQ_STATUS_POS      8
#endif

#ifndef ICTL_FIQ_STATUS_RSVD_FIQ_STATUS_LEN
#define ICTL_FIQ_STATUS_RSVD_FIQ_STATUS_LEN      24
#endif

#if defined(_V1) && !defined(FIQ_STATUS_RSVD_FIQ_STATUS_R)
#define FIQ_STATUS_RSVD_FIQ_STATUS_R        GetGroupBits32( (ICTL1_FIQ_STATUS_VAL),ICTL_FIQ_STATUS_RSVD_FIQ_STATUS_POS, ICTL_FIQ_STATUS_RSVD_FIQ_STATUS_LEN)
#endif

#if defined(_V1) && !defined(FIQ_STATUS_RSVD_FIQ_STATUS_W)
#define FIQ_STATUS_RSVD_FIQ_STATUS_W(value) SetGroupBits32( (ICTL1_FIQ_STATUS_VAL),ICTL_FIQ_STATUS_RSVD_FIQ_STATUS_POS, ICTL_FIQ_STATUS_RSVD_FIQ_STATUS_LEN,value)
#endif

#define ICTL1_FIQ_STATUS_RSVD_FIQ_STATUS_R        GetGroupBits32( (ICTL1_FIQ_STATUS_VAL),ICTL_FIQ_STATUS_RSVD_FIQ_STATUS_POS, ICTL_FIQ_STATUS_RSVD_FIQ_STATUS_LEN)

#define ICTL1_FIQ_STATUS_RSVD_FIQ_STATUS_W(value) SetGroupBits32( (ICTL1_FIQ_STATUS_VAL),ICTL_FIQ_STATUS_RSVD_FIQ_STATUS_POS, ICTL_FIQ_STATUS_RSVD_FIQ_STATUS_LEN,value)


/* REGISTER: FIQ_FINALSTATUS */

#if defined(_V1) && !defined(FIQ_FINALSTATUS_OFFSET)
#define FIQ_FINALSTATUS_OFFSET 0xd4
#endif

#if !defined(ICTL_FIQ_FINALSTATUS_OFFSET)
#define ICTL_FIQ_FINALSTATUS_OFFSET 0xd4
#endif

#if defined(_V1) && !defined(FIQ_FINALSTATUS_REG)
#define FIQ_FINALSTATUS_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_FIQ_FINALSTATUS_OFFSET))
#endif

#if defined(_V1) && !defined(FIQ_FINALSTATUS_VAL)
#define FIQ_FINALSTATUS_VAL  PREFIX_VAL(FIQ_FINALSTATUS_REG)
#endif

#define ICTL1_FIQ_FINALSTATUS_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_FIQ_FINALSTATUS_OFFSET))
#define ICTL1_FIQ_FINALSTATUS_VAL  PREFIX_VAL(ICTL1_FIQ_FINALSTATUS_REG)

/* FIELDS: */

/* FIQ_FINALSTATUS */

#ifndef ICTL_FIQ_FINALSTATUS_FIQ_FINALSTATUS_POS
#define ICTL_FIQ_FINALSTATUS_FIQ_FINALSTATUS_POS      0
#endif

#ifndef ICTL_FIQ_FINALSTATUS_FIQ_FINALSTATUS_LEN
#define ICTL_FIQ_FINALSTATUS_FIQ_FINALSTATUS_LEN      8
#endif

#if defined(_V1) && !defined(FIQ_FINALSTATUS_FIQ_FINALSTATUS_R)
#define FIQ_FINALSTATUS_FIQ_FINALSTATUS_R        GetGroupBits32( (ICTL1_FIQ_FINALSTATUS_VAL),ICTL_FIQ_FINALSTATUS_FIQ_FINALSTATUS_POS, ICTL_FIQ_FINALSTATUS_FIQ_FINALSTATUS_LEN)
#endif

#if defined(_V1) && !defined(FIQ_FINALSTATUS_FIQ_FINALSTATUS_W)
#define FIQ_FINALSTATUS_FIQ_FINALSTATUS_W(value) SetGroupBits32( (ICTL1_FIQ_FINALSTATUS_VAL),ICTL_FIQ_FINALSTATUS_FIQ_FINALSTATUS_POS, ICTL_FIQ_FINALSTATUS_FIQ_FINALSTATUS_LEN,value)
#endif

#define ICTL1_FIQ_FINALSTATUS_FIQ_FINALSTATUS_R        GetGroupBits32( (ICTL1_FIQ_FINALSTATUS_VAL),ICTL_FIQ_FINALSTATUS_FIQ_FINALSTATUS_POS, ICTL_FIQ_FINALSTATUS_FIQ_FINALSTATUS_LEN)

#define ICTL1_FIQ_FINALSTATUS_FIQ_FINALSTATUS_W(value) SetGroupBits32( (ICTL1_FIQ_FINALSTATUS_VAL),ICTL_FIQ_FINALSTATUS_FIQ_FINALSTATUS_POS, ICTL_FIQ_FINALSTATUS_FIQ_FINALSTATUS_LEN,value)


/* RSVD_FIQ_FINALSTATUS */

#ifndef ICTL_FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_POS
#define ICTL_FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_POS      8
#endif

#ifndef ICTL_FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_LEN
#define ICTL_FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_LEN      24
#endif

#if defined(_V1) && !defined(FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_R)
#define FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_R        GetGroupBits32( (ICTL1_FIQ_FINALSTATUS_VAL),ICTL_FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_POS, ICTL_FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_LEN)
#endif

#if defined(_V1) && !defined(FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_W)
#define FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_W(value) SetGroupBits32( (ICTL1_FIQ_FINALSTATUS_VAL),ICTL_FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_POS, ICTL_FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_LEN,value)
#endif

#define ICTL1_FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_R        GetGroupBits32( (ICTL1_FIQ_FINALSTATUS_VAL),ICTL_FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_POS, ICTL_FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_LEN)

#define ICTL1_FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_W(value) SetGroupBits32( (ICTL1_FIQ_FINALSTATUS_VAL),ICTL_FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_POS, ICTL_FIQ_FINALSTATUS_RSVD_FIQ_FINALSTATUS_LEN,value)


/* REGISTER: IRQ_PLEVEL */

#if defined(_V1) && !defined(IRQ_PLEVEL_OFFSET)
#define IRQ_PLEVEL_OFFSET 0xd8
#endif

#if !defined(ICTL_IRQ_PLEVEL_OFFSET)
#define ICTL_IRQ_PLEVEL_OFFSET 0xd8
#endif

#if defined(_V1) && !defined(IRQ_PLEVEL_REG)
#define IRQ_PLEVEL_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PLEVEL_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PLEVEL_VAL)
#define IRQ_PLEVEL_VAL  PREFIX_VAL(IRQ_PLEVEL_REG)
#endif

#define ICTL1_IRQ_PLEVEL_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PLEVEL_OFFSET))
#define ICTL1_IRQ_PLEVEL_VAL  PREFIX_VAL(ICTL1_IRQ_PLEVEL_REG)

/* FIELDS: */

/* IRQ_PLEVEL */

#ifndef ICTL_IRQ_PLEVEL_IRQ_PLEVEL_POS
#define ICTL_IRQ_PLEVEL_IRQ_PLEVEL_POS      0
#endif

#ifndef ICTL_IRQ_PLEVEL_IRQ_PLEVEL_LEN
#define ICTL_IRQ_PLEVEL_IRQ_PLEVEL_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PLEVEL_IRQ_PLEVEL_R)
#define IRQ_PLEVEL_IRQ_PLEVEL_R        GetGroupBits32( (ICTL1_IRQ_PLEVEL_VAL),ICTL_IRQ_PLEVEL_IRQ_PLEVEL_POS, ICTL_IRQ_PLEVEL_IRQ_PLEVEL_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PLEVEL_IRQ_PLEVEL_W)
#define IRQ_PLEVEL_IRQ_PLEVEL_W(value) SetGroupBits32( (ICTL1_IRQ_PLEVEL_VAL),ICTL_IRQ_PLEVEL_IRQ_PLEVEL_POS, ICTL_IRQ_PLEVEL_IRQ_PLEVEL_LEN,value)
#endif

#define ICTL1_IRQ_PLEVEL_IRQ_PLEVEL_R        GetGroupBits32( (ICTL1_IRQ_PLEVEL_VAL),ICTL_IRQ_PLEVEL_IRQ_PLEVEL_POS, ICTL_IRQ_PLEVEL_IRQ_PLEVEL_LEN)

#define ICTL1_IRQ_PLEVEL_IRQ_PLEVEL_W(value) SetGroupBits32( (ICTL1_IRQ_PLEVEL_VAL),ICTL_IRQ_PLEVEL_IRQ_PLEVEL_POS, ICTL_IRQ_PLEVEL_IRQ_PLEVEL_LEN,value)


/* RSVD_IRQ_PLEVEL */

#ifndef ICTL_IRQ_PLEVEL_RSVD_IRQ_PLEVEL_POS
#define ICTL_IRQ_PLEVEL_RSVD_IRQ_PLEVEL_POS      4
#endif

#ifndef ICTL_IRQ_PLEVEL_RSVD_IRQ_PLEVEL_LEN
#define ICTL_IRQ_PLEVEL_RSVD_IRQ_PLEVEL_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PLEVEL_RSVD_IRQ_PLEVEL_R)
#define IRQ_PLEVEL_RSVD_IRQ_PLEVEL_R        GetGroupBits32( (ICTL1_IRQ_PLEVEL_VAL),ICTL_IRQ_PLEVEL_RSVD_IRQ_PLEVEL_POS, ICTL_IRQ_PLEVEL_RSVD_IRQ_PLEVEL_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PLEVEL_RSVD_IRQ_PLEVEL_W)
#define IRQ_PLEVEL_RSVD_IRQ_PLEVEL_W(value) SetGroupBits32( (ICTL1_IRQ_PLEVEL_VAL),ICTL_IRQ_PLEVEL_RSVD_IRQ_PLEVEL_POS, ICTL_IRQ_PLEVEL_RSVD_IRQ_PLEVEL_LEN,value)
#endif

#define ICTL1_IRQ_PLEVEL_RSVD_IRQ_PLEVEL_R        GetGroupBits32( (ICTL1_IRQ_PLEVEL_VAL),ICTL_IRQ_PLEVEL_RSVD_IRQ_PLEVEL_POS, ICTL_IRQ_PLEVEL_RSVD_IRQ_PLEVEL_LEN)

#define ICTL1_IRQ_PLEVEL_RSVD_IRQ_PLEVEL_W(value) SetGroupBits32( (ICTL1_IRQ_PLEVEL_VAL),ICTL_IRQ_PLEVEL_RSVD_IRQ_PLEVEL_POS, ICTL_IRQ_PLEVEL_RSVD_IRQ_PLEVEL_LEN,value)


/* REGISTER: ICTL_VERSION_ID */

#if defined(_V1) && !defined(ICTL_VERSION_ID_OFFSET)
#define ICTL_VERSION_ID_OFFSET 0xe0
#endif

#if !defined(ICTL_ICTL_VERSION_ID_OFFSET)
#define ICTL_ICTL_VERSION_ID_OFFSET 0xe0
#endif

#if defined(_V1) && !defined(ICTL_VERSION_ID_REG)
#define ICTL_VERSION_ID_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_ICTL_VERSION_ID_OFFSET))
#endif

#if defined(_V1) && !defined(ICTL_VERSION_ID_VAL)
#define ICTL_VERSION_ID_VAL  PREFIX_VAL(ICTL_VERSION_ID_REG)
#endif

#define ICTL1_ICTL_VERSION_ID_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_ICTL_VERSION_ID_OFFSET))
#define ICTL1_ICTL_VERSION_ID_VAL  PREFIX_VAL(ICTL1_ICTL_VERSION_ID_REG)

/* FIELDS: */

/* ICTL_VERSION_ID */

#ifndef ICTL_ICTL_VERSION_ID_ICTL_VERSION_ID_POS
#define ICTL_ICTL_VERSION_ID_ICTL_VERSION_ID_POS      0
#endif

#ifndef ICTL_ICTL_VERSION_ID_ICTL_VERSION_ID_LEN
#define ICTL_ICTL_VERSION_ID_ICTL_VERSION_ID_LEN      32
#endif

#if defined(_V1) && !defined(ICTL_VERSION_ID_ICTL_VERSION_ID_R)
#define ICTL_VERSION_ID_ICTL_VERSION_ID_R        GetGroupBits32( (ICTL1_ICTL_VERSION_ID_VAL),ICTL_ICTL_VERSION_ID_ICTL_VERSION_ID_POS, ICTL_ICTL_VERSION_ID_ICTL_VERSION_ID_LEN)
#endif

#if defined(_V1) && !defined(ICTL_VERSION_ID_ICTL_VERSION_ID_W)
#define ICTL_VERSION_ID_ICTL_VERSION_ID_W(value) SetGroupBits32( (ICTL1_ICTL_VERSION_ID_VAL),ICTL_ICTL_VERSION_ID_ICTL_VERSION_ID_POS, ICTL_ICTL_VERSION_ID_ICTL_VERSION_ID_LEN,value)
#endif

#define ICTL1_ICTL_VERSION_ID_ICTL_VERSION_ID_R        GetGroupBits32( (ICTL1_ICTL_VERSION_ID_VAL),ICTL_ICTL_VERSION_ID_ICTL_VERSION_ID_POS, ICTL_ICTL_VERSION_ID_ICTL_VERSION_ID_LEN)

#define ICTL1_ICTL_VERSION_ID_ICTL_VERSION_ID_W(value) SetGroupBits32( (ICTL1_ICTL_VERSION_ID_VAL),ICTL_ICTL_VERSION_ID_ICTL_VERSION_ID_POS, ICTL_ICTL_VERSION_ID_ICTL_VERSION_ID_LEN,value)


/* REGISTER: IRQ_PR_0 */

#if defined(_V1) && !defined(IRQ_PR_0_OFFSET)
#define IRQ_PR_0_OFFSET 0xe8
#endif

#if !defined(ICTL_IRQ_PR_0_OFFSET)
#define ICTL_IRQ_PR_0_OFFSET 0xe8
#endif

#if defined(_V1) && !defined(IRQ_PR_0_REG)
#define IRQ_PR_0_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_0_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_0_VAL)
#define IRQ_PR_0_VAL  PREFIX_VAL(IRQ_PR_0_REG)
#endif

#define ICTL1_IRQ_PR_0_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_0_OFFSET))
#define ICTL1_IRQ_PR_0_VAL  PREFIX_VAL(ICTL1_IRQ_PR_0_REG)

/* FIELDS: */

/* irq_pr_0 */

#ifndef ICTL_IRQ_PR_0_IRQ_PR_0_POS
#define ICTL_IRQ_PR_0_IRQ_PR_0_POS      0
#endif

#ifndef ICTL_IRQ_PR_0_IRQ_PR_0_LEN
#define ICTL_IRQ_PR_0_IRQ_PR_0_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_0_IRQ_PR_0_R)
#define IRQ_PR_0_IRQ_PR_0_R        GetGroupBits32( (ICTL1_IRQ_PR_0_VAL),ICTL_IRQ_PR_0_IRQ_PR_0_POS, ICTL_IRQ_PR_0_IRQ_PR_0_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_0_IRQ_PR_0_W)
#define IRQ_PR_0_IRQ_PR_0_W(value) SetGroupBits32( (ICTL1_IRQ_PR_0_VAL),ICTL_IRQ_PR_0_IRQ_PR_0_POS, ICTL_IRQ_PR_0_IRQ_PR_0_LEN,value)
#endif

#define ICTL1_IRQ_PR_0_IRQ_PR_0_R        GetGroupBits32( (ICTL1_IRQ_PR_0_VAL),ICTL_IRQ_PR_0_IRQ_PR_0_POS, ICTL_IRQ_PR_0_IRQ_PR_0_LEN)

#define ICTL1_IRQ_PR_0_IRQ_PR_0_W(value) SetGroupBits32( (ICTL1_IRQ_PR_0_VAL),ICTL_IRQ_PR_0_IRQ_PR_0_POS, ICTL_IRQ_PR_0_IRQ_PR_0_LEN,value)


/* RSVD_irq_pr_0 */

#ifndef ICTL_IRQ_PR_0_RSVD_IRQ_PR_0_POS
#define ICTL_IRQ_PR_0_RSVD_IRQ_PR_0_POS      4
#endif

#ifndef ICTL_IRQ_PR_0_RSVD_IRQ_PR_0_LEN
#define ICTL_IRQ_PR_0_RSVD_IRQ_PR_0_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_0_RSVD_IRQ_PR_0_R)
#define IRQ_PR_0_RSVD_IRQ_PR_0_R        GetGroupBits32( (ICTL1_IRQ_PR_0_VAL),ICTL_IRQ_PR_0_RSVD_IRQ_PR_0_POS, ICTL_IRQ_PR_0_RSVD_IRQ_PR_0_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_0_RSVD_IRQ_PR_0_W)
#define IRQ_PR_0_RSVD_IRQ_PR_0_W(value) SetGroupBits32( (ICTL1_IRQ_PR_0_VAL),ICTL_IRQ_PR_0_RSVD_IRQ_PR_0_POS, ICTL_IRQ_PR_0_RSVD_IRQ_PR_0_LEN,value)
#endif

#define ICTL1_IRQ_PR_0_RSVD_IRQ_PR_0_R        GetGroupBits32( (ICTL1_IRQ_PR_0_VAL),ICTL_IRQ_PR_0_RSVD_IRQ_PR_0_POS, ICTL_IRQ_PR_0_RSVD_IRQ_PR_0_LEN)

#define ICTL1_IRQ_PR_0_RSVD_IRQ_PR_0_W(value) SetGroupBits32( (ICTL1_IRQ_PR_0_VAL),ICTL_IRQ_PR_0_RSVD_IRQ_PR_0_POS, ICTL_IRQ_PR_0_RSVD_IRQ_PR_0_LEN,value)


/* REGISTER: IRQ_PR_1 */

#if defined(_V1) && !defined(IRQ_PR_1_OFFSET)
#define IRQ_PR_1_OFFSET 0xec
#endif

#if !defined(ICTL_IRQ_PR_1_OFFSET)
#define ICTL_IRQ_PR_1_OFFSET 0xec
#endif

#if defined(_V1) && !defined(IRQ_PR_1_REG)
#define IRQ_PR_1_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_1_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_1_VAL)
#define IRQ_PR_1_VAL  PREFIX_VAL(IRQ_PR_1_REG)
#endif

#define ICTL1_IRQ_PR_1_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_1_OFFSET))
#define ICTL1_IRQ_PR_1_VAL  PREFIX_VAL(ICTL1_IRQ_PR_1_REG)

/* FIELDS: */

/* irq_pr_1 */

#ifndef ICTL_IRQ_PR_1_IRQ_PR_1_POS
#define ICTL_IRQ_PR_1_IRQ_PR_1_POS      0
#endif

#ifndef ICTL_IRQ_PR_1_IRQ_PR_1_LEN
#define ICTL_IRQ_PR_1_IRQ_PR_1_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_1_IRQ_PR_1_R)
#define IRQ_PR_1_IRQ_PR_1_R        GetGroupBits32( (ICTL1_IRQ_PR_1_VAL),ICTL_IRQ_PR_1_IRQ_PR_1_POS, ICTL_IRQ_PR_1_IRQ_PR_1_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_1_IRQ_PR_1_W)
#define IRQ_PR_1_IRQ_PR_1_W(value) SetGroupBits32( (ICTL1_IRQ_PR_1_VAL),ICTL_IRQ_PR_1_IRQ_PR_1_POS, ICTL_IRQ_PR_1_IRQ_PR_1_LEN,value)
#endif

#define ICTL1_IRQ_PR_1_IRQ_PR_1_R        GetGroupBits32( (ICTL1_IRQ_PR_1_VAL),ICTL_IRQ_PR_1_IRQ_PR_1_POS, ICTL_IRQ_PR_1_IRQ_PR_1_LEN)

#define ICTL1_IRQ_PR_1_IRQ_PR_1_W(value) SetGroupBits32( (ICTL1_IRQ_PR_1_VAL),ICTL_IRQ_PR_1_IRQ_PR_1_POS, ICTL_IRQ_PR_1_IRQ_PR_1_LEN,value)


/* RSVD_irq_pr_1 */

#ifndef ICTL_IRQ_PR_1_RSVD_IRQ_PR_1_POS
#define ICTL_IRQ_PR_1_RSVD_IRQ_PR_1_POS      4
#endif

#ifndef ICTL_IRQ_PR_1_RSVD_IRQ_PR_1_LEN
#define ICTL_IRQ_PR_1_RSVD_IRQ_PR_1_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_1_RSVD_IRQ_PR_1_R)
#define IRQ_PR_1_RSVD_IRQ_PR_1_R        GetGroupBits32( (ICTL1_IRQ_PR_1_VAL),ICTL_IRQ_PR_1_RSVD_IRQ_PR_1_POS, ICTL_IRQ_PR_1_RSVD_IRQ_PR_1_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_1_RSVD_IRQ_PR_1_W)
#define IRQ_PR_1_RSVD_IRQ_PR_1_W(value) SetGroupBits32( (ICTL1_IRQ_PR_1_VAL),ICTL_IRQ_PR_1_RSVD_IRQ_PR_1_POS, ICTL_IRQ_PR_1_RSVD_IRQ_PR_1_LEN,value)
#endif

#define ICTL1_IRQ_PR_1_RSVD_IRQ_PR_1_R        GetGroupBits32( (ICTL1_IRQ_PR_1_VAL),ICTL_IRQ_PR_1_RSVD_IRQ_PR_1_POS, ICTL_IRQ_PR_1_RSVD_IRQ_PR_1_LEN)

#define ICTL1_IRQ_PR_1_RSVD_IRQ_PR_1_W(value) SetGroupBits32( (ICTL1_IRQ_PR_1_VAL),ICTL_IRQ_PR_1_RSVD_IRQ_PR_1_POS, ICTL_IRQ_PR_1_RSVD_IRQ_PR_1_LEN,value)


/* REGISTER: IRQ_PR_2 */

#if defined(_V1) && !defined(IRQ_PR_2_OFFSET)
#define IRQ_PR_2_OFFSET 0xf0
#endif

#if !defined(ICTL_IRQ_PR_2_OFFSET)
#define ICTL_IRQ_PR_2_OFFSET 0xf0
#endif

#if defined(_V1) && !defined(IRQ_PR_2_REG)
#define IRQ_PR_2_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_2_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_2_VAL)
#define IRQ_PR_2_VAL  PREFIX_VAL(IRQ_PR_2_REG)
#endif

#define ICTL1_IRQ_PR_2_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_2_OFFSET))
#define ICTL1_IRQ_PR_2_VAL  PREFIX_VAL(ICTL1_IRQ_PR_2_REG)

/* FIELDS: */

/* irq_pr_2 */

#ifndef ICTL_IRQ_PR_2_IRQ_PR_2_POS
#define ICTL_IRQ_PR_2_IRQ_PR_2_POS      0
#endif

#ifndef ICTL_IRQ_PR_2_IRQ_PR_2_LEN
#define ICTL_IRQ_PR_2_IRQ_PR_2_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_2_IRQ_PR_2_R)
#define IRQ_PR_2_IRQ_PR_2_R        GetGroupBits32( (ICTL1_IRQ_PR_2_VAL),ICTL_IRQ_PR_2_IRQ_PR_2_POS, ICTL_IRQ_PR_2_IRQ_PR_2_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_2_IRQ_PR_2_W)
#define IRQ_PR_2_IRQ_PR_2_W(value) SetGroupBits32( (ICTL1_IRQ_PR_2_VAL),ICTL_IRQ_PR_2_IRQ_PR_2_POS, ICTL_IRQ_PR_2_IRQ_PR_2_LEN,value)
#endif

#define ICTL1_IRQ_PR_2_IRQ_PR_2_R        GetGroupBits32( (ICTL1_IRQ_PR_2_VAL),ICTL_IRQ_PR_2_IRQ_PR_2_POS, ICTL_IRQ_PR_2_IRQ_PR_2_LEN)

#define ICTL1_IRQ_PR_2_IRQ_PR_2_W(value) SetGroupBits32( (ICTL1_IRQ_PR_2_VAL),ICTL_IRQ_PR_2_IRQ_PR_2_POS, ICTL_IRQ_PR_2_IRQ_PR_2_LEN,value)


/* RSVD_irq_pr_2 */

#ifndef ICTL_IRQ_PR_2_RSVD_IRQ_PR_2_POS
#define ICTL_IRQ_PR_2_RSVD_IRQ_PR_2_POS      4
#endif

#ifndef ICTL_IRQ_PR_2_RSVD_IRQ_PR_2_LEN
#define ICTL_IRQ_PR_2_RSVD_IRQ_PR_2_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_2_RSVD_IRQ_PR_2_R)
#define IRQ_PR_2_RSVD_IRQ_PR_2_R        GetGroupBits32( (ICTL1_IRQ_PR_2_VAL),ICTL_IRQ_PR_2_RSVD_IRQ_PR_2_POS, ICTL_IRQ_PR_2_RSVD_IRQ_PR_2_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_2_RSVD_IRQ_PR_2_W)
#define IRQ_PR_2_RSVD_IRQ_PR_2_W(value) SetGroupBits32( (ICTL1_IRQ_PR_2_VAL),ICTL_IRQ_PR_2_RSVD_IRQ_PR_2_POS, ICTL_IRQ_PR_2_RSVD_IRQ_PR_2_LEN,value)
#endif

#define ICTL1_IRQ_PR_2_RSVD_IRQ_PR_2_R        GetGroupBits32( (ICTL1_IRQ_PR_2_VAL),ICTL_IRQ_PR_2_RSVD_IRQ_PR_2_POS, ICTL_IRQ_PR_2_RSVD_IRQ_PR_2_LEN)

#define ICTL1_IRQ_PR_2_RSVD_IRQ_PR_2_W(value) SetGroupBits32( (ICTL1_IRQ_PR_2_VAL),ICTL_IRQ_PR_2_RSVD_IRQ_PR_2_POS, ICTL_IRQ_PR_2_RSVD_IRQ_PR_2_LEN,value)


/* REGISTER: IRQ_PR_3 */

#if defined(_V1) && !defined(IRQ_PR_3_OFFSET)
#define IRQ_PR_3_OFFSET 0xf4
#endif

#if !defined(ICTL_IRQ_PR_3_OFFSET)
#define ICTL_IRQ_PR_3_OFFSET 0xf4
#endif

#if defined(_V1) && !defined(IRQ_PR_3_REG)
#define IRQ_PR_3_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_3_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_3_VAL)
#define IRQ_PR_3_VAL  PREFIX_VAL(IRQ_PR_3_REG)
#endif

#define ICTL1_IRQ_PR_3_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_3_OFFSET))
#define ICTL1_IRQ_PR_3_VAL  PREFIX_VAL(ICTL1_IRQ_PR_3_REG)

/* FIELDS: */

/* irq_pr_3 */

#ifndef ICTL_IRQ_PR_3_IRQ_PR_3_POS
#define ICTL_IRQ_PR_3_IRQ_PR_3_POS      0
#endif

#ifndef ICTL_IRQ_PR_3_IRQ_PR_3_LEN
#define ICTL_IRQ_PR_3_IRQ_PR_3_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_3_IRQ_PR_3_R)
#define IRQ_PR_3_IRQ_PR_3_R        GetGroupBits32( (ICTL1_IRQ_PR_3_VAL),ICTL_IRQ_PR_3_IRQ_PR_3_POS, ICTL_IRQ_PR_3_IRQ_PR_3_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_3_IRQ_PR_3_W)
#define IRQ_PR_3_IRQ_PR_3_W(value) SetGroupBits32( (ICTL1_IRQ_PR_3_VAL),ICTL_IRQ_PR_3_IRQ_PR_3_POS, ICTL_IRQ_PR_3_IRQ_PR_3_LEN,value)
#endif

#define ICTL1_IRQ_PR_3_IRQ_PR_3_R        GetGroupBits32( (ICTL1_IRQ_PR_3_VAL),ICTL_IRQ_PR_3_IRQ_PR_3_POS, ICTL_IRQ_PR_3_IRQ_PR_3_LEN)

#define ICTL1_IRQ_PR_3_IRQ_PR_3_W(value) SetGroupBits32( (ICTL1_IRQ_PR_3_VAL),ICTL_IRQ_PR_3_IRQ_PR_3_POS, ICTL_IRQ_PR_3_IRQ_PR_3_LEN,value)


/* RSVD_irq_pr_3 */

#ifndef ICTL_IRQ_PR_3_RSVD_IRQ_PR_3_POS
#define ICTL_IRQ_PR_3_RSVD_IRQ_PR_3_POS      4
#endif

#ifndef ICTL_IRQ_PR_3_RSVD_IRQ_PR_3_LEN
#define ICTL_IRQ_PR_3_RSVD_IRQ_PR_3_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_3_RSVD_IRQ_PR_3_R)
#define IRQ_PR_3_RSVD_IRQ_PR_3_R        GetGroupBits32( (ICTL1_IRQ_PR_3_VAL),ICTL_IRQ_PR_3_RSVD_IRQ_PR_3_POS, ICTL_IRQ_PR_3_RSVD_IRQ_PR_3_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_3_RSVD_IRQ_PR_3_W)
#define IRQ_PR_3_RSVD_IRQ_PR_3_W(value) SetGroupBits32( (ICTL1_IRQ_PR_3_VAL),ICTL_IRQ_PR_3_RSVD_IRQ_PR_3_POS, ICTL_IRQ_PR_3_RSVD_IRQ_PR_3_LEN,value)
#endif

#define ICTL1_IRQ_PR_3_RSVD_IRQ_PR_3_R        GetGroupBits32( (ICTL1_IRQ_PR_3_VAL),ICTL_IRQ_PR_3_RSVD_IRQ_PR_3_POS, ICTL_IRQ_PR_3_RSVD_IRQ_PR_3_LEN)

#define ICTL1_IRQ_PR_3_RSVD_IRQ_PR_3_W(value) SetGroupBits32( (ICTL1_IRQ_PR_3_VAL),ICTL_IRQ_PR_3_RSVD_IRQ_PR_3_POS, ICTL_IRQ_PR_3_RSVD_IRQ_PR_3_LEN,value)


/* REGISTER: IRQ_PR_4 */

#if defined(_V1) && !defined(IRQ_PR_4_OFFSET)
#define IRQ_PR_4_OFFSET 0xf8
#endif

#if !defined(ICTL_IRQ_PR_4_OFFSET)
#define ICTL_IRQ_PR_4_OFFSET 0xf8
#endif

#if defined(_V1) && !defined(IRQ_PR_4_REG)
#define IRQ_PR_4_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_4_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_4_VAL)
#define IRQ_PR_4_VAL  PREFIX_VAL(IRQ_PR_4_REG)
#endif

#define ICTL1_IRQ_PR_4_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_4_OFFSET))
#define ICTL1_IRQ_PR_4_VAL  PREFIX_VAL(ICTL1_IRQ_PR_4_REG)

/* FIELDS: */

/* irq_pr_4 */

#ifndef ICTL_IRQ_PR_4_IRQ_PR_4_POS
#define ICTL_IRQ_PR_4_IRQ_PR_4_POS      0
#endif

#ifndef ICTL_IRQ_PR_4_IRQ_PR_4_LEN
#define ICTL_IRQ_PR_4_IRQ_PR_4_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_4_IRQ_PR_4_R)
#define IRQ_PR_4_IRQ_PR_4_R        GetGroupBits32( (ICTL1_IRQ_PR_4_VAL),ICTL_IRQ_PR_4_IRQ_PR_4_POS, ICTL_IRQ_PR_4_IRQ_PR_4_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_4_IRQ_PR_4_W)
#define IRQ_PR_4_IRQ_PR_4_W(value) SetGroupBits32( (ICTL1_IRQ_PR_4_VAL),ICTL_IRQ_PR_4_IRQ_PR_4_POS, ICTL_IRQ_PR_4_IRQ_PR_4_LEN,value)
#endif

#define ICTL1_IRQ_PR_4_IRQ_PR_4_R        GetGroupBits32( (ICTL1_IRQ_PR_4_VAL),ICTL_IRQ_PR_4_IRQ_PR_4_POS, ICTL_IRQ_PR_4_IRQ_PR_4_LEN)

#define ICTL1_IRQ_PR_4_IRQ_PR_4_W(value) SetGroupBits32( (ICTL1_IRQ_PR_4_VAL),ICTL_IRQ_PR_4_IRQ_PR_4_POS, ICTL_IRQ_PR_4_IRQ_PR_4_LEN,value)


/* RSVD_irq_pr_4 */

#ifndef ICTL_IRQ_PR_4_RSVD_IRQ_PR_4_POS
#define ICTL_IRQ_PR_4_RSVD_IRQ_PR_4_POS      4
#endif

#ifndef ICTL_IRQ_PR_4_RSVD_IRQ_PR_4_LEN
#define ICTL_IRQ_PR_4_RSVD_IRQ_PR_4_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_4_RSVD_IRQ_PR_4_R)
#define IRQ_PR_4_RSVD_IRQ_PR_4_R        GetGroupBits32( (ICTL1_IRQ_PR_4_VAL),ICTL_IRQ_PR_4_RSVD_IRQ_PR_4_POS, ICTL_IRQ_PR_4_RSVD_IRQ_PR_4_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_4_RSVD_IRQ_PR_4_W)
#define IRQ_PR_4_RSVD_IRQ_PR_4_W(value) SetGroupBits32( (ICTL1_IRQ_PR_4_VAL),ICTL_IRQ_PR_4_RSVD_IRQ_PR_4_POS, ICTL_IRQ_PR_4_RSVD_IRQ_PR_4_LEN,value)
#endif

#define ICTL1_IRQ_PR_4_RSVD_IRQ_PR_4_R        GetGroupBits32( (ICTL1_IRQ_PR_4_VAL),ICTL_IRQ_PR_4_RSVD_IRQ_PR_4_POS, ICTL_IRQ_PR_4_RSVD_IRQ_PR_4_LEN)

#define ICTL1_IRQ_PR_4_RSVD_IRQ_PR_4_W(value) SetGroupBits32( (ICTL1_IRQ_PR_4_VAL),ICTL_IRQ_PR_4_RSVD_IRQ_PR_4_POS, ICTL_IRQ_PR_4_RSVD_IRQ_PR_4_LEN,value)


/* REGISTER: IRQ_PR_5 */

#if defined(_V1) && !defined(IRQ_PR_5_OFFSET)
#define IRQ_PR_5_OFFSET 0xfc
#endif

#if !defined(ICTL_IRQ_PR_5_OFFSET)
#define ICTL_IRQ_PR_5_OFFSET 0xfc
#endif

#if defined(_V1) && !defined(IRQ_PR_5_REG)
#define IRQ_PR_5_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_5_OFFSET))
#endif

#if defined(_V1) && !defined(IRQ_PR_5_VAL)
#define IRQ_PR_5_VAL  PREFIX_VAL(IRQ_PR_5_REG)
#endif

#define ICTL1_IRQ_PR_5_REG ((volatile UINT32 *) (ICTL1_BASE + ICTL_IRQ_PR_5_OFFSET))
#define ICTL1_IRQ_PR_5_VAL  PREFIX_VAL(ICTL1_IRQ_PR_5_REG)

/* FIELDS: */

/* irq_pr_5 */

#ifndef ICTL_IRQ_PR_5_IRQ_PR_5_POS
#define ICTL_IRQ_PR_5_IRQ_PR_5_POS      0
#endif

#ifndef ICTL_IRQ_PR_5_IRQ_PR_5_LEN
#define ICTL_IRQ_PR_5_IRQ_PR_5_LEN      4
#endif

#if defined(_V1) && !defined(IRQ_PR_5_IRQ_PR_5_R)
#define IRQ_PR_5_IRQ_PR_5_R        GetGroupBits32( (ICTL1_IRQ_PR_5_VAL),ICTL_IRQ_PR_5_IRQ_PR_5_POS, ICTL_IRQ_PR_5_IRQ_PR_5_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_5_IRQ_PR_5_W)
#define IRQ_PR_5_IRQ_PR_5_W(value) SetGroupBits32( (ICTL1_IRQ_PR_5_VAL),ICTL_IRQ_PR_5_IRQ_PR_5_POS, ICTL_IRQ_PR_5_IRQ_PR_5_LEN,value)
#endif

#define ICTL1_IRQ_PR_5_IRQ_PR_5_R        GetGroupBits32( (ICTL1_IRQ_PR_5_VAL),ICTL_IRQ_PR_5_IRQ_PR_5_POS, ICTL_IRQ_PR_5_IRQ_PR_5_LEN)

#define ICTL1_IRQ_PR_5_IRQ_PR_5_W(value) SetGroupBits32( (ICTL1_IRQ_PR_5_VAL),ICTL_IRQ_PR_5_IRQ_PR_5_POS, ICTL_IRQ_PR_5_IRQ_PR_5_LEN,value)


/* RSVD_irq_pr_5 */

#ifndef ICTL_IRQ_PR_5_RSVD_IRQ_PR_5_POS
#define ICTL_IRQ_PR_5_RSVD_IRQ_PR_5_POS      4
#endif

#ifndef ICTL_IRQ_PR_5_RSVD_IRQ_PR_5_LEN
#define ICTL_IRQ_PR_5_RSVD_IRQ_PR_5_LEN      28
#endif

#if defined(_V1) && !defined(IRQ_PR_5_RSVD_IRQ_PR_5_R)
#define IRQ_PR_5_RSVD_IRQ_PR_5_R        GetGroupBits32( (ICTL1_IRQ_PR_5_VAL),ICTL_IRQ_PR_5_RSVD_IRQ_PR_5_POS, ICTL_IRQ_PR_5_RSVD_IRQ_PR_5_LEN)
#endif

#if defined(_V1) && !defined(IRQ_PR_5_RSVD_IRQ_PR_5_W)
#define IRQ_PR_5_RSVD_IRQ_PR_5_W(value) SetGroupBits32( (ICTL1_IRQ_PR_5_VAL),ICTL_IRQ_PR_5_RSVD_IRQ_PR_5_POS, ICTL_IRQ_PR_5_RSVD_IRQ_PR_5_LEN,value)
#endif

#define ICTL1_IRQ_PR_5_RSVD_IRQ_PR_5_R        GetGroupBits32( (ICTL1_IRQ_PR_5_VAL),ICTL_IRQ_PR_5_RSVD_IRQ_PR_5_POS, ICTL_IRQ_PR_5_RSVD_IRQ_PR_5_LEN)

#define ICTL1_IRQ_PR_5_RSVD_IRQ_PR_5_W(value) SetGroupBits32( (ICTL1_IRQ_PR_5_VAL),ICTL_IRQ_PR_5_RSVD_IRQ_PR_5_POS, ICTL_IRQ_PR_5_RSVD_IRQ_PR_5_LEN,value)


/* OFFSET TABLE: */
#define ictl1_offset_tbl_values	IRQ_INTEN_L_OFFSET, IRQ_INTFORCE_L_OFFSET, IRQ_PR_6_OFFSET, IRQ_PR_7_OFFSET, IRQ_PR_8_OFFSET, IRQ_PR_9_OFFSET, IRQ_PR_10_OFFSET, IRQ_PR_11_OFFSET, IRQ_PR_12_OFFSET, IRQ_PR_13_OFFSET, IRQ_PR_14_OFFSET, IRQ_PR_15_OFFSET, IRQ_PR_16_OFFSET, IRQ_PR_17_OFFSET, IRQ_PR_18_OFFSET, IRQ_PR_19_OFFSET, IRQ_PR_20_OFFSET, IRQ_PR_21_OFFSET, IRQ_INTFORCE_H_OFFSET, IRQ_PR_22_OFFSET, IRQ_PR_23_OFFSET, IRQ_PR_24_OFFSET, IRQ_PR_25_OFFSET, IRQ_PR_26_OFFSET, IRQ_PR_27_OFFSET, IRQ_PR_28_OFFSET, IRQ_PR_29_OFFSET, IRQ_PR_30_OFFSET, IRQ_PR_31_OFFSET, IRQ_PR_32_OFFSET, IRQ_PR_33_OFFSET, IRQ_PR_34_OFFSET, IRQ_PR_35_OFFSET, IRQ_PR_36_OFFSET, IRQ_PR_37_OFFSET, IRQ_RAWSTATUS_L_OFFSET, IRQ_PR_38_OFFSET, IRQ_PR_39_OFFSET, IRQ_PR_40_OFFSET, IRQ_PR_41_OFFSET, IRQ_PR_42_OFFSET, IRQ_PR_43_OFFSET, IRQ_PR_44_OFFSET, IRQ_PR_45_OFFSET, IRQ_PR_46_OFFSET, IRQ_PR_47_OFFSET, IRQ_PR_48_OFFSET, IRQ_PR_49_OFFSET, IRQ_PR_50_OFFSET, IRQ_PR_51_OFFSET, IRQ_PR_52_OFFSET, IRQ_PR_53_OFFSET, IRQ_RAWSTATUS_H_OFFSET, IRQ_PR_54_OFFSET, IRQ_PR_55_OFFSET, IRQ_PR_56_OFFSET, IRQ_PR_57_OFFSET, IRQ_PR_58_OFFSET, IRQ_PR_59_OFFSET, IRQ_PR_60_OFFSET, IRQ_PR_61_OFFSET, IRQ_PR_62_OFFSET, IRQ_PR_63_OFFSET, IRQ_VECTOR_DEFAULT_OFFSET, IRQ_STATUS_L_OFFSET, IRQ_STATUS_H_OFFSET, IRQ_MASKSTATUS_L_OFFSET, IRQ_MASKSTATUS_H_OFFSET, IRQ_FINALSTATUS_L_OFFSET, IRQ_FINALSTATUS_H_OFFSET, IRQ_VECTOR_OFFSET, IRQ_INTEN_H_OFFSET, IRQ_VECTOR_0_OFFSET, IRQ_VECTOR_1_OFFSET, IRQ_VECTOR_2_OFFSET, IRQ_VECTOR_3_OFFSET, IRQ_VECTOR_4_OFFSET, IRQ_VECTOR_5_OFFSET, IRQ_VECTOR_6_OFFSET, IRQ_VECTOR_7_OFFSET, IRQ_INTMASK_L_OFFSET, IRQ_VECTOR_8_OFFSET, IRQ_VECTOR_9_OFFSET, IRQ_VECTOR_10_OFFSET, IRQ_VECTOR_11_OFFSET, IRQ_VECTOR_12_OFFSET, IRQ_VECTOR_13_OFFSET, IRQ_VECTOR_14_OFFSET, IRQ_VECTOR_15_OFFSET, IRQ_INTMASK_H_OFFSET, FIQ_INTEN_OFFSET, FIQ_INTMASK_OFFSET, FIQ_INTFORCE_OFFSET, FIQ_RAWSTATUS_OFFSET, FIQ_STATUS_OFFSET, FIQ_FINALSTATUS_OFFSET, IRQ_PLEVEL_OFFSET, ICTL_VERSION_ID_OFFSET, IRQ_PR_0_OFFSET, IRQ_PR_1_OFFSET, IRQ_PR_2_OFFSET, IRQ_PR_3_OFFSET, IRQ_PR_4_OFFSET, IRQ_PR_5_OFFSET


/* REGISTERS RESET VAL: */
#define ictl1_regs_reset_val	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x4, 0x8, 0xC, 0x10, 0x14, 0x18, 0x1C, 0x0, 0x20, 0x24, 0x28, 0x2C, 0x30, 0x34, 0x38, 0x3C, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x3230372A, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0

#endif

/* End of ICTL1 */
/* ///////////////////////////////////////////////////////////////////////*/
