#ifndef NU4100_DMA0_REG_H
#define NU4100_DMA0_REG_H


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
/*   DMA0 (Prototype: DMA)                */
/*****************************************/
#define DMA0_BASE 0x08140000

/* REGISTER: DSR ACCESS: RO */

#if defined(_V1) && !defined(DSR_OFFSET)
#define DSR_OFFSET 0x0
#endif

#if !defined(DMA_DSR_OFFSET)
#define DMA_DSR_OFFSET 0x0
#endif

#if defined(_V1) && !defined(DSR_REG)
#define DSR_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DSR_OFFSET))
#endif

#if defined(_V1) && !defined(DSR_VAL)
#define DSR_VAL  PREFIX_VAL(DSR_REG)
#endif

#define DMA0_DSR_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DSR_OFFSET))
#define DMA0_DSR_VAL  PREFIX_VAL(DMA0_DSR_REG)

/* FIELDS: */

/* DMA_status ACCESS: RO */

#ifndef DMA_DSR_DMA_STATUS_POS
#define DMA_DSR_DMA_STATUS_POS      0
#endif

#ifndef DMA_DSR_DMA_STATUS_LEN
#define DMA_DSR_DMA_STATUS_LEN      4
#endif

#if defined(_V1) && !defined(DSR_DMA_STATUS_R)
#define DSR_DMA_STATUS_R        GetGroupBits32( (DMA0_DSR_VAL),DMA_DSR_DMA_STATUS_POS, DMA_DSR_DMA_STATUS_LEN)
#endif

#if defined(_V1) && !defined(DSR_DMA_STATUS_W)
#define DSR_DMA_STATUS_W(value) SetGroupBits32( (DMA0_DSR_VAL),DMA_DSR_DMA_STATUS_POS, DMA_DSR_DMA_STATUS_LEN,value)
#endif

#define DMA0_DSR_DMA_STATUS_R        GetGroupBits32( (DMA0_DSR_VAL),DMA_DSR_DMA_STATUS_POS, DMA_DSR_DMA_STATUS_LEN)

#define DMA0_DSR_DMA_STATUS_W(value) SetGroupBits32( (DMA0_DSR_VAL),DMA_DSR_DMA_STATUS_POS, DMA_DSR_DMA_STATUS_LEN,value)


/* Wakeup_event ACCESS: RO */

#ifndef DMA_DSR_WAKEUP_EVENT_POS
#define DMA_DSR_WAKEUP_EVENT_POS      4
#endif

#ifndef DMA_DSR_WAKEUP_EVENT_LEN
#define DMA_DSR_WAKEUP_EVENT_LEN      5
#endif

#if defined(_V1) && !defined(DSR_WAKEUP_EVENT_R)
#define DSR_WAKEUP_EVENT_R        GetGroupBits32( (DMA0_DSR_VAL),DMA_DSR_WAKEUP_EVENT_POS, DMA_DSR_WAKEUP_EVENT_LEN)
#endif

#if defined(_V1) && !defined(DSR_WAKEUP_EVENT_W)
#define DSR_WAKEUP_EVENT_W(value) SetGroupBits32( (DMA0_DSR_VAL),DMA_DSR_WAKEUP_EVENT_POS, DMA_DSR_WAKEUP_EVENT_LEN,value)
#endif

#define DMA0_DSR_WAKEUP_EVENT_R        GetGroupBits32( (DMA0_DSR_VAL),DMA_DSR_WAKEUP_EVENT_POS, DMA_DSR_WAKEUP_EVENT_LEN)

#define DMA0_DSR_WAKEUP_EVENT_W(value) SetGroupBits32( (DMA0_DSR_VAL),DMA_DSR_WAKEUP_EVENT_POS, DMA_DSR_WAKEUP_EVENT_LEN,value)


/* DNS ACCESS: RO */

#ifndef DMA_DSR_DNS_POS
#define DMA_DSR_DNS_POS      9
#endif

#ifndef DMA_DSR_DNS_LEN
#define DMA_DSR_DNS_LEN      1
#endif

#if defined(_V1) && !defined(DSR_DNS_R)
#define DSR_DNS_R        GetGroupBits32( (DMA0_DSR_VAL),DMA_DSR_DNS_POS, DMA_DSR_DNS_LEN)
#endif

#if defined(_V1) && !defined(DSR_DNS_W)
#define DSR_DNS_W(value) SetGroupBits32( (DMA0_DSR_VAL),DMA_DSR_DNS_POS, DMA_DSR_DNS_LEN,value)
#endif

#define DMA0_DSR_DNS_R        GetGroupBits32( (DMA0_DSR_VAL),DMA_DSR_DNS_POS, DMA_DSR_DNS_LEN)

#define DMA0_DSR_DNS_W(value) SetGroupBits32( (DMA0_DSR_VAL),DMA_DSR_DNS_POS, DMA_DSR_DNS_LEN,value)


/* REGISTER: CSR0 ACCESS: RO */

#if defined(_V1) && !defined(CSR0_OFFSET)
#define CSR0_OFFSET 0x100
#endif

#if !defined(DMA_CSR0_OFFSET)
#define DMA_CSR0_OFFSET 0x100
#endif

#if defined(_V1) && !defined(CSR0_REG)
#define CSR0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR0_OFFSET))
#endif

#if defined(_V1) && !defined(CSR0_VAL)
#define CSR0_VAL  PREFIX_VAL(CSR0_REG)
#endif

#define DMA0_CSR0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR0_OFFSET))
#define DMA0_CSR0_VAL  PREFIX_VAL(DMA0_CSR0_REG)

/* FIELDS: */

/* Channel_status ACCESS: RO */

#ifndef DMA_CSR0_CHANNEL_STATUS_POS
#define DMA_CSR0_CHANNEL_STATUS_POS      0
#endif

#ifndef DMA_CSR0_CHANNEL_STATUS_LEN
#define DMA_CSR0_CHANNEL_STATUS_LEN      4
#endif

#if defined(_V1) && !defined(CSR0_CHANNEL_STATUS_R)
#define CSR0_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_CHANNEL_STATUS_POS, DMA_CSR0_CHANNEL_STATUS_LEN)
#endif

#if defined(_V1) && !defined(CSR0_CHANNEL_STATUS_W)
#define CSR0_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_CHANNEL_STATUS_POS, DMA_CSR0_CHANNEL_STATUS_LEN,value)
#endif

#define DMA0_CSR0_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_CHANNEL_STATUS_POS, DMA_CSR0_CHANNEL_STATUS_LEN)

#define DMA0_CSR0_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_CHANNEL_STATUS_POS, DMA_CSR0_CHANNEL_STATUS_LEN,value)


/* Wakeup_number ACCESS: RO */

#ifndef DMA_CSR0_WAKEUP_NUMBER_POS
#define DMA_CSR0_WAKEUP_NUMBER_POS      4
#endif

#ifndef DMA_CSR0_WAKEUP_NUMBER_LEN
#define DMA_CSR0_WAKEUP_NUMBER_LEN      5
#endif

#if defined(_V1) && !defined(CSR0_WAKEUP_NUMBER_R)
#define CSR0_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_WAKEUP_NUMBER_POS, DMA_CSR0_WAKEUP_NUMBER_LEN)
#endif

#if defined(_V1) && !defined(CSR0_WAKEUP_NUMBER_W)
#define CSR0_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_WAKEUP_NUMBER_POS, DMA_CSR0_WAKEUP_NUMBER_LEN,value)
#endif

#define DMA0_CSR0_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_WAKEUP_NUMBER_POS, DMA_CSR0_WAKEUP_NUMBER_LEN)

#define DMA0_CSR0_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_WAKEUP_NUMBER_POS, DMA_CSR0_WAKEUP_NUMBER_LEN,value)


/* dmawfp_b_ns ACCESS: RO */

#ifndef DMA_CSR0_DMAWFP_B_NS_POS
#define DMA_CSR0_DMAWFP_B_NS_POS      14
#endif

#ifndef DMA_CSR0_DMAWFP_B_NS_LEN
#define DMA_CSR0_DMAWFP_B_NS_LEN      1
#endif

#if defined(_V1) && !defined(CSR0_DMAWFP_B_NS_R)
#define CSR0_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_DMAWFP_B_NS_POS, DMA_CSR0_DMAWFP_B_NS_LEN)
#endif

#if defined(_V1) && !defined(CSR0_DMAWFP_B_NS_W)
#define CSR0_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_DMAWFP_B_NS_POS, DMA_CSR0_DMAWFP_B_NS_LEN,value)
#endif

#define DMA0_CSR0_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_DMAWFP_B_NS_POS, DMA_CSR0_DMAWFP_B_NS_LEN)

#define DMA0_CSR0_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_DMAWFP_B_NS_POS, DMA_CSR0_DMAWFP_B_NS_LEN,value)


/* dmawfp_periph ACCESS: RO */

#ifndef DMA_CSR0_DMAWFP_PERIPH_POS
#define DMA_CSR0_DMAWFP_PERIPH_POS      15
#endif

#ifndef DMA_CSR0_DMAWFP_PERIPH_LEN
#define DMA_CSR0_DMAWFP_PERIPH_LEN      1
#endif

#if defined(_V1) && !defined(CSR0_DMAWFP_PERIPH_R)
#define CSR0_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_DMAWFP_PERIPH_POS, DMA_CSR0_DMAWFP_PERIPH_LEN)
#endif

#if defined(_V1) && !defined(CSR0_DMAWFP_PERIPH_W)
#define CSR0_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_DMAWFP_PERIPH_POS, DMA_CSR0_DMAWFP_PERIPH_LEN,value)
#endif

#define DMA0_CSR0_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_DMAWFP_PERIPH_POS, DMA_CSR0_DMAWFP_PERIPH_LEN)

#define DMA0_CSR0_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_DMAWFP_PERIPH_POS, DMA_CSR0_DMAWFP_PERIPH_LEN,value)


/* CNS ACCESS: RO */

#ifndef DMA_CSR0_CNS_POS
#define DMA_CSR0_CNS_POS      21
#endif

#ifndef DMA_CSR0_CNS_LEN
#define DMA_CSR0_CNS_LEN      1
#endif

#if defined(_V1) && !defined(CSR0_CNS_R)
#define CSR0_CNS_R        GetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_CNS_POS, DMA_CSR0_CNS_LEN)
#endif

#if defined(_V1) && !defined(CSR0_CNS_W)
#define CSR0_CNS_W(value) SetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_CNS_POS, DMA_CSR0_CNS_LEN,value)
#endif

#define DMA0_CSR0_CNS_R        GetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_CNS_POS, DMA_CSR0_CNS_LEN)

#define DMA0_CSR0_CNS_W(value) SetGroupBits32( (DMA0_CSR0_VAL),DMA_CSR0_CNS_POS, DMA_CSR0_CNS_LEN,value)


/* REGISTER: CPC0 ACCESS: RO */

#if defined(_V1) && !defined(CPC0_OFFSET)
#define CPC0_OFFSET 0x104
#endif

#if !defined(DMA_CPC0_OFFSET)
#define DMA_CPC0_OFFSET 0x104
#endif

#if defined(_V1) && !defined(CPC0_REG)
#define CPC0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC0_OFFSET))
#endif

#if defined(_V1) && !defined(CPC0_VAL)
#define CPC0_VAL  PREFIX_VAL(CPC0_REG)
#endif

#define DMA0_CPC0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC0_OFFSET))
#define DMA0_CPC0_VAL  PREFIX_VAL(DMA0_CPC0_REG)

/* FIELDS: */

/* pc_chnl ACCESS: RO */

#ifndef DMA_CPC0_PC_CHNL_POS
#define DMA_CPC0_PC_CHNL_POS      0
#endif

#ifndef DMA_CPC0_PC_CHNL_LEN
#define DMA_CPC0_PC_CHNL_LEN      32
#endif

#if defined(_V1) && !defined(CPC0_PC_CHNL_R)
#define CPC0_PC_CHNL_R        GetGroupBits32( (DMA0_CPC0_VAL),DMA_CPC0_PC_CHNL_POS, DMA_CPC0_PC_CHNL_LEN)
#endif

#if defined(_V1) && !defined(CPC0_PC_CHNL_W)
#define CPC0_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC0_VAL),DMA_CPC0_PC_CHNL_POS, DMA_CPC0_PC_CHNL_LEN,value)
#endif

#define DMA0_CPC0_PC_CHNL_R        GetGroupBits32( (DMA0_CPC0_VAL),DMA_CPC0_PC_CHNL_POS, DMA_CPC0_PC_CHNL_LEN)

#define DMA0_CPC0_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC0_VAL),DMA_CPC0_PC_CHNL_POS, DMA_CPC0_PC_CHNL_LEN,value)


/* REGISTER: CSR1 ACCESS: RO */

#if defined(_V1) && !defined(CSR1_OFFSET)
#define CSR1_OFFSET 0x108
#endif

#if !defined(DMA_CSR1_OFFSET)
#define DMA_CSR1_OFFSET 0x108
#endif

#if defined(_V1) && !defined(CSR1_REG)
#define CSR1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR1_OFFSET))
#endif

#if defined(_V1) && !defined(CSR1_VAL)
#define CSR1_VAL  PREFIX_VAL(CSR1_REG)
#endif

#define DMA0_CSR1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR1_OFFSET))
#define DMA0_CSR1_VAL  PREFIX_VAL(DMA0_CSR1_REG)

/* FIELDS: */

/* Channel_status ACCESS: RO */

#ifndef DMA_CSR1_CHANNEL_STATUS_POS
#define DMA_CSR1_CHANNEL_STATUS_POS      0
#endif

#ifndef DMA_CSR1_CHANNEL_STATUS_LEN
#define DMA_CSR1_CHANNEL_STATUS_LEN      4
#endif

#if defined(_V1) && !defined(CSR1_CHANNEL_STATUS_R)
#define CSR1_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_CHANNEL_STATUS_POS, DMA_CSR1_CHANNEL_STATUS_LEN)
#endif

#if defined(_V1) && !defined(CSR1_CHANNEL_STATUS_W)
#define CSR1_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_CHANNEL_STATUS_POS, DMA_CSR1_CHANNEL_STATUS_LEN,value)
#endif

#define DMA0_CSR1_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_CHANNEL_STATUS_POS, DMA_CSR1_CHANNEL_STATUS_LEN)

#define DMA0_CSR1_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_CHANNEL_STATUS_POS, DMA_CSR1_CHANNEL_STATUS_LEN,value)


/* Wakeup_number ACCESS: RO */

#ifndef DMA_CSR1_WAKEUP_NUMBER_POS
#define DMA_CSR1_WAKEUP_NUMBER_POS      4
#endif

#ifndef DMA_CSR1_WAKEUP_NUMBER_LEN
#define DMA_CSR1_WAKEUP_NUMBER_LEN      5
#endif

#if defined(_V1) && !defined(CSR1_WAKEUP_NUMBER_R)
#define CSR1_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_WAKEUP_NUMBER_POS, DMA_CSR1_WAKEUP_NUMBER_LEN)
#endif

#if defined(_V1) && !defined(CSR1_WAKEUP_NUMBER_W)
#define CSR1_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_WAKEUP_NUMBER_POS, DMA_CSR1_WAKEUP_NUMBER_LEN,value)
#endif

#define DMA0_CSR1_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_WAKEUP_NUMBER_POS, DMA_CSR1_WAKEUP_NUMBER_LEN)

#define DMA0_CSR1_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_WAKEUP_NUMBER_POS, DMA_CSR1_WAKEUP_NUMBER_LEN,value)


/* dmawfp_b_ns ACCESS: RO */

#ifndef DMA_CSR1_DMAWFP_B_NS_POS
#define DMA_CSR1_DMAWFP_B_NS_POS      14
#endif

#ifndef DMA_CSR1_DMAWFP_B_NS_LEN
#define DMA_CSR1_DMAWFP_B_NS_LEN      1
#endif

#if defined(_V1) && !defined(CSR1_DMAWFP_B_NS_R)
#define CSR1_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_DMAWFP_B_NS_POS, DMA_CSR1_DMAWFP_B_NS_LEN)
#endif

#if defined(_V1) && !defined(CSR1_DMAWFP_B_NS_W)
#define CSR1_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_DMAWFP_B_NS_POS, DMA_CSR1_DMAWFP_B_NS_LEN,value)
#endif

#define DMA0_CSR1_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_DMAWFP_B_NS_POS, DMA_CSR1_DMAWFP_B_NS_LEN)

#define DMA0_CSR1_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_DMAWFP_B_NS_POS, DMA_CSR1_DMAWFP_B_NS_LEN,value)


/* dmawfp_periph ACCESS: RO */

#ifndef DMA_CSR1_DMAWFP_PERIPH_POS
#define DMA_CSR1_DMAWFP_PERIPH_POS      15
#endif

#ifndef DMA_CSR1_DMAWFP_PERIPH_LEN
#define DMA_CSR1_DMAWFP_PERIPH_LEN      1
#endif

#if defined(_V1) && !defined(CSR1_DMAWFP_PERIPH_R)
#define CSR1_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_DMAWFP_PERIPH_POS, DMA_CSR1_DMAWFP_PERIPH_LEN)
#endif

#if defined(_V1) && !defined(CSR1_DMAWFP_PERIPH_W)
#define CSR1_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_DMAWFP_PERIPH_POS, DMA_CSR1_DMAWFP_PERIPH_LEN,value)
#endif

#define DMA0_CSR1_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_DMAWFP_PERIPH_POS, DMA_CSR1_DMAWFP_PERIPH_LEN)

#define DMA0_CSR1_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_DMAWFP_PERIPH_POS, DMA_CSR1_DMAWFP_PERIPH_LEN,value)


/* CNS ACCESS: RO */

#ifndef DMA_CSR1_CNS_POS
#define DMA_CSR1_CNS_POS      21
#endif

#ifndef DMA_CSR1_CNS_LEN
#define DMA_CSR1_CNS_LEN      1
#endif

#if defined(_V1) && !defined(CSR1_CNS_R)
#define CSR1_CNS_R        GetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_CNS_POS, DMA_CSR1_CNS_LEN)
#endif

#if defined(_V1) && !defined(CSR1_CNS_W)
#define CSR1_CNS_W(value) SetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_CNS_POS, DMA_CSR1_CNS_LEN,value)
#endif

#define DMA0_CSR1_CNS_R        GetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_CNS_POS, DMA_CSR1_CNS_LEN)

#define DMA0_CSR1_CNS_W(value) SetGroupBits32( (DMA0_CSR1_VAL),DMA_CSR1_CNS_POS, DMA_CSR1_CNS_LEN,value)


/* REGISTER: CPC1 ACCESS: RO */

#if defined(_V1) && !defined(CPC1_OFFSET)
#define CPC1_OFFSET 0x10C
#endif

#if !defined(DMA_CPC1_OFFSET)
#define DMA_CPC1_OFFSET 0x10C
#endif

#if defined(_V1) && !defined(CPC1_REG)
#define CPC1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC1_OFFSET))
#endif

#if defined(_V1) && !defined(CPC1_VAL)
#define CPC1_VAL  PREFIX_VAL(CPC1_REG)
#endif

#define DMA0_CPC1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC1_OFFSET))
#define DMA0_CPC1_VAL  PREFIX_VAL(DMA0_CPC1_REG)

/* FIELDS: */

/* pc_chnl ACCESS: RO */

#ifndef DMA_CPC1_PC_CHNL_POS
#define DMA_CPC1_PC_CHNL_POS      0
#endif

#ifndef DMA_CPC1_PC_CHNL_LEN
#define DMA_CPC1_PC_CHNL_LEN      32
#endif

#if defined(_V1) && !defined(CPC1_PC_CHNL_R)
#define CPC1_PC_CHNL_R        GetGroupBits32( (DMA0_CPC1_VAL),DMA_CPC1_PC_CHNL_POS, DMA_CPC1_PC_CHNL_LEN)
#endif

#if defined(_V1) && !defined(CPC1_PC_CHNL_W)
#define CPC1_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC1_VAL),DMA_CPC1_PC_CHNL_POS, DMA_CPC1_PC_CHNL_LEN,value)
#endif

#define DMA0_CPC1_PC_CHNL_R        GetGroupBits32( (DMA0_CPC1_VAL),DMA_CPC1_PC_CHNL_POS, DMA_CPC1_PC_CHNL_LEN)

#define DMA0_CPC1_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC1_VAL),DMA_CPC1_PC_CHNL_POS, DMA_CPC1_PC_CHNL_LEN,value)


/* REGISTER: CSR2 ACCESS: RO */

#if defined(_V1) && !defined(CSR2_OFFSET)
#define CSR2_OFFSET 0x110
#endif

#if !defined(DMA_CSR2_OFFSET)
#define DMA_CSR2_OFFSET 0x110
#endif

#if defined(_V1) && !defined(CSR2_REG)
#define CSR2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR2_OFFSET))
#endif

#if defined(_V1) && !defined(CSR2_VAL)
#define CSR2_VAL  PREFIX_VAL(CSR2_REG)
#endif

#define DMA0_CSR2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR2_OFFSET))
#define DMA0_CSR2_VAL  PREFIX_VAL(DMA0_CSR2_REG)

/* FIELDS: */

/* Channel_status ACCESS: RO */

#ifndef DMA_CSR2_CHANNEL_STATUS_POS
#define DMA_CSR2_CHANNEL_STATUS_POS      0
#endif

#ifndef DMA_CSR2_CHANNEL_STATUS_LEN
#define DMA_CSR2_CHANNEL_STATUS_LEN      4
#endif

#if defined(_V1) && !defined(CSR2_CHANNEL_STATUS_R)
#define CSR2_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_CHANNEL_STATUS_POS, DMA_CSR2_CHANNEL_STATUS_LEN)
#endif

#if defined(_V1) && !defined(CSR2_CHANNEL_STATUS_W)
#define CSR2_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_CHANNEL_STATUS_POS, DMA_CSR2_CHANNEL_STATUS_LEN,value)
#endif

#define DMA0_CSR2_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_CHANNEL_STATUS_POS, DMA_CSR2_CHANNEL_STATUS_LEN)

#define DMA0_CSR2_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_CHANNEL_STATUS_POS, DMA_CSR2_CHANNEL_STATUS_LEN,value)


/* Wakeup_number ACCESS: RO */

#ifndef DMA_CSR2_WAKEUP_NUMBER_POS
#define DMA_CSR2_WAKEUP_NUMBER_POS      4
#endif

#ifndef DMA_CSR2_WAKEUP_NUMBER_LEN
#define DMA_CSR2_WAKEUP_NUMBER_LEN      5
#endif

#if defined(_V1) && !defined(CSR2_WAKEUP_NUMBER_R)
#define CSR2_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_WAKEUP_NUMBER_POS, DMA_CSR2_WAKEUP_NUMBER_LEN)
#endif

#if defined(_V1) && !defined(CSR2_WAKEUP_NUMBER_W)
#define CSR2_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_WAKEUP_NUMBER_POS, DMA_CSR2_WAKEUP_NUMBER_LEN,value)
#endif

#define DMA0_CSR2_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_WAKEUP_NUMBER_POS, DMA_CSR2_WAKEUP_NUMBER_LEN)

#define DMA0_CSR2_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_WAKEUP_NUMBER_POS, DMA_CSR2_WAKEUP_NUMBER_LEN,value)


/* dmawfp_b_ns ACCESS: RO */

#ifndef DMA_CSR2_DMAWFP_B_NS_POS
#define DMA_CSR2_DMAWFP_B_NS_POS      14
#endif

#ifndef DMA_CSR2_DMAWFP_B_NS_LEN
#define DMA_CSR2_DMAWFP_B_NS_LEN      1
#endif

#if defined(_V1) && !defined(CSR2_DMAWFP_B_NS_R)
#define CSR2_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_DMAWFP_B_NS_POS, DMA_CSR2_DMAWFP_B_NS_LEN)
#endif

#if defined(_V1) && !defined(CSR2_DMAWFP_B_NS_W)
#define CSR2_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_DMAWFP_B_NS_POS, DMA_CSR2_DMAWFP_B_NS_LEN,value)
#endif

#define DMA0_CSR2_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_DMAWFP_B_NS_POS, DMA_CSR2_DMAWFP_B_NS_LEN)

#define DMA0_CSR2_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_DMAWFP_B_NS_POS, DMA_CSR2_DMAWFP_B_NS_LEN,value)


/* dmawfp_periph ACCESS: RO */

#ifndef DMA_CSR2_DMAWFP_PERIPH_POS
#define DMA_CSR2_DMAWFP_PERIPH_POS      15
#endif

#ifndef DMA_CSR2_DMAWFP_PERIPH_LEN
#define DMA_CSR2_DMAWFP_PERIPH_LEN      1
#endif

#if defined(_V1) && !defined(CSR2_DMAWFP_PERIPH_R)
#define CSR2_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_DMAWFP_PERIPH_POS, DMA_CSR2_DMAWFP_PERIPH_LEN)
#endif

#if defined(_V1) && !defined(CSR2_DMAWFP_PERIPH_W)
#define CSR2_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_DMAWFP_PERIPH_POS, DMA_CSR2_DMAWFP_PERIPH_LEN,value)
#endif

#define DMA0_CSR2_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_DMAWFP_PERIPH_POS, DMA_CSR2_DMAWFP_PERIPH_LEN)

#define DMA0_CSR2_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_DMAWFP_PERIPH_POS, DMA_CSR2_DMAWFP_PERIPH_LEN,value)


/* CNS ACCESS: RO */

#ifndef DMA_CSR2_CNS_POS
#define DMA_CSR2_CNS_POS      21
#endif

#ifndef DMA_CSR2_CNS_LEN
#define DMA_CSR2_CNS_LEN      1
#endif

#if defined(_V1) && !defined(CSR2_CNS_R)
#define CSR2_CNS_R        GetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_CNS_POS, DMA_CSR2_CNS_LEN)
#endif

#if defined(_V1) && !defined(CSR2_CNS_W)
#define CSR2_CNS_W(value) SetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_CNS_POS, DMA_CSR2_CNS_LEN,value)
#endif

#define DMA0_CSR2_CNS_R        GetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_CNS_POS, DMA_CSR2_CNS_LEN)

#define DMA0_CSR2_CNS_W(value) SetGroupBits32( (DMA0_CSR2_VAL),DMA_CSR2_CNS_POS, DMA_CSR2_CNS_LEN,value)


/* REGISTER: CPC2 ACCESS: RO */

#if defined(_V1) && !defined(CPC2_OFFSET)
#define CPC2_OFFSET 0x114
#endif

#if !defined(DMA_CPC2_OFFSET)
#define DMA_CPC2_OFFSET 0x114
#endif

#if defined(_V1) && !defined(CPC2_REG)
#define CPC2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC2_OFFSET))
#endif

#if defined(_V1) && !defined(CPC2_VAL)
#define CPC2_VAL  PREFIX_VAL(CPC2_REG)
#endif

#define DMA0_CPC2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC2_OFFSET))
#define DMA0_CPC2_VAL  PREFIX_VAL(DMA0_CPC2_REG)

/* FIELDS: */

/* pc_chnl ACCESS: RO */

#ifndef DMA_CPC2_PC_CHNL_POS
#define DMA_CPC2_PC_CHNL_POS      0
#endif

#ifndef DMA_CPC2_PC_CHNL_LEN
#define DMA_CPC2_PC_CHNL_LEN      32
#endif

#if defined(_V1) && !defined(CPC2_PC_CHNL_R)
#define CPC2_PC_CHNL_R        GetGroupBits32( (DMA0_CPC2_VAL),DMA_CPC2_PC_CHNL_POS, DMA_CPC2_PC_CHNL_LEN)
#endif

#if defined(_V1) && !defined(CPC2_PC_CHNL_W)
#define CPC2_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC2_VAL),DMA_CPC2_PC_CHNL_POS, DMA_CPC2_PC_CHNL_LEN,value)
#endif

#define DMA0_CPC2_PC_CHNL_R        GetGroupBits32( (DMA0_CPC2_VAL),DMA_CPC2_PC_CHNL_POS, DMA_CPC2_PC_CHNL_LEN)

#define DMA0_CPC2_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC2_VAL),DMA_CPC2_PC_CHNL_POS, DMA_CPC2_PC_CHNL_LEN,value)


/* REGISTER: CSR3 ACCESS: RO */

#if defined(_V1) && !defined(CSR3_OFFSET)
#define CSR3_OFFSET 0x118
#endif

#if !defined(DMA_CSR3_OFFSET)
#define DMA_CSR3_OFFSET 0x118
#endif

#if defined(_V1) && !defined(CSR3_REG)
#define CSR3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR3_OFFSET))
#endif

#if defined(_V1) && !defined(CSR3_VAL)
#define CSR3_VAL  PREFIX_VAL(CSR3_REG)
#endif

#define DMA0_CSR3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR3_OFFSET))
#define DMA0_CSR3_VAL  PREFIX_VAL(DMA0_CSR3_REG)

/* FIELDS: */

/* Channel_status ACCESS: RO */

#ifndef DMA_CSR3_CHANNEL_STATUS_POS
#define DMA_CSR3_CHANNEL_STATUS_POS      0
#endif

#ifndef DMA_CSR3_CHANNEL_STATUS_LEN
#define DMA_CSR3_CHANNEL_STATUS_LEN      4
#endif

#if defined(_V1) && !defined(CSR3_CHANNEL_STATUS_R)
#define CSR3_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_CHANNEL_STATUS_POS, DMA_CSR3_CHANNEL_STATUS_LEN)
#endif

#if defined(_V1) && !defined(CSR3_CHANNEL_STATUS_W)
#define CSR3_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_CHANNEL_STATUS_POS, DMA_CSR3_CHANNEL_STATUS_LEN,value)
#endif

#define DMA0_CSR3_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_CHANNEL_STATUS_POS, DMA_CSR3_CHANNEL_STATUS_LEN)

#define DMA0_CSR3_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_CHANNEL_STATUS_POS, DMA_CSR3_CHANNEL_STATUS_LEN,value)


/* Wakeup_number ACCESS: RO */

#ifndef DMA_CSR3_WAKEUP_NUMBER_POS
#define DMA_CSR3_WAKEUP_NUMBER_POS      4
#endif

#ifndef DMA_CSR3_WAKEUP_NUMBER_LEN
#define DMA_CSR3_WAKEUP_NUMBER_LEN      5
#endif

#if defined(_V1) && !defined(CSR3_WAKEUP_NUMBER_R)
#define CSR3_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_WAKEUP_NUMBER_POS, DMA_CSR3_WAKEUP_NUMBER_LEN)
#endif

#if defined(_V1) && !defined(CSR3_WAKEUP_NUMBER_W)
#define CSR3_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_WAKEUP_NUMBER_POS, DMA_CSR3_WAKEUP_NUMBER_LEN,value)
#endif

#define DMA0_CSR3_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_WAKEUP_NUMBER_POS, DMA_CSR3_WAKEUP_NUMBER_LEN)

#define DMA0_CSR3_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_WAKEUP_NUMBER_POS, DMA_CSR3_WAKEUP_NUMBER_LEN,value)


/* dmawfp_b_ns ACCESS: RO */

#ifndef DMA_CSR3_DMAWFP_B_NS_POS
#define DMA_CSR3_DMAWFP_B_NS_POS      14
#endif

#ifndef DMA_CSR3_DMAWFP_B_NS_LEN
#define DMA_CSR3_DMAWFP_B_NS_LEN      1
#endif

#if defined(_V1) && !defined(CSR3_DMAWFP_B_NS_R)
#define CSR3_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_DMAWFP_B_NS_POS, DMA_CSR3_DMAWFP_B_NS_LEN)
#endif

#if defined(_V1) && !defined(CSR3_DMAWFP_B_NS_W)
#define CSR3_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_DMAWFP_B_NS_POS, DMA_CSR3_DMAWFP_B_NS_LEN,value)
#endif

#define DMA0_CSR3_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_DMAWFP_B_NS_POS, DMA_CSR3_DMAWFP_B_NS_LEN)

#define DMA0_CSR3_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_DMAWFP_B_NS_POS, DMA_CSR3_DMAWFP_B_NS_LEN,value)


/* dmawfp_periph ACCESS: RO */

#ifndef DMA_CSR3_DMAWFP_PERIPH_POS
#define DMA_CSR3_DMAWFP_PERIPH_POS      15
#endif

#ifndef DMA_CSR3_DMAWFP_PERIPH_LEN
#define DMA_CSR3_DMAWFP_PERIPH_LEN      1
#endif

#if defined(_V1) && !defined(CSR3_DMAWFP_PERIPH_R)
#define CSR3_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_DMAWFP_PERIPH_POS, DMA_CSR3_DMAWFP_PERIPH_LEN)
#endif

#if defined(_V1) && !defined(CSR3_DMAWFP_PERIPH_W)
#define CSR3_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_DMAWFP_PERIPH_POS, DMA_CSR3_DMAWFP_PERIPH_LEN,value)
#endif

#define DMA0_CSR3_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_DMAWFP_PERIPH_POS, DMA_CSR3_DMAWFP_PERIPH_LEN)

#define DMA0_CSR3_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_DMAWFP_PERIPH_POS, DMA_CSR3_DMAWFP_PERIPH_LEN,value)


/* CNS ACCESS: RO */

#ifndef DMA_CSR3_CNS_POS
#define DMA_CSR3_CNS_POS      21
#endif

#ifndef DMA_CSR3_CNS_LEN
#define DMA_CSR3_CNS_LEN      1
#endif

#if defined(_V1) && !defined(CSR3_CNS_R)
#define CSR3_CNS_R        GetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_CNS_POS, DMA_CSR3_CNS_LEN)
#endif

#if defined(_V1) && !defined(CSR3_CNS_W)
#define CSR3_CNS_W(value) SetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_CNS_POS, DMA_CSR3_CNS_LEN,value)
#endif

#define DMA0_CSR3_CNS_R        GetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_CNS_POS, DMA_CSR3_CNS_LEN)

#define DMA0_CSR3_CNS_W(value) SetGroupBits32( (DMA0_CSR3_VAL),DMA_CSR3_CNS_POS, DMA_CSR3_CNS_LEN,value)


/* REGISTER: CPC3 ACCESS: RO */

#if defined(_V1) && !defined(CPC3_OFFSET)
#define CPC3_OFFSET 0x11C
#endif

#if !defined(DMA_CPC3_OFFSET)
#define DMA_CPC3_OFFSET 0x11C
#endif

#if defined(_V1) && !defined(CPC3_REG)
#define CPC3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC3_OFFSET))
#endif

#if defined(_V1) && !defined(CPC3_VAL)
#define CPC3_VAL  PREFIX_VAL(CPC3_REG)
#endif

#define DMA0_CPC3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC3_OFFSET))
#define DMA0_CPC3_VAL  PREFIX_VAL(DMA0_CPC3_REG)

/* FIELDS: */

/* pc_chnl ACCESS: RO */

#ifndef DMA_CPC3_PC_CHNL_POS
#define DMA_CPC3_PC_CHNL_POS      0
#endif

#ifndef DMA_CPC3_PC_CHNL_LEN
#define DMA_CPC3_PC_CHNL_LEN      32
#endif

#if defined(_V1) && !defined(CPC3_PC_CHNL_R)
#define CPC3_PC_CHNL_R        GetGroupBits32( (DMA0_CPC3_VAL),DMA_CPC3_PC_CHNL_POS, DMA_CPC3_PC_CHNL_LEN)
#endif

#if defined(_V1) && !defined(CPC3_PC_CHNL_W)
#define CPC3_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC3_VAL),DMA_CPC3_PC_CHNL_POS, DMA_CPC3_PC_CHNL_LEN,value)
#endif

#define DMA0_CPC3_PC_CHNL_R        GetGroupBits32( (DMA0_CPC3_VAL),DMA_CPC3_PC_CHNL_POS, DMA_CPC3_PC_CHNL_LEN)

#define DMA0_CPC3_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC3_VAL),DMA_CPC3_PC_CHNL_POS, DMA_CPC3_PC_CHNL_LEN,value)


/* REGISTER: CSR4 ACCESS: RO */

#if defined(_V1) && !defined(CSR4_OFFSET)
#define CSR4_OFFSET 0x120
#endif

#if !defined(DMA_CSR4_OFFSET)
#define DMA_CSR4_OFFSET 0x120
#endif

#if defined(_V1) && !defined(CSR4_REG)
#define CSR4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR4_OFFSET))
#endif

#if defined(_V1) && !defined(CSR4_VAL)
#define CSR4_VAL  PREFIX_VAL(CSR4_REG)
#endif

#define DMA0_CSR4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR4_OFFSET))
#define DMA0_CSR4_VAL  PREFIX_VAL(DMA0_CSR4_REG)

/* FIELDS: */

/* Channel_status ACCESS: RO */

#ifndef DMA_CSR4_CHANNEL_STATUS_POS
#define DMA_CSR4_CHANNEL_STATUS_POS      0
#endif

#ifndef DMA_CSR4_CHANNEL_STATUS_LEN
#define DMA_CSR4_CHANNEL_STATUS_LEN      4
#endif

#if defined(_V1) && !defined(CSR4_CHANNEL_STATUS_R)
#define CSR4_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_CHANNEL_STATUS_POS, DMA_CSR4_CHANNEL_STATUS_LEN)
#endif

#if defined(_V1) && !defined(CSR4_CHANNEL_STATUS_W)
#define CSR4_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_CHANNEL_STATUS_POS, DMA_CSR4_CHANNEL_STATUS_LEN,value)
#endif

#define DMA0_CSR4_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_CHANNEL_STATUS_POS, DMA_CSR4_CHANNEL_STATUS_LEN)

#define DMA0_CSR4_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_CHANNEL_STATUS_POS, DMA_CSR4_CHANNEL_STATUS_LEN,value)


/* Wakeup_number ACCESS: RO */

#ifndef DMA_CSR4_WAKEUP_NUMBER_POS
#define DMA_CSR4_WAKEUP_NUMBER_POS      4
#endif

#ifndef DMA_CSR4_WAKEUP_NUMBER_LEN
#define DMA_CSR4_WAKEUP_NUMBER_LEN      5
#endif

#if defined(_V1) && !defined(CSR4_WAKEUP_NUMBER_R)
#define CSR4_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_WAKEUP_NUMBER_POS, DMA_CSR4_WAKEUP_NUMBER_LEN)
#endif

#if defined(_V1) && !defined(CSR4_WAKEUP_NUMBER_W)
#define CSR4_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_WAKEUP_NUMBER_POS, DMA_CSR4_WAKEUP_NUMBER_LEN,value)
#endif

#define DMA0_CSR4_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_WAKEUP_NUMBER_POS, DMA_CSR4_WAKEUP_NUMBER_LEN)

#define DMA0_CSR4_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_WAKEUP_NUMBER_POS, DMA_CSR4_WAKEUP_NUMBER_LEN,value)


/* dmawfp_b_ns ACCESS: RO */

#ifndef DMA_CSR4_DMAWFP_B_NS_POS
#define DMA_CSR4_DMAWFP_B_NS_POS      14
#endif

#ifndef DMA_CSR4_DMAWFP_B_NS_LEN
#define DMA_CSR4_DMAWFP_B_NS_LEN      1
#endif

#if defined(_V1) && !defined(CSR4_DMAWFP_B_NS_R)
#define CSR4_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_DMAWFP_B_NS_POS, DMA_CSR4_DMAWFP_B_NS_LEN)
#endif

#if defined(_V1) && !defined(CSR4_DMAWFP_B_NS_W)
#define CSR4_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_DMAWFP_B_NS_POS, DMA_CSR4_DMAWFP_B_NS_LEN,value)
#endif

#define DMA0_CSR4_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_DMAWFP_B_NS_POS, DMA_CSR4_DMAWFP_B_NS_LEN)

#define DMA0_CSR4_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_DMAWFP_B_NS_POS, DMA_CSR4_DMAWFP_B_NS_LEN,value)


/* dmawfp_periph ACCESS: RO */

#ifndef DMA_CSR4_DMAWFP_PERIPH_POS
#define DMA_CSR4_DMAWFP_PERIPH_POS      15
#endif

#ifndef DMA_CSR4_DMAWFP_PERIPH_LEN
#define DMA_CSR4_DMAWFP_PERIPH_LEN      1
#endif

#if defined(_V1) && !defined(CSR4_DMAWFP_PERIPH_R)
#define CSR4_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_DMAWFP_PERIPH_POS, DMA_CSR4_DMAWFP_PERIPH_LEN)
#endif

#if defined(_V1) && !defined(CSR4_DMAWFP_PERIPH_W)
#define CSR4_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_DMAWFP_PERIPH_POS, DMA_CSR4_DMAWFP_PERIPH_LEN,value)
#endif

#define DMA0_CSR4_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_DMAWFP_PERIPH_POS, DMA_CSR4_DMAWFP_PERIPH_LEN)

#define DMA0_CSR4_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_DMAWFP_PERIPH_POS, DMA_CSR4_DMAWFP_PERIPH_LEN,value)


/* CNS ACCESS: RO */

#ifndef DMA_CSR4_CNS_POS
#define DMA_CSR4_CNS_POS      21
#endif

#ifndef DMA_CSR4_CNS_LEN
#define DMA_CSR4_CNS_LEN      1
#endif

#if defined(_V1) && !defined(CSR4_CNS_R)
#define CSR4_CNS_R        GetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_CNS_POS, DMA_CSR4_CNS_LEN)
#endif

#if defined(_V1) && !defined(CSR4_CNS_W)
#define CSR4_CNS_W(value) SetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_CNS_POS, DMA_CSR4_CNS_LEN,value)
#endif

#define DMA0_CSR4_CNS_R        GetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_CNS_POS, DMA_CSR4_CNS_LEN)

#define DMA0_CSR4_CNS_W(value) SetGroupBits32( (DMA0_CSR4_VAL),DMA_CSR4_CNS_POS, DMA_CSR4_CNS_LEN,value)


/* REGISTER: CPC4 ACCESS: RO */

#if defined(_V1) && !defined(CPC4_OFFSET)
#define CPC4_OFFSET 0x124
#endif

#if !defined(DMA_CPC4_OFFSET)
#define DMA_CPC4_OFFSET 0x124
#endif

#if defined(_V1) && !defined(CPC4_REG)
#define CPC4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC4_OFFSET))
#endif

#if defined(_V1) && !defined(CPC4_VAL)
#define CPC4_VAL  PREFIX_VAL(CPC4_REG)
#endif

#define DMA0_CPC4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC4_OFFSET))
#define DMA0_CPC4_VAL  PREFIX_VAL(DMA0_CPC4_REG)

/* FIELDS: */

/* pc_chnl ACCESS: RO */

#ifndef DMA_CPC4_PC_CHNL_POS
#define DMA_CPC4_PC_CHNL_POS      0
#endif

#ifndef DMA_CPC4_PC_CHNL_LEN
#define DMA_CPC4_PC_CHNL_LEN      32
#endif

#if defined(_V1) && !defined(CPC4_PC_CHNL_R)
#define CPC4_PC_CHNL_R        GetGroupBits32( (DMA0_CPC4_VAL),DMA_CPC4_PC_CHNL_POS, DMA_CPC4_PC_CHNL_LEN)
#endif

#if defined(_V1) && !defined(CPC4_PC_CHNL_W)
#define CPC4_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC4_VAL),DMA_CPC4_PC_CHNL_POS, DMA_CPC4_PC_CHNL_LEN,value)
#endif

#define DMA0_CPC4_PC_CHNL_R        GetGroupBits32( (DMA0_CPC4_VAL),DMA_CPC4_PC_CHNL_POS, DMA_CPC4_PC_CHNL_LEN)

#define DMA0_CPC4_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC4_VAL),DMA_CPC4_PC_CHNL_POS, DMA_CPC4_PC_CHNL_LEN,value)


/* REGISTER: CSR5 ACCESS: RO */

#if defined(_V1) && !defined(CSR5_OFFSET)
#define CSR5_OFFSET 0x128
#endif

#if !defined(DMA_CSR5_OFFSET)
#define DMA_CSR5_OFFSET 0x128
#endif

#if defined(_V1) && !defined(CSR5_REG)
#define CSR5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR5_OFFSET))
#endif

#if defined(_V1) && !defined(CSR5_VAL)
#define CSR5_VAL  PREFIX_VAL(CSR5_REG)
#endif

#define DMA0_CSR5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR5_OFFSET))
#define DMA0_CSR5_VAL  PREFIX_VAL(DMA0_CSR5_REG)

/* FIELDS: */

/* Channel_status ACCESS: RO */

#ifndef DMA_CSR5_CHANNEL_STATUS_POS
#define DMA_CSR5_CHANNEL_STATUS_POS      0
#endif

#ifndef DMA_CSR5_CHANNEL_STATUS_LEN
#define DMA_CSR5_CHANNEL_STATUS_LEN      4
#endif

#if defined(_V1) && !defined(CSR5_CHANNEL_STATUS_R)
#define CSR5_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_CHANNEL_STATUS_POS, DMA_CSR5_CHANNEL_STATUS_LEN)
#endif

#if defined(_V1) && !defined(CSR5_CHANNEL_STATUS_W)
#define CSR5_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_CHANNEL_STATUS_POS, DMA_CSR5_CHANNEL_STATUS_LEN,value)
#endif

#define DMA0_CSR5_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_CHANNEL_STATUS_POS, DMA_CSR5_CHANNEL_STATUS_LEN)

#define DMA0_CSR5_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_CHANNEL_STATUS_POS, DMA_CSR5_CHANNEL_STATUS_LEN,value)


/* Wakeup_number ACCESS: RO */

#ifndef DMA_CSR5_WAKEUP_NUMBER_POS
#define DMA_CSR5_WAKEUP_NUMBER_POS      4
#endif

#ifndef DMA_CSR5_WAKEUP_NUMBER_LEN
#define DMA_CSR5_WAKEUP_NUMBER_LEN      5
#endif

#if defined(_V1) && !defined(CSR5_WAKEUP_NUMBER_R)
#define CSR5_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_WAKEUP_NUMBER_POS, DMA_CSR5_WAKEUP_NUMBER_LEN)
#endif

#if defined(_V1) && !defined(CSR5_WAKEUP_NUMBER_W)
#define CSR5_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_WAKEUP_NUMBER_POS, DMA_CSR5_WAKEUP_NUMBER_LEN,value)
#endif

#define DMA0_CSR5_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_WAKEUP_NUMBER_POS, DMA_CSR5_WAKEUP_NUMBER_LEN)

#define DMA0_CSR5_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_WAKEUP_NUMBER_POS, DMA_CSR5_WAKEUP_NUMBER_LEN,value)


/* dmawfp_b_ns ACCESS: RO */

#ifndef DMA_CSR5_DMAWFP_B_NS_POS
#define DMA_CSR5_DMAWFP_B_NS_POS      14
#endif

#ifndef DMA_CSR5_DMAWFP_B_NS_LEN
#define DMA_CSR5_DMAWFP_B_NS_LEN      1
#endif

#if defined(_V1) && !defined(CSR5_DMAWFP_B_NS_R)
#define CSR5_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_DMAWFP_B_NS_POS, DMA_CSR5_DMAWFP_B_NS_LEN)
#endif

#if defined(_V1) && !defined(CSR5_DMAWFP_B_NS_W)
#define CSR5_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_DMAWFP_B_NS_POS, DMA_CSR5_DMAWFP_B_NS_LEN,value)
#endif

#define DMA0_CSR5_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_DMAWFP_B_NS_POS, DMA_CSR5_DMAWFP_B_NS_LEN)

#define DMA0_CSR5_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_DMAWFP_B_NS_POS, DMA_CSR5_DMAWFP_B_NS_LEN,value)


/* dmawfp_periph ACCESS: RO */

#ifndef DMA_CSR5_DMAWFP_PERIPH_POS
#define DMA_CSR5_DMAWFP_PERIPH_POS      15
#endif

#ifndef DMA_CSR5_DMAWFP_PERIPH_LEN
#define DMA_CSR5_DMAWFP_PERIPH_LEN      1
#endif

#if defined(_V1) && !defined(CSR5_DMAWFP_PERIPH_R)
#define CSR5_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_DMAWFP_PERIPH_POS, DMA_CSR5_DMAWFP_PERIPH_LEN)
#endif

#if defined(_V1) && !defined(CSR5_DMAWFP_PERIPH_W)
#define CSR5_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_DMAWFP_PERIPH_POS, DMA_CSR5_DMAWFP_PERIPH_LEN,value)
#endif

#define DMA0_CSR5_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_DMAWFP_PERIPH_POS, DMA_CSR5_DMAWFP_PERIPH_LEN)

#define DMA0_CSR5_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_DMAWFP_PERIPH_POS, DMA_CSR5_DMAWFP_PERIPH_LEN,value)


/* CNS ACCESS: RO */

#ifndef DMA_CSR5_CNS_POS
#define DMA_CSR5_CNS_POS      21
#endif

#ifndef DMA_CSR5_CNS_LEN
#define DMA_CSR5_CNS_LEN      1
#endif

#if defined(_V1) && !defined(CSR5_CNS_R)
#define CSR5_CNS_R        GetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_CNS_POS, DMA_CSR5_CNS_LEN)
#endif

#if defined(_V1) && !defined(CSR5_CNS_W)
#define CSR5_CNS_W(value) SetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_CNS_POS, DMA_CSR5_CNS_LEN,value)
#endif

#define DMA0_CSR5_CNS_R        GetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_CNS_POS, DMA_CSR5_CNS_LEN)

#define DMA0_CSR5_CNS_W(value) SetGroupBits32( (DMA0_CSR5_VAL),DMA_CSR5_CNS_POS, DMA_CSR5_CNS_LEN,value)


/* REGISTER: CPC5 ACCESS: RO */

#if defined(_V1) && !defined(CPC5_OFFSET)
#define CPC5_OFFSET 0x12C
#endif

#if !defined(DMA_CPC5_OFFSET)
#define DMA_CPC5_OFFSET 0x12C
#endif

#if defined(_V1) && !defined(CPC5_REG)
#define CPC5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC5_OFFSET))
#endif

#if defined(_V1) && !defined(CPC5_VAL)
#define CPC5_VAL  PREFIX_VAL(CPC5_REG)
#endif

#define DMA0_CPC5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC5_OFFSET))
#define DMA0_CPC5_VAL  PREFIX_VAL(DMA0_CPC5_REG)

/* FIELDS: */

/* pc_chnl ACCESS: RO */

#ifndef DMA_CPC5_PC_CHNL_POS
#define DMA_CPC5_PC_CHNL_POS      0
#endif

#ifndef DMA_CPC5_PC_CHNL_LEN
#define DMA_CPC5_PC_CHNL_LEN      32
#endif

#if defined(_V1) && !defined(CPC5_PC_CHNL_R)
#define CPC5_PC_CHNL_R        GetGroupBits32( (DMA0_CPC5_VAL),DMA_CPC5_PC_CHNL_POS, DMA_CPC5_PC_CHNL_LEN)
#endif

#if defined(_V1) && !defined(CPC5_PC_CHNL_W)
#define CPC5_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC5_VAL),DMA_CPC5_PC_CHNL_POS, DMA_CPC5_PC_CHNL_LEN,value)
#endif

#define DMA0_CPC5_PC_CHNL_R        GetGroupBits32( (DMA0_CPC5_VAL),DMA_CPC5_PC_CHNL_POS, DMA_CPC5_PC_CHNL_LEN)

#define DMA0_CPC5_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC5_VAL),DMA_CPC5_PC_CHNL_POS, DMA_CPC5_PC_CHNL_LEN,value)


/* REGISTER: CSR6 ACCESS: RO */

#if defined(_V1) && !defined(CSR6_OFFSET)
#define CSR6_OFFSET 0x130
#endif

#if !defined(DMA_CSR6_OFFSET)
#define DMA_CSR6_OFFSET 0x130
#endif

#if defined(_V1) && !defined(CSR6_REG)
#define CSR6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR6_OFFSET))
#endif

#if defined(_V1) && !defined(CSR6_VAL)
#define CSR6_VAL  PREFIX_VAL(CSR6_REG)
#endif

#define DMA0_CSR6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR6_OFFSET))
#define DMA0_CSR6_VAL  PREFIX_VAL(DMA0_CSR6_REG)

/* FIELDS: */

/* Channel_status ACCESS: RO */

#ifndef DMA_CSR6_CHANNEL_STATUS_POS
#define DMA_CSR6_CHANNEL_STATUS_POS      0
#endif

#ifndef DMA_CSR6_CHANNEL_STATUS_LEN
#define DMA_CSR6_CHANNEL_STATUS_LEN      4
#endif

#if defined(_V1) && !defined(CSR6_CHANNEL_STATUS_R)
#define CSR6_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_CHANNEL_STATUS_POS, DMA_CSR6_CHANNEL_STATUS_LEN)
#endif

#if defined(_V1) && !defined(CSR6_CHANNEL_STATUS_W)
#define CSR6_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_CHANNEL_STATUS_POS, DMA_CSR6_CHANNEL_STATUS_LEN,value)
#endif

#define DMA0_CSR6_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_CHANNEL_STATUS_POS, DMA_CSR6_CHANNEL_STATUS_LEN)

#define DMA0_CSR6_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_CHANNEL_STATUS_POS, DMA_CSR6_CHANNEL_STATUS_LEN,value)


/* Wakeup_number ACCESS: RO */

#ifndef DMA_CSR6_WAKEUP_NUMBER_POS
#define DMA_CSR6_WAKEUP_NUMBER_POS      4
#endif

#ifndef DMA_CSR6_WAKEUP_NUMBER_LEN
#define DMA_CSR6_WAKEUP_NUMBER_LEN      5
#endif

#if defined(_V1) && !defined(CSR6_WAKEUP_NUMBER_R)
#define CSR6_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_WAKEUP_NUMBER_POS, DMA_CSR6_WAKEUP_NUMBER_LEN)
#endif

#if defined(_V1) && !defined(CSR6_WAKEUP_NUMBER_W)
#define CSR6_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_WAKEUP_NUMBER_POS, DMA_CSR6_WAKEUP_NUMBER_LEN,value)
#endif

#define DMA0_CSR6_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_WAKEUP_NUMBER_POS, DMA_CSR6_WAKEUP_NUMBER_LEN)

#define DMA0_CSR6_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_WAKEUP_NUMBER_POS, DMA_CSR6_WAKEUP_NUMBER_LEN,value)


/* dmawfp_b_ns ACCESS: RO */

#ifndef DMA_CSR6_DMAWFP_B_NS_POS
#define DMA_CSR6_DMAWFP_B_NS_POS      14
#endif

#ifndef DMA_CSR6_DMAWFP_B_NS_LEN
#define DMA_CSR6_DMAWFP_B_NS_LEN      1
#endif

#if defined(_V1) && !defined(CSR6_DMAWFP_B_NS_R)
#define CSR6_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_DMAWFP_B_NS_POS, DMA_CSR6_DMAWFP_B_NS_LEN)
#endif

#if defined(_V1) && !defined(CSR6_DMAWFP_B_NS_W)
#define CSR6_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_DMAWFP_B_NS_POS, DMA_CSR6_DMAWFP_B_NS_LEN,value)
#endif

#define DMA0_CSR6_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_DMAWFP_B_NS_POS, DMA_CSR6_DMAWFP_B_NS_LEN)

#define DMA0_CSR6_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_DMAWFP_B_NS_POS, DMA_CSR6_DMAWFP_B_NS_LEN,value)


/* dmawfp_periph ACCESS: RO */

#ifndef DMA_CSR6_DMAWFP_PERIPH_POS
#define DMA_CSR6_DMAWFP_PERIPH_POS      15
#endif

#ifndef DMA_CSR6_DMAWFP_PERIPH_LEN
#define DMA_CSR6_DMAWFP_PERIPH_LEN      1
#endif

#if defined(_V1) && !defined(CSR6_DMAWFP_PERIPH_R)
#define CSR6_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_DMAWFP_PERIPH_POS, DMA_CSR6_DMAWFP_PERIPH_LEN)
#endif

#if defined(_V1) && !defined(CSR6_DMAWFP_PERIPH_W)
#define CSR6_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_DMAWFP_PERIPH_POS, DMA_CSR6_DMAWFP_PERIPH_LEN,value)
#endif

#define DMA0_CSR6_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_DMAWFP_PERIPH_POS, DMA_CSR6_DMAWFP_PERIPH_LEN)

#define DMA0_CSR6_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_DMAWFP_PERIPH_POS, DMA_CSR6_DMAWFP_PERIPH_LEN,value)


/* CNS ACCESS: RO */

#ifndef DMA_CSR6_CNS_POS
#define DMA_CSR6_CNS_POS      21
#endif

#ifndef DMA_CSR6_CNS_LEN
#define DMA_CSR6_CNS_LEN      1
#endif

#if defined(_V1) && !defined(CSR6_CNS_R)
#define CSR6_CNS_R        GetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_CNS_POS, DMA_CSR6_CNS_LEN)
#endif

#if defined(_V1) && !defined(CSR6_CNS_W)
#define CSR6_CNS_W(value) SetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_CNS_POS, DMA_CSR6_CNS_LEN,value)
#endif

#define DMA0_CSR6_CNS_R        GetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_CNS_POS, DMA_CSR6_CNS_LEN)

#define DMA0_CSR6_CNS_W(value) SetGroupBits32( (DMA0_CSR6_VAL),DMA_CSR6_CNS_POS, DMA_CSR6_CNS_LEN,value)


/* REGISTER: CPC6 ACCESS: RO */

#if defined(_V1) && !defined(CPC6_OFFSET)
#define CPC6_OFFSET 0x134
#endif

#if !defined(DMA_CPC6_OFFSET)
#define DMA_CPC6_OFFSET 0x134
#endif

#if defined(_V1) && !defined(CPC6_REG)
#define CPC6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC6_OFFSET))
#endif

#if defined(_V1) && !defined(CPC6_VAL)
#define CPC6_VAL  PREFIX_VAL(CPC6_REG)
#endif

#define DMA0_CPC6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC6_OFFSET))
#define DMA0_CPC6_VAL  PREFIX_VAL(DMA0_CPC6_REG)

/* FIELDS: */

/* pc_chnl ACCESS: RO */

#ifndef DMA_CPC6_PC_CHNL_POS
#define DMA_CPC6_PC_CHNL_POS      0
#endif

#ifndef DMA_CPC6_PC_CHNL_LEN
#define DMA_CPC6_PC_CHNL_LEN      32
#endif

#if defined(_V1) && !defined(CPC6_PC_CHNL_R)
#define CPC6_PC_CHNL_R        GetGroupBits32( (DMA0_CPC6_VAL),DMA_CPC6_PC_CHNL_POS, DMA_CPC6_PC_CHNL_LEN)
#endif

#if defined(_V1) && !defined(CPC6_PC_CHNL_W)
#define CPC6_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC6_VAL),DMA_CPC6_PC_CHNL_POS, DMA_CPC6_PC_CHNL_LEN,value)
#endif

#define DMA0_CPC6_PC_CHNL_R        GetGroupBits32( (DMA0_CPC6_VAL),DMA_CPC6_PC_CHNL_POS, DMA_CPC6_PC_CHNL_LEN)

#define DMA0_CPC6_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC6_VAL),DMA_CPC6_PC_CHNL_POS, DMA_CPC6_PC_CHNL_LEN,value)


/* REGISTER: CSR7 ACCESS: RO */

#if defined(_V1) && !defined(CSR7_OFFSET)
#define CSR7_OFFSET 0x138
#endif

#if !defined(DMA_CSR7_OFFSET)
#define DMA_CSR7_OFFSET 0x138
#endif

#if defined(_V1) && !defined(CSR7_REG)
#define CSR7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR7_OFFSET))
#endif

#if defined(_V1) && !defined(CSR7_VAL)
#define CSR7_VAL  PREFIX_VAL(CSR7_REG)
#endif

#define DMA0_CSR7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CSR7_OFFSET))
#define DMA0_CSR7_VAL  PREFIX_VAL(DMA0_CSR7_REG)

/* FIELDS: */

/* Channel_status ACCESS: RO */

#ifndef DMA_CSR7_CHANNEL_STATUS_POS
#define DMA_CSR7_CHANNEL_STATUS_POS      0
#endif

#ifndef DMA_CSR7_CHANNEL_STATUS_LEN
#define DMA_CSR7_CHANNEL_STATUS_LEN      4
#endif

#if defined(_V1) && !defined(CSR7_CHANNEL_STATUS_R)
#define CSR7_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_CHANNEL_STATUS_POS, DMA_CSR7_CHANNEL_STATUS_LEN)
#endif

#if defined(_V1) && !defined(CSR7_CHANNEL_STATUS_W)
#define CSR7_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_CHANNEL_STATUS_POS, DMA_CSR7_CHANNEL_STATUS_LEN,value)
#endif

#define DMA0_CSR7_CHANNEL_STATUS_R        GetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_CHANNEL_STATUS_POS, DMA_CSR7_CHANNEL_STATUS_LEN)

#define DMA0_CSR7_CHANNEL_STATUS_W(value) SetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_CHANNEL_STATUS_POS, DMA_CSR7_CHANNEL_STATUS_LEN,value)


/* Wakeup_number ACCESS: RO */

#ifndef DMA_CSR7_WAKEUP_NUMBER_POS
#define DMA_CSR7_WAKEUP_NUMBER_POS      4
#endif

#ifndef DMA_CSR7_WAKEUP_NUMBER_LEN
#define DMA_CSR7_WAKEUP_NUMBER_LEN      5
#endif

#if defined(_V1) && !defined(CSR7_WAKEUP_NUMBER_R)
#define CSR7_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_WAKEUP_NUMBER_POS, DMA_CSR7_WAKEUP_NUMBER_LEN)
#endif

#if defined(_V1) && !defined(CSR7_WAKEUP_NUMBER_W)
#define CSR7_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_WAKEUP_NUMBER_POS, DMA_CSR7_WAKEUP_NUMBER_LEN,value)
#endif

#define DMA0_CSR7_WAKEUP_NUMBER_R        GetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_WAKEUP_NUMBER_POS, DMA_CSR7_WAKEUP_NUMBER_LEN)

#define DMA0_CSR7_WAKEUP_NUMBER_W(value) SetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_WAKEUP_NUMBER_POS, DMA_CSR7_WAKEUP_NUMBER_LEN,value)


/* dmawfp_b_ns ACCESS: RO */

#ifndef DMA_CSR7_DMAWFP_B_NS_POS
#define DMA_CSR7_DMAWFP_B_NS_POS      14
#endif

#ifndef DMA_CSR7_DMAWFP_B_NS_LEN
#define DMA_CSR7_DMAWFP_B_NS_LEN      1
#endif

#if defined(_V1) && !defined(CSR7_DMAWFP_B_NS_R)
#define CSR7_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_DMAWFP_B_NS_POS, DMA_CSR7_DMAWFP_B_NS_LEN)
#endif

#if defined(_V1) && !defined(CSR7_DMAWFP_B_NS_W)
#define CSR7_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_DMAWFP_B_NS_POS, DMA_CSR7_DMAWFP_B_NS_LEN,value)
#endif

#define DMA0_CSR7_DMAWFP_B_NS_R        GetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_DMAWFP_B_NS_POS, DMA_CSR7_DMAWFP_B_NS_LEN)

#define DMA0_CSR7_DMAWFP_B_NS_W(value) SetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_DMAWFP_B_NS_POS, DMA_CSR7_DMAWFP_B_NS_LEN,value)


/* dmawfp_periph ACCESS: RO */

#ifndef DMA_CSR7_DMAWFP_PERIPH_POS
#define DMA_CSR7_DMAWFP_PERIPH_POS      15
#endif

#ifndef DMA_CSR7_DMAWFP_PERIPH_LEN
#define DMA_CSR7_DMAWFP_PERIPH_LEN      1
#endif

#if defined(_V1) && !defined(CSR7_DMAWFP_PERIPH_R)
#define CSR7_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_DMAWFP_PERIPH_POS, DMA_CSR7_DMAWFP_PERIPH_LEN)
#endif

#if defined(_V1) && !defined(CSR7_DMAWFP_PERIPH_W)
#define CSR7_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_DMAWFP_PERIPH_POS, DMA_CSR7_DMAWFP_PERIPH_LEN,value)
#endif

#define DMA0_CSR7_DMAWFP_PERIPH_R        GetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_DMAWFP_PERIPH_POS, DMA_CSR7_DMAWFP_PERIPH_LEN)

#define DMA0_CSR7_DMAWFP_PERIPH_W(value) SetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_DMAWFP_PERIPH_POS, DMA_CSR7_DMAWFP_PERIPH_LEN,value)


/* CNS ACCESS: RO */

#ifndef DMA_CSR7_CNS_POS
#define DMA_CSR7_CNS_POS      21
#endif

#ifndef DMA_CSR7_CNS_LEN
#define DMA_CSR7_CNS_LEN      1
#endif

#if defined(_V1) && !defined(CSR7_CNS_R)
#define CSR7_CNS_R        GetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_CNS_POS, DMA_CSR7_CNS_LEN)
#endif

#if defined(_V1) && !defined(CSR7_CNS_W)
#define CSR7_CNS_W(value) SetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_CNS_POS, DMA_CSR7_CNS_LEN,value)
#endif

#define DMA0_CSR7_CNS_R        GetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_CNS_POS, DMA_CSR7_CNS_LEN)

#define DMA0_CSR7_CNS_W(value) SetGroupBits32( (DMA0_CSR7_VAL),DMA_CSR7_CNS_POS, DMA_CSR7_CNS_LEN,value)


/* REGISTER: CPC7 ACCESS: RO */

#if defined(_V1) && !defined(CPC7_OFFSET)
#define CPC7_OFFSET 0x13C
#endif

#if !defined(DMA_CPC7_OFFSET)
#define DMA_CPC7_OFFSET 0x13C
#endif

#if defined(_V1) && !defined(CPC7_REG)
#define CPC7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC7_OFFSET))
#endif

#if defined(_V1) && !defined(CPC7_VAL)
#define CPC7_VAL  PREFIX_VAL(CPC7_REG)
#endif

#define DMA0_CPC7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CPC7_OFFSET))
#define DMA0_CPC7_VAL  PREFIX_VAL(DMA0_CPC7_REG)

/* FIELDS: */

/* pc_chnl ACCESS: RO */

#ifndef DMA_CPC7_PC_CHNL_POS
#define DMA_CPC7_PC_CHNL_POS      0
#endif

#ifndef DMA_CPC7_PC_CHNL_LEN
#define DMA_CPC7_PC_CHNL_LEN      32
#endif

#if defined(_V1) && !defined(CPC7_PC_CHNL_R)
#define CPC7_PC_CHNL_R        GetGroupBits32( (DMA0_CPC7_VAL),DMA_CPC7_PC_CHNL_POS, DMA_CPC7_PC_CHNL_LEN)
#endif

#if defined(_V1) && !defined(CPC7_PC_CHNL_W)
#define CPC7_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC7_VAL),DMA_CPC7_PC_CHNL_POS, DMA_CPC7_PC_CHNL_LEN,value)
#endif

#define DMA0_CPC7_PC_CHNL_R        GetGroupBits32( (DMA0_CPC7_VAL),DMA_CPC7_PC_CHNL_POS, DMA_CPC7_PC_CHNL_LEN)

#define DMA0_CPC7_PC_CHNL_W(value) SetGroupBits32( (DMA0_CPC7_VAL),DMA_CPC7_PC_CHNL_POS, DMA_CPC7_PC_CHNL_LEN,value)


/* REGISTER: INTEN ACCESS: RW */

#if defined(_V1) && !defined(INTEN_OFFSET)
#define INTEN_OFFSET 0x20
#endif

#if !defined(DMA_INTEN_OFFSET)
#define DMA_INTEN_OFFSET 0x20
#endif

#if defined(_V1) && !defined(INTEN_REG)
#define INTEN_REG ((volatile UINT32 *) (DMA0_BASE + DMA_INTEN_OFFSET))
#endif

#if defined(_V1) && !defined(INTEN_VAL)
#define INTEN_VAL  PREFIX_VAL(INTEN_REG)
#endif

#define DMA0_INTEN_REG ((volatile UINT32 *) (DMA0_BASE + DMA_INTEN_OFFSET))
#define DMA0_INTEN_VAL  PREFIX_VAL(DMA0_INTEN_REG)

/* FIELDS: */

/* event_irq_select ACCESS: RW */

#ifndef DMA_INTEN_EVENT_IRQ_SELECT_POS
#define DMA_INTEN_EVENT_IRQ_SELECT_POS      0
#endif

#ifndef DMA_INTEN_EVENT_IRQ_SELECT_LEN
#define DMA_INTEN_EVENT_IRQ_SELECT_LEN      32
#endif

#if defined(_V1) && !defined(INTEN_EVENT_IRQ_SELECT_R)
#define INTEN_EVENT_IRQ_SELECT_R        GetGroupBits32( (DMA0_INTEN_VAL),DMA_INTEN_EVENT_IRQ_SELECT_POS, DMA_INTEN_EVENT_IRQ_SELECT_LEN)
#endif

#if defined(_V1) && !defined(INTEN_EVENT_IRQ_SELECT_W)
#define INTEN_EVENT_IRQ_SELECT_W(value) SetGroupBits32( (DMA0_INTEN_VAL),DMA_INTEN_EVENT_IRQ_SELECT_POS, DMA_INTEN_EVENT_IRQ_SELECT_LEN,value)
#endif

#define DMA0_INTEN_EVENT_IRQ_SELECT_R        GetGroupBits32( (DMA0_INTEN_VAL),DMA_INTEN_EVENT_IRQ_SELECT_POS, DMA_INTEN_EVENT_IRQ_SELECT_LEN)

#define DMA0_INTEN_EVENT_IRQ_SELECT_W(value) SetGroupBits32( (DMA0_INTEN_VAL),DMA_INTEN_EVENT_IRQ_SELECT_POS, DMA_INTEN_EVENT_IRQ_SELECT_LEN,value)


/* REGISTER: INT_EVENT_RIS ACCESS: RO */

#if defined(_V1) && !defined(INT_EVENT_RIS_OFFSET)
#define INT_EVENT_RIS_OFFSET 0x24
#endif

#if !defined(DMA_INT_EVENT_RIS_OFFSET)
#define DMA_INT_EVENT_RIS_OFFSET 0x24
#endif

#if defined(_V1) && !defined(INT_EVENT_RIS_REG)
#define INT_EVENT_RIS_REG ((volatile UINT32 *) (DMA0_BASE + DMA_INT_EVENT_RIS_OFFSET))
#endif

#if defined(_V1) && !defined(INT_EVENT_RIS_VAL)
#define INT_EVENT_RIS_VAL  PREFIX_VAL(INT_EVENT_RIS_REG)
#endif

#define DMA0_INT_EVENT_RIS_REG ((volatile UINT32 *) (DMA0_BASE + DMA_INT_EVENT_RIS_OFFSET))
#define DMA0_INT_EVENT_RIS_VAL  PREFIX_VAL(DMA0_INT_EVENT_RIS_REG)

/* FIELDS: */

/* DMASEV_active ACCESS: RO */

#ifndef DMA_INT_EVENT_RIS_DMASEV_ACTIVE_POS
#define DMA_INT_EVENT_RIS_DMASEV_ACTIVE_POS      0
#endif

#ifndef DMA_INT_EVENT_RIS_DMASEV_ACTIVE_LEN
#define DMA_INT_EVENT_RIS_DMASEV_ACTIVE_LEN      32
#endif

#if defined(_V1) && !defined(INT_EVENT_RIS_DMASEV_ACTIVE_R)
#define INT_EVENT_RIS_DMASEV_ACTIVE_R        GetGroupBits32( (DMA0_INT_EVENT_RIS_VAL),DMA_INT_EVENT_RIS_DMASEV_ACTIVE_POS, DMA_INT_EVENT_RIS_DMASEV_ACTIVE_LEN)
#endif

#if defined(_V1) && !defined(INT_EVENT_RIS_DMASEV_ACTIVE_W)
#define INT_EVENT_RIS_DMASEV_ACTIVE_W(value) SetGroupBits32( (DMA0_INT_EVENT_RIS_VAL),DMA_INT_EVENT_RIS_DMASEV_ACTIVE_POS, DMA_INT_EVENT_RIS_DMASEV_ACTIVE_LEN,value)
#endif

#define DMA0_INT_EVENT_RIS_DMASEV_ACTIVE_R        GetGroupBits32( (DMA0_INT_EVENT_RIS_VAL),DMA_INT_EVENT_RIS_DMASEV_ACTIVE_POS, DMA_INT_EVENT_RIS_DMASEV_ACTIVE_LEN)

#define DMA0_INT_EVENT_RIS_DMASEV_ACTIVE_W(value) SetGroupBits32( (DMA0_INT_EVENT_RIS_VAL),DMA_INT_EVENT_RIS_DMASEV_ACTIVE_POS, DMA_INT_EVENT_RIS_DMASEV_ACTIVE_LEN,value)


/* REGISTER: INTMIS ACCESS: RO */

#if defined(_V1) && !defined(INTMIS_OFFSET)
#define INTMIS_OFFSET 0x28
#endif

#if !defined(DMA_INTMIS_OFFSET)
#define DMA_INTMIS_OFFSET 0x28
#endif

#if defined(_V1) && !defined(INTMIS_REG)
#define INTMIS_REG ((volatile UINT32 *) (DMA0_BASE + DMA_INTMIS_OFFSET))
#endif

#if defined(_V1) && !defined(INTMIS_VAL)
#define INTMIS_VAL  PREFIX_VAL(INTMIS_REG)
#endif

#define DMA0_INTMIS_REG ((volatile UINT32 *) (DMA0_BASE + DMA_INTMIS_OFFSET))
#define DMA0_INTMIS_VAL  PREFIX_VAL(DMA0_INTMIS_REG)

/* FIELDS: */

/* irq_status ACCESS: RO */

#ifndef DMA_INTMIS_IRQ_STATUS_POS
#define DMA_INTMIS_IRQ_STATUS_POS      0
#endif

#ifndef DMA_INTMIS_IRQ_STATUS_LEN
#define DMA_INTMIS_IRQ_STATUS_LEN      32
#endif

#if defined(_V1) && !defined(INTMIS_IRQ_STATUS_R)
#define INTMIS_IRQ_STATUS_R        GetGroupBits32( (DMA0_INTMIS_VAL),DMA_INTMIS_IRQ_STATUS_POS, DMA_INTMIS_IRQ_STATUS_LEN)
#endif

#if defined(_V1) && !defined(INTMIS_IRQ_STATUS_W)
#define INTMIS_IRQ_STATUS_W(value) SetGroupBits32( (DMA0_INTMIS_VAL),DMA_INTMIS_IRQ_STATUS_POS, DMA_INTMIS_IRQ_STATUS_LEN,value)
#endif

#define DMA0_INTMIS_IRQ_STATUS_R        GetGroupBits32( (DMA0_INTMIS_VAL),DMA_INTMIS_IRQ_STATUS_POS, DMA_INTMIS_IRQ_STATUS_LEN)

#define DMA0_INTMIS_IRQ_STATUS_W(value) SetGroupBits32( (DMA0_INTMIS_VAL),DMA_INTMIS_IRQ_STATUS_POS, DMA_INTMIS_IRQ_STATUS_LEN,value)


/* REGISTER: INTCLR ACCESS: WO */

#if defined(_V1) && !defined(INTCLR_OFFSET)
#define INTCLR_OFFSET 0x2C
#endif

#if !defined(DMA_INTCLR_OFFSET)
#define DMA_INTCLR_OFFSET 0x2C
#endif

#if defined(_V1) && !defined(INTCLR_REG)
#define INTCLR_REG ((volatile UINT32 *) (DMA0_BASE + DMA_INTCLR_OFFSET))
#endif

#if defined(_V1) && !defined(INTCLR_VAL)
#define INTCLR_VAL  PREFIX_VAL(INTCLR_REG)
#endif

#define DMA0_INTCLR_REG ((volatile UINT32 *) (DMA0_BASE + DMA_INTCLR_OFFSET))
#define DMA0_INTCLR_VAL  PREFIX_VAL(DMA0_INTCLR_REG)

/* FIELDS: */

/* irq_clr ACCESS: WO */

#ifndef DMA_INTCLR_IRQ_CLR_POS
#define DMA_INTCLR_IRQ_CLR_POS      0
#endif

#ifndef DMA_INTCLR_IRQ_CLR_LEN
#define DMA_INTCLR_IRQ_CLR_LEN      32
#endif

#if defined(_V1) && !defined(INTCLR_IRQ_CLR_R)
#define INTCLR_IRQ_CLR_R        GetGroupBits32( (DMA0_INTCLR_VAL),DMA_INTCLR_IRQ_CLR_POS, DMA_INTCLR_IRQ_CLR_LEN)
#endif

#if defined(_V1) && !defined(INTCLR_IRQ_CLR_W)
#define INTCLR_IRQ_CLR_W(value) SetGroupBits32( (DMA0_INTCLR_VAL),DMA_INTCLR_IRQ_CLR_POS, DMA_INTCLR_IRQ_CLR_LEN,value)
#endif

#define DMA0_INTCLR_IRQ_CLR_R        GetGroupBits32( (DMA0_INTCLR_VAL),DMA_INTCLR_IRQ_CLR_POS, DMA_INTCLR_IRQ_CLR_LEN)

#define DMA0_INTCLR_IRQ_CLR_W(value) SetGroupBits32( (DMA0_INTCLR_VAL),DMA_INTCLR_IRQ_CLR_POS, DMA_INTCLR_IRQ_CLR_LEN,value)


/* REGISTER: FSRD ACCESS: RO */

#if defined(_V1) && !defined(FSRD_OFFSET)
#define FSRD_OFFSET 0x30
#endif

#if !defined(DMA_FSRD_OFFSET)
#define DMA_FSRD_OFFSET 0x30
#endif

#if defined(_V1) && !defined(FSRD_REG)
#define FSRD_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FSRD_OFFSET))
#endif

#if defined(_V1) && !defined(FSRD_VAL)
#define FSRD_VAL  PREFIX_VAL(FSRD_REG)
#endif

#define DMA0_FSRD_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FSRD_OFFSET))
#define DMA0_FSRD_VAL  PREFIX_VAL(DMA0_FSRD_REG)

/* FIELDS: */

/* fs_mgr ACCESS: RO */

#ifndef DMA_FSRD_FS_MGR_POS
#define DMA_FSRD_FS_MGR_POS      0
#endif

#ifndef DMA_FSRD_FS_MGR_LEN
#define DMA_FSRD_FS_MGR_LEN      1
#endif

#if defined(_V1) && !defined(FSRD_FS_MGR_R)
#define FSRD_FS_MGR_R        GetGroupBits32( (DMA0_FSRD_VAL),DMA_FSRD_FS_MGR_POS, DMA_FSRD_FS_MGR_LEN)
#endif

#if defined(_V1) && !defined(FSRD_FS_MGR_W)
#define FSRD_FS_MGR_W(value) SetGroupBits32( (DMA0_FSRD_VAL),DMA_FSRD_FS_MGR_POS, DMA_FSRD_FS_MGR_LEN,value)
#endif

#define DMA0_FSRD_FS_MGR_R        GetGroupBits32( (DMA0_FSRD_VAL),DMA_FSRD_FS_MGR_POS, DMA_FSRD_FS_MGR_LEN)

#define DMA0_FSRD_FS_MGR_W(value) SetGroupBits32( (DMA0_FSRD_VAL),DMA_FSRD_FS_MGR_POS, DMA_FSRD_FS_MGR_LEN,value)


/* REGISTER: FSRC ACCESS: RO */

#if defined(_V1) && !defined(FSRC_OFFSET)
#define FSRC_OFFSET 0x34
#endif

#if !defined(DMA_FSRC_OFFSET)
#define DMA_FSRC_OFFSET 0x34
#endif

#if defined(_V1) && !defined(FSRC_REG)
#define FSRC_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FSRC_OFFSET))
#endif

#if defined(_V1) && !defined(FSRC_VAL)
#define FSRC_VAL  PREFIX_VAL(FSRC_REG)
#endif

#define DMA0_FSRC_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FSRC_OFFSET))
#define DMA0_FSRC_VAL  PREFIX_VAL(DMA0_FSRC_REG)

/* FIELDS: */

/* fault_status ACCESS: RO */

#ifndef DMA_FSRC_FAULT_STATUS_POS
#define DMA_FSRC_FAULT_STATUS_POS      0
#endif

#ifndef DMA_FSRC_FAULT_STATUS_LEN
#define DMA_FSRC_FAULT_STATUS_LEN      8
#endif

#if defined(_V1) && !defined(FSRC_FAULT_STATUS_R)
#define FSRC_FAULT_STATUS_R        GetGroupBits32( (DMA0_FSRC_VAL),DMA_FSRC_FAULT_STATUS_POS, DMA_FSRC_FAULT_STATUS_LEN)
#endif

#if defined(_V1) && !defined(FSRC_FAULT_STATUS_W)
#define FSRC_FAULT_STATUS_W(value) SetGroupBits32( (DMA0_FSRC_VAL),DMA_FSRC_FAULT_STATUS_POS, DMA_FSRC_FAULT_STATUS_LEN,value)
#endif

#define DMA0_FSRC_FAULT_STATUS_R        GetGroupBits32( (DMA0_FSRC_VAL),DMA_FSRC_FAULT_STATUS_POS, DMA_FSRC_FAULT_STATUS_LEN)

#define DMA0_FSRC_FAULT_STATUS_W(value) SetGroupBits32( (DMA0_FSRC_VAL),DMA_FSRC_FAULT_STATUS_POS, DMA_FSRC_FAULT_STATUS_LEN,value)


/* REGISTER: FTRD ACCESS: RO */

#if defined(_V1) && !defined(FTRD_OFFSET)
#define FTRD_OFFSET 0x38
#endif

#if !defined(DMA_FTRD_OFFSET)
#define DMA_FTRD_OFFSET 0x38
#endif

#if defined(_V1) && !defined(FTRD_REG)
#define FTRD_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTRD_OFFSET))
#endif

#if defined(_V1) && !defined(FTRD_VAL)
#define FTRD_VAL  PREFIX_VAL(FTRD_REG)
#endif

#define DMA0_FTRD_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTRD_OFFSET))
#define DMA0_FTRD_VAL  PREFIX_VAL(DMA0_FTRD_REG)

/* FIELDS: */

/* undef_instr ACCESS: RO */

#ifndef DMA_FTRD_UNDEF_INSTR_POS
#define DMA_FTRD_UNDEF_INSTR_POS      0
#endif

#ifndef DMA_FTRD_UNDEF_INSTR_LEN
#define DMA_FTRD_UNDEF_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTRD_UNDEF_INSTR_R)
#define FTRD_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_UNDEF_INSTR_POS, DMA_FTRD_UNDEF_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTRD_UNDEF_INSTR_W)
#define FTRD_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_UNDEF_INSTR_POS, DMA_FTRD_UNDEF_INSTR_LEN,value)
#endif

#define DMA0_FTRD_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_UNDEF_INSTR_POS, DMA_FTRD_UNDEF_INSTR_LEN)

#define DMA0_FTRD_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_UNDEF_INSTR_POS, DMA_FTRD_UNDEF_INSTR_LEN,value)


/* operand_invalid ACCESS: RO */

#ifndef DMA_FTRD_OPERAND_INVALID_POS
#define DMA_FTRD_OPERAND_INVALID_POS      1
#endif

#ifndef DMA_FTRD_OPERAND_INVALID_LEN
#define DMA_FTRD_OPERAND_INVALID_LEN      1
#endif

#if defined(_V1) && !defined(FTRD_OPERAND_INVALID_R)
#define FTRD_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_OPERAND_INVALID_POS, DMA_FTRD_OPERAND_INVALID_LEN)
#endif

#if defined(_V1) && !defined(FTRD_OPERAND_INVALID_W)
#define FTRD_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_OPERAND_INVALID_POS, DMA_FTRD_OPERAND_INVALID_LEN,value)
#endif

#define DMA0_FTRD_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_OPERAND_INVALID_POS, DMA_FTRD_OPERAND_INVALID_LEN)

#define DMA0_FTRD_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_OPERAND_INVALID_POS, DMA_FTRD_OPERAND_INVALID_LEN,value)


/* dmago_err ACCESS: RO */

#ifndef DMA_FTRD_DMAGO_ERR_POS
#define DMA_FTRD_DMAGO_ERR_POS      4
#endif

#ifndef DMA_FTRD_DMAGO_ERR_LEN
#define DMA_FTRD_DMAGO_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTRD_DMAGO_ERR_R)
#define FTRD_DMAGO_ERR_R        GetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_DMAGO_ERR_POS, DMA_FTRD_DMAGO_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTRD_DMAGO_ERR_W)
#define FTRD_DMAGO_ERR_W(value) SetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_DMAGO_ERR_POS, DMA_FTRD_DMAGO_ERR_LEN,value)
#endif

#define DMA0_FTRD_DMAGO_ERR_R        GetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_DMAGO_ERR_POS, DMA_FTRD_DMAGO_ERR_LEN)

#define DMA0_FTRD_DMAGO_ERR_W(value) SetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_DMAGO_ERR_POS, DMA_FTRD_DMAGO_ERR_LEN,value)


/* mgr_evnt_err ACCESS: RO */

#ifndef DMA_FTRD_MGR_EVNT_ERR_POS
#define DMA_FTRD_MGR_EVNT_ERR_POS      5
#endif

#ifndef DMA_FTRD_MGR_EVNT_ERR_LEN
#define DMA_FTRD_MGR_EVNT_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTRD_MGR_EVNT_ERR_R)
#define FTRD_MGR_EVNT_ERR_R        GetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_MGR_EVNT_ERR_POS, DMA_FTRD_MGR_EVNT_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTRD_MGR_EVNT_ERR_W)
#define FTRD_MGR_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_MGR_EVNT_ERR_POS, DMA_FTRD_MGR_EVNT_ERR_LEN,value)
#endif

#define DMA0_FTRD_MGR_EVNT_ERR_R        GetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_MGR_EVNT_ERR_POS, DMA_FTRD_MGR_EVNT_ERR_LEN)

#define DMA0_FTRD_MGR_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_MGR_EVNT_ERR_POS, DMA_FTRD_MGR_EVNT_ERR_LEN,value)


/* instr_fetch_err ACCESS: RO */

#ifndef DMA_FTRD_INSTR_FETCH_ERR_POS
#define DMA_FTRD_INSTR_FETCH_ERR_POS      16
#endif

#ifndef DMA_FTRD_INSTR_FETCH_ERR_LEN
#define DMA_FTRD_INSTR_FETCH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTRD_INSTR_FETCH_ERR_R)
#define FTRD_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_INSTR_FETCH_ERR_POS, DMA_FTRD_INSTR_FETCH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTRD_INSTR_FETCH_ERR_W)
#define FTRD_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_INSTR_FETCH_ERR_POS, DMA_FTRD_INSTR_FETCH_ERR_LEN,value)
#endif

#define DMA0_FTRD_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_INSTR_FETCH_ERR_POS, DMA_FTRD_INSTR_FETCH_ERR_LEN)

#define DMA0_FTRD_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_INSTR_FETCH_ERR_POS, DMA_FTRD_INSTR_FETCH_ERR_LEN,value)


/* dbg_instr ACCESS: RO */

#ifndef DMA_FTRD_DBG_INSTR_POS
#define DMA_FTRD_DBG_INSTR_POS      30
#endif

#ifndef DMA_FTRD_DBG_INSTR_LEN
#define DMA_FTRD_DBG_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTRD_DBG_INSTR_R)
#define FTRD_DBG_INSTR_R        GetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_DBG_INSTR_POS, DMA_FTRD_DBG_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTRD_DBG_INSTR_W)
#define FTRD_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_DBG_INSTR_POS, DMA_FTRD_DBG_INSTR_LEN,value)
#endif

#define DMA0_FTRD_DBG_INSTR_R        GetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_DBG_INSTR_POS, DMA_FTRD_DBG_INSTR_LEN)

#define DMA0_FTRD_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTRD_VAL),DMA_FTRD_DBG_INSTR_POS, DMA_FTRD_DBG_INSTR_LEN,value)


/* REGISTER: DPC ACCESS: RO */

#if defined(_V1) && !defined(DPC_OFFSET)
#define DPC_OFFSET 0x4
#endif

#if !defined(DMA_DPC_OFFSET)
#define DMA_DPC_OFFSET 0x4
#endif

#if defined(_V1) && !defined(DPC_REG)
#define DPC_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DPC_OFFSET))
#endif

#if defined(_V1) && !defined(DPC_VAL)
#define DPC_VAL  PREFIX_VAL(DPC_REG)
#endif

#define DMA0_DPC_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DPC_OFFSET))
#define DMA0_DPC_VAL  PREFIX_VAL(DMA0_DPC_REG)

/* FIELDS: */

/* pc_mgr ACCESS: RO */

#ifndef DMA_DPC_PC_MGR_POS
#define DMA_DPC_PC_MGR_POS      0
#endif

#ifndef DMA_DPC_PC_MGR_LEN
#define DMA_DPC_PC_MGR_LEN      32
#endif

#if defined(_V1) && !defined(DPC_PC_MGR_R)
#define DPC_PC_MGR_R        GetGroupBits32( (DMA0_DPC_VAL),DMA_DPC_PC_MGR_POS, DMA_DPC_PC_MGR_LEN)
#endif

#if defined(_V1) && !defined(DPC_PC_MGR_W)
#define DPC_PC_MGR_W(value) SetGroupBits32( (DMA0_DPC_VAL),DMA_DPC_PC_MGR_POS, DMA_DPC_PC_MGR_LEN,value)
#endif

#define DMA0_DPC_PC_MGR_R        GetGroupBits32( (DMA0_DPC_VAL),DMA_DPC_PC_MGR_POS, DMA_DPC_PC_MGR_LEN)

#define DMA0_DPC_PC_MGR_W(value) SetGroupBits32( (DMA0_DPC_VAL),DMA_DPC_PC_MGR_POS, DMA_DPC_PC_MGR_LEN,value)


/* REGISTER: FTR0 ACCESS: RO */

#if defined(_V1) && !defined(FTR0_OFFSET)
#define FTR0_OFFSET 0x40
#endif

#if !defined(DMA_FTR0_OFFSET)
#define DMA_FTR0_OFFSET 0x40
#endif

#if defined(_V1) && !defined(FTR0_REG)
#define FTR0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR0_OFFSET))
#endif

#if defined(_V1) && !defined(FTR0_VAL)
#define FTR0_VAL  PREFIX_VAL(FTR0_REG)
#endif

#define DMA0_FTR0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR0_OFFSET))
#define DMA0_FTR0_VAL  PREFIX_VAL(DMA0_FTR0_REG)

/* FIELDS: */

/* undef_instr ACCESS: RO */

#ifndef DMA_FTR0_UNDEF_INSTR_POS
#define DMA_FTR0_UNDEF_INSTR_POS      0
#endif

#ifndef DMA_FTR0_UNDEF_INSTR_LEN
#define DMA_FTR0_UNDEF_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR0_UNDEF_INSTR_R)
#define FTR0_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_UNDEF_INSTR_POS, DMA_FTR0_UNDEF_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR0_UNDEF_INSTR_W)
#define FTR0_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_UNDEF_INSTR_POS, DMA_FTR0_UNDEF_INSTR_LEN,value)
#endif

#define DMA0_FTR0_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_UNDEF_INSTR_POS, DMA_FTR0_UNDEF_INSTR_LEN)

#define DMA0_FTR0_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_UNDEF_INSTR_POS, DMA_FTR0_UNDEF_INSTR_LEN,value)


/* operand_invalid ACCESS: RO */

#ifndef DMA_FTR0_OPERAND_INVALID_POS
#define DMA_FTR0_OPERAND_INVALID_POS      1
#endif

#ifndef DMA_FTR0_OPERAND_INVALID_LEN
#define DMA_FTR0_OPERAND_INVALID_LEN      1
#endif

#if defined(_V1) && !defined(FTR0_OPERAND_INVALID_R)
#define FTR0_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_OPERAND_INVALID_POS, DMA_FTR0_OPERAND_INVALID_LEN)
#endif

#if defined(_V1) && !defined(FTR0_OPERAND_INVALID_W)
#define FTR0_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_OPERAND_INVALID_POS, DMA_FTR0_OPERAND_INVALID_LEN,value)
#endif

#define DMA0_FTR0_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_OPERAND_INVALID_POS, DMA_FTR0_OPERAND_INVALID_LEN)

#define DMA0_FTR0_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_OPERAND_INVALID_POS, DMA_FTR0_OPERAND_INVALID_LEN,value)


/* ch_evnt_err ACCESS: RO */

#ifndef DMA_FTR0_CH_EVNT_ERR_POS
#define DMA_FTR0_CH_EVNT_ERR_POS      5
#endif

#ifndef DMA_FTR0_CH_EVNT_ERR_LEN
#define DMA_FTR0_CH_EVNT_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR0_CH_EVNT_ERR_R)
#define FTR0_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_CH_EVNT_ERR_POS, DMA_FTR0_CH_EVNT_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR0_CH_EVNT_ERR_W)
#define FTR0_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_CH_EVNT_ERR_POS, DMA_FTR0_CH_EVNT_ERR_LEN,value)
#endif

#define DMA0_FTR0_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_CH_EVNT_ERR_POS, DMA_FTR0_CH_EVNT_ERR_LEN)

#define DMA0_FTR0_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_CH_EVNT_ERR_POS, DMA_FTR0_CH_EVNT_ERR_LEN,value)


/* ch_periph_err ACCESS: RO */

#ifndef DMA_FTR0_CH_PERIPH_ERR_POS
#define DMA_FTR0_CH_PERIPH_ERR_POS      6
#endif

#ifndef DMA_FTR0_CH_PERIPH_ERR_LEN
#define DMA_FTR0_CH_PERIPH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR0_CH_PERIPH_ERR_R)
#define FTR0_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_CH_PERIPH_ERR_POS, DMA_FTR0_CH_PERIPH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR0_CH_PERIPH_ERR_W)
#define FTR0_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_CH_PERIPH_ERR_POS, DMA_FTR0_CH_PERIPH_ERR_LEN,value)
#endif

#define DMA0_FTR0_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_CH_PERIPH_ERR_POS, DMA_FTR0_CH_PERIPH_ERR_LEN)

#define DMA0_FTR0_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_CH_PERIPH_ERR_POS, DMA_FTR0_CH_PERIPH_ERR_LEN,value)


/* ch_rdwr_err ACCESS: RO */

#ifndef DMA_FTR0_CH_RDWR_ERR_POS
#define DMA_FTR0_CH_RDWR_ERR_POS      7
#endif

#ifndef DMA_FTR0_CH_RDWR_ERR_LEN
#define DMA_FTR0_CH_RDWR_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR0_CH_RDWR_ERR_R)
#define FTR0_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_CH_RDWR_ERR_POS, DMA_FTR0_CH_RDWR_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR0_CH_RDWR_ERR_W)
#define FTR0_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_CH_RDWR_ERR_POS, DMA_FTR0_CH_RDWR_ERR_LEN,value)
#endif

#define DMA0_FTR0_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_CH_RDWR_ERR_POS, DMA_FTR0_CH_RDWR_ERR_LEN)

#define DMA0_FTR0_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_CH_RDWR_ERR_POS, DMA_FTR0_CH_RDWR_ERR_LEN,value)


/* mfifo_err ACCESS: RO */

#ifndef DMA_FTR0_MFIFO_ERR_POS
#define DMA_FTR0_MFIFO_ERR_POS      12
#endif

#ifndef DMA_FTR0_MFIFO_ERR_LEN
#define DMA_FTR0_MFIFO_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR0_MFIFO_ERR_R)
#define FTR0_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_MFIFO_ERR_POS, DMA_FTR0_MFIFO_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR0_MFIFO_ERR_W)
#define FTR0_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_MFIFO_ERR_POS, DMA_FTR0_MFIFO_ERR_LEN,value)
#endif

#define DMA0_FTR0_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_MFIFO_ERR_POS, DMA_FTR0_MFIFO_ERR_LEN)

#define DMA0_FTR0_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_MFIFO_ERR_POS, DMA_FTR0_MFIFO_ERR_LEN,value)


/* st_data_unavailable ACCESS: RO */

#ifndef DMA_FTR0_ST_DATA_UNAVAILABLE_POS
#define DMA_FTR0_ST_DATA_UNAVAILABLE_POS      13
#endif

#ifndef DMA_FTR0_ST_DATA_UNAVAILABLE_LEN
#define DMA_FTR0_ST_DATA_UNAVAILABLE_LEN      1
#endif

#if defined(_V1) && !defined(FTR0_ST_DATA_UNAVAILABLE_R)
#define FTR0_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_ST_DATA_UNAVAILABLE_POS, DMA_FTR0_ST_DATA_UNAVAILABLE_LEN)
#endif

#if defined(_V1) && !defined(FTR0_ST_DATA_UNAVAILABLE_W)
#define FTR0_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_ST_DATA_UNAVAILABLE_POS, DMA_FTR0_ST_DATA_UNAVAILABLE_LEN,value)
#endif

#define DMA0_FTR0_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_ST_DATA_UNAVAILABLE_POS, DMA_FTR0_ST_DATA_UNAVAILABLE_LEN)

#define DMA0_FTR0_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_ST_DATA_UNAVAILABLE_POS, DMA_FTR0_ST_DATA_UNAVAILABLE_LEN,value)


/* instr_fetch_err ACCESS: RO */

#ifndef DMA_FTR0_INSTR_FETCH_ERR_POS
#define DMA_FTR0_INSTR_FETCH_ERR_POS      16
#endif

#ifndef DMA_FTR0_INSTR_FETCH_ERR_LEN
#define DMA_FTR0_INSTR_FETCH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR0_INSTR_FETCH_ERR_R)
#define FTR0_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_INSTR_FETCH_ERR_POS, DMA_FTR0_INSTR_FETCH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR0_INSTR_FETCH_ERR_W)
#define FTR0_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_INSTR_FETCH_ERR_POS, DMA_FTR0_INSTR_FETCH_ERR_LEN,value)
#endif

#define DMA0_FTR0_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_INSTR_FETCH_ERR_POS, DMA_FTR0_INSTR_FETCH_ERR_LEN)

#define DMA0_FTR0_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_INSTR_FETCH_ERR_POS, DMA_FTR0_INSTR_FETCH_ERR_LEN,value)


/* data_write_err ACCESS: RO */

#ifndef DMA_FTR0_DATA_WRITE_ERR_POS
#define DMA_FTR0_DATA_WRITE_ERR_POS      17
#endif

#ifndef DMA_FTR0_DATA_WRITE_ERR_LEN
#define DMA_FTR0_DATA_WRITE_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR0_DATA_WRITE_ERR_R)
#define FTR0_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_DATA_WRITE_ERR_POS, DMA_FTR0_DATA_WRITE_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR0_DATA_WRITE_ERR_W)
#define FTR0_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_DATA_WRITE_ERR_POS, DMA_FTR0_DATA_WRITE_ERR_LEN,value)
#endif

#define DMA0_FTR0_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_DATA_WRITE_ERR_POS, DMA_FTR0_DATA_WRITE_ERR_LEN)

#define DMA0_FTR0_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_DATA_WRITE_ERR_POS, DMA_FTR0_DATA_WRITE_ERR_LEN,value)


/* data_read_err ACCESS: RO */

#ifndef DMA_FTR0_DATA_READ_ERR_POS
#define DMA_FTR0_DATA_READ_ERR_POS      18
#endif

#ifndef DMA_FTR0_DATA_READ_ERR_LEN
#define DMA_FTR0_DATA_READ_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR0_DATA_READ_ERR_R)
#define FTR0_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_DATA_READ_ERR_POS, DMA_FTR0_DATA_READ_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR0_DATA_READ_ERR_W)
#define FTR0_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_DATA_READ_ERR_POS, DMA_FTR0_DATA_READ_ERR_LEN,value)
#endif

#define DMA0_FTR0_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_DATA_READ_ERR_POS, DMA_FTR0_DATA_READ_ERR_LEN)

#define DMA0_FTR0_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_DATA_READ_ERR_POS, DMA_FTR0_DATA_READ_ERR_LEN,value)


/* dbg_instr ACCESS: RO */

#ifndef DMA_FTR0_DBG_INSTR_POS
#define DMA_FTR0_DBG_INSTR_POS      30
#endif

#ifndef DMA_FTR0_DBG_INSTR_LEN
#define DMA_FTR0_DBG_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR0_DBG_INSTR_R)
#define FTR0_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_DBG_INSTR_POS, DMA_FTR0_DBG_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR0_DBG_INSTR_W)
#define FTR0_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_DBG_INSTR_POS, DMA_FTR0_DBG_INSTR_LEN,value)
#endif

#define DMA0_FTR0_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_DBG_INSTR_POS, DMA_FTR0_DBG_INSTR_LEN)

#define DMA0_FTR0_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_DBG_INSTR_POS, DMA_FTR0_DBG_INSTR_LEN,value)


/* lockup_err ACCESS: RO */

#ifndef DMA_FTR0_LOCKUP_ERR_POS
#define DMA_FTR0_LOCKUP_ERR_POS      31
#endif

#ifndef DMA_FTR0_LOCKUP_ERR_LEN
#define DMA_FTR0_LOCKUP_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR0_LOCKUP_ERR_R)
#define FTR0_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_LOCKUP_ERR_POS, DMA_FTR0_LOCKUP_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR0_LOCKUP_ERR_W)
#define FTR0_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_LOCKUP_ERR_POS, DMA_FTR0_LOCKUP_ERR_LEN,value)
#endif

#define DMA0_FTR0_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_LOCKUP_ERR_POS, DMA_FTR0_LOCKUP_ERR_LEN)

#define DMA0_FTR0_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR0_VAL),DMA_FTR0_LOCKUP_ERR_POS, DMA_FTR0_LOCKUP_ERR_LEN,value)


/* REGISTER: SAR0 ACCESS: RO */

#if defined(_V1) && !defined(SAR0_OFFSET)
#define SAR0_OFFSET 0x400
#endif

#if !defined(DMA_SAR0_OFFSET)
#define DMA_SAR0_OFFSET 0x400
#endif

#if defined(_V1) && !defined(SAR0_REG)
#define SAR0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR0_OFFSET))
#endif

#if defined(_V1) && !defined(SAR0_VAL)
#define SAR0_VAL  PREFIX_VAL(SAR0_REG)
#endif

#define DMA0_SAR0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR0_OFFSET))
#define DMA0_SAR0_VAL  PREFIX_VAL(DMA0_SAR0_REG)

/* FIELDS: */

/* src_addr ACCESS: RO */

#ifndef DMA_SAR0_SRC_ADDR_POS
#define DMA_SAR0_SRC_ADDR_POS      0
#endif

#ifndef DMA_SAR0_SRC_ADDR_LEN
#define DMA_SAR0_SRC_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(SAR0_SRC_ADDR_R)
#define SAR0_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR0_VAL),DMA_SAR0_SRC_ADDR_POS, DMA_SAR0_SRC_ADDR_LEN)
#endif

#if defined(_V1) && !defined(SAR0_SRC_ADDR_W)
#define SAR0_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR0_VAL),DMA_SAR0_SRC_ADDR_POS, DMA_SAR0_SRC_ADDR_LEN,value)
#endif

#define DMA0_SAR0_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR0_VAL),DMA_SAR0_SRC_ADDR_POS, DMA_SAR0_SRC_ADDR_LEN)

#define DMA0_SAR0_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR0_VAL),DMA_SAR0_SRC_ADDR_POS, DMA_SAR0_SRC_ADDR_LEN,value)


/* REGISTER: DAR0 ACCESS: RO */

#if defined(_V1) && !defined(DAR0_OFFSET)
#define DAR0_OFFSET 0x404
#endif

#if !defined(DMA_DAR0_OFFSET)
#define DMA_DAR0_OFFSET 0x404
#endif

#if defined(_V1) && !defined(DAR0_REG)
#define DAR0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR0_OFFSET))
#endif

#if defined(_V1) && !defined(DAR0_VAL)
#define DAR0_VAL  PREFIX_VAL(DAR0_REG)
#endif

#define DMA0_DAR0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR0_OFFSET))
#define DMA0_DAR0_VAL  PREFIX_VAL(DMA0_DAR0_REG)

/* FIELDS: */

/* dst_addr ACCESS: RO */

#ifndef DMA_DAR0_DST_ADDR_POS
#define DMA_DAR0_DST_ADDR_POS      0
#endif

#ifndef DMA_DAR0_DST_ADDR_LEN
#define DMA_DAR0_DST_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(DAR0_DST_ADDR_R)
#define DAR0_DST_ADDR_R        GetGroupBits32( (DMA0_DAR0_VAL),DMA_DAR0_DST_ADDR_POS, DMA_DAR0_DST_ADDR_LEN)
#endif

#if defined(_V1) && !defined(DAR0_DST_ADDR_W)
#define DAR0_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR0_VAL),DMA_DAR0_DST_ADDR_POS, DMA_DAR0_DST_ADDR_LEN,value)
#endif

#define DMA0_DAR0_DST_ADDR_R        GetGroupBits32( (DMA0_DAR0_VAL),DMA_DAR0_DST_ADDR_POS, DMA_DAR0_DST_ADDR_LEN)

#define DMA0_DAR0_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR0_VAL),DMA_DAR0_DST_ADDR_POS, DMA_DAR0_DST_ADDR_LEN,value)


/* REGISTER: CCR0 ACCESS: RO */

#if defined(_V1) && !defined(CCR0_OFFSET)
#define CCR0_OFFSET 0x408
#endif

#if !defined(DMA_CCR0_OFFSET)
#define DMA_CCR0_OFFSET 0x408
#endif

#if defined(_V1) && !defined(CCR0_REG)
#define CCR0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR0_OFFSET))
#endif

#if defined(_V1) && !defined(CCR0_VAL)
#define CCR0_VAL  PREFIX_VAL(CCR0_REG)
#endif

#define DMA0_CCR0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR0_OFFSET))
#define DMA0_CCR0_VAL  PREFIX_VAL(DMA0_CCR0_REG)

/* FIELDS: */

/* src_inc ACCESS: RO */

#ifndef DMA_CCR0_SRC_INC_POS
#define DMA_CCR0_SRC_INC_POS      0
#endif

#ifndef DMA_CCR0_SRC_INC_LEN
#define DMA_CCR0_SRC_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR0_SRC_INC_R)
#define CCR0_SRC_INC_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_INC_POS, DMA_CCR0_SRC_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR0_SRC_INC_W)
#define CCR0_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_INC_POS, DMA_CCR0_SRC_INC_LEN,value)
#endif

#define DMA0_CCR0_SRC_INC_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_INC_POS, DMA_CCR0_SRC_INC_LEN)

#define DMA0_CCR0_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_INC_POS, DMA_CCR0_SRC_INC_LEN,value)


/* src_burst_size ACCESS: RO */

#ifndef DMA_CCR0_SRC_BURST_SIZE_POS
#define DMA_CCR0_SRC_BURST_SIZE_POS      1
#endif

#ifndef DMA_CCR0_SRC_BURST_SIZE_LEN
#define DMA_CCR0_SRC_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR0_SRC_BURST_SIZE_R)
#define CCR0_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_BURST_SIZE_POS, DMA_CCR0_SRC_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR0_SRC_BURST_SIZE_W)
#define CCR0_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_BURST_SIZE_POS, DMA_CCR0_SRC_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR0_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_BURST_SIZE_POS, DMA_CCR0_SRC_BURST_SIZE_LEN)

#define DMA0_CCR0_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_BURST_SIZE_POS, DMA_CCR0_SRC_BURST_SIZE_LEN,value)


/* src_burst_len ACCESS: RO */

#ifndef DMA_CCR0_SRC_BURST_LEN_POS
#define DMA_CCR0_SRC_BURST_LEN_POS      4
#endif

#ifndef DMA_CCR0_SRC_BURST_LEN_LEN
#define DMA_CCR0_SRC_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR0_SRC_BURST_LEN_R)
#define CCR0_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_BURST_LEN_POS, DMA_CCR0_SRC_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR0_SRC_BURST_LEN_W)
#define CCR0_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_BURST_LEN_POS, DMA_CCR0_SRC_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR0_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_BURST_LEN_POS, DMA_CCR0_SRC_BURST_LEN_LEN)

#define DMA0_CCR0_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_BURST_LEN_POS, DMA_CCR0_SRC_BURST_LEN_LEN,value)


/* src_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR0_SRC_PROT_CTRL_POS
#define DMA_CCR0_SRC_PROT_CTRL_POS      8
#endif

#ifndef DMA_CCR0_SRC_PROT_CTRL_LEN
#define DMA_CCR0_SRC_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR0_SRC_PROT_CTRL_R)
#define CCR0_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_PROT_CTRL_POS, DMA_CCR0_SRC_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR0_SRC_PROT_CTRL_W)
#define CCR0_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_PROT_CTRL_POS, DMA_CCR0_SRC_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR0_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_PROT_CTRL_POS, DMA_CCR0_SRC_PROT_CTRL_LEN)

#define DMA0_CCR0_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_PROT_CTRL_POS, DMA_CCR0_SRC_PROT_CTRL_LEN,value)


/* src_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR0_SRC_CACHE_CTRL_POS
#define DMA_CCR0_SRC_CACHE_CTRL_POS      11
#endif

#ifndef DMA_CCR0_SRC_CACHE_CTRL_LEN
#define DMA_CCR0_SRC_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR0_SRC_CACHE_CTRL_R)
#define CCR0_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_CACHE_CTRL_POS, DMA_CCR0_SRC_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR0_SRC_CACHE_CTRL_W)
#define CCR0_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_CACHE_CTRL_POS, DMA_CCR0_SRC_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR0_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_CACHE_CTRL_POS, DMA_CCR0_SRC_CACHE_CTRL_LEN)

#define DMA0_CCR0_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_SRC_CACHE_CTRL_POS, DMA_CCR0_SRC_CACHE_CTRL_LEN,value)


/* dst_inc ACCESS: RO */

#ifndef DMA_CCR0_DST_INC_POS
#define DMA_CCR0_DST_INC_POS      14
#endif

#ifndef DMA_CCR0_DST_INC_LEN
#define DMA_CCR0_DST_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR0_DST_INC_R)
#define CCR0_DST_INC_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_INC_POS, DMA_CCR0_DST_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR0_DST_INC_W)
#define CCR0_DST_INC_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_INC_POS, DMA_CCR0_DST_INC_LEN,value)
#endif

#define DMA0_CCR0_DST_INC_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_INC_POS, DMA_CCR0_DST_INC_LEN)

#define DMA0_CCR0_DST_INC_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_INC_POS, DMA_CCR0_DST_INC_LEN,value)


/* dst_burst_size ACCESS: RO */

#ifndef DMA_CCR0_DST_BURST_SIZE_POS
#define DMA_CCR0_DST_BURST_SIZE_POS      15
#endif

#ifndef DMA_CCR0_DST_BURST_SIZE_LEN
#define DMA_CCR0_DST_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR0_DST_BURST_SIZE_R)
#define CCR0_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_BURST_SIZE_POS, DMA_CCR0_DST_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR0_DST_BURST_SIZE_W)
#define CCR0_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_BURST_SIZE_POS, DMA_CCR0_DST_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR0_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_BURST_SIZE_POS, DMA_CCR0_DST_BURST_SIZE_LEN)

#define DMA0_CCR0_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_BURST_SIZE_POS, DMA_CCR0_DST_BURST_SIZE_LEN,value)


/* dst_burst_len ACCESS: RO */

#ifndef DMA_CCR0_DST_BURST_LEN_POS
#define DMA_CCR0_DST_BURST_LEN_POS      18
#endif

#ifndef DMA_CCR0_DST_BURST_LEN_LEN
#define DMA_CCR0_DST_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR0_DST_BURST_LEN_R)
#define CCR0_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_BURST_LEN_POS, DMA_CCR0_DST_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR0_DST_BURST_LEN_W)
#define CCR0_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_BURST_LEN_POS, DMA_CCR0_DST_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR0_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_BURST_LEN_POS, DMA_CCR0_DST_BURST_LEN_LEN)

#define DMA0_CCR0_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_BURST_LEN_POS, DMA_CCR0_DST_BURST_LEN_LEN,value)


/* dst_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR0_DST_PROT_CTRL_POS
#define DMA_CCR0_DST_PROT_CTRL_POS      22
#endif

#ifndef DMA_CCR0_DST_PROT_CTRL_LEN
#define DMA_CCR0_DST_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR0_DST_PROT_CTRL_R)
#define CCR0_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_PROT_CTRL_POS, DMA_CCR0_DST_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR0_DST_PROT_CTRL_W)
#define CCR0_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_PROT_CTRL_POS, DMA_CCR0_DST_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR0_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_PROT_CTRL_POS, DMA_CCR0_DST_PROT_CTRL_LEN)

#define DMA0_CCR0_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_PROT_CTRL_POS, DMA_CCR0_DST_PROT_CTRL_LEN,value)


/* dst_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR0_DST_CACHE_CTRL_POS
#define DMA_CCR0_DST_CACHE_CTRL_POS      25
#endif

#ifndef DMA_CCR0_DST_CACHE_CTRL_LEN
#define DMA_CCR0_DST_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR0_DST_CACHE_CTRL_R)
#define CCR0_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_CACHE_CTRL_POS, DMA_CCR0_DST_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR0_DST_CACHE_CTRL_W)
#define CCR0_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_CACHE_CTRL_POS, DMA_CCR0_DST_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR0_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_CACHE_CTRL_POS, DMA_CCR0_DST_CACHE_CTRL_LEN)

#define DMA0_CCR0_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_DST_CACHE_CTRL_POS, DMA_CCR0_DST_CACHE_CTRL_LEN,value)


/* endian_swap_size ACCESS: RO */

#ifndef DMA_CCR0_ENDIAN_SWAP_SIZE_POS
#define DMA_CCR0_ENDIAN_SWAP_SIZE_POS      28
#endif

#ifndef DMA_CCR0_ENDIAN_SWAP_SIZE_LEN
#define DMA_CCR0_ENDIAN_SWAP_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR0_ENDIAN_SWAP_SIZE_R)
#define CCR0_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_ENDIAN_SWAP_SIZE_POS, DMA_CCR0_ENDIAN_SWAP_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR0_ENDIAN_SWAP_SIZE_W)
#define CCR0_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_ENDIAN_SWAP_SIZE_POS, DMA_CCR0_ENDIAN_SWAP_SIZE_LEN,value)
#endif

#define DMA0_CCR0_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_ENDIAN_SWAP_SIZE_POS, DMA_CCR0_ENDIAN_SWAP_SIZE_LEN)

#define DMA0_CCR0_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR0_VAL),DMA_CCR0_ENDIAN_SWAP_SIZE_POS, DMA_CCR0_ENDIAN_SWAP_SIZE_LEN,value)


/* REGISTER: LC0_0 ACCESS: RO */

#if defined(_V1) && !defined(LC0_0_OFFSET)
#define LC0_0_OFFSET 0x40C
#endif

#if !defined(DMA_LC0_0_OFFSET)
#define DMA_LC0_0_OFFSET 0x40C
#endif

#if defined(_V1) && !defined(LC0_0_REG)
#define LC0_0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_0_OFFSET))
#endif

#if defined(_V1) && !defined(LC0_0_VAL)
#define LC0_0_VAL  PREFIX_VAL(LC0_0_REG)
#endif

#define DMA0_LC0_0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_0_OFFSET))
#define DMA0_LC0_0_VAL  PREFIX_VAL(DMA0_LC0_0_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC0_0_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC0_0_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC0_0_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC0_0_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC0_0_LOOP_COUNTER_ITERATIONS_R)
#define LC0_0_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_0_VAL),DMA_LC0_0_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_0_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC0_0_LOOP_COUNTER_ITERATIONS_W)
#define LC0_0_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_0_VAL),DMA_LC0_0_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_0_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC0_0_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_0_VAL),DMA_LC0_0_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_0_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC0_0_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_0_VAL),DMA_LC0_0_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_0_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: LC1_0 ACCESS: RO */

#if defined(_V1) && !defined(LC1_0_OFFSET)
#define LC1_0_OFFSET 0x410
#endif

#if !defined(DMA_LC1_0_OFFSET)
#define DMA_LC1_0_OFFSET 0x410
#endif

#if defined(_V1) && !defined(LC1_0_REG)
#define LC1_0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_0_OFFSET))
#endif

#if defined(_V1) && !defined(LC1_0_VAL)
#define LC1_0_VAL  PREFIX_VAL(LC1_0_REG)
#endif

#define DMA0_LC1_0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_0_OFFSET))
#define DMA0_LC1_0_VAL  PREFIX_VAL(DMA0_LC1_0_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC1_0_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC1_0_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC1_0_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC1_0_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC1_0_LOOP_COUNTER_ITERATIONS_R)
#define LC1_0_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_0_VAL),DMA_LC1_0_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_0_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC1_0_LOOP_COUNTER_ITERATIONS_W)
#define LC1_0_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_0_VAL),DMA_LC1_0_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_0_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC1_0_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_0_VAL),DMA_LC1_0_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_0_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC1_0_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_0_VAL),DMA_LC1_0_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_0_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: SAR1 ACCESS: RO */

#if defined(_V1) && !defined(SAR1_OFFSET)
#define SAR1_OFFSET 0x420
#endif

#if !defined(DMA_SAR1_OFFSET)
#define DMA_SAR1_OFFSET 0x420
#endif

#if defined(_V1) && !defined(SAR1_REG)
#define SAR1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR1_OFFSET))
#endif

#if defined(_V1) && !defined(SAR1_VAL)
#define SAR1_VAL  PREFIX_VAL(SAR1_REG)
#endif

#define DMA0_SAR1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR1_OFFSET))
#define DMA0_SAR1_VAL  PREFIX_VAL(DMA0_SAR1_REG)

/* FIELDS: */

/* src_addr ACCESS: RO */

#ifndef DMA_SAR1_SRC_ADDR_POS
#define DMA_SAR1_SRC_ADDR_POS      0
#endif

#ifndef DMA_SAR1_SRC_ADDR_LEN
#define DMA_SAR1_SRC_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(SAR1_SRC_ADDR_R)
#define SAR1_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR1_VAL),DMA_SAR1_SRC_ADDR_POS, DMA_SAR1_SRC_ADDR_LEN)
#endif

#if defined(_V1) && !defined(SAR1_SRC_ADDR_W)
#define SAR1_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR1_VAL),DMA_SAR1_SRC_ADDR_POS, DMA_SAR1_SRC_ADDR_LEN,value)
#endif

#define DMA0_SAR1_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR1_VAL),DMA_SAR1_SRC_ADDR_POS, DMA_SAR1_SRC_ADDR_LEN)

#define DMA0_SAR1_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR1_VAL),DMA_SAR1_SRC_ADDR_POS, DMA_SAR1_SRC_ADDR_LEN,value)


/* REGISTER: DAR1 ACCESS: RO */

#if defined(_V1) && !defined(DAR1_OFFSET)
#define DAR1_OFFSET 0x424
#endif

#if !defined(DMA_DAR1_OFFSET)
#define DMA_DAR1_OFFSET 0x424
#endif

#if defined(_V1) && !defined(DAR1_REG)
#define DAR1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR1_OFFSET))
#endif

#if defined(_V1) && !defined(DAR1_VAL)
#define DAR1_VAL  PREFIX_VAL(DAR1_REG)
#endif

#define DMA0_DAR1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR1_OFFSET))
#define DMA0_DAR1_VAL  PREFIX_VAL(DMA0_DAR1_REG)

/* FIELDS: */

/* dst_addr ACCESS: RO */

#ifndef DMA_DAR1_DST_ADDR_POS
#define DMA_DAR1_DST_ADDR_POS      0
#endif

#ifndef DMA_DAR1_DST_ADDR_LEN
#define DMA_DAR1_DST_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(DAR1_DST_ADDR_R)
#define DAR1_DST_ADDR_R        GetGroupBits32( (DMA0_DAR1_VAL),DMA_DAR1_DST_ADDR_POS, DMA_DAR1_DST_ADDR_LEN)
#endif

#if defined(_V1) && !defined(DAR1_DST_ADDR_W)
#define DAR1_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR1_VAL),DMA_DAR1_DST_ADDR_POS, DMA_DAR1_DST_ADDR_LEN,value)
#endif

#define DMA0_DAR1_DST_ADDR_R        GetGroupBits32( (DMA0_DAR1_VAL),DMA_DAR1_DST_ADDR_POS, DMA_DAR1_DST_ADDR_LEN)

#define DMA0_DAR1_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR1_VAL),DMA_DAR1_DST_ADDR_POS, DMA_DAR1_DST_ADDR_LEN,value)


/* REGISTER: CCR1 ACCESS: RO */

#if defined(_V1) && !defined(CCR1_OFFSET)
#define CCR1_OFFSET 0x428
#endif

#if !defined(DMA_CCR1_OFFSET)
#define DMA_CCR1_OFFSET 0x428
#endif

#if defined(_V1) && !defined(CCR1_REG)
#define CCR1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR1_OFFSET))
#endif

#if defined(_V1) && !defined(CCR1_VAL)
#define CCR1_VAL  PREFIX_VAL(CCR1_REG)
#endif

#define DMA0_CCR1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR1_OFFSET))
#define DMA0_CCR1_VAL  PREFIX_VAL(DMA0_CCR1_REG)

/* FIELDS: */

/* src_inc ACCESS: RO */

#ifndef DMA_CCR1_SRC_INC_POS
#define DMA_CCR1_SRC_INC_POS      0
#endif

#ifndef DMA_CCR1_SRC_INC_LEN
#define DMA_CCR1_SRC_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR1_SRC_INC_R)
#define CCR1_SRC_INC_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_INC_POS, DMA_CCR1_SRC_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR1_SRC_INC_W)
#define CCR1_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_INC_POS, DMA_CCR1_SRC_INC_LEN,value)
#endif

#define DMA0_CCR1_SRC_INC_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_INC_POS, DMA_CCR1_SRC_INC_LEN)

#define DMA0_CCR1_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_INC_POS, DMA_CCR1_SRC_INC_LEN,value)


/* src_burst_size ACCESS: RO */

#ifndef DMA_CCR1_SRC_BURST_SIZE_POS
#define DMA_CCR1_SRC_BURST_SIZE_POS      1
#endif

#ifndef DMA_CCR1_SRC_BURST_SIZE_LEN
#define DMA_CCR1_SRC_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR1_SRC_BURST_SIZE_R)
#define CCR1_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_BURST_SIZE_POS, DMA_CCR1_SRC_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR1_SRC_BURST_SIZE_W)
#define CCR1_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_BURST_SIZE_POS, DMA_CCR1_SRC_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR1_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_BURST_SIZE_POS, DMA_CCR1_SRC_BURST_SIZE_LEN)

#define DMA0_CCR1_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_BURST_SIZE_POS, DMA_CCR1_SRC_BURST_SIZE_LEN,value)


/* src_burst_len ACCESS: RO */

#ifndef DMA_CCR1_SRC_BURST_LEN_POS
#define DMA_CCR1_SRC_BURST_LEN_POS      4
#endif

#ifndef DMA_CCR1_SRC_BURST_LEN_LEN
#define DMA_CCR1_SRC_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR1_SRC_BURST_LEN_R)
#define CCR1_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_BURST_LEN_POS, DMA_CCR1_SRC_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR1_SRC_BURST_LEN_W)
#define CCR1_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_BURST_LEN_POS, DMA_CCR1_SRC_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR1_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_BURST_LEN_POS, DMA_CCR1_SRC_BURST_LEN_LEN)

#define DMA0_CCR1_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_BURST_LEN_POS, DMA_CCR1_SRC_BURST_LEN_LEN,value)


/* src_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR1_SRC_PROT_CTRL_POS
#define DMA_CCR1_SRC_PROT_CTRL_POS      8
#endif

#ifndef DMA_CCR1_SRC_PROT_CTRL_LEN
#define DMA_CCR1_SRC_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR1_SRC_PROT_CTRL_R)
#define CCR1_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_PROT_CTRL_POS, DMA_CCR1_SRC_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR1_SRC_PROT_CTRL_W)
#define CCR1_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_PROT_CTRL_POS, DMA_CCR1_SRC_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR1_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_PROT_CTRL_POS, DMA_CCR1_SRC_PROT_CTRL_LEN)

#define DMA0_CCR1_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_PROT_CTRL_POS, DMA_CCR1_SRC_PROT_CTRL_LEN,value)


/* src_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR1_SRC_CACHE_CTRL_POS
#define DMA_CCR1_SRC_CACHE_CTRL_POS      11
#endif

#ifndef DMA_CCR1_SRC_CACHE_CTRL_LEN
#define DMA_CCR1_SRC_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR1_SRC_CACHE_CTRL_R)
#define CCR1_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_CACHE_CTRL_POS, DMA_CCR1_SRC_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR1_SRC_CACHE_CTRL_W)
#define CCR1_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_CACHE_CTRL_POS, DMA_CCR1_SRC_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR1_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_CACHE_CTRL_POS, DMA_CCR1_SRC_CACHE_CTRL_LEN)

#define DMA0_CCR1_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_SRC_CACHE_CTRL_POS, DMA_CCR1_SRC_CACHE_CTRL_LEN,value)


/* dst_inc ACCESS: RO */

#ifndef DMA_CCR1_DST_INC_POS
#define DMA_CCR1_DST_INC_POS      14
#endif

#ifndef DMA_CCR1_DST_INC_LEN
#define DMA_CCR1_DST_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR1_DST_INC_R)
#define CCR1_DST_INC_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_INC_POS, DMA_CCR1_DST_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR1_DST_INC_W)
#define CCR1_DST_INC_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_INC_POS, DMA_CCR1_DST_INC_LEN,value)
#endif

#define DMA0_CCR1_DST_INC_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_INC_POS, DMA_CCR1_DST_INC_LEN)

#define DMA0_CCR1_DST_INC_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_INC_POS, DMA_CCR1_DST_INC_LEN,value)


/* dst_burst_size ACCESS: RO */

#ifndef DMA_CCR1_DST_BURST_SIZE_POS
#define DMA_CCR1_DST_BURST_SIZE_POS      15
#endif

#ifndef DMA_CCR1_DST_BURST_SIZE_LEN
#define DMA_CCR1_DST_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR1_DST_BURST_SIZE_R)
#define CCR1_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_BURST_SIZE_POS, DMA_CCR1_DST_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR1_DST_BURST_SIZE_W)
#define CCR1_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_BURST_SIZE_POS, DMA_CCR1_DST_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR1_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_BURST_SIZE_POS, DMA_CCR1_DST_BURST_SIZE_LEN)

#define DMA0_CCR1_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_BURST_SIZE_POS, DMA_CCR1_DST_BURST_SIZE_LEN,value)


/* dst_burst_len ACCESS: RO */

#ifndef DMA_CCR1_DST_BURST_LEN_POS
#define DMA_CCR1_DST_BURST_LEN_POS      18
#endif

#ifndef DMA_CCR1_DST_BURST_LEN_LEN
#define DMA_CCR1_DST_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR1_DST_BURST_LEN_R)
#define CCR1_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_BURST_LEN_POS, DMA_CCR1_DST_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR1_DST_BURST_LEN_W)
#define CCR1_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_BURST_LEN_POS, DMA_CCR1_DST_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR1_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_BURST_LEN_POS, DMA_CCR1_DST_BURST_LEN_LEN)

#define DMA0_CCR1_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_BURST_LEN_POS, DMA_CCR1_DST_BURST_LEN_LEN,value)


/* dst_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR1_DST_PROT_CTRL_POS
#define DMA_CCR1_DST_PROT_CTRL_POS      22
#endif

#ifndef DMA_CCR1_DST_PROT_CTRL_LEN
#define DMA_CCR1_DST_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR1_DST_PROT_CTRL_R)
#define CCR1_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_PROT_CTRL_POS, DMA_CCR1_DST_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR1_DST_PROT_CTRL_W)
#define CCR1_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_PROT_CTRL_POS, DMA_CCR1_DST_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR1_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_PROT_CTRL_POS, DMA_CCR1_DST_PROT_CTRL_LEN)

#define DMA0_CCR1_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_PROT_CTRL_POS, DMA_CCR1_DST_PROT_CTRL_LEN,value)


/* dst_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR1_DST_CACHE_CTRL_POS
#define DMA_CCR1_DST_CACHE_CTRL_POS      25
#endif

#ifndef DMA_CCR1_DST_CACHE_CTRL_LEN
#define DMA_CCR1_DST_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR1_DST_CACHE_CTRL_R)
#define CCR1_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_CACHE_CTRL_POS, DMA_CCR1_DST_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR1_DST_CACHE_CTRL_W)
#define CCR1_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_CACHE_CTRL_POS, DMA_CCR1_DST_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR1_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_CACHE_CTRL_POS, DMA_CCR1_DST_CACHE_CTRL_LEN)

#define DMA0_CCR1_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_DST_CACHE_CTRL_POS, DMA_CCR1_DST_CACHE_CTRL_LEN,value)


/* endian_swap_size ACCESS: RO */

#ifndef DMA_CCR1_ENDIAN_SWAP_SIZE_POS
#define DMA_CCR1_ENDIAN_SWAP_SIZE_POS      28
#endif

#ifndef DMA_CCR1_ENDIAN_SWAP_SIZE_LEN
#define DMA_CCR1_ENDIAN_SWAP_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR1_ENDIAN_SWAP_SIZE_R)
#define CCR1_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_ENDIAN_SWAP_SIZE_POS, DMA_CCR1_ENDIAN_SWAP_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR1_ENDIAN_SWAP_SIZE_W)
#define CCR1_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_ENDIAN_SWAP_SIZE_POS, DMA_CCR1_ENDIAN_SWAP_SIZE_LEN,value)
#endif

#define DMA0_CCR1_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_ENDIAN_SWAP_SIZE_POS, DMA_CCR1_ENDIAN_SWAP_SIZE_LEN)

#define DMA0_CCR1_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR1_VAL),DMA_CCR1_ENDIAN_SWAP_SIZE_POS, DMA_CCR1_ENDIAN_SWAP_SIZE_LEN,value)


/* REGISTER: LC0_1 ACCESS: RO */

#if defined(_V1) && !defined(LC0_1_OFFSET)
#define LC0_1_OFFSET 0x42C
#endif

#if !defined(DMA_LC0_1_OFFSET)
#define DMA_LC0_1_OFFSET 0x42C
#endif

#if defined(_V1) && !defined(LC0_1_REG)
#define LC0_1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_1_OFFSET))
#endif

#if defined(_V1) && !defined(LC0_1_VAL)
#define LC0_1_VAL  PREFIX_VAL(LC0_1_REG)
#endif

#define DMA0_LC0_1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_1_OFFSET))
#define DMA0_LC0_1_VAL  PREFIX_VAL(DMA0_LC0_1_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC0_1_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC0_1_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC0_1_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC0_1_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC0_1_LOOP_COUNTER_ITERATIONS_R)
#define LC0_1_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_1_VAL),DMA_LC0_1_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_1_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC0_1_LOOP_COUNTER_ITERATIONS_W)
#define LC0_1_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_1_VAL),DMA_LC0_1_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_1_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC0_1_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_1_VAL),DMA_LC0_1_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_1_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC0_1_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_1_VAL),DMA_LC0_1_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_1_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: LC1_1 ACCESS: RO */

#if defined(_V1) && !defined(LC1_1_OFFSET)
#define LC1_1_OFFSET 0x430
#endif

#if !defined(DMA_LC1_1_OFFSET)
#define DMA_LC1_1_OFFSET 0x430
#endif

#if defined(_V1) && !defined(LC1_1_REG)
#define LC1_1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_1_OFFSET))
#endif

#if defined(_V1) && !defined(LC1_1_VAL)
#define LC1_1_VAL  PREFIX_VAL(LC1_1_REG)
#endif

#define DMA0_LC1_1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_1_OFFSET))
#define DMA0_LC1_1_VAL  PREFIX_VAL(DMA0_LC1_1_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC1_1_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC1_1_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC1_1_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC1_1_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC1_1_LOOP_COUNTER_ITERATIONS_R)
#define LC1_1_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_1_VAL),DMA_LC1_1_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_1_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC1_1_LOOP_COUNTER_ITERATIONS_W)
#define LC1_1_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_1_VAL),DMA_LC1_1_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_1_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC1_1_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_1_VAL),DMA_LC1_1_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_1_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC1_1_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_1_VAL),DMA_LC1_1_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_1_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: FTR1 ACCESS: RO */

#if defined(_V1) && !defined(FTR1_OFFSET)
#define FTR1_OFFSET 0x44
#endif

#if !defined(DMA_FTR1_OFFSET)
#define DMA_FTR1_OFFSET 0x44
#endif

#if defined(_V1) && !defined(FTR1_REG)
#define FTR1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR1_OFFSET))
#endif

#if defined(_V1) && !defined(FTR1_VAL)
#define FTR1_VAL  PREFIX_VAL(FTR1_REG)
#endif

#define DMA0_FTR1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR1_OFFSET))
#define DMA0_FTR1_VAL  PREFIX_VAL(DMA0_FTR1_REG)

/* FIELDS: */

/* undef_instr ACCESS: RO */

#ifndef DMA_FTR1_UNDEF_INSTR_POS
#define DMA_FTR1_UNDEF_INSTR_POS      0
#endif

#ifndef DMA_FTR1_UNDEF_INSTR_LEN
#define DMA_FTR1_UNDEF_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR1_UNDEF_INSTR_R)
#define FTR1_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_UNDEF_INSTR_POS, DMA_FTR1_UNDEF_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR1_UNDEF_INSTR_W)
#define FTR1_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_UNDEF_INSTR_POS, DMA_FTR1_UNDEF_INSTR_LEN,value)
#endif

#define DMA0_FTR1_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_UNDEF_INSTR_POS, DMA_FTR1_UNDEF_INSTR_LEN)

#define DMA0_FTR1_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_UNDEF_INSTR_POS, DMA_FTR1_UNDEF_INSTR_LEN,value)


/* operand_invalid ACCESS: RO */

#ifndef DMA_FTR1_OPERAND_INVALID_POS
#define DMA_FTR1_OPERAND_INVALID_POS      1
#endif

#ifndef DMA_FTR1_OPERAND_INVALID_LEN
#define DMA_FTR1_OPERAND_INVALID_LEN      1
#endif

#if defined(_V1) && !defined(FTR1_OPERAND_INVALID_R)
#define FTR1_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_OPERAND_INVALID_POS, DMA_FTR1_OPERAND_INVALID_LEN)
#endif

#if defined(_V1) && !defined(FTR1_OPERAND_INVALID_W)
#define FTR1_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_OPERAND_INVALID_POS, DMA_FTR1_OPERAND_INVALID_LEN,value)
#endif

#define DMA0_FTR1_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_OPERAND_INVALID_POS, DMA_FTR1_OPERAND_INVALID_LEN)

#define DMA0_FTR1_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_OPERAND_INVALID_POS, DMA_FTR1_OPERAND_INVALID_LEN,value)


/* ch_evnt_err ACCESS: RO */

#ifndef DMA_FTR1_CH_EVNT_ERR_POS
#define DMA_FTR1_CH_EVNT_ERR_POS      5
#endif

#ifndef DMA_FTR1_CH_EVNT_ERR_LEN
#define DMA_FTR1_CH_EVNT_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR1_CH_EVNT_ERR_R)
#define FTR1_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_CH_EVNT_ERR_POS, DMA_FTR1_CH_EVNT_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR1_CH_EVNT_ERR_W)
#define FTR1_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_CH_EVNT_ERR_POS, DMA_FTR1_CH_EVNT_ERR_LEN,value)
#endif

#define DMA0_FTR1_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_CH_EVNT_ERR_POS, DMA_FTR1_CH_EVNT_ERR_LEN)

#define DMA0_FTR1_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_CH_EVNT_ERR_POS, DMA_FTR1_CH_EVNT_ERR_LEN,value)


/* ch_periph_err ACCESS: RO */

#ifndef DMA_FTR1_CH_PERIPH_ERR_POS
#define DMA_FTR1_CH_PERIPH_ERR_POS      6
#endif

#ifndef DMA_FTR1_CH_PERIPH_ERR_LEN
#define DMA_FTR1_CH_PERIPH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR1_CH_PERIPH_ERR_R)
#define FTR1_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_CH_PERIPH_ERR_POS, DMA_FTR1_CH_PERIPH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR1_CH_PERIPH_ERR_W)
#define FTR1_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_CH_PERIPH_ERR_POS, DMA_FTR1_CH_PERIPH_ERR_LEN,value)
#endif

#define DMA0_FTR1_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_CH_PERIPH_ERR_POS, DMA_FTR1_CH_PERIPH_ERR_LEN)

#define DMA0_FTR1_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_CH_PERIPH_ERR_POS, DMA_FTR1_CH_PERIPH_ERR_LEN,value)


/* ch_rdwr_err ACCESS: RO */

#ifndef DMA_FTR1_CH_RDWR_ERR_POS
#define DMA_FTR1_CH_RDWR_ERR_POS      7
#endif

#ifndef DMA_FTR1_CH_RDWR_ERR_LEN
#define DMA_FTR1_CH_RDWR_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR1_CH_RDWR_ERR_R)
#define FTR1_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_CH_RDWR_ERR_POS, DMA_FTR1_CH_RDWR_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR1_CH_RDWR_ERR_W)
#define FTR1_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_CH_RDWR_ERR_POS, DMA_FTR1_CH_RDWR_ERR_LEN,value)
#endif

#define DMA0_FTR1_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_CH_RDWR_ERR_POS, DMA_FTR1_CH_RDWR_ERR_LEN)

#define DMA0_FTR1_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_CH_RDWR_ERR_POS, DMA_FTR1_CH_RDWR_ERR_LEN,value)


/* mfifo_err ACCESS: RO */

#ifndef DMA_FTR1_MFIFO_ERR_POS
#define DMA_FTR1_MFIFO_ERR_POS      12
#endif

#ifndef DMA_FTR1_MFIFO_ERR_LEN
#define DMA_FTR1_MFIFO_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR1_MFIFO_ERR_R)
#define FTR1_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_MFIFO_ERR_POS, DMA_FTR1_MFIFO_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR1_MFIFO_ERR_W)
#define FTR1_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_MFIFO_ERR_POS, DMA_FTR1_MFIFO_ERR_LEN,value)
#endif

#define DMA0_FTR1_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_MFIFO_ERR_POS, DMA_FTR1_MFIFO_ERR_LEN)

#define DMA0_FTR1_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_MFIFO_ERR_POS, DMA_FTR1_MFIFO_ERR_LEN,value)


/* st_data_unavailable ACCESS: RO */

#ifndef DMA_FTR1_ST_DATA_UNAVAILABLE_POS
#define DMA_FTR1_ST_DATA_UNAVAILABLE_POS      13
#endif

#ifndef DMA_FTR1_ST_DATA_UNAVAILABLE_LEN
#define DMA_FTR1_ST_DATA_UNAVAILABLE_LEN      1
#endif

#if defined(_V1) && !defined(FTR1_ST_DATA_UNAVAILABLE_R)
#define FTR1_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_ST_DATA_UNAVAILABLE_POS, DMA_FTR1_ST_DATA_UNAVAILABLE_LEN)
#endif

#if defined(_V1) && !defined(FTR1_ST_DATA_UNAVAILABLE_W)
#define FTR1_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_ST_DATA_UNAVAILABLE_POS, DMA_FTR1_ST_DATA_UNAVAILABLE_LEN,value)
#endif

#define DMA0_FTR1_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_ST_DATA_UNAVAILABLE_POS, DMA_FTR1_ST_DATA_UNAVAILABLE_LEN)

#define DMA0_FTR1_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_ST_DATA_UNAVAILABLE_POS, DMA_FTR1_ST_DATA_UNAVAILABLE_LEN,value)


/* instr_fetch_err ACCESS: RO */

#ifndef DMA_FTR1_INSTR_FETCH_ERR_POS
#define DMA_FTR1_INSTR_FETCH_ERR_POS      16
#endif

#ifndef DMA_FTR1_INSTR_FETCH_ERR_LEN
#define DMA_FTR1_INSTR_FETCH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR1_INSTR_FETCH_ERR_R)
#define FTR1_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_INSTR_FETCH_ERR_POS, DMA_FTR1_INSTR_FETCH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR1_INSTR_FETCH_ERR_W)
#define FTR1_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_INSTR_FETCH_ERR_POS, DMA_FTR1_INSTR_FETCH_ERR_LEN,value)
#endif

#define DMA0_FTR1_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_INSTR_FETCH_ERR_POS, DMA_FTR1_INSTR_FETCH_ERR_LEN)

#define DMA0_FTR1_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_INSTR_FETCH_ERR_POS, DMA_FTR1_INSTR_FETCH_ERR_LEN,value)


/* data_write_err ACCESS: RO */

#ifndef DMA_FTR1_DATA_WRITE_ERR_POS
#define DMA_FTR1_DATA_WRITE_ERR_POS      17
#endif

#ifndef DMA_FTR1_DATA_WRITE_ERR_LEN
#define DMA_FTR1_DATA_WRITE_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR1_DATA_WRITE_ERR_R)
#define FTR1_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_DATA_WRITE_ERR_POS, DMA_FTR1_DATA_WRITE_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR1_DATA_WRITE_ERR_W)
#define FTR1_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_DATA_WRITE_ERR_POS, DMA_FTR1_DATA_WRITE_ERR_LEN,value)
#endif

#define DMA0_FTR1_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_DATA_WRITE_ERR_POS, DMA_FTR1_DATA_WRITE_ERR_LEN)

#define DMA0_FTR1_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_DATA_WRITE_ERR_POS, DMA_FTR1_DATA_WRITE_ERR_LEN,value)


/* data_read_err ACCESS: RO */

#ifndef DMA_FTR1_DATA_READ_ERR_POS
#define DMA_FTR1_DATA_READ_ERR_POS      18
#endif

#ifndef DMA_FTR1_DATA_READ_ERR_LEN
#define DMA_FTR1_DATA_READ_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR1_DATA_READ_ERR_R)
#define FTR1_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_DATA_READ_ERR_POS, DMA_FTR1_DATA_READ_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR1_DATA_READ_ERR_W)
#define FTR1_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_DATA_READ_ERR_POS, DMA_FTR1_DATA_READ_ERR_LEN,value)
#endif

#define DMA0_FTR1_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_DATA_READ_ERR_POS, DMA_FTR1_DATA_READ_ERR_LEN)

#define DMA0_FTR1_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_DATA_READ_ERR_POS, DMA_FTR1_DATA_READ_ERR_LEN,value)


/* dbg_instr ACCESS: RO */

#ifndef DMA_FTR1_DBG_INSTR_POS
#define DMA_FTR1_DBG_INSTR_POS      30
#endif

#ifndef DMA_FTR1_DBG_INSTR_LEN
#define DMA_FTR1_DBG_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR1_DBG_INSTR_R)
#define FTR1_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_DBG_INSTR_POS, DMA_FTR1_DBG_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR1_DBG_INSTR_W)
#define FTR1_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_DBG_INSTR_POS, DMA_FTR1_DBG_INSTR_LEN,value)
#endif

#define DMA0_FTR1_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_DBG_INSTR_POS, DMA_FTR1_DBG_INSTR_LEN)

#define DMA0_FTR1_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_DBG_INSTR_POS, DMA_FTR1_DBG_INSTR_LEN,value)


/* lockup_err ACCESS: RO */

#ifndef DMA_FTR1_LOCKUP_ERR_POS
#define DMA_FTR1_LOCKUP_ERR_POS      31
#endif

#ifndef DMA_FTR1_LOCKUP_ERR_LEN
#define DMA_FTR1_LOCKUP_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR1_LOCKUP_ERR_R)
#define FTR1_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_LOCKUP_ERR_POS, DMA_FTR1_LOCKUP_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR1_LOCKUP_ERR_W)
#define FTR1_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_LOCKUP_ERR_POS, DMA_FTR1_LOCKUP_ERR_LEN,value)
#endif

#define DMA0_FTR1_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_LOCKUP_ERR_POS, DMA_FTR1_LOCKUP_ERR_LEN)

#define DMA0_FTR1_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR1_VAL),DMA_FTR1_LOCKUP_ERR_POS, DMA_FTR1_LOCKUP_ERR_LEN,value)


/* REGISTER: SAR2 ACCESS: RO */

#if defined(_V1) && !defined(SAR2_OFFSET)
#define SAR2_OFFSET 0x440
#endif

#if !defined(DMA_SAR2_OFFSET)
#define DMA_SAR2_OFFSET 0x440
#endif

#if defined(_V1) && !defined(SAR2_REG)
#define SAR2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR2_OFFSET))
#endif

#if defined(_V1) && !defined(SAR2_VAL)
#define SAR2_VAL  PREFIX_VAL(SAR2_REG)
#endif

#define DMA0_SAR2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR2_OFFSET))
#define DMA0_SAR2_VAL  PREFIX_VAL(DMA0_SAR2_REG)

/* FIELDS: */

/* src_addr ACCESS: RO */

#ifndef DMA_SAR2_SRC_ADDR_POS
#define DMA_SAR2_SRC_ADDR_POS      0
#endif

#ifndef DMA_SAR2_SRC_ADDR_LEN
#define DMA_SAR2_SRC_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(SAR2_SRC_ADDR_R)
#define SAR2_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR2_VAL),DMA_SAR2_SRC_ADDR_POS, DMA_SAR2_SRC_ADDR_LEN)
#endif

#if defined(_V1) && !defined(SAR2_SRC_ADDR_W)
#define SAR2_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR2_VAL),DMA_SAR2_SRC_ADDR_POS, DMA_SAR2_SRC_ADDR_LEN,value)
#endif

#define DMA0_SAR2_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR2_VAL),DMA_SAR2_SRC_ADDR_POS, DMA_SAR2_SRC_ADDR_LEN)

#define DMA0_SAR2_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR2_VAL),DMA_SAR2_SRC_ADDR_POS, DMA_SAR2_SRC_ADDR_LEN,value)


/* REGISTER: DAR2 ACCESS: RO */

#if defined(_V1) && !defined(DAR2_OFFSET)
#define DAR2_OFFSET 0x444
#endif

#if !defined(DMA_DAR2_OFFSET)
#define DMA_DAR2_OFFSET 0x444
#endif

#if defined(_V1) && !defined(DAR2_REG)
#define DAR2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR2_OFFSET))
#endif

#if defined(_V1) && !defined(DAR2_VAL)
#define DAR2_VAL  PREFIX_VAL(DAR2_REG)
#endif

#define DMA0_DAR2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR2_OFFSET))
#define DMA0_DAR2_VAL  PREFIX_VAL(DMA0_DAR2_REG)

/* FIELDS: */

/* dst_addr ACCESS: RO */

#ifndef DMA_DAR2_DST_ADDR_POS
#define DMA_DAR2_DST_ADDR_POS      0
#endif

#ifndef DMA_DAR2_DST_ADDR_LEN
#define DMA_DAR2_DST_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(DAR2_DST_ADDR_R)
#define DAR2_DST_ADDR_R        GetGroupBits32( (DMA0_DAR2_VAL),DMA_DAR2_DST_ADDR_POS, DMA_DAR2_DST_ADDR_LEN)
#endif

#if defined(_V1) && !defined(DAR2_DST_ADDR_W)
#define DAR2_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR2_VAL),DMA_DAR2_DST_ADDR_POS, DMA_DAR2_DST_ADDR_LEN,value)
#endif

#define DMA0_DAR2_DST_ADDR_R        GetGroupBits32( (DMA0_DAR2_VAL),DMA_DAR2_DST_ADDR_POS, DMA_DAR2_DST_ADDR_LEN)

#define DMA0_DAR2_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR2_VAL),DMA_DAR2_DST_ADDR_POS, DMA_DAR2_DST_ADDR_LEN,value)


/* REGISTER: CCR2 ACCESS: RO */

#if defined(_V1) && !defined(CCR2_OFFSET)
#define CCR2_OFFSET 0x448
#endif

#if !defined(DMA_CCR2_OFFSET)
#define DMA_CCR2_OFFSET 0x448
#endif

#if defined(_V1) && !defined(CCR2_REG)
#define CCR2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR2_OFFSET))
#endif

#if defined(_V1) && !defined(CCR2_VAL)
#define CCR2_VAL  PREFIX_VAL(CCR2_REG)
#endif

#define DMA0_CCR2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR2_OFFSET))
#define DMA0_CCR2_VAL  PREFIX_VAL(DMA0_CCR2_REG)

/* FIELDS: */

/* src_inc ACCESS: RO */

#ifndef DMA_CCR2_SRC_INC_POS
#define DMA_CCR2_SRC_INC_POS      0
#endif

#ifndef DMA_CCR2_SRC_INC_LEN
#define DMA_CCR2_SRC_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR2_SRC_INC_R)
#define CCR2_SRC_INC_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_INC_POS, DMA_CCR2_SRC_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR2_SRC_INC_W)
#define CCR2_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_INC_POS, DMA_CCR2_SRC_INC_LEN,value)
#endif

#define DMA0_CCR2_SRC_INC_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_INC_POS, DMA_CCR2_SRC_INC_LEN)

#define DMA0_CCR2_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_INC_POS, DMA_CCR2_SRC_INC_LEN,value)


/* src_burst_size ACCESS: RO */

#ifndef DMA_CCR2_SRC_BURST_SIZE_POS
#define DMA_CCR2_SRC_BURST_SIZE_POS      1
#endif

#ifndef DMA_CCR2_SRC_BURST_SIZE_LEN
#define DMA_CCR2_SRC_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR2_SRC_BURST_SIZE_R)
#define CCR2_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_BURST_SIZE_POS, DMA_CCR2_SRC_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR2_SRC_BURST_SIZE_W)
#define CCR2_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_BURST_SIZE_POS, DMA_CCR2_SRC_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR2_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_BURST_SIZE_POS, DMA_CCR2_SRC_BURST_SIZE_LEN)

#define DMA0_CCR2_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_BURST_SIZE_POS, DMA_CCR2_SRC_BURST_SIZE_LEN,value)


/* src_burst_len ACCESS: RO */

#ifndef DMA_CCR2_SRC_BURST_LEN_POS
#define DMA_CCR2_SRC_BURST_LEN_POS      4
#endif

#ifndef DMA_CCR2_SRC_BURST_LEN_LEN
#define DMA_CCR2_SRC_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR2_SRC_BURST_LEN_R)
#define CCR2_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_BURST_LEN_POS, DMA_CCR2_SRC_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR2_SRC_BURST_LEN_W)
#define CCR2_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_BURST_LEN_POS, DMA_CCR2_SRC_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR2_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_BURST_LEN_POS, DMA_CCR2_SRC_BURST_LEN_LEN)

#define DMA0_CCR2_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_BURST_LEN_POS, DMA_CCR2_SRC_BURST_LEN_LEN,value)


/* src_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR2_SRC_PROT_CTRL_POS
#define DMA_CCR2_SRC_PROT_CTRL_POS      8
#endif

#ifndef DMA_CCR2_SRC_PROT_CTRL_LEN
#define DMA_CCR2_SRC_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR2_SRC_PROT_CTRL_R)
#define CCR2_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_PROT_CTRL_POS, DMA_CCR2_SRC_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR2_SRC_PROT_CTRL_W)
#define CCR2_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_PROT_CTRL_POS, DMA_CCR2_SRC_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR2_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_PROT_CTRL_POS, DMA_CCR2_SRC_PROT_CTRL_LEN)

#define DMA0_CCR2_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_PROT_CTRL_POS, DMA_CCR2_SRC_PROT_CTRL_LEN,value)


/* src_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR2_SRC_CACHE_CTRL_POS
#define DMA_CCR2_SRC_CACHE_CTRL_POS      11
#endif

#ifndef DMA_CCR2_SRC_CACHE_CTRL_LEN
#define DMA_CCR2_SRC_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR2_SRC_CACHE_CTRL_R)
#define CCR2_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_CACHE_CTRL_POS, DMA_CCR2_SRC_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR2_SRC_CACHE_CTRL_W)
#define CCR2_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_CACHE_CTRL_POS, DMA_CCR2_SRC_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR2_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_CACHE_CTRL_POS, DMA_CCR2_SRC_CACHE_CTRL_LEN)

#define DMA0_CCR2_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_SRC_CACHE_CTRL_POS, DMA_CCR2_SRC_CACHE_CTRL_LEN,value)


/* dst_inc ACCESS: RO */

#ifndef DMA_CCR2_DST_INC_POS
#define DMA_CCR2_DST_INC_POS      14
#endif

#ifndef DMA_CCR2_DST_INC_LEN
#define DMA_CCR2_DST_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR2_DST_INC_R)
#define CCR2_DST_INC_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_INC_POS, DMA_CCR2_DST_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR2_DST_INC_W)
#define CCR2_DST_INC_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_INC_POS, DMA_CCR2_DST_INC_LEN,value)
#endif

#define DMA0_CCR2_DST_INC_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_INC_POS, DMA_CCR2_DST_INC_LEN)

#define DMA0_CCR2_DST_INC_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_INC_POS, DMA_CCR2_DST_INC_LEN,value)


/* dst_burst_size ACCESS: RO */

#ifndef DMA_CCR2_DST_BURST_SIZE_POS
#define DMA_CCR2_DST_BURST_SIZE_POS      15
#endif

#ifndef DMA_CCR2_DST_BURST_SIZE_LEN
#define DMA_CCR2_DST_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR2_DST_BURST_SIZE_R)
#define CCR2_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_BURST_SIZE_POS, DMA_CCR2_DST_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR2_DST_BURST_SIZE_W)
#define CCR2_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_BURST_SIZE_POS, DMA_CCR2_DST_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR2_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_BURST_SIZE_POS, DMA_CCR2_DST_BURST_SIZE_LEN)

#define DMA0_CCR2_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_BURST_SIZE_POS, DMA_CCR2_DST_BURST_SIZE_LEN,value)


/* dst_burst_len ACCESS: RO */

#ifndef DMA_CCR2_DST_BURST_LEN_POS
#define DMA_CCR2_DST_BURST_LEN_POS      18
#endif

#ifndef DMA_CCR2_DST_BURST_LEN_LEN
#define DMA_CCR2_DST_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR2_DST_BURST_LEN_R)
#define CCR2_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_BURST_LEN_POS, DMA_CCR2_DST_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR2_DST_BURST_LEN_W)
#define CCR2_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_BURST_LEN_POS, DMA_CCR2_DST_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR2_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_BURST_LEN_POS, DMA_CCR2_DST_BURST_LEN_LEN)

#define DMA0_CCR2_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_BURST_LEN_POS, DMA_CCR2_DST_BURST_LEN_LEN,value)


/* dst_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR2_DST_PROT_CTRL_POS
#define DMA_CCR2_DST_PROT_CTRL_POS      22
#endif

#ifndef DMA_CCR2_DST_PROT_CTRL_LEN
#define DMA_CCR2_DST_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR2_DST_PROT_CTRL_R)
#define CCR2_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_PROT_CTRL_POS, DMA_CCR2_DST_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR2_DST_PROT_CTRL_W)
#define CCR2_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_PROT_CTRL_POS, DMA_CCR2_DST_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR2_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_PROT_CTRL_POS, DMA_CCR2_DST_PROT_CTRL_LEN)

#define DMA0_CCR2_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_PROT_CTRL_POS, DMA_CCR2_DST_PROT_CTRL_LEN,value)


/* dst_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR2_DST_CACHE_CTRL_POS
#define DMA_CCR2_DST_CACHE_CTRL_POS      25
#endif

#ifndef DMA_CCR2_DST_CACHE_CTRL_LEN
#define DMA_CCR2_DST_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR2_DST_CACHE_CTRL_R)
#define CCR2_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_CACHE_CTRL_POS, DMA_CCR2_DST_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR2_DST_CACHE_CTRL_W)
#define CCR2_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_CACHE_CTRL_POS, DMA_CCR2_DST_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR2_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_CACHE_CTRL_POS, DMA_CCR2_DST_CACHE_CTRL_LEN)

#define DMA0_CCR2_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_DST_CACHE_CTRL_POS, DMA_CCR2_DST_CACHE_CTRL_LEN,value)


/* endian_swap_size ACCESS: RO */

#ifndef DMA_CCR2_ENDIAN_SWAP_SIZE_POS
#define DMA_CCR2_ENDIAN_SWAP_SIZE_POS      28
#endif

#ifndef DMA_CCR2_ENDIAN_SWAP_SIZE_LEN
#define DMA_CCR2_ENDIAN_SWAP_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR2_ENDIAN_SWAP_SIZE_R)
#define CCR2_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_ENDIAN_SWAP_SIZE_POS, DMA_CCR2_ENDIAN_SWAP_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR2_ENDIAN_SWAP_SIZE_W)
#define CCR2_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_ENDIAN_SWAP_SIZE_POS, DMA_CCR2_ENDIAN_SWAP_SIZE_LEN,value)
#endif

#define DMA0_CCR2_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_ENDIAN_SWAP_SIZE_POS, DMA_CCR2_ENDIAN_SWAP_SIZE_LEN)

#define DMA0_CCR2_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR2_VAL),DMA_CCR2_ENDIAN_SWAP_SIZE_POS, DMA_CCR2_ENDIAN_SWAP_SIZE_LEN,value)


/* REGISTER: LC0_2 ACCESS: RO */

#if defined(_V1) && !defined(LC0_2_OFFSET)
#define LC0_2_OFFSET 0x44C
#endif

#if !defined(DMA_LC0_2_OFFSET)
#define DMA_LC0_2_OFFSET 0x44C
#endif

#if defined(_V1) && !defined(LC0_2_REG)
#define LC0_2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_2_OFFSET))
#endif

#if defined(_V1) && !defined(LC0_2_VAL)
#define LC0_2_VAL  PREFIX_VAL(LC0_2_REG)
#endif

#define DMA0_LC0_2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_2_OFFSET))
#define DMA0_LC0_2_VAL  PREFIX_VAL(DMA0_LC0_2_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC0_2_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC0_2_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC0_2_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC0_2_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC0_2_LOOP_COUNTER_ITERATIONS_R)
#define LC0_2_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_2_VAL),DMA_LC0_2_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_2_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC0_2_LOOP_COUNTER_ITERATIONS_W)
#define LC0_2_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_2_VAL),DMA_LC0_2_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_2_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC0_2_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_2_VAL),DMA_LC0_2_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_2_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC0_2_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_2_VAL),DMA_LC0_2_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_2_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: LC1_2 ACCESS: RO */

#if defined(_V1) && !defined(LC1_2_OFFSET)
#define LC1_2_OFFSET 0x450
#endif

#if !defined(DMA_LC1_2_OFFSET)
#define DMA_LC1_2_OFFSET 0x450
#endif

#if defined(_V1) && !defined(LC1_2_REG)
#define LC1_2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_2_OFFSET))
#endif

#if defined(_V1) && !defined(LC1_2_VAL)
#define LC1_2_VAL  PREFIX_VAL(LC1_2_REG)
#endif

#define DMA0_LC1_2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_2_OFFSET))
#define DMA0_LC1_2_VAL  PREFIX_VAL(DMA0_LC1_2_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC1_2_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC1_2_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC1_2_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC1_2_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC1_2_LOOP_COUNTER_ITERATIONS_R)
#define LC1_2_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_2_VAL),DMA_LC1_2_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_2_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC1_2_LOOP_COUNTER_ITERATIONS_W)
#define LC1_2_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_2_VAL),DMA_LC1_2_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_2_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC1_2_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_2_VAL),DMA_LC1_2_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_2_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC1_2_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_2_VAL),DMA_LC1_2_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_2_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: SAR3 ACCESS: RO */

#if defined(_V1) && !defined(SAR3_OFFSET)
#define SAR3_OFFSET 0x460
#endif

#if !defined(DMA_SAR3_OFFSET)
#define DMA_SAR3_OFFSET 0x460
#endif

#if defined(_V1) && !defined(SAR3_REG)
#define SAR3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR3_OFFSET))
#endif

#if defined(_V1) && !defined(SAR3_VAL)
#define SAR3_VAL  PREFIX_VAL(SAR3_REG)
#endif

#define DMA0_SAR3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR3_OFFSET))
#define DMA0_SAR3_VAL  PREFIX_VAL(DMA0_SAR3_REG)

/* FIELDS: */

/* src_addr ACCESS: RO */

#ifndef DMA_SAR3_SRC_ADDR_POS
#define DMA_SAR3_SRC_ADDR_POS      0
#endif

#ifndef DMA_SAR3_SRC_ADDR_LEN
#define DMA_SAR3_SRC_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(SAR3_SRC_ADDR_R)
#define SAR3_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR3_VAL),DMA_SAR3_SRC_ADDR_POS, DMA_SAR3_SRC_ADDR_LEN)
#endif

#if defined(_V1) && !defined(SAR3_SRC_ADDR_W)
#define SAR3_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR3_VAL),DMA_SAR3_SRC_ADDR_POS, DMA_SAR3_SRC_ADDR_LEN,value)
#endif

#define DMA0_SAR3_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR3_VAL),DMA_SAR3_SRC_ADDR_POS, DMA_SAR3_SRC_ADDR_LEN)

#define DMA0_SAR3_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR3_VAL),DMA_SAR3_SRC_ADDR_POS, DMA_SAR3_SRC_ADDR_LEN,value)


/* REGISTER: DAR3 ACCESS: RO */

#if defined(_V1) && !defined(DAR3_OFFSET)
#define DAR3_OFFSET 0x464
#endif

#if !defined(DMA_DAR3_OFFSET)
#define DMA_DAR3_OFFSET 0x464
#endif

#if defined(_V1) && !defined(DAR3_REG)
#define DAR3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR3_OFFSET))
#endif

#if defined(_V1) && !defined(DAR3_VAL)
#define DAR3_VAL  PREFIX_VAL(DAR3_REG)
#endif

#define DMA0_DAR3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR3_OFFSET))
#define DMA0_DAR3_VAL  PREFIX_VAL(DMA0_DAR3_REG)

/* FIELDS: */

/* dst_addr ACCESS: RO */

#ifndef DMA_DAR3_DST_ADDR_POS
#define DMA_DAR3_DST_ADDR_POS      0
#endif

#ifndef DMA_DAR3_DST_ADDR_LEN
#define DMA_DAR3_DST_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(DAR3_DST_ADDR_R)
#define DAR3_DST_ADDR_R        GetGroupBits32( (DMA0_DAR3_VAL),DMA_DAR3_DST_ADDR_POS, DMA_DAR3_DST_ADDR_LEN)
#endif

#if defined(_V1) && !defined(DAR3_DST_ADDR_W)
#define DAR3_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR3_VAL),DMA_DAR3_DST_ADDR_POS, DMA_DAR3_DST_ADDR_LEN,value)
#endif

#define DMA0_DAR3_DST_ADDR_R        GetGroupBits32( (DMA0_DAR3_VAL),DMA_DAR3_DST_ADDR_POS, DMA_DAR3_DST_ADDR_LEN)

#define DMA0_DAR3_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR3_VAL),DMA_DAR3_DST_ADDR_POS, DMA_DAR3_DST_ADDR_LEN,value)


/* REGISTER: CCR3 ACCESS: RO */

#if defined(_V1) && !defined(CCR3_OFFSET)
#define CCR3_OFFSET 0x468
#endif

#if !defined(DMA_CCR3_OFFSET)
#define DMA_CCR3_OFFSET 0x468
#endif

#if defined(_V1) && !defined(CCR3_REG)
#define CCR3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR3_OFFSET))
#endif

#if defined(_V1) && !defined(CCR3_VAL)
#define CCR3_VAL  PREFIX_VAL(CCR3_REG)
#endif

#define DMA0_CCR3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR3_OFFSET))
#define DMA0_CCR3_VAL  PREFIX_VAL(DMA0_CCR3_REG)

/* FIELDS: */

/* src_inc ACCESS: RO */

#ifndef DMA_CCR3_SRC_INC_POS
#define DMA_CCR3_SRC_INC_POS      0
#endif

#ifndef DMA_CCR3_SRC_INC_LEN
#define DMA_CCR3_SRC_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR3_SRC_INC_R)
#define CCR3_SRC_INC_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_INC_POS, DMA_CCR3_SRC_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR3_SRC_INC_W)
#define CCR3_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_INC_POS, DMA_CCR3_SRC_INC_LEN,value)
#endif

#define DMA0_CCR3_SRC_INC_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_INC_POS, DMA_CCR3_SRC_INC_LEN)

#define DMA0_CCR3_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_INC_POS, DMA_CCR3_SRC_INC_LEN,value)


/* src_burst_size ACCESS: RO */

#ifndef DMA_CCR3_SRC_BURST_SIZE_POS
#define DMA_CCR3_SRC_BURST_SIZE_POS      1
#endif

#ifndef DMA_CCR3_SRC_BURST_SIZE_LEN
#define DMA_CCR3_SRC_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR3_SRC_BURST_SIZE_R)
#define CCR3_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_BURST_SIZE_POS, DMA_CCR3_SRC_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR3_SRC_BURST_SIZE_W)
#define CCR3_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_BURST_SIZE_POS, DMA_CCR3_SRC_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR3_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_BURST_SIZE_POS, DMA_CCR3_SRC_BURST_SIZE_LEN)

#define DMA0_CCR3_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_BURST_SIZE_POS, DMA_CCR3_SRC_BURST_SIZE_LEN,value)


/* src_burst_len ACCESS: RO */

#ifndef DMA_CCR3_SRC_BURST_LEN_POS
#define DMA_CCR3_SRC_BURST_LEN_POS      4
#endif

#ifndef DMA_CCR3_SRC_BURST_LEN_LEN
#define DMA_CCR3_SRC_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR3_SRC_BURST_LEN_R)
#define CCR3_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_BURST_LEN_POS, DMA_CCR3_SRC_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR3_SRC_BURST_LEN_W)
#define CCR3_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_BURST_LEN_POS, DMA_CCR3_SRC_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR3_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_BURST_LEN_POS, DMA_CCR3_SRC_BURST_LEN_LEN)

#define DMA0_CCR3_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_BURST_LEN_POS, DMA_CCR3_SRC_BURST_LEN_LEN,value)


/* src_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR3_SRC_PROT_CTRL_POS
#define DMA_CCR3_SRC_PROT_CTRL_POS      8
#endif

#ifndef DMA_CCR3_SRC_PROT_CTRL_LEN
#define DMA_CCR3_SRC_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR3_SRC_PROT_CTRL_R)
#define CCR3_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_PROT_CTRL_POS, DMA_CCR3_SRC_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR3_SRC_PROT_CTRL_W)
#define CCR3_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_PROT_CTRL_POS, DMA_CCR3_SRC_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR3_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_PROT_CTRL_POS, DMA_CCR3_SRC_PROT_CTRL_LEN)

#define DMA0_CCR3_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_PROT_CTRL_POS, DMA_CCR3_SRC_PROT_CTRL_LEN,value)


/* src_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR3_SRC_CACHE_CTRL_POS
#define DMA_CCR3_SRC_CACHE_CTRL_POS      11
#endif

#ifndef DMA_CCR3_SRC_CACHE_CTRL_LEN
#define DMA_CCR3_SRC_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR3_SRC_CACHE_CTRL_R)
#define CCR3_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_CACHE_CTRL_POS, DMA_CCR3_SRC_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR3_SRC_CACHE_CTRL_W)
#define CCR3_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_CACHE_CTRL_POS, DMA_CCR3_SRC_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR3_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_CACHE_CTRL_POS, DMA_CCR3_SRC_CACHE_CTRL_LEN)

#define DMA0_CCR3_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_SRC_CACHE_CTRL_POS, DMA_CCR3_SRC_CACHE_CTRL_LEN,value)


/* dst_inc ACCESS: RO */

#ifndef DMA_CCR3_DST_INC_POS
#define DMA_CCR3_DST_INC_POS      14
#endif

#ifndef DMA_CCR3_DST_INC_LEN
#define DMA_CCR3_DST_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR3_DST_INC_R)
#define CCR3_DST_INC_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_INC_POS, DMA_CCR3_DST_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR3_DST_INC_W)
#define CCR3_DST_INC_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_INC_POS, DMA_CCR3_DST_INC_LEN,value)
#endif

#define DMA0_CCR3_DST_INC_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_INC_POS, DMA_CCR3_DST_INC_LEN)

#define DMA0_CCR3_DST_INC_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_INC_POS, DMA_CCR3_DST_INC_LEN,value)


/* dst_burst_size ACCESS: RO */

#ifndef DMA_CCR3_DST_BURST_SIZE_POS
#define DMA_CCR3_DST_BURST_SIZE_POS      15
#endif

#ifndef DMA_CCR3_DST_BURST_SIZE_LEN
#define DMA_CCR3_DST_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR3_DST_BURST_SIZE_R)
#define CCR3_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_BURST_SIZE_POS, DMA_CCR3_DST_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR3_DST_BURST_SIZE_W)
#define CCR3_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_BURST_SIZE_POS, DMA_CCR3_DST_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR3_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_BURST_SIZE_POS, DMA_CCR3_DST_BURST_SIZE_LEN)

#define DMA0_CCR3_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_BURST_SIZE_POS, DMA_CCR3_DST_BURST_SIZE_LEN,value)


/* dst_burst_len ACCESS: RO */

#ifndef DMA_CCR3_DST_BURST_LEN_POS
#define DMA_CCR3_DST_BURST_LEN_POS      18
#endif

#ifndef DMA_CCR3_DST_BURST_LEN_LEN
#define DMA_CCR3_DST_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR3_DST_BURST_LEN_R)
#define CCR3_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_BURST_LEN_POS, DMA_CCR3_DST_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR3_DST_BURST_LEN_W)
#define CCR3_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_BURST_LEN_POS, DMA_CCR3_DST_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR3_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_BURST_LEN_POS, DMA_CCR3_DST_BURST_LEN_LEN)

#define DMA0_CCR3_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_BURST_LEN_POS, DMA_CCR3_DST_BURST_LEN_LEN,value)


/* dst_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR3_DST_PROT_CTRL_POS
#define DMA_CCR3_DST_PROT_CTRL_POS      22
#endif

#ifndef DMA_CCR3_DST_PROT_CTRL_LEN
#define DMA_CCR3_DST_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR3_DST_PROT_CTRL_R)
#define CCR3_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_PROT_CTRL_POS, DMA_CCR3_DST_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR3_DST_PROT_CTRL_W)
#define CCR3_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_PROT_CTRL_POS, DMA_CCR3_DST_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR3_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_PROT_CTRL_POS, DMA_CCR3_DST_PROT_CTRL_LEN)

#define DMA0_CCR3_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_PROT_CTRL_POS, DMA_CCR3_DST_PROT_CTRL_LEN,value)


/* dst_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR3_DST_CACHE_CTRL_POS
#define DMA_CCR3_DST_CACHE_CTRL_POS      25
#endif

#ifndef DMA_CCR3_DST_CACHE_CTRL_LEN
#define DMA_CCR3_DST_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR3_DST_CACHE_CTRL_R)
#define CCR3_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_CACHE_CTRL_POS, DMA_CCR3_DST_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR3_DST_CACHE_CTRL_W)
#define CCR3_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_CACHE_CTRL_POS, DMA_CCR3_DST_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR3_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_CACHE_CTRL_POS, DMA_CCR3_DST_CACHE_CTRL_LEN)

#define DMA0_CCR3_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_DST_CACHE_CTRL_POS, DMA_CCR3_DST_CACHE_CTRL_LEN,value)


/* endian_swap_size ACCESS: RO */

#ifndef DMA_CCR3_ENDIAN_SWAP_SIZE_POS
#define DMA_CCR3_ENDIAN_SWAP_SIZE_POS      28
#endif

#ifndef DMA_CCR3_ENDIAN_SWAP_SIZE_LEN
#define DMA_CCR3_ENDIAN_SWAP_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR3_ENDIAN_SWAP_SIZE_R)
#define CCR3_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_ENDIAN_SWAP_SIZE_POS, DMA_CCR3_ENDIAN_SWAP_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR3_ENDIAN_SWAP_SIZE_W)
#define CCR3_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_ENDIAN_SWAP_SIZE_POS, DMA_CCR3_ENDIAN_SWAP_SIZE_LEN,value)
#endif

#define DMA0_CCR3_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_ENDIAN_SWAP_SIZE_POS, DMA_CCR3_ENDIAN_SWAP_SIZE_LEN)

#define DMA0_CCR3_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR3_VAL),DMA_CCR3_ENDIAN_SWAP_SIZE_POS, DMA_CCR3_ENDIAN_SWAP_SIZE_LEN,value)


/* REGISTER: LC0_3 ACCESS: RO */

#if defined(_V1) && !defined(LC0_3_OFFSET)
#define LC0_3_OFFSET 0x46C
#endif

#if !defined(DMA_LC0_3_OFFSET)
#define DMA_LC0_3_OFFSET 0x46C
#endif

#if defined(_V1) && !defined(LC0_3_REG)
#define LC0_3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_3_OFFSET))
#endif

#if defined(_V1) && !defined(LC0_3_VAL)
#define LC0_3_VAL  PREFIX_VAL(LC0_3_REG)
#endif

#define DMA0_LC0_3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_3_OFFSET))
#define DMA0_LC0_3_VAL  PREFIX_VAL(DMA0_LC0_3_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC0_3_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC0_3_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC0_3_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC0_3_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC0_3_LOOP_COUNTER_ITERATIONS_R)
#define LC0_3_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_3_VAL),DMA_LC0_3_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_3_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC0_3_LOOP_COUNTER_ITERATIONS_W)
#define LC0_3_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_3_VAL),DMA_LC0_3_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_3_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC0_3_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_3_VAL),DMA_LC0_3_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_3_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC0_3_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_3_VAL),DMA_LC0_3_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_3_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: LC1_3 ACCESS: RO */

#if defined(_V1) && !defined(LC1_3_OFFSET)
#define LC1_3_OFFSET 0x470
#endif

#if !defined(DMA_LC1_3_OFFSET)
#define DMA_LC1_3_OFFSET 0x470
#endif

#if defined(_V1) && !defined(LC1_3_REG)
#define LC1_3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_3_OFFSET))
#endif

#if defined(_V1) && !defined(LC1_3_VAL)
#define LC1_3_VAL  PREFIX_VAL(LC1_3_REG)
#endif

#define DMA0_LC1_3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_3_OFFSET))
#define DMA0_LC1_3_VAL  PREFIX_VAL(DMA0_LC1_3_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC1_3_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC1_3_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC1_3_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC1_3_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC1_3_LOOP_COUNTER_ITERATIONS_R)
#define LC1_3_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_3_VAL),DMA_LC1_3_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_3_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC1_3_LOOP_COUNTER_ITERATIONS_W)
#define LC1_3_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_3_VAL),DMA_LC1_3_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_3_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC1_3_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_3_VAL),DMA_LC1_3_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_3_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC1_3_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_3_VAL),DMA_LC1_3_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_3_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: FTR2 ACCESS: RO */

#if defined(_V1) && !defined(FTR2_OFFSET)
#define FTR2_OFFSET 0x48
#endif

#if !defined(DMA_FTR2_OFFSET)
#define DMA_FTR2_OFFSET 0x48
#endif

#if defined(_V1) && !defined(FTR2_REG)
#define FTR2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR2_OFFSET))
#endif

#if defined(_V1) && !defined(FTR2_VAL)
#define FTR2_VAL  PREFIX_VAL(FTR2_REG)
#endif

#define DMA0_FTR2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR2_OFFSET))
#define DMA0_FTR2_VAL  PREFIX_VAL(DMA0_FTR2_REG)

/* FIELDS: */

/* undef_instr ACCESS: RO */

#ifndef DMA_FTR2_UNDEF_INSTR_POS
#define DMA_FTR2_UNDEF_INSTR_POS      0
#endif

#ifndef DMA_FTR2_UNDEF_INSTR_LEN
#define DMA_FTR2_UNDEF_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR2_UNDEF_INSTR_R)
#define FTR2_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_UNDEF_INSTR_POS, DMA_FTR2_UNDEF_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR2_UNDEF_INSTR_W)
#define FTR2_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_UNDEF_INSTR_POS, DMA_FTR2_UNDEF_INSTR_LEN,value)
#endif

#define DMA0_FTR2_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_UNDEF_INSTR_POS, DMA_FTR2_UNDEF_INSTR_LEN)

#define DMA0_FTR2_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_UNDEF_INSTR_POS, DMA_FTR2_UNDEF_INSTR_LEN,value)


/* operand_invalid ACCESS: RO */

#ifndef DMA_FTR2_OPERAND_INVALID_POS
#define DMA_FTR2_OPERAND_INVALID_POS      1
#endif

#ifndef DMA_FTR2_OPERAND_INVALID_LEN
#define DMA_FTR2_OPERAND_INVALID_LEN      1
#endif

#if defined(_V1) && !defined(FTR2_OPERAND_INVALID_R)
#define FTR2_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_OPERAND_INVALID_POS, DMA_FTR2_OPERAND_INVALID_LEN)
#endif

#if defined(_V1) && !defined(FTR2_OPERAND_INVALID_W)
#define FTR2_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_OPERAND_INVALID_POS, DMA_FTR2_OPERAND_INVALID_LEN,value)
#endif

#define DMA0_FTR2_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_OPERAND_INVALID_POS, DMA_FTR2_OPERAND_INVALID_LEN)

#define DMA0_FTR2_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_OPERAND_INVALID_POS, DMA_FTR2_OPERAND_INVALID_LEN,value)


/* ch_evnt_err ACCESS: RO */

#ifndef DMA_FTR2_CH_EVNT_ERR_POS
#define DMA_FTR2_CH_EVNT_ERR_POS      5
#endif

#ifndef DMA_FTR2_CH_EVNT_ERR_LEN
#define DMA_FTR2_CH_EVNT_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR2_CH_EVNT_ERR_R)
#define FTR2_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_CH_EVNT_ERR_POS, DMA_FTR2_CH_EVNT_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR2_CH_EVNT_ERR_W)
#define FTR2_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_CH_EVNT_ERR_POS, DMA_FTR2_CH_EVNT_ERR_LEN,value)
#endif

#define DMA0_FTR2_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_CH_EVNT_ERR_POS, DMA_FTR2_CH_EVNT_ERR_LEN)

#define DMA0_FTR2_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_CH_EVNT_ERR_POS, DMA_FTR2_CH_EVNT_ERR_LEN,value)


/* ch_periph_err ACCESS: RO */

#ifndef DMA_FTR2_CH_PERIPH_ERR_POS
#define DMA_FTR2_CH_PERIPH_ERR_POS      6
#endif

#ifndef DMA_FTR2_CH_PERIPH_ERR_LEN
#define DMA_FTR2_CH_PERIPH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR2_CH_PERIPH_ERR_R)
#define FTR2_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_CH_PERIPH_ERR_POS, DMA_FTR2_CH_PERIPH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR2_CH_PERIPH_ERR_W)
#define FTR2_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_CH_PERIPH_ERR_POS, DMA_FTR2_CH_PERIPH_ERR_LEN,value)
#endif

#define DMA0_FTR2_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_CH_PERIPH_ERR_POS, DMA_FTR2_CH_PERIPH_ERR_LEN)

#define DMA0_FTR2_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_CH_PERIPH_ERR_POS, DMA_FTR2_CH_PERIPH_ERR_LEN,value)


/* ch_rdwr_err ACCESS: RO */

#ifndef DMA_FTR2_CH_RDWR_ERR_POS
#define DMA_FTR2_CH_RDWR_ERR_POS      7
#endif

#ifndef DMA_FTR2_CH_RDWR_ERR_LEN
#define DMA_FTR2_CH_RDWR_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR2_CH_RDWR_ERR_R)
#define FTR2_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_CH_RDWR_ERR_POS, DMA_FTR2_CH_RDWR_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR2_CH_RDWR_ERR_W)
#define FTR2_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_CH_RDWR_ERR_POS, DMA_FTR2_CH_RDWR_ERR_LEN,value)
#endif

#define DMA0_FTR2_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_CH_RDWR_ERR_POS, DMA_FTR2_CH_RDWR_ERR_LEN)

#define DMA0_FTR2_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_CH_RDWR_ERR_POS, DMA_FTR2_CH_RDWR_ERR_LEN,value)


/* mfifo_err ACCESS: RO */

#ifndef DMA_FTR2_MFIFO_ERR_POS
#define DMA_FTR2_MFIFO_ERR_POS      12
#endif

#ifndef DMA_FTR2_MFIFO_ERR_LEN
#define DMA_FTR2_MFIFO_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR2_MFIFO_ERR_R)
#define FTR2_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_MFIFO_ERR_POS, DMA_FTR2_MFIFO_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR2_MFIFO_ERR_W)
#define FTR2_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_MFIFO_ERR_POS, DMA_FTR2_MFIFO_ERR_LEN,value)
#endif

#define DMA0_FTR2_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_MFIFO_ERR_POS, DMA_FTR2_MFIFO_ERR_LEN)

#define DMA0_FTR2_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_MFIFO_ERR_POS, DMA_FTR2_MFIFO_ERR_LEN,value)


/* st_data_unavailable ACCESS: RO */

#ifndef DMA_FTR2_ST_DATA_UNAVAILABLE_POS
#define DMA_FTR2_ST_DATA_UNAVAILABLE_POS      13
#endif

#ifndef DMA_FTR2_ST_DATA_UNAVAILABLE_LEN
#define DMA_FTR2_ST_DATA_UNAVAILABLE_LEN      1
#endif

#if defined(_V1) && !defined(FTR2_ST_DATA_UNAVAILABLE_R)
#define FTR2_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_ST_DATA_UNAVAILABLE_POS, DMA_FTR2_ST_DATA_UNAVAILABLE_LEN)
#endif

#if defined(_V1) && !defined(FTR2_ST_DATA_UNAVAILABLE_W)
#define FTR2_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_ST_DATA_UNAVAILABLE_POS, DMA_FTR2_ST_DATA_UNAVAILABLE_LEN,value)
#endif

#define DMA0_FTR2_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_ST_DATA_UNAVAILABLE_POS, DMA_FTR2_ST_DATA_UNAVAILABLE_LEN)

#define DMA0_FTR2_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_ST_DATA_UNAVAILABLE_POS, DMA_FTR2_ST_DATA_UNAVAILABLE_LEN,value)


/* instr_fetch_err ACCESS: RO */

#ifndef DMA_FTR2_INSTR_FETCH_ERR_POS
#define DMA_FTR2_INSTR_FETCH_ERR_POS      16
#endif

#ifndef DMA_FTR2_INSTR_FETCH_ERR_LEN
#define DMA_FTR2_INSTR_FETCH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR2_INSTR_FETCH_ERR_R)
#define FTR2_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_INSTR_FETCH_ERR_POS, DMA_FTR2_INSTR_FETCH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR2_INSTR_FETCH_ERR_W)
#define FTR2_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_INSTR_FETCH_ERR_POS, DMA_FTR2_INSTR_FETCH_ERR_LEN,value)
#endif

#define DMA0_FTR2_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_INSTR_FETCH_ERR_POS, DMA_FTR2_INSTR_FETCH_ERR_LEN)

#define DMA0_FTR2_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_INSTR_FETCH_ERR_POS, DMA_FTR2_INSTR_FETCH_ERR_LEN,value)


/* data_write_err ACCESS: RO */

#ifndef DMA_FTR2_DATA_WRITE_ERR_POS
#define DMA_FTR2_DATA_WRITE_ERR_POS      17
#endif

#ifndef DMA_FTR2_DATA_WRITE_ERR_LEN
#define DMA_FTR2_DATA_WRITE_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR2_DATA_WRITE_ERR_R)
#define FTR2_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_DATA_WRITE_ERR_POS, DMA_FTR2_DATA_WRITE_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR2_DATA_WRITE_ERR_W)
#define FTR2_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_DATA_WRITE_ERR_POS, DMA_FTR2_DATA_WRITE_ERR_LEN,value)
#endif

#define DMA0_FTR2_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_DATA_WRITE_ERR_POS, DMA_FTR2_DATA_WRITE_ERR_LEN)

#define DMA0_FTR2_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_DATA_WRITE_ERR_POS, DMA_FTR2_DATA_WRITE_ERR_LEN,value)


/* data_read_err ACCESS: RO */

#ifndef DMA_FTR2_DATA_READ_ERR_POS
#define DMA_FTR2_DATA_READ_ERR_POS      18
#endif

#ifndef DMA_FTR2_DATA_READ_ERR_LEN
#define DMA_FTR2_DATA_READ_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR2_DATA_READ_ERR_R)
#define FTR2_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_DATA_READ_ERR_POS, DMA_FTR2_DATA_READ_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR2_DATA_READ_ERR_W)
#define FTR2_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_DATA_READ_ERR_POS, DMA_FTR2_DATA_READ_ERR_LEN,value)
#endif

#define DMA0_FTR2_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_DATA_READ_ERR_POS, DMA_FTR2_DATA_READ_ERR_LEN)

#define DMA0_FTR2_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_DATA_READ_ERR_POS, DMA_FTR2_DATA_READ_ERR_LEN,value)


/* dbg_instr ACCESS: RO */

#ifndef DMA_FTR2_DBG_INSTR_POS
#define DMA_FTR2_DBG_INSTR_POS      30
#endif

#ifndef DMA_FTR2_DBG_INSTR_LEN
#define DMA_FTR2_DBG_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR2_DBG_INSTR_R)
#define FTR2_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_DBG_INSTR_POS, DMA_FTR2_DBG_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR2_DBG_INSTR_W)
#define FTR2_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_DBG_INSTR_POS, DMA_FTR2_DBG_INSTR_LEN,value)
#endif

#define DMA0_FTR2_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_DBG_INSTR_POS, DMA_FTR2_DBG_INSTR_LEN)

#define DMA0_FTR2_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_DBG_INSTR_POS, DMA_FTR2_DBG_INSTR_LEN,value)


/* lockup_err ACCESS: RO */

#ifndef DMA_FTR2_LOCKUP_ERR_POS
#define DMA_FTR2_LOCKUP_ERR_POS      31
#endif

#ifndef DMA_FTR2_LOCKUP_ERR_LEN
#define DMA_FTR2_LOCKUP_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR2_LOCKUP_ERR_R)
#define FTR2_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_LOCKUP_ERR_POS, DMA_FTR2_LOCKUP_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR2_LOCKUP_ERR_W)
#define FTR2_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_LOCKUP_ERR_POS, DMA_FTR2_LOCKUP_ERR_LEN,value)
#endif

#define DMA0_FTR2_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_LOCKUP_ERR_POS, DMA_FTR2_LOCKUP_ERR_LEN)

#define DMA0_FTR2_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR2_VAL),DMA_FTR2_LOCKUP_ERR_POS, DMA_FTR2_LOCKUP_ERR_LEN,value)


/* REGISTER: SAR4 ACCESS: RO */

#if defined(_V1) && !defined(SAR4_OFFSET)
#define SAR4_OFFSET 0x480
#endif

#if !defined(DMA_SAR4_OFFSET)
#define DMA_SAR4_OFFSET 0x480
#endif

#if defined(_V1) && !defined(SAR4_REG)
#define SAR4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR4_OFFSET))
#endif

#if defined(_V1) && !defined(SAR4_VAL)
#define SAR4_VAL  PREFIX_VAL(SAR4_REG)
#endif

#define DMA0_SAR4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR4_OFFSET))
#define DMA0_SAR4_VAL  PREFIX_VAL(DMA0_SAR4_REG)

/* FIELDS: */

/* src_addr ACCESS: RO */

#ifndef DMA_SAR4_SRC_ADDR_POS
#define DMA_SAR4_SRC_ADDR_POS      0
#endif

#ifndef DMA_SAR4_SRC_ADDR_LEN
#define DMA_SAR4_SRC_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(SAR4_SRC_ADDR_R)
#define SAR4_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR4_VAL),DMA_SAR4_SRC_ADDR_POS, DMA_SAR4_SRC_ADDR_LEN)
#endif

#if defined(_V1) && !defined(SAR4_SRC_ADDR_W)
#define SAR4_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR4_VAL),DMA_SAR4_SRC_ADDR_POS, DMA_SAR4_SRC_ADDR_LEN,value)
#endif

#define DMA0_SAR4_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR4_VAL),DMA_SAR4_SRC_ADDR_POS, DMA_SAR4_SRC_ADDR_LEN)

#define DMA0_SAR4_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR4_VAL),DMA_SAR4_SRC_ADDR_POS, DMA_SAR4_SRC_ADDR_LEN,value)


/* REGISTER: DAR4 ACCESS: RO */

#if defined(_V1) && !defined(DAR4_OFFSET)
#define DAR4_OFFSET 0x484
#endif

#if !defined(DMA_DAR4_OFFSET)
#define DMA_DAR4_OFFSET 0x484
#endif

#if defined(_V1) && !defined(DAR4_REG)
#define DAR4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR4_OFFSET))
#endif

#if defined(_V1) && !defined(DAR4_VAL)
#define DAR4_VAL  PREFIX_VAL(DAR4_REG)
#endif

#define DMA0_DAR4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR4_OFFSET))
#define DMA0_DAR4_VAL  PREFIX_VAL(DMA0_DAR4_REG)

/* FIELDS: */

/* dst_addr ACCESS: RO */

#ifndef DMA_DAR4_DST_ADDR_POS
#define DMA_DAR4_DST_ADDR_POS      0
#endif

#ifndef DMA_DAR4_DST_ADDR_LEN
#define DMA_DAR4_DST_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(DAR4_DST_ADDR_R)
#define DAR4_DST_ADDR_R        GetGroupBits32( (DMA0_DAR4_VAL),DMA_DAR4_DST_ADDR_POS, DMA_DAR4_DST_ADDR_LEN)
#endif

#if defined(_V1) && !defined(DAR4_DST_ADDR_W)
#define DAR4_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR4_VAL),DMA_DAR4_DST_ADDR_POS, DMA_DAR4_DST_ADDR_LEN,value)
#endif

#define DMA0_DAR4_DST_ADDR_R        GetGroupBits32( (DMA0_DAR4_VAL),DMA_DAR4_DST_ADDR_POS, DMA_DAR4_DST_ADDR_LEN)

#define DMA0_DAR4_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR4_VAL),DMA_DAR4_DST_ADDR_POS, DMA_DAR4_DST_ADDR_LEN,value)


/* REGISTER: CCR4 ACCESS: RO */

#if defined(_V1) && !defined(CCR4_OFFSET)
#define CCR4_OFFSET 0x488
#endif

#if !defined(DMA_CCR4_OFFSET)
#define DMA_CCR4_OFFSET 0x488
#endif

#if defined(_V1) && !defined(CCR4_REG)
#define CCR4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR4_OFFSET))
#endif

#if defined(_V1) && !defined(CCR4_VAL)
#define CCR4_VAL  PREFIX_VAL(CCR4_REG)
#endif

#define DMA0_CCR4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR4_OFFSET))
#define DMA0_CCR4_VAL  PREFIX_VAL(DMA0_CCR4_REG)

/* FIELDS: */

/* src_inc ACCESS: RO */

#ifndef DMA_CCR4_SRC_INC_POS
#define DMA_CCR4_SRC_INC_POS      0
#endif

#ifndef DMA_CCR4_SRC_INC_LEN
#define DMA_CCR4_SRC_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR4_SRC_INC_R)
#define CCR4_SRC_INC_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_INC_POS, DMA_CCR4_SRC_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR4_SRC_INC_W)
#define CCR4_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_INC_POS, DMA_CCR4_SRC_INC_LEN,value)
#endif

#define DMA0_CCR4_SRC_INC_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_INC_POS, DMA_CCR4_SRC_INC_LEN)

#define DMA0_CCR4_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_INC_POS, DMA_CCR4_SRC_INC_LEN,value)


/* src_burst_size ACCESS: RO */

#ifndef DMA_CCR4_SRC_BURST_SIZE_POS
#define DMA_CCR4_SRC_BURST_SIZE_POS      1
#endif

#ifndef DMA_CCR4_SRC_BURST_SIZE_LEN
#define DMA_CCR4_SRC_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR4_SRC_BURST_SIZE_R)
#define CCR4_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_BURST_SIZE_POS, DMA_CCR4_SRC_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR4_SRC_BURST_SIZE_W)
#define CCR4_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_BURST_SIZE_POS, DMA_CCR4_SRC_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR4_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_BURST_SIZE_POS, DMA_CCR4_SRC_BURST_SIZE_LEN)

#define DMA0_CCR4_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_BURST_SIZE_POS, DMA_CCR4_SRC_BURST_SIZE_LEN,value)


/* src_burst_len ACCESS: RO */

#ifndef DMA_CCR4_SRC_BURST_LEN_POS
#define DMA_CCR4_SRC_BURST_LEN_POS      4
#endif

#ifndef DMA_CCR4_SRC_BURST_LEN_LEN
#define DMA_CCR4_SRC_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR4_SRC_BURST_LEN_R)
#define CCR4_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_BURST_LEN_POS, DMA_CCR4_SRC_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR4_SRC_BURST_LEN_W)
#define CCR4_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_BURST_LEN_POS, DMA_CCR4_SRC_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR4_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_BURST_LEN_POS, DMA_CCR4_SRC_BURST_LEN_LEN)

#define DMA0_CCR4_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_BURST_LEN_POS, DMA_CCR4_SRC_BURST_LEN_LEN,value)


/* src_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR4_SRC_PROT_CTRL_POS
#define DMA_CCR4_SRC_PROT_CTRL_POS      8
#endif

#ifndef DMA_CCR4_SRC_PROT_CTRL_LEN
#define DMA_CCR4_SRC_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR4_SRC_PROT_CTRL_R)
#define CCR4_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_PROT_CTRL_POS, DMA_CCR4_SRC_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR4_SRC_PROT_CTRL_W)
#define CCR4_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_PROT_CTRL_POS, DMA_CCR4_SRC_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR4_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_PROT_CTRL_POS, DMA_CCR4_SRC_PROT_CTRL_LEN)

#define DMA0_CCR4_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_PROT_CTRL_POS, DMA_CCR4_SRC_PROT_CTRL_LEN,value)


/* src_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR4_SRC_CACHE_CTRL_POS
#define DMA_CCR4_SRC_CACHE_CTRL_POS      11
#endif

#ifndef DMA_CCR4_SRC_CACHE_CTRL_LEN
#define DMA_CCR4_SRC_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR4_SRC_CACHE_CTRL_R)
#define CCR4_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_CACHE_CTRL_POS, DMA_CCR4_SRC_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR4_SRC_CACHE_CTRL_W)
#define CCR4_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_CACHE_CTRL_POS, DMA_CCR4_SRC_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR4_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_CACHE_CTRL_POS, DMA_CCR4_SRC_CACHE_CTRL_LEN)

#define DMA0_CCR4_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_SRC_CACHE_CTRL_POS, DMA_CCR4_SRC_CACHE_CTRL_LEN,value)


/* dst_inc ACCESS: RO */

#ifndef DMA_CCR4_DST_INC_POS
#define DMA_CCR4_DST_INC_POS      14
#endif

#ifndef DMA_CCR4_DST_INC_LEN
#define DMA_CCR4_DST_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR4_DST_INC_R)
#define CCR4_DST_INC_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_INC_POS, DMA_CCR4_DST_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR4_DST_INC_W)
#define CCR4_DST_INC_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_INC_POS, DMA_CCR4_DST_INC_LEN,value)
#endif

#define DMA0_CCR4_DST_INC_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_INC_POS, DMA_CCR4_DST_INC_LEN)

#define DMA0_CCR4_DST_INC_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_INC_POS, DMA_CCR4_DST_INC_LEN,value)


/* dst_burst_size ACCESS: RO */

#ifndef DMA_CCR4_DST_BURST_SIZE_POS
#define DMA_CCR4_DST_BURST_SIZE_POS      15
#endif

#ifndef DMA_CCR4_DST_BURST_SIZE_LEN
#define DMA_CCR4_DST_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR4_DST_BURST_SIZE_R)
#define CCR4_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_BURST_SIZE_POS, DMA_CCR4_DST_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR4_DST_BURST_SIZE_W)
#define CCR4_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_BURST_SIZE_POS, DMA_CCR4_DST_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR4_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_BURST_SIZE_POS, DMA_CCR4_DST_BURST_SIZE_LEN)

#define DMA0_CCR4_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_BURST_SIZE_POS, DMA_CCR4_DST_BURST_SIZE_LEN,value)


/* dst_burst_len ACCESS: RO */

#ifndef DMA_CCR4_DST_BURST_LEN_POS
#define DMA_CCR4_DST_BURST_LEN_POS      18
#endif

#ifndef DMA_CCR4_DST_BURST_LEN_LEN
#define DMA_CCR4_DST_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR4_DST_BURST_LEN_R)
#define CCR4_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_BURST_LEN_POS, DMA_CCR4_DST_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR4_DST_BURST_LEN_W)
#define CCR4_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_BURST_LEN_POS, DMA_CCR4_DST_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR4_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_BURST_LEN_POS, DMA_CCR4_DST_BURST_LEN_LEN)

#define DMA0_CCR4_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_BURST_LEN_POS, DMA_CCR4_DST_BURST_LEN_LEN,value)


/* dst_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR4_DST_PROT_CTRL_POS
#define DMA_CCR4_DST_PROT_CTRL_POS      22
#endif

#ifndef DMA_CCR4_DST_PROT_CTRL_LEN
#define DMA_CCR4_DST_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR4_DST_PROT_CTRL_R)
#define CCR4_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_PROT_CTRL_POS, DMA_CCR4_DST_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR4_DST_PROT_CTRL_W)
#define CCR4_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_PROT_CTRL_POS, DMA_CCR4_DST_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR4_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_PROT_CTRL_POS, DMA_CCR4_DST_PROT_CTRL_LEN)

#define DMA0_CCR4_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_PROT_CTRL_POS, DMA_CCR4_DST_PROT_CTRL_LEN,value)


/* dst_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR4_DST_CACHE_CTRL_POS
#define DMA_CCR4_DST_CACHE_CTRL_POS      25
#endif

#ifndef DMA_CCR4_DST_CACHE_CTRL_LEN
#define DMA_CCR4_DST_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR4_DST_CACHE_CTRL_R)
#define CCR4_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_CACHE_CTRL_POS, DMA_CCR4_DST_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR4_DST_CACHE_CTRL_W)
#define CCR4_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_CACHE_CTRL_POS, DMA_CCR4_DST_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR4_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_CACHE_CTRL_POS, DMA_CCR4_DST_CACHE_CTRL_LEN)

#define DMA0_CCR4_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_DST_CACHE_CTRL_POS, DMA_CCR4_DST_CACHE_CTRL_LEN,value)


/* endian_swap_size ACCESS: RO */

#ifndef DMA_CCR4_ENDIAN_SWAP_SIZE_POS
#define DMA_CCR4_ENDIAN_SWAP_SIZE_POS      28
#endif

#ifndef DMA_CCR4_ENDIAN_SWAP_SIZE_LEN
#define DMA_CCR4_ENDIAN_SWAP_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR4_ENDIAN_SWAP_SIZE_R)
#define CCR4_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_ENDIAN_SWAP_SIZE_POS, DMA_CCR4_ENDIAN_SWAP_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR4_ENDIAN_SWAP_SIZE_W)
#define CCR4_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_ENDIAN_SWAP_SIZE_POS, DMA_CCR4_ENDIAN_SWAP_SIZE_LEN,value)
#endif

#define DMA0_CCR4_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_ENDIAN_SWAP_SIZE_POS, DMA_CCR4_ENDIAN_SWAP_SIZE_LEN)

#define DMA0_CCR4_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR4_VAL),DMA_CCR4_ENDIAN_SWAP_SIZE_POS, DMA_CCR4_ENDIAN_SWAP_SIZE_LEN,value)


/* REGISTER: LC0_4 ACCESS: RO */

#if defined(_V1) && !defined(LC0_4_OFFSET)
#define LC0_4_OFFSET 0x48C
#endif

#if !defined(DMA_LC0_4_OFFSET)
#define DMA_LC0_4_OFFSET 0x48C
#endif

#if defined(_V1) && !defined(LC0_4_REG)
#define LC0_4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_4_OFFSET))
#endif

#if defined(_V1) && !defined(LC0_4_VAL)
#define LC0_4_VAL  PREFIX_VAL(LC0_4_REG)
#endif

#define DMA0_LC0_4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_4_OFFSET))
#define DMA0_LC0_4_VAL  PREFIX_VAL(DMA0_LC0_4_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC0_4_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC0_4_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC0_4_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC0_4_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC0_4_LOOP_COUNTER_ITERATIONS_R)
#define LC0_4_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_4_VAL),DMA_LC0_4_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_4_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC0_4_LOOP_COUNTER_ITERATIONS_W)
#define LC0_4_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_4_VAL),DMA_LC0_4_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_4_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC0_4_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_4_VAL),DMA_LC0_4_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_4_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC0_4_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_4_VAL),DMA_LC0_4_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_4_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: LC1_4 ACCESS: RO */

#if defined(_V1) && !defined(LC1_4_OFFSET)
#define LC1_4_OFFSET 0x490
#endif

#if !defined(DMA_LC1_4_OFFSET)
#define DMA_LC1_4_OFFSET 0x490
#endif

#if defined(_V1) && !defined(LC1_4_REG)
#define LC1_4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_4_OFFSET))
#endif

#if defined(_V1) && !defined(LC1_4_VAL)
#define LC1_4_VAL  PREFIX_VAL(LC1_4_REG)
#endif

#define DMA0_LC1_4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_4_OFFSET))
#define DMA0_LC1_4_VAL  PREFIX_VAL(DMA0_LC1_4_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC1_4_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC1_4_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC1_4_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC1_4_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC1_4_LOOP_COUNTER_ITERATIONS_R)
#define LC1_4_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_4_VAL),DMA_LC1_4_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_4_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC1_4_LOOP_COUNTER_ITERATIONS_W)
#define LC1_4_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_4_VAL),DMA_LC1_4_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_4_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC1_4_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_4_VAL),DMA_LC1_4_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_4_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC1_4_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_4_VAL),DMA_LC1_4_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_4_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: SAR5 ACCESS: RO */

#if defined(_V1) && !defined(SAR5_OFFSET)
#define SAR5_OFFSET 0x4A0
#endif

#if !defined(DMA_SAR5_OFFSET)
#define DMA_SAR5_OFFSET 0x4A0
#endif

#if defined(_V1) && !defined(SAR5_REG)
#define SAR5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR5_OFFSET))
#endif

#if defined(_V1) && !defined(SAR5_VAL)
#define SAR5_VAL  PREFIX_VAL(SAR5_REG)
#endif

#define DMA0_SAR5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR5_OFFSET))
#define DMA0_SAR5_VAL  PREFIX_VAL(DMA0_SAR5_REG)

/* FIELDS: */

/* src_addr ACCESS: RO */

#ifndef DMA_SAR5_SRC_ADDR_POS
#define DMA_SAR5_SRC_ADDR_POS      0
#endif

#ifndef DMA_SAR5_SRC_ADDR_LEN
#define DMA_SAR5_SRC_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(SAR5_SRC_ADDR_R)
#define SAR5_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR5_VAL),DMA_SAR5_SRC_ADDR_POS, DMA_SAR5_SRC_ADDR_LEN)
#endif

#if defined(_V1) && !defined(SAR5_SRC_ADDR_W)
#define SAR5_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR5_VAL),DMA_SAR5_SRC_ADDR_POS, DMA_SAR5_SRC_ADDR_LEN,value)
#endif

#define DMA0_SAR5_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR5_VAL),DMA_SAR5_SRC_ADDR_POS, DMA_SAR5_SRC_ADDR_LEN)

#define DMA0_SAR5_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR5_VAL),DMA_SAR5_SRC_ADDR_POS, DMA_SAR5_SRC_ADDR_LEN,value)


/* REGISTER: DAR5 ACCESS: RO */

#if defined(_V1) && !defined(DAR5_OFFSET)
#define DAR5_OFFSET 0x4A4
#endif

#if !defined(DMA_DAR5_OFFSET)
#define DMA_DAR5_OFFSET 0x4A4
#endif

#if defined(_V1) && !defined(DAR5_REG)
#define DAR5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR5_OFFSET))
#endif

#if defined(_V1) && !defined(DAR5_VAL)
#define DAR5_VAL  PREFIX_VAL(DAR5_REG)
#endif

#define DMA0_DAR5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR5_OFFSET))
#define DMA0_DAR5_VAL  PREFIX_VAL(DMA0_DAR5_REG)

/* FIELDS: */

/* dst_addr ACCESS: RO */

#ifndef DMA_DAR5_DST_ADDR_POS
#define DMA_DAR5_DST_ADDR_POS      0
#endif

#ifndef DMA_DAR5_DST_ADDR_LEN
#define DMA_DAR5_DST_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(DAR5_DST_ADDR_R)
#define DAR5_DST_ADDR_R        GetGroupBits32( (DMA0_DAR5_VAL),DMA_DAR5_DST_ADDR_POS, DMA_DAR5_DST_ADDR_LEN)
#endif

#if defined(_V1) && !defined(DAR5_DST_ADDR_W)
#define DAR5_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR5_VAL),DMA_DAR5_DST_ADDR_POS, DMA_DAR5_DST_ADDR_LEN,value)
#endif

#define DMA0_DAR5_DST_ADDR_R        GetGroupBits32( (DMA0_DAR5_VAL),DMA_DAR5_DST_ADDR_POS, DMA_DAR5_DST_ADDR_LEN)

#define DMA0_DAR5_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR5_VAL),DMA_DAR5_DST_ADDR_POS, DMA_DAR5_DST_ADDR_LEN,value)


/* REGISTER: CCR5 ACCESS: RO */

#if defined(_V1) && !defined(CCR5_OFFSET)
#define CCR5_OFFSET 0x4A8
#endif

#if !defined(DMA_CCR5_OFFSET)
#define DMA_CCR5_OFFSET 0x4A8
#endif

#if defined(_V1) && !defined(CCR5_REG)
#define CCR5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR5_OFFSET))
#endif

#if defined(_V1) && !defined(CCR5_VAL)
#define CCR5_VAL  PREFIX_VAL(CCR5_REG)
#endif

#define DMA0_CCR5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR5_OFFSET))
#define DMA0_CCR5_VAL  PREFIX_VAL(DMA0_CCR5_REG)

/* FIELDS: */

/* src_inc ACCESS: RO */

#ifndef DMA_CCR5_SRC_INC_POS
#define DMA_CCR5_SRC_INC_POS      0
#endif

#ifndef DMA_CCR5_SRC_INC_LEN
#define DMA_CCR5_SRC_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR5_SRC_INC_R)
#define CCR5_SRC_INC_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_INC_POS, DMA_CCR5_SRC_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR5_SRC_INC_W)
#define CCR5_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_INC_POS, DMA_CCR5_SRC_INC_LEN,value)
#endif

#define DMA0_CCR5_SRC_INC_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_INC_POS, DMA_CCR5_SRC_INC_LEN)

#define DMA0_CCR5_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_INC_POS, DMA_CCR5_SRC_INC_LEN,value)


/* src_burst_size ACCESS: RO */

#ifndef DMA_CCR5_SRC_BURST_SIZE_POS
#define DMA_CCR5_SRC_BURST_SIZE_POS      1
#endif

#ifndef DMA_CCR5_SRC_BURST_SIZE_LEN
#define DMA_CCR5_SRC_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR5_SRC_BURST_SIZE_R)
#define CCR5_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_BURST_SIZE_POS, DMA_CCR5_SRC_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR5_SRC_BURST_SIZE_W)
#define CCR5_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_BURST_SIZE_POS, DMA_CCR5_SRC_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR5_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_BURST_SIZE_POS, DMA_CCR5_SRC_BURST_SIZE_LEN)

#define DMA0_CCR5_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_BURST_SIZE_POS, DMA_CCR5_SRC_BURST_SIZE_LEN,value)


/* src_burst_len ACCESS: RO */

#ifndef DMA_CCR5_SRC_BURST_LEN_POS
#define DMA_CCR5_SRC_BURST_LEN_POS      4
#endif

#ifndef DMA_CCR5_SRC_BURST_LEN_LEN
#define DMA_CCR5_SRC_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR5_SRC_BURST_LEN_R)
#define CCR5_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_BURST_LEN_POS, DMA_CCR5_SRC_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR5_SRC_BURST_LEN_W)
#define CCR5_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_BURST_LEN_POS, DMA_CCR5_SRC_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR5_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_BURST_LEN_POS, DMA_CCR5_SRC_BURST_LEN_LEN)

#define DMA0_CCR5_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_BURST_LEN_POS, DMA_CCR5_SRC_BURST_LEN_LEN,value)


/* src_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR5_SRC_PROT_CTRL_POS
#define DMA_CCR5_SRC_PROT_CTRL_POS      8
#endif

#ifndef DMA_CCR5_SRC_PROT_CTRL_LEN
#define DMA_CCR5_SRC_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR5_SRC_PROT_CTRL_R)
#define CCR5_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_PROT_CTRL_POS, DMA_CCR5_SRC_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR5_SRC_PROT_CTRL_W)
#define CCR5_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_PROT_CTRL_POS, DMA_CCR5_SRC_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR5_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_PROT_CTRL_POS, DMA_CCR5_SRC_PROT_CTRL_LEN)

#define DMA0_CCR5_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_PROT_CTRL_POS, DMA_CCR5_SRC_PROT_CTRL_LEN,value)


/* src_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR5_SRC_CACHE_CTRL_POS
#define DMA_CCR5_SRC_CACHE_CTRL_POS      11
#endif

#ifndef DMA_CCR5_SRC_CACHE_CTRL_LEN
#define DMA_CCR5_SRC_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR5_SRC_CACHE_CTRL_R)
#define CCR5_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_CACHE_CTRL_POS, DMA_CCR5_SRC_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR5_SRC_CACHE_CTRL_W)
#define CCR5_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_CACHE_CTRL_POS, DMA_CCR5_SRC_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR5_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_CACHE_CTRL_POS, DMA_CCR5_SRC_CACHE_CTRL_LEN)

#define DMA0_CCR5_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_SRC_CACHE_CTRL_POS, DMA_CCR5_SRC_CACHE_CTRL_LEN,value)


/* dst_inc ACCESS: RO */

#ifndef DMA_CCR5_DST_INC_POS
#define DMA_CCR5_DST_INC_POS      14
#endif

#ifndef DMA_CCR5_DST_INC_LEN
#define DMA_CCR5_DST_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR5_DST_INC_R)
#define CCR5_DST_INC_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_INC_POS, DMA_CCR5_DST_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR5_DST_INC_W)
#define CCR5_DST_INC_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_INC_POS, DMA_CCR5_DST_INC_LEN,value)
#endif

#define DMA0_CCR5_DST_INC_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_INC_POS, DMA_CCR5_DST_INC_LEN)

#define DMA0_CCR5_DST_INC_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_INC_POS, DMA_CCR5_DST_INC_LEN,value)


/* dst_burst_size ACCESS: RO */

#ifndef DMA_CCR5_DST_BURST_SIZE_POS
#define DMA_CCR5_DST_BURST_SIZE_POS      15
#endif

#ifndef DMA_CCR5_DST_BURST_SIZE_LEN
#define DMA_CCR5_DST_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR5_DST_BURST_SIZE_R)
#define CCR5_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_BURST_SIZE_POS, DMA_CCR5_DST_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR5_DST_BURST_SIZE_W)
#define CCR5_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_BURST_SIZE_POS, DMA_CCR5_DST_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR5_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_BURST_SIZE_POS, DMA_CCR5_DST_BURST_SIZE_LEN)

#define DMA0_CCR5_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_BURST_SIZE_POS, DMA_CCR5_DST_BURST_SIZE_LEN,value)


/* dst_burst_len ACCESS: RO */

#ifndef DMA_CCR5_DST_BURST_LEN_POS
#define DMA_CCR5_DST_BURST_LEN_POS      18
#endif

#ifndef DMA_CCR5_DST_BURST_LEN_LEN
#define DMA_CCR5_DST_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR5_DST_BURST_LEN_R)
#define CCR5_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_BURST_LEN_POS, DMA_CCR5_DST_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR5_DST_BURST_LEN_W)
#define CCR5_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_BURST_LEN_POS, DMA_CCR5_DST_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR5_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_BURST_LEN_POS, DMA_CCR5_DST_BURST_LEN_LEN)

#define DMA0_CCR5_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_BURST_LEN_POS, DMA_CCR5_DST_BURST_LEN_LEN,value)


/* dst_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR5_DST_PROT_CTRL_POS
#define DMA_CCR5_DST_PROT_CTRL_POS      22
#endif

#ifndef DMA_CCR5_DST_PROT_CTRL_LEN
#define DMA_CCR5_DST_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR5_DST_PROT_CTRL_R)
#define CCR5_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_PROT_CTRL_POS, DMA_CCR5_DST_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR5_DST_PROT_CTRL_W)
#define CCR5_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_PROT_CTRL_POS, DMA_CCR5_DST_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR5_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_PROT_CTRL_POS, DMA_CCR5_DST_PROT_CTRL_LEN)

#define DMA0_CCR5_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_PROT_CTRL_POS, DMA_CCR5_DST_PROT_CTRL_LEN,value)


/* dst_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR5_DST_CACHE_CTRL_POS
#define DMA_CCR5_DST_CACHE_CTRL_POS      25
#endif

#ifndef DMA_CCR5_DST_CACHE_CTRL_LEN
#define DMA_CCR5_DST_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR5_DST_CACHE_CTRL_R)
#define CCR5_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_CACHE_CTRL_POS, DMA_CCR5_DST_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR5_DST_CACHE_CTRL_W)
#define CCR5_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_CACHE_CTRL_POS, DMA_CCR5_DST_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR5_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_CACHE_CTRL_POS, DMA_CCR5_DST_CACHE_CTRL_LEN)

#define DMA0_CCR5_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_DST_CACHE_CTRL_POS, DMA_CCR5_DST_CACHE_CTRL_LEN,value)


/* endian_swap_size ACCESS: RO */

#ifndef DMA_CCR5_ENDIAN_SWAP_SIZE_POS
#define DMA_CCR5_ENDIAN_SWAP_SIZE_POS      28
#endif

#ifndef DMA_CCR5_ENDIAN_SWAP_SIZE_LEN
#define DMA_CCR5_ENDIAN_SWAP_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR5_ENDIAN_SWAP_SIZE_R)
#define CCR5_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_ENDIAN_SWAP_SIZE_POS, DMA_CCR5_ENDIAN_SWAP_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR5_ENDIAN_SWAP_SIZE_W)
#define CCR5_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_ENDIAN_SWAP_SIZE_POS, DMA_CCR5_ENDIAN_SWAP_SIZE_LEN,value)
#endif

#define DMA0_CCR5_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_ENDIAN_SWAP_SIZE_POS, DMA_CCR5_ENDIAN_SWAP_SIZE_LEN)

#define DMA0_CCR5_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR5_VAL),DMA_CCR5_ENDIAN_SWAP_SIZE_POS, DMA_CCR5_ENDIAN_SWAP_SIZE_LEN,value)


/* REGISTER: LC0_5 ACCESS: RO */

#if defined(_V1) && !defined(LC0_5_OFFSET)
#define LC0_5_OFFSET 0x4AC
#endif

#if !defined(DMA_LC0_5_OFFSET)
#define DMA_LC0_5_OFFSET 0x4AC
#endif

#if defined(_V1) && !defined(LC0_5_REG)
#define LC0_5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_5_OFFSET))
#endif

#if defined(_V1) && !defined(LC0_5_VAL)
#define LC0_5_VAL  PREFIX_VAL(LC0_5_REG)
#endif

#define DMA0_LC0_5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_5_OFFSET))
#define DMA0_LC0_5_VAL  PREFIX_VAL(DMA0_LC0_5_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC0_5_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC0_5_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC0_5_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC0_5_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC0_5_LOOP_COUNTER_ITERATIONS_R)
#define LC0_5_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_5_VAL),DMA_LC0_5_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_5_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC0_5_LOOP_COUNTER_ITERATIONS_W)
#define LC0_5_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_5_VAL),DMA_LC0_5_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_5_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC0_5_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_5_VAL),DMA_LC0_5_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_5_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC0_5_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_5_VAL),DMA_LC0_5_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_5_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: LC1_5 ACCESS: RO */

#if defined(_V1) && !defined(LC1_5_OFFSET)
#define LC1_5_OFFSET 0x4B0
#endif

#if !defined(DMA_LC1_5_OFFSET)
#define DMA_LC1_5_OFFSET 0x4B0
#endif

#if defined(_V1) && !defined(LC1_5_REG)
#define LC1_5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_5_OFFSET))
#endif

#if defined(_V1) && !defined(LC1_5_VAL)
#define LC1_5_VAL  PREFIX_VAL(LC1_5_REG)
#endif

#define DMA0_LC1_5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_5_OFFSET))
#define DMA0_LC1_5_VAL  PREFIX_VAL(DMA0_LC1_5_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC1_5_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC1_5_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC1_5_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC1_5_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC1_5_LOOP_COUNTER_ITERATIONS_R)
#define LC1_5_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_5_VAL),DMA_LC1_5_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_5_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC1_5_LOOP_COUNTER_ITERATIONS_W)
#define LC1_5_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_5_VAL),DMA_LC1_5_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_5_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC1_5_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_5_VAL),DMA_LC1_5_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_5_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC1_5_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_5_VAL),DMA_LC1_5_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_5_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: FTR3 ACCESS: RO */

#if defined(_V1) && !defined(FTR3_OFFSET)
#define FTR3_OFFSET 0x4C
#endif

#if !defined(DMA_FTR3_OFFSET)
#define DMA_FTR3_OFFSET 0x4C
#endif

#if defined(_V1) && !defined(FTR3_REG)
#define FTR3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR3_OFFSET))
#endif

#if defined(_V1) && !defined(FTR3_VAL)
#define FTR3_VAL  PREFIX_VAL(FTR3_REG)
#endif

#define DMA0_FTR3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR3_OFFSET))
#define DMA0_FTR3_VAL  PREFIX_VAL(DMA0_FTR3_REG)

/* FIELDS: */

/* undef_instr ACCESS: RO */

#ifndef DMA_FTR3_UNDEF_INSTR_POS
#define DMA_FTR3_UNDEF_INSTR_POS      0
#endif

#ifndef DMA_FTR3_UNDEF_INSTR_LEN
#define DMA_FTR3_UNDEF_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR3_UNDEF_INSTR_R)
#define FTR3_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_UNDEF_INSTR_POS, DMA_FTR3_UNDEF_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR3_UNDEF_INSTR_W)
#define FTR3_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_UNDEF_INSTR_POS, DMA_FTR3_UNDEF_INSTR_LEN,value)
#endif

#define DMA0_FTR3_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_UNDEF_INSTR_POS, DMA_FTR3_UNDEF_INSTR_LEN)

#define DMA0_FTR3_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_UNDEF_INSTR_POS, DMA_FTR3_UNDEF_INSTR_LEN,value)


/* operand_invalid ACCESS: RO */

#ifndef DMA_FTR3_OPERAND_INVALID_POS
#define DMA_FTR3_OPERAND_INVALID_POS      1
#endif

#ifndef DMA_FTR3_OPERAND_INVALID_LEN
#define DMA_FTR3_OPERAND_INVALID_LEN      1
#endif

#if defined(_V1) && !defined(FTR3_OPERAND_INVALID_R)
#define FTR3_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_OPERAND_INVALID_POS, DMA_FTR3_OPERAND_INVALID_LEN)
#endif

#if defined(_V1) && !defined(FTR3_OPERAND_INVALID_W)
#define FTR3_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_OPERAND_INVALID_POS, DMA_FTR3_OPERAND_INVALID_LEN,value)
#endif

#define DMA0_FTR3_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_OPERAND_INVALID_POS, DMA_FTR3_OPERAND_INVALID_LEN)

#define DMA0_FTR3_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_OPERAND_INVALID_POS, DMA_FTR3_OPERAND_INVALID_LEN,value)


/* ch_evnt_err ACCESS: RO */

#ifndef DMA_FTR3_CH_EVNT_ERR_POS
#define DMA_FTR3_CH_EVNT_ERR_POS      5
#endif

#ifndef DMA_FTR3_CH_EVNT_ERR_LEN
#define DMA_FTR3_CH_EVNT_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR3_CH_EVNT_ERR_R)
#define FTR3_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_CH_EVNT_ERR_POS, DMA_FTR3_CH_EVNT_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR3_CH_EVNT_ERR_W)
#define FTR3_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_CH_EVNT_ERR_POS, DMA_FTR3_CH_EVNT_ERR_LEN,value)
#endif

#define DMA0_FTR3_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_CH_EVNT_ERR_POS, DMA_FTR3_CH_EVNT_ERR_LEN)

#define DMA0_FTR3_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_CH_EVNT_ERR_POS, DMA_FTR3_CH_EVNT_ERR_LEN,value)


/* ch_periph_err ACCESS: RO */

#ifndef DMA_FTR3_CH_PERIPH_ERR_POS
#define DMA_FTR3_CH_PERIPH_ERR_POS      6
#endif

#ifndef DMA_FTR3_CH_PERIPH_ERR_LEN
#define DMA_FTR3_CH_PERIPH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR3_CH_PERIPH_ERR_R)
#define FTR3_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_CH_PERIPH_ERR_POS, DMA_FTR3_CH_PERIPH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR3_CH_PERIPH_ERR_W)
#define FTR3_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_CH_PERIPH_ERR_POS, DMA_FTR3_CH_PERIPH_ERR_LEN,value)
#endif

#define DMA0_FTR3_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_CH_PERIPH_ERR_POS, DMA_FTR3_CH_PERIPH_ERR_LEN)

#define DMA0_FTR3_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_CH_PERIPH_ERR_POS, DMA_FTR3_CH_PERIPH_ERR_LEN,value)


/* ch_rdwr_err ACCESS: RO */

#ifndef DMA_FTR3_CH_RDWR_ERR_POS
#define DMA_FTR3_CH_RDWR_ERR_POS      7
#endif

#ifndef DMA_FTR3_CH_RDWR_ERR_LEN
#define DMA_FTR3_CH_RDWR_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR3_CH_RDWR_ERR_R)
#define FTR3_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_CH_RDWR_ERR_POS, DMA_FTR3_CH_RDWR_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR3_CH_RDWR_ERR_W)
#define FTR3_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_CH_RDWR_ERR_POS, DMA_FTR3_CH_RDWR_ERR_LEN,value)
#endif

#define DMA0_FTR3_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_CH_RDWR_ERR_POS, DMA_FTR3_CH_RDWR_ERR_LEN)

#define DMA0_FTR3_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_CH_RDWR_ERR_POS, DMA_FTR3_CH_RDWR_ERR_LEN,value)


/* mfifo_err ACCESS: RO */

#ifndef DMA_FTR3_MFIFO_ERR_POS
#define DMA_FTR3_MFIFO_ERR_POS      12
#endif

#ifndef DMA_FTR3_MFIFO_ERR_LEN
#define DMA_FTR3_MFIFO_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR3_MFIFO_ERR_R)
#define FTR3_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_MFIFO_ERR_POS, DMA_FTR3_MFIFO_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR3_MFIFO_ERR_W)
#define FTR3_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_MFIFO_ERR_POS, DMA_FTR3_MFIFO_ERR_LEN,value)
#endif

#define DMA0_FTR3_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_MFIFO_ERR_POS, DMA_FTR3_MFIFO_ERR_LEN)

#define DMA0_FTR3_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_MFIFO_ERR_POS, DMA_FTR3_MFIFO_ERR_LEN,value)


/* st_data_unavailable ACCESS: RO */

#ifndef DMA_FTR3_ST_DATA_UNAVAILABLE_POS
#define DMA_FTR3_ST_DATA_UNAVAILABLE_POS      13
#endif

#ifndef DMA_FTR3_ST_DATA_UNAVAILABLE_LEN
#define DMA_FTR3_ST_DATA_UNAVAILABLE_LEN      1
#endif

#if defined(_V1) && !defined(FTR3_ST_DATA_UNAVAILABLE_R)
#define FTR3_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_ST_DATA_UNAVAILABLE_POS, DMA_FTR3_ST_DATA_UNAVAILABLE_LEN)
#endif

#if defined(_V1) && !defined(FTR3_ST_DATA_UNAVAILABLE_W)
#define FTR3_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_ST_DATA_UNAVAILABLE_POS, DMA_FTR3_ST_DATA_UNAVAILABLE_LEN,value)
#endif

#define DMA0_FTR3_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_ST_DATA_UNAVAILABLE_POS, DMA_FTR3_ST_DATA_UNAVAILABLE_LEN)

#define DMA0_FTR3_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_ST_DATA_UNAVAILABLE_POS, DMA_FTR3_ST_DATA_UNAVAILABLE_LEN,value)


/* instr_fetch_err ACCESS: RO */

#ifndef DMA_FTR3_INSTR_FETCH_ERR_POS
#define DMA_FTR3_INSTR_FETCH_ERR_POS      16
#endif

#ifndef DMA_FTR3_INSTR_FETCH_ERR_LEN
#define DMA_FTR3_INSTR_FETCH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR3_INSTR_FETCH_ERR_R)
#define FTR3_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_INSTR_FETCH_ERR_POS, DMA_FTR3_INSTR_FETCH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR3_INSTR_FETCH_ERR_W)
#define FTR3_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_INSTR_FETCH_ERR_POS, DMA_FTR3_INSTR_FETCH_ERR_LEN,value)
#endif

#define DMA0_FTR3_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_INSTR_FETCH_ERR_POS, DMA_FTR3_INSTR_FETCH_ERR_LEN)

#define DMA0_FTR3_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_INSTR_FETCH_ERR_POS, DMA_FTR3_INSTR_FETCH_ERR_LEN,value)


/* data_write_err ACCESS: RO */

#ifndef DMA_FTR3_DATA_WRITE_ERR_POS
#define DMA_FTR3_DATA_WRITE_ERR_POS      17
#endif

#ifndef DMA_FTR3_DATA_WRITE_ERR_LEN
#define DMA_FTR3_DATA_WRITE_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR3_DATA_WRITE_ERR_R)
#define FTR3_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_DATA_WRITE_ERR_POS, DMA_FTR3_DATA_WRITE_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR3_DATA_WRITE_ERR_W)
#define FTR3_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_DATA_WRITE_ERR_POS, DMA_FTR3_DATA_WRITE_ERR_LEN,value)
#endif

#define DMA0_FTR3_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_DATA_WRITE_ERR_POS, DMA_FTR3_DATA_WRITE_ERR_LEN)

#define DMA0_FTR3_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_DATA_WRITE_ERR_POS, DMA_FTR3_DATA_WRITE_ERR_LEN,value)


/* data_read_err ACCESS: RO */

#ifndef DMA_FTR3_DATA_READ_ERR_POS
#define DMA_FTR3_DATA_READ_ERR_POS      18
#endif

#ifndef DMA_FTR3_DATA_READ_ERR_LEN
#define DMA_FTR3_DATA_READ_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR3_DATA_READ_ERR_R)
#define FTR3_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_DATA_READ_ERR_POS, DMA_FTR3_DATA_READ_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR3_DATA_READ_ERR_W)
#define FTR3_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_DATA_READ_ERR_POS, DMA_FTR3_DATA_READ_ERR_LEN,value)
#endif

#define DMA0_FTR3_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_DATA_READ_ERR_POS, DMA_FTR3_DATA_READ_ERR_LEN)

#define DMA0_FTR3_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_DATA_READ_ERR_POS, DMA_FTR3_DATA_READ_ERR_LEN,value)


/* dbg_instr ACCESS: RO */

#ifndef DMA_FTR3_DBG_INSTR_POS
#define DMA_FTR3_DBG_INSTR_POS      30
#endif

#ifndef DMA_FTR3_DBG_INSTR_LEN
#define DMA_FTR3_DBG_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR3_DBG_INSTR_R)
#define FTR3_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_DBG_INSTR_POS, DMA_FTR3_DBG_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR3_DBG_INSTR_W)
#define FTR3_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_DBG_INSTR_POS, DMA_FTR3_DBG_INSTR_LEN,value)
#endif

#define DMA0_FTR3_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_DBG_INSTR_POS, DMA_FTR3_DBG_INSTR_LEN)

#define DMA0_FTR3_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_DBG_INSTR_POS, DMA_FTR3_DBG_INSTR_LEN,value)


/* lockup_err ACCESS: RO */

#ifndef DMA_FTR3_LOCKUP_ERR_POS
#define DMA_FTR3_LOCKUP_ERR_POS      31
#endif

#ifndef DMA_FTR3_LOCKUP_ERR_LEN
#define DMA_FTR3_LOCKUP_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR3_LOCKUP_ERR_R)
#define FTR3_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_LOCKUP_ERR_POS, DMA_FTR3_LOCKUP_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR3_LOCKUP_ERR_W)
#define FTR3_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_LOCKUP_ERR_POS, DMA_FTR3_LOCKUP_ERR_LEN,value)
#endif

#define DMA0_FTR3_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_LOCKUP_ERR_POS, DMA_FTR3_LOCKUP_ERR_LEN)

#define DMA0_FTR3_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR3_VAL),DMA_FTR3_LOCKUP_ERR_POS, DMA_FTR3_LOCKUP_ERR_LEN,value)


/* REGISTER: SAR6 ACCESS: RO */

#if defined(_V1) && !defined(SAR6_OFFSET)
#define SAR6_OFFSET 0x4C0
#endif

#if !defined(DMA_SAR6_OFFSET)
#define DMA_SAR6_OFFSET 0x4C0
#endif

#if defined(_V1) && !defined(SAR6_REG)
#define SAR6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR6_OFFSET))
#endif

#if defined(_V1) && !defined(SAR6_VAL)
#define SAR6_VAL  PREFIX_VAL(SAR6_REG)
#endif

#define DMA0_SAR6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR6_OFFSET))
#define DMA0_SAR6_VAL  PREFIX_VAL(DMA0_SAR6_REG)

/* FIELDS: */

/* src_addr ACCESS: RO */

#ifndef DMA_SAR6_SRC_ADDR_POS
#define DMA_SAR6_SRC_ADDR_POS      0
#endif

#ifndef DMA_SAR6_SRC_ADDR_LEN
#define DMA_SAR6_SRC_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(SAR6_SRC_ADDR_R)
#define SAR6_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR6_VAL),DMA_SAR6_SRC_ADDR_POS, DMA_SAR6_SRC_ADDR_LEN)
#endif

#if defined(_V1) && !defined(SAR6_SRC_ADDR_W)
#define SAR6_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR6_VAL),DMA_SAR6_SRC_ADDR_POS, DMA_SAR6_SRC_ADDR_LEN,value)
#endif

#define DMA0_SAR6_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR6_VAL),DMA_SAR6_SRC_ADDR_POS, DMA_SAR6_SRC_ADDR_LEN)

#define DMA0_SAR6_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR6_VAL),DMA_SAR6_SRC_ADDR_POS, DMA_SAR6_SRC_ADDR_LEN,value)


/* REGISTER: DAR6 ACCESS: RO */

#if defined(_V1) && !defined(DAR6_OFFSET)
#define DAR6_OFFSET 0x4C4
#endif

#if !defined(DMA_DAR6_OFFSET)
#define DMA_DAR6_OFFSET 0x4C4
#endif

#if defined(_V1) && !defined(DAR6_REG)
#define DAR6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR6_OFFSET))
#endif

#if defined(_V1) && !defined(DAR6_VAL)
#define DAR6_VAL  PREFIX_VAL(DAR6_REG)
#endif

#define DMA0_DAR6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR6_OFFSET))
#define DMA0_DAR6_VAL  PREFIX_VAL(DMA0_DAR6_REG)

/* FIELDS: */

/* dst_addr ACCESS: RO */

#ifndef DMA_DAR6_DST_ADDR_POS
#define DMA_DAR6_DST_ADDR_POS      0
#endif

#ifndef DMA_DAR6_DST_ADDR_LEN
#define DMA_DAR6_DST_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(DAR6_DST_ADDR_R)
#define DAR6_DST_ADDR_R        GetGroupBits32( (DMA0_DAR6_VAL),DMA_DAR6_DST_ADDR_POS, DMA_DAR6_DST_ADDR_LEN)
#endif

#if defined(_V1) && !defined(DAR6_DST_ADDR_W)
#define DAR6_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR6_VAL),DMA_DAR6_DST_ADDR_POS, DMA_DAR6_DST_ADDR_LEN,value)
#endif

#define DMA0_DAR6_DST_ADDR_R        GetGroupBits32( (DMA0_DAR6_VAL),DMA_DAR6_DST_ADDR_POS, DMA_DAR6_DST_ADDR_LEN)

#define DMA0_DAR6_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR6_VAL),DMA_DAR6_DST_ADDR_POS, DMA_DAR6_DST_ADDR_LEN,value)


/* REGISTER: CCR6 ACCESS: RO */

#if defined(_V1) && !defined(CCR6_OFFSET)
#define CCR6_OFFSET 0x4C8
#endif

#if !defined(DMA_CCR6_OFFSET)
#define DMA_CCR6_OFFSET 0x4C8
#endif

#if defined(_V1) && !defined(CCR6_REG)
#define CCR6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR6_OFFSET))
#endif

#if defined(_V1) && !defined(CCR6_VAL)
#define CCR6_VAL  PREFIX_VAL(CCR6_REG)
#endif

#define DMA0_CCR6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR6_OFFSET))
#define DMA0_CCR6_VAL  PREFIX_VAL(DMA0_CCR6_REG)

/* FIELDS: */

/* src_inc ACCESS: RO */

#ifndef DMA_CCR6_SRC_INC_POS
#define DMA_CCR6_SRC_INC_POS      0
#endif

#ifndef DMA_CCR6_SRC_INC_LEN
#define DMA_CCR6_SRC_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR6_SRC_INC_R)
#define CCR6_SRC_INC_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_INC_POS, DMA_CCR6_SRC_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR6_SRC_INC_W)
#define CCR6_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_INC_POS, DMA_CCR6_SRC_INC_LEN,value)
#endif

#define DMA0_CCR6_SRC_INC_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_INC_POS, DMA_CCR6_SRC_INC_LEN)

#define DMA0_CCR6_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_INC_POS, DMA_CCR6_SRC_INC_LEN,value)


/* src_burst_size ACCESS: RO */

#ifndef DMA_CCR6_SRC_BURST_SIZE_POS
#define DMA_CCR6_SRC_BURST_SIZE_POS      1
#endif

#ifndef DMA_CCR6_SRC_BURST_SIZE_LEN
#define DMA_CCR6_SRC_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR6_SRC_BURST_SIZE_R)
#define CCR6_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_BURST_SIZE_POS, DMA_CCR6_SRC_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR6_SRC_BURST_SIZE_W)
#define CCR6_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_BURST_SIZE_POS, DMA_CCR6_SRC_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR6_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_BURST_SIZE_POS, DMA_CCR6_SRC_BURST_SIZE_LEN)

#define DMA0_CCR6_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_BURST_SIZE_POS, DMA_CCR6_SRC_BURST_SIZE_LEN,value)


/* src_burst_len ACCESS: RO */

#ifndef DMA_CCR6_SRC_BURST_LEN_POS
#define DMA_CCR6_SRC_BURST_LEN_POS      4
#endif

#ifndef DMA_CCR6_SRC_BURST_LEN_LEN
#define DMA_CCR6_SRC_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR6_SRC_BURST_LEN_R)
#define CCR6_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_BURST_LEN_POS, DMA_CCR6_SRC_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR6_SRC_BURST_LEN_W)
#define CCR6_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_BURST_LEN_POS, DMA_CCR6_SRC_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR6_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_BURST_LEN_POS, DMA_CCR6_SRC_BURST_LEN_LEN)

#define DMA0_CCR6_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_BURST_LEN_POS, DMA_CCR6_SRC_BURST_LEN_LEN,value)


/* src_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR6_SRC_PROT_CTRL_POS
#define DMA_CCR6_SRC_PROT_CTRL_POS      8
#endif

#ifndef DMA_CCR6_SRC_PROT_CTRL_LEN
#define DMA_CCR6_SRC_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR6_SRC_PROT_CTRL_R)
#define CCR6_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_PROT_CTRL_POS, DMA_CCR6_SRC_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR6_SRC_PROT_CTRL_W)
#define CCR6_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_PROT_CTRL_POS, DMA_CCR6_SRC_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR6_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_PROT_CTRL_POS, DMA_CCR6_SRC_PROT_CTRL_LEN)

#define DMA0_CCR6_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_PROT_CTRL_POS, DMA_CCR6_SRC_PROT_CTRL_LEN,value)


/* src_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR6_SRC_CACHE_CTRL_POS
#define DMA_CCR6_SRC_CACHE_CTRL_POS      11
#endif

#ifndef DMA_CCR6_SRC_CACHE_CTRL_LEN
#define DMA_CCR6_SRC_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR6_SRC_CACHE_CTRL_R)
#define CCR6_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_CACHE_CTRL_POS, DMA_CCR6_SRC_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR6_SRC_CACHE_CTRL_W)
#define CCR6_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_CACHE_CTRL_POS, DMA_CCR6_SRC_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR6_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_CACHE_CTRL_POS, DMA_CCR6_SRC_CACHE_CTRL_LEN)

#define DMA0_CCR6_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_SRC_CACHE_CTRL_POS, DMA_CCR6_SRC_CACHE_CTRL_LEN,value)


/* dst_inc ACCESS: RO */

#ifndef DMA_CCR6_DST_INC_POS
#define DMA_CCR6_DST_INC_POS      14
#endif

#ifndef DMA_CCR6_DST_INC_LEN
#define DMA_CCR6_DST_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR6_DST_INC_R)
#define CCR6_DST_INC_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_INC_POS, DMA_CCR6_DST_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR6_DST_INC_W)
#define CCR6_DST_INC_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_INC_POS, DMA_CCR6_DST_INC_LEN,value)
#endif

#define DMA0_CCR6_DST_INC_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_INC_POS, DMA_CCR6_DST_INC_LEN)

#define DMA0_CCR6_DST_INC_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_INC_POS, DMA_CCR6_DST_INC_LEN,value)


/* dst_burst_size ACCESS: RO */

#ifndef DMA_CCR6_DST_BURST_SIZE_POS
#define DMA_CCR6_DST_BURST_SIZE_POS      15
#endif

#ifndef DMA_CCR6_DST_BURST_SIZE_LEN
#define DMA_CCR6_DST_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR6_DST_BURST_SIZE_R)
#define CCR6_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_BURST_SIZE_POS, DMA_CCR6_DST_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR6_DST_BURST_SIZE_W)
#define CCR6_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_BURST_SIZE_POS, DMA_CCR6_DST_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR6_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_BURST_SIZE_POS, DMA_CCR6_DST_BURST_SIZE_LEN)

#define DMA0_CCR6_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_BURST_SIZE_POS, DMA_CCR6_DST_BURST_SIZE_LEN,value)


/* dst_burst_len ACCESS: RO */

#ifndef DMA_CCR6_DST_BURST_LEN_POS
#define DMA_CCR6_DST_BURST_LEN_POS      18
#endif

#ifndef DMA_CCR6_DST_BURST_LEN_LEN
#define DMA_CCR6_DST_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR6_DST_BURST_LEN_R)
#define CCR6_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_BURST_LEN_POS, DMA_CCR6_DST_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR6_DST_BURST_LEN_W)
#define CCR6_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_BURST_LEN_POS, DMA_CCR6_DST_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR6_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_BURST_LEN_POS, DMA_CCR6_DST_BURST_LEN_LEN)

#define DMA0_CCR6_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_BURST_LEN_POS, DMA_CCR6_DST_BURST_LEN_LEN,value)


/* dst_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR6_DST_PROT_CTRL_POS
#define DMA_CCR6_DST_PROT_CTRL_POS      22
#endif

#ifndef DMA_CCR6_DST_PROT_CTRL_LEN
#define DMA_CCR6_DST_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR6_DST_PROT_CTRL_R)
#define CCR6_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_PROT_CTRL_POS, DMA_CCR6_DST_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR6_DST_PROT_CTRL_W)
#define CCR6_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_PROT_CTRL_POS, DMA_CCR6_DST_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR6_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_PROT_CTRL_POS, DMA_CCR6_DST_PROT_CTRL_LEN)

#define DMA0_CCR6_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_PROT_CTRL_POS, DMA_CCR6_DST_PROT_CTRL_LEN,value)


/* dst_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR6_DST_CACHE_CTRL_POS
#define DMA_CCR6_DST_CACHE_CTRL_POS      25
#endif

#ifndef DMA_CCR6_DST_CACHE_CTRL_LEN
#define DMA_CCR6_DST_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR6_DST_CACHE_CTRL_R)
#define CCR6_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_CACHE_CTRL_POS, DMA_CCR6_DST_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR6_DST_CACHE_CTRL_W)
#define CCR6_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_CACHE_CTRL_POS, DMA_CCR6_DST_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR6_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_CACHE_CTRL_POS, DMA_CCR6_DST_CACHE_CTRL_LEN)

#define DMA0_CCR6_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_DST_CACHE_CTRL_POS, DMA_CCR6_DST_CACHE_CTRL_LEN,value)


/* endian_swap_size ACCESS: RO */

#ifndef DMA_CCR6_ENDIAN_SWAP_SIZE_POS
#define DMA_CCR6_ENDIAN_SWAP_SIZE_POS      28
#endif

#ifndef DMA_CCR6_ENDIAN_SWAP_SIZE_LEN
#define DMA_CCR6_ENDIAN_SWAP_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR6_ENDIAN_SWAP_SIZE_R)
#define CCR6_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_ENDIAN_SWAP_SIZE_POS, DMA_CCR6_ENDIAN_SWAP_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR6_ENDIAN_SWAP_SIZE_W)
#define CCR6_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_ENDIAN_SWAP_SIZE_POS, DMA_CCR6_ENDIAN_SWAP_SIZE_LEN,value)
#endif

#define DMA0_CCR6_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_ENDIAN_SWAP_SIZE_POS, DMA_CCR6_ENDIAN_SWAP_SIZE_LEN)

#define DMA0_CCR6_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR6_VAL),DMA_CCR6_ENDIAN_SWAP_SIZE_POS, DMA_CCR6_ENDIAN_SWAP_SIZE_LEN,value)


/* REGISTER: LC0_6 ACCESS: RO */

#if defined(_V1) && !defined(LC0_6_OFFSET)
#define LC0_6_OFFSET 0x4CC
#endif

#if !defined(DMA_LC0_6_OFFSET)
#define DMA_LC0_6_OFFSET 0x4CC
#endif

#if defined(_V1) && !defined(LC0_6_REG)
#define LC0_6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_6_OFFSET))
#endif

#if defined(_V1) && !defined(LC0_6_VAL)
#define LC0_6_VAL  PREFIX_VAL(LC0_6_REG)
#endif

#define DMA0_LC0_6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_6_OFFSET))
#define DMA0_LC0_6_VAL  PREFIX_VAL(DMA0_LC0_6_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC0_6_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC0_6_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC0_6_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC0_6_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC0_6_LOOP_COUNTER_ITERATIONS_R)
#define LC0_6_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_6_VAL),DMA_LC0_6_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_6_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC0_6_LOOP_COUNTER_ITERATIONS_W)
#define LC0_6_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_6_VAL),DMA_LC0_6_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_6_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC0_6_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_6_VAL),DMA_LC0_6_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_6_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC0_6_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_6_VAL),DMA_LC0_6_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_6_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: LC1_6 ACCESS: RO */

#if defined(_V1) && !defined(LC1_6_OFFSET)
#define LC1_6_OFFSET 0x4D0
#endif

#if !defined(DMA_LC1_6_OFFSET)
#define DMA_LC1_6_OFFSET 0x4D0
#endif

#if defined(_V1) && !defined(LC1_6_REG)
#define LC1_6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_6_OFFSET))
#endif

#if defined(_V1) && !defined(LC1_6_VAL)
#define LC1_6_VAL  PREFIX_VAL(LC1_6_REG)
#endif

#define DMA0_LC1_6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_6_OFFSET))
#define DMA0_LC1_6_VAL  PREFIX_VAL(DMA0_LC1_6_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC1_6_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC1_6_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC1_6_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC1_6_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC1_6_LOOP_COUNTER_ITERATIONS_R)
#define LC1_6_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_6_VAL),DMA_LC1_6_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_6_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC1_6_LOOP_COUNTER_ITERATIONS_W)
#define LC1_6_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_6_VAL),DMA_LC1_6_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_6_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC1_6_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_6_VAL),DMA_LC1_6_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_6_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC1_6_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_6_VAL),DMA_LC1_6_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_6_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: SAR7 ACCESS: RO */

#if defined(_V1) && !defined(SAR7_OFFSET)
#define SAR7_OFFSET 0x4E0
#endif

#if !defined(DMA_SAR7_OFFSET)
#define DMA_SAR7_OFFSET 0x4E0
#endif

#if defined(_V1) && !defined(SAR7_REG)
#define SAR7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR7_OFFSET))
#endif

#if defined(_V1) && !defined(SAR7_VAL)
#define SAR7_VAL  PREFIX_VAL(SAR7_REG)
#endif

#define DMA0_SAR7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_SAR7_OFFSET))
#define DMA0_SAR7_VAL  PREFIX_VAL(DMA0_SAR7_REG)

/* FIELDS: */

/* src_addr ACCESS: RO */

#ifndef DMA_SAR7_SRC_ADDR_POS
#define DMA_SAR7_SRC_ADDR_POS      0
#endif

#ifndef DMA_SAR7_SRC_ADDR_LEN
#define DMA_SAR7_SRC_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(SAR7_SRC_ADDR_R)
#define SAR7_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR7_VAL),DMA_SAR7_SRC_ADDR_POS, DMA_SAR7_SRC_ADDR_LEN)
#endif

#if defined(_V1) && !defined(SAR7_SRC_ADDR_W)
#define SAR7_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR7_VAL),DMA_SAR7_SRC_ADDR_POS, DMA_SAR7_SRC_ADDR_LEN,value)
#endif

#define DMA0_SAR7_SRC_ADDR_R        GetGroupBits32( (DMA0_SAR7_VAL),DMA_SAR7_SRC_ADDR_POS, DMA_SAR7_SRC_ADDR_LEN)

#define DMA0_SAR7_SRC_ADDR_W(value) SetGroupBits32( (DMA0_SAR7_VAL),DMA_SAR7_SRC_ADDR_POS, DMA_SAR7_SRC_ADDR_LEN,value)


/* REGISTER: DAR7 ACCESS: RO */

#if defined(_V1) && !defined(DAR7_OFFSET)
#define DAR7_OFFSET 0x4E4
#endif

#if !defined(DMA_DAR7_OFFSET)
#define DMA_DAR7_OFFSET 0x4E4
#endif

#if defined(_V1) && !defined(DAR7_REG)
#define DAR7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR7_OFFSET))
#endif

#if defined(_V1) && !defined(DAR7_VAL)
#define DAR7_VAL  PREFIX_VAL(DAR7_REG)
#endif

#define DMA0_DAR7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DAR7_OFFSET))
#define DMA0_DAR7_VAL  PREFIX_VAL(DMA0_DAR7_REG)

/* FIELDS: */

/* dst_addr ACCESS: RO */

#ifndef DMA_DAR7_DST_ADDR_POS
#define DMA_DAR7_DST_ADDR_POS      0
#endif

#ifndef DMA_DAR7_DST_ADDR_LEN
#define DMA_DAR7_DST_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(DAR7_DST_ADDR_R)
#define DAR7_DST_ADDR_R        GetGroupBits32( (DMA0_DAR7_VAL),DMA_DAR7_DST_ADDR_POS, DMA_DAR7_DST_ADDR_LEN)
#endif

#if defined(_V1) && !defined(DAR7_DST_ADDR_W)
#define DAR7_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR7_VAL),DMA_DAR7_DST_ADDR_POS, DMA_DAR7_DST_ADDR_LEN,value)
#endif

#define DMA0_DAR7_DST_ADDR_R        GetGroupBits32( (DMA0_DAR7_VAL),DMA_DAR7_DST_ADDR_POS, DMA_DAR7_DST_ADDR_LEN)

#define DMA0_DAR7_DST_ADDR_W(value) SetGroupBits32( (DMA0_DAR7_VAL),DMA_DAR7_DST_ADDR_POS, DMA_DAR7_DST_ADDR_LEN,value)


/* REGISTER: CCR7 ACCESS: RO */

#if defined(_V1) && !defined(CCR7_OFFSET)
#define CCR7_OFFSET 0x4E8
#endif

#if !defined(DMA_CCR7_OFFSET)
#define DMA_CCR7_OFFSET 0x4E8
#endif

#if defined(_V1) && !defined(CCR7_REG)
#define CCR7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR7_OFFSET))
#endif

#if defined(_V1) && !defined(CCR7_VAL)
#define CCR7_VAL  PREFIX_VAL(CCR7_REG)
#endif

#define DMA0_CCR7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CCR7_OFFSET))
#define DMA0_CCR7_VAL  PREFIX_VAL(DMA0_CCR7_REG)

/* FIELDS: */

/* src_inc ACCESS: RO */

#ifndef DMA_CCR7_SRC_INC_POS
#define DMA_CCR7_SRC_INC_POS      0
#endif

#ifndef DMA_CCR7_SRC_INC_LEN
#define DMA_CCR7_SRC_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR7_SRC_INC_R)
#define CCR7_SRC_INC_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_INC_POS, DMA_CCR7_SRC_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR7_SRC_INC_W)
#define CCR7_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_INC_POS, DMA_CCR7_SRC_INC_LEN,value)
#endif

#define DMA0_CCR7_SRC_INC_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_INC_POS, DMA_CCR7_SRC_INC_LEN)

#define DMA0_CCR7_SRC_INC_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_INC_POS, DMA_CCR7_SRC_INC_LEN,value)


/* src_burst_size ACCESS: RO */

#ifndef DMA_CCR7_SRC_BURST_SIZE_POS
#define DMA_CCR7_SRC_BURST_SIZE_POS      1
#endif

#ifndef DMA_CCR7_SRC_BURST_SIZE_LEN
#define DMA_CCR7_SRC_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR7_SRC_BURST_SIZE_R)
#define CCR7_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_BURST_SIZE_POS, DMA_CCR7_SRC_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR7_SRC_BURST_SIZE_W)
#define CCR7_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_BURST_SIZE_POS, DMA_CCR7_SRC_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR7_SRC_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_BURST_SIZE_POS, DMA_CCR7_SRC_BURST_SIZE_LEN)

#define DMA0_CCR7_SRC_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_BURST_SIZE_POS, DMA_CCR7_SRC_BURST_SIZE_LEN,value)


/* src_burst_len ACCESS: RO */

#ifndef DMA_CCR7_SRC_BURST_LEN_POS
#define DMA_CCR7_SRC_BURST_LEN_POS      4
#endif

#ifndef DMA_CCR7_SRC_BURST_LEN_LEN
#define DMA_CCR7_SRC_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR7_SRC_BURST_LEN_R)
#define CCR7_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_BURST_LEN_POS, DMA_CCR7_SRC_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR7_SRC_BURST_LEN_W)
#define CCR7_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_BURST_LEN_POS, DMA_CCR7_SRC_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR7_SRC_BURST_LEN_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_BURST_LEN_POS, DMA_CCR7_SRC_BURST_LEN_LEN)

#define DMA0_CCR7_SRC_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_BURST_LEN_POS, DMA_CCR7_SRC_BURST_LEN_LEN,value)


/* src_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR7_SRC_PROT_CTRL_POS
#define DMA_CCR7_SRC_PROT_CTRL_POS      8
#endif

#ifndef DMA_CCR7_SRC_PROT_CTRL_LEN
#define DMA_CCR7_SRC_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR7_SRC_PROT_CTRL_R)
#define CCR7_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_PROT_CTRL_POS, DMA_CCR7_SRC_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR7_SRC_PROT_CTRL_W)
#define CCR7_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_PROT_CTRL_POS, DMA_CCR7_SRC_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR7_SRC_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_PROT_CTRL_POS, DMA_CCR7_SRC_PROT_CTRL_LEN)

#define DMA0_CCR7_SRC_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_PROT_CTRL_POS, DMA_CCR7_SRC_PROT_CTRL_LEN,value)


/* src_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR7_SRC_CACHE_CTRL_POS
#define DMA_CCR7_SRC_CACHE_CTRL_POS      11
#endif

#ifndef DMA_CCR7_SRC_CACHE_CTRL_LEN
#define DMA_CCR7_SRC_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR7_SRC_CACHE_CTRL_R)
#define CCR7_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_CACHE_CTRL_POS, DMA_CCR7_SRC_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR7_SRC_CACHE_CTRL_W)
#define CCR7_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_CACHE_CTRL_POS, DMA_CCR7_SRC_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR7_SRC_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_CACHE_CTRL_POS, DMA_CCR7_SRC_CACHE_CTRL_LEN)

#define DMA0_CCR7_SRC_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_SRC_CACHE_CTRL_POS, DMA_CCR7_SRC_CACHE_CTRL_LEN,value)


/* dst_inc ACCESS: RO */

#ifndef DMA_CCR7_DST_INC_POS
#define DMA_CCR7_DST_INC_POS      14
#endif

#ifndef DMA_CCR7_DST_INC_LEN
#define DMA_CCR7_DST_INC_LEN      1
#endif

#if defined(_V1) && !defined(CCR7_DST_INC_R)
#define CCR7_DST_INC_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_INC_POS, DMA_CCR7_DST_INC_LEN)
#endif

#if defined(_V1) && !defined(CCR7_DST_INC_W)
#define CCR7_DST_INC_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_INC_POS, DMA_CCR7_DST_INC_LEN,value)
#endif

#define DMA0_CCR7_DST_INC_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_INC_POS, DMA_CCR7_DST_INC_LEN)

#define DMA0_CCR7_DST_INC_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_INC_POS, DMA_CCR7_DST_INC_LEN,value)


/* dst_burst_size ACCESS: RO */

#ifndef DMA_CCR7_DST_BURST_SIZE_POS
#define DMA_CCR7_DST_BURST_SIZE_POS      15
#endif

#ifndef DMA_CCR7_DST_BURST_SIZE_LEN
#define DMA_CCR7_DST_BURST_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR7_DST_BURST_SIZE_R)
#define CCR7_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_BURST_SIZE_POS, DMA_CCR7_DST_BURST_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR7_DST_BURST_SIZE_W)
#define CCR7_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_BURST_SIZE_POS, DMA_CCR7_DST_BURST_SIZE_LEN,value)
#endif

#define DMA0_CCR7_DST_BURST_SIZE_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_BURST_SIZE_POS, DMA_CCR7_DST_BURST_SIZE_LEN)

#define DMA0_CCR7_DST_BURST_SIZE_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_BURST_SIZE_POS, DMA_CCR7_DST_BURST_SIZE_LEN,value)


/* dst_burst_len ACCESS: RO */

#ifndef DMA_CCR7_DST_BURST_LEN_POS
#define DMA_CCR7_DST_BURST_LEN_POS      18
#endif

#ifndef DMA_CCR7_DST_BURST_LEN_LEN
#define DMA_CCR7_DST_BURST_LEN_LEN      4
#endif

#if defined(_V1) && !defined(CCR7_DST_BURST_LEN_R)
#define CCR7_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_BURST_LEN_POS, DMA_CCR7_DST_BURST_LEN_LEN)
#endif

#if defined(_V1) && !defined(CCR7_DST_BURST_LEN_W)
#define CCR7_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_BURST_LEN_POS, DMA_CCR7_DST_BURST_LEN_LEN,value)
#endif

#define DMA0_CCR7_DST_BURST_LEN_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_BURST_LEN_POS, DMA_CCR7_DST_BURST_LEN_LEN)

#define DMA0_CCR7_DST_BURST_LEN_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_BURST_LEN_POS, DMA_CCR7_DST_BURST_LEN_LEN,value)


/* dst_prot_ctrl ACCESS: RO */

#ifndef DMA_CCR7_DST_PROT_CTRL_POS
#define DMA_CCR7_DST_PROT_CTRL_POS      22
#endif

#ifndef DMA_CCR7_DST_PROT_CTRL_LEN
#define DMA_CCR7_DST_PROT_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR7_DST_PROT_CTRL_R)
#define CCR7_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_PROT_CTRL_POS, DMA_CCR7_DST_PROT_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR7_DST_PROT_CTRL_W)
#define CCR7_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_PROT_CTRL_POS, DMA_CCR7_DST_PROT_CTRL_LEN,value)
#endif

#define DMA0_CCR7_DST_PROT_CTRL_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_PROT_CTRL_POS, DMA_CCR7_DST_PROT_CTRL_LEN)

#define DMA0_CCR7_DST_PROT_CTRL_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_PROT_CTRL_POS, DMA_CCR7_DST_PROT_CTRL_LEN,value)


/* dst_cache_ctrl ACCESS: RO */

#ifndef DMA_CCR7_DST_CACHE_CTRL_POS
#define DMA_CCR7_DST_CACHE_CTRL_POS      25
#endif

#ifndef DMA_CCR7_DST_CACHE_CTRL_LEN
#define DMA_CCR7_DST_CACHE_CTRL_LEN      3
#endif

#if defined(_V1) && !defined(CCR7_DST_CACHE_CTRL_R)
#define CCR7_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_CACHE_CTRL_POS, DMA_CCR7_DST_CACHE_CTRL_LEN)
#endif

#if defined(_V1) && !defined(CCR7_DST_CACHE_CTRL_W)
#define CCR7_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_CACHE_CTRL_POS, DMA_CCR7_DST_CACHE_CTRL_LEN,value)
#endif

#define DMA0_CCR7_DST_CACHE_CTRL_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_CACHE_CTRL_POS, DMA_CCR7_DST_CACHE_CTRL_LEN)

#define DMA0_CCR7_DST_CACHE_CTRL_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_DST_CACHE_CTRL_POS, DMA_CCR7_DST_CACHE_CTRL_LEN,value)


/* endian_swap_size ACCESS: RO */

#ifndef DMA_CCR7_ENDIAN_SWAP_SIZE_POS
#define DMA_CCR7_ENDIAN_SWAP_SIZE_POS      28
#endif

#ifndef DMA_CCR7_ENDIAN_SWAP_SIZE_LEN
#define DMA_CCR7_ENDIAN_SWAP_SIZE_LEN      3
#endif

#if defined(_V1) && !defined(CCR7_ENDIAN_SWAP_SIZE_R)
#define CCR7_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_ENDIAN_SWAP_SIZE_POS, DMA_CCR7_ENDIAN_SWAP_SIZE_LEN)
#endif

#if defined(_V1) && !defined(CCR7_ENDIAN_SWAP_SIZE_W)
#define CCR7_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_ENDIAN_SWAP_SIZE_POS, DMA_CCR7_ENDIAN_SWAP_SIZE_LEN,value)
#endif

#define DMA0_CCR7_ENDIAN_SWAP_SIZE_R        GetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_ENDIAN_SWAP_SIZE_POS, DMA_CCR7_ENDIAN_SWAP_SIZE_LEN)

#define DMA0_CCR7_ENDIAN_SWAP_SIZE_W(value) SetGroupBits32( (DMA0_CCR7_VAL),DMA_CCR7_ENDIAN_SWAP_SIZE_POS, DMA_CCR7_ENDIAN_SWAP_SIZE_LEN,value)


/* REGISTER: LC0_7 ACCESS: RO */

#if defined(_V1) && !defined(LC0_7_OFFSET)
#define LC0_7_OFFSET 0x4EC
#endif

#if !defined(DMA_LC0_7_OFFSET)
#define DMA_LC0_7_OFFSET 0x4EC
#endif

#if defined(_V1) && !defined(LC0_7_REG)
#define LC0_7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_7_OFFSET))
#endif

#if defined(_V1) && !defined(LC0_7_VAL)
#define LC0_7_VAL  PREFIX_VAL(LC0_7_REG)
#endif

#define DMA0_LC0_7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC0_7_OFFSET))
#define DMA0_LC0_7_VAL  PREFIX_VAL(DMA0_LC0_7_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC0_7_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC0_7_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC0_7_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC0_7_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC0_7_LOOP_COUNTER_ITERATIONS_R)
#define LC0_7_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_7_VAL),DMA_LC0_7_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_7_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC0_7_LOOP_COUNTER_ITERATIONS_W)
#define LC0_7_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_7_VAL),DMA_LC0_7_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_7_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC0_7_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC0_7_VAL),DMA_LC0_7_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_7_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC0_7_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC0_7_VAL),DMA_LC0_7_LOOP_COUNTER_ITERATIONS_POS, DMA_LC0_7_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: LC1_7 ACCESS: RO */

#if defined(_V1) && !defined(LC1_7_OFFSET)
#define LC1_7_OFFSET 0x4F0
#endif

#if !defined(DMA_LC1_7_OFFSET)
#define DMA_LC1_7_OFFSET 0x4F0
#endif

#if defined(_V1) && !defined(LC1_7_REG)
#define LC1_7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_7_OFFSET))
#endif

#if defined(_V1) && !defined(LC1_7_VAL)
#define LC1_7_VAL  PREFIX_VAL(LC1_7_REG)
#endif

#define DMA0_LC1_7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_LC1_7_OFFSET))
#define DMA0_LC1_7_VAL  PREFIX_VAL(DMA0_LC1_7_REG)

/* FIELDS: */

/* Loop_counter_iterations ACCESS: RO */

#ifndef DMA_LC1_7_LOOP_COUNTER_ITERATIONS_POS
#define DMA_LC1_7_LOOP_COUNTER_ITERATIONS_POS      0
#endif

#ifndef DMA_LC1_7_LOOP_COUNTER_ITERATIONS_LEN
#define DMA_LC1_7_LOOP_COUNTER_ITERATIONS_LEN      8
#endif

#if defined(_V1) && !defined(LC1_7_LOOP_COUNTER_ITERATIONS_R)
#define LC1_7_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_7_VAL),DMA_LC1_7_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_7_LOOP_COUNTER_ITERATIONS_LEN)
#endif

#if defined(_V1) && !defined(LC1_7_LOOP_COUNTER_ITERATIONS_W)
#define LC1_7_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_7_VAL),DMA_LC1_7_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_7_LOOP_COUNTER_ITERATIONS_LEN,value)
#endif

#define DMA0_LC1_7_LOOP_COUNTER_ITERATIONS_R        GetGroupBits32( (DMA0_LC1_7_VAL),DMA_LC1_7_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_7_LOOP_COUNTER_ITERATIONS_LEN)

#define DMA0_LC1_7_LOOP_COUNTER_ITERATIONS_W(value) SetGroupBits32( (DMA0_LC1_7_VAL),DMA_LC1_7_LOOP_COUNTER_ITERATIONS_POS, DMA_LC1_7_LOOP_COUNTER_ITERATIONS_LEN,value)


/* REGISTER: FTR4 ACCESS: RO */

#if defined(_V1) && !defined(FTR4_OFFSET)
#define FTR4_OFFSET 0x50
#endif

#if !defined(DMA_FTR4_OFFSET)
#define DMA_FTR4_OFFSET 0x50
#endif

#if defined(_V1) && !defined(FTR4_REG)
#define FTR4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR4_OFFSET))
#endif

#if defined(_V1) && !defined(FTR4_VAL)
#define FTR4_VAL  PREFIX_VAL(FTR4_REG)
#endif

#define DMA0_FTR4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR4_OFFSET))
#define DMA0_FTR4_VAL  PREFIX_VAL(DMA0_FTR4_REG)

/* FIELDS: */

/* undef_instr ACCESS: RO */

#ifndef DMA_FTR4_UNDEF_INSTR_POS
#define DMA_FTR4_UNDEF_INSTR_POS      0
#endif

#ifndef DMA_FTR4_UNDEF_INSTR_LEN
#define DMA_FTR4_UNDEF_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR4_UNDEF_INSTR_R)
#define FTR4_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_UNDEF_INSTR_POS, DMA_FTR4_UNDEF_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR4_UNDEF_INSTR_W)
#define FTR4_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_UNDEF_INSTR_POS, DMA_FTR4_UNDEF_INSTR_LEN,value)
#endif

#define DMA0_FTR4_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_UNDEF_INSTR_POS, DMA_FTR4_UNDEF_INSTR_LEN)

#define DMA0_FTR4_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_UNDEF_INSTR_POS, DMA_FTR4_UNDEF_INSTR_LEN,value)


/* operand_invalid ACCESS: RO */

#ifndef DMA_FTR4_OPERAND_INVALID_POS
#define DMA_FTR4_OPERAND_INVALID_POS      1
#endif

#ifndef DMA_FTR4_OPERAND_INVALID_LEN
#define DMA_FTR4_OPERAND_INVALID_LEN      1
#endif

#if defined(_V1) && !defined(FTR4_OPERAND_INVALID_R)
#define FTR4_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_OPERAND_INVALID_POS, DMA_FTR4_OPERAND_INVALID_LEN)
#endif

#if defined(_V1) && !defined(FTR4_OPERAND_INVALID_W)
#define FTR4_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_OPERAND_INVALID_POS, DMA_FTR4_OPERAND_INVALID_LEN,value)
#endif

#define DMA0_FTR4_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_OPERAND_INVALID_POS, DMA_FTR4_OPERAND_INVALID_LEN)

#define DMA0_FTR4_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_OPERAND_INVALID_POS, DMA_FTR4_OPERAND_INVALID_LEN,value)


/* ch_evnt_err ACCESS: RO */

#ifndef DMA_FTR4_CH_EVNT_ERR_POS
#define DMA_FTR4_CH_EVNT_ERR_POS      5
#endif

#ifndef DMA_FTR4_CH_EVNT_ERR_LEN
#define DMA_FTR4_CH_EVNT_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR4_CH_EVNT_ERR_R)
#define FTR4_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_CH_EVNT_ERR_POS, DMA_FTR4_CH_EVNT_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR4_CH_EVNT_ERR_W)
#define FTR4_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_CH_EVNT_ERR_POS, DMA_FTR4_CH_EVNT_ERR_LEN,value)
#endif

#define DMA0_FTR4_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_CH_EVNT_ERR_POS, DMA_FTR4_CH_EVNT_ERR_LEN)

#define DMA0_FTR4_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_CH_EVNT_ERR_POS, DMA_FTR4_CH_EVNT_ERR_LEN,value)


/* ch_periph_err ACCESS: RO */

#ifndef DMA_FTR4_CH_PERIPH_ERR_POS
#define DMA_FTR4_CH_PERIPH_ERR_POS      6
#endif

#ifndef DMA_FTR4_CH_PERIPH_ERR_LEN
#define DMA_FTR4_CH_PERIPH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR4_CH_PERIPH_ERR_R)
#define FTR4_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_CH_PERIPH_ERR_POS, DMA_FTR4_CH_PERIPH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR4_CH_PERIPH_ERR_W)
#define FTR4_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_CH_PERIPH_ERR_POS, DMA_FTR4_CH_PERIPH_ERR_LEN,value)
#endif

#define DMA0_FTR4_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_CH_PERIPH_ERR_POS, DMA_FTR4_CH_PERIPH_ERR_LEN)

#define DMA0_FTR4_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_CH_PERIPH_ERR_POS, DMA_FTR4_CH_PERIPH_ERR_LEN,value)


/* ch_rdwr_err ACCESS: RO */

#ifndef DMA_FTR4_CH_RDWR_ERR_POS
#define DMA_FTR4_CH_RDWR_ERR_POS      7
#endif

#ifndef DMA_FTR4_CH_RDWR_ERR_LEN
#define DMA_FTR4_CH_RDWR_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR4_CH_RDWR_ERR_R)
#define FTR4_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_CH_RDWR_ERR_POS, DMA_FTR4_CH_RDWR_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR4_CH_RDWR_ERR_W)
#define FTR4_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_CH_RDWR_ERR_POS, DMA_FTR4_CH_RDWR_ERR_LEN,value)
#endif

#define DMA0_FTR4_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_CH_RDWR_ERR_POS, DMA_FTR4_CH_RDWR_ERR_LEN)

#define DMA0_FTR4_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_CH_RDWR_ERR_POS, DMA_FTR4_CH_RDWR_ERR_LEN,value)


/* mfifo_err ACCESS: RO */

#ifndef DMA_FTR4_MFIFO_ERR_POS
#define DMA_FTR4_MFIFO_ERR_POS      12
#endif

#ifndef DMA_FTR4_MFIFO_ERR_LEN
#define DMA_FTR4_MFIFO_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR4_MFIFO_ERR_R)
#define FTR4_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_MFIFO_ERR_POS, DMA_FTR4_MFIFO_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR4_MFIFO_ERR_W)
#define FTR4_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_MFIFO_ERR_POS, DMA_FTR4_MFIFO_ERR_LEN,value)
#endif

#define DMA0_FTR4_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_MFIFO_ERR_POS, DMA_FTR4_MFIFO_ERR_LEN)

#define DMA0_FTR4_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_MFIFO_ERR_POS, DMA_FTR4_MFIFO_ERR_LEN,value)


/* st_data_unavailable ACCESS: RO */

#ifndef DMA_FTR4_ST_DATA_UNAVAILABLE_POS
#define DMA_FTR4_ST_DATA_UNAVAILABLE_POS      13
#endif

#ifndef DMA_FTR4_ST_DATA_UNAVAILABLE_LEN
#define DMA_FTR4_ST_DATA_UNAVAILABLE_LEN      1
#endif

#if defined(_V1) && !defined(FTR4_ST_DATA_UNAVAILABLE_R)
#define FTR4_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_ST_DATA_UNAVAILABLE_POS, DMA_FTR4_ST_DATA_UNAVAILABLE_LEN)
#endif

#if defined(_V1) && !defined(FTR4_ST_DATA_UNAVAILABLE_W)
#define FTR4_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_ST_DATA_UNAVAILABLE_POS, DMA_FTR4_ST_DATA_UNAVAILABLE_LEN,value)
#endif

#define DMA0_FTR4_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_ST_DATA_UNAVAILABLE_POS, DMA_FTR4_ST_DATA_UNAVAILABLE_LEN)

#define DMA0_FTR4_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_ST_DATA_UNAVAILABLE_POS, DMA_FTR4_ST_DATA_UNAVAILABLE_LEN,value)


/* instr_fetch_err ACCESS: RO */

#ifndef DMA_FTR4_INSTR_FETCH_ERR_POS
#define DMA_FTR4_INSTR_FETCH_ERR_POS      16
#endif

#ifndef DMA_FTR4_INSTR_FETCH_ERR_LEN
#define DMA_FTR4_INSTR_FETCH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR4_INSTR_FETCH_ERR_R)
#define FTR4_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_INSTR_FETCH_ERR_POS, DMA_FTR4_INSTR_FETCH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR4_INSTR_FETCH_ERR_W)
#define FTR4_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_INSTR_FETCH_ERR_POS, DMA_FTR4_INSTR_FETCH_ERR_LEN,value)
#endif

#define DMA0_FTR4_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_INSTR_FETCH_ERR_POS, DMA_FTR4_INSTR_FETCH_ERR_LEN)

#define DMA0_FTR4_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_INSTR_FETCH_ERR_POS, DMA_FTR4_INSTR_FETCH_ERR_LEN,value)


/* data_write_err ACCESS: RO */

#ifndef DMA_FTR4_DATA_WRITE_ERR_POS
#define DMA_FTR4_DATA_WRITE_ERR_POS      17
#endif

#ifndef DMA_FTR4_DATA_WRITE_ERR_LEN
#define DMA_FTR4_DATA_WRITE_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR4_DATA_WRITE_ERR_R)
#define FTR4_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_DATA_WRITE_ERR_POS, DMA_FTR4_DATA_WRITE_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR4_DATA_WRITE_ERR_W)
#define FTR4_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_DATA_WRITE_ERR_POS, DMA_FTR4_DATA_WRITE_ERR_LEN,value)
#endif

#define DMA0_FTR4_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_DATA_WRITE_ERR_POS, DMA_FTR4_DATA_WRITE_ERR_LEN)

#define DMA0_FTR4_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_DATA_WRITE_ERR_POS, DMA_FTR4_DATA_WRITE_ERR_LEN,value)


/* data_read_err ACCESS: RO */

#ifndef DMA_FTR4_DATA_READ_ERR_POS
#define DMA_FTR4_DATA_READ_ERR_POS      18
#endif

#ifndef DMA_FTR4_DATA_READ_ERR_LEN
#define DMA_FTR4_DATA_READ_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR4_DATA_READ_ERR_R)
#define FTR4_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_DATA_READ_ERR_POS, DMA_FTR4_DATA_READ_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR4_DATA_READ_ERR_W)
#define FTR4_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_DATA_READ_ERR_POS, DMA_FTR4_DATA_READ_ERR_LEN,value)
#endif

#define DMA0_FTR4_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_DATA_READ_ERR_POS, DMA_FTR4_DATA_READ_ERR_LEN)

#define DMA0_FTR4_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_DATA_READ_ERR_POS, DMA_FTR4_DATA_READ_ERR_LEN,value)


/* dbg_instr ACCESS: RO */

#ifndef DMA_FTR4_DBG_INSTR_POS
#define DMA_FTR4_DBG_INSTR_POS      30
#endif

#ifndef DMA_FTR4_DBG_INSTR_LEN
#define DMA_FTR4_DBG_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR4_DBG_INSTR_R)
#define FTR4_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_DBG_INSTR_POS, DMA_FTR4_DBG_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR4_DBG_INSTR_W)
#define FTR4_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_DBG_INSTR_POS, DMA_FTR4_DBG_INSTR_LEN,value)
#endif

#define DMA0_FTR4_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_DBG_INSTR_POS, DMA_FTR4_DBG_INSTR_LEN)

#define DMA0_FTR4_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_DBG_INSTR_POS, DMA_FTR4_DBG_INSTR_LEN,value)


/* lockup_err ACCESS: RO */

#ifndef DMA_FTR4_LOCKUP_ERR_POS
#define DMA_FTR4_LOCKUP_ERR_POS      31
#endif

#ifndef DMA_FTR4_LOCKUP_ERR_LEN
#define DMA_FTR4_LOCKUP_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR4_LOCKUP_ERR_R)
#define FTR4_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_LOCKUP_ERR_POS, DMA_FTR4_LOCKUP_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR4_LOCKUP_ERR_W)
#define FTR4_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_LOCKUP_ERR_POS, DMA_FTR4_LOCKUP_ERR_LEN,value)
#endif

#define DMA0_FTR4_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_LOCKUP_ERR_POS, DMA_FTR4_LOCKUP_ERR_LEN)

#define DMA0_FTR4_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR4_VAL),DMA_FTR4_LOCKUP_ERR_POS, DMA_FTR4_LOCKUP_ERR_LEN,value)


/* REGISTER: FTR5 ACCESS: RO */

#if defined(_V1) && !defined(FTR5_OFFSET)
#define FTR5_OFFSET 0x54
#endif

#if !defined(DMA_FTR5_OFFSET)
#define DMA_FTR5_OFFSET 0x54
#endif

#if defined(_V1) && !defined(FTR5_REG)
#define FTR5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR5_OFFSET))
#endif

#if defined(_V1) && !defined(FTR5_VAL)
#define FTR5_VAL  PREFIX_VAL(FTR5_REG)
#endif

#define DMA0_FTR5_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR5_OFFSET))
#define DMA0_FTR5_VAL  PREFIX_VAL(DMA0_FTR5_REG)

/* FIELDS: */

/* undef_instr ACCESS: RO */

#ifndef DMA_FTR5_UNDEF_INSTR_POS
#define DMA_FTR5_UNDEF_INSTR_POS      0
#endif

#ifndef DMA_FTR5_UNDEF_INSTR_LEN
#define DMA_FTR5_UNDEF_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR5_UNDEF_INSTR_R)
#define FTR5_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_UNDEF_INSTR_POS, DMA_FTR5_UNDEF_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR5_UNDEF_INSTR_W)
#define FTR5_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_UNDEF_INSTR_POS, DMA_FTR5_UNDEF_INSTR_LEN,value)
#endif

#define DMA0_FTR5_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_UNDEF_INSTR_POS, DMA_FTR5_UNDEF_INSTR_LEN)

#define DMA0_FTR5_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_UNDEF_INSTR_POS, DMA_FTR5_UNDEF_INSTR_LEN,value)


/* operand_invalid ACCESS: RO */

#ifndef DMA_FTR5_OPERAND_INVALID_POS
#define DMA_FTR5_OPERAND_INVALID_POS      1
#endif

#ifndef DMA_FTR5_OPERAND_INVALID_LEN
#define DMA_FTR5_OPERAND_INVALID_LEN      1
#endif

#if defined(_V1) && !defined(FTR5_OPERAND_INVALID_R)
#define FTR5_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_OPERAND_INVALID_POS, DMA_FTR5_OPERAND_INVALID_LEN)
#endif

#if defined(_V1) && !defined(FTR5_OPERAND_INVALID_W)
#define FTR5_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_OPERAND_INVALID_POS, DMA_FTR5_OPERAND_INVALID_LEN,value)
#endif

#define DMA0_FTR5_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_OPERAND_INVALID_POS, DMA_FTR5_OPERAND_INVALID_LEN)

#define DMA0_FTR5_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_OPERAND_INVALID_POS, DMA_FTR5_OPERAND_INVALID_LEN,value)


/* ch_evnt_err ACCESS: RO */

#ifndef DMA_FTR5_CH_EVNT_ERR_POS
#define DMA_FTR5_CH_EVNT_ERR_POS      5
#endif

#ifndef DMA_FTR5_CH_EVNT_ERR_LEN
#define DMA_FTR5_CH_EVNT_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR5_CH_EVNT_ERR_R)
#define FTR5_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_CH_EVNT_ERR_POS, DMA_FTR5_CH_EVNT_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR5_CH_EVNT_ERR_W)
#define FTR5_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_CH_EVNT_ERR_POS, DMA_FTR5_CH_EVNT_ERR_LEN,value)
#endif

#define DMA0_FTR5_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_CH_EVNT_ERR_POS, DMA_FTR5_CH_EVNT_ERR_LEN)

#define DMA0_FTR5_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_CH_EVNT_ERR_POS, DMA_FTR5_CH_EVNT_ERR_LEN,value)


/* ch_periph_err ACCESS: RO */

#ifndef DMA_FTR5_CH_PERIPH_ERR_POS
#define DMA_FTR5_CH_PERIPH_ERR_POS      6
#endif

#ifndef DMA_FTR5_CH_PERIPH_ERR_LEN
#define DMA_FTR5_CH_PERIPH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR5_CH_PERIPH_ERR_R)
#define FTR5_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_CH_PERIPH_ERR_POS, DMA_FTR5_CH_PERIPH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR5_CH_PERIPH_ERR_W)
#define FTR5_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_CH_PERIPH_ERR_POS, DMA_FTR5_CH_PERIPH_ERR_LEN,value)
#endif

#define DMA0_FTR5_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_CH_PERIPH_ERR_POS, DMA_FTR5_CH_PERIPH_ERR_LEN)

#define DMA0_FTR5_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_CH_PERIPH_ERR_POS, DMA_FTR5_CH_PERIPH_ERR_LEN,value)


/* ch_rdwr_err ACCESS: RO */

#ifndef DMA_FTR5_CH_RDWR_ERR_POS
#define DMA_FTR5_CH_RDWR_ERR_POS      7
#endif

#ifndef DMA_FTR5_CH_RDWR_ERR_LEN
#define DMA_FTR5_CH_RDWR_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR5_CH_RDWR_ERR_R)
#define FTR5_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_CH_RDWR_ERR_POS, DMA_FTR5_CH_RDWR_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR5_CH_RDWR_ERR_W)
#define FTR5_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_CH_RDWR_ERR_POS, DMA_FTR5_CH_RDWR_ERR_LEN,value)
#endif

#define DMA0_FTR5_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_CH_RDWR_ERR_POS, DMA_FTR5_CH_RDWR_ERR_LEN)

#define DMA0_FTR5_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_CH_RDWR_ERR_POS, DMA_FTR5_CH_RDWR_ERR_LEN,value)


/* mfifo_err ACCESS: RO */

#ifndef DMA_FTR5_MFIFO_ERR_POS
#define DMA_FTR5_MFIFO_ERR_POS      12
#endif

#ifndef DMA_FTR5_MFIFO_ERR_LEN
#define DMA_FTR5_MFIFO_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR5_MFIFO_ERR_R)
#define FTR5_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_MFIFO_ERR_POS, DMA_FTR5_MFIFO_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR5_MFIFO_ERR_W)
#define FTR5_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_MFIFO_ERR_POS, DMA_FTR5_MFIFO_ERR_LEN,value)
#endif

#define DMA0_FTR5_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_MFIFO_ERR_POS, DMA_FTR5_MFIFO_ERR_LEN)

#define DMA0_FTR5_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_MFIFO_ERR_POS, DMA_FTR5_MFIFO_ERR_LEN,value)


/* st_data_unavailable ACCESS: RO */

#ifndef DMA_FTR5_ST_DATA_UNAVAILABLE_POS
#define DMA_FTR5_ST_DATA_UNAVAILABLE_POS      13
#endif

#ifndef DMA_FTR5_ST_DATA_UNAVAILABLE_LEN
#define DMA_FTR5_ST_DATA_UNAVAILABLE_LEN      1
#endif

#if defined(_V1) && !defined(FTR5_ST_DATA_UNAVAILABLE_R)
#define FTR5_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_ST_DATA_UNAVAILABLE_POS, DMA_FTR5_ST_DATA_UNAVAILABLE_LEN)
#endif

#if defined(_V1) && !defined(FTR5_ST_DATA_UNAVAILABLE_W)
#define FTR5_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_ST_DATA_UNAVAILABLE_POS, DMA_FTR5_ST_DATA_UNAVAILABLE_LEN,value)
#endif

#define DMA0_FTR5_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_ST_DATA_UNAVAILABLE_POS, DMA_FTR5_ST_DATA_UNAVAILABLE_LEN)

#define DMA0_FTR5_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_ST_DATA_UNAVAILABLE_POS, DMA_FTR5_ST_DATA_UNAVAILABLE_LEN,value)


/* instr_fetch_err ACCESS: RO */

#ifndef DMA_FTR5_INSTR_FETCH_ERR_POS
#define DMA_FTR5_INSTR_FETCH_ERR_POS      16
#endif

#ifndef DMA_FTR5_INSTR_FETCH_ERR_LEN
#define DMA_FTR5_INSTR_FETCH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR5_INSTR_FETCH_ERR_R)
#define FTR5_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_INSTR_FETCH_ERR_POS, DMA_FTR5_INSTR_FETCH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR5_INSTR_FETCH_ERR_W)
#define FTR5_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_INSTR_FETCH_ERR_POS, DMA_FTR5_INSTR_FETCH_ERR_LEN,value)
#endif

#define DMA0_FTR5_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_INSTR_FETCH_ERR_POS, DMA_FTR5_INSTR_FETCH_ERR_LEN)

#define DMA0_FTR5_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_INSTR_FETCH_ERR_POS, DMA_FTR5_INSTR_FETCH_ERR_LEN,value)


/* data_write_err ACCESS: RO */

#ifndef DMA_FTR5_DATA_WRITE_ERR_POS
#define DMA_FTR5_DATA_WRITE_ERR_POS      17
#endif

#ifndef DMA_FTR5_DATA_WRITE_ERR_LEN
#define DMA_FTR5_DATA_WRITE_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR5_DATA_WRITE_ERR_R)
#define FTR5_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_DATA_WRITE_ERR_POS, DMA_FTR5_DATA_WRITE_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR5_DATA_WRITE_ERR_W)
#define FTR5_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_DATA_WRITE_ERR_POS, DMA_FTR5_DATA_WRITE_ERR_LEN,value)
#endif

#define DMA0_FTR5_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_DATA_WRITE_ERR_POS, DMA_FTR5_DATA_WRITE_ERR_LEN)

#define DMA0_FTR5_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_DATA_WRITE_ERR_POS, DMA_FTR5_DATA_WRITE_ERR_LEN,value)


/* data_read_err ACCESS: RO */

#ifndef DMA_FTR5_DATA_READ_ERR_POS
#define DMA_FTR5_DATA_READ_ERR_POS      18
#endif

#ifndef DMA_FTR5_DATA_READ_ERR_LEN
#define DMA_FTR5_DATA_READ_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR5_DATA_READ_ERR_R)
#define FTR5_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_DATA_READ_ERR_POS, DMA_FTR5_DATA_READ_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR5_DATA_READ_ERR_W)
#define FTR5_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_DATA_READ_ERR_POS, DMA_FTR5_DATA_READ_ERR_LEN,value)
#endif

#define DMA0_FTR5_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_DATA_READ_ERR_POS, DMA_FTR5_DATA_READ_ERR_LEN)

#define DMA0_FTR5_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_DATA_READ_ERR_POS, DMA_FTR5_DATA_READ_ERR_LEN,value)


/* dbg_instr ACCESS: RO */

#ifndef DMA_FTR5_DBG_INSTR_POS
#define DMA_FTR5_DBG_INSTR_POS      30
#endif

#ifndef DMA_FTR5_DBG_INSTR_LEN
#define DMA_FTR5_DBG_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR5_DBG_INSTR_R)
#define FTR5_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_DBG_INSTR_POS, DMA_FTR5_DBG_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR5_DBG_INSTR_W)
#define FTR5_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_DBG_INSTR_POS, DMA_FTR5_DBG_INSTR_LEN,value)
#endif

#define DMA0_FTR5_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_DBG_INSTR_POS, DMA_FTR5_DBG_INSTR_LEN)

#define DMA0_FTR5_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_DBG_INSTR_POS, DMA_FTR5_DBG_INSTR_LEN,value)


/* lockup_err ACCESS: RO */

#ifndef DMA_FTR5_LOCKUP_ERR_POS
#define DMA_FTR5_LOCKUP_ERR_POS      31
#endif

#ifndef DMA_FTR5_LOCKUP_ERR_LEN
#define DMA_FTR5_LOCKUP_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR5_LOCKUP_ERR_R)
#define FTR5_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_LOCKUP_ERR_POS, DMA_FTR5_LOCKUP_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR5_LOCKUP_ERR_W)
#define FTR5_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_LOCKUP_ERR_POS, DMA_FTR5_LOCKUP_ERR_LEN,value)
#endif

#define DMA0_FTR5_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_LOCKUP_ERR_POS, DMA_FTR5_LOCKUP_ERR_LEN)

#define DMA0_FTR5_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR5_VAL),DMA_FTR5_LOCKUP_ERR_POS, DMA_FTR5_LOCKUP_ERR_LEN,value)


/* REGISTER: FTR6 ACCESS: RO */

#if defined(_V1) && !defined(FTR6_OFFSET)
#define FTR6_OFFSET 0x58
#endif

#if !defined(DMA_FTR6_OFFSET)
#define DMA_FTR6_OFFSET 0x58
#endif

#if defined(_V1) && !defined(FTR6_REG)
#define FTR6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR6_OFFSET))
#endif

#if defined(_V1) && !defined(FTR6_VAL)
#define FTR6_VAL  PREFIX_VAL(FTR6_REG)
#endif

#define DMA0_FTR6_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR6_OFFSET))
#define DMA0_FTR6_VAL  PREFIX_VAL(DMA0_FTR6_REG)

/* FIELDS: */

/* undef_instr ACCESS: RO */

#ifndef DMA_FTR6_UNDEF_INSTR_POS
#define DMA_FTR6_UNDEF_INSTR_POS      0
#endif

#ifndef DMA_FTR6_UNDEF_INSTR_LEN
#define DMA_FTR6_UNDEF_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR6_UNDEF_INSTR_R)
#define FTR6_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_UNDEF_INSTR_POS, DMA_FTR6_UNDEF_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR6_UNDEF_INSTR_W)
#define FTR6_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_UNDEF_INSTR_POS, DMA_FTR6_UNDEF_INSTR_LEN,value)
#endif

#define DMA0_FTR6_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_UNDEF_INSTR_POS, DMA_FTR6_UNDEF_INSTR_LEN)

#define DMA0_FTR6_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_UNDEF_INSTR_POS, DMA_FTR6_UNDEF_INSTR_LEN,value)


/* operand_invalid ACCESS: RO */

#ifndef DMA_FTR6_OPERAND_INVALID_POS
#define DMA_FTR6_OPERAND_INVALID_POS      1
#endif

#ifndef DMA_FTR6_OPERAND_INVALID_LEN
#define DMA_FTR6_OPERAND_INVALID_LEN      1
#endif

#if defined(_V1) && !defined(FTR6_OPERAND_INVALID_R)
#define FTR6_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_OPERAND_INVALID_POS, DMA_FTR6_OPERAND_INVALID_LEN)
#endif

#if defined(_V1) && !defined(FTR6_OPERAND_INVALID_W)
#define FTR6_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_OPERAND_INVALID_POS, DMA_FTR6_OPERAND_INVALID_LEN,value)
#endif

#define DMA0_FTR6_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_OPERAND_INVALID_POS, DMA_FTR6_OPERAND_INVALID_LEN)

#define DMA0_FTR6_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_OPERAND_INVALID_POS, DMA_FTR6_OPERAND_INVALID_LEN,value)


/* ch_evnt_err ACCESS: RO */

#ifndef DMA_FTR6_CH_EVNT_ERR_POS
#define DMA_FTR6_CH_EVNT_ERR_POS      5
#endif

#ifndef DMA_FTR6_CH_EVNT_ERR_LEN
#define DMA_FTR6_CH_EVNT_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR6_CH_EVNT_ERR_R)
#define FTR6_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_CH_EVNT_ERR_POS, DMA_FTR6_CH_EVNT_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR6_CH_EVNT_ERR_W)
#define FTR6_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_CH_EVNT_ERR_POS, DMA_FTR6_CH_EVNT_ERR_LEN,value)
#endif

#define DMA0_FTR6_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_CH_EVNT_ERR_POS, DMA_FTR6_CH_EVNT_ERR_LEN)

#define DMA0_FTR6_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_CH_EVNT_ERR_POS, DMA_FTR6_CH_EVNT_ERR_LEN,value)


/* ch_periph_err ACCESS: RO */

#ifndef DMA_FTR6_CH_PERIPH_ERR_POS
#define DMA_FTR6_CH_PERIPH_ERR_POS      6
#endif

#ifndef DMA_FTR6_CH_PERIPH_ERR_LEN
#define DMA_FTR6_CH_PERIPH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR6_CH_PERIPH_ERR_R)
#define FTR6_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_CH_PERIPH_ERR_POS, DMA_FTR6_CH_PERIPH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR6_CH_PERIPH_ERR_W)
#define FTR6_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_CH_PERIPH_ERR_POS, DMA_FTR6_CH_PERIPH_ERR_LEN,value)
#endif

#define DMA0_FTR6_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_CH_PERIPH_ERR_POS, DMA_FTR6_CH_PERIPH_ERR_LEN)

#define DMA0_FTR6_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_CH_PERIPH_ERR_POS, DMA_FTR6_CH_PERIPH_ERR_LEN,value)


/* ch_rdwr_err ACCESS: RO */

#ifndef DMA_FTR6_CH_RDWR_ERR_POS
#define DMA_FTR6_CH_RDWR_ERR_POS      7
#endif

#ifndef DMA_FTR6_CH_RDWR_ERR_LEN
#define DMA_FTR6_CH_RDWR_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR6_CH_RDWR_ERR_R)
#define FTR6_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_CH_RDWR_ERR_POS, DMA_FTR6_CH_RDWR_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR6_CH_RDWR_ERR_W)
#define FTR6_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_CH_RDWR_ERR_POS, DMA_FTR6_CH_RDWR_ERR_LEN,value)
#endif

#define DMA0_FTR6_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_CH_RDWR_ERR_POS, DMA_FTR6_CH_RDWR_ERR_LEN)

#define DMA0_FTR6_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_CH_RDWR_ERR_POS, DMA_FTR6_CH_RDWR_ERR_LEN,value)


/* mfifo_err ACCESS: RO */

#ifndef DMA_FTR6_MFIFO_ERR_POS
#define DMA_FTR6_MFIFO_ERR_POS      12
#endif

#ifndef DMA_FTR6_MFIFO_ERR_LEN
#define DMA_FTR6_MFIFO_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR6_MFIFO_ERR_R)
#define FTR6_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_MFIFO_ERR_POS, DMA_FTR6_MFIFO_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR6_MFIFO_ERR_W)
#define FTR6_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_MFIFO_ERR_POS, DMA_FTR6_MFIFO_ERR_LEN,value)
#endif

#define DMA0_FTR6_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_MFIFO_ERR_POS, DMA_FTR6_MFIFO_ERR_LEN)

#define DMA0_FTR6_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_MFIFO_ERR_POS, DMA_FTR6_MFIFO_ERR_LEN,value)


/* st_data_unavailable ACCESS: RO */

#ifndef DMA_FTR6_ST_DATA_UNAVAILABLE_POS
#define DMA_FTR6_ST_DATA_UNAVAILABLE_POS      13
#endif

#ifndef DMA_FTR6_ST_DATA_UNAVAILABLE_LEN
#define DMA_FTR6_ST_DATA_UNAVAILABLE_LEN      1
#endif

#if defined(_V1) && !defined(FTR6_ST_DATA_UNAVAILABLE_R)
#define FTR6_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_ST_DATA_UNAVAILABLE_POS, DMA_FTR6_ST_DATA_UNAVAILABLE_LEN)
#endif

#if defined(_V1) && !defined(FTR6_ST_DATA_UNAVAILABLE_W)
#define FTR6_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_ST_DATA_UNAVAILABLE_POS, DMA_FTR6_ST_DATA_UNAVAILABLE_LEN,value)
#endif

#define DMA0_FTR6_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_ST_DATA_UNAVAILABLE_POS, DMA_FTR6_ST_DATA_UNAVAILABLE_LEN)

#define DMA0_FTR6_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_ST_DATA_UNAVAILABLE_POS, DMA_FTR6_ST_DATA_UNAVAILABLE_LEN,value)


/* instr_fetch_err ACCESS: RO */

#ifndef DMA_FTR6_INSTR_FETCH_ERR_POS
#define DMA_FTR6_INSTR_FETCH_ERR_POS      16
#endif

#ifndef DMA_FTR6_INSTR_FETCH_ERR_LEN
#define DMA_FTR6_INSTR_FETCH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR6_INSTR_FETCH_ERR_R)
#define FTR6_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_INSTR_FETCH_ERR_POS, DMA_FTR6_INSTR_FETCH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR6_INSTR_FETCH_ERR_W)
#define FTR6_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_INSTR_FETCH_ERR_POS, DMA_FTR6_INSTR_FETCH_ERR_LEN,value)
#endif

#define DMA0_FTR6_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_INSTR_FETCH_ERR_POS, DMA_FTR6_INSTR_FETCH_ERR_LEN)

#define DMA0_FTR6_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_INSTR_FETCH_ERR_POS, DMA_FTR6_INSTR_FETCH_ERR_LEN,value)


/* data_write_err ACCESS: RO */

#ifndef DMA_FTR6_DATA_WRITE_ERR_POS
#define DMA_FTR6_DATA_WRITE_ERR_POS      17
#endif

#ifndef DMA_FTR6_DATA_WRITE_ERR_LEN
#define DMA_FTR6_DATA_WRITE_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR6_DATA_WRITE_ERR_R)
#define FTR6_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_DATA_WRITE_ERR_POS, DMA_FTR6_DATA_WRITE_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR6_DATA_WRITE_ERR_W)
#define FTR6_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_DATA_WRITE_ERR_POS, DMA_FTR6_DATA_WRITE_ERR_LEN,value)
#endif

#define DMA0_FTR6_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_DATA_WRITE_ERR_POS, DMA_FTR6_DATA_WRITE_ERR_LEN)

#define DMA0_FTR6_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_DATA_WRITE_ERR_POS, DMA_FTR6_DATA_WRITE_ERR_LEN,value)


/* data_read_err ACCESS: RO */

#ifndef DMA_FTR6_DATA_READ_ERR_POS
#define DMA_FTR6_DATA_READ_ERR_POS      18
#endif

#ifndef DMA_FTR6_DATA_READ_ERR_LEN
#define DMA_FTR6_DATA_READ_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR6_DATA_READ_ERR_R)
#define FTR6_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_DATA_READ_ERR_POS, DMA_FTR6_DATA_READ_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR6_DATA_READ_ERR_W)
#define FTR6_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_DATA_READ_ERR_POS, DMA_FTR6_DATA_READ_ERR_LEN,value)
#endif

#define DMA0_FTR6_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_DATA_READ_ERR_POS, DMA_FTR6_DATA_READ_ERR_LEN)

#define DMA0_FTR6_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_DATA_READ_ERR_POS, DMA_FTR6_DATA_READ_ERR_LEN,value)


/* dbg_instr ACCESS: RO */

#ifndef DMA_FTR6_DBG_INSTR_POS
#define DMA_FTR6_DBG_INSTR_POS      30
#endif

#ifndef DMA_FTR6_DBG_INSTR_LEN
#define DMA_FTR6_DBG_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR6_DBG_INSTR_R)
#define FTR6_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_DBG_INSTR_POS, DMA_FTR6_DBG_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR6_DBG_INSTR_W)
#define FTR6_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_DBG_INSTR_POS, DMA_FTR6_DBG_INSTR_LEN,value)
#endif

#define DMA0_FTR6_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_DBG_INSTR_POS, DMA_FTR6_DBG_INSTR_LEN)

#define DMA0_FTR6_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_DBG_INSTR_POS, DMA_FTR6_DBG_INSTR_LEN,value)


/* lockup_err ACCESS: RO */

#ifndef DMA_FTR6_LOCKUP_ERR_POS
#define DMA_FTR6_LOCKUP_ERR_POS      31
#endif

#ifndef DMA_FTR6_LOCKUP_ERR_LEN
#define DMA_FTR6_LOCKUP_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR6_LOCKUP_ERR_R)
#define FTR6_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_LOCKUP_ERR_POS, DMA_FTR6_LOCKUP_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR6_LOCKUP_ERR_W)
#define FTR6_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_LOCKUP_ERR_POS, DMA_FTR6_LOCKUP_ERR_LEN,value)
#endif

#define DMA0_FTR6_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_LOCKUP_ERR_POS, DMA_FTR6_LOCKUP_ERR_LEN)

#define DMA0_FTR6_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR6_VAL),DMA_FTR6_LOCKUP_ERR_POS, DMA_FTR6_LOCKUP_ERR_LEN,value)


/* REGISTER: FTR7 ACCESS: RO */

#if defined(_V1) && !defined(FTR7_OFFSET)
#define FTR7_OFFSET 0x5C
#endif

#if !defined(DMA_FTR7_OFFSET)
#define DMA_FTR7_OFFSET 0x5C
#endif

#if defined(_V1) && !defined(FTR7_REG)
#define FTR7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR7_OFFSET))
#endif

#if defined(_V1) && !defined(FTR7_VAL)
#define FTR7_VAL  PREFIX_VAL(FTR7_REG)
#endif

#define DMA0_FTR7_REG ((volatile UINT32 *) (DMA0_BASE + DMA_FTR7_OFFSET))
#define DMA0_FTR7_VAL  PREFIX_VAL(DMA0_FTR7_REG)

/* FIELDS: */

/* undef_instr ACCESS: RO */

#ifndef DMA_FTR7_UNDEF_INSTR_POS
#define DMA_FTR7_UNDEF_INSTR_POS      0
#endif

#ifndef DMA_FTR7_UNDEF_INSTR_LEN
#define DMA_FTR7_UNDEF_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR7_UNDEF_INSTR_R)
#define FTR7_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_UNDEF_INSTR_POS, DMA_FTR7_UNDEF_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR7_UNDEF_INSTR_W)
#define FTR7_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_UNDEF_INSTR_POS, DMA_FTR7_UNDEF_INSTR_LEN,value)
#endif

#define DMA0_FTR7_UNDEF_INSTR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_UNDEF_INSTR_POS, DMA_FTR7_UNDEF_INSTR_LEN)

#define DMA0_FTR7_UNDEF_INSTR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_UNDEF_INSTR_POS, DMA_FTR7_UNDEF_INSTR_LEN,value)


/* operand_invalid ACCESS: RO */

#ifndef DMA_FTR7_OPERAND_INVALID_POS
#define DMA_FTR7_OPERAND_INVALID_POS      1
#endif

#ifndef DMA_FTR7_OPERAND_INVALID_LEN
#define DMA_FTR7_OPERAND_INVALID_LEN      1
#endif

#if defined(_V1) && !defined(FTR7_OPERAND_INVALID_R)
#define FTR7_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_OPERAND_INVALID_POS, DMA_FTR7_OPERAND_INVALID_LEN)
#endif

#if defined(_V1) && !defined(FTR7_OPERAND_INVALID_W)
#define FTR7_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_OPERAND_INVALID_POS, DMA_FTR7_OPERAND_INVALID_LEN,value)
#endif

#define DMA0_FTR7_OPERAND_INVALID_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_OPERAND_INVALID_POS, DMA_FTR7_OPERAND_INVALID_LEN)

#define DMA0_FTR7_OPERAND_INVALID_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_OPERAND_INVALID_POS, DMA_FTR7_OPERAND_INVALID_LEN,value)


/* ch_evnt_err ACCESS: RO */

#ifndef DMA_FTR7_CH_EVNT_ERR_POS
#define DMA_FTR7_CH_EVNT_ERR_POS      5
#endif

#ifndef DMA_FTR7_CH_EVNT_ERR_LEN
#define DMA_FTR7_CH_EVNT_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR7_CH_EVNT_ERR_R)
#define FTR7_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_CH_EVNT_ERR_POS, DMA_FTR7_CH_EVNT_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR7_CH_EVNT_ERR_W)
#define FTR7_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_CH_EVNT_ERR_POS, DMA_FTR7_CH_EVNT_ERR_LEN,value)
#endif

#define DMA0_FTR7_CH_EVNT_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_CH_EVNT_ERR_POS, DMA_FTR7_CH_EVNT_ERR_LEN)

#define DMA0_FTR7_CH_EVNT_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_CH_EVNT_ERR_POS, DMA_FTR7_CH_EVNT_ERR_LEN,value)


/* ch_periph_err ACCESS: RO */

#ifndef DMA_FTR7_CH_PERIPH_ERR_POS
#define DMA_FTR7_CH_PERIPH_ERR_POS      6
#endif

#ifndef DMA_FTR7_CH_PERIPH_ERR_LEN
#define DMA_FTR7_CH_PERIPH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR7_CH_PERIPH_ERR_R)
#define FTR7_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_CH_PERIPH_ERR_POS, DMA_FTR7_CH_PERIPH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR7_CH_PERIPH_ERR_W)
#define FTR7_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_CH_PERIPH_ERR_POS, DMA_FTR7_CH_PERIPH_ERR_LEN,value)
#endif

#define DMA0_FTR7_CH_PERIPH_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_CH_PERIPH_ERR_POS, DMA_FTR7_CH_PERIPH_ERR_LEN)

#define DMA0_FTR7_CH_PERIPH_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_CH_PERIPH_ERR_POS, DMA_FTR7_CH_PERIPH_ERR_LEN,value)


/* ch_rdwr_err ACCESS: RO */

#ifndef DMA_FTR7_CH_RDWR_ERR_POS
#define DMA_FTR7_CH_RDWR_ERR_POS      7
#endif

#ifndef DMA_FTR7_CH_RDWR_ERR_LEN
#define DMA_FTR7_CH_RDWR_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR7_CH_RDWR_ERR_R)
#define FTR7_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_CH_RDWR_ERR_POS, DMA_FTR7_CH_RDWR_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR7_CH_RDWR_ERR_W)
#define FTR7_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_CH_RDWR_ERR_POS, DMA_FTR7_CH_RDWR_ERR_LEN,value)
#endif

#define DMA0_FTR7_CH_RDWR_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_CH_RDWR_ERR_POS, DMA_FTR7_CH_RDWR_ERR_LEN)

#define DMA0_FTR7_CH_RDWR_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_CH_RDWR_ERR_POS, DMA_FTR7_CH_RDWR_ERR_LEN,value)


/* mfifo_err ACCESS: RO */

#ifndef DMA_FTR7_MFIFO_ERR_POS
#define DMA_FTR7_MFIFO_ERR_POS      12
#endif

#ifndef DMA_FTR7_MFIFO_ERR_LEN
#define DMA_FTR7_MFIFO_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR7_MFIFO_ERR_R)
#define FTR7_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_MFIFO_ERR_POS, DMA_FTR7_MFIFO_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR7_MFIFO_ERR_W)
#define FTR7_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_MFIFO_ERR_POS, DMA_FTR7_MFIFO_ERR_LEN,value)
#endif

#define DMA0_FTR7_MFIFO_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_MFIFO_ERR_POS, DMA_FTR7_MFIFO_ERR_LEN)

#define DMA0_FTR7_MFIFO_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_MFIFO_ERR_POS, DMA_FTR7_MFIFO_ERR_LEN,value)


/* st_data_unavailable ACCESS: RO */

#ifndef DMA_FTR7_ST_DATA_UNAVAILABLE_POS
#define DMA_FTR7_ST_DATA_UNAVAILABLE_POS      13
#endif

#ifndef DMA_FTR7_ST_DATA_UNAVAILABLE_LEN
#define DMA_FTR7_ST_DATA_UNAVAILABLE_LEN      1
#endif

#if defined(_V1) && !defined(FTR7_ST_DATA_UNAVAILABLE_R)
#define FTR7_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_ST_DATA_UNAVAILABLE_POS, DMA_FTR7_ST_DATA_UNAVAILABLE_LEN)
#endif

#if defined(_V1) && !defined(FTR7_ST_DATA_UNAVAILABLE_W)
#define FTR7_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_ST_DATA_UNAVAILABLE_POS, DMA_FTR7_ST_DATA_UNAVAILABLE_LEN,value)
#endif

#define DMA0_FTR7_ST_DATA_UNAVAILABLE_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_ST_DATA_UNAVAILABLE_POS, DMA_FTR7_ST_DATA_UNAVAILABLE_LEN)

#define DMA0_FTR7_ST_DATA_UNAVAILABLE_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_ST_DATA_UNAVAILABLE_POS, DMA_FTR7_ST_DATA_UNAVAILABLE_LEN,value)


/* instr_fetch_err ACCESS: RO */

#ifndef DMA_FTR7_INSTR_FETCH_ERR_POS
#define DMA_FTR7_INSTR_FETCH_ERR_POS      16
#endif

#ifndef DMA_FTR7_INSTR_FETCH_ERR_LEN
#define DMA_FTR7_INSTR_FETCH_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR7_INSTR_FETCH_ERR_R)
#define FTR7_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_INSTR_FETCH_ERR_POS, DMA_FTR7_INSTR_FETCH_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR7_INSTR_FETCH_ERR_W)
#define FTR7_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_INSTR_FETCH_ERR_POS, DMA_FTR7_INSTR_FETCH_ERR_LEN,value)
#endif

#define DMA0_FTR7_INSTR_FETCH_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_INSTR_FETCH_ERR_POS, DMA_FTR7_INSTR_FETCH_ERR_LEN)

#define DMA0_FTR7_INSTR_FETCH_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_INSTR_FETCH_ERR_POS, DMA_FTR7_INSTR_FETCH_ERR_LEN,value)


/* data_write_err ACCESS: RO */

#ifndef DMA_FTR7_DATA_WRITE_ERR_POS
#define DMA_FTR7_DATA_WRITE_ERR_POS      17
#endif

#ifndef DMA_FTR7_DATA_WRITE_ERR_LEN
#define DMA_FTR7_DATA_WRITE_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR7_DATA_WRITE_ERR_R)
#define FTR7_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_DATA_WRITE_ERR_POS, DMA_FTR7_DATA_WRITE_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR7_DATA_WRITE_ERR_W)
#define FTR7_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_DATA_WRITE_ERR_POS, DMA_FTR7_DATA_WRITE_ERR_LEN,value)
#endif

#define DMA0_FTR7_DATA_WRITE_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_DATA_WRITE_ERR_POS, DMA_FTR7_DATA_WRITE_ERR_LEN)

#define DMA0_FTR7_DATA_WRITE_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_DATA_WRITE_ERR_POS, DMA_FTR7_DATA_WRITE_ERR_LEN,value)


/* data_read_err ACCESS: RO */

#ifndef DMA_FTR7_DATA_READ_ERR_POS
#define DMA_FTR7_DATA_READ_ERR_POS      18
#endif

#ifndef DMA_FTR7_DATA_READ_ERR_LEN
#define DMA_FTR7_DATA_READ_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR7_DATA_READ_ERR_R)
#define FTR7_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_DATA_READ_ERR_POS, DMA_FTR7_DATA_READ_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR7_DATA_READ_ERR_W)
#define FTR7_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_DATA_READ_ERR_POS, DMA_FTR7_DATA_READ_ERR_LEN,value)
#endif

#define DMA0_FTR7_DATA_READ_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_DATA_READ_ERR_POS, DMA_FTR7_DATA_READ_ERR_LEN)

#define DMA0_FTR7_DATA_READ_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_DATA_READ_ERR_POS, DMA_FTR7_DATA_READ_ERR_LEN,value)


/* dbg_instr ACCESS: RO */

#ifndef DMA_FTR7_DBG_INSTR_POS
#define DMA_FTR7_DBG_INSTR_POS      30
#endif

#ifndef DMA_FTR7_DBG_INSTR_LEN
#define DMA_FTR7_DBG_INSTR_LEN      1
#endif

#if defined(_V1) && !defined(FTR7_DBG_INSTR_R)
#define FTR7_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_DBG_INSTR_POS, DMA_FTR7_DBG_INSTR_LEN)
#endif

#if defined(_V1) && !defined(FTR7_DBG_INSTR_W)
#define FTR7_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_DBG_INSTR_POS, DMA_FTR7_DBG_INSTR_LEN,value)
#endif

#define DMA0_FTR7_DBG_INSTR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_DBG_INSTR_POS, DMA_FTR7_DBG_INSTR_LEN)

#define DMA0_FTR7_DBG_INSTR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_DBG_INSTR_POS, DMA_FTR7_DBG_INSTR_LEN,value)


/* lockup_err ACCESS: RO */

#ifndef DMA_FTR7_LOCKUP_ERR_POS
#define DMA_FTR7_LOCKUP_ERR_POS      31
#endif

#ifndef DMA_FTR7_LOCKUP_ERR_LEN
#define DMA_FTR7_LOCKUP_ERR_LEN      1
#endif

#if defined(_V1) && !defined(FTR7_LOCKUP_ERR_R)
#define FTR7_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_LOCKUP_ERR_POS, DMA_FTR7_LOCKUP_ERR_LEN)
#endif

#if defined(_V1) && !defined(FTR7_LOCKUP_ERR_W)
#define FTR7_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_LOCKUP_ERR_POS, DMA_FTR7_LOCKUP_ERR_LEN,value)
#endif

#define DMA0_FTR7_LOCKUP_ERR_R        GetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_LOCKUP_ERR_POS, DMA_FTR7_LOCKUP_ERR_LEN)

#define DMA0_FTR7_LOCKUP_ERR_W(value) SetGroupBits32( (DMA0_FTR7_VAL),DMA_FTR7_LOCKUP_ERR_POS, DMA_FTR7_LOCKUP_ERR_LEN,value)


/* REGISTER: DBGSTATUS ACCESS: RO */

#if defined(_V1) && !defined(DBGSTATUS_OFFSET)
#define DBGSTATUS_OFFSET 0xD00
#endif

#if !defined(DMA_DBGSTATUS_OFFSET)
#define DMA_DBGSTATUS_OFFSET 0xD00
#endif

#if defined(_V1) && !defined(DBGSTATUS_REG)
#define DBGSTATUS_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DBGSTATUS_OFFSET))
#endif

#if defined(_V1) && !defined(DBGSTATUS_VAL)
#define DBGSTATUS_VAL  PREFIX_VAL(DBGSTATUS_REG)
#endif

#define DMA0_DBGSTATUS_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DBGSTATUS_OFFSET))
#define DMA0_DBGSTATUS_VAL  PREFIX_VAL(DMA0_DBGSTATUS_REG)

/* FIELDS: */

/* dbgstatus ACCESS: RO */

#ifndef DMA_DBGSTATUS_DBGSTATUS_POS
#define DMA_DBGSTATUS_DBGSTATUS_POS      0
#endif

#ifndef DMA_DBGSTATUS_DBGSTATUS_LEN
#define DMA_DBGSTATUS_DBGSTATUS_LEN      1
#endif

#if defined(_V1) && !defined(DBGSTATUS_DBGSTATUS_R)
#define DBGSTATUS_DBGSTATUS_R        GetGroupBits32( (DMA0_DBGSTATUS_VAL),DMA_DBGSTATUS_DBGSTATUS_POS, DMA_DBGSTATUS_DBGSTATUS_LEN)
#endif

#if defined(_V1) && !defined(DBGSTATUS_DBGSTATUS_W)
#define DBGSTATUS_DBGSTATUS_W(value) SetGroupBits32( (DMA0_DBGSTATUS_VAL),DMA_DBGSTATUS_DBGSTATUS_POS, DMA_DBGSTATUS_DBGSTATUS_LEN,value)
#endif

#define DMA0_DBGSTATUS_DBGSTATUS_R        GetGroupBits32( (DMA0_DBGSTATUS_VAL),DMA_DBGSTATUS_DBGSTATUS_POS, DMA_DBGSTATUS_DBGSTATUS_LEN)

#define DMA0_DBGSTATUS_DBGSTATUS_W(value) SetGroupBits32( (DMA0_DBGSTATUS_VAL),DMA_DBGSTATUS_DBGSTATUS_POS, DMA_DBGSTATUS_DBGSTATUS_LEN,value)


/* REGISTER: DBGCMD ACCESS: WO */

#if defined(_V1) && !defined(DBGCMD_OFFSET)
#define DBGCMD_OFFSET 0xD04
#endif

#if !defined(DMA_DBGCMD_OFFSET)
#define DMA_DBGCMD_OFFSET 0xD04
#endif

#if defined(_V1) && !defined(DBGCMD_REG)
#define DBGCMD_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DBGCMD_OFFSET))
#endif

#if defined(_V1) && !defined(DBGCMD_VAL)
#define DBGCMD_VAL  PREFIX_VAL(DBGCMD_REG)
#endif

#define DMA0_DBGCMD_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DBGCMD_OFFSET))
#define DMA0_DBGCMD_VAL  PREFIX_VAL(DMA0_DBGCMD_REG)

/* FIELDS: */

/* dbgcmd ACCESS: WO */

#ifndef DMA_DBGCMD_DBGCMD_POS
#define DMA_DBGCMD_DBGCMD_POS      0
#endif

#ifndef DMA_DBGCMD_DBGCMD_LEN
#define DMA_DBGCMD_DBGCMD_LEN      2
#endif

#if defined(_V1) && !defined(DBGCMD_DBGCMD_R)
#define DBGCMD_DBGCMD_R        GetGroupBits32( (DMA0_DBGCMD_VAL),DMA_DBGCMD_DBGCMD_POS, DMA_DBGCMD_DBGCMD_LEN)
#endif

#if defined(_V1) && !defined(DBGCMD_DBGCMD_W)
#define DBGCMD_DBGCMD_W(value) SetGroupBits32( (DMA0_DBGCMD_VAL),DMA_DBGCMD_DBGCMD_POS, DMA_DBGCMD_DBGCMD_LEN,value)
#endif

#define DMA0_DBGCMD_DBGCMD_R        GetGroupBits32( (DMA0_DBGCMD_VAL),DMA_DBGCMD_DBGCMD_POS, DMA_DBGCMD_DBGCMD_LEN)

#define DMA0_DBGCMD_DBGCMD_W(value) SetGroupBits32( (DMA0_DBGCMD_VAL),DMA_DBGCMD_DBGCMD_POS, DMA_DBGCMD_DBGCMD_LEN,value)


/* REGISTER: DBGINST0 ACCESS: WO */

#if defined(_V1) && !defined(DBGINST0_OFFSET)
#define DBGINST0_OFFSET 0xD08
#endif

#if !defined(DMA_DBGINST0_OFFSET)
#define DMA_DBGINST0_OFFSET 0xD08
#endif

#if defined(_V1) && !defined(DBGINST0_REG)
#define DBGINST0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DBGINST0_OFFSET))
#endif

#if defined(_V1) && !defined(DBGINST0_VAL)
#define DBGINST0_VAL  PREFIX_VAL(DBGINST0_REG)
#endif

#define DMA0_DBGINST0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DBGINST0_OFFSET))
#define DMA0_DBGINST0_VAL  PREFIX_VAL(DMA0_DBGINST0_REG)

/* FIELDS: */

/* Debug_thread ACCESS: WO */

#ifndef DMA_DBGINST0_DEBUG_THREAD_POS
#define DMA_DBGINST0_DEBUG_THREAD_POS      0
#endif

#ifndef DMA_DBGINST0_DEBUG_THREAD_LEN
#define DMA_DBGINST0_DEBUG_THREAD_LEN      1
#endif

#if defined(_V1) && !defined(DBGINST0_DEBUG_THREAD_R)
#define DBGINST0_DEBUG_THREAD_R        GetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_DEBUG_THREAD_POS, DMA_DBGINST0_DEBUG_THREAD_LEN)
#endif

#if defined(_V1) && !defined(DBGINST0_DEBUG_THREAD_W)
#define DBGINST0_DEBUG_THREAD_W(value) SetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_DEBUG_THREAD_POS, DMA_DBGINST0_DEBUG_THREAD_LEN,value)
#endif

#define DMA0_DBGINST0_DEBUG_THREAD_R        GetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_DEBUG_THREAD_POS, DMA_DBGINST0_DEBUG_THREAD_LEN)

#define DMA0_DBGINST0_DEBUG_THREAD_W(value) SetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_DEBUG_THREAD_POS, DMA_DBGINST0_DEBUG_THREAD_LEN,value)


/* Channel_number ACCESS: WO */

#ifndef DMA_DBGINST0_CHANNEL_NUMBER_POS
#define DMA_DBGINST0_CHANNEL_NUMBER_POS      8
#endif

#ifndef DMA_DBGINST0_CHANNEL_NUMBER_LEN
#define DMA_DBGINST0_CHANNEL_NUMBER_LEN      3
#endif

#if defined(_V1) && !defined(DBGINST0_CHANNEL_NUMBER_R)
#define DBGINST0_CHANNEL_NUMBER_R        GetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_CHANNEL_NUMBER_POS, DMA_DBGINST0_CHANNEL_NUMBER_LEN)
#endif

#if defined(_V1) && !defined(DBGINST0_CHANNEL_NUMBER_W)
#define DBGINST0_CHANNEL_NUMBER_W(value) SetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_CHANNEL_NUMBER_POS, DMA_DBGINST0_CHANNEL_NUMBER_LEN,value)
#endif

#define DMA0_DBGINST0_CHANNEL_NUMBER_R        GetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_CHANNEL_NUMBER_POS, DMA_DBGINST0_CHANNEL_NUMBER_LEN)

#define DMA0_DBGINST0_CHANNEL_NUMBER_W(value) SetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_CHANNEL_NUMBER_POS, DMA_DBGINST0_CHANNEL_NUMBER_LEN,value)


/* Instruction_byte_0 ACCESS: WO */

#ifndef DMA_DBGINST0_INSTRUCTION_BYTE_0_POS
#define DMA_DBGINST0_INSTRUCTION_BYTE_0_POS      16
#endif

#ifndef DMA_DBGINST0_INSTRUCTION_BYTE_0_LEN
#define DMA_DBGINST0_INSTRUCTION_BYTE_0_LEN      8
#endif

#if defined(_V1) && !defined(DBGINST0_INSTRUCTION_BYTE_0_R)
#define DBGINST0_INSTRUCTION_BYTE_0_R        GetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_INSTRUCTION_BYTE_0_POS, DMA_DBGINST0_INSTRUCTION_BYTE_0_LEN)
#endif

#if defined(_V1) && !defined(DBGINST0_INSTRUCTION_BYTE_0_W)
#define DBGINST0_INSTRUCTION_BYTE_0_W(value) SetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_INSTRUCTION_BYTE_0_POS, DMA_DBGINST0_INSTRUCTION_BYTE_0_LEN,value)
#endif

#define DMA0_DBGINST0_INSTRUCTION_BYTE_0_R        GetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_INSTRUCTION_BYTE_0_POS, DMA_DBGINST0_INSTRUCTION_BYTE_0_LEN)

#define DMA0_DBGINST0_INSTRUCTION_BYTE_0_W(value) SetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_INSTRUCTION_BYTE_0_POS, DMA_DBGINST0_INSTRUCTION_BYTE_0_LEN,value)


/* Instruction_byte_1 ACCESS: WO */

#ifndef DMA_DBGINST0_INSTRUCTION_BYTE_1_POS
#define DMA_DBGINST0_INSTRUCTION_BYTE_1_POS      24
#endif

#ifndef DMA_DBGINST0_INSTRUCTION_BYTE_1_LEN
#define DMA_DBGINST0_INSTRUCTION_BYTE_1_LEN      8
#endif

#if defined(_V1) && !defined(DBGINST0_INSTRUCTION_BYTE_1_R)
#define DBGINST0_INSTRUCTION_BYTE_1_R        GetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_INSTRUCTION_BYTE_1_POS, DMA_DBGINST0_INSTRUCTION_BYTE_1_LEN)
#endif

#if defined(_V1) && !defined(DBGINST0_INSTRUCTION_BYTE_1_W)
#define DBGINST0_INSTRUCTION_BYTE_1_W(value) SetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_INSTRUCTION_BYTE_1_POS, DMA_DBGINST0_INSTRUCTION_BYTE_1_LEN,value)
#endif

#define DMA0_DBGINST0_INSTRUCTION_BYTE_1_R        GetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_INSTRUCTION_BYTE_1_POS, DMA_DBGINST0_INSTRUCTION_BYTE_1_LEN)

#define DMA0_DBGINST0_INSTRUCTION_BYTE_1_W(value) SetGroupBits32( (DMA0_DBGINST0_VAL),DMA_DBGINST0_INSTRUCTION_BYTE_1_POS, DMA_DBGINST0_INSTRUCTION_BYTE_1_LEN,value)


/* REGISTER: DBGINST1 ACCESS: WO */

#if defined(_V1) && !defined(DBGINST1_OFFSET)
#define DBGINST1_OFFSET 0xD0C
#endif

#if !defined(DMA_DBGINST1_OFFSET)
#define DMA_DBGINST1_OFFSET 0xD0C
#endif

#if defined(_V1) && !defined(DBGINST1_REG)
#define DBGINST1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DBGINST1_OFFSET))
#endif

#if defined(_V1) && !defined(DBGINST1_VAL)
#define DBGINST1_VAL  PREFIX_VAL(DBGINST1_REG)
#endif

#define DMA0_DBGINST1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_DBGINST1_OFFSET))
#define DMA0_DBGINST1_VAL  PREFIX_VAL(DMA0_DBGINST1_REG)

/* FIELDS: */

/* Instruction_byte_2 ACCESS: WO */

#ifndef DMA_DBGINST1_INSTRUCTION_BYTE_2_POS
#define DMA_DBGINST1_INSTRUCTION_BYTE_2_POS      0
#endif

#ifndef DMA_DBGINST1_INSTRUCTION_BYTE_2_LEN
#define DMA_DBGINST1_INSTRUCTION_BYTE_2_LEN      8
#endif

#if defined(_V1) && !defined(DBGINST1_INSTRUCTION_BYTE_2_R)
#define DBGINST1_INSTRUCTION_BYTE_2_R        GetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_2_POS, DMA_DBGINST1_INSTRUCTION_BYTE_2_LEN)
#endif

#if defined(_V1) && !defined(DBGINST1_INSTRUCTION_BYTE_2_W)
#define DBGINST1_INSTRUCTION_BYTE_2_W(value) SetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_2_POS, DMA_DBGINST1_INSTRUCTION_BYTE_2_LEN,value)
#endif

#define DMA0_DBGINST1_INSTRUCTION_BYTE_2_R        GetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_2_POS, DMA_DBGINST1_INSTRUCTION_BYTE_2_LEN)

#define DMA0_DBGINST1_INSTRUCTION_BYTE_2_W(value) SetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_2_POS, DMA_DBGINST1_INSTRUCTION_BYTE_2_LEN,value)


/* Instruction_byte_3 ACCESS: WO */

#ifndef DMA_DBGINST1_INSTRUCTION_BYTE_3_POS
#define DMA_DBGINST1_INSTRUCTION_BYTE_3_POS      8
#endif

#ifndef DMA_DBGINST1_INSTRUCTION_BYTE_3_LEN
#define DMA_DBGINST1_INSTRUCTION_BYTE_3_LEN      8
#endif

#if defined(_V1) && !defined(DBGINST1_INSTRUCTION_BYTE_3_R)
#define DBGINST1_INSTRUCTION_BYTE_3_R        GetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_3_POS, DMA_DBGINST1_INSTRUCTION_BYTE_3_LEN)
#endif

#if defined(_V1) && !defined(DBGINST1_INSTRUCTION_BYTE_3_W)
#define DBGINST1_INSTRUCTION_BYTE_3_W(value) SetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_3_POS, DMA_DBGINST1_INSTRUCTION_BYTE_3_LEN,value)
#endif

#define DMA0_DBGINST1_INSTRUCTION_BYTE_3_R        GetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_3_POS, DMA_DBGINST1_INSTRUCTION_BYTE_3_LEN)

#define DMA0_DBGINST1_INSTRUCTION_BYTE_3_W(value) SetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_3_POS, DMA_DBGINST1_INSTRUCTION_BYTE_3_LEN,value)


/* Instruction_byte_4 ACCESS: WO */

#ifndef DMA_DBGINST1_INSTRUCTION_BYTE_4_POS
#define DMA_DBGINST1_INSTRUCTION_BYTE_4_POS      16
#endif

#ifndef DMA_DBGINST1_INSTRUCTION_BYTE_4_LEN
#define DMA_DBGINST1_INSTRUCTION_BYTE_4_LEN      8
#endif

#if defined(_V1) && !defined(DBGINST1_INSTRUCTION_BYTE_4_R)
#define DBGINST1_INSTRUCTION_BYTE_4_R        GetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_4_POS, DMA_DBGINST1_INSTRUCTION_BYTE_4_LEN)
#endif

#if defined(_V1) && !defined(DBGINST1_INSTRUCTION_BYTE_4_W)
#define DBGINST1_INSTRUCTION_BYTE_4_W(value) SetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_4_POS, DMA_DBGINST1_INSTRUCTION_BYTE_4_LEN,value)
#endif

#define DMA0_DBGINST1_INSTRUCTION_BYTE_4_R        GetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_4_POS, DMA_DBGINST1_INSTRUCTION_BYTE_4_LEN)

#define DMA0_DBGINST1_INSTRUCTION_BYTE_4_W(value) SetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_4_POS, DMA_DBGINST1_INSTRUCTION_BYTE_4_LEN,value)


/* Instruction_byte_5 ACCESS: WO */

#ifndef DMA_DBGINST1_INSTRUCTION_BYTE_5_POS
#define DMA_DBGINST1_INSTRUCTION_BYTE_5_POS      24
#endif

#ifndef DMA_DBGINST1_INSTRUCTION_BYTE_5_LEN
#define DMA_DBGINST1_INSTRUCTION_BYTE_5_LEN      8
#endif

#if defined(_V1) && !defined(DBGINST1_INSTRUCTION_BYTE_5_R)
#define DBGINST1_INSTRUCTION_BYTE_5_R        GetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_5_POS, DMA_DBGINST1_INSTRUCTION_BYTE_5_LEN)
#endif

#if defined(_V1) && !defined(DBGINST1_INSTRUCTION_BYTE_5_W)
#define DBGINST1_INSTRUCTION_BYTE_5_W(value) SetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_5_POS, DMA_DBGINST1_INSTRUCTION_BYTE_5_LEN,value)
#endif

#define DMA0_DBGINST1_INSTRUCTION_BYTE_5_R        GetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_5_POS, DMA_DBGINST1_INSTRUCTION_BYTE_5_LEN)

#define DMA0_DBGINST1_INSTRUCTION_BYTE_5_W(value) SetGroupBits32( (DMA0_DBGINST1_VAL),DMA_DBGINST1_INSTRUCTION_BYTE_5_POS, DMA_DBGINST1_INSTRUCTION_BYTE_5_LEN,value)


/* REGISTER: CR0 ACCESS: RO */

#if defined(_V1) && !defined(CR0_OFFSET)
#define CR0_OFFSET 0xE00
#endif

#if !defined(DMA_CR0_OFFSET)
#define DMA_CR0_OFFSET 0xE00
#endif

#if defined(_V1) && !defined(CR0_REG)
#define CR0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CR0_OFFSET))
#endif

#if defined(_V1) && !defined(CR0_VAL)
#define CR0_VAL  PREFIX_VAL(CR0_REG)
#endif

#define DMA0_CR0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CR0_OFFSET))
#define DMA0_CR0_VAL  PREFIX_VAL(DMA0_CR0_REG)

/* FIELDS: */

/* periph_req ACCESS: RO */

#ifndef DMA_CR0_PERIPH_REQ_POS
#define DMA_CR0_PERIPH_REQ_POS      0
#endif

#ifndef DMA_CR0_PERIPH_REQ_LEN
#define DMA_CR0_PERIPH_REQ_LEN      1
#endif

#if defined(_V1) && !defined(CR0_PERIPH_REQ_R)
#define CR0_PERIPH_REQ_R        GetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_PERIPH_REQ_POS, DMA_CR0_PERIPH_REQ_LEN)
#endif

#if defined(_V1) && !defined(CR0_PERIPH_REQ_W)
#define CR0_PERIPH_REQ_W(value) SetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_PERIPH_REQ_POS, DMA_CR0_PERIPH_REQ_LEN,value)
#endif

#define DMA0_CR0_PERIPH_REQ_R        GetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_PERIPH_REQ_POS, DMA_CR0_PERIPH_REQ_LEN)

#define DMA0_CR0_PERIPH_REQ_W(value) SetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_PERIPH_REQ_POS, DMA_CR0_PERIPH_REQ_LEN,value)


/* boot_en ACCESS: RO */

#ifndef DMA_CR0_BOOT_EN_POS
#define DMA_CR0_BOOT_EN_POS      1
#endif

#ifndef DMA_CR0_BOOT_EN_LEN
#define DMA_CR0_BOOT_EN_LEN      1
#endif

#if defined(_V1) && !defined(CR0_BOOT_EN_R)
#define CR0_BOOT_EN_R        GetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_BOOT_EN_POS, DMA_CR0_BOOT_EN_LEN)
#endif

#if defined(_V1) && !defined(CR0_BOOT_EN_W)
#define CR0_BOOT_EN_W(value) SetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_BOOT_EN_POS, DMA_CR0_BOOT_EN_LEN,value)
#endif

#define DMA0_CR0_BOOT_EN_R        GetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_BOOT_EN_POS, DMA_CR0_BOOT_EN_LEN)

#define DMA0_CR0_BOOT_EN_W(value) SetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_BOOT_EN_POS, DMA_CR0_BOOT_EN_LEN,value)


/* mgr_ns_at_rst ACCESS: RO */

#ifndef DMA_CR0_MGR_NS_AT_RST_POS
#define DMA_CR0_MGR_NS_AT_RST_POS      2
#endif

#ifndef DMA_CR0_MGR_NS_AT_RST_LEN
#define DMA_CR0_MGR_NS_AT_RST_LEN      1
#endif

#if defined(_V1) && !defined(CR0_MGR_NS_AT_RST_R)
#define CR0_MGR_NS_AT_RST_R        GetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_MGR_NS_AT_RST_POS, DMA_CR0_MGR_NS_AT_RST_LEN)
#endif

#if defined(_V1) && !defined(CR0_MGR_NS_AT_RST_W)
#define CR0_MGR_NS_AT_RST_W(value) SetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_MGR_NS_AT_RST_POS, DMA_CR0_MGR_NS_AT_RST_LEN,value)
#endif

#define DMA0_CR0_MGR_NS_AT_RST_R        GetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_MGR_NS_AT_RST_POS, DMA_CR0_MGR_NS_AT_RST_LEN)

#define DMA0_CR0_MGR_NS_AT_RST_W(value) SetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_MGR_NS_AT_RST_POS, DMA_CR0_MGR_NS_AT_RST_LEN,value)


/* num_chnls ACCESS: RO */

#ifndef DMA_CR0_NUM_CHNLS_POS
#define DMA_CR0_NUM_CHNLS_POS      4
#endif

#ifndef DMA_CR0_NUM_CHNLS_LEN
#define DMA_CR0_NUM_CHNLS_LEN      3
#endif

#if defined(_V1) && !defined(CR0_NUM_CHNLS_R)
#define CR0_NUM_CHNLS_R        GetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_NUM_CHNLS_POS, DMA_CR0_NUM_CHNLS_LEN)
#endif

#if defined(_V1) && !defined(CR0_NUM_CHNLS_W)
#define CR0_NUM_CHNLS_W(value) SetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_NUM_CHNLS_POS, DMA_CR0_NUM_CHNLS_LEN,value)
#endif

#define DMA0_CR0_NUM_CHNLS_R        GetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_NUM_CHNLS_POS, DMA_CR0_NUM_CHNLS_LEN)

#define DMA0_CR0_NUM_CHNLS_W(value) SetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_NUM_CHNLS_POS, DMA_CR0_NUM_CHNLS_LEN,value)


/* num_periph_req ACCESS: RO */

#ifndef DMA_CR0_NUM_PERIPH_REQ_POS
#define DMA_CR0_NUM_PERIPH_REQ_POS      12
#endif

#ifndef DMA_CR0_NUM_PERIPH_REQ_LEN
#define DMA_CR0_NUM_PERIPH_REQ_LEN      5
#endif

#if defined(_V1) && !defined(CR0_NUM_PERIPH_REQ_R)
#define CR0_NUM_PERIPH_REQ_R        GetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_NUM_PERIPH_REQ_POS, DMA_CR0_NUM_PERIPH_REQ_LEN)
#endif

#if defined(_V1) && !defined(CR0_NUM_PERIPH_REQ_W)
#define CR0_NUM_PERIPH_REQ_W(value) SetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_NUM_PERIPH_REQ_POS, DMA_CR0_NUM_PERIPH_REQ_LEN,value)
#endif

#define DMA0_CR0_NUM_PERIPH_REQ_R        GetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_NUM_PERIPH_REQ_POS, DMA_CR0_NUM_PERIPH_REQ_LEN)

#define DMA0_CR0_NUM_PERIPH_REQ_W(value) SetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_NUM_PERIPH_REQ_POS, DMA_CR0_NUM_PERIPH_REQ_LEN,value)


/* num_events ACCESS: RO */

#ifndef DMA_CR0_NUM_EVENTS_POS
#define DMA_CR0_NUM_EVENTS_POS      17
#endif

#ifndef DMA_CR0_NUM_EVENTS_LEN
#define DMA_CR0_NUM_EVENTS_LEN      5
#endif

#if defined(_V1) && !defined(CR0_NUM_EVENTS_R)
#define CR0_NUM_EVENTS_R        GetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_NUM_EVENTS_POS, DMA_CR0_NUM_EVENTS_LEN)
#endif

#if defined(_V1) && !defined(CR0_NUM_EVENTS_W)
#define CR0_NUM_EVENTS_W(value) SetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_NUM_EVENTS_POS, DMA_CR0_NUM_EVENTS_LEN,value)
#endif

#define DMA0_CR0_NUM_EVENTS_R        GetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_NUM_EVENTS_POS, DMA_CR0_NUM_EVENTS_LEN)

#define DMA0_CR0_NUM_EVENTS_W(value) SetGroupBits32( (DMA0_CR0_VAL),DMA_CR0_NUM_EVENTS_POS, DMA_CR0_NUM_EVENTS_LEN,value)


/* REGISTER: CR1 ACCESS: RO */

#if defined(_V1) && !defined(CR1_OFFSET)
#define CR1_OFFSET 0xE04
#endif

#if !defined(DMA_CR1_OFFSET)
#define DMA_CR1_OFFSET 0xE04
#endif

#if defined(_V1) && !defined(CR1_REG)
#define CR1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CR1_OFFSET))
#endif

#if defined(_V1) && !defined(CR1_VAL)
#define CR1_VAL  PREFIX_VAL(CR1_REG)
#endif

#define DMA0_CR1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CR1_OFFSET))
#define DMA0_CR1_VAL  PREFIX_VAL(DMA0_CR1_REG)

/* FIELDS: */

/* i_cache_len ACCESS: RO */

#ifndef DMA_CR1_I_CACHE_LEN_POS
#define DMA_CR1_I_CACHE_LEN_POS      0
#endif

#ifndef DMA_CR1_I_CACHE_LEN_LEN
#define DMA_CR1_I_CACHE_LEN_LEN      3
#endif

#if defined(_V1) && !defined(CR1_I_CACHE_LEN_R)
#define CR1_I_CACHE_LEN_R        GetGroupBits32( (DMA0_CR1_VAL),DMA_CR1_I_CACHE_LEN_POS, DMA_CR1_I_CACHE_LEN_LEN)
#endif

#if defined(_V1) && !defined(CR1_I_CACHE_LEN_W)
#define CR1_I_CACHE_LEN_W(value) SetGroupBits32( (DMA0_CR1_VAL),DMA_CR1_I_CACHE_LEN_POS, DMA_CR1_I_CACHE_LEN_LEN,value)
#endif

#define DMA0_CR1_I_CACHE_LEN_R        GetGroupBits32( (DMA0_CR1_VAL),DMA_CR1_I_CACHE_LEN_POS, DMA_CR1_I_CACHE_LEN_LEN)

#define DMA0_CR1_I_CACHE_LEN_W(value) SetGroupBits32( (DMA0_CR1_VAL),DMA_CR1_I_CACHE_LEN_POS, DMA_CR1_I_CACHE_LEN_LEN,value)


/* num_i_cache_lines ACCESS: RO */

#ifndef DMA_CR1_NUM_I_CACHE_LINES_POS
#define DMA_CR1_NUM_I_CACHE_LINES_POS      4
#endif

#ifndef DMA_CR1_NUM_I_CACHE_LINES_LEN
#define DMA_CR1_NUM_I_CACHE_LINES_LEN      4
#endif

#if defined(_V1) && !defined(CR1_NUM_I_CACHE_LINES_R)
#define CR1_NUM_I_CACHE_LINES_R        GetGroupBits32( (DMA0_CR1_VAL),DMA_CR1_NUM_I_CACHE_LINES_POS, DMA_CR1_NUM_I_CACHE_LINES_LEN)
#endif

#if defined(_V1) && !defined(CR1_NUM_I_CACHE_LINES_W)
#define CR1_NUM_I_CACHE_LINES_W(value) SetGroupBits32( (DMA0_CR1_VAL),DMA_CR1_NUM_I_CACHE_LINES_POS, DMA_CR1_NUM_I_CACHE_LINES_LEN,value)
#endif

#define DMA0_CR1_NUM_I_CACHE_LINES_R        GetGroupBits32( (DMA0_CR1_VAL),DMA_CR1_NUM_I_CACHE_LINES_POS, DMA_CR1_NUM_I_CACHE_LINES_LEN)

#define DMA0_CR1_NUM_I_CACHE_LINES_W(value) SetGroupBits32( (DMA0_CR1_VAL),DMA_CR1_NUM_I_CACHE_LINES_POS, DMA_CR1_NUM_I_CACHE_LINES_LEN,value)


/* REGISTER: CR2 ACCESS: RO */

#if defined(_V1) && !defined(CR2_OFFSET)
#define CR2_OFFSET 0xE08
#endif

#if !defined(DMA_CR2_OFFSET)
#define DMA_CR2_OFFSET 0xE08
#endif

#if defined(_V1) && !defined(CR2_REG)
#define CR2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CR2_OFFSET))
#endif

#if defined(_V1) && !defined(CR2_VAL)
#define CR2_VAL  PREFIX_VAL(CR2_REG)
#endif

#define DMA0_CR2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CR2_OFFSET))
#define DMA0_CR2_VAL  PREFIX_VAL(DMA0_CR2_REG)

/* FIELDS: */

/* boot_addr ACCESS: RO */

#ifndef DMA_CR2_BOOT_ADDR_POS
#define DMA_CR2_BOOT_ADDR_POS      0
#endif

#ifndef DMA_CR2_BOOT_ADDR_LEN
#define DMA_CR2_BOOT_ADDR_LEN      32
#endif

#if defined(_V1) && !defined(CR2_BOOT_ADDR_R)
#define CR2_BOOT_ADDR_R        GetGroupBits32( (DMA0_CR2_VAL),DMA_CR2_BOOT_ADDR_POS, DMA_CR2_BOOT_ADDR_LEN)
#endif

#if defined(_V1) && !defined(CR2_BOOT_ADDR_W)
#define CR2_BOOT_ADDR_W(value) SetGroupBits32( (DMA0_CR2_VAL),DMA_CR2_BOOT_ADDR_POS, DMA_CR2_BOOT_ADDR_LEN,value)
#endif

#define DMA0_CR2_BOOT_ADDR_R        GetGroupBits32( (DMA0_CR2_VAL),DMA_CR2_BOOT_ADDR_POS, DMA_CR2_BOOT_ADDR_LEN)

#define DMA0_CR2_BOOT_ADDR_W(value) SetGroupBits32( (DMA0_CR2_VAL),DMA_CR2_BOOT_ADDR_POS, DMA_CR2_BOOT_ADDR_LEN,value)


/* REGISTER: CR3 ACCESS: RO */

#if defined(_V1) && !defined(CR3_OFFSET)
#define CR3_OFFSET 0xE0C
#endif

#if !defined(DMA_CR3_OFFSET)
#define DMA_CR3_OFFSET 0xE0C
#endif

#if defined(_V1) && !defined(CR3_REG)
#define CR3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CR3_OFFSET))
#endif

#if defined(_V1) && !defined(CR3_VAL)
#define CR3_VAL  PREFIX_VAL(CR3_REG)
#endif

#define DMA0_CR3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CR3_OFFSET))
#define DMA0_CR3_VAL  PREFIX_VAL(DMA0_CR3_REG)

/* FIELDS: */

/* INS ACCESS: RO */

#ifndef DMA_CR3_INS_POS
#define DMA_CR3_INS_POS      0
#endif

#ifndef DMA_CR3_INS_LEN
#define DMA_CR3_INS_LEN      32
#endif

#if defined(_V1) && !defined(CR3_INS_R)
#define CR3_INS_R        GetGroupBits32( (DMA0_CR3_VAL),DMA_CR3_INS_POS, DMA_CR3_INS_LEN)
#endif

#if defined(_V1) && !defined(CR3_INS_W)
#define CR3_INS_W(value) SetGroupBits32( (DMA0_CR3_VAL),DMA_CR3_INS_POS, DMA_CR3_INS_LEN,value)
#endif

#define DMA0_CR3_INS_R        GetGroupBits32( (DMA0_CR3_VAL),DMA_CR3_INS_POS, DMA_CR3_INS_LEN)

#define DMA0_CR3_INS_W(value) SetGroupBits32( (DMA0_CR3_VAL),DMA_CR3_INS_POS, DMA_CR3_INS_LEN,value)


/* REGISTER: CR4 ACCESS: RO */

#if defined(_V1) && !defined(CR4_OFFSET)
#define CR4_OFFSET 0xE10
#endif

#if !defined(DMA_CR4_OFFSET)
#define DMA_CR4_OFFSET 0xE10
#endif

#if defined(_V1) && !defined(CR4_REG)
#define CR4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CR4_OFFSET))
#endif

#if defined(_V1) && !defined(CR4_VAL)
#define CR4_VAL  PREFIX_VAL(CR4_REG)
#endif

#define DMA0_CR4_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CR4_OFFSET))
#define DMA0_CR4_VAL  PREFIX_VAL(DMA0_CR4_REG)

/* FIELDS: */

/* PNS ACCESS: RO */

#ifndef DMA_CR4_PNS_POS
#define DMA_CR4_PNS_POS      0
#endif

#ifndef DMA_CR4_PNS_LEN
#define DMA_CR4_PNS_LEN      32
#endif

#if defined(_V1) && !defined(CR4_PNS_R)
#define CR4_PNS_R        GetGroupBits32( (DMA0_CR4_VAL),DMA_CR4_PNS_POS, DMA_CR4_PNS_LEN)
#endif

#if defined(_V1) && !defined(CR4_PNS_W)
#define CR4_PNS_W(value) SetGroupBits32( (DMA0_CR4_VAL),DMA_CR4_PNS_POS, DMA_CR4_PNS_LEN,value)
#endif

#define DMA0_CR4_PNS_R        GetGroupBits32( (DMA0_CR4_VAL),DMA_CR4_PNS_POS, DMA_CR4_PNS_LEN)

#define DMA0_CR4_PNS_W(value) SetGroupBits32( (DMA0_CR4_VAL),DMA_CR4_PNS_POS, DMA_CR4_PNS_LEN,value)


/* REGISTER: CRD ACCESS: RO */

#if defined(_V1) && !defined(CRD_OFFSET)
#define CRD_OFFSET 0xE14
#endif

#if !defined(DMA_CRD_OFFSET)
#define DMA_CRD_OFFSET 0xE14
#endif

#if defined(_V1) && !defined(CRD_REG)
#define CRD_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CRD_OFFSET))
#endif

#if defined(_V1) && !defined(CRD_VAL)
#define CRD_VAL  PREFIX_VAL(CRD_REG)
#endif

#define DMA0_CRD_REG ((volatile UINT32 *) (DMA0_BASE + DMA_CRD_OFFSET))
#define DMA0_CRD_VAL  PREFIX_VAL(DMA0_CRD_REG)

/* FIELDS: */

/* data_width ACCESS: RO */

#ifndef DMA_CRD_DATA_WIDTH_POS
#define DMA_CRD_DATA_WIDTH_POS      0
#endif

#ifndef DMA_CRD_DATA_WIDTH_LEN
#define DMA_CRD_DATA_WIDTH_LEN      3
#endif

#if defined(_V1) && !defined(CRD_DATA_WIDTH_R)
#define CRD_DATA_WIDTH_R        GetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_DATA_WIDTH_POS, DMA_CRD_DATA_WIDTH_LEN)
#endif

#if defined(_V1) && !defined(CRD_DATA_WIDTH_W)
#define CRD_DATA_WIDTH_W(value) SetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_DATA_WIDTH_POS, DMA_CRD_DATA_WIDTH_LEN,value)
#endif

#define DMA0_CRD_DATA_WIDTH_R        GetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_DATA_WIDTH_POS, DMA_CRD_DATA_WIDTH_LEN)

#define DMA0_CRD_DATA_WIDTH_W(value) SetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_DATA_WIDTH_POS, DMA_CRD_DATA_WIDTH_LEN,value)


/* wr_cap ACCESS: RO */

#ifndef DMA_CRD_WR_CAP_POS
#define DMA_CRD_WR_CAP_POS      4
#endif

#ifndef DMA_CRD_WR_CAP_LEN
#define DMA_CRD_WR_CAP_LEN      3
#endif

#if defined(_V1) && !defined(CRD_WR_CAP_R)
#define CRD_WR_CAP_R        GetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_WR_CAP_POS, DMA_CRD_WR_CAP_LEN)
#endif

#if defined(_V1) && !defined(CRD_WR_CAP_W)
#define CRD_WR_CAP_W(value) SetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_WR_CAP_POS, DMA_CRD_WR_CAP_LEN,value)
#endif

#define DMA0_CRD_WR_CAP_R        GetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_WR_CAP_POS, DMA_CRD_WR_CAP_LEN)

#define DMA0_CRD_WR_CAP_W(value) SetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_WR_CAP_POS, DMA_CRD_WR_CAP_LEN,value)


/* wr_q_dep ACCESS: RO */

#ifndef DMA_CRD_WR_Q_DEP_POS
#define DMA_CRD_WR_Q_DEP_POS      8
#endif

#ifndef DMA_CRD_WR_Q_DEP_LEN
#define DMA_CRD_WR_Q_DEP_LEN      4
#endif

#if defined(_V1) && !defined(CRD_WR_Q_DEP_R)
#define CRD_WR_Q_DEP_R        GetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_WR_Q_DEP_POS, DMA_CRD_WR_Q_DEP_LEN)
#endif

#if defined(_V1) && !defined(CRD_WR_Q_DEP_W)
#define CRD_WR_Q_DEP_W(value) SetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_WR_Q_DEP_POS, DMA_CRD_WR_Q_DEP_LEN,value)
#endif

#define DMA0_CRD_WR_Q_DEP_R        GetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_WR_Q_DEP_POS, DMA_CRD_WR_Q_DEP_LEN)

#define DMA0_CRD_WR_Q_DEP_W(value) SetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_WR_Q_DEP_POS, DMA_CRD_WR_Q_DEP_LEN,value)


/* rd_cap ACCESS: RO */

#ifndef DMA_CRD_RD_CAP_POS
#define DMA_CRD_RD_CAP_POS      12
#endif

#ifndef DMA_CRD_RD_CAP_LEN
#define DMA_CRD_RD_CAP_LEN      3
#endif

#if defined(_V1) && !defined(CRD_RD_CAP_R)
#define CRD_RD_CAP_R        GetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_RD_CAP_POS, DMA_CRD_RD_CAP_LEN)
#endif

#if defined(_V1) && !defined(CRD_RD_CAP_W)
#define CRD_RD_CAP_W(value) SetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_RD_CAP_POS, DMA_CRD_RD_CAP_LEN,value)
#endif

#define DMA0_CRD_RD_CAP_R        GetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_RD_CAP_POS, DMA_CRD_RD_CAP_LEN)

#define DMA0_CRD_RD_CAP_W(value) SetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_RD_CAP_POS, DMA_CRD_RD_CAP_LEN,value)


/* rd_q_dep ACCESS: RO */

#ifndef DMA_CRD_RD_Q_DEP_POS
#define DMA_CRD_RD_Q_DEP_POS      16
#endif

#ifndef DMA_CRD_RD_Q_DEP_LEN
#define DMA_CRD_RD_Q_DEP_LEN      4
#endif

#if defined(_V1) && !defined(CRD_RD_Q_DEP_R)
#define CRD_RD_Q_DEP_R        GetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_RD_Q_DEP_POS, DMA_CRD_RD_Q_DEP_LEN)
#endif

#if defined(_V1) && !defined(CRD_RD_Q_DEP_W)
#define CRD_RD_Q_DEP_W(value) SetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_RD_Q_DEP_POS, DMA_CRD_RD_Q_DEP_LEN,value)
#endif

#define DMA0_CRD_RD_Q_DEP_R        GetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_RD_Q_DEP_POS, DMA_CRD_RD_Q_DEP_LEN)

#define DMA0_CRD_RD_Q_DEP_W(value) SetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_RD_Q_DEP_POS, DMA_CRD_RD_Q_DEP_LEN,value)


/* data_buffer_dep ACCESS: RO */

#ifndef DMA_CRD_DATA_BUFFER_DEP_POS
#define DMA_CRD_DATA_BUFFER_DEP_POS      20
#endif

#ifndef DMA_CRD_DATA_BUFFER_DEP_LEN
#define DMA_CRD_DATA_BUFFER_DEP_LEN      10
#endif

#if defined(_V1) && !defined(CRD_DATA_BUFFER_DEP_R)
#define CRD_DATA_BUFFER_DEP_R        GetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_DATA_BUFFER_DEP_POS, DMA_CRD_DATA_BUFFER_DEP_LEN)
#endif

#if defined(_V1) && !defined(CRD_DATA_BUFFER_DEP_W)
#define CRD_DATA_BUFFER_DEP_W(value) SetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_DATA_BUFFER_DEP_POS, DMA_CRD_DATA_BUFFER_DEP_LEN,value)
#endif

#define DMA0_CRD_DATA_BUFFER_DEP_R        GetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_DATA_BUFFER_DEP_POS, DMA_CRD_DATA_BUFFER_DEP_LEN)

#define DMA0_CRD_DATA_BUFFER_DEP_W(value) SetGroupBits32( (DMA0_CRD_VAL),DMA_CRD_DATA_BUFFER_DEP_POS, DMA_CRD_DATA_BUFFER_DEP_LEN,value)


/* REGISTER: WD ACCESS: RW */

#if defined(_V1) && !defined(WD_OFFSET)
#define WD_OFFSET 0xE80
#endif

#if !defined(DMA_WD_OFFSET)
#define DMA_WD_OFFSET 0xE80
#endif

#if defined(_V1) && !defined(WD_REG)
#define WD_REG ((volatile UINT32 *) (DMA0_BASE + DMA_WD_OFFSET))
#endif

#if defined(_V1) && !defined(WD_VAL)
#define WD_VAL  PREFIX_VAL(WD_REG)
#endif

#define DMA0_WD_REG ((volatile UINT32 *) (DMA0_BASE + DMA_WD_OFFSET))
#define DMA0_WD_VAL  PREFIX_VAL(DMA0_WD_REG)

/* FIELDS: */

/* wd_irq_only ACCESS: RW */

#ifndef DMA_WD_WD_IRQ_ONLY_POS
#define DMA_WD_WD_IRQ_ONLY_POS      0
#endif

#ifndef DMA_WD_WD_IRQ_ONLY_LEN
#define DMA_WD_WD_IRQ_ONLY_LEN      1
#endif

#if defined(_V1) && !defined(WD_WD_IRQ_ONLY_R)
#define WD_WD_IRQ_ONLY_R        GetGroupBits32( (DMA0_WD_VAL),DMA_WD_WD_IRQ_ONLY_POS, DMA_WD_WD_IRQ_ONLY_LEN)
#endif

#if defined(_V1) && !defined(WD_WD_IRQ_ONLY_W)
#define WD_WD_IRQ_ONLY_W(value) SetGroupBits32( (DMA0_WD_VAL),DMA_WD_WD_IRQ_ONLY_POS, DMA_WD_WD_IRQ_ONLY_LEN,value)
#endif

#define DMA0_WD_WD_IRQ_ONLY_R        GetGroupBits32( (DMA0_WD_VAL),DMA_WD_WD_IRQ_ONLY_POS, DMA_WD_WD_IRQ_ONLY_LEN)

#define DMA0_WD_WD_IRQ_ONLY_W(value) SetGroupBits32( (DMA0_WD_VAL),DMA_WD_WD_IRQ_ONLY_POS, DMA_WD_WD_IRQ_ONLY_LEN,value)


/* REGISTER: PERIPH_ID_0 ACCESS: RO */

#if defined(_V1) && !defined(PERIPH_ID_0_OFFSET)
#define PERIPH_ID_0_OFFSET 0xFE0
#endif

#if !defined(DMA_PERIPH_ID_0_OFFSET)
#define DMA_PERIPH_ID_0_OFFSET 0xFE0
#endif

#if defined(_V1) && !defined(PERIPH_ID_0_REG)
#define PERIPH_ID_0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PERIPH_ID_0_OFFSET))
#endif

#if defined(_V1) && !defined(PERIPH_ID_0_VAL)
#define PERIPH_ID_0_VAL  PREFIX_VAL(PERIPH_ID_0_REG)
#endif

#define DMA0_PERIPH_ID_0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PERIPH_ID_0_OFFSET))
#define DMA0_PERIPH_ID_0_VAL  PREFIX_VAL(DMA0_PERIPH_ID_0_REG)

/* FIELDS: */

/* part_number_0 ACCESS: RO */

#ifndef DMA_PERIPH_ID_0_PART_NUMBER_0_POS
#define DMA_PERIPH_ID_0_PART_NUMBER_0_POS      0
#endif

#ifndef DMA_PERIPH_ID_0_PART_NUMBER_0_LEN
#define DMA_PERIPH_ID_0_PART_NUMBER_0_LEN      8
#endif

#if defined(_V1) && !defined(PERIPH_ID_0_PART_NUMBER_0_R)
#define PERIPH_ID_0_PART_NUMBER_0_R        GetGroupBits32( (DMA0_PERIPH_ID_0_VAL),DMA_PERIPH_ID_0_PART_NUMBER_0_POS, DMA_PERIPH_ID_0_PART_NUMBER_0_LEN)
#endif

#if defined(_V1) && !defined(PERIPH_ID_0_PART_NUMBER_0_W)
#define PERIPH_ID_0_PART_NUMBER_0_W(value) SetGroupBits32( (DMA0_PERIPH_ID_0_VAL),DMA_PERIPH_ID_0_PART_NUMBER_0_POS, DMA_PERIPH_ID_0_PART_NUMBER_0_LEN,value)
#endif

#define DMA0_PERIPH_ID_0_PART_NUMBER_0_R        GetGroupBits32( (DMA0_PERIPH_ID_0_VAL),DMA_PERIPH_ID_0_PART_NUMBER_0_POS, DMA_PERIPH_ID_0_PART_NUMBER_0_LEN)

#define DMA0_PERIPH_ID_0_PART_NUMBER_0_W(value) SetGroupBits32( (DMA0_PERIPH_ID_0_VAL),DMA_PERIPH_ID_0_PART_NUMBER_0_POS, DMA_PERIPH_ID_0_PART_NUMBER_0_LEN,value)


/* REGISTER: PERIPH_ID_1 ACCESS: RO */

#if defined(_V1) && !defined(PERIPH_ID_1_OFFSET)
#define PERIPH_ID_1_OFFSET 0xFE4
#endif

#if !defined(DMA_PERIPH_ID_1_OFFSET)
#define DMA_PERIPH_ID_1_OFFSET 0xFE4
#endif

#if defined(_V1) && !defined(PERIPH_ID_1_REG)
#define PERIPH_ID_1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PERIPH_ID_1_OFFSET))
#endif

#if defined(_V1) && !defined(PERIPH_ID_1_VAL)
#define PERIPH_ID_1_VAL  PREFIX_VAL(PERIPH_ID_1_REG)
#endif

#define DMA0_PERIPH_ID_1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PERIPH_ID_1_OFFSET))
#define DMA0_PERIPH_ID_1_VAL  PREFIX_VAL(DMA0_PERIPH_ID_1_REG)

/* FIELDS: */

/* part_number_1 ACCESS: RO */

#ifndef DMA_PERIPH_ID_1_PART_NUMBER_1_POS
#define DMA_PERIPH_ID_1_PART_NUMBER_1_POS      0
#endif

#ifndef DMA_PERIPH_ID_1_PART_NUMBER_1_LEN
#define DMA_PERIPH_ID_1_PART_NUMBER_1_LEN      4
#endif

#if defined(_V1) && !defined(PERIPH_ID_1_PART_NUMBER_1_R)
#define PERIPH_ID_1_PART_NUMBER_1_R        GetGroupBits32( (DMA0_PERIPH_ID_1_VAL),DMA_PERIPH_ID_1_PART_NUMBER_1_POS, DMA_PERIPH_ID_1_PART_NUMBER_1_LEN)
#endif

#if defined(_V1) && !defined(PERIPH_ID_1_PART_NUMBER_1_W)
#define PERIPH_ID_1_PART_NUMBER_1_W(value) SetGroupBits32( (DMA0_PERIPH_ID_1_VAL),DMA_PERIPH_ID_1_PART_NUMBER_1_POS, DMA_PERIPH_ID_1_PART_NUMBER_1_LEN,value)
#endif

#define DMA0_PERIPH_ID_1_PART_NUMBER_1_R        GetGroupBits32( (DMA0_PERIPH_ID_1_VAL),DMA_PERIPH_ID_1_PART_NUMBER_1_POS, DMA_PERIPH_ID_1_PART_NUMBER_1_LEN)

#define DMA0_PERIPH_ID_1_PART_NUMBER_1_W(value) SetGroupBits32( (DMA0_PERIPH_ID_1_VAL),DMA_PERIPH_ID_1_PART_NUMBER_1_POS, DMA_PERIPH_ID_1_PART_NUMBER_1_LEN,value)


/* designer_0 ACCESS: RO */

#ifndef DMA_PERIPH_ID_1_DESIGNER_0_POS
#define DMA_PERIPH_ID_1_DESIGNER_0_POS      4
#endif

#ifndef DMA_PERIPH_ID_1_DESIGNER_0_LEN
#define DMA_PERIPH_ID_1_DESIGNER_0_LEN      4
#endif

#if defined(_V1) && !defined(PERIPH_ID_1_DESIGNER_0_R)
#define PERIPH_ID_1_DESIGNER_0_R        GetGroupBits32( (DMA0_PERIPH_ID_1_VAL),DMA_PERIPH_ID_1_DESIGNER_0_POS, DMA_PERIPH_ID_1_DESIGNER_0_LEN)
#endif

#if defined(_V1) && !defined(PERIPH_ID_1_DESIGNER_0_W)
#define PERIPH_ID_1_DESIGNER_0_W(value) SetGroupBits32( (DMA0_PERIPH_ID_1_VAL),DMA_PERIPH_ID_1_DESIGNER_0_POS, DMA_PERIPH_ID_1_DESIGNER_0_LEN,value)
#endif

#define DMA0_PERIPH_ID_1_DESIGNER_0_R        GetGroupBits32( (DMA0_PERIPH_ID_1_VAL),DMA_PERIPH_ID_1_DESIGNER_0_POS, DMA_PERIPH_ID_1_DESIGNER_0_LEN)

#define DMA0_PERIPH_ID_1_DESIGNER_0_W(value) SetGroupBits32( (DMA0_PERIPH_ID_1_VAL),DMA_PERIPH_ID_1_DESIGNER_0_POS, DMA_PERIPH_ID_1_DESIGNER_0_LEN,value)


/* REGISTER: PERIPH_ID_2 ACCESS: RO */

#if defined(_V1) && !defined(PERIPH_ID_2_OFFSET)
#define PERIPH_ID_2_OFFSET 0xFE8
#endif

#if !defined(DMA_PERIPH_ID_2_OFFSET)
#define DMA_PERIPH_ID_2_OFFSET 0xFE8
#endif

#if defined(_V1) && !defined(PERIPH_ID_2_REG)
#define PERIPH_ID_2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PERIPH_ID_2_OFFSET))
#endif

#if defined(_V1) && !defined(PERIPH_ID_2_VAL)
#define PERIPH_ID_2_VAL  PREFIX_VAL(PERIPH_ID_2_REG)
#endif

#define DMA0_PERIPH_ID_2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PERIPH_ID_2_OFFSET))
#define DMA0_PERIPH_ID_2_VAL  PREFIX_VAL(DMA0_PERIPH_ID_2_REG)

/* FIELDS: */

/* designer_1 ACCESS: RO */

#ifndef DMA_PERIPH_ID_2_DESIGNER_1_POS
#define DMA_PERIPH_ID_2_DESIGNER_1_POS      0
#endif

#ifndef DMA_PERIPH_ID_2_DESIGNER_1_LEN
#define DMA_PERIPH_ID_2_DESIGNER_1_LEN      4
#endif

#if defined(_V1) && !defined(PERIPH_ID_2_DESIGNER_1_R)
#define PERIPH_ID_2_DESIGNER_1_R        GetGroupBits32( (DMA0_PERIPH_ID_2_VAL),DMA_PERIPH_ID_2_DESIGNER_1_POS, DMA_PERIPH_ID_2_DESIGNER_1_LEN)
#endif

#if defined(_V1) && !defined(PERIPH_ID_2_DESIGNER_1_W)
#define PERIPH_ID_2_DESIGNER_1_W(value) SetGroupBits32( (DMA0_PERIPH_ID_2_VAL),DMA_PERIPH_ID_2_DESIGNER_1_POS, DMA_PERIPH_ID_2_DESIGNER_1_LEN,value)
#endif

#define DMA0_PERIPH_ID_2_DESIGNER_1_R        GetGroupBits32( (DMA0_PERIPH_ID_2_VAL),DMA_PERIPH_ID_2_DESIGNER_1_POS, DMA_PERIPH_ID_2_DESIGNER_1_LEN)

#define DMA0_PERIPH_ID_2_DESIGNER_1_W(value) SetGroupBits32( (DMA0_PERIPH_ID_2_VAL),DMA_PERIPH_ID_2_DESIGNER_1_POS, DMA_PERIPH_ID_2_DESIGNER_1_LEN,value)


/* revision ACCESS: RO */

#ifndef DMA_PERIPH_ID_2_REVISION_POS
#define DMA_PERIPH_ID_2_REVISION_POS      4
#endif

#ifndef DMA_PERIPH_ID_2_REVISION_LEN
#define DMA_PERIPH_ID_2_REVISION_LEN      4
#endif

#if defined(_V1) && !defined(PERIPH_ID_2_REVISION_R)
#define PERIPH_ID_2_REVISION_R        GetGroupBits32( (DMA0_PERIPH_ID_2_VAL),DMA_PERIPH_ID_2_REVISION_POS, DMA_PERIPH_ID_2_REVISION_LEN)
#endif

#if defined(_V1) && !defined(PERIPH_ID_2_REVISION_W)
#define PERIPH_ID_2_REVISION_W(value) SetGroupBits32( (DMA0_PERIPH_ID_2_VAL),DMA_PERIPH_ID_2_REVISION_POS, DMA_PERIPH_ID_2_REVISION_LEN,value)
#endif

#define DMA0_PERIPH_ID_2_REVISION_R        GetGroupBits32( (DMA0_PERIPH_ID_2_VAL),DMA_PERIPH_ID_2_REVISION_POS, DMA_PERIPH_ID_2_REVISION_LEN)

#define DMA0_PERIPH_ID_2_REVISION_W(value) SetGroupBits32( (DMA0_PERIPH_ID_2_VAL),DMA_PERIPH_ID_2_REVISION_POS, DMA_PERIPH_ID_2_REVISION_LEN,value)


/* REGISTER: PERIPH_ID_3 ACCESS: RO */

#if defined(_V1) && !defined(PERIPH_ID_3_OFFSET)
#define PERIPH_ID_3_OFFSET 0xFEC
#endif

#if !defined(DMA_PERIPH_ID_3_OFFSET)
#define DMA_PERIPH_ID_3_OFFSET 0xFEC
#endif

#if defined(_V1) && !defined(PERIPH_ID_3_REG)
#define PERIPH_ID_3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PERIPH_ID_3_OFFSET))
#endif

#if defined(_V1) && !defined(PERIPH_ID_3_VAL)
#define PERIPH_ID_3_VAL  PREFIX_VAL(PERIPH_ID_3_REG)
#endif

#define DMA0_PERIPH_ID_3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PERIPH_ID_3_OFFSET))
#define DMA0_PERIPH_ID_3_VAL  PREFIX_VAL(DMA0_PERIPH_ID_3_REG)

/* FIELDS: */

/* integration_cfg ACCESS: RO */

#ifndef DMA_PERIPH_ID_3_INTEGRATION_CFG_POS
#define DMA_PERIPH_ID_3_INTEGRATION_CFG_POS      0
#endif

#ifndef DMA_PERIPH_ID_3_INTEGRATION_CFG_LEN
#define DMA_PERIPH_ID_3_INTEGRATION_CFG_LEN      1
#endif

#if defined(_V1) && !defined(PERIPH_ID_3_INTEGRATION_CFG_R)
#define PERIPH_ID_3_INTEGRATION_CFG_R        GetGroupBits32( (DMA0_PERIPH_ID_3_VAL),DMA_PERIPH_ID_3_INTEGRATION_CFG_POS, DMA_PERIPH_ID_3_INTEGRATION_CFG_LEN)
#endif

#if defined(_V1) && !defined(PERIPH_ID_3_INTEGRATION_CFG_W)
#define PERIPH_ID_3_INTEGRATION_CFG_W(value) SetGroupBits32( (DMA0_PERIPH_ID_3_VAL),DMA_PERIPH_ID_3_INTEGRATION_CFG_POS, DMA_PERIPH_ID_3_INTEGRATION_CFG_LEN,value)
#endif

#define DMA0_PERIPH_ID_3_INTEGRATION_CFG_R        GetGroupBits32( (DMA0_PERIPH_ID_3_VAL),DMA_PERIPH_ID_3_INTEGRATION_CFG_POS, DMA_PERIPH_ID_3_INTEGRATION_CFG_LEN)

#define DMA0_PERIPH_ID_3_INTEGRATION_CFG_W(value) SetGroupBits32( (DMA0_PERIPH_ID_3_VAL),DMA_PERIPH_ID_3_INTEGRATION_CFG_POS, DMA_PERIPH_ID_3_INTEGRATION_CFG_LEN,value)


/* REGISTER: PCELL_ID_0 ACCESS: RO */

#if defined(_V1) && !defined(PCELL_ID_0_OFFSET)
#define PCELL_ID_0_OFFSET 0xFF0
#endif

#if !defined(DMA_PCELL_ID_0_OFFSET)
#define DMA_PCELL_ID_0_OFFSET 0xFF0
#endif

#if defined(_V1) && !defined(PCELL_ID_0_REG)
#define PCELL_ID_0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PCELL_ID_0_OFFSET))
#endif

#if defined(_V1) && !defined(PCELL_ID_0_VAL)
#define PCELL_ID_0_VAL  PREFIX_VAL(PCELL_ID_0_REG)
#endif

#define DMA0_PCELL_ID_0_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PCELL_ID_0_OFFSET))
#define DMA0_PCELL_ID_0_VAL  PREFIX_VAL(DMA0_PCELL_ID_0_REG)

/* FIELDS: */

/* pcell_id_0 ACCESS: RO */

#ifndef DMA_PCELL_ID_0_PCELL_ID_0_POS
#define DMA_PCELL_ID_0_PCELL_ID_0_POS      0
#endif

#ifndef DMA_PCELL_ID_0_PCELL_ID_0_LEN
#define DMA_PCELL_ID_0_PCELL_ID_0_LEN      8
#endif

#if defined(_V1) && !defined(PCELL_ID_0_PCELL_ID_0_R)
#define PCELL_ID_0_PCELL_ID_0_R        GetGroupBits32( (DMA0_PCELL_ID_0_VAL),DMA_PCELL_ID_0_PCELL_ID_0_POS, DMA_PCELL_ID_0_PCELL_ID_0_LEN)
#endif

#if defined(_V1) && !defined(PCELL_ID_0_PCELL_ID_0_W)
#define PCELL_ID_0_PCELL_ID_0_W(value) SetGroupBits32( (DMA0_PCELL_ID_0_VAL),DMA_PCELL_ID_0_PCELL_ID_0_POS, DMA_PCELL_ID_0_PCELL_ID_0_LEN,value)
#endif

#define DMA0_PCELL_ID_0_PCELL_ID_0_R        GetGroupBits32( (DMA0_PCELL_ID_0_VAL),DMA_PCELL_ID_0_PCELL_ID_0_POS, DMA_PCELL_ID_0_PCELL_ID_0_LEN)

#define DMA0_PCELL_ID_0_PCELL_ID_0_W(value) SetGroupBits32( (DMA0_PCELL_ID_0_VAL),DMA_PCELL_ID_0_PCELL_ID_0_POS, DMA_PCELL_ID_0_PCELL_ID_0_LEN,value)


/* REGISTER: PCELL_ID_1 ACCESS: RO */

#if defined(_V1) && !defined(PCELL_ID_1_OFFSET)
#define PCELL_ID_1_OFFSET 0xFF4
#endif

#if !defined(DMA_PCELL_ID_1_OFFSET)
#define DMA_PCELL_ID_1_OFFSET 0xFF4
#endif

#if defined(_V1) && !defined(PCELL_ID_1_REG)
#define PCELL_ID_1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PCELL_ID_1_OFFSET))
#endif

#if defined(_V1) && !defined(PCELL_ID_1_VAL)
#define PCELL_ID_1_VAL  PREFIX_VAL(PCELL_ID_1_REG)
#endif

#define DMA0_PCELL_ID_1_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PCELL_ID_1_OFFSET))
#define DMA0_PCELL_ID_1_VAL  PREFIX_VAL(DMA0_PCELL_ID_1_REG)

/* FIELDS: */

/* pcell_id_1 ACCESS: RO */

#ifndef DMA_PCELL_ID_1_PCELL_ID_1_POS
#define DMA_PCELL_ID_1_PCELL_ID_1_POS      0
#endif

#ifndef DMA_PCELL_ID_1_PCELL_ID_1_LEN
#define DMA_PCELL_ID_1_PCELL_ID_1_LEN      8
#endif

#if defined(_V1) && !defined(PCELL_ID_1_PCELL_ID_1_R)
#define PCELL_ID_1_PCELL_ID_1_R        GetGroupBits32( (DMA0_PCELL_ID_1_VAL),DMA_PCELL_ID_1_PCELL_ID_1_POS, DMA_PCELL_ID_1_PCELL_ID_1_LEN)
#endif

#if defined(_V1) && !defined(PCELL_ID_1_PCELL_ID_1_W)
#define PCELL_ID_1_PCELL_ID_1_W(value) SetGroupBits32( (DMA0_PCELL_ID_1_VAL),DMA_PCELL_ID_1_PCELL_ID_1_POS, DMA_PCELL_ID_1_PCELL_ID_1_LEN,value)
#endif

#define DMA0_PCELL_ID_1_PCELL_ID_1_R        GetGroupBits32( (DMA0_PCELL_ID_1_VAL),DMA_PCELL_ID_1_PCELL_ID_1_POS, DMA_PCELL_ID_1_PCELL_ID_1_LEN)

#define DMA0_PCELL_ID_1_PCELL_ID_1_W(value) SetGroupBits32( (DMA0_PCELL_ID_1_VAL),DMA_PCELL_ID_1_PCELL_ID_1_POS, DMA_PCELL_ID_1_PCELL_ID_1_LEN,value)


/* REGISTER: PCELL_ID_2 ACCESS: RO */

#if defined(_V1) && !defined(PCELL_ID_2_OFFSET)
#define PCELL_ID_2_OFFSET 0xFF8
#endif

#if !defined(DMA_PCELL_ID_2_OFFSET)
#define DMA_PCELL_ID_2_OFFSET 0xFF8
#endif

#if defined(_V1) && !defined(PCELL_ID_2_REG)
#define PCELL_ID_2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PCELL_ID_2_OFFSET))
#endif

#if defined(_V1) && !defined(PCELL_ID_2_VAL)
#define PCELL_ID_2_VAL  PREFIX_VAL(PCELL_ID_2_REG)
#endif

#define DMA0_PCELL_ID_2_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PCELL_ID_2_OFFSET))
#define DMA0_PCELL_ID_2_VAL  PREFIX_VAL(DMA0_PCELL_ID_2_REG)

/* FIELDS: */

/* pcell_id_2 ACCESS: RO */

#ifndef DMA_PCELL_ID_2_PCELL_ID_2_POS
#define DMA_PCELL_ID_2_PCELL_ID_2_POS      0
#endif

#ifndef DMA_PCELL_ID_2_PCELL_ID_2_LEN
#define DMA_PCELL_ID_2_PCELL_ID_2_LEN      8
#endif

#if defined(_V1) && !defined(PCELL_ID_2_PCELL_ID_2_R)
#define PCELL_ID_2_PCELL_ID_2_R        GetGroupBits32( (DMA0_PCELL_ID_2_VAL),DMA_PCELL_ID_2_PCELL_ID_2_POS, DMA_PCELL_ID_2_PCELL_ID_2_LEN)
#endif

#if defined(_V1) && !defined(PCELL_ID_2_PCELL_ID_2_W)
#define PCELL_ID_2_PCELL_ID_2_W(value) SetGroupBits32( (DMA0_PCELL_ID_2_VAL),DMA_PCELL_ID_2_PCELL_ID_2_POS, DMA_PCELL_ID_2_PCELL_ID_2_LEN,value)
#endif

#define DMA0_PCELL_ID_2_PCELL_ID_2_R        GetGroupBits32( (DMA0_PCELL_ID_2_VAL),DMA_PCELL_ID_2_PCELL_ID_2_POS, DMA_PCELL_ID_2_PCELL_ID_2_LEN)

#define DMA0_PCELL_ID_2_PCELL_ID_2_W(value) SetGroupBits32( (DMA0_PCELL_ID_2_VAL),DMA_PCELL_ID_2_PCELL_ID_2_POS, DMA_PCELL_ID_2_PCELL_ID_2_LEN,value)


/* REGISTER: PCELL_ID_3 ACCESS: RO */

#if defined(_V1) && !defined(PCELL_ID_3_OFFSET)
#define PCELL_ID_3_OFFSET 0xFFC
#endif

#if !defined(DMA_PCELL_ID_3_OFFSET)
#define DMA_PCELL_ID_3_OFFSET 0xFFC
#endif

#if defined(_V1) && !defined(PCELL_ID_3_REG)
#define PCELL_ID_3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PCELL_ID_3_OFFSET))
#endif

#if defined(_V1) && !defined(PCELL_ID_3_VAL)
#define PCELL_ID_3_VAL  PREFIX_VAL(PCELL_ID_3_REG)
#endif

#define DMA0_PCELL_ID_3_REG ((volatile UINT32 *) (DMA0_BASE + DMA_PCELL_ID_3_OFFSET))
#define DMA0_PCELL_ID_3_VAL  PREFIX_VAL(DMA0_PCELL_ID_3_REG)

/* FIELDS: */

/* pcell_id_3 ACCESS: RO */

#ifndef DMA_PCELL_ID_3_PCELL_ID_3_POS
#define DMA_PCELL_ID_3_PCELL_ID_3_POS      0
#endif

#ifndef DMA_PCELL_ID_3_PCELL_ID_3_LEN
#define DMA_PCELL_ID_3_PCELL_ID_3_LEN      8
#endif

#if defined(_V1) && !defined(PCELL_ID_3_PCELL_ID_3_R)
#define PCELL_ID_3_PCELL_ID_3_R        GetGroupBits32( (DMA0_PCELL_ID_3_VAL),DMA_PCELL_ID_3_PCELL_ID_3_POS, DMA_PCELL_ID_3_PCELL_ID_3_LEN)
#endif

#if defined(_V1) && !defined(PCELL_ID_3_PCELL_ID_3_W)
#define PCELL_ID_3_PCELL_ID_3_W(value) SetGroupBits32( (DMA0_PCELL_ID_3_VAL),DMA_PCELL_ID_3_PCELL_ID_3_POS, DMA_PCELL_ID_3_PCELL_ID_3_LEN,value)
#endif

#define DMA0_PCELL_ID_3_PCELL_ID_3_R        GetGroupBits32( (DMA0_PCELL_ID_3_VAL),DMA_PCELL_ID_3_PCELL_ID_3_POS, DMA_PCELL_ID_3_PCELL_ID_3_LEN)

#define DMA0_PCELL_ID_3_PCELL_ID_3_W(value) SetGroupBits32( (DMA0_PCELL_ID_3_VAL),DMA_PCELL_ID_3_PCELL_ID_3_POS, DMA_PCELL_ID_3_PCELL_ID_3_LEN,value)


/* OFFSET TABLE: */
#define dma0_offset_tbl_values	DSR_OFFSET, CSR0_OFFSET, CPC0_OFFSET, CSR1_OFFSET, CPC1_OFFSET, CSR2_OFFSET, CPC2_OFFSET, CSR3_OFFSET, CPC3_OFFSET, CSR4_OFFSET, CPC4_OFFSET, CSR5_OFFSET, CPC5_OFFSET, CSR6_OFFSET, CPC6_OFFSET, CSR7_OFFSET, CPC7_OFFSET, INTEN_OFFSET, INT_EVENT_RIS_OFFSET, INTMIS_OFFSET, INTCLR_OFFSET, FSRD_OFFSET, FSRC_OFFSET, FTRD_OFFSET, DPC_OFFSET, FTR0_OFFSET, SAR0_OFFSET, DAR0_OFFSET, CCR0_OFFSET, LC0_0_OFFSET, LC1_0_OFFSET, SAR1_OFFSET, DAR1_OFFSET, CCR1_OFFSET, LC0_1_OFFSET, LC1_1_OFFSET, FTR1_OFFSET, SAR2_OFFSET, DAR2_OFFSET, CCR2_OFFSET, LC0_2_OFFSET, LC1_2_OFFSET, SAR3_OFFSET, DAR3_OFFSET, CCR3_OFFSET, LC0_3_OFFSET, LC1_3_OFFSET, FTR2_OFFSET, SAR4_OFFSET, DAR4_OFFSET, CCR4_OFFSET, LC0_4_OFFSET, LC1_4_OFFSET, SAR5_OFFSET, DAR5_OFFSET, CCR5_OFFSET, LC0_5_OFFSET, LC1_5_OFFSET, FTR3_OFFSET, SAR6_OFFSET, DAR6_OFFSET, CCR6_OFFSET, LC0_6_OFFSET, LC1_6_OFFSET, SAR7_OFFSET, DAR7_OFFSET, CCR7_OFFSET, LC0_7_OFFSET, LC1_7_OFFSET, FTR4_OFFSET, FTR5_OFFSET, FTR6_OFFSET, FTR7_OFFSET, DBGSTATUS_OFFSET, DBGCMD_OFFSET, DBGINST0_OFFSET, DBGINST1_OFFSET, CR0_OFFSET, CR1_OFFSET, CR2_OFFSET, CR3_OFFSET, CR4_OFFSET, CRD_OFFSET, WD_OFFSET, PERIPH_ID_0_OFFSET, PERIPH_ID_1_OFFSET, PERIPH_ID_2_OFFSET, PERIPH_ID_3_OFFSET, PCELL_ID_0_OFFSET, PCELL_ID_1_OFFSET, PCELL_ID_2_OFFSET, PCELL_ID_3_OFFSET


/* REGISTERS RESET VAL: */
#define dma0_regs_reset_val	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00800200, 0x0, 0x0, 0x0, 0x0, 0x00800200, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00800200, 0x0, 0x0, 0x0, 0x0, 0x00800200, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00800200, 0x0, 0x0, 0x0, 0x0, 0x00800200, 0x0, 0x0, 0x0, 0x0, 0x0, 0x00800200, 0x0, 0x0, 0x0, 0x0, 0x00800200, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xF5, 0x0, 0x0, 0x0, 0x3FFF7F74, 0x0, 0x30, 0x13, 0x34, 0x0, 0x0D, 0xF0, 0x05, 0xB1

#endif

/* End of DMA0 */
/* ///////////////////////////////////////////////////////////////////////*/
