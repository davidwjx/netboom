/* umdevxsproxy_interrupt.h
 *
 * This user-mode library handles the communication with the
 * Linux User Mode Device Access (UMDevXS) driver.
 * Using this part of the API it is possible to wait for an interrupt event.
 */

/*****************************************************************************
* Copyright (c) 2009-2018 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef INCLUDE_GUARD_UMDEVXSPROXY_INTERRUPT_H
#define INCLUDE_GUARD_UMDEVXSPROXY_INTERRUPT_H


/*----------------------------------------------------------------------------
 * UMDevXSProxy_Interrupt_WaitWithTimeout
 *
 * This function must be used to wait for an interrupt even to occur. As soon
 * as the interrupt has been reported (by the OS), the function call returns.
 * The timeout value requests a maximum wait duration, in milliseconds.
 *
 * Timeout_ms (input)
 *     Maximum time to wait for the interrupt. If the interrupt does not occur
 *     when this amount of time has elapsed, the function returns and reports
 *     the timeout instead of interrupt occurance.
 *
 * Return Value
 *     0  Return due to interrupt
 *     1  Return due to timeout
 *    <0  Error code
 */
int
UMDevXSProxy_Interrupt_WaitWithTimeout(
    const unsigned int Timeout_ms);


#endif /* INCLUDE_GUARD_UMDEVXSPROXY_INTERRUPT_H */

/* umdevxsproxy_interrupt.h */
