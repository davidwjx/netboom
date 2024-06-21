#ifndef _INU_TYPES_H_
#define _INU_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

//Processor defines
#define DEFSG_HOST       (1)
#define DEFSG_GP         (2)
#define DEFSG_CEVA       (3)
#define DEFSG_EV72       (4)


typedef enum {
   DEFSG_CEVA_1 =      DEFSG_CEVA+1,
   DEFSG_CEVA_2,
   DEFSG_CEVA_NONE
} INU_DEFSG_cevaId;

//Current processor
//OS defines
#define DEFSG_WINDOWS       (1)
#define DEFSG_LINUX_GLIBC   (2)
#define DEFSG_LINUX_BIONIC  (3)
#define DEFSG_CEVA_OS       (4)
#define DEFSG_EV72_OS       (5)

//Current OS - use compiler flags
#if defined(WIN32)
#define DEFSG_OS         (DEFSG_WINDOWS)
#define DEFSG_PROCESSOR  (DEFSG_HOST)
#elif defined(ANDROID_HOST)
#define DEFSG_OS         (DEFSG_LINUX_BIONIC)
#define DEFSG_PROCESSOR  (DEFSG_HOST)
#elif defined(LINUX_HOST)
#define DEFSG_OS         (DEFSG_LINUX_GLIBC)
#define DEFSG_PROCESSOR  (DEFSG_HOST)
#elif defined(LINUX_TARGET)
#define DEFSG_OS         (DEFSG_LINUX_GLIBC)
#define DEFSG_PROCESSOR  (DEFSG_GP)
#elif defined(CEVA)
#define DEFSG_OS         (DEFSG_CEVA_OS)
#define DEFSG_PROCESSOR  (DEFSG_CEVA)
#elif defined(EV72)
#define DEFSG_OS         (DEFSG_EV72_OS)
#define DEFSG_PROCESSOR  (DEFSG_EV72)
#elif defined(LINUX_GP_HOST)
#define DEFSG_OS         (DEFSG_LINUX_GLIBC)
#define DEFSG_PROCESSOR  (DEFSG_HOST)
#define DEFSG_GP_HOST    (1)
#else
#error "Unknown OS"
#endif

#define DEFSG_IS_HOST   (DEFSG_PROCESSOR == DEFSG_HOST)
#define DEFSG_IS_GP     (DEFSG_PROCESSOR == DEFSG_GP)
#define DEFSG_IS_CEVA   (DEFSG_PROCESSOR == DEFSG_CEVA)
#define DEFSG_IS_EV72   (DEFSG_PROCESSOR == DEFSG_EV72)
#define DEFSG_IS_DSP    ((DEFSG_PROCESSOR == DEFSG_CEVA) || (DEFSG_PROCESSOR == DEFSG_EV72))
#define DEFSG_IS_TARGET ((DEFSG_PROCESSOR == DEFSG_CEVA) || (DEFSG_PROCESSOR == DEFSG_GP) || (DEFSG_PROCESSOR == DEFSG_EV72))
#define DEFSG_IS_NOT_CEVA   (DEFSG_PROCESSOR != DEFSG_CEVA)
#define DEFSG_IS_NOT_HOST   (DEFSG_PROCESSOR != DEFSG_HOST)
#define DEFSG_IS_NOT_DSP    (!DEFSG_IS_DSP)


#define DEFSG_IS_OS_LINUX   ((DEFSG_OS == DEFSG_LINUX_GLIBC || DEFSG_OS == DEFSG_LINUX_BIONIC))
#define DEFSG_IS_OS_LINUX_GLIBC   (DEFSG_OS == DEFSG_LINUX_GLIBC)
#define DEFSG_IS_OS_LINUX_BIONIC   (DEFSG_OS == DEFSG_LINUX_BIONIC)
#define DEFSG_IS_OS_WINDOWS (DEFSG_OS == DEFSG_WINDOWS)
#define DEFSG_IS_OS_CEVA   (DEFSG_OS == DEFSG_CEVA_OS)
#define DEFSG_IS_OS_EV72   (DEFSG_OS == DEFSG_EV72_OS)


#define DEFSG_IS_GP_LINUX   (DEFSG_PROCESSOR == DEFSG_GP && DEFSG_OS == DEFSG_LINUX_GLIBC)
#define DEFSG_IS_HOST_LINUX (DEFSG_OS == DEFSG_LINUX_GLIBC && DEFSG_PROCESSOR == DEFSG_HOST)
#define DEFSG_IS_HOST_LINUX_DESKTOP (DEFSG_OS == DEFSG_LINUX_GLIBC  && DEFSG_PROCESSOR == DEFSG_HOST)
#define DEFSG_IS_HOST_ANDROID (DEFSG_OS == DEFSG_LINUX_BIONIC && DEFSG_PROCESSOR == DEFSG_HOST)
#define DEFSG_IS_HOST_WINDOWS (DEFSG_OS == DEFSG_WINDOWS && DEFSG_PROCESSOR == DEFSG_HOST)

#include <stdint.h>

typedef uint8_t UINT8;
typedef uint8_t BYTE;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;
typedef int8_t INT8;
typedef int16_t INT16;
typedef int32_t INT32;
typedef int64_t INT64;

#include <stdio.h>
#include <stdlib.h>

#if DEFSG_IS_OS_WINDOWS
   typedef uint8_t BOOLEAN;
   #pragma warning( disable : 4996 ) //disable _sprintf warning
   #define __func__ __FUNCTION__
#else
   #include <stdbool.h>
   typedef bool BOOLEAN;
   #if !DEFSG_IS_CEVA
   typedef bool BOOL;
   #endif
   #ifndef TRUE
      #define TRUE     true
   #endif
   #ifndef FALSE
      #define FALSE    false
   #endif
#endif

#ifndef ARR_NELEM
#define ARR_NELEM(arr) (sizeof(arr)/sizeof(arr)[0])
#endif

#ifdef __cplusplus
}
#endif

#endif
