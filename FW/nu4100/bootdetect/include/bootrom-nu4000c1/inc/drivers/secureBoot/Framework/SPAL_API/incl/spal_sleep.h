/* spal_sleep.h
 *
 * Description: Sleep APIs
 */

/*****************************************************************************
* Copyright (c) 2010-2018 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef INCLUDE_GUARD_SPAL_SLEEP_H
#define INCLUDE_GUARD_SPAL_SLEEP_H

/*----------------------------------------------------------------------------
 * SPAL_SleepMS
 *
 * This function blocks the caller for the specified number of milliseconds.
 * The typical implementation will sleep the execution context, allowing other
 * execution contexts to be scheduled. This function must be called from a
 * schedulable execution context.
 *
 * Milliseconds
 *     During in milliseconds to sleep before returning.
 */
void
SPAL_SleepMS(
    unsigned int Milliseconds);


#endif /* Include guard */

/* end of file spal_sleep.h */
