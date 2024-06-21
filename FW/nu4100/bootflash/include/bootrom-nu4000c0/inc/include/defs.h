#ifndef _DEFS_H_
#define _DEFS_H_

typedef unsigned char       BYTE;
typedef unsigned char       BOOL;
typedef unsigned char       UINT8;
typedef signed char         INT8;
typedef unsigned short      UINT16;
typedef signed short        INT16;  
typedef unsigned int        UINT32;
typedef signed int          INT32;

#ifndef MAX
    #define MAX(A,B) ((A)>(B) ? (A):(B))
#endif

#ifndef MIN
    #define MIN(A,B) ((A)<(B) ? (A):(B))
#endif

#ifndef ABS
    #define  ABS(A)  (((A) < 0) ? -(A) : (A))
#endif

#ifndef SIGN
    #define SIGN(A)     (((A)<0) ? (-1) : (1))
#endif

#ifndef TRUE
   #define TRUE    1
#endif 

#ifndef FALSE
   #define FALSE   0
#endif 

#ifndef NULL
   #define NULL   0
#endif 

#endif //_DEFS_H_
