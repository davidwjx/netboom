#ifndef NU4000_B0_LRAM_REG_H
#define NU4000_B0_LRAM_REG_H


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
/*   LRAM (Prototype: LRAM)                */
/*****************************************/
#define LRAM_BASE 0x02000000

/* OFFSET TABLE: */
#define lram_offset_tbl_values	


/* REGISTERS RESET VAL: */
#define lram_regs_reset_val	

#endif

/* End of LRAM */
/* ///////////////////////////////////////////////////////////////////////*/
