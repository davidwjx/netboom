/* sfzutf-heap.h
 *
 * Description: SFZUTF heap measurement routines.
 */

/*****************************************************************************
* Copyright (c) 2008-2018 INSIDE Secure B.V. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
* For more information or support, please go to our online support system at
* https://customersupport.insidesecure.com.
* In case you do not have an account for this system, please send an e-mail
* to ESSEmbeddedHW-Support@insidesecure.com.
*****************************************************************************/

#ifndef INCLUDE_GUARD_SFZUTF_HEAP_H
#define INCLUDE_GUARD_SFZUTF_HEAP_H

#ifndef INCLUDE_GUARD_SFZUTF_H
#error Never include sfzutf-heap.h directly, instead include sfzutf.h.
#endif /* INCLUDE_GUARD_SFZUTF_H */

#ifdef INCLUDE_GUARD_SFZUTF_STACK_H
#error Heap and Stack measurement cannot be done together.
#endif /* INCLUDE_GUARD_SFZUTF_STACK_H */

#ifdef HEAP_MEASUREMENT


extern bool heap_measurement_started;
extern size_t current_heap_usage;
extern size_t measured_heap_usage;


#define HEAP_MEASUREMENT_START \
   current_heap_usage = measured_heap_usage = 0; \
   heap_measurement_started = true;

#define HEAP_MEASUREMENT_STOP \
   heap_measurement_started = false;

#define HEAP_MEASUREMENT_ON (heap_measurement_started == true )

#define HEAP_MEASUREMENT_REPORT L_TESTLOG(LF_HEAPUSAGE, "%u",         \
                                          (unsigned int)measured_heap_usage)

#define HEAP_MEASUREMENT_RECORD                              \
    if(measured_heap_usage < current_heap_usage)             \
    measured_heap_usage = current_heap_usage;


#define HEAP_MEASUREMENT_END \
    if (HEAP_MEASUREMENT_ON) HEAP_MEASUREMENT_REPORT; \
    HEAP_MEASUREMENT_STOP

#endif

/* Define new START_TEST and END_TEST
   that include starting measurement and ending measurement. */
#undef START_TEST
#define START_TEST(name)                                \
  static void name(int _i)                              \
  {                                                     \
    const char *funcname = #name;                       \
    L_TESTLOG(LF_TESTFUNC_INVOKED, "%s:%d", #name, _i); \
    HEAP_MEASUREMENT_START;                             \
    do

#undef END_TEST
#define END_TEST while(0);                                                \
    if (sfzutf_unsupported_quick_process()) {                             \
         HEAP_MEASUREMENT_STOP;                                           \
         L_TESTLOG(LF_TESTFUNC_UNSUPPORTED_QUICK, "%s:%d", funcname, _i); \
    } else {                                                              \
         HEAP_MEASUREMENT_END;                                            \
         L_TESTLOG(LF_TESTFUNC_SUCCESS, "%s:%d", funcname, _i);           \
    }                                                                     \
  }

#endif /* Include Guard */

/* end of file sfzutf-heap.h */
