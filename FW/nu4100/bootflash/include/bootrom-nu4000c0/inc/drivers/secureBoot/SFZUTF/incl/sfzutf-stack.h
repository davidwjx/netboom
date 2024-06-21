/* sfzutf-stack.h
 *
 * Definitions for SFZUTF stack measurement facility.
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

#ifndef INCLUDE_GUARD_SFZUTF_STACK_H
#define INCLUDE_GUARD_SFZUTF_STACK_H

#ifndef INCLUDE_GUARD_SFZUTF_H
#error Never include sfzutf-stack.h directly, instead include sfzutf.h.
#endif /* INCLUDE_GUARD_SFZUTF_H */

#ifdef STACK_MEASUREMENT
#include <alloca.h>

extern const void *initial_stack_pointer;
extern const void *current_stack_pointer;
extern size_t measured_stack_usage;

/* Using alloca() here takes some stack but is fairly portable. */
#define STACK_MEASUREMENT_START \
    current_stack_pointer = initial_stack_pointer = alloca(1)

#define STACK_MEASUREMENT_STOP \
    initial_stack_pointer = current_stack_pointer = NULL

#define STACK_MEASUREMENT_ON (initial_stack_pointer != NULL)

#define STACK_MEASUREMENT_REPORT \
    L_TESTLOG(LF_STACKUSAGE, "%u", (unsigned int)measured_stack_usage)

#define STACK_MEASUREMENT_RECORD                                       \
    measured_stack_usage = ((char *)initial_stack_pointer -            \
                            (char *)current_stack_pointer)


#define STACK_MEASUREMENT_END \
    STACK_MEASUREMENT_RECORD; \
    if (STACK_MEASUREMENT_ON) STACK_MEASUREMENT_REPORT; \
    STACK_MEASUREMENT_STOP

#endif

/* Define new START_TEST and END_TEST
   that include starting measurement and ending measurement. */
#undef START_TEST
#define START_TEST(name)                                \
  static void name(int _i)                              \
  {                                                     \
    const char *funcname = #name;                       \
    L_TESTLOG(LF_TESTFUNC_INVOKED, "%s:%d", #name, _i); \
    STACK_MEASUREMENT_START;                            \
    do

#undef END_TEST
#define END_TEST while(0);                                                \
    if (sfzutf_unsupported_quick_process()) {                             \
         STACK_MEASUREMENT_STOP;                                          \
         L_TESTLOG(LF_TESTFUNC_UNSUPPORTED_QUICK, "%s:%d", funcname, _i); \
    } else {                                                              \
         STACK_MEASUREMENT_END;                                           \
         L_TESTLOG(LF_TESTFUNC_SUCCESS, "%s:%d", funcname, _i);           \
    }                                                                     \
  }

#endif /* Include Guard */

/* end of file sfzutf-stack.h */
