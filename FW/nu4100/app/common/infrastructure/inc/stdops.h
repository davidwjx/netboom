#ifndef _STDOPS_H_
#define _STDOPS_H_

#include "inu_types.h"


#define BYTES_IN_WORD   sizeof(UINT16)
#define BYTES_IN_DWORD  sizeof(UINT32)
#define BITS_IN_BYTE    (8)
#define BITS_IN_WORD    (BITS_IN_BYTE*BYTES_IN_WORD)
#define NIBBLES_IN_BYTE (2)

#define MASK_BIT_0   0x00000001
#define MASK_BIT_1   0x00000002
#define MASK_BIT_2   0x00000004
#define MASK_BIT_3   0x00000008
#define MASK_BIT_4   0x00000010
#define MASK_BIT_5   0x00000020
#define MASK_BIT_6   0x00000040
#define MASK_BIT_7   0x00000080
#define MASK_BIT_8   0x00000100
#define MASK_BIT_9   0x00000200
#define MASK_BIT_10  0x00000400
#define MASK_BIT_11  0x00000800
#define MASK_BIT_12  0x00001000
#define MASK_BIT_13  0x00002000
#define MASK_BIT_14  0x00004000
#define MASK_BIT_15  0x00008000
#define MASK_BIT_16  0x00010000
#define MASK_BIT_17  0x00020000
#define MASK_BIT_18  0x00040000
#define MASK_BIT_19  0x00080000
#define MASK_BIT_20  0x00100000
#define MASK_BIT_21  0x00200000
#define MASK_BIT_22  0x00400000
#define MASK_BIT_23  0x00800000
#define MASK_BIT_24  0x01000000
#define MASK_BIT_25  0x02000000
#define MASK_BIT_26  0x04000000
#define MASK_BIT_27  0x08000000
#define MASK_BIT_28  0x10000000
#define MASK_BIT_29  0x20000000
#define MASK_BIT_30  0x40000000
#define MASK_BIT_31  0x80000000

#define MASK_2_LSBITS   (MASK_BIT_2-1)
#define MASK_3_LSBITS   (MASK_BIT_3-1)
#define MASK_LS_NIBBLE  (MASK_BIT_4-1)
#define MASK_5_LSBITS   (MASK_BIT_5-1)
#define MASK_6_LSBITS   (MASK_BIT_6-1)
#define MASK_7_LSBITS   (MASK_BIT_7-1)
#define MASK_LSBYTE     (MASK_BIT_8-1)
#define MASK_9_LSBITS   (MASK_BIT_9-1)
#define MASK_10_LSBITS  (MASK_BIT_10-1)
#define MASK_11_LSBITS  (MASK_BIT_11-1)
#define MASK_12_LSBITS  (MASK_BIT_12-1)
#define MASK_13_LSBITS  (MASK_BIT_13-1)
#define MASK_14_LSBITS  (MASK_BIT_14-1)
#define MASK_15_LSBITS  (MASK_BIT_15-1)
#define MASK_16_LSBITS  (MASK_BIT_16-1)
#define MASK_17_LSBITS  (MASK_BIT_17-1)
#define MASK_18_LSBITS  (MASK_BIT_18-1)
#define MASK_19_LSBITS  (MASK_BIT_19-1)
#define MASK_20_LSBITS  (MASK_BIT_20-1)
#define MASK_21_LSBITS  (MASK_BIT_21-1)
#define MASK_22_LSBITS  (MASK_BIT_22-1)
#define MASK_23_LSBITS  (MASK_BIT_23-1)
#define MASK_24_LSBITS  (MASK_BIT_24-1)
#define MASK_25_LSBITS  (MASK_BIT_25-1)
#define MASK_26_LSBITS  (MASK_BIT_26-1)
#define MASK_27_LSBITS  (MASK_BIT_27-1)
#define MASK_28_LSBITS  (MASK_BIT_28-1)
#define MASK_29_LSBITS  (MASK_BIT_29-1)
#define MASK_30_LSBITS  (MASK_BIT_30-1)

#define SHIFT_RIGHT(OPERAND,PARAM)  (OPERAND >> PARAM)
#define SHIFT_LEFT(OPERAND,PARAM)   (OPERAND << PARAM)

#define FIX_UNUSED_PARAM_WARN(param) (void)(param)

#if DEFSG_IS_OS_LINUX
#ifndef ATTR_PACKED
#define ATTR_PACKED __attribute__ ((__packed__))
#endif
#elif DEFSG_IS_CEVA
#define ATTR_PACKED 
#endif

#if DEFSG_IS_GP
/*
* __clz returns the number of leading zeros, zero input will return 32, and
* 0x80000000 will return 0.
*/
static inline unsigned int __clz(unsigned int x)
{
        unsigned int ret;

        asm("clz\t%0, %1" : "=r" (ret) : "r" (x));

        return ret;
}

static inline int __popcount(unsigned int x)
{
   return __builtin_popcount(x);
}
#endif


 
static inline UINT32 ALIGN_TO_64(UINT32 number)
{
   if(number & 0x3F)
   {
      number = (number & 0xFFFFFFC0) + 64;
   }
   return number;
}

#ifdef CEVA
//	   #include "InV_defines.h"
#else
#ifndef MAX
       #define MAX(A,B) ((A)>(B) ? (A):(B))
       #define FAST_MAX(A,B) (A ^ ((B ^ A) & -(B > A)))
#endif

#ifndef MIN
       #define MIN(A,B) ((A)<(B) ? (A):(B))
       #define FAST_MIN(A,B) (B ^ ((A ^ B) & -(A < B)))
#endif

#ifndef ABS
       #define  ABS(A)  (((A) < 0) ? -(A) : (A))
#endif
#endif

#ifndef SIGN
    #define SIGN(A)     (((A)<0) ? (-1) : (1))
#endif

#if DEFSG_IS_GP
//Memory barrier instructions
#define ARM_DMB           asm volatile ("dmb": : :"memory")  // data memory barrier
#define ARM_DSB           asm volatile ("dsb": : :"memory")  // data synchronization barrier
#define ARM_ISB           asm volatile ("isb": : :"memory")  // instruction synchronization barrier
#endif

#endif //_STDOPS_H_