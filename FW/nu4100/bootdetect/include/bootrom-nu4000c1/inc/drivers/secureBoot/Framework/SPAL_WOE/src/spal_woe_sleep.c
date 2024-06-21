/* spal_woe_sleep.c
 *
 * Description: Win32 specific implementation of SPAL Sleep API
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

#include "spal_thread.h"
#include "implementation_defs.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


/*----------------------------------------------------------------------------
 * SPAL_SleepMS
 */
void
SPAL_SleepMS(unsigned int Milliseconds)
{
    Sleep(Milliseconds);
}


/* end of file spal_woe_sleep.c */
