#ifndef NU4000_B0_CRAM_REG_H
#define NU4000_B0_CRAM_REG_H


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
/*   CRAM (Prototype: CRAM)                */
/*****************************************/
#define CRAM_BASE 0x01000000

/* OFFSET TABLE: */
#define cram_offset_tbl_values	


/* REGISTERS RESET VAL: */
#define cram_regs_reset_val	

#endif

/* End of CRAM */
/* ///////////////////////////////////////////////////////////////////////*/
