#ifndef _NU_REGS_H_
#define _NU_REGS_H_

/* build environment definition */
#include "config.h"
/* types */
#include "defs.h"

#define PREFIX_VAL *

#define SetGroupBits32(reg,position,len,value)\
{       \
        UINT32 tmp=reg;\
        tmp&=~(((UINT32)0xFFFFFFFF>>(32-(len)))<<(position));\
        tmp|=((value&((UINT32)0xFFFFFFFF>>(32-(len))))<<(position));\
        reg=tmp;\
}

#define GetGroupBits32(reg,position,len) (((reg) & (((UINT32)0xFFFFFFFF>>(32-(len)))<<(position)))>>(position))

#define SDRAM_BASE         (0x80000000)

#include "regs/nu4000C0/nu4000_regs.h"
#endif //_NU_REGS_H_
