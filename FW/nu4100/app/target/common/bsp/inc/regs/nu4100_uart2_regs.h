#ifndef NU4100_UART2_REG_H
#define NU4100_UART2_REG_H


/* types */
#include "inu_types.h"

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
/*   UART2 (Prototype: UART)                */
/*****************************************/
#define UART2_BASE 0x081E0000

/* REGISTER: RBR ACCESS: RW */

#if defined(_V1) && !defined(RBR_OFFSET)
#define RBR_OFFSET 0x0
#endif

#if !defined(UART_RBR_OFFSET)
#define UART_RBR_OFFSET 0x0
#endif

#if defined(_V1) && !defined(RBR_REG)
#define RBR_REG ((volatile UINT32 *) (UART2_BASE + UART_RBR_OFFSET))
#endif

#if defined(_V1) && !defined(RBR_VAL)
#define RBR_VAL  PREFIX_VAL(RBR_REG)
#endif

#define UART2_RBR_REG ((volatile UINT32 *) (UART2_BASE + UART_RBR_OFFSET))
#define UART2_RBR_VAL  PREFIX_VAL(UART2_RBR_REG)

/* FIELDS: */

/* rbr ACCESS: RW */

#ifndef UART_RBR_RBR_POS
#define UART_RBR_RBR_POS      0
#endif

#ifndef UART_RBR_RBR_LEN
#define UART_RBR_RBR_LEN      8
#endif

#if defined(_V1) && !defined(RBR_RBR_R)
#define RBR_RBR_R        GetGroupBits32( (UART2_RBR_VAL),UART_RBR_RBR_POS, UART_RBR_RBR_LEN)
#endif

#if defined(_V1) && !defined(RBR_RBR_W)
#define RBR_RBR_W(value) SetGroupBits32( (UART2_RBR_VAL),UART_RBR_RBR_POS, UART_RBR_RBR_LEN,value)
#endif

#define UART2_RBR_RBR_R        GetGroupBits32( (UART2_RBR_VAL),UART_RBR_RBR_POS, UART_RBR_RBR_LEN)

#define UART2_RBR_RBR_W(value) SetGroupBits32( (UART2_RBR_VAL),UART_RBR_RBR_POS, UART_RBR_RBR_LEN,value)


/* RSVD_RBR ACCESS: RW */

#ifndef UART_RBR_RSVD_RBR_POS
#define UART_RBR_RSVD_RBR_POS      8
#endif

#ifndef UART_RBR_RSVD_RBR_LEN
#define UART_RBR_RSVD_RBR_LEN      24
#endif

#if defined(_V1) && !defined(RBR_RSVD_RBR_R)
#define RBR_RSVD_RBR_R        GetGroupBits32( (UART2_RBR_VAL),UART_RBR_RSVD_RBR_POS, UART_RBR_RSVD_RBR_LEN)
#endif

#if defined(_V1) && !defined(RBR_RSVD_RBR_W)
#define RBR_RSVD_RBR_W(value) SetGroupBits32( (UART2_RBR_VAL),UART_RBR_RSVD_RBR_POS, UART_RBR_RSVD_RBR_LEN,value)
#endif

#define UART2_RBR_RSVD_RBR_R        GetGroupBits32( (UART2_RBR_VAL),UART_RBR_RSVD_RBR_POS, UART_RBR_RSVD_RBR_LEN)

#define UART2_RBR_RSVD_RBR_W(value) SetGroupBits32( (UART2_RBR_VAL),UART_RBR_RSVD_RBR_POS, UART_RBR_RSVD_RBR_LEN,value)


/* REGISTER: MCR ACCESS: RW */

#if defined(_V1) && !defined(MCR_OFFSET)
#define MCR_OFFSET 0x10
#endif

#if !defined(UART_MCR_OFFSET)
#define UART_MCR_OFFSET 0x10
#endif

#if defined(_V1) && !defined(MCR_REG)
#define MCR_REG ((volatile UINT32 *) (UART2_BASE + UART_MCR_OFFSET))
#endif

#if defined(_V1) && !defined(MCR_VAL)
#define MCR_VAL  PREFIX_VAL(MCR_REG)
#endif

#define UART2_MCR_REG ((volatile UINT32 *) (UART2_BASE + UART_MCR_OFFSET))
#define UART2_MCR_VAL  PREFIX_VAL(UART2_MCR_REG)

/* FIELDS: */

/* DTR ACCESS: RW */

#ifndef UART_MCR_DTR_POS
#define UART_MCR_DTR_POS      0
#endif

#ifndef UART_MCR_DTR_LEN
#define UART_MCR_DTR_LEN      1
#endif

#if defined(_V1) && !defined(MCR_DTR_R)
#define MCR_DTR_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_DTR_POS, UART_MCR_DTR_LEN)
#endif

#if defined(_V1) && !defined(MCR_DTR_W)
#define MCR_DTR_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_DTR_POS, UART_MCR_DTR_LEN,value)
#endif

#define UART2_MCR_DTR_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_DTR_POS, UART_MCR_DTR_LEN)

#define UART2_MCR_DTR_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_DTR_POS, UART_MCR_DTR_LEN,value)


/* RTS ACCESS: RW */

#ifndef UART_MCR_RTS_POS
#define UART_MCR_RTS_POS      1
#endif

#ifndef UART_MCR_RTS_LEN
#define UART_MCR_RTS_LEN      1
#endif

#if defined(_V1) && !defined(MCR_RTS_R)
#define MCR_RTS_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_RTS_POS, UART_MCR_RTS_LEN)
#endif

#if defined(_V1) && !defined(MCR_RTS_W)
#define MCR_RTS_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_RTS_POS, UART_MCR_RTS_LEN,value)
#endif

#define UART2_MCR_RTS_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_RTS_POS, UART_MCR_RTS_LEN)

#define UART2_MCR_RTS_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_RTS_POS, UART_MCR_RTS_LEN,value)


/* OUT1 ACCESS: RW */

#ifndef UART_MCR_OUT1_POS
#define UART_MCR_OUT1_POS      2
#endif

#ifndef UART_MCR_OUT1_LEN
#define UART_MCR_OUT1_LEN      1
#endif

#if defined(_V1) && !defined(MCR_OUT1_R)
#define MCR_OUT1_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_OUT1_POS, UART_MCR_OUT1_LEN)
#endif

#if defined(_V1) && !defined(MCR_OUT1_W)
#define MCR_OUT1_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_OUT1_POS, UART_MCR_OUT1_LEN,value)
#endif

#define UART2_MCR_OUT1_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_OUT1_POS, UART_MCR_OUT1_LEN)

#define UART2_MCR_OUT1_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_OUT1_POS, UART_MCR_OUT1_LEN,value)


/* OUT2 ACCESS: RW */

#ifndef UART_MCR_OUT2_POS
#define UART_MCR_OUT2_POS      3
#endif

#ifndef UART_MCR_OUT2_LEN
#define UART_MCR_OUT2_LEN      1
#endif

#if defined(_V1) && !defined(MCR_OUT2_R)
#define MCR_OUT2_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_OUT2_POS, UART_MCR_OUT2_LEN)
#endif

#if defined(_V1) && !defined(MCR_OUT2_W)
#define MCR_OUT2_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_OUT2_POS, UART_MCR_OUT2_LEN,value)
#endif

#define UART2_MCR_OUT2_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_OUT2_POS, UART_MCR_OUT2_LEN)

#define UART2_MCR_OUT2_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_OUT2_POS, UART_MCR_OUT2_LEN,value)


/* LoopBack ACCESS: RW */

#ifndef UART_MCR_LOOPBACK_POS
#define UART_MCR_LOOPBACK_POS      4
#endif

#ifndef UART_MCR_LOOPBACK_LEN
#define UART_MCR_LOOPBACK_LEN      1
#endif

#if defined(_V1) && !defined(MCR_LOOPBACK_R)
#define MCR_LOOPBACK_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_LOOPBACK_POS, UART_MCR_LOOPBACK_LEN)
#endif

#if defined(_V1) && !defined(MCR_LOOPBACK_W)
#define MCR_LOOPBACK_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_LOOPBACK_POS, UART_MCR_LOOPBACK_LEN,value)
#endif

#define UART2_MCR_LOOPBACK_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_LOOPBACK_POS, UART_MCR_LOOPBACK_LEN)

#define UART2_MCR_LOOPBACK_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_LOOPBACK_POS, UART_MCR_LOOPBACK_LEN,value)


/* AFCE ACCESS: RW */

#ifndef UART_MCR_AFCE_POS
#define UART_MCR_AFCE_POS      5
#endif

#ifndef UART_MCR_AFCE_LEN
#define UART_MCR_AFCE_LEN      1
#endif

#if defined(_V1) && !defined(MCR_AFCE_R)
#define MCR_AFCE_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_AFCE_POS, UART_MCR_AFCE_LEN)
#endif

#if defined(_V1) && !defined(MCR_AFCE_W)
#define MCR_AFCE_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_AFCE_POS, UART_MCR_AFCE_LEN,value)
#endif

#define UART2_MCR_AFCE_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_AFCE_POS, UART_MCR_AFCE_LEN)

#define UART2_MCR_AFCE_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_AFCE_POS, UART_MCR_AFCE_LEN,value)


/* SIRE ACCESS: RW */

#ifndef UART_MCR_SIRE_POS
#define UART_MCR_SIRE_POS      6
#endif

#ifndef UART_MCR_SIRE_LEN
#define UART_MCR_SIRE_LEN      1
#endif

#if defined(_V1) && !defined(MCR_SIRE_R)
#define MCR_SIRE_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_SIRE_POS, UART_MCR_SIRE_LEN)
#endif

#if defined(_V1) && !defined(MCR_SIRE_W)
#define MCR_SIRE_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_SIRE_POS, UART_MCR_SIRE_LEN,value)
#endif

#define UART2_MCR_SIRE_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_SIRE_POS, UART_MCR_SIRE_LEN)

#define UART2_MCR_SIRE_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_SIRE_POS, UART_MCR_SIRE_LEN,value)


/* RSVD_MCR_31to7 ACCESS: RW */

#ifndef UART_MCR_RSVD_MCR_31TO7_POS
#define UART_MCR_RSVD_MCR_31TO7_POS      7
#endif

#ifndef UART_MCR_RSVD_MCR_31TO7_LEN
#define UART_MCR_RSVD_MCR_31TO7_LEN      25
#endif

#if defined(_V1) && !defined(MCR_RSVD_MCR_31TO7_R)
#define MCR_RSVD_MCR_31TO7_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_RSVD_MCR_31TO7_POS, UART_MCR_RSVD_MCR_31TO7_LEN)
#endif

#if defined(_V1) && !defined(MCR_RSVD_MCR_31TO7_W)
#define MCR_RSVD_MCR_31TO7_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_RSVD_MCR_31TO7_POS, UART_MCR_RSVD_MCR_31TO7_LEN,value)
#endif

#define UART2_MCR_RSVD_MCR_31TO7_R        GetGroupBits32( (UART2_MCR_VAL),UART_MCR_RSVD_MCR_31TO7_POS, UART_MCR_RSVD_MCR_31TO7_LEN)

#define UART2_MCR_RSVD_MCR_31TO7_W(value) SetGroupBits32( (UART2_MCR_VAL),UART_MCR_RSVD_MCR_31TO7_POS, UART_MCR_RSVD_MCR_31TO7_LEN,value)


/* REGISTER: LSR ACCESS: RW */

#if defined(_V1) && !defined(LSR_OFFSET)
#define LSR_OFFSET 0x14
#endif

#if !defined(UART_LSR_OFFSET)
#define UART_LSR_OFFSET 0x14
#endif

#if defined(_V1) && !defined(LSR_REG)
#define LSR_REG ((volatile UINT32 *) (UART2_BASE + UART_LSR_OFFSET))
#endif

#if defined(_V1) && !defined(LSR_VAL)
#define LSR_VAL  PREFIX_VAL(LSR_REG)
#endif

#define UART2_LSR_REG ((volatile UINT32 *) (UART2_BASE + UART_LSR_OFFSET))
#define UART2_LSR_VAL  PREFIX_VAL(UART2_LSR_REG)

/* FIELDS: */

/* DR ACCESS: RW */

#ifndef UART_LSR_DR_POS
#define UART_LSR_DR_POS      0
#endif

#ifndef UART_LSR_DR_LEN
#define UART_LSR_DR_LEN      1
#endif

#if defined(_V1) && !defined(LSR_DR_R)
#define LSR_DR_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_DR_POS, UART_LSR_DR_LEN)
#endif

#if defined(_V1) && !defined(LSR_DR_W)
#define LSR_DR_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_DR_POS, UART_LSR_DR_LEN,value)
#endif

#define UART2_LSR_DR_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_DR_POS, UART_LSR_DR_LEN)

#define UART2_LSR_DR_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_DR_POS, UART_LSR_DR_LEN,value)


/* OE ACCESS: RW */

#ifndef UART_LSR_OE_POS
#define UART_LSR_OE_POS      1
#endif

#ifndef UART_LSR_OE_LEN
#define UART_LSR_OE_LEN      1
#endif

#if defined(_V1) && !defined(LSR_OE_R)
#define LSR_OE_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_OE_POS, UART_LSR_OE_LEN)
#endif

#if defined(_V1) && !defined(LSR_OE_W)
#define LSR_OE_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_OE_POS, UART_LSR_OE_LEN,value)
#endif

#define UART2_LSR_OE_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_OE_POS, UART_LSR_OE_LEN)

#define UART2_LSR_OE_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_OE_POS, UART_LSR_OE_LEN,value)


/* PE ACCESS: RW */

#ifndef UART_LSR_PE_POS
#define UART_LSR_PE_POS      2
#endif

#ifndef UART_LSR_PE_LEN
#define UART_LSR_PE_LEN      1
#endif

#if defined(_V1) && !defined(LSR_PE_R)
#define LSR_PE_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_PE_POS, UART_LSR_PE_LEN)
#endif

#if defined(_V1) && !defined(LSR_PE_W)
#define LSR_PE_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_PE_POS, UART_LSR_PE_LEN,value)
#endif

#define UART2_LSR_PE_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_PE_POS, UART_LSR_PE_LEN)

#define UART2_LSR_PE_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_PE_POS, UART_LSR_PE_LEN,value)


/* FE ACCESS: RW */

#ifndef UART_LSR_FE_POS
#define UART_LSR_FE_POS      3
#endif

#ifndef UART_LSR_FE_LEN
#define UART_LSR_FE_LEN      1
#endif

#if defined(_V1) && !defined(LSR_FE_R)
#define LSR_FE_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_FE_POS, UART_LSR_FE_LEN)
#endif

#if defined(_V1) && !defined(LSR_FE_W)
#define LSR_FE_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_FE_POS, UART_LSR_FE_LEN,value)
#endif

#define UART2_LSR_FE_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_FE_POS, UART_LSR_FE_LEN)

#define UART2_LSR_FE_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_FE_POS, UART_LSR_FE_LEN,value)


/* BI ACCESS: RW */

#ifndef UART_LSR_BI_POS
#define UART_LSR_BI_POS      4
#endif

#ifndef UART_LSR_BI_LEN
#define UART_LSR_BI_LEN      1
#endif

#if defined(_V1) && !defined(LSR_BI_R)
#define LSR_BI_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_BI_POS, UART_LSR_BI_LEN)
#endif

#if defined(_V1) && !defined(LSR_BI_W)
#define LSR_BI_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_BI_POS, UART_LSR_BI_LEN,value)
#endif

#define UART2_LSR_BI_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_BI_POS, UART_LSR_BI_LEN)

#define UART2_LSR_BI_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_BI_POS, UART_LSR_BI_LEN,value)


/* THRE ACCESS: RW */

#ifndef UART_LSR_THRE_POS
#define UART_LSR_THRE_POS      5
#endif

#ifndef UART_LSR_THRE_LEN
#define UART_LSR_THRE_LEN      1
#endif

#if defined(_V1) && !defined(LSR_THRE_R)
#define LSR_THRE_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_THRE_POS, UART_LSR_THRE_LEN)
#endif

#if defined(_V1) && !defined(LSR_THRE_W)
#define LSR_THRE_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_THRE_POS, UART_LSR_THRE_LEN,value)
#endif

#define UART2_LSR_THRE_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_THRE_POS, UART_LSR_THRE_LEN)

#define UART2_LSR_THRE_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_THRE_POS, UART_LSR_THRE_LEN,value)


/* TEMT ACCESS: RW */

#ifndef UART_LSR_TEMT_POS
#define UART_LSR_TEMT_POS      6
#endif

#ifndef UART_LSR_TEMT_LEN
#define UART_LSR_TEMT_LEN      1
#endif

#if defined(_V1) && !defined(LSR_TEMT_R)
#define LSR_TEMT_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_TEMT_POS, UART_LSR_TEMT_LEN)
#endif

#if defined(_V1) && !defined(LSR_TEMT_W)
#define LSR_TEMT_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_TEMT_POS, UART_LSR_TEMT_LEN,value)
#endif

#define UART2_LSR_TEMT_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_TEMT_POS, UART_LSR_TEMT_LEN)

#define UART2_LSR_TEMT_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_TEMT_POS, UART_LSR_TEMT_LEN,value)


/* RFE ACCESS: RW */

#ifndef UART_LSR_RFE_POS
#define UART_LSR_RFE_POS      7
#endif

#ifndef UART_LSR_RFE_LEN
#define UART_LSR_RFE_LEN      1
#endif

#if defined(_V1) && !defined(LSR_RFE_R)
#define LSR_RFE_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_RFE_POS, UART_LSR_RFE_LEN)
#endif

#if defined(_V1) && !defined(LSR_RFE_W)
#define LSR_RFE_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_RFE_POS, UART_LSR_RFE_LEN,value)
#endif

#define UART2_LSR_RFE_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_RFE_POS, UART_LSR_RFE_LEN)

#define UART2_LSR_RFE_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_RFE_POS, UART_LSR_RFE_LEN,value)


/* RSVD_ADDR_RCVD ACCESS: RW */

#ifndef UART_LSR_RSVD_ADDR_RCVD_POS
#define UART_LSR_RSVD_ADDR_RCVD_POS      8
#endif

#ifndef UART_LSR_RSVD_ADDR_RCVD_LEN
#define UART_LSR_RSVD_ADDR_RCVD_LEN      1
#endif

#if defined(_V1) && !defined(LSR_RSVD_ADDR_RCVD_R)
#define LSR_RSVD_ADDR_RCVD_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_RSVD_ADDR_RCVD_POS, UART_LSR_RSVD_ADDR_RCVD_LEN)
#endif

#if defined(_V1) && !defined(LSR_RSVD_ADDR_RCVD_W)
#define LSR_RSVD_ADDR_RCVD_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_RSVD_ADDR_RCVD_POS, UART_LSR_RSVD_ADDR_RCVD_LEN,value)
#endif

#define UART2_LSR_RSVD_ADDR_RCVD_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_RSVD_ADDR_RCVD_POS, UART_LSR_RSVD_ADDR_RCVD_LEN)

#define UART2_LSR_RSVD_ADDR_RCVD_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_RSVD_ADDR_RCVD_POS, UART_LSR_RSVD_ADDR_RCVD_LEN,value)


/* RSVD_LSR_31to9 ACCESS: RW */

#ifndef UART_LSR_RSVD_LSR_31TO9_POS
#define UART_LSR_RSVD_LSR_31TO9_POS      9
#endif

#ifndef UART_LSR_RSVD_LSR_31TO9_LEN
#define UART_LSR_RSVD_LSR_31TO9_LEN      23
#endif

#if defined(_V1) && !defined(LSR_RSVD_LSR_31TO9_R)
#define LSR_RSVD_LSR_31TO9_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_RSVD_LSR_31TO9_POS, UART_LSR_RSVD_LSR_31TO9_LEN)
#endif

#if defined(_V1) && !defined(LSR_RSVD_LSR_31TO9_W)
#define LSR_RSVD_LSR_31TO9_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_RSVD_LSR_31TO9_POS, UART_LSR_RSVD_LSR_31TO9_LEN,value)
#endif

#define UART2_LSR_RSVD_LSR_31TO9_R        GetGroupBits32( (UART2_LSR_VAL),UART_LSR_RSVD_LSR_31TO9_POS, UART_LSR_RSVD_LSR_31TO9_LEN)

#define UART2_LSR_RSVD_LSR_31TO9_W(value) SetGroupBits32( (UART2_LSR_VAL),UART_LSR_RSVD_LSR_31TO9_POS, UART_LSR_RSVD_LSR_31TO9_LEN,value)


/* REGISTER: MSR ACCESS: RW */

#if defined(_V1) && !defined(MSR_OFFSET)
#define MSR_OFFSET 0x18
#endif

#if !defined(UART_MSR_OFFSET)
#define UART_MSR_OFFSET 0x18
#endif

#if defined(_V1) && !defined(MSR_REG)
#define MSR_REG ((volatile UINT32 *) (UART2_BASE + UART_MSR_OFFSET))
#endif

#if defined(_V1) && !defined(MSR_VAL)
#define MSR_VAL  PREFIX_VAL(MSR_REG)
#endif

#define UART2_MSR_REG ((volatile UINT32 *) (UART2_BASE + UART_MSR_OFFSET))
#define UART2_MSR_VAL  PREFIX_VAL(UART2_MSR_REG)

/* FIELDS: */

/* DCTS ACCESS: RW */

#ifndef UART_MSR_DCTS_POS
#define UART_MSR_DCTS_POS      0
#endif

#ifndef UART_MSR_DCTS_LEN
#define UART_MSR_DCTS_LEN      1
#endif

#if defined(_V1) && !defined(MSR_DCTS_R)
#define MSR_DCTS_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_DCTS_POS, UART_MSR_DCTS_LEN)
#endif

#if defined(_V1) && !defined(MSR_DCTS_W)
#define MSR_DCTS_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_DCTS_POS, UART_MSR_DCTS_LEN,value)
#endif

#define UART2_MSR_DCTS_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_DCTS_POS, UART_MSR_DCTS_LEN)

#define UART2_MSR_DCTS_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_DCTS_POS, UART_MSR_DCTS_LEN,value)


/* DDSR ACCESS: RW */

#ifndef UART_MSR_DDSR_POS
#define UART_MSR_DDSR_POS      1
#endif

#ifndef UART_MSR_DDSR_LEN
#define UART_MSR_DDSR_LEN      1
#endif

#if defined(_V1) && !defined(MSR_DDSR_R)
#define MSR_DDSR_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_DDSR_POS, UART_MSR_DDSR_LEN)
#endif

#if defined(_V1) && !defined(MSR_DDSR_W)
#define MSR_DDSR_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_DDSR_POS, UART_MSR_DDSR_LEN,value)
#endif

#define UART2_MSR_DDSR_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_DDSR_POS, UART_MSR_DDSR_LEN)

#define UART2_MSR_DDSR_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_DDSR_POS, UART_MSR_DDSR_LEN,value)


/* TERI ACCESS: RW */

#ifndef UART_MSR_TERI_POS
#define UART_MSR_TERI_POS      2
#endif

#ifndef UART_MSR_TERI_LEN
#define UART_MSR_TERI_LEN      1
#endif

#if defined(_V1) && !defined(MSR_TERI_R)
#define MSR_TERI_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_TERI_POS, UART_MSR_TERI_LEN)
#endif

#if defined(_V1) && !defined(MSR_TERI_W)
#define MSR_TERI_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_TERI_POS, UART_MSR_TERI_LEN,value)
#endif

#define UART2_MSR_TERI_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_TERI_POS, UART_MSR_TERI_LEN)

#define UART2_MSR_TERI_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_TERI_POS, UART_MSR_TERI_LEN,value)


/* DDCD ACCESS: RW */

#ifndef UART_MSR_DDCD_POS
#define UART_MSR_DDCD_POS      3
#endif

#ifndef UART_MSR_DDCD_LEN
#define UART_MSR_DDCD_LEN      1
#endif

#if defined(_V1) && !defined(MSR_DDCD_R)
#define MSR_DDCD_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_DDCD_POS, UART_MSR_DDCD_LEN)
#endif

#if defined(_V1) && !defined(MSR_DDCD_W)
#define MSR_DDCD_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_DDCD_POS, UART_MSR_DDCD_LEN,value)
#endif

#define UART2_MSR_DDCD_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_DDCD_POS, UART_MSR_DDCD_LEN)

#define UART2_MSR_DDCD_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_DDCD_POS, UART_MSR_DDCD_LEN,value)


/* CTS ACCESS: RW */

#ifndef UART_MSR_CTS_POS
#define UART_MSR_CTS_POS      4
#endif

#ifndef UART_MSR_CTS_LEN
#define UART_MSR_CTS_LEN      1
#endif

#if defined(_V1) && !defined(MSR_CTS_R)
#define MSR_CTS_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_CTS_POS, UART_MSR_CTS_LEN)
#endif

#if defined(_V1) && !defined(MSR_CTS_W)
#define MSR_CTS_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_CTS_POS, UART_MSR_CTS_LEN,value)
#endif

#define UART2_MSR_CTS_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_CTS_POS, UART_MSR_CTS_LEN)

#define UART2_MSR_CTS_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_CTS_POS, UART_MSR_CTS_LEN,value)


/* DSR ACCESS: RW */

#ifndef UART_MSR_DSR_POS
#define UART_MSR_DSR_POS      5
#endif

#ifndef UART_MSR_DSR_LEN
#define UART_MSR_DSR_LEN      1
#endif

#if defined(_V1) && !defined(MSR_DSR_R)
#define MSR_DSR_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_DSR_POS, UART_MSR_DSR_LEN)
#endif

#if defined(_V1) && !defined(MSR_DSR_W)
#define MSR_DSR_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_DSR_POS, UART_MSR_DSR_LEN,value)
#endif

#define UART2_MSR_DSR_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_DSR_POS, UART_MSR_DSR_LEN)

#define UART2_MSR_DSR_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_DSR_POS, UART_MSR_DSR_LEN,value)


/* RI ACCESS: RW */

#ifndef UART_MSR_RI_POS
#define UART_MSR_RI_POS      6
#endif

#ifndef UART_MSR_RI_LEN
#define UART_MSR_RI_LEN      1
#endif

#if defined(_V1) && !defined(MSR_RI_R)
#define MSR_RI_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_RI_POS, UART_MSR_RI_LEN)
#endif

#if defined(_V1) && !defined(MSR_RI_W)
#define MSR_RI_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_RI_POS, UART_MSR_RI_LEN,value)
#endif

#define UART2_MSR_RI_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_RI_POS, UART_MSR_RI_LEN)

#define UART2_MSR_RI_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_RI_POS, UART_MSR_RI_LEN,value)


/* DCD ACCESS: RW */

#ifndef UART_MSR_DCD_POS
#define UART_MSR_DCD_POS      7
#endif

#ifndef UART_MSR_DCD_LEN
#define UART_MSR_DCD_LEN      1
#endif

#if defined(_V1) && !defined(MSR_DCD_R)
#define MSR_DCD_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_DCD_POS, UART_MSR_DCD_LEN)
#endif

#if defined(_V1) && !defined(MSR_DCD_W)
#define MSR_DCD_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_DCD_POS, UART_MSR_DCD_LEN,value)
#endif

#define UART2_MSR_DCD_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_DCD_POS, UART_MSR_DCD_LEN)

#define UART2_MSR_DCD_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_DCD_POS, UART_MSR_DCD_LEN,value)


/* RSVD_MSR_31to8 ACCESS: RW */

#ifndef UART_MSR_RSVD_MSR_31TO8_POS
#define UART_MSR_RSVD_MSR_31TO8_POS      8
#endif

#ifndef UART_MSR_RSVD_MSR_31TO8_LEN
#define UART_MSR_RSVD_MSR_31TO8_LEN      24
#endif

#if defined(_V1) && !defined(MSR_RSVD_MSR_31TO8_R)
#define MSR_RSVD_MSR_31TO8_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_RSVD_MSR_31TO8_POS, UART_MSR_RSVD_MSR_31TO8_LEN)
#endif

#if defined(_V1) && !defined(MSR_RSVD_MSR_31TO8_W)
#define MSR_RSVD_MSR_31TO8_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_RSVD_MSR_31TO8_POS, UART_MSR_RSVD_MSR_31TO8_LEN,value)
#endif

#define UART2_MSR_RSVD_MSR_31TO8_R        GetGroupBits32( (UART2_MSR_VAL),UART_MSR_RSVD_MSR_31TO8_POS, UART_MSR_RSVD_MSR_31TO8_LEN)

#define UART2_MSR_RSVD_MSR_31TO8_W(value) SetGroupBits32( (UART2_MSR_VAL),UART_MSR_RSVD_MSR_31TO8_POS, UART_MSR_RSVD_MSR_31TO8_LEN,value)


/* REGISTER: SCR ACCESS: RW */

#if defined(_V1) && !defined(SCR_OFFSET)
#define SCR_OFFSET 0x1c
#endif

#if !defined(UART_SCR_OFFSET)
#define UART_SCR_OFFSET 0x1c
#endif

#if defined(_V1) && !defined(SCR_REG)
#define SCR_REG ((volatile UINT32 *) (UART2_BASE + UART_SCR_OFFSET))
#endif

#if defined(_V1) && !defined(SCR_VAL)
#define SCR_VAL  PREFIX_VAL(SCR_REG)
#endif

#define UART2_SCR_REG ((volatile UINT32 *) (UART2_BASE + UART_SCR_OFFSET))
#define UART2_SCR_VAL  PREFIX_VAL(UART2_SCR_REG)

/* FIELDS: */

/* scr ACCESS: RW */

#ifndef UART_SCR_SCR_POS
#define UART_SCR_SCR_POS      0
#endif

#ifndef UART_SCR_SCR_LEN
#define UART_SCR_SCR_LEN      8
#endif

#if defined(_V1) && !defined(SCR_SCR_R)
#define SCR_SCR_R        GetGroupBits32( (UART2_SCR_VAL),UART_SCR_SCR_POS, UART_SCR_SCR_LEN)
#endif

#if defined(_V1) && !defined(SCR_SCR_W)
#define SCR_SCR_W(value) SetGroupBits32( (UART2_SCR_VAL),UART_SCR_SCR_POS, UART_SCR_SCR_LEN,value)
#endif

#define UART2_SCR_SCR_R        GetGroupBits32( (UART2_SCR_VAL),UART_SCR_SCR_POS, UART_SCR_SCR_LEN)

#define UART2_SCR_SCR_W(value) SetGroupBits32( (UART2_SCR_VAL),UART_SCR_SCR_POS, UART_SCR_SCR_LEN,value)


/* RSVD_SCR_31to8 ACCESS: RW */

#ifndef UART_SCR_RSVD_SCR_31TO8_POS
#define UART_SCR_RSVD_SCR_31TO8_POS      8
#endif

#ifndef UART_SCR_RSVD_SCR_31TO8_LEN
#define UART_SCR_RSVD_SCR_31TO8_LEN      24
#endif

#if defined(_V1) && !defined(SCR_RSVD_SCR_31TO8_R)
#define SCR_RSVD_SCR_31TO8_R        GetGroupBits32( (UART2_SCR_VAL),UART_SCR_RSVD_SCR_31TO8_POS, UART_SCR_RSVD_SCR_31TO8_LEN)
#endif

#if defined(_V1) && !defined(SCR_RSVD_SCR_31TO8_W)
#define SCR_RSVD_SCR_31TO8_W(value) SetGroupBits32( (UART2_SCR_VAL),UART_SCR_RSVD_SCR_31TO8_POS, UART_SCR_RSVD_SCR_31TO8_LEN,value)
#endif

#define UART2_SCR_RSVD_SCR_31TO8_R        GetGroupBits32( (UART2_SCR_VAL),UART_SCR_RSVD_SCR_31TO8_POS, UART_SCR_RSVD_SCR_31TO8_LEN)

#define UART2_SCR_RSVD_SCR_31TO8_W(value) SetGroupBits32( (UART2_SCR_VAL),UART_SCR_RSVD_SCR_31TO8_POS, UART_SCR_RSVD_SCR_31TO8_LEN,value)


/* REGISTER: SRBR0 ACCESS: RW */

#if defined(_V1) && !defined(SRBR0_OFFSET)
#define SRBR0_OFFSET 0x30
#endif

#if !defined(UART_SRBR0_OFFSET)
#define UART_SRBR0_OFFSET 0x30
#endif

#if defined(_V1) && !defined(SRBR0_REG)
#define SRBR0_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR0_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR0_VAL)
#define SRBR0_VAL  PREFIX_VAL(SRBR0_REG)
#endif

#define UART2_SRBR0_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR0_OFFSET))
#define UART2_SRBR0_VAL  PREFIX_VAL(UART2_SRBR0_REG)

/* FIELDS: */

/* srbr0 ACCESS: RW */

#ifndef UART_SRBR0_SRBR0_POS
#define UART_SRBR0_SRBR0_POS      0
#endif

#ifndef UART_SRBR0_SRBR0_LEN
#define UART_SRBR0_SRBR0_LEN      8
#endif

#if defined(_V1) && !defined(SRBR0_SRBR0_R)
#define SRBR0_SRBR0_R        GetGroupBits32( (UART2_SRBR0_VAL),UART_SRBR0_SRBR0_POS, UART_SRBR0_SRBR0_LEN)
#endif

#if defined(_V1) && !defined(SRBR0_SRBR0_W)
#define SRBR0_SRBR0_W(value) SetGroupBits32( (UART2_SRBR0_VAL),UART_SRBR0_SRBR0_POS, UART_SRBR0_SRBR0_LEN,value)
#endif

#define UART2_SRBR0_SRBR0_R        GetGroupBits32( (UART2_SRBR0_VAL),UART_SRBR0_SRBR0_POS, UART_SRBR0_SRBR0_LEN)

#define UART2_SRBR0_SRBR0_W(value) SetGroupBits32( (UART2_SRBR0_VAL),UART_SRBR0_SRBR0_POS, UART_SRBR0_SRBR0_LEN,value)


/* RSVD_SRBR0 ACCESS: RW */

#ifndef UART_SRBR0_RSVD_SRBR0_POS
#define UART_SRBR0_RSVD_SRBR0_POS      8
#endif

#ifndef UART_SRBR0_RSVD_SRBR0_LEN
#define UART_SRBR0_RSVD_SRBR0_LEN      24
#endif

#if defined(_V1) && !defined(SRBR0_RSVD_SRBR0_R)
#define SRBR0_RSVD_SRBR0_R        GetGroupBits32( (UART2_SRBR0_VAL),UART_SRBR0_RSVD_SRBR0_POS, UART_SRBR0_RSVD_SRBR0_LEN)
#endif

#if defined(_V1) && !defined(SRBR0_RSVD_SRBR0_W)
#define SRBR0_RSVD_SRBR0_W(value) SetGroupBits32( (UART2_SRBR0_VAL),UART_SRBR0_RSVD_SRBR0_POS, UART_SRBR0_RSVD_SRBR0_LEN,value)
#endif

#define UART2_SRBR0_RSVD_SRBR0_R        GetGroupBits32( (UART2_SRBR0_VAL),UART_SRBR0_RSVD_SRBR0_POS, UART_SRBR0_RSVD_SRBR0_LEN)

#define UART2_SRBR0_RSVD_SRBR0_W(value) SetGroupBits32( (UART2_SRBR0_VAL),UART_SRBR0_RSVD_SRBR0_POS, UART_SRBR0_RSVD_SRBR0_LEN,value)


/* REGISTER: SRBR1 ACCESS: RW */

#if defined(_V1) && !defined(SRBR1_OFFSET)
#define SRBR1_OFFSET 0x34
#endif

#if !defined(UART_SRBR1_OFFSET)
#define UART_SRBR1_OFFSET 0x34
#endif

#if defined(_V1) && !defined(SRBR1_REG)
#define SRBR1_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR1_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR1_VAL)
#define SRBR1_VAL  PREFIX_VAL(SRBR1_REG)
#endif

#define UART2_SRBR1_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR1_OFFSET))
#define UART2_SRBR1_VAL  PREFIX_VAL(UART2_SRBR1_REG)

/* FIELDS: */

/* srbr1 ACCESS: RW */

#ifndef UART_SRBR1_SRBR1_POS
#define UART_SRBR1_SRBR1_POS      0
#endif

#ifndef UART_SRBR1_SRBR1_LEN
#define UART_SRBR1_SRBR1_LEN      8
#endif

#if defined(_V1) && !defined(SRBR1_SRBR1_R)
#define SRBR1_SRBR1_R        GetGroupBits32( (UART2_SRBR1_VAL),UART_SRBR1_SRBR1_POS, UART_SRBR1_SRBR1_LEN)
#endif

#if defined(_V1) && !defined(SRBR1_SRBR1_W)
#define SRBR1_SRBR1_W(value) SetGroupBits32( (UART2_SRBR1_VAL),UART_SRBR1_SRBR1_POS, UART_SRBR1_SRBR1_LEN,value)
#endif

#define UART2_SRBR1_SRBR1_R        GetGroupBits32( (UART2_SRBR1_VAL),UART_SRBR1_SRBR1_POS, UART_SRBR1_SRBR1_LEN)

#define UART2_SRBR1_SRBR1_W(value) SetGroupBits32( (UART2_SRBR1_VAL),UART_SRBR1_SRBR1_POS, UART_SRBR1_SRBR1_LEN,value)


/* RSVD_SRBR1 ACCESS: RW */

#ifndef UART_SRBR1_RSVD_SRBR1_POS
#define UART_SRBR1_RSVD_SRBR1_POS      8
#endif

#ifndef UART_SRBR1_RSVD_SRBR1_LEN
#define UART_SRBR1_RSVD_SRBR1_LEN      24
#endif

#if defined(_V1) && !defined(SRBR1_RSVD_SRBR1_R)
#define SRBR1_RSVD_SRBR1_R        GetGroupBits32( (UART2_SRBR1_VAL),UART_SRBR1_RSVD_SRBR1_POS, UART_SRBR1_RSVD_SRBR1_LEN)
#endif

#if defined(_V1) && !defined(SRBR1_RSVD_SRBR1_W)
#define SRBR1_RSVD_SRBR1_W(value) SetGroupBits32( (UART2_SRBR1_VAL),UART_SRBR1_RSVD_SRBR1_POS, UART_SRBR1_RSVD_SRBR1_LEN,value)
#endif

#define UART2_SRBR1_RSVD_SRBR1_R        GetGroupBits32( (UART2_SRBR1_VAL),UART_SRBR1_RSVD_SRBR1_POS, UART_SRBR1_RSVD_SRBR1_LEN)

#define UART2_SRBR1_RSVD_SRBR1_W(value) SetGroupBits32( (UART2_SRBR1_VAL),UART_SRBR1_RSVD_SRBR1_POS, UART_SRBR1_RSVD_SRBR1_LEN,value)


/* REGISTER: SRBR2 ACCESS: RW */

#if defined(_V1) && !defined(SRBR2_OFFSET)
#define SRBR2_OFFSET 0x38
#endif

#if !defined(UART_SRBR2_OFFSET)
#define UART_SRBR2_OFFSET 0x38
#endif

#if defined(_V1) && !defined(SRBR2_REG)
#define SRBR2_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR2_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR2_VAL)
#define SRBR2_VAL  PREFIX_VAL(SRBR2_REG)
#endif

#define UART2_SRBR2_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR2_OFFSET))
#define UART2_SRBR2_VAL  PREFIX_VAL(UART2_SRBR2_REG)

/* FIELDS: */

/* srbr2 ACCESS: RW */

#ifndef UART_SRBR2_SRBR2_POS
#define UART_SRBR2_SRBR2_POS      0
#endif

#ifndef UART_SRBR2_SRBR2_LEN
#define UART_SRBR2_SRBR2_LEN      8
#endif

#if defined(_V1) && !defined(SRBR2_SRBR2_R)
#define SRBR2_SRBR2_R        GetGroupBits32( (UART2_SRBR2_VAL),UART_SRBR2_SRBR2_POS, UART_SRBR2_SRBR2_LEN)
#endif

#if defined(_V1) && !defined(SRBR2_SRBR2_W)
#define SRBR2_SRBR2_W(value) SetGroupBits32( (UART2_SRBR2_VAL),UART_SRBR2_SRBR2_POS, UART_SRBR2_SRBR2_LEN,value)
#endif

#define UART2_SRBR2_SRBR2_R        GetGroupBits32( (UART2_SRBR2_VAL),UART_SRBR2_SRBR2_POS, UART_SRBR2_SRBR2_LEN)

#define UART2_SRBR2_SRBR2_W(value) SetGroupBits32( (UART2_SRBR2_VAL),UART_SRBR2_SRBR2_POS, UART_SRBR2_SRBR2_LEN,value)


/* RSVD_SRBR2 ACCESS: RW */

#ifndef UART_SRBR2_RSVD_SRBR2_POS
#define UART_SRBR2_RSVD_SRBR2_POS      8
#endif

#ifndef UART_SRBR2_RSVD_SRBR2_LEN
#define UART_SRBR2_RSVD_SRBR2_LEN      24
#endif

#if defined(_V1) && !defined(SRBR2_RSVD_SRBR2_R)
#define SRBR2_RSVD_SRBR2_R        GetGroupBits32( (UART2_SRBR2_VAL),UART_SRBR2_RSVD_SRBR2_POS, UART_SRBR2_RSVD_SRBR2_LEN)
#endif

#if defined(_V1) && !defined(SRBR2_RSVD_SRBR2_W)
#define SRBR2_RSVD_SRBR2_W(value) SetGroupBits32( (UART2_SRBR2_VAL),UART_SRBR2_RSVD_SRBR2_POS, UART_SRBR2_RSVD_SRBR2_LEN,value)
#endif

#define UART2_SRBR2_RSVD_SRBR2_R        GetGroupBits32( (UART2_SRBR2_VAL),UART_SRBR2_RSVD_SRBR2_POS, UART_SRBR2_RSVD_SRBR2_LEN)

#define UART2_SRBR2_RSVD_SRBR2_W(value) SetGroupBits32( (UART2_SRBR2_VAL),UART_SRBR2_RSVD_SRBR2_POS, UART_SRBR2_RSVD_SRBR2_LEN,value)


/* REGISTER: SRBR3 ACCESS: RW */

#if defined(_V1) && !defined(SRBR3_OFFSET)
#define SRBR3_OFFSET 0x3c
#endif

#if !defined(UART_SRBR3_OFFSET)
#define UART_SRBR3_OFFSET 0x3c
#endif

#if defined(_V1) && !defined(SRBR3_REG)
#define SRBR3_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR3_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR3_VAL)
#define SRBR3_VAL  PREFIX_VAL(SRBR3_REG)
#endif

#define UART2_SRBR3_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR3_OFFSET))
#define UART2_SRBR3_VAL  PREFIX_VAL(UART2_SRBR3_REG)

/* FIELDS: */

/* srbr3 ACCESS: RW */

#ifndef UART_SRBR3_SRBR3_POS
#define UART_SRBR3_SRBR3_POS      0
#endif

#ifndef UART_SRBR3_SRBR3_LEN
#define UART_SRBR3_SRBR3_LEN      8
#endif

#if defined(_V1) && !defined(SRBR3_SRBR3_R)
#define SRBR3_SRBR3_R        GetGroupBits32( (UART2_SRBR3_VAL),UART_SRBR3_SRBR3_POS, UART_SRBR3_SRBR3_LEN)
#endif

#if defined(_V1) && !defined(SRBR3_SRBR3_W)
#define SRBR3_SRBR3_W(value) SetGroupBits32( (UART2_SRBR3_VAL),UART_SRBR3_SRBR3_POS, UART_SRBR3_SRBR3_LEN,value)
#endif

#define UART2_SRBR3_SRBR3_R        GetGroupBits32( (UART2_SRBR3_VAL),UART_SRBR3_SRBR3_POS, UART_SRBR3_SRBR3_LEN)

#define UART2_SRBR3_SRBR3_W(value) SetGroupBits32( (UART2_SRBR3_VAL),UART_SRBR3_SRBR3_POS, UART_SRBR3_SRBR3_LEN,value)


/* RSVD_SRBR3 ACCESS: RW */

#ifndef UART_SRBR3_RSVD_SRBR3_POS
#define UART_SRBR3_RSVD_SRBR3_POS      8
#endif

#ifndef UART_SRBR3_RSVD_SRBR3_LEN
#define UART_SRBR3_RSVD_SRBR3_LEN      24
#endif

#if defined(_V1) && !defined(SRBR3_RSVD_SRBR3_R)
#define SRBR3_RSVD_SRBR3_R        GetGroupBits32( (UART2_SRBR3_VAL),UART_SRBR3_RSVD_SRBR3_POS, UART_SRBR3_RSVD_SRBR3_LEN)
#endif

#if defined(_V1) && !defined(SRBR3_RSVD_SRBR3_W)
#define SRBR3_RSVD_SRBR3_W(value) SetGroupBits32( (UART2_SRBR3_VAL),UART_SRBR3_RSVD_SRBR3_POS, UART_SRBR3_RSVD_SRBR3_LEN,value)
#endif

#define UART2_SRBR3_RSVD_SRBR3_R        GetGroupBits32( (UART2_SRBR3_VAL),UART_SRBR3_RSVD_SRBR3_POS, UART_SRBR3_RSVD_SRBR3_LEN)

#define UART2_SRBR3_RSVD_SRBR3_W(value) SetGroupBits32( (UART2_SRBR3_VAL),UART_SRBR3_RSVD_SRBR3_POS, UART_SRBR3_RSVD_SRBR3_LEN,value)


/* REGISTER: IER ACCESS: RW */

#if defined(_V1) && !defined(IER_OFFSET)
#define IER_OFFSET 0x4
#endif

#if !defined(UART_IER_OFFSET)
#define UART_IER_OFFSET 0x4
#endif

#if defined(_V1) && !defined(IER_REG)
#define IER_REG ((volatile UINT32 *) (UART2_BASE + UART_IER_OFFSET))
#endif

#if defined(_V1) && !defined(IER_VAL)
#define IER_VAL  PREFIX_VAL(IER_REG)
#endif

#define UART2_IER_REG ((volatile UINT32 *) (UART2_BASE + UART_IER_OFFSET))
#define UART2_IER_VAL  PREFIX_VAL(UART2_IER_REG)

/* FIELDS: */

/* ERBFI ACCESS: RW */

#ifndef UART_IER_ERBFI_POS
#define UART_IER_ERBFI_POS      0
#endif

#ifndef UART_IER_ERBFI_LEN
#define UART_IER_ERBFI_LEN      1
#endif

#if defined(_V1) && !defined(IER_ERBFI_R)
#define IER_ERBFI_R        GetGroupBits32( (UART2_IER_VAL),UART_IER_ERBFI_POS, UART_IER_ERBFI_LEN)
#endif

#if defined(_V1) && !defined(IER_ERBFI_W)
#define IER_ERBFI_W(value) SetGroupBits32( (UART2_IER_VAL),UART_IER_ERBFI_POS, UART_IER_ERBFI_LEN,value)
#endif

#define UART2_IER_ERBFI_R        GetGroupBits32( (UART2_IER_VAL),UART_IER_ERBFI_POS, UART_IER_ERBFI_LEN)

#define UART2_IER_ERBFI_W(value) SetGroupBits32( (UART2_IER_VAL),UART_IER_ERBFI_POS, UART_IER_ERBFI_LEN,value)


/* ETBEI ACCESS: RW */

#ifndef UART_IER_ETBEI_POS
#define UART_IER_ETBEI_POS      1
#endif

#ifndef UART_IER_ETBEI_LEN
#define UART_IER_ETBEI_LEN      1
#endif

#if defined(_V1) && !defined(IER_ETBEI_R)
#define IER_ETBEI_R        GetGroupBits32( (UART2_IER_VAL),UART_IER_ETBEI_POS, UART_IER_ETBEI_LEN)
#endif

#if defined(_V1) && !defined(IER_ETBEI_W)
#define IER_ETBEI_W(value) SetGroupBits32( (UART2_IER_VAL),UART_IER_ETBEI_POS, UART_IER_ETBEI_LEN,value)
#endif

#define UART2_IER_ETBEI_R        GetGroupBits32( (UART2_IER_VAL),UART_IER_ETBEI_POS, UART_IER_ETBEI_LEN)

#define UART2_IER_ETBEI_W(value) SetGroupBits32( (UART2_IER_VAL),UART_IER_ETBEI_POS, UART_IER_ETBEI_LEN,value)


/* ELSI ACCESS: RW */

#ifndef UART_IER_ELSI_POS
#define UART_IER_ELSI_POS      2
#endif

#ifndef UART_IER_ELSI_LEN
#define UART_IER_ELSI_LEN      1
#endif

#if defined(_V1) && !defined(IER_ELSI_R)
#define IER_ELSI_R        GetGroupBits32( (UART2_IER_VAL),UART_IER_ELSI_POS, UART_IER_ELSI_LEN)
#endif

#if defined(_V1) && !defined(IER_ELSI_W)
#define IER_ELSI_W(value) SetGroupBits32( (UART2_IER_VAL),UART_IER_ELSI_POS, UART_IER_ELSI_LEN,value)
#endif

#define UART2_IER_ELSI_R        GetGroupBits32( (UART2_IER_VAL),UART_IER_ELSI_POS, UART_IER_ELSI_LEN)

#define UART2_IER_ELSI_W(value) SetGroupBits32( (UART2_IER_VAL),UART_IER_ELSI_POS, UART_IER_ELSI_LEN,value)


/* EDSSI ACCESS: RW */

#ifndef UART_IER_EDSSI_POS
#define UART_IER_EDSSI_POS      3
#endif

#ifndef UART_IER_EDSSI_LEN
#define UART_IER_EDSSI_LEN      1
#endif

#if defined(_V1) && !defined(IER_EDSSI_R)
#define IER_EDSSI_R        GetGroupBits32( (UART2_IER_VAL),UART_IER_EDSSI_POS, UART_IER_EDSSI_LEN)
#endif

#if defined(_V1) && !defined(IER_EDSSI_W)
#define IER_EDSSI_W(value) SetGroupBits32( (UART2_IER_VAL),UART_IER_EDSSI_POS, UART_IER_EDSSI_LEN,value)
#endif

#define UART2_IER_EDSSI_R        GetGroupBits32( (UART2_IER_VAL),UART_IER_EDSSI_POS, UART_IER_EDSSI_LEN)

#define UART2_IER_EDSSI_W(value) SetGroupBits32( (UART2_IER_VAL),UART_IER_EDSSI_POS, UART_IER_EDSSI_LEN,value)


/* RSVD_IER_6to4 ACCESS: RW */

#ifndef UART_IER_RSVD_IER_6TO4_POS
#define UART_IER_RSVD_IER_6TO4_POS      4
#endif

#ifndef UART_IER_RSVD_IER_6TO4_LEN
#define UART_IER_RSVD_IER_6TO4_LEN      3
#endif

#if defined(_V1) && !defined(IER_RSVD_IER_6TO4_R)
#define IER_RSVD_IER_6TO4_R        GetGroupBits32( (UART2_IER_VAL),UART_IER_RSVD_IER_6TO4_POS, UART_IER_RSVD_IER_6TO4_LEN)
#endif

#if defined(_V1) && !defined(IER_RSVD_IER_6TO4_W)
#define IER_RSVD_IER_6TO4_W(value) SetGroupBits32( (UART2_IER_VAL),UART_IER_RSVD_IER_6TO4_POS, UART_IER_RSVD_IER_6TO4_LEN,value)
#endif

#define UART2_IER_RSVD_IER_6TO4_R        GetGroupBits32( (UART2_IER_VAL),UART_IER_RSVD_IER_6TO4_POS, UART_IER_RSVD_IER_6TO4_LEN)

#define UART2_IER_RSVD_IER_6TO4_W(value) SetGroupBits32( (UART2_IER_VAL),UART_IER_RSVD_IER_6TO4_POS, UART_IER_RSVD_IER_6TO4_LEN,value)


/* PTIME ACCESS: RW */

#ifndef UART_IER_PTIME_POS
#define UART_IER_PTIME_POS      7
#endif

#ifndef UART_IER_PTIME_LEN
#define UART_IER_PTIME_LEN      1
#endif

#if defined(_V1) && !defined(IER_PTIME_R)
#define IER_PTIME_R        GetGroupBits32( (UART2_IER_VAL),UART_IER_PTIME_POS, UART_IER_PTIME_LEN)
#endif

#if defined(_V1) && !defined(IER_PTIME_W)
#define IER_PTIME_W(value) SetGroupBits32( (UART2_IER_VAL),UART_IER_PTIME_POS, UART_IER_PTIME_LEN,value)
#endif

#define UART2_IER_PTIME_R        GetGroupBits32( (UART2_IER_VAL),UART_IER_PTIME_POS, UART_IER_PTIME_LEN)

#define UART2_IER_PTIME_W(value) SetGroupBits32( (UART2_IER_VAL),UART_IER_PTIME_POS, UART_IER_PTIME_LEN,value)


/* RSVD_IER_31to8 ACCESS: RW */

#ifndef UART_IER_RSVD_IER_31TO8_POS
#define UART_IER_RSVD_IER_31TO8_POS      8
#endif

#ifndef UART_IER_RSVD_IER_31TO8_LEN
#define UART_IER_RSVD_IER_31TO8_LEN      24
#endif

#if defined(_V1) && !defined(IER_RSVD_IER_31TO8_R)
#define IER_RSVD_IER_31TO8_R        GetGroupBits32( (UART2_IER_VAL),UART_IER_RSVD_IER_31TO8_POS, UART_IER_RSVD_IER_31TO8_LEN)
#endif

#if defined(_V1) && !defined(IER_RSVD_IER_31TO8_W)
#define IER_RSVD_IER_31TO8_W(value) SetGroupBits32( (UART2_IER_VAL),UART_IER_RSVD_IER_31TO8_POS, UART_IER_RSVD_IER_31TO8_LEN,value)
#endif

#define UART2_IER_RSVD_IER_31TO8_R        GetGroupBits32( (UART2_IER_VAL),UART_IER_RSVD_IER_31TO8_POS, UART_IER_RSVD_IER_31TO8_LEN)

#define UART2_IER_RSVD_IER_31TO8_W(value) SetGroupBits32( (UART2_IER_VAL),UART_IER_RSVD_IER_31TO8_POS, UART_IER_RSVD_IER_31TO8_LEN,value)


/* REGISTER: SRBR4 ACCESS: RW */

#if defined(_V1) && !defined(SRBR4_OFFSET)
#define SRBR4_OFFSET 0x40
#endif

#if !defined(UART_SRBR4_OFFSET)
#define UART_SRBR4_OFFSET 0x40
#endif

#if defined(_V1) && !defined(SRBR4_REG)
#define SRBR4_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR4_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR4_VAL)
#define SRBR4_VAL  PREFIX_VAL(SRBR4_REG)
#endif

#define UART2_SRBR4_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR4_OFFSET))
#define UART2_SRBR4_VAL  PREFIX_VAL(UART2_SRBR4_REG)

/* FIELDS: */

/* srbr4 ACCESS: RW */

#ifndef UART_SRBR4_SRBR4_POS
#define UART_SRBR4_SRBR4_POS      0
#endif

#ifndef UART_SRBR4_SRBR4_LEN
#define UART_SRBR4_SRBR4_LEN      8
#endif

#if defined(_V1) && !defined(SRBR4_SRBR4_R)
#define SRBR4_SRBR4_R        GetGroupBits32( (UART2_SRBR4_VAL),UART_SRBR4_SRBR4_POS, UART_SRBR4_SRBR4_LEN)
#endif

#if defined(_V1) && !defined(SRBR4_SRBR4_W)
#define SRBR4_SRBR4_W(value) SetGroupBits32( (UART2_SRBR4_VAL),UART_SRBR4_SRBR4_POS, UART_SRBR4_SRBR4_LEN,value)
#endif

#define UART2_SRBR4_SRBR4_R        GetGroupBits32( (UART2_SRBR4_VAL),UART_SRBR4_SRBR4_POS, UART_SRBR4_SRBR4_LEN)

#define UART2_SRBR4_SRBR4_W(value) SetGroupBits32( (UART2_SRBR4_VAL),UART_SRBR4_SRBR4_POS, UART_SRBR4_SRBR4_LEN,value)


/* RSVD_SRBR4 ACCESS: RW */

#ifndef UART_SRBR4_RSVD_SRBR4_POS
#define UART_SRBR4_RSVD_SRBR4_POS      8
#endif

#ifndef UART_SRBR4_RSVD_SRBR4_LEN
#define UART_SRBR4_RSVD_SRBR4_LEN      24
#endif

#if defined(_V1) && !defined(SRBR4_RSVD_SRBR4_R)
#define SRBR4_RSVD_SRBR4_R        GetGroupBits32( (UART2_SRBR4_VAL),UART_SRBR4_RSVD_SRBR4_POS, UART_SRBR4_RSVD_SRBR4_LEN)
#endif

#if defined(_V1) && !defined(SRBR4_RSVD_SRBR4_W)
#define SRBR4_RSVD_SRBR4_W(value) SetGroupBits32( (UART2_SRBR4_VAL),UART_SRBR4_RSVD_SRBR4_POS, UART_SRBR4_RSVD_SRBR4_LEN,value)
#endif

#define UART2_SRBR4_RSVD_SRBR4_R        GetGroupBits32( (UART2_SRBR4_VAL),UART_SRBR4_RSVD_SRBR4_POS, UART_SRBR4_RSVD_SRBR4_LEN)

#define UART2_SRBR4_RSVD_SRBR4_W(value) SetGroupBits32( (UART2_SRBR4_VAL),UART_SRBR4_RSVD_SRBR4_POS, UART_SRBR4_RSVD_SRBR4_LEN,value)


/* REGISTER: SRBR5 ACCESS: RW */

#if defined(_V1) && !defined(SRBR5_OFFSET)
#define SRBR5_OFFSET 0x44
#endif

#if !defined(UART_SRBR5_OFFSET)
#define UART_SRBR5_OFFSET 0x44
#endif

#if defined(_V1) && !defined(SRBR5_REG)
#define SRBR5_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR5_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR5_VAL)
#define SRBR5_VAL  PREFIX_VAL(SRBR5_REG)
#endif

#define UART2_SRBR5_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR5_OFFSET))
#define UART2_SRBR5_VAL  PREFIX_VAL(UART2_SRBR5_REG)

/* FIELDS: */

/* srbr5 ACCESS: RW */

#ifndef UART_SRBR5_SRBR5_POS
#define UART_SRBR5_SRBR5_POS      0
#endif

#ifndef UART_SRBR5_SRBR5_LEN
#define UART_SRBR5_SRBR5_LEN      8
#endif

#if defined(_V1) && !defined(SRBR5_SRBR5_R)
#define SRBR5_SRBR5_R        GetGroupBits32( (UART2_SRBR5_VAL),UART_SRBR5_SRBR5_POS, UART_SRBR5_SRBR5_LEN)
#endif

#if defined(_V1) && !defined(SRBR5_SRBR5_W)
#define SRBR5_SRBR5_W(value) SetGroupBits32( (UART2_SRBR5_VAL),UART_SRBR5_SRBR5_POS, UART_SRBR5_SRBR5_LEN,value)
#endif

#define UART2_SRBR5_SRBR5_R        GetGroupBits32( (UART2_SRBR5_VAL),UART_SRBR5_SRBR5_POS, UART_SRBR5_SRBR5_LEN)

#define UART2_SRBR5_SRBR5_W(value) SetGroupBits32( (UART2_SRBR5_VAL),UART_SRBR5_SRBR5_POS, UART_SRBR5_SRBR5_LEN,value)


/* RSVD_SRBR5 ACCESS: RW */

#ifndef UART_SRBR5_RSVD_SRBR5_POS
#define UART_SRBR5_RSVD_SRBR5_POS      8
#endif

#ifndef UART_SRBR5_RSVD_SRBR5_LEN
#define UART_SRBR5_RSVD_SRBR5_LEN      24
#endif

#if defined(_V1) && !defined(SRBR5_RSVD_SRBR5_R)
#define SRBR5_RSVD_SRBR5_R        GetGroupBits32( (UART2_SRBR5_VAL),UART_SRBR5_RSVD_SRBR5_POS, UART_SRBR5_RSVD_SRBR5_LEN)
#endif

#if defined(_V1) && !defined(SRBR5_RSVD_SRBR5_W)
#define SRBR5_RSVD_SRBR5_W(value) SetGroupBits32( (UART2_SRBR5_VAL),UART_SRBR5_RSVD_SRBR5_POS, UART_SRBR5_RSVD_SRBR5_LEN,value)
#endif

#define UART2_SRBR5_RSVD_SRBR5_R        GetGroupBits32( (UART2_SRBR5_VAL),UART_SRBR5_RSVD_SRBR5_POS, UART_SRBR5_RSVD_SRBR5_LEN)

#define UART2_SRBR5_RSVD_SRBR5_W(value) SetGroupBits32( (UART2_SRBR5_VAL),UART_SRBR5_RSVD_SRBR5_POS, UART_SRBR5_RSVD_SRBR5_LEN,value)


/* REGISTER: SRBR6 ACCESS: RW */

#if defined(_V1) && !defined(SRBR6_OFFSET)
#define SRBR6_OFFSET 0x48
#endif

#if !defined(UART_SRBR6_OFFSET)
#define UART_SRBR6_OFFSET 0x48
#endif

#if defined(_V1) && !defined(SRBR6_REG)
#define SRBR6_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR6_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR6_VAL)
#define SRBR6_VAL  PREFIX_VAL(SRBR6_REG)
#endif

#define UART2_SRBR6_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR6_OFFSET))
#define UART2_SRBR6_VAL  PREFIX_VAL(UART2_SRBR6_REG)

/* FIELDS: */

/* srbr6 ACCESS: RW */

#ifndef UART_SRBR6_SRBR6_POS
#define UART_SRBR6_SRBR6_POS      0
#endif

#ifndef UART_SRBR6_SRBR6_LEN
#define UART_SRBR6_SRBR6_LEN      8
#endif

#if defined(_V1) && !defined(SRBR6_SRBR6_R)
#define SRBR6_SRBR6_R        GetGroupBits32( (UART2_SRBR6_VAL),UART_SRBR6_SRBR6_POS, UART_SRBR6_SRBR6_LEN)
#endif

#if defined(_V1) && !defined(SRBR6_SRBR6_W)
#define SRBR6_SRBR6_W(value) SetGroupBits32( (UART2_SRBR6_VAL),UART_SRBR6_SRBR6_POS, UART_SRBR6_SRBR6_LEN,value)
#endif

#define UART2_SRBR6_SRBR6_R        GetGroupBits32( (UART2_SRBR6_VAL),UART_SRBR6_SRBR6_POS, UART_SRBR6_SRBR6_LEN)

#define UART2_SRBR6_SRBR6_W(value) SetGroupBits32( (UART2_SRBR6_VAL),UART_SRBR6_SRBR6_POS, UART_SRBR6_SRBR6_LEN,value)


/* RSVD_SRBR6 ACCESS: RW */

#ifndef UART_SRBR6_RSVD_SRBR6_POS
#define UART_SRBR6_RSVD_SRBR6_POS      8
#endif

#ifndef UART_SRBR6_RSVD_SRBR6_LEN
#define UART_SRBR6_RSVD_SRBR6_LEN      24
#endif

#if defined(_V1) && !defined(SRBR6_RSVD_SRBR6_R)
#define SRBR6_RSVD_SRBR6_R        GetGroupBits32( (UART2_SRBR6_VAL),UART_SRBR6_RSVD_SRBR6_POS, UART_SRBR6_RSVD_SRBR6_LEN)
#endif

#if defined(_V1) && !defined(SRBR6_RSVD_SRBR6_W)
#define SRBR6_RSVD_SRBR6_W(value) SetGroupBits32( (UART2_SRBR6_VAL),UART_SRBR6_RSVD_SRBR6_POS, UART_SRBR6_RSVD_SRBR6_LEN,value)
#endif

#define UART2_SRBR6_RSVD_SRBR6_R        GetGroupBits32( (UART2_SRBR6_VAL),UART_SRBR6_RSVD_SRBR6_POS, UART_SRBR6_RSVD_SRBR6_LEN)

#define UART2_SRBR6_RSVD_SRBR6_W(value) SetGroupBits32( (UART2_SRBR6_VAL),UART_SRBR6_RSVD_SRBR6_POS, UART_SRBR6_RSVD_SRBR6_LEN,value)


/* REGISTER: SRBR7 ACCESS: RW */

#if defined(_V1) && !defined(SRBR7_OFFSET)
#define SRBR7_OFFSET 0x4c
#endif

#if !defined(UART_SRBR7_OFFSET)
#define UART_SRBR7_OFFSET 0x4c
#endif

#if defined(_V1) && !defined(SRBR7_REG)
#define SRBR7_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR7_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR7_VAL)
#define SRBR7_VAL  PREFIX_VAL(SRBR7_REG)
#endif

#define UART2_SRBR7_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR7_OFFSET))
#define UART2_SRBR7_VAL  PREFIX_VAL(UART2_SRBR7_REG)

/* FIELDS: */

/* srbr7 ACCESS: RW */

#ifndef UART_SRBR7_SRBR7_POS
#define UART_SRBR7_SRBR7_POS      0
#endif

#ifndef UART_SRBR7_SRBR7_LEN
#define UART_SRBR7_SRBR7_LEN      8
#endif

#if defined(_V1) && !defined(SRBR7_SRBR7_R)
#define SRBR7_SRBR7_R        GetGroupBits32( (UART2_SRBR7_VAL),UART_SRBR7_SRBR7_POS, UART_SRBR7_SRBR7_LEN)
#endif

#if defined(_V1) && !defined(SRBR7_SRBR7_W)
#define SRBR7_SRBR7_W(value) SetGroupBits32( (UART2_SRBR7_VAL),UART_SRBR7_SRBR7_POS, UART_SRBR7_SRBR7_LEN,value)
#endif

#define UART2_SRBR7_SRBR7_R        GetGroupBits32( (UART2_SRBR7_VAL),UART_SRBR7_SRBR7_POS, UART_SRBR7_SRBR7_LEN)

#define UART2_SRBR7_SRBR7_W(value) SetGroupBits32( (UART2_SRBR7_VAL),UART_SRBR7_SRBR7_POS, UART_SRBR7_SRBR7_LEN,value)


/* RSVD_SRBR7 ACCESS: RW */

#ifndef UART_SRBR7_RSVD_SRBR7_POS
#define UART_SRBR7_RSVD_SRBR7_POS      8
#endif

#ifndef UART_SRBR7_RSVD_SRBR7_LEN
#define UART_SRBR7_RSVD_SRBR7_LEN      24
#endif

#if defined(_V1) && !defined(SRBR7_RSVD_SRBR7_R)
#define SRBR7_RSVD_SRBR7_R        GetGroupBits32( (UART2_SRBR7_VAL),UART_SRBR7_RSVD_SRBR7_POS, UART_SRBR7_RSVD_SRBR7_LEN)
#endif

#if defined(_V1) && !defined(SRBR7_RSVD_SRBR7_W)
#define SRBR7_RSVD_SRBR7_W(value) SetGroupBits32( (UART2_SRBR7_VAL),UART_SRBR7_RSVD_SRBR7_POS, UART_SRBR7_RSVD_SRBR7_LEN,value)
#endif

#define UART2_SRBR7_RSVD_SRBR7_R        GetGroupBits32( (UART2_SRBR7_VAL),UART_SRBR7_RSVD_SRBR7_POS, UART_SRBR7_RSVD_SRBR7_LEN)

#define UART2_SRBR7_RSVD_SRBR7_W(value) SetGroupBits32( (UART2_SRBR7_VAL),UART_SRBR7_RSVD_SRBR7_POS, UART_SRBR7_RSVD_SRBR7_LEN,value)


/* REGISTER: SRBR8 ACCESS: RW */

#if defined(_V1) && !defined(SRBR8_OFFSET)
#define SRBR8_OFFSET 0x50
#endif

#if !defined(UART_SRBR8_OFFSET)
#define UART_SRBR8_OFFSET 0x50
#endif

#if defined(_V1) && !defined(SRBR8_REG)
#define SRBR8_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR8_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR8_VAL)
#define SRBR8_VAL  PREFIX_VAL(SRBR8_REG)
#endif

#define UART2_SRBR8_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR8_OFFSET))
#define UART2_SRBR8_VAL  PREFIX_VAL(UART2_SRBR8_REG)

/* FIELDS: */

/* srbr8 ACCESS: RW */

#ifndef UART_SRBR8_SRBR8_POS
#define UART_SRBR8_SRBR8_POS      0
#endif

#ifndef UART_SRBR8_SRBR8_LEN
#define UART_SRBR8_SRBR8_LEN      8
#endif

#if defined(_V1) && !defined(SRBR8_SRBR8_R)
#define SRBR8_SRBR8_R        GetGroupBits32( (UART2_SRBR8_VAL),UART_SRBR8_SRBR8_POS, UART_SRBR8_SRBR8_LEN)
#endif

#if defined(_V1) && !defined(SRBR8_SRBR8_W)
#define SRBR8_SRBR8_W(value) SetGroupBits32( (UART2_SRBR8_VAL),UART_SRBR8_SRBR8_POS, UART_SRBR8_SRBR8_LEN,value)
#endif

#define UART2_SRBR8_SRBR8_R        GetGroupBits32( (UART2_SRBR8_VAL),UART_SRBR8_SRBR8_POS, UART_SRBR8_SRBR8_LEN)

#define UART2_SRBR8_SRBR8_W(value) SetGroupBits32( (UART2_SRBR8_VAL),UART_SRBR8_SRBR8_POS, UART_SRBR8_SRBR8_LEN,value)


/* RSVD_SRBR8 ACCESS: RW */

#ifndef UART_SRBR8_RSVD_SRBR8_POS
#define UART_SRBR8_RSVD_SRBR8_POS      8
#endif

#ifndef UART_SRBR8_RSVD_SRBR8_LEN
#define UART_SRBR8_RSVD_SRBR8_LEN      24
#endif

#if defined(_V1) && !defined(SRBR8_RSVD_SRBR8_R)
#define SRBR8_RSVD_SRBR8_R        GetGroupBits32( (UART2_SRBR8_VAL),UART_SRBR8_RSVD_SRBR8_POS, UART_SRBR8_RSVD_SRBR8_LEN)
#endif

#if defined(_V1) && !defined(SRBR8_RSVD_SRBR8_W)
#define SRBR8_RSVD_SRBR8_W(value) SetGroupBits32( (UART2_SRBR8_VAL),UART_SRBR8_RSVD_SRBR8_POS, UART_SRBR8_RSVD_SRBR8_LEN,value)
#endif

#define UART2_SRBR8_RSVD_SRBR8_R        GetGroupBits32( (UART2_SRBR8_VAL),UART_SRBR8_RSVD_SRBR8_POS, UART_SRBR8_RSVD_SRBR8_LEN)

#define UART2_SRBR8_RSVD_SRBR8_W(value) SetGroupBits32( (UART2_SRBR8_VAL),UART_SRBR8_RSVD_SRBR8_POS, UART_SRBR8_RSVD_SRBR8_LEN,value)


/* REGISTER: SRBR9 ACCESS: RW */

#if defined(_V1) && !defined(SRBR9_OFFSET)
#define SRBR9_OFFSET 0x54
#endif

#if !defined(UART_SRBR9_OFFSET)
#define UART_SRBR9_OFFSET 0x54
#endif

#if defined(_V1) && !defined(SRBR9_REG)
#define SRBR9_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR9_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR9_VAL)
#define SRBR9_VAL  PREFIX_VAL(SRBR9_REG)
#endif

#define UART2_SRBR9_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR9_OFFSET))
#define UART2_SRBR9_VAL  PREFIX_VAL(UART2_SRBR9_REG)

/* FIELDS: */

/* srbr9 ACCESS: RW */

#ifndef UART_SRBR9_SRBR9_POS
#define UART_SRBR9_SRBR9_POS      0
#endif

#ifndef UART_SRBR9_SRBR9_LEN
#define UART_SRBR9_SRBR9_LEN      8
#endif

#if defined(_V1) && !defined(SRBR9_SRBR9_R)
#define SRBR9_SRBR9_R        GetGroupBits32( (UART2_SRBR9_VAL),UART_SRBR9_SRBR9_POS, UART_SRBR9_SRBR9_LEN)
#endif

#if defined(_V1) && !defined(SRBR9_SRBR9_W)
#define SRBR9_SRBR9_W(value) SetGroupBits32( (UART2_SRBR9_VAL),UART_SRBR9_SRBR9_POS, UART_SRBR9_SRBR9_LEN,value)
#endif

#define UART2_SRBR9_SRBR9_R        GetGroupBits32( (UART2_SRBR9_VAL),UART_SRBR9_SRBR9_POS, UART_SRBR9_SRBR9_LEN)

#define UART2_SRBR9_SRBR9_W(value) SetGroupBits32( (UART2_SRBR9_VAL),UART_SRBR9_SRBR9_POS, UART_SRBR9_SRBR9_LEN,value)


/* RSVD_SRBR9 ACCESS: RW */

#ifndef UART_SRBR9_RSVD_SRBR9_POS
#define UART_SRBR9_RSVD_SRBR9_POS      8
#endif

#ifndef UART_SRBR9_RSVD_SRBR9_LEN
#define UART_SRBR9_RSVD_SRBR9_LEN      24
#endif

#if defined(_V1) && !defined(SRBR9_RSVD_SRBR9_R)
#define SRBR9_RSVD_SRBR9_R        GetGroupBits32( (UART2_SRBR9_VAL),UART_SRBR9_RSVD_SRBR9_POS, UART_SRBR9_RSVD_SRBR9_LEN)
#endif

#if defined(_V1) && !defined(SRBR9_RSVD_SRBR9_W)
#define SRBR9_RSVD_SRBR9_W(value) SetGroupBits32( (UART2_SRBR9_VAL),UART_SRBR9_RSVD_SRBR9_POS, UART_SRBR9_RSVD_SRBR9_LEN,value)
#endif

#define UART2_SRBR9_RSVD_SRBR9_R        GetGroupBits32( (UART2_SRBR9_VAL),UART_SRBR9_RSVD_SRBR9_POS, UART_SRBR9_RSVD_SRBR9_LEN)

#define UART2_SRBR9_RSVD_SRBR9_W(value) SetGroupBits32( (UART2_SRBR9_VAL),UART_SRBR9_RSVD_SRBR9_POS, UART_SRBR9_RSVD_SRBR9_LEN,value)


/* REGISTER: SRBR10 ACCESS: RW */

#if defined(_V1) && !defined(SRBR10_OFFSET)
#define SRBR10_OFFSET 0x58
#endif

#if !defined(UART_SRBR10_OFFSET)
#define UART_SRBR10_OFFSET 0x58
#endif

#if defined(_V1) && !defined(SRBR10_REG)
#define SRBR10_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR10_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR10_VAL)
#define SRBR10_VAL  PREFIX_VAL(SRBR10_REG)
#endif

#define UART2_SRBR10_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR10_OFFSET))
#define UART2_SRBR10_VAL  PREFIX_VAL(UART2_SRBR10_REG)

/* FIELDS: */

/* srbr10 ACCESS: RW */

#ifndef UART_SRBR10_SRBR10_POS
#define UART_SRBR10_SRBR10_POS      0
#endif

#ifndef UART_SRBR10_SRBR10_LEN
#define UART_SRBR10_SRBR10_LEN      8
#endif

#if defined(_V1) && !defined(SRBR10_SRBR10_R)
#define SRBR10_SRBR10_R        GetGroupBits32( (UART2_SRBR10_VAL),UART_SRBR10_SRBR10_POS, UART_SRBR10_SRBR10_LEN)
#endif

#if defined(_V1) && !defined(SRBR10_SRBR10_W)
#define SRBR10_SRBR10_W(value) SetGroupBits32( (UART2_SRBR10_VAL),UART_SRBR10_SRBR10_POS, UART_SRBR10_SRBR10_LEN,value)
#endif

#define UART2_SRBR10_SRBR10_R        GetGroupBits32( (UART2_SRBR10_VAL),UART_SRBR10_SRBR10_POS, UART_SRBR10_SRBR10_LEN)

#define UART2_SRBR10_SRBR10_W(value) SetGroupBits32( (UART2_SRBR10_VAL),UART_SRBR10_SRBR10_POS, UART_SRBR10_SRBR10_LEN,value)


/* RSVD_SRBR10 ACCESS: RW */

#ifndef UART_SRBR10_RSVD_SRBR10_POS
#define UART_SRBR10_RSVD_SRBR10_POS      8
#endif

#ifndef UART_SRBR10_RSVD_SRBR10_LEN
#define UART_SRBR10_RSVD_SRBR10_LEN      24
#endif

#if defined(_V1) && !defined(SRBR10_RSVD_SRBR10_R)
#define SRBR10_RSVD_SRBR10_R        GetGroupBits32( (UART2_SRBR10_VAL),UART_SRBR10_RSVD_SRBR10_POS, UART_SRBR10_RSVD_SRBR10_LEN)
#endif

#if defined(_V1) && !defined(SRBR10_RSVD_SRBR10_W)
#define SRBR10_RSVD_SRBR10_W(value) SetGroupBits32( (UART2_SRBR10_VAL),UART_SRBR10_RSVD_SRBR10_POS, UART_SRBR10_RSVD_SRBR10_LEN,value)
#endif

#define UART2_SRBR10_RSVD_SRBR10_R        GetGroupBits32( (UART2_SRBR10_VAL),UART_SRBR10_RSVD_SRBR10_POS, UART_SRBR10_RSVD_SRBR10_LEN)

#define UART2_SRBR10_RSVD_SRBR10_W(value) SetGroupBits32( (UART2_SRBR10_VAL),UART_SRBR10_RSVD_SRBR10_POS, UART_SRBR10_RSVD_SRBR10_LEN,value)


/* REGISTER: SRBR11 ACCESS: RW */

#if defined(_V1) && !defined(SRBR11_OFFSET)
#define SRBR11_OFFSET 0x5c
#endif

#if !defined(UART_SRBR11_OFFSET)
#define UART_SRBR11_OFFSET 0x5c
#endif

#if defined(_V1) && !defined(SRBR11_REG)
#define SRBR11_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR11_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR11_VAL)
#define SRBR11_VAL  PREFIX_VAL(SRBR11_REG)
#endif

#define UART2_SRBR11_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR11_OFFSET))
#define UART2_SRBR11_VAL  PREFIX_VAL(UART2_SRBR11_REG)

/* FIELDS: */

/* srbr11 ACCESS: RW */

#ifndef UART_SRBR11_SRBR11_POS
#define UART_SRBR11_SRBR11_POS      0
#endif

#ifndef UART_SRBR11_SRBR11_LEN
#define UART_SRBR11_SRBR11_LEN      8
#endif

#if defined(_V1) && !defined(SRBR11_SRBR11_R)
#define SRBR11_SRBR11_R        GetGroupBits32( (UART2_SRBR11_VAL),UART_SRBR11_SRBR11_POS, UART_SRBR11_SRBR11_LEN)
#endif

#if defined(_V1) && !defined(SRBR11_SRBR11_W)
#define SRBR11_SRBR11_W(value) SetGroupBits32( (UART2_SRBR11_VAL),UART_SRBR11_SRBR11_POS, UART_SRBR11_SRBR11_LEN,value)
#endif

#define UART2_SRBR11_SRBR11_R        GetGroupBits32( (UART2_SRBR11_VAL),UART_SRBR11_SRBR11_POS, UART_SRBR11_SRBR11_LEN)

#define UART2_SRBR11_SRBR11_W(value) SetGroupBits32( (UART2_SRBR11_VAL),UART_SRBR11_SRBR11_POS, UART_SRBR11_SRBR11_LEN,value)


/* RSVD_SRBR11 ACCESS: RW */

#ifndef UART_SRBR11_RSVD_SRBR11_POS
#define UART_SRBR11_RSVD_SRBR11_POS      8
#endif

#ifndef UART_SRBR11_RSVD_SRBR11_LEN
#define UART_SRBR11_RSVD_SRBR11_LEN      24
#endif

#if defined(_V1) && !defined(SRBR11_RSVD_SRBR11_R)
#define SRBR11_RSVD_SRBR11_R        GetGroupBits32( (UART2_SRBR11_VAL),UART_SRBR11_RSVD_SRBR11_POS, UART_SRBR11_RSVD_SRBR11_LEN)
#endif

#if defined(_V1) && !defined(SRBR11_RSVD_SRBR11_W)
#define SRBR11_RSVD_SRBR11_W(value) SetGroupBits32( (UART2_SRBR11_VAL),UART_SRBR11_RSVD_SRBR11_POS, UART_SRBR11_RSVD_SRBR11_LEN,value)
#endif

#define UART2_SRBR11_RSVD_SRBR11_R        GetGroupBits32( (UART2_SRBR11_VAL),UART_SRBR11_RSVD_SRBR11_POS, UART_SRBR11_RSVD_SRBR11_LEN)

#define UART2_SRBR11_RSVD_SRBR11_W(value) SetGroupBits32( (UART2_SRBR11_VAL),UART_SRBR11_RSVD_SRBR11_POS, UART_SRBR11_RSVD_SRBR11_LEN,value)


/* REGISTER: SRBR12 ACCESS: RW */

#if defined(_V1) && !defined(SRBR12_OFFSET)
#define SRBR12_OFFSET 0x60
#endif

#if !defined(UART_SRBR12_OFFSET)
#define UART_SRBR12_OFFSET 0x60
#endif

#if defined(_V1) && !defined(SRBR12_REG)
#define SRBR12_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR12_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR12_VAL)
#define SRBR12_VAL  PREFIX_VAL(SRBR12_REG)
#endif

#define UART2_SRBR12_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR12_OFFSET))
#define UART2_SRBR12_VAL  PREFIX_VAL(UART2_SRBR12_REG)

/* FIELDS: */

/* srbr12 ACCESS: RW */

#ifndef UART_SRBR12_SRBR12_POS
#define UART_SRBR12_SRBR12_POS      0
#endif

#ifndef UART_SRBR12_SRBR12_LEN
#define UART_SRBR12_SRBR12_LEN      8
#endif

#if defined(_V1) && !defined(SRBR12_SRBR12_R)
#define SRBR12_SRBR12_R        GetGroupBits32( (UART2_SRBR12_VAL),UART_SRBR12_SRBR12_POS, UART_SRBR12_SRBR12_LEN)
#endif

#if defined(_V1) && !defined(SRBR12_SRBR12_W)
#define SRBR12_SRBR12_W(value) SetGroupBits32( (UART2_SRBR12_VAL),UART_SRBR12_SRBR12_POS, UART_SRBR12_SRBR12_LEN,value)
#endif

#define UART2_SRBR12_SRBR12_R        GetGroupBits32( (UART2_SRBR12_VAL),UART_SRBR12_SRBR12_POS, UART_SRBR12_SRBR12_LEN)

#define UART2_SRBR12_SRBR12_W(value) SetGroupBits32( (UART2_SRBR12_VAL),UART_SRBR12_SRBR12_POS, UART_SRBR12_SRBR12_LEN,value)


/* RSVD_SRBR12 ACCESS: RW */

#ifndef UART_SRBR12_RSVD_SRBR12_POS
#define UART_SRBR12_RSVD_SRBR12_POS      8
#endif

#ifndef UART_SRBR12_RSVD_SRBR12_LEN
#define UART_SRBR12_RSVD_SRBR12_LEN      24
#endif

#if defined(_V1) && !defined(SRBR12_RSVD_SRBR12_R)
#define SRBR12_RSVD_SRBR12_R        GetGroupBits32( (UART2_SRBR12_VAL),UART_SRBR12_RSVD_SRBR12_POS, UART_SRBR12_RSVD_SRBR12_LEN)
#endif

#if defined(_V1) && !defined(SRBR12_RSVD_SRBR12_W)
#define SRBR12_RSVD_SRBR12_W(value) SetGroupBits32( (UART2_SRBR12_VAL),UART_SRBR12_RSVD_SRBR12_POS, UART_SRBR12_RSVD_SRBR12_LEN,value)
#endif

#define UART2_SRBR12_RSVD_SRBR12_R        GetGroupBits32( (UART2_SRBR12_VAL),UART_SRBR12_RSVD_SRBR12_POS, UART_SRBR12_RSVD_SRBR12_LEN)

#define UART2_SRBR12_RSVD_SRBR12_W(value) SetGroupBits32( (UART2_SRBR12_VAL),UART_SRBR12_RSVD_SRBR12_POS, UART_SRBR12_RSVD_SRBR12_LEN,value)


/* REGISTER: SRBR13 ACCESS: RW */

#if defined(_V1) && !defined(SRBR13_OFFSET)
#define SRBR13_OFFSET 0x64
#endif

#if !defined(UART_SRBR13_OFFSET)
#define UART_SRBR13_OFFSET 0x64
#endif

#if defined(_V1) && !defined(SRBR13_REG)
#define SRBR13_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR13_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR13_VAL)
#define SRBR13_VAL  PREFIX_VAL(SRBR13_REG)
#endif

#define UART2_SRBR13_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR13_OFFSET))
#define UART2_SRBR13_VAL  PREFIX_VAL(UART2_SRBR13_REG)

/* FIELDS: */

/* srbr13 ACCESS: RW */

#ifndef UART_SRBR13_SRBR13_POS
#define UART_SRBR13_SRBR13_POS      0
#endif

#ifndef UART_SRBR13_SRBR13_LEN
#define UART_SRBR13_SRBR13_LEN      8
#endif

#if defined(_V1) && !defined(SRBR13_SRBR13_R)
#define SRBR13_SRBR13_R        GetGroupBits32( (UART2_SRBR13_VAL),UART_SRBR13_SRBR13_POS, UART_SRBR13_SRBR13_LEN)
#endif

#if defined(_V1) && !defined(SRBR13_SRBR13_W)
#define SRBR13_SRBR13_W(value) SetGroupBits32( (UART2_SRBR13_VAL),UART_SRBR13_SRBR13_POS, UART_SRBR13_SRBR13_LEN,value)
#endif

#define UART2_SRBR13_SRBR13_R        GetGroupBits32( (UART2_SRBR13_VAL),UART_SRBR13_SRBR13_POS, UART_SRBR13_SRBR13_LEN)

#define UART2_SRBR13_SRBR13_W(value) SetGroupBits32( (UART2_SRBR13_VAL),UART_SRBR13_SRBR13_POS, UART_SRBR13_SRBR13_LEN,value)


/* RSVD_SRBR13 ACCESS: RW */

#ifndef UART_SRBR13_RSVD_SRBR13_POS
#define UART_SRBR13_RSVD_SRBR13_POS      8
#endif

#ifndef UART_SRBR13_RSVD_SRBR13_LEN
#define UART_SRBR13_RSVD_SRBR13_LEN      24
#endif

#if defined(_V1) && !defined(SRBR13_RSVD_SRBR13_R)
#define SRBR13_RSVD_SRBR13_R        GetGroupBits32( (UART2_SRBR13_VAL),UART_SRBR13_RSVD_SRBR13_POS, UART_SRBR13_RSVD_SRBR13_LEN)
#endif

#if defined(_V1) && !defined(SRBR13_RSVD_SRBR13_W)
#define SRBR13_RSVD_SRBR13_W(value) SetGroupBits32( (UART2_SRBR13_VAL),UART_SRBR13_RSVD_SRBR13_POS, UART_SRBR13_RSVD_SRBR13_LEN,value)
#endif

#define UART2_SRBR13_RSVD_SRBR13_R        GetGroupBits32( (UART2_SRBR13_VAL),UART_SRBR13_RSVD_SRBR13_POS, UART_SRBR13_RSVD_SRBR13_LEN)

#define UART2_SRBR13_RSVD_SRBR13_W(value) SetGroupBits32( (UART2_SRBR13_VAL),UART_SRBR13_RSVD_SRBR13_POS, UART_SRBR13_RSVD_SRBR13_LEN,value)


/* REGISTER: SRBR14 ACCESS: RW */

#if defined(_V1) && !defined(SRBR14_OFFSET)
#define SRBR14_OFFSET 0x68
#endif

#if !defined(UART_SRBR14_OFFSET)
#define UART_SRBR14_OFFSET 0x68
#endif

#if defined(_V1) && !defined(SRBR14_REG)
#define SRBR14_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR14_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR14_VAL)
#define SRBR14_VAL  PREFIX_VAL(SRBR14_REG)
#endif

#define UART2_SRBR14_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR14_OFFSET))
#define UART2_SRBR14_VAL  PREFIX_VAL(UART2_SRBR14_REG)

/* FIELDS: */

/* srbr14 ACCESS: RW */

#ifndef UART_SRBR14_SRBR14_POS
#define UART_SRBR14_SRBR14_POS      0
#endif

#ifndef UART_SRBR14_SRBR14_LEN
#define UART_SRBR14_SRBR14_LEN      8
#endif

#if defined(_V1) && !defined(SRBR14_SRBR14_R)
#define SRBR14_SRBR14_R        GetGroupBits32( (UART2_SRBR14_VAL),UART_SRBR14_SRBR14_POS, UART_SRBR14_SRBR14_LEN)
#endif

#if defined(_V1) && !defined(SRBR14_SRBR14_W)
#define SRBR14_SRBR14_W(value) SetGroupBits32( (UART2_SRBR14_VAL),UART_SRBR14_SRBR14_POS, UART_SRBR14_SRBR14_LEN,value)
#endif

#define UART2_SRBR14_SRBR14_R        GetGroupBits32( (UART2_SRBR14_VAL),UART_SRBR14_SRBR14_POS, UART_SRBR14_SRBR14_LEN)

#define UART2_SRBR14_SRBR14_W(value) SetGroupBits32( (UART2_SRBR14_VAL),UART_SRBR14_SRBR14_POS, UART_SRBR14_SRBR14_LEN,value)


/* RSVD_SRBR14 ACCESS: RW */

#ifndef UART_SRBR14_RSVD_SRBR14_POS
#define UART_SRBR14_RSVD_SRBR14_POS      8
#endif

#ifndef UART_SRBR14_RSVD_SRBR14_LEN
#define UART_SRBR14_RSVD_SRBR14_LEN      24
#endif

#if defined(_V1) && !defined(SRBR14_RSVD_SRBR14_R)
#define SRBR14_RSVD_SRBR14_R        GetGroupBits32( (UART2_SRBR14_VAL),UART_SRBR14_RSVD_SRBR14_POS, UART_SRBR14_RSVD_SRBR14_LEN)
#endif

#if defined(_V1) && !defined(SRBR14_RSVD_SRBR14_W)
#define SRBR14_RSVD_SRBR14_W(value) SetGroupBits32( (UART2_SRBR14_VAL),UART_SRBR14_RSVD_SRBR14_POS, UART_SRBR14_RSVD_SRBR14_LEN,value)
#endif

#define UART2_SRBR14_RSVD_SRBR14_R        GetGroupBits32( (UART2_SRBR14_VAL),UART_SRBR14_RSVD_SRBR14_POS, UART_SRBR14_RSVD_SRBR14_LEN)

#define UART2_SRBR14_RSVD_SRBR14_W(value) SetGroupBits32( (UART2_SRBR14_VAL),UART_SRBR14_RSVD_SRBR14_POS, UART_SRBR14_RSVD_SRBR14_LEN,value)


/* REGISTER: SRBR15 ACCESS: RW */

#if defined(_V1) && !defined(SRBR15_OFFSET)
#define SRBR15_OFFSET 0x6c
#endif

#if !defined(UART_SRBR15_OFFSET)
#define UART_SRBR15_OFFSET 0x6c
#endif

#if defined(_V1) && !defined(SRBR15_REG)
#define SRBR15_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR15_OFFSET))
#endif

#if defined(_V1) && !defined(SRBR15_VAL)
#define SRBR15_VAL  PREFIX_VAL(SRBR15_REG)
#endif

#define UART2_SRBR15_REG ((volatile UINT32 *) (UART2_BASE + UART_SRBR15_OFFSET))
#define UART2_SRBR15_VAL  PREFIX_VAL(UART2_SRBR15_REG)

/* FIELDS: */

/* srbr15 ACCESS: RW */

#ifndef UART_SRBR15_SRBR15_POS
#define UART_SRBR15_SRBR15_POS      0
#endif

#ifndef UART_SRBR15_SRBR15_LEN
#define UART_SRBR15_SRBR15_LEN      8
#endif

#if defined(_V1) && !defined(SRBR15_SRBR15_R)
#define SRBR15_SRBR15_R        GetGroupBits32( (UART2_SRBR15_VAL),UART_SRBR15_SRBR15_POS, UART_SRBR15_SRBR15_LEN)
#endif

#if defined(_V1) && !defined(SRBR15_SRBR15_W)
#define SRBR15_SRBR15_W(value) SetGroupBits32( (UART2_SRBR15_VAL),UART_SRBR15_SRBR15_POS, UART_SRBR15_SRBR15_LEN,value)
#endif

#define UART2_SRBR15_SRBR15_R        GetGroupBits32( (UART2_SRBR15_VAL),UART_SRBR15_SRBR15_POS, UART_SRBR15_SRBR15_LEN)

#define UART2_SRBR15_SRBR15_W(value) SetGroupBits32( (UART2_SRBR15_VAL),UART_SRBR15_SRBR15_POS, UART_SRBR15_SRBR15_LEN,value)


/* RSVD_SRBR15 ACCESS: RW */

#ifndef UART_SRBR15_RSVD_SRBR15_POS
#define UART_SRBR15_RSVD_SRBR15_POS      8
#endif

#ifndef UART_SRBR15_RSVD_SRBR15_LEN
#define UART_SRBR15_RSVD_SRBR15_LEN      24
#endif

#if defined(_V1) && !defined(SRBR15_RSVD_SRBR15_R)
#define SRBR15_RSVD_SRBR15_R        GetGroupBits32( (UART2_SRBR15_VAL),UART_SRBR15_RSVD_SRBR15_POS, UART_SRBR15_RSVD_SRBR15_LEN)
#endif

#if defined(_V1) && !defined(SRBR15_RSVD_SRBR15_W)
#define SRBR15_RSVD_SRBR15_W(value) SetGroupBits32( (UART2_SRBR15_VAL),UART_SRBR15_RSVD_SRBR15_POS, UART_SRBR15_RSVD_SRBR15_LEN,value)
#endif

#define UART2_SRBR15_RSVD_SRBR15_R        GetGroupBits32( (UART2_SRBR15_VAL),UART_SRBR15_RSVD_SRBR15_POS, UART_SRBR15_RSVD_SRBR15_LEN)

#define UART2_SRBR15_RSVD_SRBR15_W(value) SetGroupBits32( (UART2_SRBR15_VAL),UART_SRBR15_RSVD_SRBR15_POS, UART_SRBR15_RSVD_SRBR15_LEN,value)


/* REGISTER: FAR ACCESS: RW */

#if defined(_V1) && !defined(FAR_OFFSET)
#define FAR_OFFSET 0x70
#endif

#if !defined(UART_FAR_OFFSET)
#define UART_FAR_OFFSET 0x70
#endif

#if defined(_V1) && !defined(FAR_REG)
#define FAR_REG ((volatile UINT32 *) (UART2_BASE + UART_FAR_OFFSET))
#endif

#if defined(_V1) && !defined(FAR_VAL)
#define FAR_VAL  PREFIX_VAL(FAR_REG)
#endif

#define UART2_FAR_REG ((volatile UINT32 *) (UART2_BASE + UART_FAR_OFFSET))
#define UART2_FAR_VAL  PREFIX_VAL(UART2_FAR_REG)

/* FIELDS: */

/* far ACCESS: RW */

#ifndef UART_FAR_FAR_POS
#define UART_FAR_FAR_POS      0
#endif

#ifndef UART_FAR_FAR_LEN
#define UART_FAR_FAR_LEN      1
#endif

#if defined(_V1) && !defined(FAR_FAR_R)
#define FAR_FAR_R        GetGroupBits32( (UART2_FAR_VAL),UART_FAR_FAR_POS, UART_FAR_FAR_LEN)
#endif

#if defined(_V1) && !defined(FAR_FAR_W)
#define FAR_FAR_W(value) SetGroupBits32( (UART2_FAR_VAL),UART_FAR_FAR_POS, UART_FAR_FAR_LEN,value)
#endif

#define UART2_FAR_FAR_R        GetGroupBits32( (UART2_FAR_VAL),UART_FAR_FAR_POS, UART_FAR_FAR_LEN)

#define UART2_FAR_FAR_W(value) SetGroupBits32( (UART2_FAR_VAL),UART_FAR_FAR_POS, UART_FAR_FAR_LEN,value)


/* RSVD_FAR_31to1 ACCESS: RW */

#ifndef UART_FAR_RSVD_FAR_31TO1_POS
#define UART_FAR_RSVD_FAR_31TO1_POS      1
#endif

#ifndef UART_FAR_RSVD_FAR_31TO1_LEN
#define UART_FAR_RSVD_FAR_31TO1_LEN      31
#endif

#if defined(_V1) && !defined(FAR_RSVD_FAR_31TO1_R)
#define FAR_RSVD_FAR_31TO1_R        GetGroupBits32( (UART2_FAR_VAL),UART_FAR_RSVD_FAR_31TO1_POS, UART_FAR_RSVD_FAR_31TO1_LEN)
#endif

#if defined(_V1) && !defined(FAR_RSVD_FAR_31TO1_W)
#define FAR_RSVD_FAR_31TO1_W(value) SetGroupBits32( (UART2_FAR_VAL),UART_FAR_RSVD_FAR_31TO1_POS, UART_FAR_RSVD_FAR_31TO1_LEN,value)
#endif

#define UART2_FAR_RSVD_FAR_31TO1_R        GetGroupBits32( (UART2_FAR_VAL),UART_FAR_RSVD_FAR_31TO1_POS, UART_FAR_RSVD_FAR_31TO1_LEN)

#define UART2_FAR_RSVD_FAR_31TO1_W(value) SetGroupBits32( (UART2_FAR_VAL),UART_FAR_RSVD_FAR_31TO1_POS, UART_FAR_RSVD_FAR_31TO1_LEN,value)


/* REGISTER: TFR ACCESS: RW */

#if defined(_V1) && !defined(TFR_OFFSET)
#define TFR_OFFSET 0x74
#endif

#if !defined(UART_TFR_OFFSET)
#define UART_TFR_OFFSET 0x74
#endif

#if defined(_V1) && !defined(TFR_REG)
#define TFR_REG ((volatile UINT32 *) (UART2_BASE + UART_TFR_OFFSET))
#endif

#if defined(_V1) && !defined(TFR_VAL)
#define TFR_VAL  PREFIX_VAL(TFR_REG)
#endif

#define UART2_TFR_REG ((volatile UINT32 *) (UART2_BASE + UART_TFR_OFFSET))
#define UART2_TFR_VAL  PREFIX_VAL(UART2_TFR_REG)

/* FIELDS: */

/* tfr ACCESS: RW */

#ifndef UART_TFR_TFR_POS
#define UART_TFR_TFR_POS      0
#endif

#ifndef UART_TFR_TFR_LEN
#define UART_TFR_TFR_LEN      8
#endif

#if defined(_V1) && !defined(TFR_TFR_R)
#define TFR_TFR_R        GetGroupBits32( (UART2_TFR_VAL),UART_TFR_TFR_POS, UART_TFR_TFR_LEN)
#endif

#if defined(_V1) && !defined(TFR_TFR_W)
#define TFR_TFR_W(value) SetGroupBits32( (UART2_TFR_VAL),UART_TFR_TFR_POS, UART_TFR_TFR_LEN,value)
#endif

#define UART2_TFR_TFR_R        GetGroupBits32( (UART2_TFR_VAL),UART_TFR_TFR_POS, UART_TFR_TFR_LEN)

#define UART2_TFR_TFR_W(value) SetGroupBits32( (UART2_TFR_VAL),UART_TFR_TFR_POS, UART_TFR_TFR_LEN,value)


/* RSVD_TFR_31to8 ACCESS: RW */

#ifndef UART_TFR_RSVD_TFR_31TO8_POS
#define UART_TFR_RSVD_TFR_31TO8_POS      8
#endif

#ifndef UART_TFR_RSVD_TFR_31TO8_LEN
#define UART_TFR_RSVD_TFR_31TO8_LEN      24
#endif

#if defined(_V1) && !defined(TFR_RSVD_TFR_31TO8_R)
#define TFR_RSVD_TFR_31TO8_R        GetGroupBits32( (UART2_TFR_VAL),UART_TFR_RSVD_TFR_31TO8_POS, UART_TFR_RSVD_TFR_31TO8_LEN)
#endif

#if defined(_V1) && !defined(TFR_RSVD_TFR_31TO8_W)
#define TFR_RSVD_TFR_31TO8_W(value) SetGroupBits32( (UART2_TFR_VAL),UART_TFR_RSVD_TFR_31TO8_POS, UART_TFR_RSVD_TFR_31TO8_LEN,value)
#endif

#define UART2_TFR_RSVD_TFR_31TO8_R        GetGroupBits32( (UART2_TFR_VAL),UART_TFR_RSVD_TFR_31TO8_POS, UART_TFR_RSVD_TFR_31TO8_LEN)

#define UART2_TFR_RSVD_TFR_31TO8_W(value) SetGroupBits32( (UART2_TFR_VAL),UART_TFR_RSVD_TFR_31TO8_POS, UART_TFR_RSVD_TFR_31TO8_LEN,value)


/* REGISTER: RFW ACCESS: RW */

#if defined(_V1) && !defined(RFW_OFFSET)
#define RFW_OFFSET 0x78
#endif

#if !defined(UART_RFW_OFFSET)
#define UART_RFW_OFFSET 0x78
#endif

#if defined(_V1) && !defined(RFW_REG)
#define RFW_REG ((volatile UINT32 *) (UART2_BASE + UART_RFW_OFFSET))
#endif

#if defined(_V1) && !defined(RFW_VAL)
#define RFW_VAL  PREFIX_VAL(RFW_REG)
#endif

#define UART2_RFW_REG ((volatile UINT32 *) (UART2_BASE + UART_RFW_OFFSET))
#define UART2_RFW_VAL  PREFIX_VAL(UART2_RFW_REG)

/* FIELDS: */

/* RFWD ACCESS: RW */

#ifndef UART_RFW_RFWD_POS
#define UART_RFW_RFWD_POS      0
#endif

#ifndef UART_RFW_RFWD_LEN
#define UART_RFW_RFWD_LEN      8
#endif

#if defined(_V1) && !defined(RFW_RFWD_R)
#define RFW_RFWD_R        GetGroupBits32( (UART2_RFW_VAL),UART_RFW_RFWD_POS, UART_RFW_RFWD_LEN)
#endif

#if defined(_V1) && !defined(RFW_RFWD_W)
#define RFW_RFWD_W(value) SetGroupBits32( (UART2_RFW_VAL),UART_RFW_RFWD_POS, UART_RFW_RFWD_LEN,value)
#endif

#define UART2_RFW_RFWD_R        GetGroupBits32( (UART2_RFW_VAL),UART_RFW_RFWD_POS, UART_RFW_RFWD_LEN)

#define UART2_RFW_RFWD_W(value) SetGroupBits32( (UART2_RFW_VAL),UART_RFW_RFWD_POS, UART_RFW_RFWD_LEN,value)


/* RFPE ACCESS: RW */

#ifndef UART_RFW_RFPE_POS
#define UART_RFW_RFPE_POS      8
#endif

#ifndef UART_RFW_RFPE_LEN
#define UART_RFW_RFPE_LEN      1
#endif

#if defined(_V1) && !defined(RFW_RFPE_R)
#define RFW_RFPE_R        GetGroupBits32( (UART2_RFW_VAL),UART_RFW_RFPE_POS, UART_RFW_RFPE_LEN)
#endif

#if defined(_V1) && !defined(RFW_RFPE_W)
#define RFW_RFPE_W(value) SetGroupBits32( (UART2_RFW_VAL),UART_RFW_RFPE_POS, UART_RFW_RFPE_LEN,value)
#endif

#define UART2_RFW_RFPE_R        GetGroupBits32( (UART2_RFW_VAL),UART_RFW_RFPE_POS, UART_RFW_RFPE_LEN)

#define UART2_RFW_RFPE_W(value) SetGroupBits32( (UART2_RFW_VAL),UART_RFW_RFPE_POS, UART_RFW_RFPE_LEN,value)


/* RFFE ACCESS: RW */

#ifndef UART_RFW_RFFE_POS
#define UART_RFW_RFFE_POS      9
#endif

#ifndef UART_RFW_RFFE_LEN
#define UART_RFW_RFFE_LEN      1
#endif

#if defined(_V1) && !defined(RFW_RFFE_R)
#define RFW_RFFE_R        GetGroupBits32( (UART2_RFW_VAL),UART_RFW_RFFE_POS, UART_RFW_RFFE_LEN)
#endif

#if defined(_V1) && !defined(RFW_RFFE_W)
#define RFW_RFFE_W(value) SetGroupBits32( (UART2_RFW_VAL),UART_RFW_RFFE_POS, UART_RFW_RFFE_LEN,value)
#endif

#define UART2_RFW_RFFE_R        GetGroupBits32( (UART2_RFW_VAL),UART_RFW_RFFE_POS, UART_RFW_RFFE_LEN)

#define UART2_RFW_RFFE_W(value) SetGroupBits32( (UART2_RFW_VAL),UART_RFW_RFFE_POS, UART_RFW_RFFE_LEN,value)


/* RSVD_RFW_31to10 ACCESS: RW */

#ifndef UART_RFW_RSVD_RFW_31TO10_POS
#define UART_RFW_RSVD_RFW_31TO10_POS      10
#endif

#ifndef UART_RFW_RSVD_RFW_31TO10_LEN
#define UART_RFW_RSVD_RFW_31TO10_LEN      22
#endif

#if defined(_V1) && !defined(RFW_RSVD_RFW_31TO10_R)
#define RFW_RSVD_RFW_31TO10_R        GetGroupBits32( (UART2_RFW_VAL),UART_RFW_RSVD_RFW_31TO10_POS, UART_RFW_RSVD_RFW_31TO10_LEN)
#endif

#if defined(_V1) && !defined(RFW_RSVD_RFW_31TO10_W)
#define RFW_RSVD_RFW_31TO10_W(value) SetGroupBits32( (UART2_RFW_VAL),UART_RFW_RSVD_RFW_31TO10_POS, UART_RFW_RSVD_RFW_31TO10_LEN,value)
#endif

#define UART2_RFW_RSVD_RFW_31TO10_R        GetGroupBits32( (UART2_RFW_VAL),UART_RFW_RSVD_RFW_31TO10_POS, UART_RFW_RSVD_RFW_31TO10_LEN)

#define UART2_RFW_RSVD_RFW_31TO10_W(value) SetGroupBits32( (UART2_RFW_VAL),UART_RFW_RSVD_RFW_31TO10_POS, UART_RFW_RSVD_RFW_31TO10_LEN,value)


/* REGISTER: USR ACCESS: RW */

#if defined(_V1) && !defined(USR_OFFSET)
#define USR_OFFSET 0x7c
#endif

#if !defined(UART_USR_OFFSET)
#define UART_USR_OFFSET 0x7c
#endif

#if defined(_V1) && !defined(USR_REG)
#define USR_REG ((volatile UINT32 *) (UART2_BASE + UART_USR_OFFSET))
#endif

#if defined(_V1) && !defined(USR_VAL)
#define USR_VAL  PREFIX_VAL(USR_REG)
#endif

#define UART2_USR_REG ((volatile UINT32 *) (UART2_BASE + UART_USR_OFFSET))
#define UART2_USR_VAL  PREFIX_VAL(UART2_USR_REG)

/* FIELDS: */

/* BUSY ACCESS: RW */

#ifndef UART_USR_BUSY_POS
#define UART_USR_BUSY_POS      0
#endif

#ifndef UART_USR_BUSY_LEN
#define UART_USR_BUSY_LEN      1
#endif

#if defined(_V1) && !defined(USR_BUSY_R)
#define USR_BUSY_R        GetGroupBits32( (UART2_USR_VAL),UART_USR_BUSY_POS, UART_USR_BUSY_LEN)
#endif

#if defined(_V1) && !defined(USR_BUSY_W)
#define USR_BUSY_W(value) SetGroupBits32( (UART2_USR_VAL),UART_USR_BUSY_POS, UART_USR_BUSY_LEN,value)
#endif

#define UART2_USR_BUSY_R        GetGroupBits32( (UART2_USR_VAL),UART_USR_BUSY_POS, UART_USR_BUSY_LEN)

#define UART2_USR_BUSY_W(value) SetGroupBits32( (UART2_USR_VAL),UART_USR_BUSY_POS, UART_USR_BUSY_LEN,value)


/* TFNF ACCESS: RW */

#ifndef UART_USR_TFNF_POS
#define UART_USR_TFNF_POS      1
#endif

#ifndef UART_USR_TFNF_LEN
#define UART_USR_TFNF_LEN      1
#endif

#if defined(_V1) && !defined(USR_TFNF_R)
#define USR_TFNF_R        GetGroupBits32( (UART2_USR_VAL),UART_USR_TFNF_POS, UART_USR_TFNF_LEN)
#endif

#if defined(_V1) && !defined(USR_TFNF_W)
#define USR_TFNF_W(value) SetGroupBits32( (UART2_USR_VAL),UART_USR_TFNF_POS, UART_USR_TFNF_LEN,value)
#endif

#define UART2_USR_TFNF_R        GetGroupBits32( (UART2_USR_VAL),UART_USR_TFNF_POS, UART_USR_TFNF_LEN)

#define UART2_USR_TFNF_W(value) SetGroupBits32( (UART2_USR_VAL),UART_USR_TFNF_POS, UART_USR_TFNF_LEN,value)


/* TFE ACCESS: RW */

#ifndef UART_USR_TFE_POS
#define UART_USR_TFE_POS      2
#endif

#ifndef UART_USR_TFE_LEN
#define UART_USR_TFE_LEN      1
#endif

#if defined(_V1) && !defined(USR_TFE_R)
#define USR_TFE_R        GetGroupBits32( (UART2_USR_VAL),UART_USR_TFE_POS, UART_USR_TFE_LEN)
#endif

#if defined(_V1) && !defined(USR_TFE_W)
#define USR_TFE_W(value) SetGroupBits32( (UART2_USR_VAL),UART_USR_TFE_POS, UART_USR_TFE_LEN,value)
#endif

#define UART2_USR_TFE_R        GetGroupBits32( (UART2_USR_VAL),UART_USR_TFE_POS, UART_USR_TFE_LEN)

#define UART2_USR_TFE_W(value) SetGroupBits32( (UART2_USR_VAL),UART_USR_TFE_POS, UART_USR_TFE_LEN,value)


/* RFNE ACCESS: RW */

#ifndef UART_USR_RFNE_POS
#define UART_USR_RFNE_POS      3
#endif

#ifndef UART_USR_RFNE_LEN
#define UART_USR_RFNE_LEN      1
#endif

#if defined(_V1) && !defined(USR_RFNE_R)
#define USR_RFNE_R        GetGroupBits32( (UART2_USR_VAL),UART_USR_RFNE_POS, UART_USR_RFNE_LEN)
#endif

#if defined(_V1) && !defined(USR_RFNE_W)
#define USR_RFNE_W(value) SetGroupBits32( (UART2_USR_VAL),UART_USR_RFNE_POS, UART_USR_RFNE_LEN,value)
#endif

#define UART2_USR_RFNE_R        GetGroupBits32( (UART2_USR_VAL),UART_USR_RFNE_POS, UART_USR_RFNE_LEN)

#define UART2_USR_RFNE_W(value) SetGroupBits32( (UART2_USR_VAL),UART_USR_RFNE_POS, UART_USR_RFNE_LEN,value)


/* RFF ACCESS: RW */

#ifndef UART_USR_RFF_POS
#define UART_USR_RFF_POS      4
#endif

#ifndef UART_USR_RFF_LEN
#define UART_USR_RFF_LEN      1
#endif

#if defined(_V1) && !defined(USR_RFF_R)
#define USR_RFF_R        GetGroupBits32( (UART2_USR_VAL),UART_USR_RFF_POS, UART_USR_RFF_LEN)
#endif

#if defined(_V1) && !defined(USR_RFF_W)
#define USR_RFF_W(value) SetGroupBits32( (UART2_USR_VAL),UART_USR_RFF_POS, UART_USR_RFF_LEN,value)
#endif

#define UART2_USR_RFF_R        GetGroupBits32( (UART2_USR_VAL),UART_USR_RFF_POS, UART_USR_RFF_LEN)

#define UART2_USR_RFF_W(value) SetGroupBits32( (UART2_USR_VAL),UART_USR_RFF_POS, UART_USR_RFF_LEN,value)


/* RSVD_USR_31to5 ACCESS: RW */

#ifndef UART_USR_RSVD_USR_31TO5_POS
#define UART_USR_RSVD_USR_31TO5_POS      5
#endif

#ifndef UART_USR_RSVD_USR_31TO5_LEN
#define UART_USR_RSVD_USR_31TO5_LEN      27
#endif

#if defined(_V1) && !defined(USR_RSVD_USR_31TO5_R)
#define USR_RSVD_USR_31TO5_R        GetGroupBits32( (UART2_USR_VAL),UART_USR_RSVD_USR_31TO5_POS, UART_USR_RSVD_USR_31TO5_LEN)
#endif

#if defined(_V1) && !defined(USR_RSVD_USR_31TO5_W)
#define USR_RSVD_USR_31TO5_W(value) SetGroupBits32( (UART2_USR_VAL),UART_USR_RSVD_USR_31TO5_POS, UART_USR_RSVD_USR_31TO5_LEN,value)
#endif

#define UART2_USR_RSVD_USR_31TO5_R        GetGroupBits32( (UART2_USR_VAL),UART_USR_RSVD_USR_31TO5_POS, UART_USR_RSVD_USR_31TO5_LEN)

#define UART2_USR_RSVD_USR_31TO5_W(value) SetGroupBits32( (UART2_USR_VAL),UART_USR_RSVD_USR_31TO5_POS, UART_USR_RSVD_USR_31TO5_LEN,value)


/* REGISTER: IIR ACCESS: RW */

#if defined(_V1) && !defined(IIR_OFFSET)
#define IIR_OFFSET 0x8
#endif

#if !defined(UART_IIR_OFFSET)
#define UART_IIR_OFFSET 0x8
#endif

#if defined(_V1) && !defined(IIR_REG)
#define IIR_REG ((volatile UINT32 *) (UART2_BASE + UART_IIR_OFFSET))
#endif

#if defined(_V1) && !defined(IIR_VAL)
#define IIR_VAL  PREFIX_VAL(IIR_REG)
#endif

#define UART2_IIR_REG ((volatile UINT32 *) (UART2_BASE + UART_IIR_OFFSET))
#define UART2_IIR_VAL  PREFIX_VAL(UART2_IIR_REG)

/* FIELDS: */

/* IID ACCESS: RW */

#ifndef UART_IIR_IID_POS
#define UART_IIR_IID_POS      0
#endif

#ifndef UART_IIR_IID_LEN
#define UART_IIR_IID_LEN      4
#endif

#if defined(_V1) && !defined(IIR_IID_R)
#define IIR_IID_R        GetGroupBits32( (UART2_IIR_VAL),UART_IIR_IID_POS, UART_IIR_IID_LEN)
#endif

#if defined(_V1) && !defined(IIR_IID_W)
#define IIR_IID_W(value) SetGroupBits32( (UART2_IIR_VAL),UART_IIR_IID_POS, UART_IIR_IID_LEN,value)
#endif

#define UART2_IIR_IID_R        GetGroupBits32( (UART2_IIR_VAL),UART_IIR_IID_POS, UART_IIR_IID_LEN)

#define UART2_IIR_IID_W(value) SetGroupBits32( (UART2_IIR_VAL),UART_IIR_IID_POS, UART_IIR_IID_LEN,value)


/* RSVD_IIR_5to4 ACCESS: RW */

#ifndef UART_IIR_RSVD_IIR_5TO4_POS
#define UART_IIR_RSVD_IIR_5TO4_POS      4
#endif

#ifndef UART_IIR_RSVD_IIR_5TO4_LEN
#define UART_IIR_RSVD_IIR_5TO4_LEN      2
#endif

#if defined(_V1) && !defined(IIR_RSVD_IIR_5TO4_R)
#define IIR_RSVD_IIR_5TO4_R        GetGroupBits32( (UART2_IIR_VAL),UART_IIR_RSVD_IIR_5TO4_POS, UART_IIR_RSVD_IIR_5TO4_LEN)
#endif

#if defined(_V1) && !defined(IIR_RSVD_IIR_5TO4_W)
#define IIR_RSVD_IIR_5TO4_W(value) SetGroupBits32( (UART2_IIR_VAL),UART_IIR_RSVD_IIR_5TO4_POS, UART_IIR_RSVD_IIR_5TO4_LEN,value)
#endif

#define UART2_IIR_RSVD_IIR_5TO4_R        GetGroupBits32( (UART2_IIR_VAL),UART_IIR_RSVD_IIR_5TO4_POS, UART_IIR_RSVD_IIR_5TO4_LEN)

#define UART2_IIR_RSVD_IIR_5TO4_W(value) SetGroupBits32( (UART2_IIR_VAL),UART_IIR_RSVD_IIR_5TO4_POS, UART_IIR_RSVD_IIR_5TO4_LEN,value)


/* FIFOSE ACCESS: RW */

#ifndef UART_IIR_FIFOSE_POS
#define UART_IIR_FIFOSE_POS      6
#endif

#ifndef UART_IIR_FIFOSE_LEN
#define UART_IIR_FIFOSE_LEN      2
#endif

#if defined(_V1) && !defined(IIR_FIFOSE_R)
#define IIR_FIFOSE_R        GetGroupBits32( (UART2_IIR_VAL),UART_IIR_FIFOSE_POS, UART_IIR_FIFOSE_LEN)
#endif

#if defined(_V1) && !defined(IIR_FIFOSE_W)
#define IIR_FIFOSE_W(value) SetGroupBits32( (UART2_IIR_VAL),UART_IIR_FIFOSE_POS, UART_IIR_FIFOSE_LEN,value)
#endif

#define UART2_IIR_FIFOSE_R        GetGroupBits32( (UART2_IIR_VAL),UART_IIR_FIFOSE_POS, UART_IIR_FIFOSE_LEN)

#define UART2_IIR_FIFOSE_W(value) SetGroupBits32( (UART2_IIR_VAL),UART_IIR_FIFOSE_POS, UART_IIR_FIFOSE_LEN,value)


/* RSVD_IIR_31to8 ACCESS: RW */

#ifndef UART_IIR_RSVD_IIR_31TO8_POS
#define UART_IIR_RSVD_IIR_31TO8_POS      8
#endif

#ifndef UART_IIR_RSVD_IIR_31TO8_LEN
#define UART_IIR_RSVD_IIR_31TO8_LEN      24
#endif

#if defined(_V1) && !defined(IIR_RSVD_IIR_31TO8_R)
#define IIR_RSVD_IIR_31TO8_R        GetGroupBits32( (UART2_IIR_VAL),UART_IIR_RSVD_IIR_31TO8_POS, UART_IIR_RSVD_IIR_31TO8_LEN)
#endif

#if defined(_V1) && !defined(IIR_RSVD_IIR_31TO8_W)
#define IIR_RSVD_IIR_31TO8_W(value) SetGroupBits32( (UART2_IIR_VAL),UART_IIR_RSVD_IIR_31TO8_POS, UART_IIR_RSVD_IIR_31TO8_LEN,value)
#endif

#define UART2_IIR_RSVD_IIR_31TO8_R        GetGroupBits32( (UART2_IIR_VAL),UART_IIR_RSVD_IIR_31TO8_POS, UART_IIR_RSVD_IIR_31TO8_LEN)

#define UART2_IIR_RSVD_IIR_31TO8_W(value) SetGroupBits32( (UART2_IIR_VAL),UART_IIR_RSVD_IIR_31TO8_POS, UART_IIR_RSVD_IIR_31TO8_LEN,value)


/* REGISTER: TFL ACCESS: RW */

#if defined(_V1) && !defined(TFL_OFFSET)
#define TFL_OFFSET 0x80
#endif

#if !defined(UART_TFL_OFFSET)
#define UART_TFL_OFFSET 0x80
#endif

#if defined(_V1) && !defined(TFL_REG)
#define TFL_REG ((volatile UINT32 *) (UART2_BASE + UART_TFL_OFFSET))
#endif

#if defined(_V1) && !defined(TFL_VAL)
#define TFL_VAL  PREFIX_VAL(TFL_REG)
#endif

#define UART2_TFL_REG ((volatile UINT32 *) (UART2_BASE + UART_TFL_OFFSET))
#define UART2_TFL_VAL  PREFIX_VAL(UART2_TFL_REG)

/* FIELDS: */

/* tfl ACCESS: RW */

#ifndef UART_TFL_TFL_POS
#define UART_TFL_TFL_POS      0
#endif

#ifndef UART_TFL_TFL_LEN
#define UART_TFL_TFL_LEN      7
#endif

#if defined(_V1) && !defined(TFL_TFL_R)
#define TFL_TFL_R        GetGroupBits32( (UART2_TFL_VAL),UART_TFL_TFL_POS, UART_TFL_TFL_LEN)
#endif

#if defined(_V1) && !defined(TFL_TFL_W)
#define TFL_TFL_W(value) SetGroupBits32( (UART2_TFL_VAL),UART_TFL_TFL_POS, UART_TFL_TFL_LEN,value)
#endif

#define UART2_TFL_TFL_R        GetGroupBits32( (UART2_TFL_VAL),UART_TFL_TFL_POS, UART_TFL_TFL_LEN)

#define UART2_TFL_TFL_W(value) SetGroupBits32( (UART2_TFL_VAL),UART_TFL_TFL_POS, UART_TFL_TFL_LEN,value)


/* RSVD_TFL_31toADDR_WIDTH ACCESS: RW */

#ifndef UART_TFL_RSVD_TFL_31TOADDR_WIDTH_POS
#define UART_TFL_RSVD_TFL_31TOADDR_WIDTH_POS      7
#endif

#ifndef UART_TFL_RSVD_TFL_31TOADDR_WIDTH_LEN
#define UART_TFL_RSVD_TFL_31TOADDR_WIDTH_LEN      25
#endif

#if defined(_V1) && !defined(TFL_RSVD_TFL_31TOADDR_WIDTH_R)
#define TFL_RSVD_TFL_31TOADDR_WIDTH_R        GetGroupBits32( (UART2_TFL_VAL),UART_TFL_RSVD_TFL_31TOADDR_WIDTH_POS, UART_TFL_RSVD_TFL_31TOADDR_WIDTH_LEN)
#endif

#if defined(_V1) && !defined(TFL_RSVD_TFL_31TOADDR_WIDTH_W)
#define TFL_RSVD_TFL_31TOADDR_WIDTH_W(value) SetGroupBits32( (UART2_TFL_VAL),UART_TFL_RSVD_TFL_31TOADDR_WIDTH_POS, UART_TFL_RSVD_TFL_31TOADDR_WIDTH_LEN,value)
#endif

#define UART2_TFL_RSVD_TFL_31TOADDR_WIDTH_R        GetGroupBits32( (UART2_TFL_VAL),UART_TFL_RSVD_TFL_31TOADDR_WIDTH_POS, UART_TFL_RSVD_TFL_31TOADDR_WIDTH_LEN)

#define UART2_TFL_RSVD_TFL_31TOADDR_WIDTH_W(value) SetGroupBits32( (UART2_TFL_VAL),UART_TFL_RSVD_TFL_31TOADDR_WIDTH_POS, UART_TFL_RSVD_TFL_31TOADDR_WIDTH_LEN,value)


/* REGISTER: RFL ACCESS: RW */

#if defined(_V1) && !defined(RFL_OFFSET)
#define RFL_OFFSET 0x84
#endif

#if !defined(UART_RFL_OFFSET)
#define UART_RFL_OFFSET 0x84
#endif

#if defined(_V1) && !defined(RFL_REG)
#define RFL_REG ((volatile UINT32 *) (UART2_BASE + UART_RFL_OFFSET))
#endif

#if defined(_V1) && !defined(RFL_VAL)
#define RFL_VAL  PREFIX_VAL(RFL_REG)
#endif

#define UART2_RFL_REG ((volatile UINT32 *) (UART2_BASE + UART_RFL_OFFSET))
#define UART2_RFL_VAL  PREFIX_VAL(UART2_RFL_REG)

/* FIELDS: */

/* rfl ACCESS: RW */

#ifndef UART_RFL_RFL_POS
#define UART_RFL_RFL_POS      0
#endif

#ifndef UART_RFL_RFL_LEN
#define UART_RFL_RFL_LEN      7
#endif

#if defined(_V1) && !defined(RFL_RFL_R)
#define RFL_RFL_R        GetGroupBits32( (UART2_RFL_VAL),UART_RFL_RFL_POS, UART_RFL_RFL_LEN)
#endif

#if defined(_V1) && !defined(RFL_RFL_W)
#define RFL_RFL_W(value) SetGroupBits32( (UART2_RFL_VAL),UART_RFL_RFL_POS, UART_RFL_RFL_LEN,value)
#endif

#define UART2_RFL_RFL_R        GetGroupBits32( (UART2_RFL_VAL),UART_RFL_RFL_POS, UART_RFL_RFL_LEN)

#define UART2_RFL_RFL_W(value) SetGroupBits32( (UART2_RFL_VAL),UART_RFL_RFL_POS, UART_RFL_RFL_LEN,value)


/* RSVD_RFL_31toADDR_WIDTH ACCESS: RW */

#ifndef UART_RFL_RSVD_RFL_31TOADDR_WIDTH_POS
#define UART_RFL_RSVD_RFL_31TOADDR_WIDTH_POS      7
#endif

#ifndef UART_RFL_RSVD_RFL_31TOADDR_WIDTH_LEN
#define UART_RFL_RSVD_RFL_31TOADDR_WIDTH_LEN      25
#endif

#if defined(_V1) && !defined(RFL_RSVD_RFL_31TOADDR_WIDTH_R)
#define RFL_RSVD_RFL_31TOADDR_WIDTH_R        GetGroupBits32( (UART2_RFL_VAL),UART_RFL_RSVD_RFL_31TOADDR_WIDTH_POS, UART_RFL_RSVD_RFL_31TOADDR_WIDTH_LEN)
#endif

#if defined(_V1) && !defined(RFL_RSVD_RFL_31TOADDR_WIDTH_W)
#define RFL_RSVD_RFL_31TOADDR_WIDTH_W(value) SetGroupBits32( (UART2_RFL_VAL),UART_RFL_RSVD_RFL_31TOADDR_WIDTH_POS, UART_RFL_RSVD_RFL_31TOADDR_WIDTH_LEN,value)
#endif

#define UART2_RFL_RSVD_RFL_31TOADDR_WIDTH_R        GetGroupBits32( (UART2_RFL_VAL),UART_RFL_RSVD_RFL_31TOADDR_WIDTH_POS, UART_RFL_RSVD_RFL_31TOADDR_WIDTH_LEN)

#define UART2_RFL_RSVD_RFL_31TOADDR_WIDTH_W(value) SetGroupBits32( (UART2_RFL_VAL),UART_RFL_RSVD_RFL_31TOADDR_WIDTH_POS, UART_RFL_RSVD_RFL_31TOADDR_WIDTH_LEN,value)


/* REGISTER: SRR ACCESS: RW */

#if defined(_V1) && !defined(SRR_OFFSET)
#define SRR_OFFSET 0x88
#endif

#if !defined(UART_SRR_OFFSET)
#define UART_SRR_OFFSET 0x88
#endif

#if defined(_V1) && !defined(SRR_REG)
#define SRR_REG ((volatile UINT32 *) (UART2_BASE + UART_SRR_OFFSET))
#endif

#if defined(_V1) && !defined(SRR_VAL)
#define SRR_VAL  PREFIX_VAL(SRR_REG)
#endif

#define UART2_SRR_REG ((volatile UINT32 *) (UART2_BASE + UART_SRR_OFFSET))
#define UART2_SRR_VAL  PREFIX_VAL(UART2_SRR_REG)

/* FIELDS: */

/* UR ACCESS: RW */

#ifndef UART_SRR_UR_POS
#define UART_SRR_UR_POS      0
#endif

#ifndef UART_SRR_UR_LEN
#define UART_SRR_UR_LEN      1
#endif

#if defined(_V1) && !defined(SRR_UR_R)
#define SRR_UR_R        GetGroupBits32( (UART2_SRR_VAL),UART_SRR_UR_POS, UART_SRR_UR_LEN)
#endif

#if defined(_V1) && !defined(SRR_UR_W)
#define SRR_UR_W(value) SetGroupBits32( (UART2_SRR_VAL),UART_SRR_UR_POS, UART_SRR_UR_LEN,value)
#endif

#define UART2_SRR_UR_R        GetGroupBits32( (UART2_SRR_VAL),UART_SRR_UR_POS, UART_SRR_UR_LEN)

#define UART2_SRR_UR_W(value) SetGroupBits32( (UART2_SRR_VAL),UART_SRR_UR_POS, UART_SRR_UR_LEN,value)


/* RFR ACCESS: RW */

#ifndef UART_SRR_RFR_POS
#define UART_SRR_RFR_POS      1
#endif

#ifndef UART_SRR_RFR_LEN
#define UART_SRR_RFR_LEN      1
#endif

#if defined(_V1) && !defined(SRR_RFR_R)
#define SRR_RFR_R        GetGroupBits32( (UART2_SRR_VAL),UART_SRR_RFR_POS, UART_SRR_RFR_LEN)
#endif

#if defined(_V1) && !defined(SRR_RFR_W)
#define SRR_RFR_W(value) SetGroupBits32( (UART2_SRR_VAL),UART_SRR_RFR_POS, UART_SRR_RFR_LEN,value)
#endif

#define UART2_SRR_RFR_R        GetGroupBits32( (UART2_SRR_VAL),UART_SRR_RFR_POS, UART_SRR_RFR_LEN)

#define UART2_SRR_RFR_W(value) SetGroupBits32( (UART2_SRR_VAL),UART_SRR_RFR_POS, UART_SRR_RFR_LEN,value)


/* XFR ACCESS: RW */

#ifndef UART_SRR_XFR_POS
#define UART_SRR_XFR_POS      2
#endif

#ifndef UART_SRR_XFR_LEN
#define UART_SRR_XFR_LEN      1
#endif

#if defined(_V1) && !defined(SRR_XFR_R)
#define SRR_XFR_R        GetGroupBits32( (UART2_SRR_VAL),UART_SRR_XFR_POS, UART_SRR_XFR_LEN)
#endif

#if defined(_V1) && !defined(SRR_XFR_W)
#define SRR_XFR_W(value) SetGroupBits32( (UART2_SRR_VAL),UART_SRR_XFR_POS, UART_SRR_XFR_LEN,value)
#endif

#define UART2_SRR_XFR_R        GetGroupBits32( (UART2_SRR_VAL),UART_SRR_XFR_POS, UART_SRR_XFR_LEN)

#define UART2_SRR_XFR_W(value) SetGroupBits32( (UART2_SRR_VAL),UART_SRR_XFR_POS, UART_SRR_XFR_LEN,value)


/* RSVD_SRR_31to3 ACCESS: RW */

#ifndef UART_SRR_RSVD_SRR_31TO3_POS
#define UART_SRR_RSVD_SRR_31TO3_POS      3
#endif

#ifndef UART_SRR_RSVD_SRR_31TO3_LEN
#define UART_SRR_RSVD_SRR_31TO3_LEN      29
#endif

#if defined(_V1) && !defined(SRR_RSVD_SRR_31TO3_R)
#define SRR_RSVD_SRR_31TO3_R        GetGroupBits32( (UART2_SRR_VAL),UART_SRR_RSVD_SRR_31TO3_POS, UART_SRR_RSVD_SRR_31TO3_LEN)
#endif

#if defined(_V1) && !defined(SRR_RSVD_SRR_31TO3_W)
#define SRR_RSVD_SRR_31TO3_W(value) SetGroupBits32( (UART2_SRR_VAL),UART_SRR_RSVD_SRR_31TO3_POS, UART_SRR_RSVD_SRR_31TO3_LEN,value)
#endif

#define UART2_SRR_RSVD_SRR_31TO3_R        GetGroupBits32( (UART2_SRR_VAL),UART_SRR_RSVD_SRR_31TO3_POS, UART_SRR_RSVD_SRR_31TO3_LEN)

#define UART2_SRR_RSVD_SRR_31TO3_W(value) SetGroupBits32( (UART2_SRR_VAL),UART_SRR_RSVD_SRR_31TO3_POS, UART_SRR_RSVD_SRR_31TO3_LEN,value)


/* REGISTER: SRTS ACCESS: RW */

#if defined(_V1) && !defined(SRTS_OFFSET)
#define SRTS_OFFSET 0x8c
#endif

#if !defined(UART_SRTS_OFFSET)
#define UART_SRTS_OFFSET 0x8c
#endif

#if defined(_V1) && !defined(SRTS_REG)
#define SRTS_REG ((volatile UINT32 *) (UART2_BASE + UART_SRTS_OFFSET))
#endif

#if defined(_V1) && !defined(SRTS_VAL)
#define SRTS_VAL  PREFIX_VAL(SRTS_REG)
#endif

#define UART2_SRTS_REG ((volatile UINT32 *) (UART2_BASE + UART_SRTS_OFFSET))
#define UART2_SRTS_VAL  PREFIX_VAL(UART2_SRTS_REG)

/* FIELDS: */

/* srts ACCESS: RW */

#ifndef UART_SRTS_SRTS_POS
#define UART_SRTS_SRTS_POS      0
#endif

#ifndef UART_SRTS_SRTS_LEN
#define UART_SRTS_SRTS_LEN      1
#endif

#if defined(_V1) && !defined(SRTS_SRTS_R)
#define SRTS_SRTS_R        GetGroupBits32( (UART2_SRTS_VAL),UART_SRTS_SRTS_POS, UART_SRTS_SRTS_LEN)
#endif

#if defined(_V1) && !defined(SRTS_SRTS_W)
#define SRTS_SRTS_W(value) SetGroupBits32( (UART2_SRTS_VAL),UART_SRTS_SRTS_POS, UART_SRTS_SRTS_LEN,value)
#endif

#define UART2_SRTS_SRTS_R        GetGroupBits32( (UART2_SRTS_VAL),UART_SRTS_SRTS_POS, UART_SRTS_SRTS_LEN)

#define UART2_SRTS_SRTS_W(value) SetGroupBits32( (UART2_SRTS_VAL),UART_SRTS_SRTS_POS, UART_SRTS_SRTS_LEN,value)


/* RSVD_SRTS_31to1 ACCESS: RW */

#ifndef UART_SRTS_RSVD_SRTS_31TO1_POS
#define UART_SRTS_RSVD_SRTS_31TO1_POS      1
#endif

#ifndef UART_SRTS_RSVD_SRTS_31TO1_LEN
#define UART_SRTS_RSVD_SRTS_31TO1_LEN      31
#endif

#if defined(_V1) && !defined(SRTS_RSVD_SRTS_31TO1_R)
#define SRTS_RSVD_SRTS_31TO1_R        GetGroupBits32( (UART2_SRTS_VAL),UART_SRTS_RSVD_SRTS_31TO1_POS, UART_SRTS_RSVD_SRTS_31TO1_LEN)
#endif

#if defined(_V1) && !defined(SRTS_RSVD_SRTS_31TO1_W)
#define SRTS_RSVD_SRTS_31TO1_W(value) SetGroupBits32( (UART2_SRTS_VAL),UART_SRTS_RSVD_SRTS_31TO1_POS, UART_SRTS_RSVD_SRTS_31TO1_LEN,value)
#endif

#define UART2_SRTS_RSVD_SRTS_31TO1_R        GetGroupBits32( (UART2_SRTS_VAL),UART_SRTS_RSVD_SRTS_31TO1_POS, UART_SRTS_RSVD_SRTS_31TO1_LEN)

#define UART2_SRTS_RSVD_SRTS_31TO1_W(value) SetGroupBits32( (UART2_SRTS_VAL),UART_SRTS_RSVD_SRTS_31TO1_POS, UART_SRTS_RSVD_SRTS_31TO1_LEN,value)


/* REGISTER: SBCR ACCESS: RW */

#if defined(_V1) && !defined(SBCR_OFFSET)
#define SBCR_OFFSET 0x90
#endif

#if !defined(UART_SBCR_OFFSET)
#define UART_SBCR_OFFSET 0x90
#endif

#if defined(_V1) && !defined(SBCR_REG)
#define SBCR_REG ((volatile UINT32 *) (UART2_BASE + UART_SBCR_OFFSET))
#endif

#if defined(_V1) && !defined(SBCR_VAL)
#define SBCR_VAL  PREFIX_VAL(SBCR_REG)
#endif

#define UART2_SBCR_REG ((volatile UINT32 *) (UART2_BASE + UART_SBCR_OFFSET))
#define UART2_SBCR_VAL  PREFIX_VAL(UART2_SBCR_REG)

/* FIELDS: */

/* sbcb ACCESS: RW */

#ifndef UART_SBCR_SBCB_POS
#define UART_SBCR_SBCB_POS      0
#endif

#ifndef UART_SBCR_SBCB_LEN
#define UART_SBCR_SBCB_LEN      1
#endif

#if defined(_V1) && !defined(SBCR_SBCB_R)
#define SBCR_SBCB_R        GetGroupBits32( (UART2_SBCR_VAL),UART_SBCR_SBCB_POS, UART_SBCR_SBCB_LEN)
#endif

#if defined(_V1) && !defined(SBCR_SBCB_W)
#define SBCR_SBCB_W(value) SetGroupBits32( (UART2_SBCR_VAL),UART_SBCR_SBCB_POS, UART_SBCR_SBCB_LEN,value)
#endif

#define UART2_SBCR_SBCB_R        GetGroupBits32( (UART2_SBCR_VAL),UART_SBCR_SBCB_POS, UART_SBCR_SBCB_LEN)

#define UART2_SBCR_SBCB_W(value) SetGroupBits32( (UART2_SBCR_VAL),UART_SBCR_SBCB_POS, UART_SBCR_SBCB_LEN,value)


/* RSVD_SBCR_31to1 ACCESS: RW */

#ifndef UART_SBCR_RSVD_SBCR_31TO1_POS
#define UART_SBCR_RSVD_SBCR_31TO1_POS      1
#endif

#ifndef UART_SBCR_RSVD_SBCR_31TO1_LEN
#define UART_SBCR_RSVD_SBCR_31TO1_LEN      31
#endif

#if defined(_V1) && !defined(SBCR_RSVD_SBCR_31TO1_R)
#define SBCR_RSVD_SBCR_31TO1_R        GetGroupBits32( (UART2_SBCR_VAL),UART_SBCR_RSVD_SBCR_31TO1_POS, UART_SBCR_RSVD_SBCR_31TO1_LEN)
#endif

#if defined(_V1) && !defined(SBCR_RSVD_SBCR_31TO1_W)
#define SBCR_RSVD_SBCR_31TO1_W(value) SetGroupBits32( (UART2_SBCR_VAL),UART_SBCR_RSVD_SBCR_31TO1_POS, UART_SBCR_RSVD_SBCR_31TO1_LEN,value)
#endif

#define UART2_SBCR_RSVD_SBCR_31TO1_R        GetGroupBits32( (UART2_SBCR_VAL),UART_SBCR_RSVD_SBCR_31TO1_POS, UART_SBCR_RSVD_SBCR_31TO1_LEN)

#define UART2_SBCR_RSVD_SBCR_31TO1_W(value) SetGroupBits32( (UART2_SBCR_VAL),UART_SBCR_RSVD_SBCR_31TO1_POS, UART_SBCR_RSVD_SBCR_31TO1_LEN,value)


/* REGISTER: SDMAM ACCESS: RW */

#if defined(_V1) && !defined(SDMAM_OFFSET)
#define SDMAM_OFFSET 0x94
#endif

#if !defined(UART_SDMAM_OFFSET)
#define UART_SDMAM_OFFSET 0x94
#endif

#if defined(_V1) && !defined(SDMAM_REG)
#define SDMAM_REG ((volatile UINT32 *) (UART2_BASE + UART_SDMAM_OFFSET))
#endif

#if defined(_V1) && !defined(SDMAM_VAL)
#define SDMAM_VAL  PREFIX_VAL(SDMAM_REG)
#endif

#define UART2_SDMAM_REG ((volatile UINT32 *) (UART2_BASE + UART_SDMAM_OFFSET))
#define UART2_SDMAM_VAL  PREFIX_VAL(UART2_SDMAM_REG)

/* FIELDS: */

/* sdmam ACCESS: RW */

#ifndef UART_SDMAM_SDMAM_POS
#define UART_SDMAM_SDMAM_POS      0
#endif

#ifndef UART_SDMAM_SDMAM_LEN
#define UART_SDMAM_SDMAM_LEN      1
#endif

#if defined(_V1) && !defined(SDMAM_SDMAM_R)
#define SDMAM_SDMAM_R        GetGroupBits32( (UART2_SDMAM_VAL),UART_SDMAM_SDMAM_POS, UART_SDMAM_SDMAM_LEN)
#endif

#if defined(_V1) && !defined(SDMAM_SDMAM_W)
#define SDMAM_SDMAM_W(value) SetGroupBits32( (UART2_SDMAM_VAL),UART_SDMAM_SDMAM_POS, UART_SDMAM_SDMAM_LEN,value)
#endif

#define UART2_SDMAM_SDMAM_R        GetGroupBits32( (UART2_SDMAM_VAL),UART_SDMAM_SDMAM_POS, UART_SDMAM_SDMAM_LEN)

#define UART2_SDMAM_SDMAM_W(value) SetGroupBits32( (UART2_SDMAM_VAL),UART_SDMAM_SDMAM_POS, UART_SDMAM_SDMAM_LEN,value)


/* RSVD_SDMAM_31to1 ACCESS: RW */

#ifndef UART_SDMAM_RSVD_SDMAM_31TO1_POS
#define UART_SDMAM_RSVD_SDMAM_31TO1_POS      1
#endif

#ifndef UART_SDMAM_RSVD_SDMAM_31TO1_LEN
#define UART_SDMAM_RSVD_SDMAM_31TO1_LEN      31
#endif

#if defined(_V1) && !defined(SDMAM_RSVD_SDMAM_31TO1_R)
#define SDMAM_RSVD_SDMAM_31TO1_R        GetGroupBits32( (UART2_SDMAM_VAL),UART_SDMAM_RSVD_SDMAM_31TO1_POS, UART_SDMAM_RSVD_SDMAM_31TO1_LEN)
#endif

#if defined(_V1) && !defined(SDMAM_RSVD_SDMAM_31TO1_W)
#define SDMAM_RSVD_SDMAM_31TO1_W(value) SetGroupBits32( (UART2_SDMAM_VAL),UART_SDMAM_RSVD_SDMAM_31TO1_POS, UART_SDMAM_RSVD_SDMAM_31TO1_LEN,value)
#endif

#define UART2_SDMAM_RSVD_SDMAM_31TO1_R        GetGroupBits32( (UART2_SDMAM_VAL),UART_SDMAM_RSVD_SDMAM_31TO1_POS, UART_SDMAM_RSVD_SDMAM_31TO1_LEN)

#define UART2_SDMAM_RSVD_SDMAM_31TO1_W(value) SetGroupBits32( (UART2_SDMAM_VAL),UART_SDMAM_RSVD_SDMAM_31TO1_POS, UART_SDMAM_RSVD_SDMAM_31TO1_LEN,value)


/* REGISTER: SFE ACCESS: RW */

#if defined(_V1) && !defined(SFE_OFFSET)
#define SFE_OFFSET 0x98
#endif

#if !defined(UART_SFE_OFFSET)
#define UART_SFE_OFFSET 0x98
#endif

#if defined(_V1) && !defined(SFE_REG)
#define SFE_REG ((volatile UINT32 *) (UART2_BASE + UART_SFE_OFFSET))
#endif

#if defined(_V1) && !defined(SFE_VAL)
#define SFE_VAL  PREFIX_VAL(SFE_REG)
#endif

#define UART2_SFE_REG ((volatile UINT32 *) (UART2_BASE + UART_SFE_OFFSET))
#define UART2_SFE_VAL  PREFIX_VAL(UART2_SFE_REG)

/* FIELDS: */

/* sfe ACCESS: RW */

#ifndef UART_SFE_SFE_POS
#define UART_SFE_SFE_POS      0
#endif

#ifndef UART_SFE_SFE_LEN
#define UART_SFE_SFE_LEN      1
#endif

#if defined(_V1) && !defined(SFE_SFE_R)
#define SFE_SFE_R        GetGroupBits32( (UART2_SFE_VAL),UART_SFE_SFE_POS, UART_SFE_SFE_LEN)
#endif

#if defined(_V1) && !defined(SFE_SFE_W)
#define SFE_SFE_W(value) SetGroupBits32( (UART2_SFE_VAL),UART_SFE_SFE_POS, UART_SFE_SFE_LEN,value)
#endif

#define UART2_SFE_SFE_R        GetGroupBits32( (UART2_SFE_VAL),UART_SFE_SFE_POS, UART_SFE_SFE_LEN)

#define UART2_SFE_SFE_W(value) SetGroupBits32( (UART2_SFE_VAL),UART_SFE_SFE_POS, UART_SFE_SFE_LEN,value)


/* RSVD_SFE_31to1 ACCESS: RW */

#ifndef UART_SFE_RSVD_SFE_31TO1_POS
#define UART_SFE_RSVD_SFE_31TO1_POS      1
#endif

#ifndef UART_SFE_RSVD_SFE_31TO1_LEN
#define UART_SFE_RSVD_SFE_31TO1_LEN      31
#endif

#if defined(_V1) && !defined(SFE_RSVD_SFE_31TO1_R)
#define SFE_RSVD_SFE_31TO1_R        GetGroupBits32( (UART2_SFE_VAL),UART_SFE_RSVD_SFE_31TO1_POS, UART_SFE_RSVD_SFE_31TO1_LEN)
#endif

#if defined(_V1) && !defined(SFE_RSVD_SFE_31TO1_W)
#define SFE_RSVD_SFE_31TO1_W(value) SetGroupBits32( (UART2_SFE_VAL),UART_SFE_RSVD_SFE_31TO1_POS, UART_SFE_RSVD_SFE_31TO1_LEN,value)
#endif

#define UART2_SFE_RSVD_SFE_31TO1_R        GetGroupBits32( (UART2_SFE_VAL),UART_SFE_RSVD_SFE_31TO1_POS, UART_SFE_RSVD_SFE_31TO1_LEN)

#define UART2_SFE_RSVD_SFE_31TO1_W(value) SetGroupBits32( (UART2_SFE_VAL),UART_SFE_RSVD_SFE_31TO1_POS, UART_SFE_RSVD_SFE_31TO1_LEN,value)


/* REGISTER: SRT ACCESS: RW */

#if defined(_V1) && !defined(SRT_OFFSET)
#define SRT_OFFSET 0x9c
#endif

#if !defined(UART_SRT_OFFSET)
#define UART_SRT_OFFSET 0x9c
#endif

#if defined(_V1) && !defined(SRT_REG)
#define SRT_REG ((volatile UINT32 *) (UART2_BASE + UART_SRT_OFFSET))
#endif

#if defined(_V1) && !defined(SRT_VAL)
#define SRT_VAL  PREFIX_VAL(SRT_REG)
#endif

#define UART2_SRT_REG ((volatile UINT32 *) (UART2_BASE + UART_SRT_OFFSET))
#define UART2_SRT_VAL  PREFIX_VAL(UART2_SRT_REG)

/* FIELDS: */

/* srt ACCESS: RW */

#ifndef UART_SRT_SRT_POS
#define UART_SRT_SRT_POS      0
#endif

#ifndef UART_SRT_SRT_LEN
#define UART_SRT_SRT_LEN      2
#endif

#if defined(_V1) && !defined(SRT_SRT_R)
#define SRT_SRT_R        GetGroupBits32( (UART2_SRT_VAL),UART_SRT_SRT_POS, UART_SRT_SRT_LEN)
#endif

#if defined(_V1) && !defined(SRT_SRT_W)
#define SRT_SRT_W(value) SetGroupBits32( (UART2_SRT_VAL),UART_SRT_SRT_POS, UART_SRT_SRT_LEN,value)
#endif

#define UART2_SRT_SRT_R        GetGroupBits32( (UART2_SRT_VAL),UART_SRT_SRT_POS, UART_SRT_SRT_LEN)

#define UART2_SRT_SRT_W(value) SetGroupBits32( (UART2_SRT_VAL),UART_SRT_SRT_POS, UART_SRT_SRT_LEN,value)


/* RSVD_SRT_31to2 ACCESS: RW */

#ifndef UART_SRT_RSVD_SRT_31TO2_POS
#define UART_SRT_RSVD_SRT_31TO2_POS      2
#endif

#ifndef UART_SRT_RSVD_SRT_31TO2_LEN
#define UART_SRT_RSVD_SRT_31TO2_LEN      30
#endif

#if defined(_V1) && !defined(SRT_RSVD_SRT_31TO2_R)
#define SRT_RSVD_SRT_31TO2_R        GetGroupBits32( (UART2_SRT_VAL),UART_SRT_RSVD_SRT_31TO2_POS, UART_SRT_RSVD_SRT_31TO2_LEN)
#endif

#if defined(_V1) && !defined(SRT_RSVD_SRT_31TO2_W)
#define SRT_RSVD_SRT_31TO2_W(value) SetGroupBits32( (UART2_SRT_VAL),UART_SRT_RSVD_SRT_31TO2_POS, UART_SRT_RSVD_SRT_31TO2_LEN,value)
#endif

#define UART2_SRT_RSVD_SRT_31TO2_R        GetGroupBits32( (UART2_SRT_VAL),UART_SRT_RSVD_SRT_31TO2_POS, UART_SRT_RSVD_SRT_31TO2_LEN)

#define UART2_SRT_RSVD_SRT_31TO2_W(value) SetGroupBits32( (UART2_SRT_VAL),UART_SRT_RSVD_SRT_31TO2_POS, UART_SRT_RSVD_SRT_31TO2_LEN,value)


/* REGISTER: STET ACCESS: RW */

#if defined(_V1) && !defined(STET_OFFSET)
#define STET_OFFSET 0xa0
#endif

#if !defined(UART_STET_OFFSET)
#define UART_STET_OFFSET 0xa0
#endif

#if defined(_V1) && !defined(STET_REG)
#define STET_REG ((volatile UINT32 *) (UART2_BASE + UART_STET_OFFSET))
#endif

#if defined(_V1) && !defined(STET_VAL)
#define STET_VAL  PREFIX_VAL(STET_REG)
#endif

#define UART2_STET_REG ((volatile UINT32 *) (UART2_BASE + UART_STET_OFFSET))
#define UART2_STET_VAL  PREFIX_VAL(UART2_STET_REG)

/* FIELDS: */

/* stet ACCESS: RW */

#ifndef UART_STET_STET_POS
#define UART_STET_STET_POS      0
#endif

#ifndef UART_STET_STET_LEN
#define UART_STET_STET_LEN      2
#endif

#if defined(_V1) && !defined(STET_STET_R)
#define STET_STET_R        GetGroupBits32( (UART2_STET_VAL),UART_STET_STET_POS, UART_STET_STET_LEN)
#endif

#if defined(_V1) && !defined(STET_STET_W)
#define STET_STET_W(value) SetGroupBits32( (UART2_STET_VAL),UART_STET_STET_POS, UART_STET_STET_LEN,value)
#endif

#define UART2_STET_STET_R        GetGroupBits32( (UART2_STET_VAL),UART_STET_STET_POS, UART_STET_STET_LEN)

#define UART2_STET_STET_W(value) SetGroupBits32( (UART2_STET_VAL),UART_STET_STET_POS, UART_STET_STET_LEN,value)


/* RSVD_STET_31to2 ACCESS: RW */

#ifndef UART_STET_RSVD_STET_31TO2_POS
#define UART_STET_RSVD_STET_31TO2_POS      2
#endif

#ifndef UART_STET_RSVD_STET_31TO2_LEN
#define UART_STET_RSVD_STET_31TO2_LEN      30
#endif

#if defined(_V1) && !defined(STET_RSVD_STET_31TO2_R)
#define STET_RSVD_STET_31TO2_R        GetGroupBits32( (UART2_STET_VAL),UART_STET_RSVD_STET_31TO2_POS, UART_STET_RSVD_STET_31TO2_LEN)
#endif

#if defined(_V1) && !defined(STET_RSVD_STET_31TO2_W)
#define STET_RSVD_STET_31TO2_W(value) SetGroupBits32( (UART2_STET_VAL),UART_STET_RSVD_STET_31TO2_POS, UART_STET_RSVD_STET_31TO2_LEN,value)
#endif

#define UART2_STET_RSVD_STET_31TO2_R        GetGroupBits32( (UART2_STET_VAL),UART_STET_RSVD_STET_31TO2_POS, UART_STET_RSVD_STET_31TO2_LEN)

#define UART2_STET_RSVD_STET_31TO2_W(value) SetGroupBits32( (UART2_STET_VAL),UART_STET_RSVD_STET_31TO2_POS, UART_STET_RSVD_STET_31TO2_LEN,value)


/* REGISTER: HTX ACCESS: RW */

#if defined(_V1) && !defined(HTX_OFFSET)
#define HTX_OFFSET 0xa4
#endif

#if !defined(UART_HTX_OFFSET)
#define UART_HTX_OFFSET 0xa4
#endif

#if defined(_V1) && !defined(HTX_REG)
#define HTX_REG ((volatile UINT32 *) (UART2_BASE + UART_HTX_OFFSET))
#endif

#if defined(_V1) && !defined(HTX_VAL)
#define HTX_VAL  PREFIX_VAL(HTX_REG)
#endif

#define UART2_HTX_REG ((volatile UINT32 *) (UART2_BASE + UART_HTX_OFFSET))
#define UART2_HTX_VAL  PREFIX_VAL(UART2_HTX_REG)

/* FIELDS: */

/* htx ACCESS: RW */

#ifndef UART_HTX_HTX_POS
#define UART_HTX_HTX_POS      0
#endif

#ifndef UART_HTX_HTX_LEN
#define UART_HTX_HTX_LEN      1
#endif

#if defined(_V1) && !defined(HTX_HTX_R)
#define HTX_HTX_R        GetGroupBits32( (UART2_HTX_VAL),UART_HTX_HTX_POS, UART_HTX_HTX_LEN)
#endif

#if defined(_V1) && !defined(HTX_HTX_W)
#define HTX_HTX_W(value) SetGroupBits32( (UART2_HTX_VAL),UART_HTX_HTX_POS, UART_HTX_HTX_LEN,value)
#endif

#define UART2_HTX_HTX_R        GetGroupBits32( (UART2_HTX_VAL),UART_HTX_HTX_POS, UART_HTX_HTX_LEN)

#define UART2_HTX_HTX_W(value) SetGroupBits32( (UART2_HTX_VAL),UART_HTX_HTX_POS, UART_HTX_HTX_LEN,value)


/* RSVD_HTX_31to1 ACCESS: RW */

#ifndef UART_HTX_RSVD_HTX_31TO1_POS
#define UART_HTX_RSVD_HTX_31TO1_POS      1
#endif

#ifndef UART_HTX_RSVD_HTX_31TO1_LEN
#define UART_HTX_RSVD_HTX_31TO1_LEN      31
#endif

#if defined(_V1) && !defined(HTX_RSVD_HTX_31TO1_R)
#define HTX_RSVD_HTX_31TO1_R        GetGroupBits32( (UART2_HTX_VAL),UART_HTX_RSVD_HTX_31TO1_POS, UART_HTX_RSVD_HTX_31TO1_LEN)
#endif

#if defined(_V1) && !defined(HTX_RSVD_HTX_31TO1_W)
#define HTX_RSVD_HTX_31TO1_W(value) SetGroupBits32( (UART2_HTX_VAL),UART_HTX_RSVD_HTX_31TO1_POS, UART_HTX_RSVD_HTX_31TO1_LEN,value)
#endif

#define UART2_HTX_RSVD_HTX_31TO1_R        GetGroupBits32( (UART2_HTX_VAL),UART_HTX_RSVD_HTX_31TO1_POS, UART_HTX_RSVD_HTX_31TO1_LEN)

#define UART2_HTX_RSVD_HTX_31TO1_W(value) SetGroupBits32( (UART2_HTX_VAL),UART_HTX_RSVD_HTX_31TO1_POS, UART_HTX_RSVD_HTX_31TO1_LEN,value)


/* REGISTER: DMASA ACCESS: RW */

#if defined(_V1) && !defined(DMASA_OFFSET)
#define DMASA_OFFSET 0xa8
#endif

#if !defined(UART_DMASA_OFFSET)
#define UART_DMASA_OFFSET 0xa8
#endif

#if defined(_V1) && !defined(DMASA_REG)
#define DMASA_REG ((volatile UINT32 *) (UART2_BASE + UART_DMASA_OFFSET))
#endif

#if defined(_V1) && !defined(DMASA_VAL)
#define DMASA_VAL  PREFIX_VAL(DMASA_REG)
#endif

#define UART2_DMASA_REG ((volatile UINT32 *) (UART2_BASE + UART_DMASA_OFFSET))
#define UART2_DMASA_VAL  PREFIX_VAL(UART2_DMASA_REG)

/* FIELDS: */

/* dmasa ACCESS: RW */

#ifndef UART_DMASA_DMASA_POS
#define UART_DMASA_DMASA_POS      0
#endif

#ifndef UART_DMASA_DMASA_LEN
#define UART_DMASA_DMASA_LEN      1
#endif

#if defined(_V1) && !defined(DMASA_DMASA_R)
#define DMASA_DMASA_R        GetGroupBits32( (UART2_DMASA_VAL),UART_DMASA_DMASA_POS, UART_DMASA_DMASA_LEN)
#endif

#if defined(_V1) && !defined(DMASA_DMASA_W)
#define DMASA_DMASA_W(value) SetGroupBits32( (UART2_DMASA_VAL),UART_DMASA_DMASA_POS, UART_DMASA_DMASA_LEN,value)
#endif

#define UART2_DMASA_DMASA_R        GetGroupBits32( (UART2_DMASA_VAL),UART_DMASA_DMASA_POS, UART_DMASA_DMASA_LEN)

#define UART2_DMASA_DMASA_W(value) SetGroupBits32( (UART2_DMASA_VAL),UART_DMASA_DMASA_POS, UART_DMASA_DMASA_LEN,value)


/* RSVD_DMASA_31to1 ACCESS: RW */

#ifndef UART_DMASA_RSVD_DMASA_31TO1_POS
#define UART_DMASA_RSVD_DMASA_31TO1_POS      1
#endif

#ifndef UART_DMASA_RSVD_DMASA_31TO1_LEN
#define UART_DMASA_RSVD_DMASA_31TO1_LEN      31
#endif

#if defined(_V1) && !defined(DMASA_RSVD_DMASA_31TO1_R)
#define DMASA_RSVD_DMASA_31TO1_R        GetGroupBits32( (UART2_DMASA_VAL),UART_DMASA_RSVD_DMASA_31TO1_POS, UART_DMASA_RSVD_DMASA_31TO1_LEN)
#endif

#if defined(_V1) && !defined(DMASA_RSVD_DMASA_31TO1_W)
#define DMASA_RSVD_DMASA_31TO1_W(value) SetGroupBits32( (UART2_DMASA_VAL),UART_DMASA_RSVD_DMASA_31TO1_POS, UART_DMASA_RSVD_DMASA_31TO1_LEN,value)
#endif

#define UART2_DMASA_RSVD_DMASA_31TO1_R        GetGroupBits32( (UART2_DMASA_VAL),UART_DMASA_RSVD_DMASA_31TO1_POS, UART_DMASA_RSVD_DMASA_31TO1_LEN)

#define UART2_DMASA_RSVD_DMASA_31TO1_W(value) SetGroupBits32( (UART2_DMASA_VAL),UART_DMASA_RSVD_DMASA_31TO1_POS, UART_DMASA_RSVD_DMASA_31TO1_LEN,value)


/* REGISTER: LCR ACCESS: RW */

#if defined(_V1) && !defined(LCR_OFFSET)
#define LCR_OFFSET 0xc
#endif

#if !defined(UART_LCR_OFFSET)
#define UART_LCR_OFFSET 0xc
#endif

#if defined(_V1) && !defined(LCR_REG)
#define LCR_REG ((volatile UINT32 *) (UART2_BASE + UART_LCR_OFFSET))
#endif

#if defined(_V1) && !defined(LCR_VAL)
#define LCR_VAL  PREFIX_VAL(LCR_REG)
#endif

#define UART2_LCR_REG ((volatile UINT32 *) (UART2_BASE + UART_LCR_OFFSET))
#define UART2_LCR_VAL  PREFIX_VAL(UART2_LCR_REG)

/* FIELDS: */

/* DLS ACCESS: RW */

#ifndef UART_LCR_DLS_POS
#define UART_LCR_DLS_POS      0
#endif

#ifndef UART_LCR_DLS_LEN
#define UART_LCR_DLS_LEN      2
#endif

#if defined(_V1) && !defined(LCR_DLS_R)
#define LCR_DLS_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_DLS_POS, UART_LCR_DLS_LEN)
#endif

#if defined(_V1) && !defined(LCR_DLS_W)
#define LCR_DLS_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_DLS_POS, UART_LCR_DLS_LEN,value)
#endif

#define UART2_LCR_DLS_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_DLS_POS, UART_LCR_DLS_LEN)

#define UART2_LCR_DLS_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_DLS_POS, UART_LCR_DLS_LEN,value)


/* STOP ACCESS: RW */

#ifndef UART_LCR_STOP_POS
#define UART_LCR_STOP_POS      2
#endif

#ifndef UART_LCR_STOP_LEN
#define UART_LCR_STOP_LEN      1
#endif

#if defined(_V1) && !defined(LCR_STOP_R)
#define LCR_STOP_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_STOP_POS, UART_LCR_STOP_LEN)
#endif

#if defined(_V1) && !defined(LCR_STOP_W)
#define LCR_STOP_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_STOP_POS, UART_LCR_STOP_LEN,value)
#endif

#define UART2_LCR_STOP_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_STOP_POS, UART_LCR_STOP_LEN)

#define UART2_LCR_STOP_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_STOP_POS, UART_LCR_STOP_LEN,value)


/* PEN ACCESS: RW */

#ifndef UART_LCR_PEN_POS
#define UART_LCR_PEN_POS      3
#endif

#ifndef UART_LCR_PEN_LEN
#define UART_LCR_PEN_LEN      1
#endif

#if defined(_V1) && !defined(LCR_PEN_R)
#define LCR_PEN_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_PEN_POS, UART_LCR_PEN_LEN)
#endif

#if defined(_V1) && !defined(LCR_PEN_W)
#define LCR_PEN_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_PEN_POS, UART_LCR_PEN_LEN,value)
#endif

#define UART2_LCR_PEN_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_PEN_POS, UART_LCR_PEN_LEN)

#define UART2_LCR_PEN_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_PEN_POS, UART_LCR_PEN_LEN,value)


/* EPS ACCESS: RW */

#ifndef UART_LCR_EPS_POS
#define UART_LCR_EPS_POS      4
#endif

#ifndef UART_LCR_EPS_LEN
#define UART_LCR_EPS_LEN      1
#endif

#if defined(_V1) && !defined(LCR_EPS_R)
#define LCR_EPS_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_EPS_POS, UART_LCR_EPS_LEN)
#endif

#if defined(_V1) && !defined(LCR_EPS_W)
#define LCR_EPS_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_EPS_POS, UART_LCR_EPS_LEN,value)
#endif

#define UART2_LCR_EPS_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_EPS_POS, UART_LCR_EPS_LEN)

#define UART2_LCR_EPS_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_EPS_POS, UART_LCR_EPS_LEN,value)


/* SP ACCESS: RW */

#ifndef UART_LCR_SP_POS
#define UART_LCR_SP_POS      5
#endif

#ifndef UART_LCR_SP_LEN
#define UART_LCR_SP_LEN      1
#endif

#if defined(_V1) && !defined(LCR_SP_R)
#define LCR_SP_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_SP_POS, UART_LCR_SP_LEN)
#endif

#if defined(_V1) && !defined(LCR_SP_W)
#define LCR_SP_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_SP_POS, UART_LCR_SP_LEN,value)
#endif

#define UART2_LCR_SP_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_SP_POS, UART_LCR_SP_LEN)

#define UART2_LCR_SP_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_SP_POS, UART_LCR_SP_LEN,value)


/* Break ACCESS: RW */

#ifndef UART_LCR_BREAK_POS
#define UART_LCR_BREAK_POS      6
#endif

#ifndef UART_LCR_BREAK_LEN
#define UART_LCR_BREAK_LEN      1
#endif

#if defined(_V1) && !defined(LCR_BREAK_R)
#define LCR_BREAK_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_BREAK_POS, UART_LCR_BREAK_LEN)
#endif

#if defined(_V1) && !defined(LCR_BREAK_W)
#define LCR_BREAK_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_BREAK_POS, UART_LCR_BREAK_LEN,value)
#endif

#define UART2_LCR_BREAK_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_BREAK_POS, UART_LCR_BREAK_LEN)

#define UART2_LCR_BREAK_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_BREAK_POS, UART_LCR_BREAK_LEN,value)


/* DLAB ACCESS: RW */

#ifndef UART_LCR_DLAB_POS
#define UART_LCR_DLAB_POS      7
#endif

#ifndef UART_LCR_DLAB_LEN
#define UART_LCR_DLAB_LEN      1
#endif

#if defined(_V1) && !defined(LCR_DLAB_R)
#define LCR_DLAB_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_DLAB_POS, UART_LCR_DLAB_LEN)
#endif

#if defined(_V1) && !defined(LCR_DLAB_W)
#define LCR_DLAB_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_DLAB_POS, UART_LCR_DLAB_LEN,value)
#endif

#define UART2_LCR_DLAB_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_DLAB_POS, UART_LCR_DLAB_LEN)

#define UART2_LCR_DLAB_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_DLAB_POS, UART_LCR_DLAB_LEN,value)


/* RSVD_LCR_31to8 ACCESS: RW */

#ifndef UART_LCR_RSVD_LCR_31TO8_POS
#define UART_LCR_RSVD_LCR_31TO8_POS      8
#endif

#ifndef UART_LCR_RSVD_LCR_31TO8_LEN
#define UART_LCR_RSVD_LCR_31TO8_LEN      24
#endif

#if defined(_V1) && !defined(LCR_RSVD_LCR_31TO8_R)
#define LCR_RSVD_LCR_31TO8_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_RSVD_LCR_31TO8_POS, UART_LCR_RSVD_LCR_31TO8_LEN)
#endif

#if defined(_V1) && !defined(LCR_RSVD_LCR_31TO8_W)
#define LCR_RSVD_LCR_31TO8_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_RSVD_LCR_31TO8_POS, UART_LCR_RSVD_LCR_31TO8_LEN,value)
#endif

#define UART2_LCR_RSVD_LCR_31TO8_R        GetGroupBits32( (UART2_LCR_VAL),UART_LCR_RSVD_LCR_31TO8_POS, UART_LCR_RSVD_LCR_31TO8_LEN)

#define UART2_LCR_RSVD_LCR_31TO8_W(value) SetGroupBits32( (UART2_LCR_VAL),UART_LCR_RSVD_LCR_31TO8_POS, UART_LCR_RSVD_LCR_31TO8_LEN,value)


/* REGISTER: DLF ACCESS: RW */

#if defined(_V1) && !defined(DLF_OFFSET)
#define DLF_OFFSET 0xc0
#endif

#if !defined(UART_DLF_OFFSET)
#define UART_DLF_OFFSET 0xc0
#endif

#if defined(_V1) && !defined(DLF_REG)
#define DLF_REG ((volatile UINT32 *) (UART2_BASE + UART_DLF_OFFSET))
#endif

#if defined(_V1) && !defined(DLF_VAL)
#define DLF_VAL  PREFIX_VAL(DLF_REG)
#endif

#define UART2_DLF_REG ((volatile UINT32 *) (UART2_BASE + UART_DLF_OFFSET))
#define UART2_DLF_VAL  PREFIX_VAL(UART2_DLF_REG)

/* FIELDS: */

/* DLF ACCESS: RW */

#ifndef UART_DLF_DLF_POS
#define UART_DLF_DLF_POS      0
#endif

#ifndef UART_DLF_DLF_LEN
#define UART_DLF_DLF_LEN      6
#endif

#if defined(_V1) && !defined(DLF_DLF_R)
#define DLF_DLF_R        GetGroupBits32( (UART2_DLF_VAL),UART_DLF_DLF_POS, UART_DLF_DLF_LEN)
#endif

#if defined(_V1) && !defined(DLF_DLF_W)
#define DLF_DLF_W(value) SetGroupBits32( (UART2_DLF_VAL),UART_DLF_DLF_POS, UART_DLF_DLF_LEN,value)
#endif

#define UART2_DLF_DLF_R        GetGroupBits32( (UART2_DLF_VAL),UART_DLF_DLF_POS, UART_DLF_DLF_LEN)

#define UART2_DLF_DLF_W(value) SetGroupBits32( (UART2_DLF_VAL),UART_DLF_DLF_POS, UART_DLF_DLF_LEN,value)


/* RSVD_DLF ACCESS: RW */

#ifndef UART_DLF_RSVD_DLF_POS
#define UART_DLF_RSVD_DLF_POS      6
#endif

#ifndef UART_DLF_RSVD_DLF_LEN
#define UART_DLF_RSVD_DLF_LEN      26
#endif

#if defined(_V1) && !defined(DLF_RSVD_DLF_R)
#define DLF_RSVD_DLF_R        GetGroupBits32( (UART2_DLF_VAL),UART_DLF_RSVD_DLF_POS, UART_DLF_RSVD_DLF_LEN)
#endif

#if defined(_V1) && !defined(DLF_RSVD_DLF_W)
#define DLF_RSVD_DLF_W(value) SetGroupBits32( (UART2_DLF_VAL),UART_DLF_RSVD_DLF_POS, UART_DLF_RSVD_DLF_LEN,value)
#endif

#define UART2_DLF_RSVD_DLF_R        GetGroupBits32( (UART2_DLF_VAL),UART_DLF_RSVD_DLF_POS, UART_DLF_RSVD_DLF_LEN)

#define UART2_DLF_RSVD_DLF_W(value) SetGroupBits32( (UART2_DLF_VAL),UART_DLF_RSVD_DLF_POS, UART_DLF_RSVD_DLF_LEN,value)


/* REGISTER: CPR ACCESS: RW */

#if defined(_V1) && !defined(CPR_OFFSET)
#define CPR_OFFSET 0xf4
#endif

#if !defined(UART_CPR_OFFSET)
#define UART_CPR_OFFSET 0xf4
#endif

#if defined(_V1) && !defined(CPR_REG)
#define CPR_REG ((volatile UINT32 *) (UART2_BASE + UART_CPR_OFFSET))
#endif

#if defined(_V1) && !defined(CPR_VAL)
#define CPR_VAL  PREFIX_VAL(CPR_REG)
#endif

#define UART2_CPR_REG ((volatile UINT32 *) (UART2_BASE + UART_CPR_OFFSET))
#define UART2_CPR_VAL  PREFIX_VAL(UART2_CPR_REG)

/* FIELDS: */

/* APB_DATA_WIDTH ACCESS: RW */

#ifndef UART_CPR_APB_DATA_WIDTH_POS
#define UART_CPR_APB_DATA_WIDTH_POS      0
#endif

#ifndef UART_CPR_APB_DATA_WIDTH_LEN
#define UART_CPR_APB_DATA_WIDTH_LEN      2
#endif

#if defined(_V1) && !defined(CPR_APB_DATA_WIDTH_R)
#define CPR_APB_DATA_WIDTH_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_APB_DATA_WIDTH_POS, UART_CPR_APB_DATA_WIDTH_LEN)
#endif

#if defined(_V1) && !defined(CPR_APB_DATA_WIDTH_W)
#define CPR_APB_DATA_WIDTH_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_APB_DATA_WIDTH_POS, UART_CPR_APB_DATA_WIDTH_LEN,value)
#endif

#define UART2_CPR_APB_DATA_WIDTH_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_APB_DATA_WIDTH_POS, UART_CPR_APB_DATA_WIDTH_LEN)

#define UART2_CPR_APB_DATA_WIDTH_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_APB_DATA_WIDTH_POS, UART_CPR_APB_DATA_WIDTH_LEN,value)


/* RSVD_CPR_3to2 ACCESS: RW */

#ifndef UART_CPR_RSVD_CPR_3TO2_POS
#define UART_CPR_RSVD_CPR_3TO2_POS      2
#endif

#ifndef UART_CPR_RSVD_CPR_3TO2_LEN
#define UART_CPR_RSVD_CPR_3TO2_LEN      2
#endif

#if defined(_V1) && !defined(CPR_RSVD_CPR_3TO2_R)
#define CPR_RSVD_CPR_3TO2_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_RSVD_CPR_3TO2_POS, UART_CPR_RSVD_CPR_3TO2_LEN)
#endif

#if defined(_V1) && !defined(CPR_RSVD_CPR_3TO2_W)
#define CPR_RSVD_CPR_3TO2_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_RSVD_CPR_3TO2_POS, UART_CPR_RSVD_CPR_3TO2_LEN,value)
#endif

#define UART2_CPR_RSVD_CPR_3TO2_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_RSVD_CPR_3TO2_POS, UART_CPR_RSVD_CPR_3TO2_LEN)

#define UART2_CPR_RSVD_CPR_3TO2_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_RSVD_CPR_3TO2_POS, UART_CPR_RSVD_CPR_3TO2_LEN,value)


/* AFCE_MODE ACCESS: RW */

#ifndef UART_CPR_AFCE_MODE_POS
#define UART_CPR_AFCE_MODE_POS      4
#endif

#ifndef UART_CPR_AFCE_MODE_LEN
#define UART_CPR_AFCE_MODE_LEN      1
#endif

#if defined(_V1) && !defined(CPR_AFCE_MODE_R)
#define CPR_AFCE_MODE_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_AFCE_MODE_POS, UART_CPR_AFCE_MODE_LEN)
#endif

#if defined(_V1) && !defined(CPR_AFCE_MODE_W)
#define CPR_AFCE_MODE_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_AFCE_MODE_POS, UART_CPR_AFCE_MODE_LEN,value)
#endif

#define UART2_CPR_AFCE_MODE_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_AFCE_MODE_POS, UART_CPR_AFCE_MODE_LEN)

#define UART2_CPR_AFCE_MODE_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_AFCE_MODE_POS, UART_CPR_AFCE_MODE_LEN,value)


/* THRE_MODE ACCESS: RW */

#ifndef UART_CPR_THRE_MODE_POS
#define UART_CPR_THRE_MODE_POS      5
#endif

#ifndef UART_CPR_THRE_MODE_LEN
#define UART_CPR_THRE_MODE_LEN      1
#endif

#if defined(_V1) && !defined(CPR_THRE_MODE_R)
#define CPR_THRE_MODE_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_THRE_MODE_POS, UART_CPR_THRE_MODE_LEN)
#endif

#if defined(_V1) && !defined(CPR_THRE_MODE_W)
#define CPR_THRE_MODE_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_THRE_MODE_POS, UART_CPR_THRE_MODE_LEN,value)
#endif

#define UART2_CPR_THRE_MODE_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_THRE_MODE_POS, UART_CPR_THRE_MODE_LEN)

#define UART2_CPR_THRE_MODE_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_THRE_MODE_POS, UART_CPR_THRE_MODE_LEN,value)


/* SIR_MODE ACCESS: RW */

#ifndef UART_CPR_SIR_MODE_POS
#define UART_CPR_SIR_MODE_POS      6
#endif

#ifndef UART_CPR_SIR_MODE_LEN
#define UART_CPR_SIR_MODE_LEN      1
#endif

#if defined(_V1) && !defined(CPR_SIR_MODE_R)
#define CPR_SIR_MODE_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_SIR_MODE_POS, UART_CPR_SIR_MODE_LEN)
#endif

#if defined(_V1) && !defined(CPR_SIR_MODE_W)
#define CPR_SIR_MODE_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_SIR_MODE_POS, UART_CPR_SIR_MODE_LEN,value)
#endif

#define UART2_CPR_SIR_MODE_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_SIR_MODE_POS, UART_CPR_SIR_MODE_LEN)

#define UART2_CPR_SIR_MODE_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_SIR_MODE_POS, UART_CPR_SIR_MODE_LEN,value)


/* SIR_LP_MODE ACCESS: RW */

#ifndef UART_CPR_SIR_LP_MODE_POS
#define UART_CPR_SIR_LP_MODE_POS      7
#endif

#ifndef UART_CPR_SIR_LP_MODE_LEN
#define UART_CPR_SIR_LP_MODE_LEN      1
#endif

#if defined(_V1) && !defined(CPR_SIR_LP_MODE_R)
#define CPR_SIR_LP_MODE_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_SIR_LP_MODE_POS, UART_CPR_SIR_LP_MODE_LEN)
#endif

#if defined(_V1) && !defined(CPR_SIR_LP_MODE_W)
#define CPR_SIR_LP_MODE_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_SIR_LP_MODE_POS, UART_CPR_SIR_LP_MODE_LEN,value)
#endif

#define UART2_CPR_SIR_LP_MODE_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_SIR_LP_MODE_POS, UART_CPR_SIR_LP_MODE_LEN)

#define UART2_CPR_SIR_LP_MODE_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_SIR_LP_MODE_POS, UART_CPR_SIR_LP_MODE_LEN,value)


/* ADDITIONAL_FEAT ACCESS: RW */

#ifndef UART_CPR_ADDITIONAL_FEAT_POS
#define UART_CPR_ADDITIONAL_FEAT_POS      8
#endif

#ifndef UART_CPR_ADDITIONAL_FEAT_LEN
#define UART_CPR_ADDITIONAL_FEAT_LEN      1
#endif

#if defined(_V1) && !defined(CPR_ADDITIONAL_FEAT_R)
#define CPR_ADDITIONAL_FEAT_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_ADDITIONAL_FEAT_POS, UART_CPR_ADDITIONAL_FEAT_LEN)
#endif

#if defined(_V1) && !defined(CPR_ADDITIONAL_FEAT_W)
#define CPR_ADDITIONAL_FEAT_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_ADDITIONAL_FEAT_POS, UART_CPR_ADDITIONAL_FEAT_LEN,value)
#endif

#define UART2_CPR_ADDITIONAL_FEAT_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_ADDITIONAL_FEAT_POS, UART_CPR_ADDITIONAL_FEAT_LEN)

#define UART2_CPR_ADDITIONAL_FEAT_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_ADDITIONAL_FEAT_POS, UART_CPR_ADDITIONAL_FEAT_LEN,value)


/* FIFO_ACCESS ACCESS: RW */

#ifndef UART_CPR_FIFO_ACCESS_POS
#define UART_CPR_FIFO_ACCESS_POS      9
#endif

#ifndef UART_CPR_FIFO_ACCESS_LEN
#define UART_CPR_FIFO_ACCESS_LEN      1
#endif

#if defined(_V1) && !defined(CPR_FIFO_ACCESS_R)
#define CPR_FIFO_ACCESS_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_FIFO_ACCESS_POS, UART_CPR_FIFO_ACCESS_LEN)
#endif

#if defined(_V1) && !defined(CPR_FIFO_ACCESS_W)
#define CPR_FIFO_ACCESS_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_FIFO_ACCESS_POS, UART_CPR_FIFO_ACCESS_LEN,value)
#endif

#define UART2_CPR_FIFO_ACCESS_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_FIFO_ACCESS_POS, UART_CPR_FIFO_ACCESS_LEN)

#define UART2_CPR_FIFO_ACCESS_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_FIFO_ACCESS_POS, UART_CPR_FIFO_ACCESS_LEN,value)


/* FIFO_STAT ACCESS: RW */

#ifndef UART_CPR_FIFO_STAT_POS
#define UART_CPR_FIFO_STAT_POS      10
#endif

#ifndef UART_CPR_FIFO_STAT_LEN
#define UART_CPR_FIFO_STAT_LEN      1
#endif

#if defined(_V1) && !defined(CPR_FIFO_STAT_R)
#define CPR_FIFO_STAT_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_FIFO_STAT_POS, UART_CPR_FIFO_STAT_LEN)
#endif

#if defined(_V1) && !defined(CPR_FIFO_STAT_W)
#define CPR_FIFO_STAT_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_FIFO_STAT_POS, UART_CPR_FIFO_STAT_LEN,value)
#endif

#define UART2_CPR_FIFO_STAT_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_FIFO_STAT_POS, UART_CPR_FIFO_STAT_LEN)

#define UART2_CPR_FIFO_STAT_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_FIFO_STAT_POS, UART_CPR_FIFO_STAT_LEN,value)


/* SHADOW ACCESS: RW */

#ifndef UART_CPR_SHADOW_POS
#define UART_CPR_SHADOW_POS      11
#endif

#ifndef UART_CPR_SHADOW_LEN
#define UART_CPR_SHADOW_LEN      1
#endif

#if defined(_V1) && !defined(CPR_SHADOW_R)
#define CPR_SHADOW_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_SHADOW_POS, UART_CPR_SHADOW_LEN)
#endif

#if defined(_V1) && !defined(CPR_SHADOW_W)
#define CPR_SHADOW_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_SHADOW_POS, UART_CPR_SHADOW_LEN,value)
#endif

#define UART2_CPR_SHADOW_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_SHADOW_POS, UART_CPR_SHADOW_LEN)

#define UART2_CPR_SHADOW_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_SHADOW_POS, UART_CPR_SHADOW_LEN,value)


/* UART_ADD_ENCODED_PARAMS ACCESS: RW */

#ifndef UART_CPR_UART_ADD_ENCODED_PARAMS_POS
#define UART_CPR_UART_ADD_ENCODED_PARAMS_POS      12
#endif

#ifndef UART_CPR_UART_ADD_ENCODED_PARAMS_LEN
#define UART_CPR_UART_ADD_ENCODED_PARAMS_LEN      1
#endif

#if defined(_V1) && !defined(CPR_UART_ADD_ENCODED_PARAMS_R)
#define CPR_UART_ADD_ENCODED_PARAMS_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_UART_ADD_ENCODED_PARAMS_POS, UART_CPR_UART_ADD_ENCODED_PARAMS_LEN)
#endif

#if defined(_V1) && !defined(CPR_UART_ADD_ENCODED_PARAMS_W)
#define CPR_UART_ADD_ENCODED_PARAMS_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_UART_ADD_ENCODED_PARAMS_POS, UART_CPR_UART_ADD_ENCODED_PARAMS_LEN,value)
#endif

#define UART2_CPR_UART_ADD_ENCODED_PARAMS_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_UART_ADD_ENCODED_PARAMS_POS, UART_CPR_UART_ADD_ENCODED_PARAMS_LEN)

#define UART2_CPR_UART_ADD_ENCODED_PARAMS_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_UART_ADD_ENCODED_PARAMS_POS, UART_CPR_UART_ADD_ENCODED_PARAMS_LEN,value)


/* DMA_EXTRA ACCESS: RW */

#ifndef UART_CPR_DMA_EXTRA_POS
#define UART_CPR_DMA_EXTRA_POS      13
#endif

#ifndef UART_CPR_DMA_EXTRA_LEN
#define UART_CPR_DMA_EXTRA_LEN      1
#endif

#if defined(_V1) && !defined(CPR_DMA_EXTRA_R)
#define CPR_DMA_EXTRA_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_DMA_EXTRA_POS, UART_CPR_DMA_EXTRA_LEN)
#endif

#if defined(_V1) && !defined(CPR_DMA_EXTRA_W)
#define CPR_DMA_EXTRA_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_DMA_EXTRA_POS, UART_CPR_DMA_EXTRA_LEN,value)
#endif

#define UART2_CPR_DMA_EXTRA_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_DMA_EXTRA_POS, UART_CPR_DMA_EXTRA_LEN)

#define UART2_CPR_DMA_EXTRA_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_DMA_EXTRA_POS, UART_CPR_DMA_EXTRA_LEN,value)


/* RSVD_CPR_15to14 ACCESS: RW */

#ifndef UART_CPR_RSVD_CPR_15TO14_POS
#define UART_CPR_RSVD_CPR_15TO14_POS      14
#endif

#ifndef UART_CPR_RSVD_CPR_15TO14_LEN
#define UART_CPR_RSVD_CPR_15TO14_LEN      2
#endif

#if defined(_V1) && !defined(CPR_RSVD_CPR_15TO14_R)
#define CPR_RSVD_CPR_15TO14_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_RSVD_CPR_15TO14_POS, UART_CPR_RSVD_CPR_15TO14_LEN)
#endif

#if defined(_V1) && !defined(CPR_RSVD_CPR_15TO14_W)
#define CPR_RSVD_CPR_15TO14_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_RSVD_CPR_15TO14_POS, UART_CPR_RSVD_CPR_15TO14_LEN,value)
#endif

#define UART2_CPR_RSVD_CPR_15TO14_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_RSVD_CPR_15TO14_POS, UART_CPR_RSVD_CPR_15TO14_LEN)

#define UART2_CPR_RSVD_CPR_15TO14_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_RSVD_CPR_15TO14_POS, UART_CPR_RSVD_CPR_15TO14_LEN,value)


/* FIFO_MODE ACCESS: RW */

#ifndef UART_CPR_FIFO_MODE_POS
#define UART_CPR_FIFO_MODE_POS      16
#endif

#ifndef UART_CPR_FIFO_MODE_LEN
#define UART_CPR_FIFO_MODE_LEN      8
#endif

#if defined(_V1) && !defined(CPR_FIFO_MODE_R)
#define CPR_FIFO_MODE_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_FIFO_MODE_POS, UART_CPR_FIFO_MODE_LEN)
#endif

#if defined(_V1) && !defined(CPR_FIFO_MODE_W)
#define CPR_FIFO_MODE_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_FIFO_MODE_POS, UART_CPR_FIFO_MODE_LEN,value)
#endif

#define UART2_CPR_FIFO_MODE_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_FIFO_MODE_POS, UART_CPR_FIFO_MODE_LEN)

#define UART2_CPR_FIFO_MODE_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_FIFO_MODE_POS, UART_CPR_FIFO_MODE_LEN,value)


/* RSVD_CPR_31to24 ACCESS: RW */

#ifndef UART_CPR_RSVD_CPR_31TO24_POS
#define UART_CPR_RSVD_CPR_31TO24_POS      24
#endif

#ifndef UART_CPR_RSVD_CPR_31TO24_LEN
#define UART_CPR_RSVD_CPR_31TO24_LEN      8
#endif

#if defined(_V1) && !defined(CPR_RSVD_CPR_31TO24_R)
#define CPR_RSVD_CPR_31TO24_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_RSVD_CPR_31TO24_POS, UART_CPR_RSVD_CPR_31TO24_LEN)
#endif

#if defined(_V1) && !defined(CPR_RSVD_CPR_31TO24_W)
#define CPR_RSVD_CPR_31TO24_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_RSVD_CPR_31TO24_POS, UART_CPR_RSVD_CPR_31TO24_LEN,value)
#endif

#define UART2_CPR_RSVD_CPR_31TO24_R        GetGroupBits32( (UART2_CPR_VAL),UART_CPR_RSVD_CPR_31TO24_POS, UART_CPR_RSVD_CPR_31TO24_LEN)

#define UART2_CPR_RSVD_CPR_31TO24_W(value) SetGroupBits32( (UART2_CPR_VAL),UART_CPR_RSVD_CPR_31TO24_POS, UART_CPR_RSVD_CPR_31TO24_LEN,value)


/* REGISTER: UCV ACCESS: RW */

#if defined(_V1) && !defined(UCV_OFFSET)
#define UCV_OFFSET 0xf8
#endif

#if !defined(UART_UCV_OFFSET)
#define UART_UCV_OFFSET 0xf8
#endif

#if defined(_V1) && !defined(UCV_REG)
#define UCV_REG ((volatile UINT32 *) (UART2_BASE + UART_UCV_OFFSET))
#endif

#if defined(_V1) && !defined(UCV_VAL)
#define UCV_VAL  PREFIX_VAL(UCV_REG)
#endif

#define UART2_UCV_REG ((volatile UINT32 *) (UART2_BASE + UART_UCV_OFFSET))
#define UART2_UCV_VAL  PREFIX_VAL(UART2_UCV_REG)

/* FIELDS: */

/* UART_Component_Version ACCESS: RW */

#ifndef UART_UCV_UART_COMPONENT_VERSION_POS
#define UART_UCV_UART_COMPONENT_VERSION_POS      0
#endif

#ifndef UART_UCV_UART_COMPONENT_VERSION_LEN
#define UART_UCV_UART_COMPONENT_VERSION_LEN      32
#endif

#if defined(_V1) && !defined(UCV_UART_COMPONENT_VERSION_R)
#define UCV_UART_COMPONENT_VERSION_R        GetGroupBits32( (UART2_UCV_VAL),UART_UCV_UART_COMPONENT_VERSION_POS, UART_UCV_UART_COMPONENT_VERSION_LEN)
#endif

#if defined(_V1) && !defined(UCV_UART_COMPONENT_VERSION_W)
#define UCV_UART_COMPONENT_VERSION_W(value) SetGroupBits32( (UART2_UCV_VAL),UART_UCV_UART_COMPONENT_VERSION_POS, UART_UCV_UART_COMPONENT_VERSION_LEN,value)
#endif

#define UART2_UCV_UART_COMPONENT_VERSION_R        GetGroupBits32( (UART2_UCV_VAL),UART_UCV_UART_COMPONENT_VERSION_POS, UART_UCV_UART_COMPONENT_VERSION_LEN)

#define UART2_UCV_UART_COMPONENT_VERSION_W(value) SetGroupBits32( (UART2_UCV_VAL),UART_UCV_UART_COMPONENT_VERSION_POS, UART_UCV_UART_COMPONENT_VERSION_LEN,value)


/* REGISTER: CTR ACCESS: RW */

#if defined(_V1) && !defined(CTR_OFFSET)
#define CTR_OFFSET 0xfc
#endif

#if !defined(UART_CTR_OFFSET)
#define UART_CTR_OFFSET 0xfc
#endif

#if defined(_V1) && !defined(CTR_REG)
#define CTR_REG ((volatile UINT32 *) (UART2_BASE + UART_CTR_OFFSET))
#endif

#if defined(_V1) && !defined(CTR_VAL)
#define CTR_VAL  PREFIX_VAL(CTR_REG)
#endif

#define UART2_CTR_REG ((volatile UINT32 *) (UART2_BASE + UART_CTR_OFFSET))
#define UART2_CTR_VAL  PREFIX_VAL(UART2_CTR_REG)

/* FIELDS: */

/* Peripheral_ID ACCESS: RW */

#ifndef UART_CTR_PERIPHERAL_ID_POS
#define UART_CTR_PERIPHERAL_ID_POS      0
#endif

#ifndef UART_CTR_PERIPHERAL_ID_LEN
#define UART_CTR_PERIPHERAL_ID_LEN      32
#endif

#if defined(_V1) && !defined(CTR_PERIPHERAL_ID_R)
#define CTR_PERIPHERAL_ID_R        GetGroupBits32( (UART2_CTR_VAL),UART_CTR_PERIPHERAL_ID_POS, UART_CTR_PERIPHERAL_ID_LEN)
#endif

#if defined(_V1) && !defined(CTR_PERIPHERAL_ID_W)
#define CTR_PERIPHERAL_ID_W(value) SetGroupBits32( (UART2_CTR_VAL),UART_CTR_PERIPHERAL_ID_POS, UART_CTR_PERIPHERAL_ID_LEN,value)
#endif

#define UART2_CTR_PERIPHERAL_ID_R        GetGroupBits32( (UART2_CTR_VAL),UART_CTR_PERIPHERAL_ID_POS, UART_CTR_PERIPHERAL_ID_LEN)

#define UART2_CTR_PERIPHERAL_ID_W(value) SetGroupBits32( (UART2_CTR_VAL),UART_CTR_PERIPHERAL_ID_POS, UART_CTR_PERIPHERAL_ID_LEN,value)


/* OFFSET TABLE: */
#define uart2_offset_tbl_values	RBR_OFFSET, MCR_OFFSET, LSR_OFFSET, MSR_OFFSET, SCR_OFFSET, SRBR0_OFFSET, SRBR1_OFFSET, SRBR2_OFFSET, SRBR3_OFFSET, IER_OFFSET, SRBR4_OFFSET, SRBR5_OFFSET, SRBR6_OFFSET, SRBR7_OFFSET, SRBR8_OFFSET, SRBR9_OFFSET, SRBR10_OFFSET, SRBR11_OFFSET, SRBR12_OFFSET, SRBR13_OFFSET, SRBR14_OFFSET, SRBR15_OFFSET, FAR_OFFSET, TFR_OFFSET, RFW_OFFSET, USR_OFFSET, IIR_OFFSET, TFL_OFFSET, RFL_OFFSET, SRR_OFFSET, SRTS_OFFSET, SBCR_OFFSET, SDMAM_OFFSET, SFE_OFFSET, SRT_OFFSET, STET_OFFSET, HTX_OFFSET, DMASA_OFFSET, LCR_OFFSET, DLF_OFFSET, CPR_OFFSET, UCV_OFFSET, CTR_OFFSET


/* REGISTERS RESET VAL: */
#define uart2_regs_reset_val	0x0, 0x0, 0x60, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x6, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x43F32, 0x3430302A, 0x44570110

#endif

/* End of UART2 */
/* ///////////////////////////////////////////////////////////////////////*/
